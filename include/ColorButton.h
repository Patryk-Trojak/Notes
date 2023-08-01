#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include <QPushButton>

class ColorButton : public QPushButton
{
  public:
    ColorButton(QWidget *parent, const QColor &color = QColor());

    QColor getColor() const;
    void setColor(const QColor &newColor);

  private:
    QColor color;
    QRect getSquereInCenterOfButton();

  protected:
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);
};

#endif // COLORBUTTON_H
