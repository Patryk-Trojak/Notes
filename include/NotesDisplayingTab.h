#ifndef NOTESDISPLAYINGTAB_H
#define NOTESDISPLAYINGTAB_H

#include "Note.h"
#include "NoteButton.h"
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHash>
#include <QPushButton>
#include <QRandomGenerator>
#include <QScrollArea>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>
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
    explicit NotesDisplayingTab(QWidget *parent = nullptr);
    ~NotesDisplayingTab();
    void onNewNoteButtonPressed();
    void saveNote();
  signals:
    void enterEditingNote(Note &note);
  private slots:
    void onNoteButtonClicked();
    void onNoteButtonChangedTitle();
    void onNoteButtonDeleted();

  private:
    Ui::NotesDisplayingTab *ui;
    std::vector<std::unique_ptr<Note>> notes;
    QHash<NoteButton *, Note *> buttonToNoteMap;
    void createNewNoteButton(Note &note);
    void createNewNoteButtonsFromNotes();
    void deleteNoteFromVector(Note *note);
};

#endif // NOTESDISPLAYINGTAB_H
