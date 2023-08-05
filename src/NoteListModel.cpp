#include "NoteListModel.h"
#include <QTimer>

NoteListModel::NoteListModel(QObject *parent, PersistenceManager &persistenceManager)
    : QAbstractItemModel(parent), persistenceManager(persistenceManager), currentSelectedFolderId(-1)
{
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &NoteListModel::saveDirtyIndexes);
    timer->start(10000);
}

QModelIndex NoteListModel::index(int row, int column, const QModelIndex &parent) const
{
    return createIndex(row, 0, &notes[row]);
}

int NoteListModel::rowCount(const QModelIndex &parent) const
{
    return notes.size();
}

int NoteListModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

const NoteData &NoteListModel::getNoteData(const QModelIndex &index) const
{
    return notes[index.row()];
}

QVariant NoteListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    int row = index.row();
    switch (role)
    {
    case Role::Title:
    case Role::EditRole:
    case Role::DisplayRole:
        return notes[row].getTitle();
    case Role::Content:
        return notes[row].getContent();
    case Role::Id:
        return notes[row].getId();
    case Role::ParentFolderId:
        return notes[row].getParentFolderId();
    case Role::CreationTime:
        return notes[row].getCreationTime();
    case Role::ModificationTime:
        return notes[row].getModificationTime();
    case Role::isInTrash:
        return notes[row].getIsInTrash();
    case Role::isPinned:
        return notes[row].getIsPinned();
    case Role::Color:
        return QVariant::fromValue(notes[row].getColor());
    default:
        return QVariant();
    }

    return QVariant();
}

void NoteListModel::setNoteData(const QModelIndex &index, const NoteData &noteData)
{
    notes[index.row()] = noteData;
    markIndexAsDirty(index);
    emit dataChanged(index, index);
}

bool NoteListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    int row = index.row();
    switch (role)
    {
    case Role::Title:
    case Role::EditRole:
        notes[row].setTitle(value.toString());
        markIndexAsDirty(index);
        emit dataChanged(index, index);
        return true;
    case Role::Content:
        notes[row].setContent(value.toString());
        markIndexAsDirty(index);
        emit dataChanged(index, index);
        return true;
    case Role::Id:
        notes[row].setId(value.toInt());
        markIndexAsDirty(index);
        emit dataChanged(index, index);
        return true;
    case Role::ParentFolderId:
        notes[row].setParentFolderId(value.toInt());
        markIndexAsDirty(index);
        emit dataChanged(index, index);
        return true;
    case Role::CreationTime:
        notes[row].setCreationTime(value.toDateTime());
        markIndexAsDirty(index);
        emit dataChanged(index, index);
        return true;
    case Role::ModificationTime:
        notes[row].setModificationTime(value.toDateTime());
        markIndexAsDirty(index);
        emit dataChanged(index, index);
        return true;
    case Role::isInTrash:
        notes[row].setIsInTrash(value.toBool());
        markIndexAsDirty(index);
        emit dataChanged(index, index);
        return true;
    case Role::isPinned:
        notes[row].setIsPinned(value.toBool());
        markIndexAsDirty(index);
        emit dataChanged(index, index);
        return true;
    case Role::Color:
        notes[row].setColor(value.value<QColor>());
        markIndexAsDirty(index);
        emit dataChanged(index, index);
        return true;
    }

    return false;
}

bool NoteListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (currentSelectedFolderId == SpecialFolderId::TrashFolder)
        return false;

    beginInsertRows(parent, row, row + count - 1);
    auto inserted = notes.insert(row, count, NoteData{});

    for (int i = 0; i < count; i++)
    {
        inserted->setParentFolderId(currentSelectedFolderId);
        inserted->setTitle("Untitled");
        inserted->setCreationTime(QDateTime::currentDateTime());
        inserted->setModificationTime(QDateTime::currentDateTime());
        inserted->setColor(QColor(133, 165, 204));
        int id = persistenceManager.addNote(*inserted);
        inserted->setId(id);
    }
    endInsertRows();

    return true;
}

bool NoteListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    for (int i = row; i < row + count; i++)
    {
        if (currentSelectedFolderId == SpecialFolderId::TrashFolder)
            persistenceManager.deleteNote(notes[i].getId());
        else
            persistenceManager.moveNoteToTrash(notes[i].getId());
    }
    beginRemoveRows(parent, row, row + count - 1);
    notes.remove(row, count);
    endRemoveRows();
    return true;
}

QModelIndex NoteListModel::createNewNote()
{
    if (currentSelectedFolderId == SpecialFolderId::TrashFolder)
    {
        return QModelIndex();
    }

    bool insertedOk = insertRows(0, 1, QModelIndex());
    if (insertedOk)
        return index(0, 0, QModelIndex());

    return QModelIndex();
}

void NoteListModel::restoreNoteFromTrash(const QModelIndex &index)
{
    persistenceManager.restoreNoteFromTrash(notes[index.row()].getId());
    beginRemoveRows(QModelIndex(), index.row(), index.row());
    notes.remove(index.row(), 1);
    endRemoveRows();
}

void NoteListModel::onNewFolderSelected(int selectedFolderId)
{
    saveDirtyIndexes();
    beginResetModel();
    currentSelectedFolderId = selectedFolderId;
    notes.clear();

    if (selectedFolderId == SpecialFolderId::AllNotesFolder)
        notes = persistenceManager.loadAllNotes();
    else if (selectedFolderId == SpecialFolderId::TrashFolder)
        notes = persistenceManager.loadAllNotesFromTrash();
    else
        notes = persistenceManager.loadAllNotesFromFolder(currentSelectedFolderId);

    endResetModel();
}

void NoteListModel::onFolderDeleted(int deletedFolderId)
{
    if (currentSelectedFolderId == SpecialFolderId::TrashFolder)
        persistenceManager.deleteAllNotesFromFolder(deletedFolderId);
    else
        persistenceManager.moveAllNotesFromFolderToTrash(deletedFolderId);
}

void NoteListModel::markIndexAsDirty(const QModelIndex &index)
{
    if (dirtyIndexes.contains(index))
        return;

    dirtyIndexes.emplace_back(index);
}

void NoteListModel::saveDirtyIndexes()
{
    for (auto const &dirty : dirtyIndexes)
    {
        if (dirty.isValid())
            persistenceManager.updateNote(notes[dirty.row()]);
    }
    dirtyIndexes.clear();
}

int NoteListModel::getCurrentSelectedFolderId() const
{
    return currentSelectedFolderId;
}

QModelIndex NoteListModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}

Qt::ItemFlags NoteListModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}
