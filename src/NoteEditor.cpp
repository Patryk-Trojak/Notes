#include "NoteEditor.h"
#include "./ui_NoteEditor.h"
#include "NoteListModelRole.h"
#include <QColorDialog>
#include <QDateTime>
#include <QFontInfo>
#include <QResizeEvent>
#include <QTextDocument>
#include <QTextList>

NoteEditor::NoteEditor(const QModelIndex &editingNote, QWidget *parent) : QWidget(parent), ui(new Ui::NoteEditor)
{
    ui->setupUi(this);
    int widthOfScrollbar = 14;
    QString style = QString("QWidget#NoteEditor{border-style: solid; border-color: transparent; border-width:10px; "
                            "border-radius:30px;} "
                            "QWidget{ background-color: %1;}"
                            "QLineEdit{background-color: transparent; border: none;}"
                            "QTextEdit{background-color: transparent; border: none;}")
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
    QObject::connect(ui->contentEdit, &QTextEdit::textChanged, this, &NoteEditor::onCurrentBlockFormatChanged);
    QObject::connect(ui->contentEdit, &QTextEdit::currentCharFormatChanged, this,
                     &NoteEditor::onCurrentBlockFormatChanged);
    QObject::connect(ui->contentEdit, &QTextEdit::currentCharFormatChanged, this,
                     &NoteEditor::onCurrentCharFormatChanged);

    QObject::connect(ui->boldCheckBox, &QCheckBox::clicked, this, &NoteEditor::switchTextBold);
    QObject::connect(ui->italicCheckBox, &QCheckBox::clicked, this, &NoteEditor::switchTextItalic);
    QObject::connect(ui->underlineCheckBox, &QCheckBox::clicked, this, &NoteEditor::switchTextUnderline);
    QObject::connect(ui->strikeOutCheckBox, &QCheckBox::clicked, this, &NoteEditor::switchTextStrikeOut);
    QObject::connect(ui->textColorButton, &QPushButton::clicked, this, &NoteEditor::setTextColor);
    QObject::connect(ui->textBackgroundColorButton, &QPushButton::clicked, this, &NoteEditor::setTextBackgroundColor);
    QObject::connect(ui->alignMenuButton, &QPushButton::clicked, this, &NoteEditor::openAlignPopupMenu);
    QObject::connect(ui->indentButton, &QPushButton::clicked, this, [this]() { this->modifyIndentation(1); });
    QObject::connect(ui->unindentButton, &QPushButton::clicked, this, [this]() { this->modifyIndentation(-1); });
    QObject::connect(ui->unorderedListCheckBox, &QPushButton::clicked, this, [this] {
        this->switchListStyle(QTextListFormat::ListDisc, QTextBlockFormat::MarkerType::NoMarker);
    });
    QObject::connect(ui->orderedListCheckBox, &QPushButton::clicked, this, [this] {
        this->switchListStyle(QTextListFormat::ListDecimal, QTextBlockFormat::MarkerType::NoMarker);
    });
    QObject::connect(ui->taskListCheckBox, &QPushButton::clicked, this, [this] {
        this->switchListStyle(QTextListFormat::ListDisc, QTextBlockFormat::MarkerType::Unchecked);
    });
    onCurrentCharFormatChanged(ui->contentEdit->currentCharFormat());
    onCurrentBlockFormatChanged();
}

NoteEditor::~NoteEditor()
{
    delete ui;
}

void NoteEditor::setResourceLoader(const NoteContentEdit::ResourceLoader &resourceProvider)
{
    ui->contentEdit->setResourceLoader(resourceProvider);
}

void NoteEditor::setResourceSaver(const NoteContentEdit::ResourceSaver &resourceSaver)
{
    ui->contentEdit->setResourceSaver(resourceSaver);
}

void NoteEditor::setCreationTime(const QDateTime &creationTime)
{
    ui->creationTime->setText("Created: " + creationTime.toString("dd.MM.yy HH:mm"));
}

void NoteEditor::setModificationTime(const QDateTime &modificationTime)
{
    ui->modificationTime->setText("Modified: " + modificationTime.toString("dd.MM.yy HH:mm"));
}

void NoteEditor::switchTextBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(ui->boldCheckBox->isChecked() ? QFont::Bold : QFont::Normal);
    ui->contentEdit->mergeCurrentCharFormat(fmt);
}

