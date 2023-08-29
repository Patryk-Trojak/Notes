#ifndef NOTEMIMEDATA_H
#define NOTEMIMEDATA_H
#include <QMimeData>
#include <QModelIndexList>
#include <QString>

class NoteMimeData
{
  public:
    NoteMimeData(int noteId, int parentFolderId);
    int noteId;
    int parentFolderId;

    static const QString type;
    static QMimeData *encodeData(const QModelIndexList &indexes);
    static QVector<NoteMimeData> decodeData(const QMimeData *data);
};

#endif // NOTEMIMEDATA_H
