#ifndef NOTEBUTTON_H
#define NOTEBUTTON_H
#include <QDateTime>
#include <QTextEdit>
#include <QWidget>
#include <QtWidgets/QPushButton>

namespace Ui
{
class NoteButton;
}

class NoteButton : public QPushButton
{
    Q_OBJECT
  public:
    explicit NoteButton(QWidget *parent);
    explicit NoteButton(const QString &title, const QDateTime &creationTime, const QDateTime &modificationTime,
                        QWidget *parent = nullptr);
    QString getTitle() const;
    void setTitle(const QString &title);
    void setCreationTime(const QDateTime &newCreationTime);
    void setModificationTime(const QDateTime &newModificationTime);
    const QDateTime &getCreationTime() const;
    const QDateTime &getModificationTime() const;
  signals:
    void saveNote();
    void enterEditingNote();
    void deleteNote();

  private:
    Ui::NoteButton *ui;
    QDateTime creationTime;
    QDateTime modificationTime;
    QString convertDateTimeToString(const QDateTime &dateTime);
    QString lastSavedTitle;
};
#endif // NOTEBUTTON_H