void NoteEditor::switchTextItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(ui->italicCheckBox->isChecked());
    ui->contentEdit->mergeCurrentCharFormat(fmt);
}

void NoteEditor::switchTextUnderline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(ui->underlineCheckBox->isChecked());
    ui->contentEdit->mergeCurrentCharFormat(fmt);
}

void NoteEditor::switchTextStrikeOut()
{
    QTextCharFormat fmt;
    fmt.setFontStrikeOut(ui->strikeOutCheckBox->isChecked());
    ui->contentEdit->mergeCurrentCharFormat(fmt);
}

void NoteEditor::setTextColor()
{
    QColor textColor = QColorDialog::getColor(ui->contentEdit->textColor(), this, "", QColorDialog::ShowAlphaChannel);
    ui->contentEdit->setTextColor(textColor);
}

void NoteEditor::setTextBackgroundColor()
{
    QColor textBackgroundColor =
        QColorDialog::getColor(ui->contentEdit->textBackgroundColor(), this, "", QColorDialog::ShowAlphaChannel);
    ui->contentEdit->setTextBackgroundColor(textBackgroundColor);
}

void NoteEditor::alignParagraph(Qt::Alignment alignment)
{
    ui->contentEdit->setAlignment(alignment);
}

void NoteEditor::openAlignPopupMenu()
{
    QWidget *cursor = new QWidget(ui->contentEdit);
    cursor->setGeometry(ui->contentEdit->cursorRect());
    cursor->setStyleSheet({"background-color: black;"});
    cursor->setAttribute(Qt::WA_StyledBackground);
    cursor->show();

    QWidget *alignPopupMenu = new QWidget(this, Qt::Popup);
    alignPopupMenu->setFixedSize(150, 40);
    alignPopupMenu->setStyleSheet(ui->toolbarScrollArea->styleSheet());
    alignPopupMenu->setFocusPolicy(Qt::NoFocus);

    QCheckBox *alignLeftButton = new QCheckBox(alignPopupMenu);
    QObject::connect(alignLeftButton, &QPushButton::clicked, this, [this, alignPopupMenu]() {
        this->alignParagraph(Qt::AlignLeft);
        alignPopupMenu->close();
    });
    alignLeftButton->setFocusPolicy(Qt::NoFocus);
    alignLeftButton->setStyleSheet(
        "QCheckBox::indicator{image: url(:/images/alignLeft.png); width: 30px; height: 30px;}");
    alignLeftButton->setChecked(ui->contentEdit->alignment().testFlag(Qt::AlignLeft));
    alignLeftButton->setFixedSize(ui->alignMenuButton->size());

    QCheckBox *alignRightButton = new QCheckBox(alignPopupMenu);
    QObject::connect(alignRightButton, &QPushButton::clicked, this, [this, alignPopupMenu]() {
        this->alignParagraph(Qt::AlignRight);
        alignPopupMenu->close();
    });
    alignRightButton->setFocusPolicy(Qt::NoFocus);
    alignRightButton->setStyleSheet(
        "QCheckBox::indicator{image: url(:/images/alignRight.png); width: 30px; height: 30px;}");
    alignRightButton->setChecked(ui->contentEdit->alignment().testFlag(Qt::AlignRight));
    alignRightButton->setFixedSize(ui->alignMenuButton->size());

    QCheckBox *alignCenterButton = new QCheckBox(alignPopupMenu);
    QObject::connect(alignCenterButton, &QPushButton::clicked, this, [this, alignPopupMenu]() {
        this->alignParagraph(Qt::AlignCenter);
        alignPopupMenu->close();
    });
    alignCenterButton->setFocusPolicy(Qt::NoFocus);
    alignCenterButton->setStyleSheet(
        "QCheckBox::indicator{image: url(:/images/alignCenter.png); width: 30px; height: 30px;}");
    alignCenterButton->setChecked(ui->contentEdit->alignment().testFlag(Qt::AlignHCenter));
    alignCenterButton->setFixedSize(ui->alignMenuButton->size());

    QCheckBox *alignJustifyButton = new QCheckBox(alignPopupMenu);
    QObject::connect(alignJustifyButton, &QPushButton::clicked, this, [this, alignPopupMenu]() {
        this->alignParagraph(Qt::AlignJustify);
        alignPopupMenu->close();
    });
    alignJustifyButton->setFocusPolicy(Qt::NoFocus);
    alignJustifyButton->setStyleSheet(
        "QCheckBox::indicator{image: url(:/images/alignJustify.png); width: 30px; height: 30px;}");
    alignJustifyButton->setChecked(ui->contentEdit->alignment().testFlag(Qt::AlignJustify));
    alignJustifyButton->setFixedSize(ui->alignMenuButton->size());

    QHBoxLayout *layout = new QHBoxLayout(alignPopupMenu);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(alignLeftButton);
    layout->addWidget(alignRightButton);
    layout->addWidget(alignCenterButton);
    layout->addWidget(alignJustifyButton);

    alignPopupMenu->move(ui->alignMenuButton->mapToGlobal(QPoint(0, ui->alignMenuButton->height())));
    alignPopupMenu->setAttribute(Qt::WA_DeleteOnClose);
    QObject::connect(alignPopupMenu, &QWidget::destroyed, this, [this, cursor]() {
        this->ui->contentEdit->setFocus();
        delete cursor;
    });

    alignPopupMenu->show();
}

