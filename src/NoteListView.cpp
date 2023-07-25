#include "NoteListView.h"

#include <QMenu>
#include <QMouseEvent>
#include <QScrollBar>
#include <qmessagebox.h>
NoteListView::NoteListView(QWidget *parent) : QListView(parent), editor(nullptr), noteListDelegate(this)
{
    setSelectionMode(QAbstractItemView::NoSelection);
    setMouseTracking(true);
    setItemDelegate(&noteListDelegate);
    setAutoScroll(false);

    QObject::connect(&noteListDelegate, &NoteListDelegate::newEditorCreated, this, &NoteListView::onNewEditorCreated);
    QObject::connect(this, &QListView::entered, this, &NoteListView::onItemEntered);
    QObject::connect(this, &QListView::customContextMenuRequested, this, &NoteListView::onCustomContextMenuRequested);
}

void NoteListView::setModel(QAbstractItemModel *model)
{
    QObject::connect(model, &QAbstractItemModel::dataChanged, this, &NoteListView::updateEditorOnDataChanged);
    QListView::setModel(model);
}

void NoteListView::removeNote()
{
    auto reply = QMessageBox::question(this, "Delete note?", "Are you sure you want to delete this note?");
    if (reply == QMessageBox::No)
        return;

    model()->removeRow(currentIndex().row());
}

void NoteListView::onNewEditorCreated(NoteButton *editor, const QModelIndex &index)
{
    this->editor = editor;
    QObject::connect(editor, &NoteButton::deleteNote, this, &NoteListView::removeNote);
    QObject::connect(editor, &NoteButton::saveNote, this,
                     [this, editor, index]() { noteListDelegate.setModelData(editor, this->model(), index); });
    QObject::connect(editor, &NoteButton::clicked, this, [this, index]() { emit this->noteSelected(index); });
}

void NoteListView::onItemEntered(const QModelIndex &index)
{
    // We create editor when mouse is over item.
    if (index.isValid())
    {
        if (currentIndex().isValid())
        {
            closePersistentEditor(currentIndex());
            editor = nullptr;
        }

        if (index.isValid())
            openPersistentEditor(index);

        setCurrentIndex(index);
    }
}

void NoteListView::updateEditorOnDataChanged()
{
    if (editor)
        noteListDelegate.setEditorData(editor, currentIndex());
}

void NoteListView::onCustomContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = indexAt(pos);
    QMenu *menu = new QMenu(this);
    QAction *createNote = new QAction("Create new note");
    QObject::connect(createNote, &QAction::triggered, this, [this, index]() { this->model()->insertRow(0, index); });
    menu->addAction(createNote);

    if (index.isValid())
    {
        QAction *deleteNote = new QAction("Delete note");
        QObject::connect(deleteNote, &QAction::triggered, this,
                         [this, index]() { this->model()->removeRow(index.row(), index.parent()); });
        menu->addAction(deleteNote);
    }

    menu->exec(mapToGlobal(pos));
}

void NoteListView::leaveEvent(QEvent *event)
{
    QListView::leaveEvent(event);
    closePersistentEditor(currentIndex());
    editor = nullptr;
}
