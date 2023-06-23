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

} // namespace myUtils
