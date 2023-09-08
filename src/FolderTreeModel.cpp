#include "FolderTreeModel.h"
#include <NoteMimeData.h>
#include <QApplication>
#include <QMessageBox>
#include <QMimeData>

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

QModelIndex FolderTreeModel::findIndex(int folderId, const QModelIndex &root) const
{
    const FolderTreeItem *rootItem = getItemFromIndex(root);
    if (rootItem->data.getId() == folderId)
        return root;

    auto &children = rootItem->getChildren();
    for (auto child = children.begin(); child < children.end(); child++)
    {
        QModelIndex found = findIndex(folderId, index(child - children.begin(), 0, root));
        if (found.isValid())
            return found;
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
    case FolderTreeModelRole::EditRole:
    case FolderTreeModelRole::DisplayRole:
    case FolderTreeModelRole::Name:
        return item->data.getName();
    case FolderTreeModelRole::Id:
        return item->data.getId();
    case FolderTreeModelRole::ParentId:
        return item->data.getParentId();
    case FolderTreeModelRole::PreviousFolderId:
        return item->data.getPreviousFolderId();
    case FolderTreeModelRole::Color:
        return item->data.getColor();
    case FolderTreeModelRole::NotesInsideCount:
        return item->data.getNotesInsideCount();
    }

    return QVariant();
}

bool FolderTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    FolderTreeItem *item = getItemFromIndex(index);
    bool dataHasBeenChanged = false;
    switch (role)
    {
    case FolderTreeModelRole::EditRole:
    case FolderTreeModelRole::DisplayRole:
    case FolderTreeModelRole::Name:
        item->data.setName(value.toString());
        dataHasBeenChanged = true;
        break;
    case FolderTreeModelRole::Id:
        item->data.setId(value.toInt());
        dataHasBeenChanged = true;
        break;
    case FolderTreeModelRole::ParentId:
        item->data.setParentId(value.toInt());
        dataHasBeenChanged = true;
        break;
    case FolderTreeModelRole::PreviousFolderId:
        item->data.setPreviousFolderId(value.toInt());
        dataHasBeenChanged = true;
        break;
    case FolderTreeModelRole::Color:
        item->data.setColor(value.value<QColor>());
        dataHasBeenChanged = true;
        break;
    case FolderTreeModelRole::NotesInsideCount:
        item->data.setNotesInsideCount(value.toInt());
        dataHasBeenChanged = true;
        break;
    }

    if (dataHasBeenChanged)
    {
        persistenceManager.updateFolder(item->data);
        emit dataChanged(index, index);
        return true;
    }

    return false;
}

Qt::ItemFlags FolderTreeModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index) | Qt::ItemIsDropEnabled;
    if (getItemFromIndex(index)->getType() == FolderTreeItem::Type::UserFolder)
        flags |= Qt::ItemIsEditable | Qt::ItemIsDragEnabled;

    return flags;
}

bool FolderTreeModel::insertRows(int row, int count, const QModelIndex &parent)
{
    FolderTreeItem *parentItem = getItemFromIndex(parent);
    if (parentItem == rootItem.get() and row <= 0) // Prevent adding row before special folders
        row = 1;

    beginInsertRows(parent, row + count - 1, row + count - 1);

    for (int i = 0; i < count; i++)
    {
        FolderData newFolderData;
        newFolderData.setName("New folder");
        newFolderData.setParentId(parentItem->data.getId());
        if (row + i == 0 or parentItem->getChild(row + i - 1)->getType() != FolderTreeItem::Type::UserFolder)
            newFolderData.setPreviousFolderId(SpecialFolderId::InvalidId);
        else
            newFolderData.setPreviousFolderId(parentItem->getChild(row + i - 1)->data.getId());

        int id = persistenceManager.addFolder(newFolderData);
        newFolderData.setId(id);

        parentItem->insertChild(row + i, newFolderData);
    }

    endInsertRows();
    updatePreviousFolderId(parent, row + count);
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
    updatePreviousFolderId(parent, row);
    return true;
}

