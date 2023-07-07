#include "PersistenceManager.h"

#include <QSqlError>
#include <QSqlQuery>

PersistenceManager::PersistenceManager() : dbFullFilepath(QCoreApplication::applicationDirPath() + QString("/notes.db"))
{
    bool needCreateNewDefaultTables = false;
    if (!QFile::exists(dbFullFilepath))
        needCreateNewDefaultTables = true;

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbFullFilepath);

    if (!db.open())
    {
        qDebug() << "Error: connection with database fail";
    }

    if (needCreateNewDefaultTables)
        createNewDefaultTables();
}

int PersistenceManager::addNote(const NoteData &note)
{
    QSqlQuery query(db);
    query.prepare("INSERT INTO note "
                  "(title, content, creation_time, modification_time)"
                  "VALUES(:title, :content, :creation_time, :modification_time)");
    query.bindValue(":title", note.getTitle());
    query.bindValue(":content", note.getContent());
    query.bindValue(":creation_time", note.getCreationTime().toMSecsSinceEpoch());
    query.bindValue(":modification_time", note.getModificationTime().toMSecsSinceEpoch());

    if (!query.exec())
    {
        qDebug() << __FUNCTION__ << __LINE__ << query.lastError();
        return -1;
    }
    return getIdOfLastInsertedNote();
}

void PersistenceManager::updateNote(const NoteData &note)
{
    QSqlQuery query(db);

    query.prepare("UPDATE note "
                  "SET title = :title, content = :content, creation_time = :creation_time, modification_time = "
                  ":modification_time "
                  "WHERE id = :id");

    query.bindValue(":title", note.getTitle());
    query.bindValue(":content", note.getContent());
    query.bindValue(":creation_time", note.getCreationTime().toMSecsSinceEpoch());
    query.bindValue(":modification_time", note.getModificationTime().toMSecsSinceEpoch());
    query.bindValue(":id", note.getId());

    if (!query.exec())
        qDebug() << __FUNCTION__ << __LINE__ << query.lastError();
}

NoteData PersistenceManager::loadNoteFromFile(int id)
{
    QSqlQuery query(db);
    query.prepare("SELECT * FROM note WHERE id = :id LIMIT 1");
    query.bindValue(":id", id);
    NoteData note(-1);
    if (!query.exec())
    {
        qDebug() << __FUNCTION__ << __LINE__ << query.lastError();
        return note;
    }
    query.next();
    note.setId(query.value(0).toInt());
    note.setTitle(query.value(1).toString());
    note.setContent(query.value(2).toString());
    note.setCreationTime(QDateTime::fromMSecsSinceEpoch(query.value(3).toLongLong()));
    note.setModificationTime(QDateTime::fromMSecsSinceEpoch(query.value(4).toLongLong()));

    return note;
}

std::vector<int> PersistenceManager::getAllIdsOfSavedNotes()
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

void PersistenceManager::deleteNoteFile(int id)
{
    QSqlQuery query(db);
    query.prepare("DELETE FROM note WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec())
        qDebug() << __FUNCTION__ << __LINE__ << query.lastError();
}

void PersistenceManager::createNewDefaultTables()
{
    QSqlQuery query(db);
    QString createNoteTable = "CREATE TABLE note("
                              "id INTEGER PRIMARY KEY, "
                              "title TEXT, "
                              "content TEXT, "
                              "creation_time INTEGER NOT NULL DEFAULT 0, "
                              "modification_time INTEGER NOT NULL DEFAULT 0"
                              ")";

    if (!query.exec(createNoteTable))
        qDebug() << __FUNCTION__ << __LINE__ << query.lastError();
}

int PersistenceManager::getIdOfLastInsertedNote()
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
