#include "PersistenceManager.h"
#include "SpecialFolderId.h"
#include <QBuffer>
#include <QImage>
#include <QSqlError>
#include <QSqlQuery>
#include <QtGlobal>

PersistenceManager::PersistenceManager() : dbFullFilepath(QCoreApplication::applicationDirPath() + QString("/notes.db"))
{
    bool needCreateNewDefaultTables = false;
    if (!QFile::exists(dbFullFilepath))
        needCreateNewDefaultTables = true;

    db = QSqlDatabase::addDatabase("QSQLITE", QString::number(QRandomGenerator::global()->generate()));

    db.setDatabaseName(dbFullFilepath);

    if (!db.open())
    {
        qDebug() << "Error: connection with database fail";
    }

    if (needCreateNewDefaultTables)
        createNewDefaultTables();
}

int PersistenceManager::addNote(const NoteData &note) const
{
    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO note "
        "(parent_folder_id, title, content, creation_time, modification_time, is_pinned, color)"
        "VALUES(:parent_folder_id, :title, :content, :creation_time, :modification_time, :is_pinned, :color)");
    query.bindValue(":parent_folder_id", note.getParentFolderId());
    query.bindValue(":title", note.getTitle());
    query.bindValue(":content", note.getContent());
    query.bindValue(":creation_time", note.getCreationTime().toMSecsSinceEpoch());
    query.bindValue(":modification_time", note.getModificationTime().toMSecsSinceEpoch());
    query.bindValue(":is_pinned", note.getIsPinned());
    query.bindValue(":color", note.getColor().name());

    if (!query.exec())
    {
        qDebug() << __FUNCTION__ << __LINE__ << query.lastError();
        return -1;
    }
    return getIdOfLastInsertedRow();
}

void PersistenceManager::updateNote(const NoteData &note) const
{
    QSqlQuery query(db);

    query.prepare("UPDATE note "
                  "SET parent_folder_id = :parent_folder_id, title = :title, content = :content, creation_time = "
                  ":creation_time, modification_time = :modification_time , is_pinned = :is_pinned, color = :color "
                  "WHERE id = :id");

    query.bindValue(":parent_folder_id", note.getParentFolderId());
    query.bindValue(":title", note.getTitle());
    query.bindValue(":content", note.getContent());
    query.bindValue(":creation_time", note.getCreationTime().toMSecsSinceEpoch());
    query.bindValue(":modification_time", note.getModificationTime().toMSecsSinceEpoch());
    query.bindValue(":is_pinned", note.getIsPinned());
    query.bindValue(":id", note.getId());
    query.bindValue(":color", note.getColor().name());

    if (!query.exec())
        qDebug() << __FUNCTION__ << __LINE__ << query.lastError();
}

void PersistenceManager::setColorOfNotes(const QVector<int> &noteIds, const QColor &color)
{
    QSqlQuery query(db);
    QString queryString = "UPDATE note "
                          "SET color = :color "
                          "WHERE id IN (";

    foreach (int id, noteIds)
        queryString.append(QString::number(id) + ",");
    queryString.removeLast();
    queryString.append(")");

    query.prepare(queryString);
    query.bindValue(":color", color.name());

    if (!query.exec())
        qDebug() << __FUNCTION__ << __LINE__ << query.lastError();
}

void PersistenceManager::setIsPinnedOfNotes(const QVector<int> &noteIds, bool isPinned)
{
    QSqlQuery query(db);
    QString queryString = "UPDATE note "
                          "SET is_pinned = :is_pinned "
                          "WHERE id IN (";

    foreach (int id, noteIds)
        queryString.append(QString::number(id) + ",");
    queryString.removeLast();
    queryString.append(")");

    query.prepare(queryString);
    query.bindValue(":is_pinned", isPinned);

    if (!query.exec())
        qDebug() << __FUNCTION__ << __LINE__ << query.lastError();
}

