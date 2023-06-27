#ifndef PERSISTENCEMANAGER_H
#define PERSISTENCEMANAGER_H

#include "Note.h"
#include <QCoreApplication>
#include <QDir>
#include <QRandomGenerator>
#include <QString>
#include <random>

class PersistenceManager
{
  public:
    PersistenceManager();
    void saveNoteToFile(Note &note);
    std::unique_ptr<Note> loadNoteFromFile(int id);
    void updateCreationAndModificationTime(Note &note);
    std::vector<int> getAllIdsOfSavedNotes();
    void deleteNoteFile(int id);

  private:
    QString notesDirectoryPathname;
    QString createFullPathToNote(int id);
    std::unique_ptr<Note> loadNoteFromFile(const QString &filename, int idOfNote);
};

#endif // PERSISTENCEMANAGER_H
