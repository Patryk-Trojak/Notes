#ifndef NOTEEDITINGTAB_H
#define NOTEEDITINGTAB_H

#include "NoteData.h"
#include "NoteListModel.h"
#include <QWidget>

namespace Ui
{
class NoteEditingTab;
}

class NoteEditingTab : public QWidget
{
    Q_OBJECT
  public:
    explicit NoteEditingTab(NoteListModel &noteModel, QWidget *parent = nullptr);
    ~NoteEditingTab();
  public slots:
    void startEditingNewNote(const QModelIndex &index);
  signals:
    void exitEditingNote(const QModelIndex &index);

  private:
    Ui::NoteEditingTab *ui;
    NoteListModel &noteModel;
    const QModelIndex *currentEditingNote;
    void saveNoteIfChanged();
    QString lastSavedTitle;
    QString lastSavedContent;
    bool hasNoteChanged();

  private slots:
    void onDeleteNoteButtonPressed();
    void onReturnWithoutSavingButtonPressed();
};

#endif // NOTEEDITINGTAB_H
