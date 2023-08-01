#ifndef NOTEBUTTON_H
#define NOTEBUTTON_H
#include <QCheckBox>
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
    explicit NoteButton(const QString &title, const QDateTime &modificationTime, bool isPinned,
                        QWidget *parent = nullptr);

    void setTitle(const QString &title);
    void setContent(const QString &content);
    void setModificationTime(const QDateTime &newModificationTime);
    void setIsPinned(bool newIsPinned);
    QString getTitle() const;
    QString getContent() const;
    const QDateTime &getModificationTime() const;
    bool getIsPinned() const;
    void setPinCheckboxVisible(bool visible);

  signals:
    void deleteNote();
    void pinCheckboxToogled(bool checked);

  private:
    Ui::NoteButton *ui;
    QPushButton *deleteButton;
    QCheckBox *pinCheckBox;
    QDateTime modificationTime;
    QString convertDateTimeToString(const QDateTime &dateTime);

  protected:
    void resizeEvent(QResizeEvent *event);
};
#endif // NOTEBUTTON_H