NoteData PersistenceManager::loadNote(int id) const
{
    QSqlQuery query(db);
    query.prepare("SELECT * FROM note WHERE id = :id LIMIT 1");
    query.bindValue(":id", id);
    NoteData note;
    if (!query.exec())
    {
        qDebug() << __FUNCTION__ << __LINE__ << query.lastError();
        return note;
    }
    query.next();
    note = createNoteDataFromQueryRecord(query);

    return note;
}

QVector<NoteData> PersistenceManager::loadAllNotes() const
{
    QVector<NoteData> notes;
    QSqlQuery query(db);
    query.prepare("SELECT * FROM note WHERE parent_folder_id != :thrash_folder_id");
    query.bindValue(":thrash_folder_id", SpecialFolderId::TrashFolder);

    if (!query.exec())
    {
        qDebug() << __FUNCTION__ << __LINE__ << query.lastError();
        return notes;
    }

    while (query.next())
    {
        notes.emplace_back(createNoteDataFromQueryRecord(query));
    }

    return notes;
}

QVector<NoteData> PersistenceManager::loadAllNotesFromFolder(int folderId) const
{
    QVector<NoteData> notes;
    QSqlQuery query(db);
    query.prepare("SELECT * FROM note WHERE parent_folder_id = :parent_folder_id");
    query.bindValue(":parent_folder_id", folderId);

    if (!query.exec())
    {
        qDebug() << __FUNCTION__ << __LINE__ << query.lastError();
        return notes;
    }

    while (query.next())
    {
        notes.emplace_back(createNoteDataFromQueryRecord(query));
    }

    return notes;
}

void PersistenceManager::moveNoteToTrash(int noteId) const
{
    QSqlQuery query(db);

    query.prepare("UPDATE note "
                  "SET parent_folder_id = :thrash_folder_id "
                  "WHERE id = :id");

    query.bindValue(":thrash_folder_id", SpecialFolderId::TrashFolder);
    query.bindValue(":id", noteId);

    if (!query.exec())
        qDebug() << __FUNCTION__ << __LINE__ << query.lastError();
}

void PersistenceManager::moveNotesToTrash(const QVector<int> &noteIds)
{
    QSqlQuery query(db);
    QString queryString = "UPDATE note "
                          "SET parent_folder_id = :thrash_folder_id "
                          "WHERE id IN (";

    foreach (int id, noteIds)
        queryString.append(QString::number(id) + ",");
    queryString.removeLast();
    queryString.append(")");

    query.prepare(queryString);
    query.bindValue(":thrash_folder_id", SpecialFolderId::TrashFolder);

    if (!query.exec())
        qDebug() << __FUNCTION__ << __LINE__ << query.lastError();
}

void PersistenceManager::moveAllNotesFromFolderToTrash(int folderId) const
{
    QSqlQuery query(db);

    query.prepare("UPDATE note "
                  "SET parent_folder_id = :thrash_folder_id "
                  "WHERE parent_folder_id = :old_parent_folder_id");

    query.bindValue(":thrash_folder_id", SpecialFolderId::TrashFolder);
    query.bindValue(":old_parent_folder_id", folderId);

    if (!query.exec())
        qDebug() << __FUNCTION__ << __LINE__ << query.lastError();
}

void PersistenceManager::restoreNoteFromTrash(int noteId) const
{
    QSqlQuery query(db);

    query.prepare("UPDATE note "
                  "SET parent_folder_id = :all_notes_folder_id "
                  "WHERE id = :id");

    query.bindValue(":all_notes_folder_id", SpecialFolderId::AllNotesFolder);
    query.bindValue(":id", noteId);

    if (!query.exec())
        qDebug() << __FUNCTION__ << __LINE__ << query.lastError();
}

QVector<NoteData> PersistenceManager::loadAllNotesFromTrash() const
{
    return loadAllNotesFromFolder(SpecialFolderId::TrashFolder);
}

int PersistenceManager::countNotesInTrash()
{
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM note WHERE parent_folder_id = :thrash_folder_id");
    query.bindValue(":thrash_folder_id", SpecialFolderId::TrashFolder);

    if (!query.exec())
    {
        qDebug() << __FUNCTION__ << __LINE__ << query.lastError();
        return 0;
    }

    query.next();
    return query.value(0).toInt();
}

