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
    int getHowManyNotesCanFitInRow(int width) const;
    int getHowManyNotesAreDisplayed() const;
    void setMinWidthToFitNotesInRow(int numberOfNotesToFitInOneRow);
    bool event(QEvent *event);

  signals:
    void noteSelected(const QModelIndex &index);

  private:
    NoteListDelegate noteListDelegate;
    NoteButton *editor;
    QPersistentModelIndex currentIndexWithEditor;
    void updateEditor();
    void removeNote(const QModelIndex &index);
    void onNewEditorCreated(NoteButton *editor, const QModelIndex &index);
    NoteListModel *getSourceModelAtTheBottom() const;
    QModelIndex mapIndexToSourceModelAtTheBott(const QModelIndex &index) const;
    bool isTrashFolderLoaded();

  private slots:
    void onCustomContextMenuRequested(const QPoint &pos);
    void onRestoreNoteFromTrashRequested(const QModelIndex &index);
};

#endif // NOTELISTVIEW_H
