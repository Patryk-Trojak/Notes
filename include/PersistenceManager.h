#ifndef PERSISTENCEMANAGER_H
#define PERSISTENCEMANAGER_H

#include "FolderData.h"
#include "NoteData.h"
#include <QCoreApplication>
#include <QDir>
#include <QRandomGenerator>
#include <QString>
#include <QtSql/QSqlDatabase>
#include <random>

class PersistenceManager
{
  public:
    PersistenceManager();
    int addNote(const NoteData &note) const;
    void updateNote(const NoteData &note) const;
    void setColorOfNotes(const QVector<int> &noteIds, const QColor &color);
    void setIsPinnedOfNotes(const QVector<int> &noteIds, const bool isPinned);
    NoteData loadNote(int id) const;
    QVector<NoteData> loadAllNotes() const;
    QVector<NoteData> loadAllNotesFromFolder(int folderId) const;
    void deleteAllNotesFromFolder(int folderId) const;
    void deleteNote(int id) const;
    void deleteNotes(const QVector<int> &noteIds);
    void moveNotesToFolder(const QSet<int> &noteIds, int folderId);
    std::vector<int> getIdsNotes() const;
    int countAllNotes();
    int countNotesInFolder(int folderId);
    QHash<int, int> getNotesInsideFoldersCounts();

    void moveNoteToTrash(int noteId) const;
    void moveNotesToTrash(const QVector<int> &noteIds);
    void moveAllNotesFromFolderToTrash(int folderId) const;
    void restoreNoteFromTrash(int noteId) const;
    QVector<NoteData> loadAllNotesFromTrash() const;
    int countNotesInTrash();

    QVector<FolderData> loadAllFolders() const;
    QVector<int> loadIdsOfSubfolders(int idOfFolder);
    int addFolder(const FolderData &folder) const;
    void updateFolder(const FolderData &folder) const;
    void deleteFolder(int id) const;

    int addImage(const QImage &image, int noteId) const;
    QImage loadImage(int id) const;
    void deleteAllImagesFromNotes(const QVector<int> &noteIds) const;

  private:
    QSqlDatabase db;
    QString dbFullFilepath;

    void createNewDefaultTables() const;
    int getIdOfLastInsertedRow() const;
    void addFolderWithGivenId(const FolderData &folder, int id) const;
    NoteData createNoteDataFromQueryRecord(const QSqlQuery &query) const;
};

#endif // PERSISTENCEMANAGER_H