bool FolderTreeModel::areAllDragingNotesInFolderIndex(const QModelIndex &index, const QMimeData *data) const
{
    if (data == lastCheckedMimeData)
        return lastParentFolderIndexOfAllDragingNotes == index;

    lastCheckedMimeData = data;
    QVector<NoteMimeData> decodedData = NoteMimeData::decodeData(data);
    if (decodedData.empty())
        return false;

    int parentFolderId = decodedData[0].parentFolderId;

    for (auto const i : decodedData)
    {
        if (parentFolderId != i.parentFolderId)
        {
            lastParentFolderIndexOfAllDragingNotes = QModelIndex();
            return false;
        }
    }

    lastParentFolderIndexOfAllDragingNotes = findIndex(parentFolderId);
    return lastParentFolderIndexOfAllDragingNotes == index;
}

void FolderTreeModel::createNewFolder()
{
    insertRow(1, QModelIndex());
}

void FolderTreeModel::onNotesMoved(int sourceFolderId, int destinationFolderId, int notesCount)
{
    FolderTreeItem *sourceFolder = static_cast<FolderTreeItem *>(findIndex(sourceFolderId).internalPointer());
    FolderTreeItem *destinationFolder = static_cast<FolderTreeItem *>(findIndex(destinationFolderId).internalPointer());

    if (sourceFolderId != SpecialFolderId::AllNotesFolder)
    {
        sourceFolder->data.setNotesInsideCount(sourceFolder->data.getNotesInsideCount() - notesCount);
        if (destinationFolderId == SpecialFolderId::TrashFolder)
        {
            FolderTreeItem *allNotesFolder =
                static_cast<FolderTreeItem *>(findIndex(SpecialFolderId::AllNotesFolder).internalPointer());
            allNotesFolder->data.setNotesInsideCount(allNotesFolder->data.getNotesInsideCount() - notesCount);
        }
    }
    else if (destinationFolderId == SpecialFolderId::TrashFolder)
        sourceFolder->data.setNotesInsideCount(sourceFolder->data.getNotesInsideCount() - notesCount);

    if (destinationFolderId != SpecialFolderId::AllNotesFolder)
    {
        destinationFolder->data.setNotesInsideCount(destinationFolder->data.getNotesInsideCount() + notesCount);
    }
}

void FolderTreeModel::onNotesAdded(int parentFolderId, int notesCount)
{
    updateNotesInsideCountOfFolders(parentFolderId, notesCount);
}

void FolderTreeModel::onNotesRemoved(int parentFolderId, int notesCount)
{
    updateNotesInsideCountOfFolders(parentFolderId, -notesCount);
}

void FolderTreeModel::updateNotesInsideCountOfFolders(int updatedFolderId, int notesCount)
{
    FolderTreeItem *folder = static_cast<FolderTreeItem *>(findIndex(updatedFolderId).internalPointer());
    folder->data.setNotesInsideCount(folder->data.getNotesInsideCount() + notesCount);
    if (updatedFolderId != SpecialFolderId::AllNotesFolder and updatedFolderId != SpecialFolderId::TrashFolder)
    {
        FolderTreeItem *allNotesFolder =
            static_cast<FolderTreeItem *>(findIndex(SpecialFolderId::AllNotesFolder).internalPointer());
        allNotesFolder->data.setNotesInsideCount(allNotesFolder->data.getNotesInsideCount() + notesCount);
    }
}

bool FolderTreeModel::canFolderBeMoved(const QModelIndex &sourceFolder, const QModelIndex &desinationParent,
                                       int row) const
{
    if (row < 0)
        row = 0;

    if (!desinationParent.isValid())
    {
        if (row < 1)
            return false;

        if (row > rowCount(QModelIndex()) - 1)
            return false;
    }
    else if (getItemFromIndex(desinationParent)->getType() == FolderTreeItem::Type::AllNotesFolder)
        return false;

    if (sourceFolder == desinationParent)
        return false;

    if (sourceFolder.parent() == desinationParent)
    {
        if (sourceFolder.row() == row)
            return false;
        if (sourceFolder.row() == row - 1)
            return false;
    }

    if (getItemFromIndex(desinationParent)->isSubfolderOf(getItemFromIndex(sourceFolder)))
        return false;

    return true;
}

