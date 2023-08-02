#include "NoteButton.h"
#include "./ui_NoteButton.h"

#include <QResizeEvent>

NoteButton::NoteButton(QWidget *parent) : QPushButton{parent}, ui(new Ui::NoteButton), colorPicker(nullptr)
{
    ui->setupUi(this);
    pinCheckBox = new QCheckBox(this);
    pinCheckBox->setStyleSheet("QCheckBox::indicator:checked {image: url(:/images/pinChecked.png);}"
                               "QCheckBox::indicator:unchecked {image: url(:/images/pinUnchecked.png);}"
                               "QCheckBox::indicator {width: 28px; height: 28px;};");
    pinCheckBox->resize(28, 28);

    deleteButton = new QPushButton(this);
    deleteButton->setIcon(QIcon(":/images/delete.png"));
    deleteButton->setStyleSheet("Margin: 0px; Padding: 0px; Border:none;");
    deleteButton->setIconSize(QSize(28, 28));
    deleteButton->resize(28, 28);

    changeColorButton = new QPushButton(this);
    changeColorButton->setIcon(QIcon(":/images/palette.png"));
    changeColorButton->setStyleSheet("Margin: 0px; Padding: 0px; Border:none;");
    changeColorButton->setIconSize(QSize(28, 28));
    changeColorButton->resize(28, 28);

    QObject::connect(pinCheckBox, &QCheckBox::toggled, this, &NoteButton::pinCheckboxToogled);
    QObject::connect(deleteButton, &QPushButton::clicked, this, &NoteButton::deleteNote);

    setStyleSheet("QPushButton#NoteButton{border-style: solid; border-color: transparent; border-width: "
                  "1px;border-radius:8px;}");

    QObject::connect(changeColorButton, &QPushButton::clicked, this, &NoteButton::onChangeColorButtonClicked);
}

NoteButton::NoteButton(const QString &title, const QDateTime &modificationTime, bool isPinned, QWidget *parent)
    : NoteButton(parent)
{
    setIsPinned(isPinned);
    setModificationTime(modificationTime);
    setTitle(title);
}

QString NoteButton::getTitle() const
{
    return ui->title->text();
}

QString NoteButton::getContent() const
{
    return ui->content->text();
}

const QColor &NoteButton::getColor() const
{
    return color;
}

void NoteButton::setTitle(const QString &title)
{
    ui->title->setText(title);
}

void NoteButton::setContent(const QString &content)
{
    ui->content->setText(content);
}

void NoteButton::setModificationTime(const QDateTime &newModificationTime)
{
    this->modificationTime = newModificationTime;
    ui->modificationTime->setText("Modified: " + convertDateTimeToString(modificationTime));
}

QString NoteButton::convertDateTimeToString(const QDateTime &dateTime)
{
    if (dateTime.date() == QDate::currentDate())
        return dateTime.toString("hh:mm");
    else
        return dateTime.toString("dd.MM.yy");
}

void NoteButton::createColorPicker()
{
    if (colorPicker == nullptr)
    {
        colorPicker = new ColorPicker(this);
        colorPicker->setGeometry(10, 155, 180, 40);
        colorPicker->show();

        QObject::connect(colorPicker, &ColorPicker::colorSelected, this, &NoteButton::onColorSelected);
    }
}

void NoteButton::deleteColorPicker()
{
    if (colorPicker != nullptr)
    {
        delete colorPicker;
        colorPicker = nullptr;
    }
}

void NoteButton::onColorSelected(const QColor &color)
{
    if (colorPicker)
    {
        setColor(color);
        deleteColorPicker();
    }
}

void NoteButton::onChangeColorButtonClicked()
{
    if (colorPicker)
        deleteColorPicker();
    else
        this->createColorPicker();
}

void NoteButton::resizeEvent(QResizeEvent *event)
{
    QPushButton::resizeEvent(event);

    QPoint rightTop = QPoint(event->size().width(), 0);
    pinCheckBox->move(rightTop + QPoint(-pinCheckBox->width() - 10, 10));

    QPoint rightBottom = QPoint(rightTop.x(), event->size().height());
    deleteButton->move(rightBottom + QPoint(-deleteButton->width() - 10, -deleteButton->height() - 10));

    QPoint leftBottom = QPoint(0, event->size().height());
    changeColorButton->move(leftBottom + QPoint(10, -changeColorButton->height() - 10));
}

bool NoteButton::getIsPinned() const
{
    return pinCheckBox->isChecked();
}

void NoteButton::setPinCheckboxVisible(bool visible)
{
    pinCheckBox->setVisible(visible);
}

void NoteButton::setColor(const QColor &newColor)
{
    if (color == newColor)
        return;

    color = newColor;
    setStyleSheet(QString("QPushButton#NoteButton{border-style: solid; border-color: transparent; border-width: "
                          "1px;border-radius:8px; background-color: %1;}")
                      .arg(newColor.name()));
    emit colorChanged(newColor);
}

void NoteButton::setIsPinned(bool newIsPinned)
{
    pinCheckBox->setChecked(newIsPinned);
}
