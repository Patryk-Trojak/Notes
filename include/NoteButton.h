#ifndef NOTEBUTTON_H
#define NOTEBUTTON_H
#include <QTextEdit>
#include <QWidget>
#include <QtWidgets/QPushButton>

namespace Ui
{
class NoteButton;
}

class NoteButton : public QWidget
{
    Q_OBJECT
  public:
    explicit NoteButton(const QString &title, QWidget *parent = nullptr);
    QString getTitle() const;
    void setTitle(const QString &title);
  signals:
    void saveNote();
    void enterEditingNote();
    void deleteNote();

  private:
    Ui::NoteButton *ui;
    QPushButton *button;
};
#endif // NOTEBUTTON_H
