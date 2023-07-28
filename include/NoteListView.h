#ifndef NOTELISTVIEW_H
#define NOTELISTVIEW_H

#include "NoteListModel.h"
#include <NoteButton.h>
#include <NoteListDelegate.h>
#include <QListView>

class NoteListView : public QListView
{
    Q_OBJECT

  public:
    NoteListView(QWidget *parent);
    void setModel(QAbstractItemModel *model);
    bool eventFilter(QObject *watched, QEvent *event);

  signals:
    void noteSelected(const QModelIndex &index);

  private:
    NoteListDelegate noteListDelegate;
    NoteButton *editor;
    void removeNote(const QModelIndex &index);
    void onNewEditorCreated(NoteButton *editor, const QModelIndex &index);
    void onItemEntered(const QModelIndex &index);
    void updateEditorOnDataChanged();
    NoteListModel *getSourceModelAtTheBottom() const;
    QModelIndex mapIndexToSourceModelAtTheBott(const QModelIndex &index) const;
    bool isTrashFolderLoaded();
  private slots:
    void onCustomContextMenuRequested(const QPoint &pos);
    void onRestoreNoteFromTrashRequested(const QModelIndex &index);
};

#endif // NOTELISTVIEW_H
