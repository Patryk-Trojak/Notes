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

FolderTreeItem *FolderTreeItem::addChild(const FolderData &noteData)
{
    children.emplace_back(std::make_unique<FolderTreeItem>(this, noteData));
    return children.back().get();
}

FolderTreeItem *FolderTreeItem::insertChild(int pos, const FolderData &noteData)
{
    auto child = children.insert(children.begin() + pos, std::make_unique<FolderTreeItem>(this, noteData));
    return child->get();
}

void FolderTreeItem::removeChildren(int first, int count)
{
    children.erase(children.begin() + first, children.begin() + first + count);
}