void PersistenceManager::deleteAllNotesFromFolder(int folderId) const
{
    QSqlQuery query(db);
    query.prepare("DELETE FROM note WHERE parent_folder_id = :parent_folder_id");
    query.bindValue(":parent_folder_id", folderId);

    if (!query.exec())
        qDebug() << __FUNCTION__ << __LINE__ << query.lastError();
}

void PersistenceManager::moveNotesToFolder(const QSet<int> &noteIds, int folderId)
{
    QSqlQuery query(db);
    QString queryString = "UPDATE note "
                          "SET parent_folder_id = :folder_id "
                          "WHERE id IN (";

    foreach (int id, noteIds)
        queryString.append(QString::number(id) + ",");
    queryString.removeLast();
    queryString.append(")");

    query.prepare(queryString);
    query.bindValue(":folder_id", folderId);

    if (!query.exec())
        qDebug() << __FUNCTION__ << __LINE__ << query.lastError();
}

std::vector<int> PersistenceManager::getIdsNotes() const
{
    std::vector<int> ids;
    QSqlQuery query(db);
    query.prepare("SELECT id FROM note");
    if (!query.exec())
    {
        qDebug() << __FUNCTION__ << __LINE__ << query.lastError();
        return ids;
    }

    while (query.next())
    {
        ids.emplace_back(query.value(0).toInt());
    }

    return ids;
}

