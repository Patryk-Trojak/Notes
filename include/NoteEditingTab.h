#ifndef NOTEEDITINGTAB_H
#define NOTEEDITINGTAB_H

#include "NoteEditor.h"
#include "NoteListModel.h"
#include <QPropertyAnimation>
#include <QWidget>

class NoteEditingTab : public QWidget
{
    Q_OBJECT
  public:
    explicit NoteEditingTab(PersistenceManager &persistenceManager, NoteListModel &noteModel,
                            const QModelIndex &editingNote, QWidget *parent = nullptr);

  signals:
    void exitEditingNoteRequested();

  private:
    NoteListModel &noteModel;
    PersistenceManager &persistenceManager;
    const QModelIndex editingNote;
    NoteEditor *editor;
    QPropertyAnimation *openingEditorAnimation;
    QRect calculateGeometryOfEditor();
    void saveNoteAndEmitExitSignal();

  private slots:
    void onTitleChanged(const QString &newTitle);
    void onContentChanged(const QString &newContent);

  protected:
    void resizeEvent(QResizeEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif // NOTEEDITINGTAB_H
