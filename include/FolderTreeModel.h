#ifndef FOLDERTREEMODEL_H
#define FOLDERTREEMODEL_H

#include "FolderTreeItem.h"
#include "PersistenceManager.h"
#include <QAbstractItemModel>

class FolderTreeModel : public QAbstractItemModel
{
  public:
    FolderTreeModel(PersistenceManager &persistenceManager);
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    FolderTreeItem *getItemFromIndex(const QModelIndex &index) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool insertRows(int row, int count, const QModelIndex &parent);
    bool removeRows(int row, int count, const QModelIndex &parent);

  private:
    std::unique_ptr<FolderTreeItem> rootItem;
    PersistenceManager &persistenceManager;
    void setupModelData();
    void setupChildrenRecursively(FolderTreeItem &folderTreeItem, const QVector<FolderData> &listOfFolders);
};

#endif // FOLDERTREEMODEL_H
