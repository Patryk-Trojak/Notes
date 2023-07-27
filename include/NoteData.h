#ifndef NOTE_H
#define NOTE_H

#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QFlags>
#include <QObject>
#include <QTextStream>
#include <random>

class NoteData
{
  public:
    explicit NoteData();
    int getId() const;
    int getParentFolderId() const;
    const QString &getContent() const;
    const QString &getTitle() const;
    const QDateTime &getCreationTime() const;
    const QDateTime &getModificationTime() const;
    void setId(int id);
    void setParentFolderId(int newParentFolderId);
    void setTitle(const QString &title);
    void setContent(const QString &content);
    void setCreationTime(const QDateTime &newCreationTime);
    void setModificationTime(const QDateTime &newModificationTime);

    bool getIsInTrash() const;
    void setIsInTrash(bool newIsInTrash);

private:
    int id;
    int parentFolderId;
    QString title;
    QString content;
    QDateTime creationTime;
    QDateTime modificationTime;
    bool isInTrash;
};

#endif // NOTE_H
