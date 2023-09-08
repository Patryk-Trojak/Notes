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
    void insertColor(const QColor &color, int pos = 0, bool isDefault = false);
    void setColumnCount(int columnCount);
    int getButtonCount() const;

  signals:
    void colorSelected(const QColor &color);
    void cancelButtonClicked();

  private:
    QVector<ColorButton *> colorButtons;
    QPushButton *cancelButton;
    void onColorButtonClicked();

  protected:
    void resizeEvent(QResizeEvent *event);
};

#endif // COLORPICKER_H
