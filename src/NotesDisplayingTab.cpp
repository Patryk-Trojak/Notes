#include "NotesDisplayingTab.h"
#include "PersistenceManager.h"
#include "ui_NotesDisplayingTab.h"

NotesDisplayingTab::NotesDisplayingTab(QWidget *parent)
    : QWidget(parent), ui(new Ui::NotesDisplayingTab), notes(PersistenceManager::loadAllNotes())
{
    ui->setupUi(this);
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

    QVBoxLayout *layout = new QVBoxLayout();
    ui->scrollArea->widget()->setLayout(layout);
    currentNoteButtonsSortingMethod = &NotesDisplayingTab::sortNoteButtonsByCreationDate;
    createNewNoteButtonsFromNotes();
}

NotesDisplayingTab::~NotesDisplayingTab()
{
    delete ui;
}

void NotesDisplayingTab::createNewNoteButtonsFromNotes()
{
    for (auto const &note : notes)
        createNewNoteButton(*note);
}

void NotesDisplayingTab::onNewNoteButtonPressed()
{
    std::unique_ptr<Note> note = PersistenceManager::createNewNoteFile();
    createNewNoteButton(*note);
    notes.push_back(std::move(note));
    emit enterEditingNote(*notes.back());
}

void NotesDisplayingTab::updateNoteButton(const Note &note)
{
    auto button = noteToButtonMap.find(&note);
    if (button != noteToButtonMap.end())
    {
        (*button)->setTitle(note.getTitle());
        (*button)->setCreationTime(note.getCreationTime());
        (*button)->setModificationTime(note.getModificationTime());
    }
    currentNoteButtonsSortingMethod(this, ui->sortInAscendingOrderButton->isChecked());
}

void NotesDisplayingTab::onNoteButtonChangedTitle()
{
    NoteButton *button = qobject_cast<NoteButton *>(QObject::sender());
    auto foundNote = buttonToNoteMap.find(button);
    if (foundNote != buttonToNoteMap.end())
    {
        (*foundNote)->setTitle(button->getTitle());
        PersistenceManager::saveNoteToFile(**foundNote);
        updateNoteButton(**foundNote);
    }
}

void NotesDisplayingTab::onNoteButtonClicked()
{
    NoteButton *button = qobject_cast<NoteButton *>(QObject::sender());
    auto foundNote = buttonToNoteMap.find(button);
    if (foundNote != buttonToNoteMap.end())
    {
        emit enterEditingNote(**foundNote);
    }
}

void NotesDisplayingTab::deleteNoteFromVector(Note *note)
{
    notes.erase(std::remove_if(notes.begin(), notes.end(),
                               [note](const std::unique_ptr<Note> &notePtr) { return notePtr.get() == note; }),
                notes.end());
}

void NotesDisplayingTab::sortNoteButtons(std::function<bool(const QWidget *, const QWidget *)> compare)
{
    QList<QWidget *> list = myUtils::getWidgetsFromLayout(*ui->scrollArea->widget()->layout());
    std::sort(list.begin(), list.end(), compare);
    auto oldLayout = ui->scrollArea->widget()->layout();
    delete oldLayout;
    QVBoxLayout *layout = new QVBoxLayout();
    ui->scrollArea->widget()->setLayout(layout);
    for (auto const &i : list)
        ui->scrollArea->widget()->layout()->addWidget(i);
}

void NotesDisplayingTab::sortNoteButtonsByTitle(bool ascendingOrder)
{
    sortNoteButtons([ascendingOrder](const QWidget *a, const QWidget *b) -> bool {
        if (ascendingOrder)
        {
            return static_cast<const NoteButton *>(a)->getTitle().toLower() <
                   static_cast<const NoteButton *>(b)->getTitle().toLower();
        }
        else
        {
            return static_cast<const NoteButton *>(a)->getTitle().toLower() >
                   static_cast<const NoteButton *>(b)->getTitle().toLower();
        }
    });
}

