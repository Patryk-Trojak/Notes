#include "NoteEditor.h"
#include "./ui_NoteEditor.h"
#include "NoteListModelRole.h"
#include "Utils.h"
#include <QColorDialog>
#include <QDateTime>
#include <QFontInfo>
#include <QResizeEvent>
#include <QTextDocument>
#include <QTextList>
NoteEditor::NoteEditor(const QModelIndex &editingNote, QWidget *parent)
    : QWidget(parent), ui(new Ui::NoteEditor), fakeTextCursor(new QWidget())
{
    ui->setupUi(this);
    QString style = QString("QWidget#NoteEditor{border-style: solid; border-color: transparent; border-width:10px; "
                            "border-radius:30px;} "
                            "QWidget{ background-color: %1;}"
                            "QLineEdit{background-color: transparent; border: none;}"
                            "QTextEdit{background-color: transparent; border: none;}")
                        .arg(editingNote.data(NoteListModelRole::Color).value<QColor>().name());
    fakeTextCursor->setParent(ui->contentEdit);
    fakeTextCursor->setStyleSheet({"background-color: black;"});
    fakeTextCursor->setAttribute(Qt::WA_StyledBackground);
    fakeTextCursor->hide();
    ui->fontSizeSpinBox->setAlignment(Qt::AlignHCenter);
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
    QObject::connect(ui->fontSizeSpinBox, &QSpinBox::valueChanged, this, &NoteEditor::setFontSize);
    QObject::connect(ui->unorderedListCheckBox, &QPushButton::clicked, this, [this] {
        this->switchListStyle(QTextListFormat::ListDisc, QTextBlockFormat::MarkerType::NoMarker);
    });
    QObject::connect(ui->orderedListCheckBox, &QPushButton::clicked, this, [this] {
        this->switchListStyle(QTextListFormat::ListDecimal, QTextBlockFormat::MarkerType::NoMarker);
    });
    QObject::connect(ui->taskListCheckBox, &QPushButton::clicked, this, [this] {
        this->switchListStyle(QTextListFormat::ListDisc, QTextBlockFormat::MarkerType::Unchecked);
    });
    QObject::connect(ui->fontFamilyComboBox, &QComboBox::textActivated, this, &NoteEditor::setFontFamily);

    ui->contentEdit->installEventFilter(this);
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

void NoteEditor::onTextColorChanged()
{
    QImage textColorButtonIcon(":/images/textColor");
    myUtils::replaceColor(textColorButtonIcon, Qt::white, QColor(ui->contentEdit->textColor()));
    ui->textColorButton->setIcon(QPixmap::fromImage(textColorButtonIcon));
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
    QObject::connect(alignPopupMenu, &QWidget::destroyed, this, [this]() { this->ui->contentEdit->setFocus(); });

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

    if (cursor.currentList())
    {
        QTextListFormat listFmt = cursor.currentList()->format();
        QTextCursor above(cursor);
        above.movePosition(QTextCursor::Up);
        QTextCursor below(cursor);
        below.movePosition(QTextCursor::Down);
        if (above.currentList() && listFmt.indent() + amount == above.currentList()->format().indent())
        {
            above.currentList()->add(cursor.block());
            cursor.setBlockFormat(above.blockFormat());
        }
        else if (below.currentList() && listFmt.indent() + amount == below.currentList()->format().indent())
        {
            below.currentList()->add(cursor.block());
            cursor.setBlockFormat(below.blockFormat());
        }
        else
        {
            listFmt.setIndent(qMax(1, listFmt.indent() + amount));
            cursor.createList(listFmt);
        }
    }
    else
    {
        QTextBlockFormat blockFmt = cursor.blockFormat();
        blockFmt.setIndent(qMax(0, blockFmt.indent() + amount));
        cursor.setBlockFormat(blockFmt);
    }

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

void NoteEditor::setFontSize()
{
    QTextCharFormat fmt;
    fmt.setFontPointSize(ui->fontSizeSpinBox->value());
    ui->contentEdit->mergeCurrentCharFormat(fmt);
}

void NoteEditor::setFontFamily(const QString &fontFamily)
{
    QTextCharFormat fmt;
    fmt.setFontFamilies({fontFamily});
    ui->contentEdit->mergeCurrentCharFormat(fmt);
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

bool NoteEditor::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->contentEdit)
    {
        if (event->type() == QEvent::KeyPress and ui->contentEdit->textCursor().atBlockStart())
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Tab)
            {
                this->modifyIndentation(1);
                return true;
            }
        }
        if (event->type() == QEvent::FocusOut and !ui->titleEdit->hasFocus())
        {
            fakeTextCursor->setGeometry(ui->contentEdit->cursorRect());
            fakeTextCursor->show();
        }
        if (event->type() == QEvent::FocusIn)
        {
            fakeTextCursor->hide();
        }
    }
    return QWidget::eventFilter(watched, event);
}

void NoteEditor::onCurrentCharFormatChanged(const QTextCharFormat &f)
{
    ui->boldCheckBox->setChecked(f.font().bold());
    ui->italicCheckBox->setChecked(f.font().italic());
    ui->underlineCheckBox->setChecked(f.font().underline());
    ui->strikeOutCheckBox->setChecked(f.font().strikeOut());
    QSignalBlocker blocker(ui->fontSizeSpinBox);
    ui->fontSizeSpinBox->setValue(f.font().pointSize());
    ui->fontFamilyComboBox->setCurrentIndex(ui->fontFamilyComboBox->findText(QFontInfo(f.font()).family()));
    fakeTextCursor->setGeometry(ui->contentEdit->cursorRect());
    onTextColorChanged();
}
