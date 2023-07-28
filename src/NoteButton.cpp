#include "NoteButton.h"
#include "./ui_NoteButton.h"

#include <QResizeEvent>

NoteButton::NoteButton(QWidget *parent) : QPushButton{parent}, ui(new Ui::NoteButton)
{
    ui->setupUi(this);
    pinCheckbox = new QCheckBox(this);
    pinCheckbox->setGeometry(0, 0, 32, 32);
    pinCheckbox->setStyleSheet("QCheckBox::indicator:checked {image: url(:/images/pinChecked.png);} "
                               "QCheckBox::indicator:unchecked {image: url(:/images/pinUnchecked.png);}");
    pinCheckbox->setChecked(false);
    QObject::connect(pinCheckbox, &QCheckBox::toggled, this, &NoteButton::pinCheckboxToogled);
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

NoteButton::NoteButton(const QString &title, const QDateTime &creationTime, const QDateTime &modificationTime,
                       bool isPinned, QWidget *parent)
    : NoteButton(parent)
{
    setIsPinned(isPinned);
    setCreationTime(creationTime);
    setModificationTime(modificationTime);
    ui->titleEdit->setText(title);
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

void NoteButton::resizeEvent(QResizeEvent *event)
{
    QPushButton::resizeEvent(event);
    pinCheckbox->move(event->size().width() - pinCheckbox->width() - 5, 5);
}

bool NoteButton::getIsPinned() const
{
    return pinCheckbox->isChecked();
}

void NoteButton::setPinCheckboxVisible(bool visible)
{
    pinCheckbox->setVisible(visible);
}

void NoteButton::setIsPinned(bool newIsPinned)
{
    pinCheckbox->setChecked(newIsPinned);
}
