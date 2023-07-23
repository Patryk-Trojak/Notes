#include "NoteButton.h"
#include "./ui_NoteButton.h"

NoteButton::NoteButton(const QString &title, const QDateTime &creationTime, const QDateTime &modificationTime,
                       QWidget *parent)
    : QPushButton{parent}, ui(new Ui::NoteButton), lastSavedTitle(title)
{
    ui->setupUi(this);
    setCreationTime(creationTime);
    setModificationTime(modificationTime);
    ui->titleEdit->setText(title);
    QObject::connect(ui->titleEdit, &QLineEdit::editingFinished, this, [this]() {
        if (ui->titleEdit->text() != lastSavedTitle)
        {
            emit saveNote();
            setModificationTime(QDateTime::currentDateTime());
            lastSavedTitle = ui->titleEdit->text();
        }
    });
    QObject::connect(this, &QPushButton::clicked, this, &NoteButton::enterEditingNote);
    QObject::connect(ui->deleteButton, &QPushButton::clicked, this, &NoteButton::deleteNote);
}

QString NoteButton::getTitle() const
{
    return ui->titleEdit->text();
}

void NoteButton::setTitle(const QString &title)
{
    ui->titleEdit->setText(title);
}

void NoteButton::setCreationTime(const QDateTime &newCreationTime)
{
    this->creationTime = newCreationTime;
    ui->creationTime->setText("Creation time: " + convertDateTimeToString(newCreationTime));
}

void NoteButton::setModificationTime(const QDateTime &newModificationTime)
{
    this->modificationTime = newModificationTime;
    ui->modificationTime->setText("Last modified: " + convertDateTimeToString(newModificationTime));
}

const QDateTime &NoteButton::getCreationTime() const
{
    return creationTime;
}

const QDateTime &NoteButton::getModificationTime() const
{
    return modificationTime;
}

QString NoteButton::convertDateTimeToString(const QDateTime &dateTime)
{
    return dateTime.toString("dd.MM.yy hh:mm:ss");
}
