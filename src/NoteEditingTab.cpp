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
    lastSavedTitle = note->getTitle();
    lastSavedContent = note->getContent();
}

void NoteEditingTab::saveNote()
{
    if (!hasNoteChanged())
        return;

    currentEditingNote->setTitle(ui->titleEdit->text());
    currentEditingNote->setContent(ui->contentEdit->toPlainText());
    lastSavedTitle = currentEditingNote->getTitle();
    lastSavedContent = currentEditingNote->getContent();
    PersistenceManager::saveNoteToFile(*currentEditingNote);
}

bool NoteEditingTab::hasNoteChanged()
{
    if (lastSavedTitle == ui->titleEdit->text() and lastSavedContent == ui->contentEdit->toPlainText())
        return false;

    return true;
}
