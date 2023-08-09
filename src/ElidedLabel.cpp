#include "ElidedLabel.h"

#include <QPainter>
#include <QTextLayout>

ElidedLabel::ElidedLabel(QWidget *parent, const QString &text) : QFrame(parent), elided(false), content(text)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setFrameShape(Shape::NoFrame);
    setStyleSheet("Border: none;");
}

void ElidedLabel::setText(const QString &newText)
{
    content = newText;
    update();
}

const QString &ElidedLabel::text() const
{
    return content;
}

bool ElidedLabel::isElided() const
{
    return elided;
}

void ElidedLabel::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);

    QPainter painter(this);
    painter.setFont(font);
    QFontMetrics fontMetrics = painter.fontMetrics();

    bool didElide = false;
    int lineSpacing = fontMetrics.lineSpacing();
    int y = 0;

    QTextLayout textLayout(content, painter.font());
    textLayout.beginLayout();
    forever
    {
        QTextLine line = textLayout.createLine();

        if (!line.isValid())
            break;

        line.setLineWidth(width());
        int nextLineY = y + lineSpacing;

        if (height() >= nextLineY + lineSpacing)
        {
            line.draw(&painter, QPoint(0, y));
            y = nextLineY;
        }
        else
        {
            QString lastLine = content.mid(line.textStart());
            QString elidedLastLine = fontMetrics.elidedText(lastLine, Qt::ElideRight, width());
            painter.drawText(QPoint(0, y + fontMetrics.ascent()), elidedLastLine);
            line = textLayout.createLine();
            didElide = line.isValid();
            break;
        }
    }

    if (didElide != elided)
    {
        elided = didElide;
        emit elisionChanged(didElide);
    }
}

QFont ElidedLabel::getFont() const
{
    return font;
}

void ElidedLabel::setFont(const QFont &newFont)
{
    font = newFont;
}
