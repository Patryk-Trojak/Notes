#include "NoteEditingTab.h"
#include <NoteListModelRole.h>
#include <QMessageBox>
#include <QModelIndex>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QShortcut>

NoteEditingTab::NoteEditingTab(NoteListModel &noteModel, const QModelIndex &editingNote, QWidget *parent)
    : QWidget(parent), editingNote(editingNote), noteModel(noteModel), editor(new NoteEditor(editingNote, this))
{
    QObject::connect(editor, &NoteEditor::closeNoteRequested, this, &NoteEditingTab::saveNoteAndEmitExitSignal);
    QObject::connect(new QShortcut(QKeySequence(Qt::Key_Escape), this), &QShortcut::activated, this,
                     &NoteEditingTab::saveNoteAndEmitExitSignal);
    QObject::connect(editor, &NoteEditor::titleChanged, this, &NoteEditingTab::onTitleChanged);
    QObject::connect(editor, &NoteEditor::contentChanged, this, &NoteEditingTab::onContentChanged);

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
