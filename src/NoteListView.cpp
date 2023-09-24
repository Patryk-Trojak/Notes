#include "NoteListView.h"
#include "NoteListModel.h"
#include "SpecialFolderId.h"
#include <QAbstractProxyModel>
#include <QApplication>
#include <QDrag>
#include <QEvent>
#include <QHBoxLayout>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QScrollBar>
#include <qmessagebox.h>

NoteListView::NoteListView(QWidget *parent)
    : QListView(parent), editor(nullptr), noteListDelegate(this), inSelectingState(false), isDragSelecting(false),
      wasCtrlPressedWhileStartingDragSelecting(false)
{
    setSelectionMode(QAbstractItemView::NoSelection);   // there is implemented custom selection
    setEditTriggers(QAbstractItemView::NoEditTriggers); // there is implemented custom editing
    setGridSize(QSize(220, 220));
    setResizeMode(ResizeMode::Adjust);
    setViewMode(ViewMode::IconMode);
    setDragDropMode(DragDropMode::DragOnly);
    setVerticalScrollMode(ScrollMode::ScrollPerPixel);
    setUniformItemSizes(true);
    setMouseTracking(true);
    setAutoScroll(false);
    setItemDelegate(&noteListDelegate);
    verticalScrollBar()->setSingleStep(20);

    QObject::connect(&noteListDelegate, &NoteListDelegate::newEditorCreated, this, &NoteListView::onNewEditorCreated);
    QObject::connect(this, &QListView::customContextMenuRequested, this, &NoteListView::onCustomContextMenuRequested);

    setStyleSheet("QListView{border: none; background-color: transparent;}");

    selectionMenu = new NoteListViewSelectionMenu(this);
    QObject::connect(selectionMenu, &NoteListViewSelectionMenu::colorSelected, this,
                     &NoteListView::changeColorOfSelectedNotes);
    QObject::connect(selectionMenu, &NoteListViewSelectionMenu::deleteButtonClicked, this,
                     &NoteListView::removeSelectedNotes);
    QObject::connect(selectionMenu, &NoteListViewSelectionMenu::pinButtonClicked, this,
                     &NoteListView::toogleIsPinnedOfSelectedNotes);
    QObject::connect(selectionMenu, &NoteListViewSelectionMenu::cancelButtonClicked, this,
                     [this]() { this->setInSelectingState(false); });
    setInSelectingState(false);
}

void NoteListView::removeNote(const QModelIndex &index)
{
    const QString message = [index]() {
        if (index.data(NoteListModelRole::isInTrash).toBool())
            return "Are you sure you want to delete this note? You won't be able to recover deleted note";
        else
            return "Are you sure you want to remove this note? You will be able to restore it from trash folder";
    }();

    auto reply = QMessageBox::question(this, "Delete note?", message);
    if (reply == QMessageBox::No)
        return;
    model()->removeRow(index.row());
}

void NoteListView::onNewEditorCreated(NoteButton *editor, const QModelIndex &index)
{
    this->editor = editor;
    editor->installEventFilter(this);
    auto children = editor->findChildren<QWidget *>();
    for (auto const &child : children)
        child->installEventFilter(this);

    editor->setIsSelected(selectionModel()->isSelected(index));
    if (inSelectingState and !editor->getIsPinned())
        editor->setPinCheckboxVisible(false);

    QObject::connect(editor, &NoteButton::deleteNote, this, [this, index]() { this->removeNote(index); });
    QObject::connect(editor, &NoteButton::clicked, this, [this, index]() { emit this->noteSelected(index); });
    QObject::connect(editor, &NoteButton::pinCheckboxToogled, this,
                     [this, editor, index]() { noteListDelegate.setModelData(editor, this->model(), index); });
    QObject::connect(editor, &NoteButton::colorChanged, this,
                     [this, editor, index]() { noteListDelegate.setModelData(editor, this->model(), index); });
}

bool NoteListView::isTrashFolderLoaded()
{
    NoteListModel *noteModel = getSourceModelAtTheBottom();
    if (noteModel)
        return noteModel->getCurrentSelectedFolderId() == SpecialFolderId::TrashFolder;

    return false;
}

void NoteListView::onCustomContextMenuRequested(const QPoint &pos)
{
    if (isDragSelecting)
        return;

    QMenu *menu;

    if (inSelectingState)
        menu = createContextMenuForSelectingState();
    else
        menu = createContextMenuForNormalState(pos);

    menu->popup(mapToGlobal(pos));
}

