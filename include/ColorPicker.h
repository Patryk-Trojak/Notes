#ifndef COLORPICKER_H
#define COLORPICKER_H

#include "ColorButton.h"

#include <QPushButton>
#include <QWidget>

class ColorPicker : public QWidget
{
    Q_OBJECT
  public:
    ColorPicker(QWidget *parent);

  signals:
    void colorSelected(const QColor &color);

  private:
    QVector<ColorButton *> colorButtons;
    void onColorButtonClicked();
};

#endif // COLORPICKER_H
