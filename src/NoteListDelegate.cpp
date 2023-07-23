#include "NoteListDelegate.h"

#include <NoteListModel.h>
#include <QPainter>

NoteListDelegate::NoteListDelegate(QObject *parent)
    : QStyledItemDelegate(parent), noteButton("", QDateTime(), QDateTime(), nullptr)
{
}

void NoteListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    const NoteListModel *model = static_cast<const NoteListModel *>(index.model());
    NoteData note = model->getNoteData(index);
    noteButton.setTitle(note.getTitle());
    noteButton.setCreationTime(note.getCreationTime());
    noteButton.setModificationTime(note.getModificationTime());
    painter->save();
    noteButton.resize(option.rect.size());
    painter->translate(option.rect.topLeft());
    noteButton.render(painter, QPoint(), QRegion(), QWidget::DrawChildren);
    painter->restore();
}

QSize NoteListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(300, 120);
}

QWidget *NoteListDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const
{
    const NoteListModel *model = static_cast<const NoteListModel *>(index.model());
    const NoteData &note = model->getNoteData(index);
    NoteButton *button = new NoteButton(note.getTitle(), note.getCreationTime(), note.getModificationTime(), parent);
    button->setGeometry(option.rect);
    emit newEditorCreated(button, index);
    return button;
}

void NoteListDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    NoteButton *button = static_cast<NoteButton *>(editor);
    const NoteListModel *model = static_cast<const NoteListModel *>(index.model());
    const NoteData &note = model->getNoteData(index);

    button->setTitle(note.getTitle());
    button->setCreationTime(note.getCreationTime());
    button->setModificationTime(note.getModificationTime());
}

void NoteListDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    NoteListModel *noteListModel = static_cast<NoteListModel *>(model);
    NoteButton *noteButton = static_cast<NoteButton *>(editor);
    if (noteButton->getTitle() != noteListModel->getNoteData(index).getTitle())
    {
        NoteData editedNote = noteListModel->getNoteData(index);
        editedNote.setTitle(noteButton->getTitle());
        editedNote.setModificationTime(QDateTime::currentDateTime());
        noteListModel->setNoteData(index, editedNote);
    }
}

void NoteListDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
