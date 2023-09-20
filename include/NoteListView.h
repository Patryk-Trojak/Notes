#ifndef NOTELISTVIEW_H
#define NOTELISTVIEW_H

#include "NoteListModel.h"
#include "NoteListViewSelectionMenu.h"
#include <NoteButton.h>
#include <NoteListDelegate.h>
#include <QListView>

class NoteListView : public QListView
{
    Q_OBJECT

  public:
    NoteListView(QWidget *parent);

    int getHowManyNotesCanFitInRow(int width) const;
    int getHowManyNotesAreDisplayed() const;
    void setMinWidthToFitNotesInRow(int numberOfNotesToFitInOneRow);
    bool event(QEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);
    void setModel(QAbstractItemModel *model);
    QPoint getOffsetOfViewport() const;
    void startDragSelecting(QPoint startPoint);
    void updateMousePositionOfDragSelecting(QPoint mousePosition);
    void endDragSelecting();
    void selectAll();

  signals:
    void noteSelected(const QModelIndex &index);
    void verticalScrollBarWidthChanged();

  protected:
    void resizeEvent(QResizeEvent *event);
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    bool viewportEvent(QEvent *event);
    void verticalScrollbarValueChanged(int value);
    void startDrag(Qt::DropActions supportedActions);

  private:
    NoteListDelegate noteListDelegate;
    NoteButton *editor;
    QPersistentModelIndex currentIndexWithEditor;
    NoteListViewSelectionMenu *selectionMenu;
    QPoint dragSelectingInitialPoint;
    bool wasCtrlPressedWhileStartingDragSelecting;
    bool isDragSelecting;
    bool inSelectingState;

    QPoint pressedPosition;
    QModelIndex pressedIndex;

    void updateEditor();
    void removeNote(const QModelIndex &index);
    void removeSelectedNotes();
    void changeColorOfSelectedNotes(const QColor &newColor);
    void toogleIsPinnedOfSelectedNotes();
    bool getInSelectingState() const;
    void setInSelectingState(bool newInSelectingState);
    void selectNotes(const QRect &rubberBand);
    NoteListModel *getSourceModelAtTheBottom() const;
    QModelIndex mapIndexToSourceModelAtTheBott(const QModelIndex &index) const;
    QModelIndexList mapIndexesToSourceModelAtTheBottom(const QModelIndexList &indexes);
    bool isTrashFolderLoaded();
    QPixmap drawDragPixmap(const QModelIndexList &indexes);
    QVector<QColor> getNMostFrequentNotesColors(const QModelIndexList &indexes, int N);
    QMenu *createContextMenuForSelectingState();
    QMenu *createContextMenuForNormalState(const QPoint &position);
  private slots:
    void onNewEditorCreated(NoteButton *editor, const QModelIndex &index);
    void onCustomContextMenuRequested(const QPoint &pos);
    void onRestoreNoteFromTrashRequested(const QModelIndex &index);
};

#endif // NOTELISTVIEW_H
