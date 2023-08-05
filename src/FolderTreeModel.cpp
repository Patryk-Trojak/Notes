#include "FolderTreeModel.h"

FolderTreeModel::FolderTreeModel(PersistenceManager &persistenceManager, QObject *parent)
    : persistenceManager(persistenceManager), QAbstractItemModel(parent)
{
    setupModelData();
}

QModelIndex FolderTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    const FolderTreeItem *parentItem = getItemFromIndex(parent);
    return createIndex(row, column, parentItem->getChild(row));
}

QModelIndex FolderTreeModel::findIndex(int folderId, const QModelIndex &parent)
{
    const FolderTreeItem *root = getItemFromIndex(parent);
    auto &children = root->getChildren();
    for (auto child = children.begin(); child < children.end(); child++)
    {
        if ((*child)->data.getId() == folderId)
            return index(child - children.begin(), 0, parent);

        findIndex(folderId, index(child - children.begin(), 0, parent));
    }
    return QModelIndex();
}

FolderTreeItem *FolderTreeModel::getItemFromIndex(const QModelIndex &index) const
{
    if (!index.isValid())
        return rootItem.get();

    return static_cast<FolderTreeItem *>(index.internalPointer());
}

QModelIndex FolderTreeModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();

    const FolderTreeItem *item = static_cast<const FolderTreeItem *>(child.constInternalPointer());
    if (item == rootItem.get())
        return QModelIndex();

    if (item->getParent() == nullptr || item->getParent() == rootItem.get())
        return QModelIndex();

    return createIndex(item->getParent()->row(), 0, item->getParent());
}

int FolderTreeModel::rowCount(const QModelIndex &parent) const
{
    const FolderTreeItem *item = getItemFromIndex(parent);
    if (item)
        return item->getChildren().size();

    return 0;
}

int FolderTreeModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant FolderTreeModel::data(const QModelIndex &index, int role) const
{
    const FolderTreeItem *item = getItemFromIndex(index);

    switch (role)
    {
    case Qt::EditRole:
    case Qt::DisplayRole:
        return item->data.getName();
    default:
        return QVariant();
    }
}

bool FolderTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    FolderTreeItem *item = getItemFromIndex(index);

    switch (role)
    {
    case Qt::EditRole:
        item->data.setName(value.toString());
        persistenceManager.updateFolder(item->data);
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

Qt::ItemFlags FolderTreeModel::flags(const QModelIndex &index) const
{
    FolderTreeItem *item = getItemFromIndex(index);
    if (item->getType() == FolderTreeItem::Type::UserFolder)
    {
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    }
    return QAbstractItemModel::flags(index);
}

bool FolderTreeModel::insertRows(int row, int count, const QModelIndex &parent)
{
    FolderTreeItem *parentItem = getItemFromIndex(parent);
    if (parentItem == rootItem.get() and row <= 0) // Prevent adding row before special folders
        row = 1;

    for (int i = 0; i < count; i++)
    {
        FolderData newFolderData;
        newFolderData.setName("New folder");
        newFolderData.setParentId(parentItem->data.getId());
        int id = persistenceManager.addFolder(newFolderData);
        newFolderData.setId(id);
        beginInsertRows(parent, row + i, row + i);
        parentItem->insertChild(row + i, newFolderData);
        endInsertRows();
    }
    return true;
}

bool FolderTreeModel::removeRows(int row, int count, const QModelIndex &parent)
{
    FolderTreeItem *parentItem = getItemFromIndex(parent);
    QModelIndex removingRow = index(row, count, parent);
    if (!(removingRow.flags() & Qt::ItemIsEditable)) // Prevent removing special folders
        return false;

    for (int i = 0; i < count; i++)
    {
        deleteFolderRecursivelyFromDb(*parentItem->getChild(row + i));
    }

    beginRemoveRows(parent, row, row + count - 1);
    parentItem->removeChildren(row, count);
    endRemoveRows();
    return true;
}

void FolderTreeModel::updateNotesInsideCountOfFolder(int folderId, int deltaNoteInsideCount)
{
    QModelIndex folderIndex = findIndex(folderId);
    FolderTreeItem *folder = static_cast<FolderTreeItem *>(folderIndex.internalPointer());
    folder->data.setNotesInsideCount(folder->data.getNotesInsideCount() + deltaNoteInsideCount);

    persistenceManager.updateFolder(folder->data);
    emit dataChanged(folderIndex, folderIndex);
}

void FolderTreeModel::setupModelData()
{
    QVector<FolderData> folders = persistenceManager.loadAllFolders();
    std::sort(folders.begin(), folders.end(), [](const FolderData &folder1, const FolderData &folder2) {
        return folder1.getParentId() < folder2.getParentId();
    });

    FolderData rootFolder(SpecialFolderId::RootFolder, SpecialFolderId::InvalidId, "/", 0);
    rootItem = std::make_unique<FolderTreeItem>(nullptr, rootFolder, FolderTreeItem::Type::RootFolder);

    setupChildrenRecursively(*rootItem, folders);

    int allNoteCount = persistenceManager.countAllNotes();
    FolderData allNotesFolder(SpecialFolderId::AllNotesFolder, rootItem->data.getId(), "All notes", allNoteCount);
    rootItem->insertChild(0, allNotesFolder, FolderTreeItem::Type::AllNotesItem);

    int notesInTrashCount = persistenceManager.countNotesInTrash();
    FolderData trashFolder(SpecialFolderId::TrashFolder, rootItem->data.getId(), "Trash", notesInTrashCount);
    rootItem->insertChild(rootItem->getChildren().size(), trashFolder, FolderTreeItem::Type::TrashFolder);
}

void FolderTreeModel::setupChildrenRecursively(FolderTreeItem &folderTreeItem, const QVector<FolderData> &listOfFolders)
{
    auto first = std::lower_bound(listOfFolders.begin(), listOfFolders.end(), folderTreeItem.data.getId(),
                                  [](const FolderData &folder, int id) { return folder.getParentId() < id; });

    if (first == listOfFolders.end())
        return;

    while (first->getParentId() == folderTreeItem.data.getId())
    {
        FolderTreeItem &addedItem = *folderTreeItem.addChild(*first);
        setupChildrenRecursively(addedItem, listOfFolders);
        first++;
    }
}

void FolderTreeModel::deleteFolderRecursivelyFromDb(const FolderTreeItem &folderTreeItem)
{
    updateNotesInsideCountOfFolder(SpecialFolderId::TrashFolder, folderTreeItem.data.getNotesInsideCount());
    persistenceManager.deleteFolder(folderTreeItem.data.getId());
    emit folderDeletedFromDatabase(folderTreeItem.data.getId());

    for (auto const &i : folderTreeItem.getChildren())
        deleteFolderRecursivelyFromDb(*i);
}
