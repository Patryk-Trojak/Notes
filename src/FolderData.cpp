#include "FolderData.h"

FolderData::FolderData() : id(0), parentId(0), notesInsideCount(0)
{
}

FolderData::FolderData(int id, int parentId, const QString &name, int notesInsideCount)
    : id(id), parentId(parentId), name(name), notesInsideCount(notesInsideCount)
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

int FolderData::getNotesInsideCount() const
{
    return notesInsideCount;
}

void FolderData::setNotesInsideCount(int newNotesInsideCount)
{
    notesInsideCount = newNotesInsideCount;
}

void FolderData::setName(const QString &newName)
{
    name = newName;
}
