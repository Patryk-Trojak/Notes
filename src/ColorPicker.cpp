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

    for (auto const &button : colorButtons)
    {
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy ::Expanding);
        QObject::connect(button, &QPushButton::clicked, this, &ColorPicker::onColorButtonClicked);
    }

    QGridLayout *layout = new QGridLayout(this);
    layout->setSpacing(0);
    for (int i = 0; i < colorButtons.size(); i++)
        layout->addWidget(colorButtons[i], 0, i);

    setAutoFillBackground(true);
    setPalette(Qt::white);
}

void ColorPicker::onColorButtonClicked()
{
    ColorButton *button = static_cast<ColorButton *>(QObject::sender());
    emit colorSelected(button->getColor());
}