void NoteListView::onRestoreNoteFromTrashRequested(const QModelIndex &index)
{
    NoteListModel *noteModel = getSourceModelAtTheBottom();
    QModelIndex noteModelIndex = mapIndexToSourceModelAtTheBott(index);
    if (noteModel)
        noteModel->restoreNoteFromTrash(noteModelIndex);
}

void NoteListView::removeSelectedNotes()
{
    const QString message = [this]() {
        if (this->isTrashFolderLoaded())
            return "Are you sure you want to delete selected notes? You won't be able to recover deleted notes";
        else
            return "Are you sure you want to remove selected notes? You will be able to restore them from trash folder";
    }();

    auto reply = QMessageBox::question(this, "Delete selected notes?", message);
    if (reply == QMessageBox::No)
        return;

    auto selected = selectionModel()->selectedIndexes();
    QModelIndexList noteModelIndexes = mapIndexesToSourceModelAtTheBottom(selected);
    NoteListModel *noteModel = getSourceModelAtTheBottom();
    noteModel->removeNotes(noteModelIndexes);
}

void NoteListView::changeColorOfSelectedNotes(const QColor &newColor)
{
    auto selected = selectionModel()->selectedIndexes();
    QModelIndexList noteModelIndexes = mapIndexesToSourceModelAtTheBottom(selected);
    NoteListModel *noteModel = getSourceModelAtTheBottom();
    noteModel->setColorOfNotes(noteModelIndexes, newColor);
}

void NoteListView::toogleIsPinnedOfSelectedNotes()
{
    auto selected = selectionModel()->selectedIndexes();
    bool allSelectedNotesArePinned = std::find_if(selected.begin(), selected.end(), [](const QModelIndex &index) {
                                         return !index.data(NoteListModelRole::isPinned).toBool();
                                     }) == selected.end();

    bool pinNotes;
    if (allSelectedNotesArePinned)
        pinNotes = false;
    else
        pinNotes = true;

    QModelIndexList noteModelIndexes = mapIndexesToSourceModelAtTheBottom(selected);
    NoteListModel *noteModel = getSourceModelAtTheBottom();
    noteModel->setIsPinnedOfNotes(noteModelIndexes, pinNotes);
}

bool NoteListView::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::Resize and watched == verticalScrollBar())
        emit verticalScrollBarWidthChanged();

    if (watched == verticalScrollBar() or watched == horizontalScrollBar())
        return QListView::eventFilter(watched, event);

    if (event->type() == QEvent::MouseButtonPress)
    {
        pressedPosition = this->mapFromGlobal(static_cast<QMouseEvent *>(event)->globalPosition().toPoint());
        pressedIndex = indexAt(pressedPosition);
    }

    if (event->type() == QEvent::MouseMove and static_cast<QMouseEvent *>(event)->buttons() & Qt::LeftButton and
        pressedIndex.isValid())
    {
        QPoint mouseDistanceSinceLastPressedPosition =
            pressedPosition - this->mapFromGlobal(static_cast<QMouseEvent *>(event)->globalPosition().toPoint());
        if (mouseDistanceSinceLastPressedPosition.manhattanLength() > QApplication::startDragDistance())
        {
            startDrag(Qt::CopyAction);
            pressedIndex = QModelIndex();
        }
    }

    if (QApplication::keyboardModifiers() & Qt::ControlModifier)
    {
        if (event->type() == QEvent::MouseButtonPress or event->type() == QEvent::MouseButtonDblClick or
            event->type() == QEvent::MouseMove)
            return true;

        if (event->type() == QEvent::MouseButtonRelease)
        {
            setInSelectingState(true);
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            QPoint positionInView = this->mapFromGlobal(mouseEvent->globalPosition().toPoint());
            QModelIndex clickedIndex = indexAt(positionInView);
            selectionModel()->select(clickedIndex, QItemSelectionModel::Select);
            setInSelectingState(true);
            return true;
        }
    }

    return QListView::eventFilter(watched, event);
}

void NoteListView::selectNotes(const QRect &rubberBand)
{
    auto currentState = state();
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setState(State::DragSelectingState);
    setSelection(rubberBand, QItemSelectionModel::Select);

    if (!selectionModel()->selection().isEmpty())
        setInSelectingState(true);

    setSelectionMode(QAbstractItemView::NoSelection);
    setState(currentState);
}

QPoint NoteListView::getOffsetOfViewport() const
{
    return QPoint(horizontalOffset(), verticalOffset());
}

