#include "MainWindow.h"
#include "./ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), notesDisplayingTab(notesManager)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    QObject::connect(&noteEditTab, &NoteEditingTab::exitEditingNote, this, &MainWindow::exitEditingNote);
    QObject::connect(&notesDisplayingTab, &NotesDisplayingTab::enterEditingNote, this, &MainWindow::enterEditingNote);
    QObject::connect(&noteEditTab, &NoteEditingTab::saveNote, &notesManager, &NotesManager::saveNote);
    QObject::connect(&noteEditTab, &NoteEditingTab::deleteNote, this, &MainWindow::onDeletingNoteInEditingTab);

    ui->stackedWidget->insertWidget(0, &notesDisplayingTab);
    ui->stackedWidget->setCurrentIndex(0);
    ui->stackedWidget->insertWidget(1, &noteEditTab);
    ui->stackedWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::enterEditingNote(NoteData &note)
{
    noteEditTab.startEditingNewNote(&note);
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::exitEditingNote(NoteData &note)
{
    notesDisplayingTab.updateNoteButton(note);
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::onDeletingNoteInEditingTab(NoteData &note)
{
    qInfo() << "DONE" << '\n';
    notesManager.deleteNote(note);
    notesDisplayingTab.deleteNoteButton(note);
    ui->stackedWidget->setCurrentIndex(0);
}
