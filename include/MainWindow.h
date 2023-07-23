#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "NoteEditingTab.h"
#include "NotesDisplayingTab.h"
#include <NotesManager.h>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
  public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
  public slots:
    void enterEditingNote(const QModelIndex &index);
    void exitEditingNote();

  private:
    Ui::MainWindow *ui;
    PersistenceManager persistenceManager;
    NoteListModel noteModel;
    NotesDisplayingTab notesDisplayingTab;
    NoteEditingTab noteEditTab;
};
#endif // MAINWINDOW_H
