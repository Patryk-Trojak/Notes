#include "NoteEditor.h"
#include "./ui_NoteEditor.h"
#include "NoteListModelRole.h"
#include <QDateTime>
#include <QResizeEvent>

NoteEditor::NoteEditor(const QModelIndex &editingNote, QWidget *parent) : QWidget(parent), ui(new Ui::NoteEditor)
{
    ui->setupUi(this);
    int widthOfScrollbar = 14;
    QString style =
        QString(
            "QWidget#NoteEditor{border-style: solid; border-color: transparent; border-width:10px; "
            "border-radius:30px; background-color: %1;} "
            "QLineEdit{background-color: transparent; border: none;}"
            "QTextEdit{background-color: transparent; border: none;}"
            "QScrollBar{background-color: transparent;}"
            "QScrollBar::handle:vertical:hover {background: rgba(40, 40, 40, 0.5);} "
            "QScrollBar::handle:vertical:pressed {background: rgba(40, 40, 40, 0.5);}"
            "QScrollBar::handle:vertical {border-radius: 4px;background: rgba(100, 100, 100, 0.5);min-height: 20px;}"
            "QScrollBar::vertical {border-radius: 6px;width: 10px;color: rgba(255, 255, 255, 0);}"
            "QScrollBar::add-line:vertical {width: 0px; height: 0px;subcontrol-position: bottom;subcontrol-origin: "
            "margin;}"
            "QScrollBar::sub-line:vertical {width: 0px;height: 0px;subcontrol-position: top;subcontrol-origin: "
            "margin;}")
            .arg(editingNote.data(NoteListModelRole::Color).value<QColor>().name());
    setStyleSheet(style);
    setAttribute(Qt::WA_StyledBackground, true);
    ui->creationTime->setStyleSheet("QLabel{color: black}");
    ui->modificationTime->setStyleSheet("QLabel{color: black}");
    ui->titleEdit->setText(editingNote.data(NoteListModelRole::Title).toString());
    ui->contentEdit->setHtml(editingNote.data(NoteListModelRole::Content).toString());
    setCreationTime(editingNote.data(NoteListModelRole::CreationTime).toDateTime());
    setModificationTime(editingNote.data(NoteListModelRole::ModificationTime).toDateTime());

    closeButton = new QPushButton(this);
    closeButton->setIcon(QIcon(":/images/clear.png"));
    closeButton->setStyleSheet("Margin: 0px; Padding: 0px; border-style: solid;");
    closeButton->setIconSize(QSize(20, 20));
    closeButton->resize(20, 20);

    QObject::connect(closeButton, &QPushButton::clicked, this, &NoteEditor::closeNoteRequested);
    QObject::connect(ui->titleEdit, &QLineEdit::textChanged, this, &NoteEditor::titleChanged);
    QObject::connect(ui->titleEdit, &QLineEdit::textChanged, this,
                     [this]() { setModificationTime(QDateTime::currentDateTime()); });
    QObject::connect(ui->contentEdit, &QTextEdit::textChanged, this,
                     [this]() { emit contentChanged(this->ui->contentEdit->toHtml()); });
    QObject::connect(ui->contentEdit, &QTextEdit::textChanged, this,
                     [this]() { setModificationTime(QDateTime::currentDateTime()); });
}

NoteEditor::~NoteEditor()
{
    delete ui;
}

void NoteEditor::setCreationTime(const QDateTime &creationTime)
{
    ui->creationTime->setText("Created: " + creationTime.toString("dd.MM.yy HH:mm"));
}

void NoteEditor::setModificationTime(const QDateTime &modificationTime)
{
    ui->modificationTime->setText("Modified: " + modificationTime.toString("dd.MM.yy HH:mm"));
}

void NoteEditor::resizeEvent(QResizeEvent *event)
{
    QPoint rightTop = QPoint(event->size().width(), 0);
    closeButton->move(rightTop + QPoint(-closeButton->width() - 20, 20));
}
