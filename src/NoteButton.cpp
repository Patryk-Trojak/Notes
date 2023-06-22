#include "NoteButton.h"
#include "./ui_NoteButton.h"

NoteButton::NoteButton(const QString &title, QWidget *parent) : QWidget{parent}, ui(new Ui::NoteButton)
{
    this->setMinimumHeight(130);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QVBoxLayout *verticalLayout = new QVBoxLayout(this);
    verticalLayout->setSpacing(0);
    verticalLayout->setContentsMargins(0, 0, 0, 0);

    button = new QPushButton(this);
    verticalLayout->addWidget(button);

    ui->setupUi(button);
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
