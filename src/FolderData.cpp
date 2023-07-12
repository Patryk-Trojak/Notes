#include "FolderData.h"

FolderData::FolderData()
{
}

int FolderData::getId() const
{
    return id;
}

void FolderData::setId(int newId)
{
    id = newId;
}

int FolderData::getParentId() const
{
    return parentId;
}

void FolderData::setParentId(int newParentId)
{
    parentId = newParentId;
}

const QString &FolderData::getName() const
{
    return name;
}

void FolderData::setName(const QString &newName)
{
    name = newName;
}
