#ifndef NOTESMANAGER_H
#define NOTESMANAGER_H
#include "NoteData.h"
#include "PersistenceManager.h"
#include <QObject>
#include <memory>
#include <vector>

class NotesManager : public QObject
{
    Q_OBJECT
  public:
    NotesManager();
    NoteData &createNewDefaultNote();
    void saveNote(NoteData &note);
    void deleteNote(NoteData &note);
    const std::vector<std::unique_ptr<NoteData>> &getNotes() const;

  private:
    std::vector<std::unique_ptr<NoteData>> notes;
    PersistenceManager persistenceManager;
    void loadAllNotes();
    int generateUniqueId();
    bool noteWithGivenIdExists(int id);
};

#endif // NOTESMANAGER_H
