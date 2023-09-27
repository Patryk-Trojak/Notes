#ifndef NOTEEDITOR_H
#define NOTEEDITOR_H

#include "NoteContentEdit.h"
#include <QModelIndex>
#include <QPushButton>
#include <QTextCharFormat>
#include <QTextDocument>
#include <QToolBar>

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
    bool eventFilter(QObject *watched, QEvent *event);

  signals:
    void closeNoteRequested();
    void titleChanged(const QString &newTitle);
    void contentChanged(const QString &newContent);

  private:
    Ui::NoteEditor *ui;
    QPushButton *closeButton;

    void setCreationTime(const QDateTime &creationTime);
    void setModificationTime(const QDateTime &modificationTime);
    void switchTextBold();
    void switchTextItalic();
    void switchTextUnderline();
    void switchTextStrikeOut();
    void setTextColor();
    void setTextBackgroundColor();
    void alignParagraph(Qt::Alignment alignment);
    void openAlignPopupMenu();
    void onAlignmentChanged();
    void modifyIndentation(int amount);
    void switchListStyle(QTextListFormat::Style listFormat, QTextBlockFormat::MarkerType);
    void onListStyleChanged();
    void setFontSize();
    void setFontFamily(const QString &fontFamily);
    void onCurrentBlockFormatChanged();
    void onCurrentCharFormatChanged(const QTextCharFormat &f);
    QWidget *fakeTextCursor;

  protected:
    void resizeEvent(QResizeEvent *event);
};

#endif // NOTEEDITOR_H
