#ifndef NOTEBUTTON_H
#define NOTEBUTTON_H
#include "ColorPicker.h"

#include <QCheckBox>
#include <QDateTime>
#include <QTextEdit>
#include <QWidget>
#include <QtWidgets/QPushButton>

namespace Ui
{
class NoteButton;
}

class NoteButton : public QPushButton
{
    Q_OBJECT
  public:
    explicit NoteButton(QWidget *parent);
    explicit NoteButton(const QString &title, const QDateTime &modificationTime, bool isPinned,
                        QWidget *parent = nullptr);
    ~NoteButton();

    void setTitle(const QString &title);
    void setContent(const QString &content);
    void setModificationTime(const QDateTime &newModificationTime);
    void setIsPinned(bool newIsPinned);
    void setPinCheckboxVisible(bool visible);
    void setColor(const QColor &newColor);
    void setIsSelected(bool newIsSelected);

    QString getTitle() const;
    QString getContent() const;
    const QDateTime &getModificationTime() const;
    bool getIsPinned() const;
    const QColor &getColor() const;
    bool getIsSelected() const;

  signals:
    void deleteNote();
    void pinCheckboxToogled(bool checked);
    void colorChanged(const QColor &newColor);

  private:
    Ui::NoteButton *ui;
    QPushButton *deleteButton;
    QPushButton *changeColorButton;
    ColorPicker *colorPicker;
    QCheckBox *pinCheckBox;
    QDateTime modificationTime;
    QColor color;
    bool isSelected;

    QString convertDateTimeToString(const QDateTime &dateTime);
    void createColorPicker();
    void deleteColorPicker();
    void onColorSelected(const QColor &color);
    void onChangeColorButtonClicked();
    void updateStyleSheets();

  protected:
    void resizeEvent(QResizeEvent *event);
};
#endif // NOTEBUTTON_H
