#include "MainWindow.h"
#include "./ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), noteModel(this, persistenceManager),
      notesDisplayingTab(noteModel, persistenceManager, this), noteEditTab(noteModel, this)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    QObject::connect(&noteEditTab, &NoteEditingTab::exitEditingNote, this, &MainWindow::exitEditingNote);
    QObject::connect(&notesDisplayingTab, &NotesDisplayingTab::enterEditingNote, this, &MainWindow::enterEditingNote);

    ui->stackedWidget->insertWidget(0, &notesDisplayingTab);
    ui->stackedWidget->setCurrentIndex(0);
    ui->stackedWidget->insertWidget(1, &noteEditTab);
    ui->stackedWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::enterEditingNote(const QModelIndex &note)
{
    noteEditTab.startEditingNewNote(note);
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::exitEditingNote()
{
    ui->stackedWidget->setCurrentIndex(0);
}
