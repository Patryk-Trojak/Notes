#include "MainWindow.h"

#include <QResizeEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), noteModel(this, persistenceManager), noteEditingTab(nullptr)
{
    setStyleSheet(
        "QMainWindow{background-color: white;}"
        "QScrollBar { background: transparent; color: rgba(255, 255, 255, 0); width: 10px; height: 10px; "
        "border-radius: 5px;} "
        "QScrollBar::handle:hover {background: rgba(40, 40, 40, 0.5);} "
        "QScrollBar::handle:pressed {background: rgba(40, 40, 40, 0.5);} "
        "QScrollBar::handle {border-radius: 4px; background: rgba(100, 100, 100, 0.5); "
        "min-height: 20px;}"
        "QScrollBar::add-line {width: 0px; height: 0px;subcontrol-position: bottom;subcontrol-origin: margin;}"
        "QScrollBar::sub-line {width: 0px;height: 0px;subcontrol-position: top;subcontrol-origin: margin;}");

    notesDisplayingTab = new NotesDisplayingTab(noteModel, persistenceManager, this);
    QObject::connect(notesDisplayingTab, &NotesDisplayingTab::enterEditingNote, this, &MainWindow::enterEditingNote);
    setMinimumSize(420, 200);
    setGeometry(200, 200, 1386, 780);
}

void MainWindow::enterEditingNote(const QModelIndex &note)
{
    if (noteEditingTab)
        delete noteEditingTab;

    noteEditingTab = new NoteEditingTab(persistenceManager, noteModel, note, this);
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

void MainWindow::closeEvent(QCloseEvent *event)
{
    noteModel.saveDirtyIndexes();
}
