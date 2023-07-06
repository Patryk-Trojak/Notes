#ifndef NOTEEDITINGTAB_H
#define NOTEEDITINGTAB_H

#include "NoteData.h"
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
    void startEditingNewNote(NoteData *note);
  signals:
    void exitEditingNote(NoteData &note);
    void saveNote(NoteData &note);
    void deleteNote(NoteData &note);

  private:
    Ui::NoteEditingTab *ui;
    NoteData *currentEditingNote;
    void saveNoteIfChanged();
    QString lastSavedTitle;
    QString lastSavedContent;
    bool hasNoteChanged();

  private slots:
    void onDeleteNoteButtonPressed();
    void onReturnWithoutSavingButtonPressed();
};

#endif // NOTEEDITINGTAB_H
