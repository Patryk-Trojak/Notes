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