void NoteListView::startDragSelecting(QPoint startPoint)
{
    isDragSelecting = true;
    startPoint.setY(qBound(-1, startPoint.y(), height()));
    dragSelectingInitialPoint = startPoint + QPoint(horizontalOffset(), verticalOffset());
    wasCtrlPressedWhileStartingDragSelecting = QApplication::keyboardModifiers() & Qt::ControlModifier;
}

void NoteListView::updateMousePositionOfDragSelecting(QPoint mousePositionPoint)
{
    if (!isDragSelecting)
        return;

    mousePositionPoint.setY(qBound(-1, mousePositionPoint.y(), height()));
    QRect selectionBound(dragSelectingInitialPoint - QPoint(horizontalOffset(), verticalOffset()), mousePositionPoint);

    if (!wasCtrlPressedWhileStartingDragSelecting)
        clearSelection();

    selectNotes(selectionBound.normalized());
}

void NoteListView::endDragSelecting()
{
    isDragSelecting = false;
}

void NoteListView::selectAll()
{
    if (model()->rowCount() <= 0)
        return;

    selectionModel()->select(QItemSelection(model()->index(0, 0), model()->index(model()->rowCount() - 1, 0)),
                             QItemSelectionModel::Select);
    setInSelectingState(true);
}

void NoteListView::resizeEvent(QResizeEvent *event)
{
    QListView::resizeEvent(event);
    int widthOfSelectionMenu = 200;
    selectionMenu->setGeometry((width() - widthOfSelectionMenu - verticalScrollBar()->width()) / 2,
                               viewport()->height() - 75, widthOfSelectionMenu, 50);
}

void NoteListView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    if (!selectionModel())
        return;

    int selectedNotesCount = selectionModel()->selectedIndexes().count();
    selectionMenu->setSelectedNotesCount(selectedNotesCount);
    if (selectedNotesCount == 0)
        setInSelectingState(false);

    QListView::selectionChanged(selected, deselected);
}

void NoteListView::setModel(QAbstractItemModel *model)
{
    QListView::setModel(model);
    QObject::connect(model, &QAbstractItemModel::modelReset, this, [this]() { setInSelectingState(false); });
}

bool NoteListView::viewportEvent(QEvent *event)
{
    bool result = QListView::viewportEvent(event);
    if (event->type() == QEvent::MouseButtonPress or event->type() == QEvent::MouseMove or
        event->type() == QEvent::MouseButtonRelease)
    {
        if (!indexAt(static_cast<QMouseEvent *>(event)->position().toPoint()).isValid() or isDragSelecting)
        {
            event->ignore();
        }
        else if (event->type() == QEvent::MouseButtonRelease)
        {
            QModelIndex clickedIndex = indexAt(static_cast<QMouseEvent *>(event)->position().toPoint());
            selectionModel()->select(clickedIndex, QItemSelectionModel::Toggle);
            if (selectionModel()->selection().empty())
                setInSelectingState(false);
            else
                setInSelectingState(true);
        }
    }
    return result;
}

void NoteListView::verticalScrollbarValueChanged(int value)
{
    QListView::verticalScrollbarValueChanged(value);
    updateMousePositionOfDragSelecting(mapFromGlobal(QCursor::pos()));
}

void NoteListView::startDrag(Qt::DropActions supportedActions)
{
    QModelIndexList indexes = selectedIndexes();

    if (indexes.isEmpty() and pressedIndex.isValid()) // Special case when user want to drag note in no electing state
    {
        indexes.emplaceBack(pressedIndex);
    }

    if (!indexes.contains(indexAt(mapFromGlobal(QCursor::pos()))))
        return;

    if (indexes.count() > 0)
    {
        setState(QAbstractItemView::DraggingState);
        updateEditor();
        QMimeData *data = model()->mimeData(indexes);
        if (!data)
            return;
        QPixmap pixmap = drawDragPixmap(indexes);
        QDrag *drag = new QDrag(this);
        drag->setPixmap(pixmap);
        drag->setMimeData(data);
        drag->setHotSpot(QPoint((qMin(indexes.size(), 4) - 1) * 3 + 20, (qMin(indexes.size(), 4) - 1) * 3 + 40));
        drag->exec(supportedActions);
    }
}

