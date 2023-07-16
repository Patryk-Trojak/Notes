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
    int addNote(const NoteData &note);
    void updateNote(const NoteData &note);
    NoteData loadNoteFromFile(int id);
    QVector<NoteData> loadAllNotes();
    QVector<NoteData> loadAllNotesFromFolder(int folderId);
    std::vector<int> getAllIdsOfSavedNotes();
    void deleteNoteFile(int id);
    QVector<FolderData> loadAllFolders();
    int addFolder(const FolderData &folder);
    void updateFolder(const FolderData &folder);
    void deleteFolder(int id);

  private:
    QSqlDatabase db;
    QString dbFullFilepath;

    void createNewDefaultTables();
    int getIdOfLastInsertedRow();
    NoteData createNoteDataFromQueryRecord(const QSqlQuery &query);
};

#endif // PERSISTENCEMANAGER_H
