#include "Note.h"

Note::Note(int id) : id(id)
{
}

int Note::getId() const
{
    return id;
}

const QString &Note::getContent() const
{
    return content;
}

const QString &Note::getTitle() const
{
    return title;
}

const QDateTime &Note::getCreationTime() const
{
    return creationTime;
}

const QDateTime &Note::getModificationTime() const
{
    return modificationTime;
}

void Note::setId(int id)
{
    this->id = id;
}

void Note::setTitle(const QString &title)
{
    this->title = title;
}

void Note::setContent(const QString &content)
{
    this->content = content;
}

void Note::setCreationTime(const QDateTime &newCreationTime)
{
    creationTime = newCreationTime;
}

void Note::setModificationTime(const QDateTime &newModificationTime)
{
    modificationTime = newModificationTime;
}
