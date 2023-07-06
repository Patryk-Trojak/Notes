#ifndef PERSISTENCEMANAGER_H
#define PERSISTENCEMANAGER_H

#include "NoteData.h"
#include <QCoreApplication>
#include <QDir>
#include <QRandomGenerator>
#include <QString>
#include <random>

class PersistenceManager
{
  public:
    PersistenceManager();
    void saveNoteToFile(NoteData &note);
    std::unique_ptr<NoteData> loadNoteFromFile(int id);
    void updateCreationAndModificationTime(NoteData &note);
    std::vector<int> getAllIdsOfSavedNotes();
    void deleteNoteFile(int id);

  private:
    QString notesDirectoryPathname;
    QString createFullPathToNote(int id);
    std::unique_ptr<NoteData> loadNoteFromFile(const QString &filename, int idOfNote);
};

#endif // PERSISTENCEMANAGER_H
