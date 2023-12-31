#include "ColorButton.h"
#include <QPainter>
#include <QResizeEvent>

ColorButton::ColorButton(QWidget *parent, const QColor &color, bool drawAsDefaultColor)
    : QPushButton(parent), color(color), drawAsDefaultColor(drawAsDefaultColor)
{
    setStyleSheet(QString("background-color: %1").arg(color.name()));
}

QColor ColorButton::getColor() const
{
    return color;
}

void ColorButton::setColor(const QColor &newColor)
{
    color = newColor;
    setStyleSheet(QString("background-color: %1").arg(color.name()));
}

void ColorButton::resizeEvent(QResizeEvent *event)
{
    QPushButton::resizeEvent(event);
    setMask(QRegion(getSquereInCenterOfButton(), QRegion::Ellipse));
}

void ColorButton::paintEvent(QPaintEvent *event)
{
    QColor backgroundColor;
    if (drawAsDefaultColor)
        backgroundColor = Qt::transparent;
    else
        backgroundColor = isDown() ? color.darker() : color;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.translate(rect().topLeft());
    painter.setPen(QPen(backgroundColor, 0));
    painter.setBrush(QBrush(backgroundColor));

    QRect drawRect = getSquereInCenterOfButton();
    drawRect.adjust(2, 2, -2, -2);
    painter.drawEllipse(drawRect);

    if (drawAsDefaultColor)
    {
        QIcon defaultColorIcon(QString(":/images/defaultColor.png"));
        painter.drawPixmap(drawRect, defaultColorIcon.pixmap(drawRect.size()));
    }
}

QRect ColorButton::getSquereInCenterOfButton()
{
    auto minEdge = qMin(width(), height());
    return QRect((width() - minEdge) / 2, (height() - minEdge) / 2, minEdge, minEdge);
}
