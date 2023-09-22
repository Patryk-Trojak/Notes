#ifndef NOTEEDITOR_H
#define NOTEEDITOR_H

#include "NoteContentEdit.h"
#include <QModelIndex>
#include <QPushButton>
#include <QTextDocument>
#include <QWidget>

namespace Ui
{
class NoteEditor;
}

class NoteEditor : public QWidget
{
    Q_OBJECT
  public:
    NoteEditor(const QModelIndex &editingNote, QWidget *parent = nullptr);
    ~NoteEditor();
    void setResourceLoader(const NoteContentEdit::ResourceLoader &resourceProvider);
    void setResourceSaver(const NoteContentEdit::ResourceSaver &resourceSaver);

  signals:
    void closeNoteRequested();
    void titleChanged(const QString &newTitle);
    void contentChanged(const QString &newContent);

  private:
    Ui::NoteEditor *ui;
    QPushButton *closeButton;

    void setCreationTime(const QDateTime &creationTime);
    void setModificationTime(const QDateTime &modificationTime);

  protected:
    void resizeEvent(QResizeEvent *event);
};

#endif // NOTEEDITOR_H
