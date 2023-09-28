#include <QLayout>
#include <QList>
#include <QWidget>

namespace myUtils
{
QList<QWidget *> getWidgetsFromLayout(const QLayout &layout);

void replaceColor(QImage &image, const QColor &oldColor, const QColor &newColor);

} // namespace myUtils
