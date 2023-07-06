#include "NotesManager.h"
#include <limits>
#include <random>

NotesManager::NotesManager()
{
    loadAllNotes();
}

NoteData &NotesManager::createNewDefaultNote()
{
    int id = generateUniqueId();
    std::unique_ptr<NoteData> note = std::make_unique<NoteData>(id);
    note->setTitle("Untitled");
    persistenceManager.saveNoteToFile(*note);
    notes.emplace_back(std::move(note));
    return *notes.back().get();
}

void NotesManager::saveNote(NoteData &note)
{
    persistenceManager.saveNoteToFile(note);
}

void NotesManager::deleteNote(NoteData &note)
{
    persistenceManager.deleteNoteFile(note.getId());
    notes.erase(std::remove_if(notes.begin(), notes.end(),
                               [note](const std::unique_ptr<NoteData> &notePtr) { return notePtr.get() == &note; }),
                notes.end());
}

const std::vector<std::unique_ptr<NoteData>> &NotesManager::getNotes() const
{
    return notes;
}

void NotesManager::loadAllNotes()
{
    auto ids = persistenceManager.getAllIdsOfSavedNotes();
    notes.reserve(ids.size());
    for (auto const &id : ids)
    {
        notes.emplace_back(persistenceManager.loadNoteFromFile(id));
    }
}

int NotesManager::generateUniqueId()
{
    int id = 0;
    do
    {
        std::uniform_int_distribution dist(1, std::numeric_limits<int>::max());
        id = dist(*QRandomGenerator::global());
    } while (noteWithGivenIdExists(id));

    return id;
}

bool NotesManager::noteWithGivenIdExists(int id)
{
    auto found = std::find_if(notes.begin(), notes.end(),
                              [id](const std::unique_ptr<NoteData> &note) { return note->getId() == id; });
    if (found == notes.end())
        return false;

    return true;
}
