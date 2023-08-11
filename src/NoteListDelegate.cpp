#include "NoteListDelegate.h"

#include <NoteListModel.h>
#include <QPainter>

NoteListDelegate::NoteListDelegate(QObject *parent)
    : QStyledItemDelegate(parent), noteButton("", QDateTime(), false, nullptr)
{
}

void NoteListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
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
    return QSize(200, 200);
}

QWidget *NoteListDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const
{
    const NoteListModel *model = static_cast<const NoteListModel *>(index.model());
    NoteButton *button = new NoteButton(parent);
    button->setFocusPolicy(Qt::NoFocus);
    setupNoteButtonFromIndex(*button, index);
    button->setGeometry(option.rect);
    emit newEditorCreated(button, index);
    return button;
}

void NoteListDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    NoteButton *button = static_cast<NoteButton *>(editor);
    //    setupNoteButtonFromIndex(*button, index);
}

void NoteListDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    NoteButton *noteButton = static_cast<NoteButton *>(editor);
    model->setData(index, noteButton->getIsPinned(), NoteListModel::isPinned);
    model->setData(index, QVariant::fromValue(noteButton->getColor()), NoteListModel::Color);
}

void NoteListDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}

void NoteListDelegate::setupNoteButtonFromIndex(NoteButton &noteButton, const QModelIndex &index) const
{
    noteButton.setTitle(index.data(NoteListModel::Title).toString());
    noteButton.setContent(index.data(NoteListModel::Content).toString());
    noteButton.setModificationTime(index.data(NoteListModel::ModificationTime).toDateTime());
    noteButton.setIsPinned(index.data(NoteListModel::isPinned).toBool());
    noteButton.setColor(index.data(NoteListModel::Color).value<QColor>());
}
