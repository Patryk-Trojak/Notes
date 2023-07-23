#ifndef NOTELISTVIEW_H
#define NOTELISTVIEW_H

#include <NoteButton.h>
#include <NoteListDelegate.h>
#include <QListView>

class NoteListView : public QListView
{
    Q_OBJECT

  public:
    NoteListView(QWidget *parent);
    void setModel(QAbstractItemModel *model);

  signals:
    void noteSelected(const QModelIndex &index);

  private:
    NoteListDelegate noteListDelegate;
    NoteButton *editor;
    void removeNote();
    void onNewEditorCreated(NoteButton *editor, const QModelIndex &index);
    void onItemEntered(const QModelIndex &index);
    void updateEditorOnDataChanged();
};

#endif // NOTELISTVIEW_H