void NoteEditor::onAlignmentChanged()
{
    Qt::Alignment alignment = ui->contentEdit->alignment();
    if (alignment.testFlag(Qt::AlignLeft))
        ui->alignMenuButton->setIcon(QIcon(":/images/alignLeft.png"));
    else if (alignment.testFlag(Qt::AlignHCenter))
        ui->alignMenuButton->setIcon(QIcon(":/images/alignCenter.png"));
    else if (alignment.testFlag(Qt::AlignRight))
        ui->alignMenuButton->setIcon(QIcon(":/images/alignRight.png"));
    else if (alignment.testFlag(Qt::AlignJustify))
        ui->alignMenuButton->setIcon(QIcon(":/images/alignJustify.png"));
}

void NoteEditor::modifyIndentation(int amount)
{
    QTextCursor cursor = ui->contentEdit->textCursor();
    cursor.beginEditBlock();

    QTextBlockFormat blockFmt = cursor.blockFormat();
    blockFmt.setIndent(qMax(0, blockFmt.indent() + amount));
    cursor.setBlockFormat(blockFmt);

    cursor.endEditBlock();
}

void NoteEditor::switchListStyle(QTextListFormat::Style listFormat, QTextBlockFormat::MarkerType markerType)
{
    QTextCursor cursor = ui->contentEdit->textCursor();
    if (cursor.currentList() and cursor.currentList()->format().style() == listFormat and
        cursor.blockFormat().marker() == markerType)
    {
        QTextBlockFormat blockFmt;
        cursor.setBlockFormat(blockFmt);
    }
    else
    {
        cursor.createList(listFormat);
        QTextBlockFormat format = cursor.blockFormat();
        format.setMarker(markerType);
        cursor.setBlockFormat(format);
    }
    onListStyleChanged();
}

void NoteEditor::onListStyleChanged()
{
    QTextCursor cursor = ui->contentEdit->textCursor();
    for (auto &checkbox : ui->listCheckBoxes->buttons())
        static_cast<QCheckBox *>(checkbox)->setChecked(false);

    if (!cursor.currentList())
        return;

    if (cursor.blockFormat().marker() == QTextBlockFormat::MarkerType::NoMarker)
    {
        if (cursor.currentList()->format().style() == QTextListFormat::ListDisc)
            ui->unorderedListCheckBox->setChecked(true);
        else if (cursor.currentList()->format().style() == QTextListFormat::ListDecimal)
            ui->orderedListCheckBox->setChecked(true);
    }
    else
        ui->taskListCheckBox->setChecked(true);
}

void NoteEditor::onCurrentBlockFormatChanged()
{
    onAlignmentChanged();
    onListStyleChanged();
}

void NoteEditor::resizeEvent(QResizeEvent *event)
{
    QPoint rightTop = QPoint(event->size().width(), 0);
    closeButton->move(rightTop + QPoint(-closeButton->width() - 20, 20));
}

void NoteEditor::onCurrentCharFormatChanged(const QTextCharFormat &f)
{
    ui->boldCheckBox->setChecked(f.font().bold());
    ui->italicCheckBox->setChecked(f.font().italic());
    ui->underlineCheckBox->setChecked(f.font().underline());
    ui->strikeOutCheckBox->setChecked(f.font().strikeOut());
}
