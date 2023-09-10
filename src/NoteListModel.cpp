#include "NoteListModel.h"
#include "SpecialFolderId.h"
#include <QHash>
#include <QMimeData>
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
    case NoteListModelRole::Title:
    case NoteListModelRole::EditRole:
    case NoteListModelRole::DisplayRole:
        return notes[row].getTitle();
    case NoteListModelRole::Content:
        return notes[row].getContent();
    case NoteListModelRole::Id:
        return notes[row].getId();
    case NoteListModelRole::ParentFolderId:
        return notes[row].getParentFolderId();
    case NoteListModelRole::CreationTime:
        return notes[row].getCreationTime();
    case NoteListModelRole::ModificationTime:
        return notes[row].getModificationTime();
    case NoteListModelRole::isInTrash:
        return notes[row].getIsInTrash();
    case NoteListModelRole::isPinned:
        return notes[row].getIsPinned();
    case NoteListModelRole::Color:
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
    case NoteListModelRole::Title:
    case NoteListModelRole::EditRole:
        notes[row].setTitle(value.toString());
        markIndexAsDirty(index);
        emit dataChanged(index, index);
        return true;
    case NoteListModelRole::Content:
        notes[row].setContent(value.toString());
        markIndexAsDirty(index);
        emit dataChanged(index, index);
        return true;
    case NoteListModelRole::Id:
        notes[row].setId(value.toInt());
        markIndexAsDirty(index);
        emit dataChanged(index, index);
        return true;
    case NoteListModelRole::ParentFolderId:
        notes[row].setParentFolderId(value.toInt());
        markIndexAsDirty(index);
        emit dataChanged(index, index);
        return true;
    case NoteListModelRole::CreationTime:
        notes[row].setCreationTime(value.toDateTime());
        markIndexAsDirty(index);
        emit dataChanged(index, index);
        return true;
    case NoteListModelRole::ModificationTime:
        notes[row].setModificationTime(value.toDateTime());
        markIndexAsDirty(index);
        emit dataChanged(index, index);
        return true;
    case NoteListModelRole::isInTrash:
        notes[row].setIsInTrash(value.toBool());
        markIndexAsDirty(index);
        emit dataChanged(index, index);
        return true;
    case NoteListModelRole::isPinned:
        notes[row].setIsPinned(value.toBool());
        markIndexAsDirty(index);
        emit dataChanged(index, index);
        return true;
    case NoteListModelRole::Color:
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

    emit notesAdded(currentSelectedFolderId, count);
    return true;
}

bool NoteListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    for (int i = row; i < row + count; i++)
    {
        if (currentSelectedFolderId == SpecialFolderId::TrashFolder)
        {
            persistenceManager.deleteNote(notes[i].getId());
            emit notesRemoved(SpecialFolderId::TrashFolder, 1);
        }
        else
        {
            persistenceManager.moveNoteToTrash(notes[i].getId());
            emit notesMoved(notes[i].getParentFolderId(), SpecialFolderId::TrashFolder, 1);
        }
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

void NoteListModel::moveNotesToFolder(const QSet<int> &noteIds, int folderId)
{
    auto note = notes.begin();
    persistenceManager.moveNotesToFolder(noteIds, folderId);

    while (note != notes.end())
    {
        if (!noteIds.contains(note->getId()))
        {
            note++;
            continue;
        }

        int oldParentFolderId = note->getParentFolderId();
        note->setParentFolderId(folderId);
        emit notesMoved(oldParentFolderId, folderId, 1);

        if (noteShouldBeDisplayedInCurrentSelectedFolder(*note))
        {
            note++;
            continue;
        }

        int indexToRemove = note - notes.begin();
        beginRemoveRows(QModelIndex(), indexToRemove, indexToRemove);
        note = notes.erase(note);
        endRemoveRows();
    }
}

void NoteListModel::setColorOfNotes(const QModelIndexList &indexes, const QColor &color)
{
    QVector<int> noteIds = getNoteIds(indexes);

    persistenceManager.setColorOfNotes(noteIds, color);

    for (auto const &index : indexes)
    {
        notes[index.row()].setColor(color);
        emit dataChanged(index, index);
    }
}

void NoteListModel::setIsPinnedOfNotes(const QModelIndexList &indexes, bool isPinned)
{
    QVector<int> noteIds = getNoteIds(indexes);
    persistenceManager.setIsPinnedOfNotes(noteIds, isPinned);
    for (auto const &index : indexes)
    {
        notes[index.row()].setIsPinned(isPinned);
        emit dataChanged(index, index);
    }
}

void NoteListModel::removeNotes(QModelIndexList &indexes)
{
    QVector<int> noteIds = getNoteIds(indexes);

    if (currentSelectedFolderId == SpecialFolderId::TrashFolder)
        persistenceManager.deleteNotes(noteIds);
    else
        persistenceManager.moveNotesToTrash(noteIds);

    std::sort(indexes.begin(), indexes.end(),
              [](const QModelIndex &first, const QModelIndex &second) { return first.row() > second.row(); });

    for (auto it = indexes.constBegin(); it != indexes.constEnd(); ++it)
    {
        if (currentSelectedFolderId == SpecialFolderId::TrashFolder)
            emit notesRemoved(notes[it->row()].getParentFolderId(), 1);
        else
            emit notesMoved(notes[it->row()].getParentFolderId(), SpecialFolderId::TrashFolder, 1);

        beginRemoveRows(it->parent(), it->row(), it->row());
        notes.remove(it->row());
        endRemoveRows();
    }
}

void NoteListModel::restoreNoteFromTrash(const QModelIndex &index)
{
    persistenceManager.restoreNoteFromTrash(notes[index.row()].getId());
    emit notesMoved(SpecialFolderId::TrashFolder, SpecialFolderId::AllNotesFolder, 1);
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
    int notesMovedCount = persistenceManager.countNotesInFolder(deletedFolderId);
    persistenceManager.moveAllNotesFromFolderToTrash(deletedFolderId);
    emit notesMoved(SpecialFolderId::AllNotesFolder, SpecialFolderId::TrashFolder, notesMovedCount);
}

void NoteListModel::markIndexAsDirty(const QModelIndex &index)
{
    if (dirtyIndexes.contains(index))
        return;

    dirtyIndexes.emplace_back(index);
}

bool NoteListModel::noteShouldBeDisplayedInCurrentSelectedFolder(const NoteData &note)
{
    if (note.getParentFolderId() == currentSelectedFolderId)
        return true;

    if (currentSelectedFolderId == SpecialFolderId::AllNotesFolder and
        note.getParentFolderId() != SpecialFolderId::TrashFolder)
        return true;

    return false;
}

QStringList NoteListModel::mimeTypes() const
{
    return QStringList(NoteMimeData::type);
}

QMimeData *NoteListModel::mimeData(const QModelIndexList &indexes) const
{
    return NoteMimeData::encodeData(indexes);
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

QVector<int> NoteListModel::getNoteIds(const QModelIndexList &indexes)
{
    QVector<int> noteIds;
    noteIds.reserve(indexes.size());
    for (auto const &index : indexes)
        noteIds.emplaceBack(notes[index.row()].getId());

    return noteIds;
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
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsDragEnabled;
}