QPixmap NoteListView::drawDragPixmap(const QModelIndexList &indexes)
{
    QPixmap pixmap(60, 60);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    int notesToDrawCount = qMin(4, indexes.size());
    QVector<QColor> colorOfNotes = getNMostFrequentNotesColors(indexes, notesToDrawCount);

    for (int i = 0; i < notesToDrawCount; i++)
    {
        QPainterPath path;
        path.addRoundedRect(QRect(i * 3, i * 3, 40, 40), 5, 5);
        painter.fillPath(path, colorOfNotes[qMax(colorOfNotes.size() - 1 - i, 0)]);
        painter.drawPath(path);
    }
    QString dragingNotesCount = QString::number(indexes.size());
    QFont f;
    f.setPixelSize(dragingNotesCount.size() <= 3 ? 20 : 15);
    painter.setFont(f);
    painter.drawText(QRect((notesToDrawCount - 1) * 3, (notesToDrawCount - 1) * 3, 40, 40), Qt::AlignCenter,
                     dragingNotesCount);

    return pixmap;
}

QVector<QColor> NoteListView::getNMostFrequentNotesColors(const QModelIndexList &indexes, int N)
{
    QHash<QColor, int> colorsCounts;
    for (auto const &index : indexes)
        colorsCounts[index.data(NoteListModelRole::Color).value<QColor>()] += 1;

    QVector<std::pair<QColor, int>> colorCountsVec;
    for (auto i = colorsCounts.begin(), end = colorsCounts.end(); i != end; ++i)
        colorCountsVec.emplaceBack(i.key(), i.value());
    std::sort(colorCountsVec.begin(), colorCountsVec.end(),
              [](const std::pair<QColor, int> &colorCount1, const std::pair<QColor, int> &colorCount2) {
                  return colorCount1.second > colorCount2.second;
              });

    if (colorCountsVec.size() == 2 and colorCountsVec[0].second == colorCountsVec[1].second and indexes.size() > 2)
        colorCountsVec.append(colorCountsVec[1]);

    if (colorCountsVec.size() > N)
        colorCountsVec.erase(colorCountsVec.cbegin() + N);

    QVector<QColor> result;
    for (auto const &i : colorCountsVec)
        result.emplaceBack(i.first);

    return result;
}

QMenu *NoteListView::createContextMenuForSelectingState()
{
    QMenu *menu = new QMenu(this);

    QAction *deleteSelectedNotes = new QAction("Remove selected notes");
    QObject::connect(deleteSelectedNotes, &QAction::triggered, this, &NoteListView::removeSelectedNotes);
    menu->addAction(deleteSelectedNotes);

    auto selected = selectionModel()->selectedIndexes();
    bool allSelectedNotesArePinned = std::find_if(selected.begin(), selected.end(), [](const QModelIndex &index) {
                                         return !index.data(NoteListModelRole::isPinned).toBool();
                                     }) == selected.end();

    QAction *toogleIsPinnedOfSelectedNotes =
        new QAction(allSelectedNotesArePinned ? "Unpin selected notes" : "Pin selected notes");
    QObject::connect(toogleIsPinnedOfSelectedNotes, &QAction::triggered, this,
                     &NoteListView::toogleIsPinnedOfSelectedNotes);
    menu->addAction(toogleIsPinnedOfSelectedNotes);

    QAction *selectAll = new QAction("Select all");
    QObject::connect(selectAll, &QAction::triggered, this, &NoteListView::selectAll);
    menu->addAction(selectAll);

    QAction *exitSelecting = new QAction("Exit selecting");
    QObject::connect(exitSelecting, &QAction::triggered, this, [this]() { this->setInSelectingState(false); });
    menu->addAction(exitSelecting);

    return menu;
}

QMenu *NoteListView::createContextMenuForNormalState(const QPoint &position)
{
    QMenu *menu = new QMenu(this);
    QModelIndex index = indexAt(position);

    if (!isTrashFolderLoaded())
    {
        QAction *createNote = new QAction("New note");
        QObject::connect(createNote, &QAction::triggered, this, [this, index]() { this->model()->insertRow(0); });
        menu->addAction(createNote);
        if (index.isValid())
        {
            if (!index.data(NoteListModelRole::isPinned).toBool())
            {
                QAction *pinNote = new QAction("Pin note");
                QObject::connect(pinNote, &QAction::triggered, this,
                                 [this, index]() { this->model()->setData(index, true, NoteListModelRole::isPinned); });
                menu->addAction(pinNote);
            }
            else
            {
                QAction *unpinNote = new QAction("Unpin note");
                QObject::connect(unpinNote, &QAction::triggered, this, [this, index]() {
                    this->model()->setData(index, false, NoteListModelRole::isPinned);
                });
                menu->addAction(unpinNote);
            }
        }
    }
    else if (index.isValid())
    {
        QAction *restoreNote = new QAction("Restore note");
        QObject::connect(restoreNote, &QAction::triggered, this,
                         [this, index]() { this->onRestoreNoteFromTrashRequested(index); });
        menu->addAction(restoreNote);
    }

    if (index.isValid())
    {
        QAction *deleteNote = new QAction("Delete note");
        QObject::connect(deleteNote, &QAction::triggered, this, [this, index]() { removeNote(index); });
        menu->addAction(deleteNote);

        QAction *selectNote = new QAction("Select note");
        QObject::connect(selectNote, &QAction::triggered, this, [this, index]() {
            this->selectionModel()->select(index, QItemSelectionModel::Select);
            this->setInSelectingState(true);
        });
        menu->addAction(selectNote);
    }

    if (model()->rowCount() > 0)
    {
        QAction *selectAll = new QAction("Select all");
        QObject::connect(selectAll, &QAction::triggered, this, &NoteListView::selectAll);
        menu->addAction(selectAll);
    }

    return menu;
}

