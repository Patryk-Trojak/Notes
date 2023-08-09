#ifndef NOTESORTOPTIONSWIDGET_H
#define NOTESORTOPTIONSWIDGET_H

#include <QWidget>

namespace Ui
{
class NoteSortOptionsWidget;
}

class NoteSortOptionsWidget : public QWidget
{
    Q_OBJECT

  public:
    NoteSortOptionsWidget(int currentSortRole, Qt::SortOrder currentSortOrder, QWidget *parent = nullptr,
                          Qt::WindowFlags windowsFlags = Qt::WindowFlags());

  signals:
    void newSortOrderSelected(Qt::SortOrder newSortOrder);
    void newSortRoleSelected(int newSortRole);

  private:
    Ui::NoteSortOptionsWidget *ui;
    QHash<int, int> indexToSortRoleMap;
    QHash<int, Qt::SortOrder> indexToSordOrderMap;
    int convertIndexToSortRole(int index);
    Qt::SortOrder convertIndexToSortOrder(int index);

  protected:
    void paintEvent(QPaintEvent *event);
};

#endif // NOTESORTOPTIONSWIDGET_H