void FolderTreeModel::updatePreviousFolderId(const QModelIndex &parentItem, int row)
{
    QModelIndex folderIndex = index(row, 0, parentItem);
    FolderTreeItem *item = getItemFromIndex(folderIndex);

    if (!item or item->getType() != FolderTreeItem::Type::UserFolder)
        return;

    FolderTreeItem *previousSiblingOfItem = item->getParent()->getChild(row - 1);

    if (previousSiblingOfItem and previousSiblingOfItem->getType() == FolderTreeItem::Type::UserFolder)
        item->data.setPreviousFolderId(previousSiblingOfItem->data.getId());
    else
        item->data.setPreviousFolderId(SpecialFolderId::InvalidId);

    persistenceManager.updateFolder(item->data);
    emit dataChanged(folderIndex, folderIndex);
}

QMimeData *FolderTreeModel::mimeData(const QModelIndexList &indexes) const
{
    QVector<const FolderTreeItem *> items;
    for (auto const &index : indexes)
        items.emplaceBack(static_cast<const FolderTreeItem *>(index.constInternalPointer()));

    QMimeData *mimeData = FolderMimeData::encodeData(items);
    return mimeData;
}

bool FolderTreeModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                                      const QModelIndex &parent) const
{
    if (data->hasFormat(NoteMimeData::type))
    {
        if (!parent.isValid())
            return false;

        return true;
    }
    else if (data->hasFormat(FolderMimeData::type))
    {
        QModelIndex dragingFolder;
        if (lastCheckedMimeData == data)
            dragingFolder = lastDragingFolder;
        else
        {
            lastCheckedMimeData = data;
            QVector<FolderMimeData> folderMimeData = FolderMimeData::decodeData(data);
            if (folderMimeData.empty())
                return false;

            lastDragingFolder = findIndex(folderMimeData.at(0).folderId);
        }
        return canFolderBeMoved(dragingFolder, parent, row);
    }

    return false;
}

void FolderTreeModel::setupModelData()
{
    QVector<FolderData> folders = setupFolderList();

    FolderData rootFolder(SpecialFolderId::RootFolder, SpecialFolderId::InvalidId, SpecialFolderId::InvalidId, "/",
                          QColor(255, 255, 255), 0);
    rootItem = std::make_unique<FolderTreeItem>(nullptr, rootFolder, FolderTreeItem::Type::RootFolder);
    setupChildrenRecursively(*rootItem, folders);

    int allNoteCount = persistenceManager.countAllNotes();
    FolderData allNotesFolder(SpecialFolderId::AllNotesFolder, rootItem->data.getId(), SpecialFolderId::InvalidId,
                              "All notes", QColor(255, 255, 255), allNoteCount);
    rootItem->insertChild(0, allNotesFolder, FolderTreeItem::Type::AllNotesFolder);

    int notesInTrashCount = persistenceManager.countNotesInTrash();
    FolderData trashFolder(SpecialFolderId::TrashFolder, rootItem->data.getId(),
                           rootItem->getChild(rootItem->getChildren().size() - 1)->data.getId(), "Trash",
                           QColor(255, 255, 255), notesInTrashCount);
    rootItem->insertChild(rootItem->getChildren().size(), trashFolder, FolderTreeItem::Type::TrashFolder);
}

QVector<FolderData> FolderTreeModel::setupFolderList()
{
    QVector<FolderData> folders = persistenceManager.loadAllFolders();
    updateNotesInsideCountsOfFolders(folders);
    sortFolders(folders);
    return folders;
}

void FolderTreeModel::updateNotesInsideCountsOfFolders(QVector<FolderData> &folders)
{
    QHash<int, int> notesInsideFoldersCounts = persistenceManager.getNotesInsideFoldersCounts();
    for (auto &folder : folders)
    {
        auto found = notesInsideFoldersCounts.find(folder.getId());
        if (found != notesInsideFoldersCounts.end())
            folder.setNotesInsideCount(*found);
        else
            folder.setNotesInsideCount(0);
    }
}

void FolderTreeModel::sortFolders(QVector<FolderData> &folders)
{
    std::sort(folders.begin(), folders.end(), [](const FolderData &folder1, const FolderData &folder2) {
        if (folder1.getParentId() < folder2.getParentId())
            return true;
        if (folder1.getParentId() > folder2.getParentId())
            return false;

        if (folder1.getPreviousFolderId() == SpecialFolderId::InvalidId)
            return true;

        if (folder2.getPreviousFolderId() == SpecialFolderId::InvalidId)
            return false;

        if (folder1.getPreviousFolderId() < folder2.getPreviousFolderId())
            return true;
        if (folder1.getPreviousFolderId() > folder2.getPreviousFolderId())
            return false;

        return false;
    });
}

