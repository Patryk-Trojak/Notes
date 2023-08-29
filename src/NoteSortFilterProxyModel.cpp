#include "NoteSortFilterProxyModel.h"

#include "NoteListModelRole.h"
#include <QDateTime>

NoteSortFilterProxyModel::NoteSortFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{
}

bool NoteSortFilterProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    QVariant leftData = sourceModel()->data(source_left, sortRole());
    QVariant rightData = sourceModel()->data(source_right, sortRole());
    bool isLeftPinned = sourceModel()->data(source_left, NoteListModelRole::isPinned).toBool();
    bool isRightPinned = sourceModel()->data(source_right, NoteListModelRole::isPinned).toBool();

    // Pinned notes always go first
    if (isLeftPinned > isRightPinned)
    {
        if (sortOrder() == Qt::DescendingOrder)
            return false;

        return true;
    }

    if (isLeftPinned < isRightPinned)
    {
        if (sortOrder() == Qt::DescendingOrder)
            return true;

        return false;
    }

    if (leftData.userType() == QMetaType::QDateTime)
    {
        return leftData.toDateTime() < rightData.toDateTime();
    }
    else
    {
        if (QString::compare(leftData.toString(), rightData.toString(), Qt::CaseInsensitive) < 0)
            return true;
        else
            return false;
    }
}
