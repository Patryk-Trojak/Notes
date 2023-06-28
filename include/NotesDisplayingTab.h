#ifndef NOTESDISPLAYINGTAB_H
#define NOTESDISPLAYINGTAB_H

#include "Note.h"
#include "NoteButton.h"
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
    explicit NotesDisplayingTab(NotesManager &notesManager, QWidget *parent = nullptr);
    ~NotesDisplayingTab();

  public:
    void updateNoteButton(const Note &note);
    void deleteNoteButton(const Note &note);
  signals:
    void enterEditingNote(Note &note);
  private slots:
    void onNewNoteButtonPressed();
    void onNoteButtonClicked();
    void onNoteButtonChangedTitle();
    void onNoteButtonDeleted();
    void onSortByTitleButtonToggled();
    void onSortByCreationDateButtonToggled();
    void onSortByModificationDateButtonToggled();
    void onSortOrderButtonToggled();

  private:
    Ui::NotesDisplayingTab *ui;
    NotesManager &notesManager;
    QHash<const NoteButton *, Note *> buttonToNoteMap;
    QHash<const Note *, NoteButton *> noteToButtonMap;
    std::function<void(NotesDisplayingTab *, bool)> currentNoteButtonsSortingMethod;
    void createNewNoteButton(Note &note);
    void createNewNoteButtonsFromNotes();
    void sortNoteButtons(std::function<bool(const QWidget *a, const QWidget *b)> compare);
    void sortNoteButtonsByTitle(bool ascendingOrder);
    void sortNoteButtonsByCreationDate(bool ascendingOrder);
    void sortNoteButtonsByModificationDate(bool ascendingOrder);
};

#endif // NOTESDISPLAYINGTAB_H
