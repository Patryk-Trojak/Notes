#include "NoteButton.h"
#include "./ui_NoteButton.h"

NoteButton::NoteButton(const QString &title, const QDateTime &creationTime, const QDateTime &modificationTime,
                       QWidget *parent)
    : QWidget{parent}, ui(new Ui::NoteButton)
{
    this->setMinimumHeight(130);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QVBoxLayout *verticalLayout = new QVBoxLayout(this);
    verticalLayout->setSpacing(0);
    verticalLayout->setContentsMargins(0, 0, 0, 0);

    button = new QPushButton(this);
    verticalLayout->addWidget(button);

    ui->setupUi(button);
    setCreationTime(creationTime);
    setModificationTime(modificationTime);
    ui->titleEdit->setText(title);
    QObject::connect(ui->titleEdit, &QLineEdit::editingFinished, this, &NoteButton::saveNote);
    QObject::connect(button, &QPushButton::clicked, this, &NoteButton::enterEditingNote);
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
