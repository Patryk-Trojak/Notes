#ifndef NOTE_H
#define NOTE_H

#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QFlags>
#include <QObject>
#include <QTextStream>
#include <random>

class Note
{
  public:
    explicit Note(int id);
    int getId() const;
    const QString &getContent() const;
    const QString &getTitle() const;
    const QDateTime &getCreationTime() const;
    const QDateTime &getModificationTime() const;
    void setId(int id);
    void setTitle(const QString &title);
    void setContent(const QString &content);
    void setCreationTime(const QDateTime &newCreationTime);
    void setModificationTime(const QDateTime &newModificationTime);

  private:
    int id;
    QString title;
    QString content;
    QDateTime creationTime;
    QDateTime modificationTime;
};

#endif // NOTE_H
