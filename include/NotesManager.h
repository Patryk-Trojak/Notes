#ifndef NOTESMANAGER_H
#define NOTESMANAGER_H
#include "Note.h"
#include "PersistenceManager.h"
#include <QObject>
#include <memory>
#include <vector>

class NotesManager : public QObject
{
    Q_OBJECT
  public:
    NotesManager();
    Note &createNewDefaultNote();
    void saveNote(Note &note);
    void deleteNote(Note &note);
    const std::vector<std::unique_ptr<Note>> &getNotes() const;

  private:
    std::vector<std::unique_ptr<Note>> notes;
    PersistenceManager persistenceManager;
    void loadAllNotes();
    int generateUniqueId();
    bool noteWithGivenIdExists(int id);
};

#endif // NOTESMANAGER_H
