#ifndef FOLDERTREEITEM_H
#define FOLDERTREEITEM_H

#include "FolderData.h"
#include <QVector>
class FolderTreeItem
{
  public:
    enum class Type
    {
        UserFolder,
        AllNotesFolder,
        RootFolder,
        TrashFolder
    };
    FolderTreeItem();
    FolderTreeItem(FolderTreeItem *parent, const FolderData &data, Type type = Type::UserFolder);
    const std::vector<std::unique_ptr<FolderTreeItem>> &getChildren() const;
    FolderTreeItem *getChild(int row) const;
    FolderTreeItem *getParent() const;
    int row() const;
    void setParent(FolderTreeItem *newParent);
    FolderTreeItem *addChild(const FolderData &folderData);
    FolderTreeItem *insertChild(int pos, const FolderData &folderData, Type type = Type::UserFolder);
    void moveChildrenFrom(FolderTreeItem *sourceParent, int sourceStartIndex, int destinationIndex, int count = 1);
    void removeChildren(int first, int count);
    FolderData data;
    Type getType() const;
    void setType(Type newType);
    bool isSubfolderOf(const FolderTreeItem *folder);

  private:
    std::vector<std::unique_ptr<FolderTreeItem>> children;
    FolderTreeItem *parent;
    Type type;
};

#endif // FOLDERTREEITEM_H
