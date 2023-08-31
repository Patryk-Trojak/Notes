#ifndef FOLDERMIMEDATA_H
#define FOLDERMIMEDATA_H

#include "FolderTreeItem.h"
#include <QMimeData>
#include <QModelIndexList>
#include <QString>

class FolderMimeData
{
  public:
    FolderMimeData(int folderId);
    int folderId;

    static const QString type;
    static QMimeData *encodeData(const QVector<const FolderTreeItem *> &items);
    static QVector<FolderMimeData> decodeData(const QMimeData *data);
};

#endif // FOLDERMIMEDATA_H
