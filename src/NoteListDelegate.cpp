#include "NoteListDelegate.h"

#include <NoteListModelRole.h>
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
    noteButton.setIsSelected(option.state & QStyle::State_Selected);
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
    setupNoteButtonFromIndex(*button, index);
}

void NoteListDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    NoteButton *noteButton = static_cast<NoteButton *>(editor);

    if (index.data(NoteListModelRole::isPinned).toBool() != noteButton->getIsPinned())
    {
        model->setData(index, noteButton->getIsPinned(), NoteListModelRole::isPinned);
    }
    else if (index.data(NoteListModelRole::Color).value<QColor>() != noteButton->getColor())
    {
        model->setData(index, QVariant::fromValue(noteButton->getColor()), NoteListModelRole::Color);
    }
}

void NoteListDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}

void NoteListDelegate::setupNoteButtonFromIndex(NoteButton &noteButton, const QModelIndex &index) const
{
    noteButton.setTitle(index.data(NoteListModelRole::Title).toString());
    noteButton.setContent(index.data(NoteListModelRole::Content).toString());
    noteButton.setModificationTime(index.data(NoteListModelRole::ModificationTime).toDateTime());
    noteButton.setIsPinned(index.data(NoteListModelRole::isPinned).toBool());
    noteButton.setColor(index.data(NoteListModelRole::Color).value<QColor>());
}
