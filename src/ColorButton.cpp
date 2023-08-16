#include "ColorButton.h"
#include <QPainter>
#include <QResizeEvent>

ColorButton::ColorButton(QWidget *parent, const QColor &color) : QPushButton(parent), color(color)
{
}

QColor ColorButton::getColor() const
{
    return color;
}

void ColorButton::setColor(const QColor &newColor)
{
    color = newColor;
}

void ColorButton::resizeEvent(QResizeEvent *event)
{
    QPushButton::resizeEvent(event);
    setMask(QRegion(getSquereInCenterOfButton(), QRegion::Ellipse));
}

void ColorButton::paintEvent(QPaintEvent *event)
{
    QColor backgroundColor = isDown() ? color.darker() : color;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.translate(rect().topLeft());
    painter.setPen(QPen(backgroundColor, 0));
    painter.setBrush(QBrush(backgroundColor));
    QRect drawRect = getSquereInCenterOfButton();
    drawRect.adjust(2, 2, -2, -2);
    painter.drawEllipse(drawRect);
}

QRect ColorButton::getSquereInCenterOfButton()
{
    auto minEdge = qMin(width(), height());
    return QRect((width() - minEdge) / 2, (height() - minEdge) / 2, minEdge, minEdge);
}
