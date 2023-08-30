#ifndef FOLDERTREEMODEL_H
#define FOLDERTREEMODEL_H

#include "FolderTreeItem.h"
#include "PersistenceManager.h"
#include <QAbstractItemModel>

class FolderTreeModel : public QAbstractItemModel
{
    Q_OBJECT

  public:
    FolderTreeModel(PersistenceManager &persistenceManager, QObject *parent);
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex findIndex(int folderId, const QModelIndex &root = QModelIndex());
    FolderTreeItem *getItemFromIndex(const QModelIndex &index) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool insertRows(int row, int count, const QModelIndex &parent);
    bool removeRows(int row, int count, const QModelIndex &parent);
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
    QStringList mimeTypes() const;
    bool areAllDragingNotesInFolderIndex(const QModelIndex &index, const QMimeData *data);

  public slots:
    void onNotesMoved(int sourceFolderId, int destinationFolderId, int notesCount);
    void onNotesAdded(int parentFolderId, int notesCount);
    void onNotesRemoved(int parentFolderId, int notesCount);

  signals:
    void folderDeletedFromDatabase(int deletedFolderId);
    void moveNotesToFolderRequested(const QSet<int> &noteIds, int folderId);

  private:
    std::unique_ptr<FolderTreeItem> rootItem;
    PersistenceManager &persistenceManager;
    void setupModelData();
    QVector<FolderData> setupFolderList();
    void setupChildrenRecursively(FolderTreeItem &folderTreeItem, const QVector<FolderData> &listOfFolders);
    void deleteFolderRecursivelyFromDb(const FolderTreeItem &folderTreeItem);
    const QMimeData *lastCheckedMimeData;
    QModelIndex lastParentFolderIndexOfAllDragingNotes;
    void updateNotesInsideCountOfFolders(int updatedFolderId, int notesCount);
};

#endif // FOLDERTREEMODEL_H
