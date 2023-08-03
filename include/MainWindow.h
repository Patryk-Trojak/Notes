#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "NoteEditingTab.h"
#include "NotesDisplayingTab.h"
#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT
  public:
    MainWindow(QWidget *parent = nullptr);

  private:
    PersistenceManager persistenceManager;
    NoteListModel noteModel;
    NotesDisplayingTab *notesDisplayingTab;
    NoteEditingTab *noteEditingTab;

  public slots:
    void enterEditingNote(const QModelIndex &index);
    void exitEditingNote();

  protected:
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
};
#endif // MAINWINDOW_H
