#include "Note.h"

Note::Note(const QString &filename) : filename(filename)
{
}

const QString &Note::getContent() const
{
    return content;
}

const QString &Note::getTitle() const
{
    return title;
}

const QString &Note::getFilename() const
{
    return filename;
}

const QDateTime &Note::getCreationTime() const
{
    return creationTime;
}

const QDateTime &Note::getModificationTime() const
{
    return modificationTime;
}

void Note::setTitle(const QString &title)
{
    this->title = title;
}

void Note::setContent(const QString &content)
{
    this->content = content;
}

void Note::setFilename(const QString &filename)
{
    this->filename = filename;
}

void Note::setCreationTime(const QDateTime &newCreationTime)
{
    creationTime = newCreationTime;
}

void Note::setModificationTime(const QDateTime &newModificationTime)
{
    modificationTime = newModificationTime;
}
