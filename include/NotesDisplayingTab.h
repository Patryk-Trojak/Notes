#ifndef NOTESDISPLAYINGTAB_H
#define NOTESDISPLAYINGTAB_H

#include "FolderTreeModel.h"
#include "NoteListModel.h"
#include "NoteListView.h"
#include "NoteSortFilterProxyModel.h"
#include "SearchBar.h"
#include <QSortFilterProxyModel>
#include <QWidget>

namespace Ui
{
class NotesDisplayingTab;
}

class NotesDisplayingTab : public QWidget
{
    Q_OBJECT
  public:
    explicit NotesDisplayingTab(NoteListModel &noteModel, PersistenceManager &persistenceManager,
                                QWidget *parent = nullptr);
    ~NotesDisplayingTab();

  signals:
    void enterEditingNote(const QModelIndex &note);
  private slots:
    void onNewNoteButtonPressed();
    void onSortByTitleButtonToggled();
    void onSortByCreationDateButtonToggled();
    void onSortByModificationDateButtonToggled();
    void onSortOrderButtonToggled();
    void onNewFolderSelected(int selectedFolderId);

  private:
    Ui::NotesDisplayingTab *ui;
    NoteListView *noteListView;
    SearchBar *searchBar;
    NoteListModel &noteModel;
    NoteSortFilterProxyModel noteProxyModel;
    FolderTreeModel folderModel;

    void layoutNoteListView();
    void layoutSearchBar();
    void layoutAllElementsWhichDependsOnNumberOfNotes();

  protected:
    void resizeEvent(QResizeEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);
};

#endif // NOTESDISPLAYINGTAB_H
