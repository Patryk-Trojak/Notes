#ifndef NOTE_H
#define NOTE_H

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
    void setTitle(const QString &title);
    void setContent(const QString &content);
    void setFilename(const QString &filename);

  private:
    QString title;
    QString content;
    QString filename;
};

#endif // NOTE_H
