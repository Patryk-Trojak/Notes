#include "NoteMimeData.h"
#include "NoteListModelRole.h"
#include <QIODevice>

const QString NoteMimeData::type = "application/note";

NoteMimeData::NoteMimeData(int noteId, int parentFolderId) : noteId(noteId), parentFolderId(parentFolderId)
{
}

QMimeData *NoteMimeData::encodeData(const QModelIndexList &indexes)
{
    if (indexes.count() <= 0)
        return nullptr;

    QMimeData *data = new QMimeData();
    QByteArray encoded;
    QDataStream stream(&encoded, QIODevice::WriteOnly);

    for (auto it = indexes.cbegin(); it != indexes.cend(); ++it)
    {
        stream << it->data(NoteListModelRole::Id).toInt();
        stream << it->data(NoteListModelRole::ParentFolderId).toInt();
    }

    data->setData(type, encoded);
    return data;
}

QVector<NoteMimeData> NoteMimeData::decodeData(const QMimeData *data)
{
    if (!data or !data->hasFormat(type))
        return {};

    QByteArray encoded = data->data(type);
    QDataStream stream(&encoded, QIODevice::ReadOnly);
    QVector<NoteMimeData> decodedData;

    while (!stream.atEnd())
    {
        int id;
        stream >> id;
        int parentId;
        stream >> parentId;
        decodedData.emplaceBack(id, parentId);
    }

    return decodedData;
}
