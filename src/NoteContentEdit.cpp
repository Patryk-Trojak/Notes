#include "NoteContentEdit.h"

#include <QFileInfo>
#include <QImageReader>
#include <QMimeData>
#include <QUuid>

NoteContentEdit::NoteContentEdit(QWidget *parent) : QTextEdit(parent)
{
}

bool NoteContentEdit::canInsertFromMimeData(const QMimeData *source) const
{
    return source->hasImage() or QTextEdit::canInsertFromMimeData(source);
}

void NoteContentEdit::insertFromMimeData(const QMimeData *source)
{
    if (source->hasImage())
    {
        insertImage(source->imageData());
        return;
    }

    QTextEdit::insertFromMimeData(source);
}

void NoteContentEdit::insertImage(const QVariant &imageData)
{
    int idOfResource = resourceSaver(imageData, QTextDocument::ImageResource);
    QUrl url = QUrl(QString::number(idOfResource));
    document()->addResource(QTextDocument::ImageResource, url, imageData);
    textCursor().insertImage(url.toString());
}

QVariant NoteContentEdit::loadResource(int type, const QUrl &name)
{
    QVariant resource = resourceLoader(name, static_cast<QTextDocument::ResourceType>(type));
    document()->addResource(QTextDocument::ImageResource, name, resource);
    return resource;
}

void NoteContentEdit::setResourceSaver(const ResourceSaver &resourceSaver)
{
    this->resourceSaver = resourceSaver;
}

void NoteContentEdit::setResourceLoader(const ResourceLoader &resourceLoader)
{
    this->resourceLoader = resourceLoader;
}
