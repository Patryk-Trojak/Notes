#include "NotesDisplayingTab.h"
#include "ui_NotesDisplayingTab.h"
#include <AboutWindow.h>
#include <QMoveEvent>
#include <QScrollBar>
#include <QShortcut>
#include <QStringListModel>

NotesDisplayingTab::NotesDisplayingTab(NoteListModel &noteModel, PersistenceManager &persistenceManager,
                                       QWidget *parent)
    : QWidget(parent), ui(new Ui::NotesDisplayingTab), folderModel(persistenceManager, this), noteModel(noteModel),
      noteProxyModel(this)
{
    ui->setupUi(this);
    ui->middleFrame->installEventFilter(this);
    ui->newNoteButton->setAttribute(Qt::WA_StyledBackground, true);
    ui->splitter->handle(1)->setStyleSheet("border: none; background-color: rgb(191, 191, 191);");
    ui->splitter->handle(1)->setAttribute(Qt::WA_StyledBackground, true);
    ui->splitter->setHandleWidth(2);
    ui->splitter->setSizes({400, 2000});
    noteListView = new NoteListView(ui->middleFrame);
    searchBar = new SearchBar(ui->middleFrame);
    openNoteSortOptionsButton = new QPushButton(ui->middleFrame);
    openNoteSortOptionsButton->setIcon(QIcon(":/images/options.png"));
    openNoteSortOptionsButton->setStyleSheet("border: none; padding: 0px; margin: 0px;");
    openNoteSortOptionsButton->resize(22, 22);
    openNoteSortOptionsButton->setIconSize(QSize(22, 22));
    QObject::connect(openNoteSortOptionsButton, &QPushButton::clicked, this,
                     &NotesDisplayingTab::onOpenNoteSortOptionsButtonClicked);

    QObject::connect(ui->newNoteButton, &QPushButton::clicked, this, &NotesDisplayingTab::onNewNoteButtonPressed);

    QShortcut *createNewNoteShortcut = new QShortcut(QKeySequence(QKeySequence::New), this);
    QObject::connect(createNewNoteShortcut, &QShortcut::activated, this, &NotesDisplayingTab::onNewNoteButtonPressed);

    ui->folderTreeView->setModel(&folderModel);
    ui->folderTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(ui->folderTreeView, &FolderTreeView::newFolderSelected, this,
                     &NotesDisplayingTab::onNewFolderSelected);
    noteProxyModel.setSourceModel(&noteModel);
    noteProxyModel.setFilterCaseSensitivity(Qt::CaseInsensitive);
    noteProxyModel.setSortCaseSensitivity(Qt::CaseInsensitive);
    noteProxyModel.setSortRole(NoteListModel::CreationTime);
    noteProxyModel.sort(0, Qt::DescendingOrder);
    QObject::connect(searchBar, &QLineEdit::textChanged, this,
                     [this](const QString &searched) { this->noteProxyModel.setFilterFixedString(searched); });

    noteListView->setModel(&noteProxyModel);
    noteListView->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(noteListView, &NoteListView::noteSelected, this, [this](const QModelIndex &index) {
        auto sourceIndex = this->noteProxyModel.mapToSource(index);
        emit this->enterEditingNote(sourceIndex);
    });

    onNewFolderSelected(ui->folderTreeView->getCurrentFolderSelectedId());
    QObject::connect(&folderModel, &FolderTreeModel::folderDeletedFromDatabase, &noteModel,
                     &NoteListModel::onFolderDeleted);

    QObject::connect(&noteModel, &NoteListModel::notesAddedToFolder, this, [this](int folderId, int addedNoteCount) {
        this->folderModel.updateNotesInsideCountOfFolder(folderId, addedNoteCount);
    });
    QObject::connect(&noteModel, &NoteListModel::notesRemovedFromFolder, this,
                     [this](int folderId, int removedNoteCount) {
                         this->folderModel.updateNotesInsideCountOfFolder(folderId, -removedNoteCount);
                     });

    QObject::connect(&folderModel, &FolderTreeModel::folderDeletedFromDatabase, &noteModel,
                     &NoteListModel::onFolderDeleted);

    QObject::connect(&noteProxyModel, &NoteSortFilterProxyModel::rowsInserted, this,
                     &NotesDisplayingTab::layoutAllElementsWhichDependsOnNumberOfNotes);
    QObject::connect(&noteProxyModel, &NoteSortFilterProxyModel::rowsRemoved, this,
                     &NotesDisplayingTab::layoutAllElementsWhichDependsOnNumberOfNotes);

    layoutAllElementsWhichDependsOnNumberOfNotes();
}

NotesDisplayingTab::~NotesDisplayingTab()
{
    delete ui;
}

