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
    if (parentItem == rootItem.get() and row <= 1) // Prevent adding row before special folders
        row = 2;

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

void FolderTreeModel::setupModelData()
{
    QVector<FolderData> folders = persistenceManager.loadAllFolders();
    std::sort(folders.begin(), folders.end(), [](const FolderData &folder1, const FolderData &folder2) {
        return folder1.getParentId() < folder2.getParentId();
    });

    FolderData rootFolder(SpecialFolderId::RootFolder, SpecialFolderId::InvalidId, "/");
    rootItem = std::make_unique<FolderTreeItem>(nullptr, rootFolder, FolderTreeItem::Type::RootFolder);

    setupChildrenRecursively(*rootItem, folders);

    FolderData allNotesFolder(SpecialFolderId::AllNotesFolder, rootItem->data.getId(), "All notes");
    rootItem->insertChild(0, allNotesFolder, FolderTreeItem::Type::AllNotesItem);

    FolderData trashFolder(SpecialFolderId::TrashFolder, rootItem->data.getId(), "Trash");
    rootItem->insertChild(1, trashFolder, FolderTreeItem::Type::TrashFolder);
}

void FolderTreeModel::setupChildrenRecursively(FolderTreeItem &folderTreeItem, const QVector<FolderData> &listOfFolders)
{
    auto first = std::lower_bound(listOfFolders.begin(), listOfFolders.end(), folderTreeItem.data.getId(),
                                  [](const FolderData &folder, int id) { return folder.getParentId() < id; });

    while (first->getParentId() == folderTreeItem.data.getId())
    {
        FolderTreeItem &addedItem = *folderTreeItem.addChild(*first);
        setupChildrenRecursively(addedItem, listOfFolders);
        first++;
    }
}

void FolderTreeModel::deleteFolderRecursivelyFromDb(const FolderTreeItem &folderTreeItem)
{
    persistenceManager.deleteFolder(folderTreeItem.data.getId());
    emit folderDeletedFromDatabase(folderTreeItem.data.getId());

    for (auto const &i : folderTreeItem.getChildren())
        deleteFolderRecursivelyFromDb(*i);
}
