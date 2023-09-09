#include "FolderData.h"

FolderData::FolderData() : id(0), parentId(0), notesInsideCount(0), previousFolderId(0), color(255, 255, 255)
{
}

FolderData::FolderData(int id, int parentId, int previousFolderId, const QString &name, const QColor &color,
                       int notesInsideCount)
    : id(id), parentId(parentId), previousFolderId(previousFolderId), name(name), color(color),
      notesInsideCount(notesInsideCount)
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

int FolderData::getPreviousFolderId() const
{
    return previousFolderId;
}

QColor FolderData::getColor() const
{
    return color;
}

void FolderData::setColor(const QColor &newColor)
{
    color = newColor;
}

void FolderData::setPreviousFolderId(int newPreviousFolderId)
{
    previousFolderId = newPreviousFolderId;
}

void FolderData::setNotesInsideCount(int newNotesInsideCount)
{
    notesInsideCount = newNotesInsideCount;
}

void FolderData::setName(const QString &newName)
{
    name = newName;
}
