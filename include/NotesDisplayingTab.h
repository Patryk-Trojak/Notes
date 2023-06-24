#ifndef NOTESDISPLAYINGTAB_H
#define NOTESDISPLAYINGTAB_H

#include "Note.h"
#include "NoteButton.h"
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
    explicit NotesDisplayingTab(QWidget *parent = nullptr);
    ~NotesDisplayingTab();

  public:
    void updateNoteButton(const Note &note);
  signals:
    void enterEditingNote(Note &note);
  private slots:
    void onNewNoteButtonPressed();
    void onNoteButtonClicked();
    void onNoteButtonChangedTitle();
    void onNoteButtonDeleted();

  private:
    Ui::NotesDisplayingTab *ui;
    std::vector<std::unique_ptr<Note>> notes;
    QHash<const NoteButton *, Note *> buttonToNoteMap;
    QHash<const Note *, NoteButton *> noteToButtonMap;
    void createNewNoteButton(Note &note);
    void createNewNoteButtonsFromNotes();
    void deleteNoteFromVector(Note *note);
    void sortNoteButtons(std::function<bool(const QWidget *a, const QWidget *b)> compare);
};

#endif // NOTESDISPLAYINGTAB_H
