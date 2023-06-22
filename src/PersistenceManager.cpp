#include "PersistenceManager.h"

QString PersistenceManager::notesDirectoryPathname;

void PersistenceManager::saveNoteToFile(const Note &note)
{
    QFile file(notesDirectoryPathname + note.getFilename());
    file.open(QFile::WriteOnly);
    QTextStream textStream(&file);
    textStream << note.getTitle() << '\n' << note.getContent();
    file.close();
}

std::unique_ptr<Note> PersistenceManager::loadNoteFromFile(const QString &filename)
{
    QFile file(notesDirectoryPathname + filename);
    file.open(QFile::ReadOnly);
    QTextStream textStream(&file);
    std::unique_ptr<Note> note = std::make_unique<Note>(filename);
    note->setTitle(textStream.readLine());
    note->setContent(textStream.readAll());
    file.close();
    return note;
}

std::unique_ptr<Note> PersistenceManager::createNewNoteFile()
{
    QString uniqueFilename = PersistenceManager::generateUniqueNoteFilename();
    std::unique_ptr<Note> note = std::make_unique<Note>(uniqueFilename);
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
        notes.emplace_back(loadNoteFromFile(noteFilename));
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
        for (int i = 0; i < 10; i++)
        {
            std::uniform_int_distribution dist(97, 122);
            filename.append(QChar(dist(*QRandomGenerator::global())));
        }
        filename.append(".txt");
    } while (QFile::exists(filename));

    return filename;
}

void PersistenceManager::deleteNoteFile(const QString &filename)
{
    QFile file(notesDirectoryPathname + filename);
    file.remove();
}
