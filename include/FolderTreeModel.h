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
    QModelIndex findIndex(int folderId, const QModelIndex &index = QModelIndex());
    FolderTreeItem *getItemFromIndex(const QModelIndex &index) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool insertRows(int row, int count, const QModelIndex &parent);
    bool removeRows(int row, int count, const QModelIndex &parent);

  public slots:
    void updateNotesInsideCountOfFolder(int folderId, int deltaNoteInsideCount);

  signals:
    void folderDeletedFromDatabase(int deletedFolderId);

  private:
    std::unique_ptr<FolderTreeItem> rootItem;
    PersistenceManager &persistenceManager;
    void setupModelData();
    void setupChildrenRecursively(FolderTreeItem &folderTreeItem, const QVector<FolderData> &listOfFolders);
    void deleteFolderRecursivelyFromDb(const FolderTreeItem &folderTreeItem);
};

#endif // FOLDERTREEMODEL_H
