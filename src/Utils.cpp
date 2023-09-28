#include "Utils.h"

namespace myUtils
{

QList<QWidget *> getWidgetsFromLayout(const QLayout &layout)
{
    QList<QWidget *> listOfWidgets;
    for (int i = 0; i < layout.count(); i++)
    {
        QWidget *widget = layout.itemAt(i)->widget();
        if (widget != nullptr)
            listOfWidgets.append(widget);
    }
    return listOfWidgets;
}

void replaceColor(QImage &image, const QColor &oldColor, const QColor &newColor)
{
    for (int y = 0; y < image.height(); y++)
    {
        for (int x = 0; x < image.width(); x++)
        {
            if (image.pixelColor(x, y).rgb() == oldColor.rgb())
            {
                QColor color = newColor;
                color.setAlpha(image.pixelColor(x, y).alpha());
                image.setPixelColor(x, y, color);
            }
        }
    }
}
} // namespace myUtils
