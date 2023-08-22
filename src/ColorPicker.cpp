#include "ColorPicker.h"

#include <QGridLayout>

ColorPicker::ColorPicker(QWidget *parent) : QWidget(parent)
{
    colorButtons.emplaceBack(new ColorButton(this, QColor(240, 198, 2)));
    colorButtons.emplaceBack(new ColorButton(this, QColor(234, 96, 68)));
    colorButtons.emplaceBack(new ColorButton(this, QColor(133, 165, 204)));
    colorButtons.emplaceBack(new ColorButton(this, QColor(73, 100, 145)));
    colorButtons.emplaceBack(new ColorButton(this, QColor(146, 196, 109)));
    colorButtons.emplaceBack(new ColorButton(this, QColor(41, 125, 125)));

    cancelButton = new QPushButton(this);
    cancelButton->setIcon(QIcon(":/images/cancel.png"));
    cancelButton->setStyleSheet("Margin: 0px; Padding: 0px; Border:none;");
    cancelButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QObject::connect(cancelButton, &QPushButton::clicked, this, &ColorPicker::cancelButtonClicked);
    for (auto const &button : colorButtons)
    {
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy ::Expanding);
        QObject::connect(button, &QPushButton::clicked, this, &ColorPicker::onColorButtonClicked);
    }

    QGridLayout *layout = new QGridLayout(this);
    layout->setSpacing(0);
    for (int i = 0; i < colorButtons.size(); i++)
        layout->addWidget(colorButtons[i], 0, i);

    layout->addWidget(cancelButton, 0, colorButtons.size());

    setAttribute(Qt::WA_StyledBackground, true);
    setAttribute(Qt::WA_NoMousePropagation, true);
    setStyleSheet("background-color: white");
}

void ColorPicker::onColorButtonClicked()
{
    ColorButton *button = static_cast<ColorButton *>(QObject::sender());
    emit colorSelected(button->getColor());
}

void ColorPicker::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (!colorButtons.empty())
        cancelButton->setIconSize(colorButtons[0]->size() - QSize(3, 3));
}
