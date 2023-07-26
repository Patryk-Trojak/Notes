#include "NotesDisplayingTab.h"
#include "ui_NotesDisplayingTab.h"
#include <AboutWindow.h>
#include <QShortcut>
#include <QStringListModel>
NotesDisplayingTab::NotesDisplayingTab(NoteListModel &noteModel, PersistenceManager &persistenceManager,
                                       QWidget *parent)
    : QWidget(parent), ui(new Ui::NotesDisplayingTab), folderModel(persistenceManager), noteModel(noteModel),
      noteProxyModel(this)
{
    ui->setupUi(this);
    QObject::connect(ui->aboutNotes, &QPushButton::clicked, this, [this]() {
        auto aboutWindow = new AboutWindow(this);
        aboutWindow->show();
    });
    QObject::connect(ui->newNoteButton, &QPushButton::clicked, this, &NotesDisplayingTab::onNewNoteButtonPressed);
    QObject::connect(ui->sortByTitleButton, &QRadioButton::toggled, this,
                     &NotesDisplayingTab::onSortByTitleButtonToggled);
    QObject::connect(ui->sortByCreationDateButton, &QRadioButton::toggled, this,
                     &NotesDisplayingTab::onSortByCreationDateButtonToggled);
    QObject::connect(ui->sortByModificationDateButton, &QRadioButton::toggled, this,
                     &NotesDisplayingTab::onSortByModificationDateButtonToggled);
    QObject::connect(ui->sortInAscendingOrderButton, &QRadioButton::toggled, this,
                     &NotesDisplayingTab::onSortOrderButtonToggled);
    QObject::connect(ui->sortInDescendingOrder, &QRadioButton::toggled, this,
                     &NotesDisplayingTab::onSortOrderButtonToggled);

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
    QObject::connect(ui->searchBar, &QLineEdit::textChanged, this,
                     [this](const QString &searched) { this->noteProxyModel.setFilterFixedString(searched); });

    ui->noteListView->setModel(&noteProxyModel);
    ui->noteListView->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(ui->noteListView, &NoteListView::noteSelected, this, [this](const QModelIndex &index) {
        auto sourceIndex = this->noteProxyModel.mapToSource(index);
        emit this->enterEditingNote(sourceIndex);
    });
    onNewFolderSelected(-1);
}

NotesDisplayingTab::~NotesDisplayingTab()
{
    delete ui;
}

void NotesDisplayingTab::onNewNoteButtonPressed()
{
    noteModel.createNewNote();
}

void NotesDisplayingTab::onSortByTitleButtonToggled()
{
    noteProxyModel.setSortRole(NoteListModel::Title);
}

void NotesDisplayingTab::onSortByCreationDateButtonToggled()
{
    noteProxyModel.setSortRole(NoteListModel::CreationTime);
}

void NotesDisplayingTab::onSortByModificationDateButtonToggled()
{
    noteProxyModel.setSortRole(NoteListModel::ModificationTime);
}

void NotesDisplayingTab::onSortOrderButtonToggled()
{
    Qt::SortOrder sortOrder = Qt::DescendingOrder;
    if (ui->sortInAscendingOrderButton->isChecked())
        sortOrder = Qt::AscendingOrder;

    noteProxyModel.sort(0, sortOrder);
}

void NotesDisplayingTab::onNewFolderSelected(int selectedFolderId)
{
    noteModel.onNewFolderSelected(selectedFolderId);
    ui->searchBar->setText("");
}