void NotesDisplayingTab::onNewNoteButtonPressed()
{
    QModelIndex createdNoteIndex = noteModel.createNewNote();
    if (createdNoteIndex.isValid())
        emit enterEditingNote(createdNoteIndex);
}

void NotesDisplayingTab::onNewNoteSortRoleSelected(int newSortRole)
{
    noteProxyModel.setSortRole(newSortRole);
}

void NotesDisplayingTab::onNewNoteSortOrderSelected(Qt::SortOrder newSortOrder)
{
    noteProxyModel.sort(0, newSortOrder);
}

void NotesDisplayingTab::onNewFolderSelected(int selectedFolderId)
{
    noteModel.onNewFolderSelected(selectedFolderId);
    searchBar->setText("");
    layoutAllElementsWhichDependsOnNumberOfNotes();
}

void NotesDisplayingTab::onOpenNoteSortOptionsButtonClicked()
{
    NoteSortOptionsWidget *noteSortOptionsWidget =
        new NoteSortOptionsWidget(noteProxyModel.sortRole(), noteProxyModel.sortOrder(), ui->middleFrame, Qt::Popup);

    QPoint positionOfNoteSortOptionsWidget = this->ui->middleFrame->mapToGlobal(openNoteSortOptionsButton->pos());
    QSize sizeOfSortOptions(210, 100);
    noteSortOptionsWidget->setGeometry(positionOfNoteSortOptionsWidget.x() - sizeOfSortOptions.width(),
                                       positionOfNoteSortOptionsWidget.y() + openNoteSortOptionsButton->height() + 10,
                                       sizeOfSortOptions.width(), sizeOfSortOptions.height());

    QObject::connect(noteSortOptionsWidget, &NoteSortOptionsWidget::newSortRoleSelected, this,
                     &NotesDisplayingTab::onNewNoteSortRoleSelected);
    QObject::connect(noteSortOptionsWidget, &NoteSortOptionsWidget::newSortOrderSelected, this,
                     &NotesDisplayingTab::onNewNoteSortOrderSelected);

    noteSortOptionsWidget->show();
}

void NotesDisplayingTab::layoutSearchBar()
{
    if (ui->splitter->sizes().at(1) == 0)
    {
        searchBar->setVisible(false);
        return;
    }
    else
        searchBar->setVisible(true);

    int widthOfSearchBar = qMin(noteListView->width() - 50, 600);
    widthOfSearchBar = qMax(widthOfSearchBar, 200);
    int leftOfSearchBar = noteListView->pos().x() +
                          (noteListView->width() - widthOfSearchBar - noteListView->verticalScrollBar()->width()) / 2;
    searchBar->setGeometry(leftOfSearchBar, 9, widthOfSearchBar, searchBar->height());
}

void NotesDisplayingTab::layoutOpenNoteSortOptionsButton()
{
    if (ui->splitter->sizes().at(1) == 0)
    {
        openNoteSortOptionsButton->setVisible(false);
        return;
    }
    else
        openNoteSortOptionsButton->setVisible(true);

    openNoteSortOptionsButton->move(searchBar->pos().x() + searchBar->width() + 5,
                                    searchBar->pos().y() +
                                        (searchBar->height() - openNoteSortOptionsButton->height()) / 2);
}

void NotesDisplayingTab::layoutNoteListView()
{
    int leftMargin = 7;
    int rightMargin = 7;
    int marginTop = 51;
    if (ui->splitter->sizes().at(1) == 0)
    {
        noteListView->setVisible(false);
        return;
    }
    else
        noteListView->setVisible(true);

    int availableWidthForNoteView = ui->middleFrame->width() - rightMargin - leftMargin;
    int numberOfNotesInRow = qMin(noteListView->getHowManyNotesCanFitInRow(availableWidthForNoteView),
                                  noteListView->getHowManyNotesAreDisplayed());
    noteListView->setMinWidthToFitNotesInRow(numberOfNotesInRow);

    int newLeftOfNoteView =
        leftMargin +
        (availableWidthForNoteView - noteListView->width() + noteListView->verticalScrollBar()->width()) / 2;

    noteListView->setGeometry(newLeftOfNoteView, marginTop, noteListView->width(),
                              ui->middleFrame->height() - marginTop);
}

void NotesDisplayingTab::layoutAllElementsWhichDependsOnNumberOfNotes()
{
    layoutNoteListView();
    layoutSearchBar();
    layoutOpenNoteSortOptionsButton();
}

void NotesDisplayingTab::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    layoutAllElementsWhichDependsOnNumberOfNotes();
}

bool NotesDisplayingTab::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->middleFrame)
    {
        if (event->type() == QEvent::Resize or event->type() == QEvent::Move)
        {
            layoutAllElementsWhichDependsOnNumberOfNotes();
        }
    }
    return false;
}
