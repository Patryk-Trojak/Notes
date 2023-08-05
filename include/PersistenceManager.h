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

enum SpecialFolderId
{
    RootFolder = -1,
    AllNotesFolder = -2,
    TrashFolder = -3,
    InvalidId = -10
};

class PersistenceManager
{
  public:
    PersistenceManager();
    int addNote(const NoteData &note) const;
    void updateNote(const NoteData &note) const;
    NoteData loadNote(int id) const;
    QVector<NoteData> loadAllNotes() const;
    QVector<NoteData> loadAllNotesFromFolder(int folderId) const;
    void deleteAllNotesFromFolder(int folderId) const;
    std::vector<int> getIdsNotes() const;
    void deleteNote(int id) const;
    int countAllNotes();

    void moveNoteToTrash(int noteId) const;
    void moveAllNotesFromFolderToTrash(int folderId) const;
    void restoreNoteFromTrash(int noteId) const;
    QVector<NoteData> loadAllNotesFromTrash() const;
    int countNotesInTrash();

    QVector<FolderData> loadAllFolders() const;
    QVector<int> loadIdsOfSubfolders(int idOfFolder);
    int addFolder(const FolderData &folder) const;
    void updateFolder(const FolderData &folder) const;
    void deleteFolder(int id) const;

  private:
    QSqlDatabase db;
    QString dbFullFilepath;

    void createNewDefaultTables() const;
    int getIdOfLastInsertedRow() const;
    NoteData createNoteDataFromQueryRecord(const QSqlQuery &query) const;
};

#endif // PERSISTENCEMANAGER_H
