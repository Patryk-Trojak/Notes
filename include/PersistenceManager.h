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
    NoteData loadNoteFromFile(int id) const;
    QVector<NoteData> loadAllNotes() const;
    QVector<NoteData> loadAllNotesFromFolder(int folderId) const;
    void deleteAllNotesFromFolder(int folderId) const;
    std::vector<int> getAllIdsOfSavedNotes() const;
    void deleteNoteFile(int id) const;
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
