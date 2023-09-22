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
    QUrl url = QUuid::createUuid().toString();
    document()->addResource(QTextDocument::ImageResource, url, imageData);
    textCursor().insertImage(url.toString());
}
