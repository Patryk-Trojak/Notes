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

std::unique_ptr<Note> PersistenceManager::createNewNoteFile()
{
    QString uniqueFilename = PersistenceManager::generateUniqueNoteFilename();
    std::unique_ptr<Note> note = std::make_unique<Note>(uniqueFilename.left(9).toInt());
    note->setTitle("Untitled");
    PersistenceManager::saveNoteToFile(*note);
    return note;
}

std::vector<std::unique_ptr<Note>> PersistenceManager::loadAllNotes()
{
    std::vector<std::unique_ptr<Note>> notes;
    QDir direcory(notesDirectoryPathname);
    if (!direcory.exists())
    {
        direcory.mkpath(".");
        return notes;
    }
    QStringList listOfNotes = direcory.entryList(QDir::Filter::Files);
    for (auto const &noteFilename : listOfNotes)
    {
        notes.emplace_back(loadNoteFromFile(noteFilename.toInt()));
    }

    return notes;
}

void PersistenceManager::saveAllNotes(const std::vector<std::unique_ptr<Note>> &notes)
{
    for (auto const &note : notes)
    {
        saveNoteToFile(*note);
    }
}

QString PersistenceManager::generateUniqueNoteFilename()
{
    QString filename = QString("");
    do
    {
        for (int i = 0; i < 9; i++)
        {
            std::uniform_int_distribution dist(48, 57);
            filename.append(QChar(dist(*QRandomGenerator::global())));
        }
        filename.append(".txt");
    } while (QFile::exists(filename));
    return filename;
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
    QFile file(notesDirectoryPathname + QString::number(id));
    file.remove();
}
