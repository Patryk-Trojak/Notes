#include "NoteSortFilterProxyModel.h"

#include <QDateTime>

NoteSortFilterProxyModel::NoteSortFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{
}

bool NoteSortFilterProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    QVariant leftData = sourceModel()->data(source_left, sortRole());
    QVariant rightData = sourceModel()->data(source_right, sortRole());
    qDebug() << "Its working";

    if (leftData.userType() == QMetaType::QDateTime)
    {
        return leftData.toDateTime() < rightData.toDateTime();
    }
    else
    {
        return leftData.toString() < leftData.toString();
    }
}
