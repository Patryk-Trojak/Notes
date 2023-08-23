#ifndef NOTESDISPLAYINGTAB_H
#define NOTESDISPLAYINGTAB_H

#include "FolderTreeModel.h"
#include "NoteListModel.h"
#include "NoteListView.h"
#include "NoteSortFilterProxyModel.h"
#include "NoteSortOptionsWidget.h"
#include "SearchBar.h"
#include <QSortFilterProxyModel>
#include <QWidget>

namespace Ui
{
class NotesDisplayingTab;
}

class NotesDisplayingTab : public QWidget
{
    Q_OBJECT

  public:
    explicit NotesDisplayingTab(NoteListModel &noteModel, PersistenceManager &persistenceManager,
                                QWidget *parent = nullptr);
    ~NotesDisplayingTab();

  signals:
    void enterEditingNote(const QModelIndex &note);

  private slots:
    void onNewNoteButtonPressed();
    void onNewNoteSortRoleSelected(int newSortRole);
    void onNewNoteSortOrderSelected(Qt::SortOrder newSortOrder);
    void onNewFolderSelected(int selectedFolderId);
    void onOpenNoteSortOptionsButtonClicked();

  private:
    Ui::NotesDisplayingTab *ui;
    NoteListView *noteListView;
    SearchBar *searchBar;
    QPushButton *openNoteSortOptionsButton;
    NoteListModel &noteModel;
    QRubberBand *rubberBand;
    QPoint originOfRubberBandInNoteListView;
    bool wasRubberBandMoved;
    void updateRubberBand();
    NoteSortFilterProxyModel noteProxyModel;
    FolderTreeModel folderModel;

    void layoutNoteListView();
    void layoutSearchBar();
    void layoutOpenNoteSortOptionsButton();
    void layoutAllElementsWhichDependsOnNumberOfNotes();

  protected:
    void resizeEvent(QResizeEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
};

#endif // NOTESDISPLAYINGTAB_H
