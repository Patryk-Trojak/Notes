#include "NoteButton.h"
#include "./ui_NoteButton.h"

#include <QPainter>
#include <QResizeEvent>

NoteButton::NoteButton(QWidget *parent)
    : QPushButton{parent}, ui(new Ui::NoteButton), colorPicker(nullptr), isSelected(false)
{
    ui->setupUi(this);
    QFont titleFont("Cantarell", 13);
    titleFont.setBold(true);
    ui->title->setFont(titleFont);
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

    updateStyleSheets();

    QObject::connect(changeColorButton, &QPushButton::clicked, this, &NoteButton::onChangeColorButtonClicked);
    ui->modificationTime->setStyleSheet("color: rgba(26, 26, 26, 200)");
}

NoteButton::NoteButton(const QString &title, const QDateTime &modificationTime, bool isPinned, QWidget *parent)
    : NoteButton(parent)
{
    setIsPinned(isPinned);
    setModificationTime(modificationTime);
    setTitle(title);
}

NoteButton::~NoteButton()
{
    delete ui;
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

bool NoteButton::getIsSelected() const
{
    return isSelected;
}

void NoteButton::setIsSelected(bool newIsSelected)
{
    isSelected = newIsSelected;
    updateStyleSheets();
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
    ui->modificationTime->setText(convertDateTimeToString(modificationTime));
}

QString NoteButton::convertDateTimeToString(const QDateTime &dateTime)
{
    if (dateTime.date() == QDate::currentDate())
        return "Today, " + dateTime.toString("hh:mm");
    else if (dateTime.date() == QDate::currentDate().addDays(-1))
        return "Yesterday, " + dateTime.toString("hh:mm");
    else
        return dateTime.toString("MMM dd, yyyy");
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

void NoteButton::updateStyleSheets()
{
    QColor borderColor = getIsSelected() ? QColor(36, 36, 36) : Qt::transparent;
    setStyleSheet(QString("QPushButton#NoteButton{background-color: %1;"
                          "border-style: solid; border-color: %2; border-width: 3px; border-radius: 8px;}")
                      .arg(getColor().name(), borderColor.name(QColor::HexArgb)));
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
    updateStyleSheets();
    emit colorChanged(newColor);
}

void NoteButton::setIsPinned(bool newIsPinned)
{
    pinCheckBox->setChecked(newIsPinned);
}
