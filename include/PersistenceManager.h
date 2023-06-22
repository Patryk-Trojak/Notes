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
    static void saveNoteToFile(const Note &note);
    static void saveAllNotes(const std::vector<std::unique_ptr<Note>> &notes);
    static std::unique_ptr<Note> loadNoteFromFile(const QString &filename);
    static std::unique_ptr<Note> createNewNoteFile();
    static std::vector<std::unique_ptr<Note>> loadAllNotes();
    static void deleteNoteFile(const QString &filename);
    static QString generateUniqueNoteFilename();
    static QString notesDirectoryPathname;
};

#endif // PERSISTENCEMANAGER_H
