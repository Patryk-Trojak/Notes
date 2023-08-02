#ifndef NOTE_H
#define NOTE_H

#include <QColor>
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
    bool getIsInTrash() const;
    bool getIsPinned() const;
    const QColor &getColor() const;

    void setId(int id);
    void setParentFolderId(int newParentFolderId);
    void setTitle(const QString &title);
    void setContent(const QString &content);
    void setCreationTime(const QDateTime &newCreationTime);
    void setModificationTime(const QDateTime &newModificationTime);
    void setIsInTrash(bool newIsInTrash);
    void setIsPinned(bool newIsPinned);
    void setColor(const QColor &newColor);

  private:
    int id;
    int parentFolderId;
    QString title;
    QString content;
    QDateTime creationTime;
    QDateTime modificationTime;
    bool isInTrash;
    bool isPinned;
    QColor color;
};

#endif // NOTE_H
