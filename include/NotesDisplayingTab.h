#ifndef NOTESDISPLAYINGTAB_H
#define NOTESDISPLAYINGTAB_H

#include "FolderTreeModel.h"
#include "NoteButton.h"
#include "NoteData.h"
#include "NotesManager.h"
#include "Utils.h"
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHash>
#include <QMessageBox>
#include <QPushButton>
#include <QRandomGenerator>
#include <QScrollArea>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>
#include <algorithm>
#include <functional>
#include <memory>
#include <vector>

namespace Ui
{
class NotesDisplayingTab;
}

class NotesDisplayingTab : public QWidget
{
    Q_OBJECT
  public:
    explicit NotesDisplayingTab(NotesManager &notesManager, PersistenceManager &persistenceManager,
                                QWidget *parent = nullptr);
    ~NotesDisplayingTab();

  public:
    void updateNoteButton(const NoteData &note);
    void deleteNoteButton(const NoteData &note);
  signals:
    void enterEditingNote(NoteData &note);
  private slots:
    void onNewNoteButtonPressed();
    void onNoteButtonClicked();
    void onNoteButtonChangedTitle();
    void onNoteButtonDeleted();
    void onSortByTitleButtonToggled();
    void onSortByCreationDateButtonToggled();
    void onSortByModificationDateButtonToggled();
    void onSortOrderButtonToggled();
    void filterSortButtonsByTitle(const QString &searched);

  private:
    Ui::NotesDisplayingTab *ui;
    PersistenceManager &persistenceManager;
    FolderTreeModel folderModel;
    NotesManager &notesManager;
    QHash<const NoteButton *, NoteData *> buttonToNoteMap;
    QHash<const NoteData *, NoteButton *> noteToButtonMap;
    std::function<void(NotesDisplayingTab *, bool)> currentNoteButtonsSortingMethod;
    void createNewNoteButton(NoteData &note);
    void createNewNoteButtonsFromNotes();
    void sortNoteButtons(std::function<bool(const QWidget *a, const QWidget *b)> compare);
    void sortNoteButtonsByTitle(bool ascendingOrder);
    void sortNoteButtonsByCreationDate(bool ascendingOrder);
    void sortNoteButtonsByModificationDate(bool ascendingOrder);
};

#endif // NOTESDISPLAYINGTAB_H
