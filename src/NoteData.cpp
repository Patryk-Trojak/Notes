#include "NoteData.h"

NoteData::NoteData(int id) : id(id)
{
}

int NoteData::getId() const
{
    return id;
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

void NoteData::setId(int id)
{
    this->id = id;
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
