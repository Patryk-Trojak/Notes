#ifndef NOTELISTMODEL_H
#define NOTELISTMODEL_H

#include "NoteData.h"
#include "NoteListModelRole.h"
#include "NoteMimeData.h"
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
    QStringList mimeTypes() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    QModelIndex createNewNote();
    void moveNotesToFolder(const QSet<int> &noteIds, int folderId);
    void setColorOfNotes(const QModelIndexList &indexes, const QColor &color);
    void removeNotes(QModelIndexList &indexes);
    void restoreNoteFromTrash(const QModelIndex &index);
    int getCurrentSelectedFolderId() const;
    void saveDirtyIndexes();
    QVector<int> getNoteIds(const QModelIndexList &indexes);

  public slots:
    void onNewFolderSelected(int selectedFolderId);
    void onFolderDeleted(int deletedFolderId);

  signals:
    void notesMoved(int sourceFolderId, int destinationFolderId, int notesCount);
    void notesAdded(int parentFolderId, int notesCount);
    void notesRemoved(int parentFolderId, int notesCount);

  private:
    PersistenceManager &persistenceManager;
    QVector<NoteData> notes;
    QVector<QPersistentModelIndex> dirtyIndexes;
    void markIndexAsDirty(const QModelIndex &index);
    int currentSelectedFolderId;
    bool noteShouldBeDisplayedInCurrentSelectedFolder(const NoteData &note);
};

#endif // NOTELISTMODEL_H