void NoteListView::updateEditor()
{
    if (inSelectingState or isDragSelecting or state() == QListView::DraggingState)
    {
        if (editor)
        {
            closePersistentEditor(currentIndexWithEditor);
            editor = nullptr;
            currentIndexWithEditor = QModelIndex();
        }
        return;
    }

    QPoint mousePosition = mapFromGlobal(QCursor::pos());
    const QModelIndex index = indexAt(mousePosition);

    if (currentIndexWithEditor == index)
        return;

    if (currentIndexWithEditor.isValid())
    {
        closePersistentEditor(currentIndexWithEditor);
        editor = nullptr;
    }

    if (index.isValid())
        openPersistentEditor(index);

    currentIndexWithEditor = index;
}

bool NoteListView::event(QEvent *event)
{
    if (event->type() == QEvent::MouseMove || event->type() == QEvent::Paint)
        updateEditor();

    return QListView::event(event);
}

bool NoteListView::getInSelectingState() const
{
    return inSelectingState;
}

void NoteListView::setInSelectingState(bool newInSelectingState)
{
    inSelectingState = newInSelectingState;
    selectionMenu->setVisible(newInSelectingState);
    if (selectionModel())
        selectionMenu->setSelectedNotesCount(selectionModel()->selectedIndexes().count());

    if (!newInSelectingState)
    {
        selectionMenu->setColorPickerVisible(false);
        if (selectionModel() and !selectionModel()->selectedIndexes().empty())
            selectionModel()->clearSelection();
    }
    updateEditor();
}

int NoteListView::getHowManyNotesCanFitInRow(int width) const
{
    return (width - verticalScrollBar()->geometry().width() - 1) / gridSize().width();
}

int NoteListView::getHowManyNotesAreDisplayed() const
{
    return model()->rowCount();
}

void NoteListView::setMinWidthToFitNotesInRow(int numberOfNotesToFitInOneRow)
{
    resize(numberOfNotesToFitInOneRow * gridSize().width() + verticalScrollBar()->geometry().width() + 1, height());
}

NoteListModel *NoteListView::getSourceModelAtTheBottom() const
{
    // We know that at the bottom there is NoteListModel.
    // If we want to access it, we need to find it, because proxy model can be set to view
    QAbstractItemModel *currentModel = model();
    forever
    {
        if (NoteListModel *noteModel = qobject_cast<NoteListModel *>(currentModel))
            return noteModel;

        if (QAbstractProxyModel *proxyModel = qobject_cast<QAbstractProxyModel *>(currentModel))
            currentModel = proxyModel->sourceModel();
        else
            return nullptr;
    }
}

QModelIndex NoteListView::mapIndexToSourceModelAtTheBott(const QModelIndex &index) const
{
    // We know that at the bottom there is NoteListModel.
    // Because indexes which view uses can be from proxy model,
    // we need to map it if we want to access directly NoteListModel
    QAbstractItemModel *currentModel = model();
    QModelIndex currentIndex = index;
    while (QAbstractProxyModel *proxyModel = qobject_cast<QAbstractProxyModel *>(currentModel))
    {
        currentIndex = proxyModel->mapToSource(currentIndex);
        currentModel = proxyModel->sourceModel();
    }

    return currentIndex;
}

QModelIndexList NoteListView::mapIndexesToSourceModelAtTheBottom(const QModelIndexList &indexes)
{
    QModelIndexList result;
    result.reserve(indexes.size());

    for (auto const &index : indexes)
    {
        result.append(mapIndexToSourceModelAtTheBott(index));
    }

    return result;
}

template <> struct std::hash<QColor>
{
    std::size_t operator()(const QColor &c) const noexcept
    {
        return std::hash<unsigned int>{}(c.rgba());
    }
};