void FolderTreeModel::setupChildrenRecursively(FolderTreeItem &folderTreeItem, const QVector<FolderData> &listOfFolders)
{
    auto firstChild = std::lower_bound(listOfFolders.begin(), listOfFolders.end(), folderTreeItem.data.getId(),
                                       [](const FolderData &folder, int id) { return folder.getParentId() < id; });

    if (firstChild == listOfFolders.end() or firstChild->getParentId() != folderTreeItem.data.getId())
        return;

    FolderTreeItem &addedItem = *folderTreeItem.addChild(*firstChild);
    setupChildrenRecursively(addedItem, listOfFolders);

    auto lastChild = std::upper_bound(firstChild, listOfFolders.end(), folderTreeItem.data.getId(),
                                      [](int id, const FolderData &folder) { return folder.getParentId() > id; });
    int numberOfChildren = lastChild - firstChild;
    for (int i = 0; i < numberOfChildren - 1; i++)
    {
        auto nextInOrder =
            std::lower_bound(firstChild, lastChild, folderTreeItem.getChild(i)->data.getId(),
                             [](const FolderData &folder, int id) { return folder.getPreviousFolderId() < id; });

        if (nextInOrder != lastChild and nextInOrder->getPreviousFolderId() == folderTreeItem.getChild(i)->data.getId())
        {
            FolderTreeItem &addedItem = *folderTreeItem.addChild(*nextInOrder);
            setupChildrenRecursively(addedItem, listOfFolders);
        }
    }
}

void FolderTreeModel::deleteFolderRecursivelyFromDb(const FolderTreeItem &folderTreeItem)
{
    persistenceManager.deleteFolder(folderTreeItem.data.getId());
    emit folderDeletedFromDatabase(folderTreeItem.data.getId());

    for (auto const &i : folderTreeItem.getChildren())
        deleteFolderRecursivelyFromDb(*i);
}

void FolderTreeModel::handleFolderMimeData(const QMimeData *data, const QModelIndex &parent, int row)
{
    QVector<FolderMimeData> decodedData = FolderMimeData::decodeData(data);
    if (decodedData.empty())
        return;
    QModelIndex movedIndex = findIndex(decodedData[0].folderId);
    QModelIndex movedIndexParent = movedIndex.parent();
    FolderTreeItem *movedItem = getItemFromIndex(movedIndex);
    FolderTreeItem *movedItemParent = movedItem->getParent();
    FolderTreeItem *parentItem = getItemFromIndex(parent);

    if (parentItem->getType() == FolderTreeItem::Type::TrashFolder)
    {
        removeRow(movedIndex.row(), movedIndex.parent());
        return;
    }

    if (row <= 0)
    {
        if (parentItem == rootItem.get()) // Prevent adding row before special folders
            row = 1;
        else
            row = 0;
    }

    beginMoveRows(movedIndex.parent(), movedIndex.row(), movedIndex.row(), parent, row);
    parentItem->moveChildrenFrom(movedItemParent, movedIndex.row(), row);
    endMoveRows();

    if (movedItemParent == parentItem and movedIndex.row() < row)
        row -= 1;
    updatePreviousFolderId(parent, row);
    updatePreviousFolderId(parent, row + 1);
    if (movedItemParent == parentItem and (movedIndex.row() > row))
        updatePreviousFolderId(movedIndex.parent(), movedIndex.row() + 1);
    else
        updatePreviousFolderId(movedIndexParent, movedIndex.row());
}

void FolderTreeModel::handleNoteMimeData(const QMimeData *data, const QModelIndex &parent)
{
    QVector<NoteMimeData> decodedData = NoteMimeData::decodeData(data);
    QSet<int> noteIds;
    for (auto const &noteData : decodedData)
        noteIds.insert(noteData.noteId);

    emit moveNotesToFolderRequested(noteIds, getItemFromIndex(parent)->data.getId());
}

bool FolderTreeModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                                   const QModelIndex &parent)
{
    if (data->hasFormat(NoteMimeData::type))
        handleNoteMimeData(data, parent);
    else if (data->hasFormat(FolderMimeData::type))
        handleFolderMimeData(data, parent, row);
    else
        return false;

    return true;
}

QStringList FolderTreeModel::mimeTypes() const
{
    return QStringList{FolderMimeData::type, NoteMimeData::type};
}
