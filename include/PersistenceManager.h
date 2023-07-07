#ifndef PERSISTENCEMANAGER_H
#define PERSISTENCEMANAGER_H

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
    std::vector<int> getAllIdsOfSavedNotes();
    void deleteNoteFile(int id);

  private:
    QString dbFullFilepath;
    QSqlDatabase db;

    void createNewDefaultTables();
    int getIdOfLastInsertedNote();
};

#endif // PERSISTENCEMANAGER_H
