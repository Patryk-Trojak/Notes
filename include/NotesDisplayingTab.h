#ifndef NOTESDISPLAYINGTAB_H
#define NOTESDISPLAYINGTAB_H

#include "FolderTreeModel.h"
#include "NoteListModel.h"
#include "NoteSortFilterProxyModel.h"
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
    NoteListModel &noteModel;
    NoteSortFilterProxyModel noteProxyModel;
    FolderTreeModel folderModel;
};

#endif // NOTESDISPLAYINGTAB_H
