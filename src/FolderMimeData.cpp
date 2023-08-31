#include "FolderMimeData.h"

#include <QIODevice>

const QString FolderMimeData::type = "application/folder";

FolderMimeData::FolderMimeData(int folderId) : folderId(folderId)
{
}

QMimeData *FolderMimeData::encodeData(const QVector<const FolderTreeItem *> &items)
{
    if (items.count() <= 0)
        return nullptr;

    QMimeData *data = new QMimeData();
    QByteArray encoded;
    QDataStream stream(&encoded, QIODevice::WriteOnly);

    for (auto it = items.cbegin(); it != items.cend(); ++it)
    {
        if (*it)
            stream << (*it)->data.getId();
    }

    data->setData(type, encoded);
    return data;
}

QVector<FolderMimeData> FolderMimeData::decodeData(const QMimeData *data)
{
    if (!data or !data->hasFormat(type))
        return {};

    QByteArray encoded = data->data(type);
    QDataStream stream(&encoded, QIODevice::ReadOnly);
    QVector<FolderMimeData> decodedData;

    while (!stream.atEnd())
    {
        int folderId;
        stream >> folderId;
        decodedData.emplaceBack(folderId);
    }

    return decodedData;
}