void NotesDisplayingTab::sortNoteButtonsByCreationDate(bool ascendingOrder)
{
    sortNoteButtons([ascendingOrder](const QWidget *a, const QWidget *b) -> bool {
        if (ascendingOrder)
        {
            return static_cast<const NoteButton *>(a)->getCreationTime() <
                   static_cast<const NoteButton *>(b)->getCreationTime();
        }
        else
        {
            return static_cast<const NoteButton *>(a)->getCreationTime() >
                   static_cast<const NoteButton *>(b)->getCreationTime();
        }
    });
}

void NotesDisplayingTab::sortNoteButtonsByModificationDate(bool ascendingOrder)
{
    sortNoteButtons([ascendingOrder](const QWidget *a, const QWidget *b) -> bool {
        if (ascendingOrder)
        {
            return static_cast<const NoteButton *>(a)->getModificationTime() <
                   static_cast<const NoteButton *>(b)->getModificationTime();
        }
        else
        {
            return static_cast<const NoteButton *>(a)->getModificationTime() >
                   static_cast<const NoteButton *>(b)->getModificationTime();
        }
    });
}

void NotesDisplayingTab::onNoteButtonDeleted()
{
    auto reply = QMessageBox::question(this, "Delete note?", "Are you sure you want to delete this note?");
    if (reply == QMessageBox::No)
        return;

    NoteButton *button = qobject_cast<NoteButton *>(QObject::sender());
    auto foundNote = buttonToNoteMap.find(button);
    if (foundNote != buttonToNoteMap.end())
    {
        PersistenceManager::deleteNoteFile((*foundNote)->getFilename());
        noteToButtonMap.erase(noteToButtonMap.find(*foundNote));
        deleteNoteFromVector(*foundNote);
        buttonToNoteMap.erase(foundNote);
    }
    delete button;
}

void NotesDisplayingTab::onSortByTitleButtonToggled()
{
    bool ascendingOrder = ui->sortInAscendingOrderButton->isChecked();
    sortNoteButtonsByTitle(ascendingOrder);
    currentNoteButtonsSortingMethod = &NotesDisplayingTab::sortNoteButtonsByTitle;
}

void NotesDisplayingTab::onSortByCreationDateButtonToggled()
{
    bool ascendingOrder = ui->sortInAscendingOrderButton->isChecked();
    sortNoteButtonsByCreationDate(ascendingOrder);
    currentNoteButtonsSortingMethod = &NotesDisplayingTab::sortNoteButtonsByCreationDate;
}

void NotesDisplayingTab::onSortByModificationDateButtonToggled()
{
    bool ascendingOrder = ui->sortInAscendingOrderButton->isChecked();
    sortNoteButtonsByModificationDate(ascendingOrder);
    currentNoteButtonsSortingMethod = &NotesDisplayingTab::sortNoteButtonsByModificationDate;
}

void NotesDisplayingTab::onSortOrderButtonToggled()
{
    currentNoteButtonsSortingMethod(this, ui->sortInAscendingOrderButton->isChecked());
}

void NotesDisplayingTab::createNewNoteButton(Note &note)
{
    NoteButton *noteButton = new NoteButton(note.getTitle(), note.getCreationTime(), note.getModificationTime());
    QObject::connect(noteButton, &NoteButton::saveNote, this, &NotesDisplayingTab::onNoteButtonChangedTitle);
    QObject::connect(noteButton, &NoteButton::enterEditingNote, this, &NotesDisplayingTab::onNoteButtonClicked);
    QObject::connect(noteButton, &NoteButton::deleteNote, this, &NotesDisplayingTab::onNoteButtonDeleted);

    buttonToNoteMap.insert(noteButton, &note);
    noteToButtonMap.insert(static_cast<const Note *>(&note), noteButton);
    ui->scrollArea->widget()->layout()->addWidget(noteButton);
    currentNoteButtonsSortingMethod(this, ui->sortInAscendingOrderButton->isChecked());
}
