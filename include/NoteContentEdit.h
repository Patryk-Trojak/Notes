#ifndef NOTECONTENTEDIT_H
#define NOTECONTENTEDIT_H
#include <QTextEdit>

class NoteContentEdit : public QTextEdit
{
    Q_OBJECT

  public:
    NoteContentEdit(QWidget *parent = nullptr);

  protected:
    bool canInsertFromMimeData(const QMimeData *source) const;
    void insertFromMimeData(const QMimeData *source);

  private:
    void insertImage(const QVariant &imageData);
};

#endif // NOTECONTENTEDIT_H
