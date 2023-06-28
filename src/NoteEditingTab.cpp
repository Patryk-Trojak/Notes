#include "NoteEditingTab.h"
#include "PersistenceManager.h"
#include "ui_NoteEditingTab.h"
#include <QMessageBox>

NoteEditingTab::NoteEditingTab(QWidget *parent) : QWidget(parent), ui(new Ui::NoteEditingTab)
{
    ui->setupUi(this);
    QObject::connect(ui->saveAndReturn, &QPushButton::clicked, this, &NoteEditingTab::onSaveNoteButtonPressed);
    QObject::connect(ui->saveAndReturn, &QPushButton::clicked, this,
                     [this]() { emit exitEditingNote(*currentEditingNote); });
    QObject::connect(ui->deleteButton, &QPushButton::clicked, this, &NoteEditingTab::onDeleteNoteButtonPressed);
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

void NoteEditingTab::onSaveNoteButtonPressed()
{
    if (!hasNoteChanged())
        return;

    currentEditingNote->setTitle(ui->titleEdit->text());
    currentEditingNote->setContent(ui->contentEdit->toPlainText());
    lastSavedTitle = currentEditingNote->getTitle();
    lastSavedContent = currentEditingNote->getContent();
    emit saveNote(*currentEditingNote);
}

bool NoteEditingTab::hasNoteChanged()
{
    if (lastSavedTitle == ui->titleEdit->text() and lastSavedContent == ui->contentEdit->toPlainText())
        return false;

    return true;
}

void NoteEditingTab::onDeleteNoteButtonPressed()
{
    auto reply = QMessageBox::question(this, "Delete note?", "Are you sure you want to delete this note?");
    if (reply == QMessageBox::No)
        return;
    emit deleteNote(*currentEditingNote);
}
