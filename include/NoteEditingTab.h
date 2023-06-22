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
    void exitEditingNote();

  private:
    Ui::NoteEditingTab *ui;
    Note *currentEditingNote;
    void saveNote();
};

#endif // NOTEEDITINGTAB_H