void PersistenceManager::deleteNote(int id) const
{
    QSqlQuery query(db);
    query.prepare("DELETE FROM note WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec())
    {
        qDebug() << __FUNCTION__ << __LINE__ << query.lastError();
        return;
    }

    deleteAllImagesFromNotes({id});
}

void PersistenceManager::deleteNotes(const QVector<int> &noteIds)
{
    QSqlQuery query(db);
    QString queryString = "DELETE FROM note WHERE id IN (";

    foreach (int id, noteIds)
        queryString.append(QString::number(id) + ",");
    queryString.removeLast();
    queryString.append(")");

    query.prepare(queryString);
    if (!query.exec())
    {
        qDebug() << __FUNCTION__ << __LINE__ << query.lastError();
        return;
    }
    deleteAllImagesFromNotes(noteIds);
}

int PersistenceManager::countAllNotes()
{
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM note WHERE parent_folder_id != :thrash_folder_id");
    query.bindValue(":thrash_folder_id", SpecialFolderId::TrashFolder);

    if (!query.exec())
    {
        qDebug() << __FUNCTION__ << __LINE__ << query.lastError();
        return 0;
    }
    query.next();
    return query.value(0).toInt();
}

int PersistenceManager::countNotesInFolder(int folderId)
{
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM note WHERE parent_folder_id = :parent_folder_id");
    query.bindValue(":parent_folder_id", folderId);

    if (!query.exec())
    {
        qDebug() << __FUNCTION__ << __LINE__ << query.lastError();
        return 0;
    }
    query.next();
    return query.value(0).toInt();
}

QVector<FolderData> PersistenceManager::loadAllFolders() const
{
    QVector<FolderData> folders;
    QSqlQuery query(db);
    query.prepare("SELECT * FROM folder");
    if (!query.exec())
    {
        qDebug() << __FUNCTION__ << __LINE__ << query.lastError();
        return folders;
    }

    while (query.next())
    {
        folders.emplace_back();
        folders.back().setId(query.value(0).toInt());
        folders.back().setName(query.value(1).toString());
        folders.back().setParentId(query.value(2).toInt());
        folders.back().setPreviousFolderId(query.value(3).toInt());
        folders.back().setColor(query.value(4).value<QColor>());
    }

    return folders;
}

QVector<int> PersistenceManager::loadIdsOfSubfolders(int idOfFolder)
{
    QVector<int> ids;
    QSqlQuery query(db);
    query.prepare("SELECT id FROM folder WHERE parent_id = :parent_id");
    query.bindValue(":parent_id", idOfFolder);

    if (!query.exec())
    {
        qDebug() << __FUNCTION__ << __LINE__ << query.lastError();
        return ids;
    }

    while (query.next())
    {
        ids.emplace_back(query.value(0).toInt());
    }

    return ids;
}

int PersistenceManager::addFolder(const FolderData &folder) const
{
    QSqlQuery query(db);
    query.prepare("INSERT INTO folder "
                  "(name, parent_id, previous_folder_id, color) "
                  "VALUES(:name, :parent_id, :previous_folder_id, :color)");

    query.bindValue(":name", folder.getName());
    query.bindValue(":parent_id", folder.getParentId());
    query.bindValue(":previous_folder_id", folder.getPreviousFolderId());
    query.bindValue(":color", folder.getColor().name());

    if (!query.exec())
    {
        qDebug() << __FUNCTION__ << __LINE__ << query.lastError();
        return -1;
    }
    return getIdOfLastInsertedRow();
}

void PersistenceManager::updateFolder(const FolderData &folder) const
{
    QSqlQuery query(db);

    query.prepare("UPDATE folder "
                  "SET name = :name, parent_id = :parent_id, previous_folder_id = :previous_folder_id, color = :color "
                  "WHERE id = :id");

    query.bindValue(":name", folder.getName());
    query.bindValue(":parent_id", folder.getParentId());
    query.bindValue(":previous_folder_id", folder.getPreviousFolderId());
    query.bindValue(":color", folder.getColor().name());

    query.bindValue(":id", folder.getId());

    if (!query.exec())
        qDebug() << __FUNCTION__ << __LINE__ << query.lastError();
}

void PersistenceManager::deleteFolder(int id) const
{
    QSqlQuery query(db);
    query.prepare("DELETE FROM folder WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec())
        qDebug() << __FUNCTION__ << __LINE__ << query.lastError();
}

int PersistenceManager::addImage(const QImage &image, int noteID) const
{
    QSqlQuery query(db);
    query.prepare("INSERT INTO image "
                  "(data, note_id) "
                  "VALUES(:data, :note_id)");

    QByteArray imageData;
    QBuffer buffer(&imageData);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG");
    query.bindValue(":data", imageData);
    query.bindValue(":note_id", noteID);

    if (!query.exec())
        qDebug() << __FUNCTION__ << __LINE__ << query.lastError();

    return getIdOfLastInsertedRow();
}

QImage PersistenceManager::loadImage(int id) const
{
    QSqlQuery query(db);
    query.prepare("SELECT data FROM image WHERE id = :id");
    query.bindValue(":id", id);
    if (!query.exec())
    {
        qDebug() << __FUNCTION__ << __LINE__ << query.lastError();
        return QImage();
    }
    query.next();
    QImage image;
    image.loadFromData(query.value(0).toByteArray());
    return image;
}

void PersistenceManager::deleteAllImagesFromNotes(const QVector<int> &noteIds) const
{
    QSqlQuery query(db);
    QString queryString = "DELETE FROM image WHERE note_id IN (";

    foreach (int id, noteIds)
        queryString.append(QString::number(id) + ",");
    queryString.removeLast();
    queryString.append(")");

    query.prepare(queryString);
    if (!query.exec())
        qDebug() << __FUNCTION__ << __LINE__ << query.lastError();
}

QHash<int, int> PersistenceManager::getNotesInsideFoldersCounts()
{
    QHash<int, int> result;
    QSqlQuery query(db);
    query.prepare("SELECT parent_folder_id, COUNT(*) "
                  "FROM note "
                  "GROUP BY parent_folder_id");

    if (!query.exec())
    {
        qDebug() << __FUNCTION__ << __LINE__ << query.lastError();
        return result;
    }

    while (query.next())
    {
        result.insert(query.value(0).toInt(), query.value(1).toInt());
    }

    return result;
}

void PersistenceManager::createNewDefaultTables() const
{
    QSqlQuery query(db);
    QString createNoteTable = "CREATE TABLE note("
                              "id INTEGER PRIMARY KEY, "
                              "parent_folder_id INTEGER, "
                              "title TEXT, "
                              "content TEXT, "
                              "creation_time INTEGER NOT NULL DEFAULT 0, "
                              "modification_time INTEGER NOT NULL DEFAULT 0, "
                              "is_pinned INTEGER NOT NULL DEFAULT 0, "
                              "color TEXT NOT NULL "
                              ")";

    if (!query.exec(createNoteTable))
        qDebug() << __FUNCTION__ << __LINE__ << query.lastError();

    query.finish();
    QString createFolderTable = "CREATE TABLE folder("
                                "id INTEGER PRIMARY KEY, "
                                "name TEXT, "
                                "parent_id INTEGER NOT NULL DEFAULT 0, "
                                "previous_folder_id INTEGER NOT NULL DEFAULT 0, "
                                "color TEXT NOT NULL "
                                ")";

    if (!query.exec(createFolderTable))
        qDebug() << __FUNCTION__ << __LINE__ << query.lastError();

    QString createImageTable = "CREATE TABLE image("
                               "id INTEGER PRIMARY KEY, "
                               "data MEDIUMBLOB, "
                               "note_id INTEGER "
                               ")";

    if (!query.exec(createImageTable))
        qDebug() << __FUNCTION__ << __LINE__ << query.lastError();

    FolderData allNotesFolder;
    allNotesFolder.setName("All notes");
    allNotesFolder.setParentId(SpecialFolderId::RootFolder);
    allNotesFolder.setPreviousFolderId(SpecialFolderId::InvalidId);
    addFolderWithGivenId(allNotesFolder, SpecialFolderId::AllNotesFolder);

    FolderData trashFolder;
    trashFolder.setName("Trash");
    trashFolder.setParentId(SpecialFolderId::RootFolder);
    addFolderWithGivenId(trashFolder, SpecialFolderId::TrashFolder);

    FolderData notesFolder;
    notesFolder.setName("New Folder");
    notesFolder.setParentId(SpecialFolderId::RootFolder);
    notesFolder.setPreviousFolderId(SpecialFolderId::AllNotesFolder);
    int userFolderId = addFolder(notesFolder);

    trashFolder.setId(SpecialFolderId::TrashFolder);
    trashFolder.setPreviousFolderId(userFolderId);
    updateFolder(trashFolder);
}

int PersistenceManager::getIdOfLastInsertedRow() const
{
    QSqlQuery query(db);
    query.prepare("SELECT last_insert_rowid()");

    if (!query.exec())
    {
        qDebug() << __FUNCTION__ << __LINE__ << query.lastError();
        return -1;
    }

    query.next();
    int id = query.value(0).toInt();
    return id;
}

NoteData PersistenceManager::createNoteDataFromQueryRecord(const QSqlQuery &query) const
{
    NoteData noteData;
    noteData.setId(query.value(0).toInt());
    noteData.setParentFolderId(query.value(1).toInt());
    noteData.setTitle(query.value(2).toString());
    noteData.setContent(query.value(3).toString());
    noteData.setCreationTime(QDateTime::fromMSecsSinceEpoch(query.value(4).toLongLong()));
    noteData.setModificationTime(QDateTime::fromMSecsSinceEpoch(query.value(5).toLongLong()));
    noteData.setIsPinned(query.value(6).toBool());
    noteData.setColor(QColor(query.value(7).toString()));

    if (noteData.getParentFolderId() == SpecialFolderId::TrashFolder)
        noteData.setIsInTrash(true);
    else
        noteData.setIsInTrash(false);

    return noteData;
}

void PersistenceManager::addFolderWithGivenId(const FolderData &folder, int id) const
{
    QSqlQuery query(db);
    query.prepare("INSERT INTO folder "
                  "(id, name, parent_id, previous_folder_id, color) "
                  "VALUES(:id, :name, :parent_id, :previous_folder_id, :color)");

    query.bindValue(":id", id);
    query.bindValue(":name", folder.getName());
    query.bindValue(":parent_id", folder.getParentId());
    query.bindValue(":previous_folder_id", folder.getPreviousFolderId());
    query.bindValue(":color", folder.getColor().name());

    if (!query.exec())
    {
        qDebug() << __FUNCTION__ << __LINE__ << query.lastError();
    }
}
