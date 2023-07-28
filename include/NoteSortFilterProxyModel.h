#ifndef NOTESORTFILTERPROXYMODEL_H
#define NOTESORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class NoteSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

  public:
    NoteSortFilterProxyModel(QObject *parent);

  protected:
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;
};

#endif // NOTESORTFILTERPROXYMODEL_H
