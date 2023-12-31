#ifndef NOTELISTDELEGATE_H
#define NOTELISTDELEGATE_H

#include <NoteButton.h>
#include <QStyledItemDelegate>

class NoteListDelegate : public QStyledItemDelegate
{
    Q_OBJECT

  public:
    NoteListDelegate(QObject *parent);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const override;

  signals:
    void newEditorCreated(NoteButton *editor, const QModelIndex &index) const;

  private:
    mutable NoteButton noteButton; // Used to drawing item
    void setupNoteButtonFromIndex(NoteButton &noteButton, const QModelIndex &index) const;
};

#endif // NOTELISTDELEGATE_H
