#ifndef FOLDERTREEITEM_H
#define FOLDERTREEITEM_H

#include "FolderData.h"
#include <QVector>
class FolderTreeItem
{
  public:
    FolderTreeItem();
    FolderTreeItem(FolderTreeItem *parent, const FolderData &data);
    const std::vector<std::unique_ptr<FolderTreeItem>> &getChildren() const;
    FolderTreeItem *getChild(int row) const;
    FolderTreeItem *getParent() const;
    int row() const;
    void setParent(FolderTreeItem *newParent);
    FolderTreeItem *addChild(const FolderData &folderData);
    FolderTreeItem *insertChild(int pos, const FolderData &folderData);
    void moveChildrenFrom(FolderTreeItem *sourceParent, int sourceStartIndex, int destinationIndex, int count = 1);
    void removeChildren(int first, int count);
    FolderData data;
    bool isSubfolderOf(const FolderTreeItem *folder);

  private:
    std::vector<std::unique_ptr<FolderTreeItem>> children;
    FolderTreeItem *parent;
};

#endif // FOLDERTREEITEM_H
