#ifndef NOTEEDITINGTAB_H
#define NOTEEDITINGTAB_H

#include "Note.h"
#include <QWidget>

namespace Ui
{
class NoteEditingTab;
}

class NoteEditingTab : public QWidget
{
    Q_OBJECT
  public:
    explicit NoteEditingTab(QWidget *parent = nullptr);
    ~NoteEditingTab();
  public slots:
    void startEditingNewNote(Note *note);
  signals:
    void exitEditingNote(Note &note);
    void saveNote(Note &note);
    void deleteNote(Note &note);

  private:
    Ui::NoteEditingTab *ui;
    Note *currentEditingNote;
    void onSaveNoteButtonPressed();
    QString lastSavedTitle;
    QString lastSavedContent;
    bool hasNoteChanged();

  private slots:
    void onDeleteNoteButtonPressed();
    void onReturnWithoutSavingButtonPressed();
};

#endif // NOTEEDITINGTAB_H
