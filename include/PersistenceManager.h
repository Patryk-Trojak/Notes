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
    static void saveNoteToFile(Note &note);
    static void saveAllNotes(const std::vector<std::unique_ptr<Note>> &notes);
    static std::unique_ptr<Note> loadNoteFromFile(int id);
    static void updateCreationAndModificationTime(Note &note);
    static std::unique_ptr<Note> createNewNoteFile();
    static std::vector<std::unique_ptr<Note>> loadAllNotes();
    static std::vector<int> getAllIdsOfSavedNotes();
    static void deleteNoteFile(int id);
    static QString generateUniqueNoteFilename();
    static QString notesDirectoryPathname;

  private:
    static QString createFullPathToNote(int id);
    static std::unique_ptr<Note> loadNoteFromFile(const QString &filename, int idOfNote);
};

#endif // PERSISTENCEMANAGER_H
