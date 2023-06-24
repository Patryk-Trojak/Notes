#include "NoteEditingTab.h"
#include "PersistenceManager.h"
#include "ui_NoteEditingTab.h"

NoteEditingTab::NoteEditingTab(QWidget *parent) : QWidget(parent), ui(new Ui::NoteEditingTab)
{
    ui->setupUi(this);
    QObject::connect(ui->saveAndReturn, &QPushButton::clicked, this, &NoteEditingTab::saveNote);

    QObject::connect(ui->saveAndReturn, &QPushButton::clicked, this,
                     [this]() { emit exitEditingNote(*currentEditingNote); });
}

NoteEditingTab::~NoteEditingTab()
{
    delete ui;
}

void NoteEditingTab::startEditingNewNote(Note *note)
{
    currentEditingNote = note;
    ui->titleEdit->setText(note->getTitle());
    ui->contentEdit->setText(note->getContent());
}

void NoteEditingTab::saveNote()
{
    currentEditingNote->setTitle(ui->titleEdit->text());
    currentEditingNote->setContent(ui->contentEdit->toPlainText());
    PersistenceManager::saveNoteToFile(*currentEditingNote);
}
