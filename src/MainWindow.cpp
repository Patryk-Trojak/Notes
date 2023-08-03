#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), noteModel(this, persistenceManager), noteEditingTab(nullptr)
{
    notesDisplayingTab = new NotesDisplayingTab(noteModel, persistenceManager, this);
    QObject::connect(notesDisplayingTab, &NotesDisplayingTab::enterEditingNote, this, &MainWindow::enterEditingNote);
    setMinimumSize(350, 350);
    setGeometry(200, 200, 975, 550);
}

void MainWindow::enterEditingNote(const QModelIndex &note)
{
    if (noteEditingTab)
        delete noteEditingTab;

    noteEditingTab = new NoteEditingTab(noteModel, note, this);
    QObject::connect(noteEditingTab, &NoteEditingTab::exitEditingNoteRequested, this, &MainWindow::exitEditingNote);
    noteEditingTab->show();
}

void MainWindow::exitEditingNote()
{
    if (noteEditingTab)
    {
        delete noteEditingTab;
        noteEditingTab = nullptr;
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    if (noteEditingTab)
        noteEditingTab->setGeometry(0, 0, width(), height());

    notesDisplayingTab->setGeometry(0, 0, width(), height());
}
