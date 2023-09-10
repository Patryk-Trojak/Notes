#include "FolderTreeItem.h"

FolderTreeItem::FolderTreeItem() : parent(nullptr)
{
}

FolderTreeItem::FolderTreeItem(FolderTreeItem *parent, const FolderData &data) : parent(parent), data(data)
{
}

const std::vector<std::unique_ptr<FolderTreeItem>> &FolderTreeItem::getChildren() const
{
    return children;
}

FolderTreeItem *FolderTreeItem::getChild(int row) const
{
    if (row < 0 or row >= children.size())
        return nullptr;

    return children.at(row).get();
}

FolderTreeItem *FolderTreeItem::getParent() const
{
    return parent;
}

int FolderTreeItem::row() const
{
    if (!parent)
        return 0;

    const auto &children = parent->getChildren();
    auto found = std::find_if(children.cbegin(), children.cend(),
                              [this](const std::unique_ptr<FolderTreeItem> &folder) { return folder.get() == this; });
    return found - children.begin();
}

void FolderTreeItem::setParent(FolderTreeItem *newParent)
{
    parent = newParent;
}

FolderTreeItem *FolderTreeItem::addChild(const FolderData &folderData)
{
    children.emplace_back(std::make_unique<FolderTreeItem>(this, folderData));
    return children.back().get();
}

FolderTreeItem *FolderTreeItem::insertChild(int pos, const FolderData &folderData)
{
    auto child = children.insert(children.begin() + pos, std::make_unique<FolderTreeItem>(this, folderData));
    return child->get();
}

void FolderTreeItem::moveChildrenFrom(FolderTreeItem *sourceParent, int sourceStartIndex, int destinationIndex,
                                      int count)
{
    if (sourceParent == this and sourceStartIndex == destinationIndex)
        return;

    for (int i = 0; i < 1; i++)
    {
        auto item = std::move(sourceParent->children[sourceStartIndex + i]);
        item->setParent(this);
        item->data.setParentId(this->data.getId());
        children.insert(children.begin() + destinationIndex + i, std::move(item));
    }

    if (sourceParent == this)
    {
        if (sourceStartIndex > destinationIndex)
            sourceStartIndex += count;
    }

    sourceParent->removeChildren(sourceStartIndex, count);
}

void FolderTreeItem::removeChildren(int first, int count)
{
    children.erase(children.begin() + first, children.begin() + first + count);
}

bool FolderTreeItem::isSubfolderOf(const FolderTreeItem *folder)
{
    FolderTreeItem *parent = this->parent;

    while (parent)
    {
        if (parent == folder)
            return true;

        parent = parent->parent;
    }

    return false;
}
