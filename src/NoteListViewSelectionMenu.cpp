#include "NoteListViewSelectionMenu.h"

#include <QHBoxLayout>

NoteListViewSelectionMenu::NoteListViewSelectionMenu(QWidget *parent) : QWidget{parent}
{
    pinButton = createButton(":/images/pinCheckedWhite.png");
    deleteButton = createButton(":/images/deleteWhite.png");
    changeColorButton = createButton(":/images/paletteWhite.png");
    cancelButton = createButton(":/images/cancelWhite.png");

    selectedNotesCountLabel = new QLabel(this);
    selectedNotesCountLabel->setText("0");
    selectedNotesCountLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    selectedNotesCountLabel->setAlignment(Qt::AlignCenter);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(selectedNotesCountLabel);
    layout->addWidget(pinButton);
    layout->addWidget(changeColorButton);
    layout->addWidget(deleteButton);
    layout->addWidget(cancelButton);

    colorPicker = new ColorPicker(this);
    colorPicker->setVisible(false);

    QObject::connect(changeColorButton, &QPushButton::clicked, this,
                     &NoteListViewSelectionMenu::toogleColorPickerVisible);
    QObject::connect(pinButton, &QPushButton::clicked, this, &NoteListViewSelectionMenu::pinButtonClicked);
    QObject::connect(deleteButton, &QPushButton::clicked, this, &NoteListViewSelectionMenu::deleteButtonClicked);
    QObject::connect(cancelButton, &QPushButton::clicked, this, &NoteListViewSelectionMenu::cancelButtonClicked);
    QObject::connect(colorPicker, &ColorPicker::colorSelected, this, &NoteListViewSelectionMenu::colorSelected);
    QObject::connect(colorPicker, &ColorPicker::colorSelected, this,
                     &NoteListViewSelectionMenu::toogleColorPickerVisible);
    QObject::connect(colorPicker, &ColorPicker::cancelButtonClicked, this,
                     &NoteListViewSelectionMenu::toogleColorPickerVisible);

    setAttribute(Qt::WA_NoMousePropagation, true);
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("QWidget{"
                  "background: rgba(0,60,255,255);"
                  "border-style: solid; "
                  "border-color: transparent; "
                  "border-width: 1px;"
                  "border-radius:20px;"
                  "color: white;}");
}

void NoteListViewSelectionMenu::setColorPickerVisible(bool visible)
{
    colorPicker->setVisible(visible);
}

void NoteListViewSelectionMenu::toogleColorPickerVisible()
{
    colorPicker->setVisible(!colorPicker->isVisible());
}

void NoteListViewSelectionMenu::setSelectedNotesCount(int selectedNotesCount)
{
    QString selectedNotesCountString = QString::number(selectedNotesCount);
    if (selectedNotesCount > 9999)
        selectedNotesCountString = "9999+";

    this->selectedNotesCountLabel->setText(selectedNotesCountString);
    int minWidth =
        this->selectedNotesCountLabel->fontMetrics().averageCharWidth() * selectedNotesCountString.size() + 10;
    this->selectedNotesCountLabel->setMinimumWidth(minWidth);
}

QPushButton *NoteListViewSelectionMenu::createButton(const QString &iconPath)
{
    QPushButton *button = new QPushButton(this);
    button->setIcon(QIcon(iconPath));
    button->setStyleSheet("Margin: 0px; Padding: 0px; Border:none;");
    button->setIconSize(QSize(28, 28));
    button->setMaximumSize(QSize(28, 28));
    button->resize(28, 28);
    return button;
}

void NoteListViewSelectionMenu::resizeEvent(QResizeEvent *event)
{
    colorPicker->setGeometry(0, 0, width(), height());
}
