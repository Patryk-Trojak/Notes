#include "NoteListView.h"
#include "NoteListModel.h"

#include <QAbstractProxyModel>
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

    setWrapping(true);
    setUniformItemSizes(true);
    setSpacing(5);
    setFlow(Flow::LeftToRight);
}

void NoteListView::removeNote(const QModelIndex &index)
{
    const QString message = [index]() {
        if (index.data(NoteListModel::isInTrash).toBool())
            return "Are you sure you want to delete this note? You won't be able to recover deleted note";
        else
            return "Are you sure you want to remove this note? You will be able to restore it from trash folder";
    }();

    auto reply = QMessageBox::question(this, "Delete note?", message);
    if (reply == QMessageBox::No)
        return;

    model()->removeRow(index.row());
}

void NoteListView::onNewEditorCreated(NoteButton *editor, const QModelIndex &index)
{
    this->editor = editor;
    editor->setMouseTracking(true);
    editor->installEventFilter(this);
    QObject::connect(editor, &NoteButton::deleteNote, this,
                     [this, index]() { this->removeNote(this->currentIndex()); });
    QObject::connect(editor, &NoteButton::clicked, this, [this, index]() { emit this->noteSelected(index); });
    QObject::connect(editor, &NoteButton::pinCheckboxToogled, this,
                     [this, editor, index]() { noteListDelegate.setModelData(editor, this->model(), index); });
    QObject::connect(editor, &NoteButton::colorChanged, this,
                     [this, editor, index]() { noteListDelegate.setModelData(editor, this->model(), index); });
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

NoteListModel *NoteListView::getSourceModelAtTheBottom() const
{
    // We know that at the bottom there is NoteListModel.
    // If we want to access it, we need to find it, because proxy model can be set to view
    QAbstractItemModel *currentModel = model();
    forever
    {
        if (NoteListModel *noteModel = qobject_cast<NoteListModel *>(currentModel))
            return noteModel;

        if (QAbstractProxyModel *proxyModel = qobject_cast<QAbstractProxyModel *>(currentModel))
            currentModel = proxyModel->sourceModel();
        else
            return nullptr;
    }
}

QModelIndex NoteListView::mapIndexToSourceModelAtTheBott(const QModelIndex &index) const
{
    // We know that at the bottom there is NoteListModel.
    // Because indexes which view uses can be from proxy model,
    // we need to map it if we want to access directly NoteListModel
    QAbstractItemModel *currentModel = model();
    QModelIndex currentIndex = index;
    while (QAbstractProxyModel *proxyModel = qobject_cast<QAbstractProxyModel *>(currentModel))
    {
        currentIndex = proxyModel->mapToSource(currentIndex);
        currentModel = proxyModel->sourceModel();
    }

    return currentIndex;
}

bool NoteListView::isTrashFolderLoaded()
{
    NoteListModel *noteModel = getSourceModelAtTheBottom();
    if (noteModel)
        return noteModel->getCurrentSelectedFolderId() == SpecialFolderId::TrashFolder;

    return false;
}

void NoteListView::onCustomContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = indexAt(pos);
    QMenu *menu = new QMenu(this);

    if (!isTrashFolderLoaded())
    {
        QAction *createNote = new QAction("Create new note");
        QObject::connect(createNote, &QAction::triggered, this,
                         [this, index]() { this->model()->insertRow(0, index); });
        menu->addAction(createNote);
        if (index.isValid())
        {
            if (!index.data(NoteListModel::isPinned).toBool())
            {
                QAction *pinNote = new QAction("Pin note");
                QObject::connect(pinNote, &QAction::triggered, this,
                                 [this, index]() { this->model()->setData(index, true, NoteListModel::isPinned); });
                menu->addAction(pinNote);
            }
            else
            {
                QAction *unpinNote = new QAction("Unpin note");
                QObject::connect(unpinNote, &QAction::triggered, this,
                                 [this, index]() { this->model()->setData(index, false, NoteListModel::isPinned); });
                menu->addAction(unpinNote);
            }
        }
    }
    else if (index.isValid())
    {
        QAction *restoreNote = new QAction("Restore note");
        QObject::connect(restoreNote, &QAction::triggered, this,
                         [this, index]() { this->onRestoreNoteFromTrashRequested(index); });
        menu->addAction(restoreNote);
    }

    if (index.isValid())
    {
        QAction *deleteNote = new QAction("Delete note");
        QObject::connect(deleteNote, &QAction::triggered, this, [this, index]() { removeNote(index); });
        menu->addAction(deleteNote);
    }

    menu->exec(mapToGlobal(pos));
}

void NoteListView::onRestoreNoteFromTrashRequested(const QModelIndex &index)
{
    NoteListModel *noteModel = getSourceModelAtTheBottom();
    QModelIndex noteModelIndex = mapIndexToSourceModelAtTheBott(index);
    if (noteModel)
        noteModel->restoreNoteFromTrash(noteModelIndex);
}

void NoteListView::resizeEvent(QResizeEvent *event)
{
    QListView::resizeEvent(event);
    setWrapping(true);
}

bool NoteListView::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == editor)
    {
        if (event->type() == QEvent::Leave)
        {
            // Destroy editor when mouse leaves it
            closePersistentEditor(currentIndex());
            editor = nullptr;
        }
    }
    return QListView::eventFilter(watched, event);
}
