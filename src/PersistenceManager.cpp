#include "PersistenceManager.h"

QString PersistenceManager::notesDirectoryPathname;

void PersistenceManager::saveNoteToFile(Note &note)
{
    QFile file(createFullPathToNote(note.getId()));
    file.open(QFile::WriteOnly);
    QTextStream textStream(&file);
    textStream << note.getTitle() << '\n' << note.getContent();
    updateCreationAndModificationTime(note);
    file.close();
}

std::unique_ptr<Note> PersistenceManager::loadNoteFromFile(int id)
{
    QFile file(createFullPathToNote(id));
    file.open(QFile::ReadOnly);
    QTextStream textStream(&file);
    std::unique_ptr<Note> note = std::make_unique<Note>(id);
    note->setTitle(textStream.readLine());
    note->setContent(textStream.readAll());
    file.close();
    updateCreationAndModificationTime(*note);
    return note;
}

void PersistenceManager::updateCreationAndModificationTime(Note &note)
{
    QFileInfo noteFile(createFullPathToNote(note.getId()));
    note.setCreationTime(noteFile.birthTime());
    note.setModificationTime(noteFile.lastModified());
}

std::vector<int> PersistenceManager::getAllIdsOfSavedNotes()
{
    std::vector<int> ids;
    QDir direcory(notesDirectoryPathname);
    if (!direcory.exists())
    {
        direcory.mkpath(".");
        return ids;
    }

    QStringList listOfNotes = direcory.entryList(QDir::Filter::Files);
    for (auto const &noteFilename : listOfNotes)
    {
        int id = noteFilename.first(noteFilename.size() - 4).toInt();
        ids.emplace_back(id);
    }
    return ids;
}

QString PersistenceManager::createFullPathToNote(int id)
{
    return notesDirectoryPathname + QString::number(id) + QString(".txt");
}

std::unique_ptr<Note> PersistenceManager::loadNoteFromFile(const QString &filename, int idOfNote)
{
    QFile file(filename);
    file.open(QFile::ReadOnly);
    QTextStream textStream(&file);
    std::unique_ptr<Note> note = std::make_unique<Note>(idOfNote);
    note->setTitle(textStream.readLine());
    note->setContent(textStream.readAll());
    file.close();
    updateCreationAndModificationTime(*note);
    return note;
}

void PersistenceManager::deleteNoteFile(int id)
{
    QFile file(createFullPathToNote(id));
    file.remove();
}
