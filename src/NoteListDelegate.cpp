#include "NoteListDelegate.h"

#include <NoteListModel.h>
#include <QPainter>

NoteListDelegate::NoteListDelegate(QObject *parent)
    : QStyledItemDelegate(parent), noteButton("", QDateTime(), QDateTime(), false, nullptr)
{
}

void NoteListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);
    setupNoteButtonFromIndex(noteButton, index);
    noteButton.setPinCheckboxVisible(noteButton.getIsPinned());
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
    NoteButton *button = new NoteButton(parent);
    setupNoteButtonFromIndex(*button, index);
    button->setGeometry(option.rect);
    emit newEditorCreated(button, index);
    return button;
}

void NoteListDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    NoteButton *button = static_cast<NoteButton *>(editor);
    setupNoteButtonFromIndex(*button, index);
}

void NoteListDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    NoteButton *noteButton = static_cast<NoteButton *>(editor);
    if (noteButton->getTitle() != model->data(index, NoteListModel::Title))
    {
        model->setData(index, noteButton->getTitle(), NoteListModel::Title);
        model->setData(index, QDateTime::currentDateTime(), NoteListModel::ModificationTime);
    }
    model->setData(index, noteButton->getIsPinned(), NoteListModel::isPinned);
}

void NoteListDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}

void NoteListDelegate::setupNoteButtonFromIndex(NoteButton &noteButton, const QModelIndex &index) const
{
    const auto *model = index.model();
    noteButton.setTitle(model->data(index, NoteListModel::Title).toString());
    noteButton.setCreationTime(model->data(index, NoteListModel::CreationTime).toDateTime());
    noteButton.setModificationTime(model->data(index, NoteListModel::ModificationTime).toDateTime());
    noteButton.setIsPinned(model->data(index, NoteListModel::isPinned).toBool());
}
