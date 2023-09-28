#include "NoteEditingTab.h"
#include <NoteListModelRole.h>
#include <QMessageBox>
#include <QModelIndex>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QShortcut>

NoteEditingTab::NoteEditingTab(PersistenceManager &persistenceManager, NoteListModel &noteModel,
                               const QModelIndex &editingNote, QWidget *parent)
    : QWidget(parent), persistenceManager(persistenceManager), editingNote(editingNote), noteModel(noteModel),
      editor(new NoteEditor(editingNote, this))

{
    QObject::connect(editor, &NoteEditor::closeNoteRequested, this, &NoteEditingTab::saveNoteAndEmitExitSignal);
    QObject::connect(new QShortcut(QKeySequence(Qt::Key_Escape), this), &QShortcut::activated, this,
                     &NoteEditingTab::saveNoteAndEmitExitSignal);
    QObject::connect(editor, &NoteEditor::titleChanged, this, &NoteEditingTab::onTitleChanged);
    QObject::connect(editor, &NoteEditor::contentChanged, this, &NoteEditingTab::onContentChanged);
    QObject::connect(editor, &NoteEditor::deleteNoteRequested, this, &NoteEditingTab::maybeDeleteNote);

    setPalette(QPalette(QColor(0, 0, 0, 190)));
    setAutoFillBackground(true);

    if (parent)
        QWidget::setGeometry(0, 0, parent->width(), parent->height());

    editor->setGeometry(QRect(width() / 2, height() / 2, 0, 0));
    openingEditorAnimation = new QPropertyAnimation(editor, "geometry");
    openingEditorAnimation->setStartValue(QRect(parent->width() / 2, parent->height() / 2, 0, 0));
    openingEditorAnimation->setEndValue(calculateGeometryOfEditor());
    openingEditorAnimation->setDuration(150);
    openingEditorAnimation->start();

    auto resourceLoader = [&persistenceManager](const QUrl &url, QTextDocument::ResourceType resourceType) {
        if (resourceType != QTextDocument::ImageResource)
            QVariant();

        return QVariant::fromValue(persistenceManager.loadImage(url.toString().toInt()));
    };

    auto resourceSaver = [&persistenceManager, noteId = editingNote.data(NoteListModelRole::Id).toInt()](
                             const QVariant &resource, QTextDocument::ResourceType resourceType) {
        if (resourceType != QTextDocument::ImageResource)
            return -1;

        int imageId = persistenceManager.addImage(resource.value<QImage>(), noteId);
        return imageId;
    };

    editor->setResourceLoader(resourceLoader);
    editor->setResourceSaver(resourceSaver);
}

void NoteEditingTab::onTitleChanged(const QString &newTitle)
{
    QDateTime modificationTime = QDateTime::currentDateTime();
    noteModel.setData(editingNote, newTitle, NoteListModelRole::Title);
    noteModel.setData(editingNote, modificationTime, NoteListModelRole::ModificationTime);
}

void NoteEditingTab::onContentChanged(const QString &newContent)
{
    QDateTime modificationTime = QDateTime::currentDateTime();
    noteModel.setData(editingNote, newContent, NoteListModelRole::Content);
    noteModel.setData(editingNote, modificationTime, NoteListModelRole::ModificationTime);
}

bool NoteEditingTab::maybeDeleteNote()
{
    const QString message = [this]() {
        if (this->editingNote.data(NoteListModelRole::isInTrash).toBool())
            return "Are you sure you want to delete this note? You won't be able to recover deleted note";
        else
            return "Are you sure you want to remove this note? You will be able to restore it from trash folder";
    }();

    auto reply = QMessageBox::question(this, "Delete note?", message);
    if (reply == QMessageBox::No)
        return false;

    noteModel.removeRow(editingNote.row());
    emit exitEditingNoteRequested();
    return true;
}

void NoteEditingTab::saveNoteAndEmitExitSignal()
{
    noteModel.saveDirtyIndexes();
    emit exitEditingNoteRequested();
}

QRect NoteEditingTab::calculateGeometryOfEditor()
{
    int editorWidth = width() * 2 / 3;
    int editorHeight = height() * 2 / 3;
    int editorLeft = (width() - editorWidth) / 2;
    int editorTop = (height() - editorHeight) / 2;

    return QRect(editorLeft, editorTop, editorWidth, editorHeight);
}

void NoteEditingTab::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (openingEditorAnimation->state() == QAbstractAnimation::Running)
        return;

    editor->setGeometry(calculateGeometryOfEditor());
}

void NoteEditingTab::mouseReleaseEvent(QMouseEvent *event)
{
    if (!editor->geometry().contains(event->pos()))
        saveNoteAndEmitExitSignal();
}
