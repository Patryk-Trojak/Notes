#ifndef NOTESDISPLAYINGTAB_H
#define NOTESDISPLAYINGTAB_H

#include "FolderTreeModel.h"
#include "NoteListModel.h"
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
    void filterSortButtonsByTitle(const QString &searched);

  private:
    Ui::NotesDisplayingTab *ui;
    NoteListModel &noteModel;
    FolderTreeModel folderModel;
    std::function<void(NotesDisplayingTab *, bool)> currentNoteButtonsSortingMethod;
    void sortNoteButtons(std::function<bool(const QWidget *a, const QWidget *b)> compare);
    void sortNoteButtonsByTitle(bool ascendingOrder);
    void sortNoteButtonsByCreationDate(bool ascendingOrder);
    void sortNoteButtonsByModificationDate(bool ascendingOrder);
};

#endif // NOTESDISPLAYINGTAB_H
