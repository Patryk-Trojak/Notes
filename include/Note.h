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
    explicit Note(const QString &filename = "");
    const QString &getContent() const;
    const QString &getTitle() const;
    const QString &getFilename() const;
    const QDateTime &getCreationTime() const;
    const QDateTime &getModificationTime() const;
    void setTitle(const QString &title);
    void setContent(const QString &content);
    void setFilename(const QString &filename);
    void setCreationTime(const QDateTime &newCreationTime);
    void setModificationTime(const QDateTime &newModificationTime);

  private:
    QString title;
    QString content;
    QString filename;
    QDateTime creationTime;
    QDateTime modificationTime;
};

#endif // NOTE_H
