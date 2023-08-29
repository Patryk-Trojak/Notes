#ifndef NOTELISTMODEL_H
#define NOTELISTMODEL_H

#include "NoteData.h"
#include "NoteListModelRole.h"
#include "PersistenceManager.h"
#include <QAbstractItemModel>

class NoteListModel : public QAbstractItemModel
{
    Q_OBJECT

  public:
    NoteListModel(QObject *parent, PersistenceManager &persistenceManager);

    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    const NoteData &getNoteData(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role) const;
    void setNoteData(const QModelIndex &index, const NoteData &noteData);
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    bool insertRows(int row, int count, const QModelIndex &parent);
    bool removeRows(int row, int count, const QModelIndex &parent);
    QModelIndex parent(const QModelIndex &child) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QModelIndex createNewNote();
    void restoreNoteFromTrash(const QModelIndex &index);
    int getCurrentSelectedFolderId() const;
    void saveDirtyIndexes();

  public slots:
    void onNewFolderSelected(int selectedFolderId = 5);
    void onFolderDeleted(int deletedFolderId);

  signals:
    void notesRemovedFromFolder(int folderId, int notesRemovedCount = 1);
    void notesAddedToFolder(int folderId, int notesAddedCount);

  private:
    PersistenceManager &persistenceManager;
    QVector<NoteData> notes;
    QVector<QPersistentModelIndex> dirtyIndexes;
    void markIndexAsDirty(const QModelIndex &index);
    int currentSelectedFolderId;
};

#endif // NOTELISTMODEL_H
