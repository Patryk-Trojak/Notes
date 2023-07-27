#include "NoteData.h"

NoteData::NoteData() : id(-1), parentFolderId(-1), isInTrash(false), isPinned(false)
{
}

int NoteData::getId() const
{
    return id;
}

int NoteData::getParentFolderId() const
{
    return parentFolderId;
}

const QString &NoteData::getContent() const
{
    return content;
}

const QString &NoteData::getTitle() const
{
    return title;
}

const QDateTime &NoteData::getCreationTime() const
{
    return creationTime;
}

const QDateTime &NoteData::getModificationTime() const
{
    return modificationTime;
}

bool NoteData::getIsInTrash() const
{
    return isInTrash;
}

bool NoteData::getIsPinned() const
{
    return isPinned;
}

void NoteData::setId(int id)
{
    this->id = id;
}

void NoteData::setParentFolderId(int newParentFolderId)
{
    parentFolderId = newParentFolderId;
}

void NoteData::setTitle(const QString &title)
{
    this->title = title;
}

void NoteData::setContent(const QString &content)
{
    this->content = content;
}

void NoteData::setCreationTime(const QDateTime &newCreationTime)
{
    creationTime = newCreationTime;
}

void NoteData::setModificationTime(const QDateTime &newModificationTime)
{
    modificationTime = newModificationTime;
}

void NoteData::setIsInTrash(bool newIsInTrash)
{
    isInTrash = newIsInTrash;
}

void NoteData::setIsPinned(bool newIsPinned)
{
    isPinned = newIsPinned;
}
