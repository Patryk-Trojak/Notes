#include "NoteListModel.h"

NoteListModel::NoteListModel(QObject *parent, PersistenceManager &persistenceManager)
    : QAbstractItemModel(parent), persistenceManager(persistenceManager), currentSelectedFolderId(-1)
{
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
    default:
        return QVariant();
    }

    return QVariant();
}

void NoteListModel::setNoteData(const QModelIndex &index, const NoteData &noteData)
{
    notes[index.row()] = noteData;
    persistenceManager.updateNote(notes[index.row()]);
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
        persistenceManager.updateNote(notes[index.row()]);
        emit dataChanged(index, index);
        return true;
    case Role::Content:
        notes[row].setContent(value.toString());
        persistenceManager.updateNote(notes[index.row()]);
        emit dataChanged(index, index);
        return true;
    case Role::Id:
        notes[row].setId(value.toInt());
        persistenceManager.updateNote(notes[index.row()]);
        emit dataChanged(index, index);
        return true;
    case Role::ParentFolderId:
        notes[row].setParentFolderId(value.toInt());
        persistenceManager.updateNote(notes[index.row()]);
        emit dataChanged(index, index);
        return true;
    case Role::CreationTime:
        notes[row].setCreationTime(value.toDateTime());
        persistenceManager.updateNote(notes[index.row()]);
        emit dataChanged(index, index);
        return true;
    case Role::ModificationTime:
        notes[row].setModificationTime(value.toDateTime());
        persistenceManager.updateNote(notes[index.row()]);
        emit dataChanged(index, index);
        return true;
    }

    return false;
}

bool NoteListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    auto inserted = notes.insert(row, count, NoteData{});

    for (int i = 0; i < count; i++)
    {
        inserted->setParentFolderId(currentSelectedFolderId);
        inserted->setTitle("Untitled");
        inserted->setCreationTime(QDateTime::currentDateTime());
        inserted->setModificationTime(QDateTime::currentDateTime());
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
        persistenceManager.deleteNoteFile(notes[i].getId());
    }
    beginRemoveRows(parent, row, row + count - 1);
    notes.remove(row, count);
    endRemoveRows();
    return true;
}

void NoteListModel::createNewNote()
{
    insertRows(0, 1, QModelIndex());
}

void NoteListModel::onNewFolderSelected(int selectedFolderId)
{
    beginResetModel();
    currentSelectedFolderId = selectedFolderId;
    notes.clear();
    if (selectedFolderId >= 0)
        notes = persistenceManager.loadAllNotesFromFolder(selectedFolderId);
    else if (selectedFolderId == SpecialFolderId::AllNotesFolder)
        notes = persistenceManager.loadAllNotes();

    endResetModel();
}

void NoteListModel::onFolderDeleted(int deletedFolderId)
{
    persistenceManager.deleteAllNotesFromFolder(deletedFolderId);
}

QModelIndex NoteListModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}

Qt::ItemFlags NoteListModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}
