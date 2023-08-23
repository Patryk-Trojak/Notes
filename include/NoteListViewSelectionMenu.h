#ifndef NOTELISTVIEWSELECTIONMENU_H
#define NOTELISTVIEWSELECTIONMENU_H

#include "ColorPicker.h"

#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

class NoteListViewSelectionMenu : public QWidget
{
    Q_OBJECT

  public:
    explicit NoteListViewSelectionMenu(QWidget *parent = nullptr);
    void setColorPickerVisible(bool visible);
    void toogleColorPickerVisible();
    void setSelectedNotesCount(int selectedNotesCount);

  signals:
    void deleteButtonClicked();
    void colorSelected(const QColor &newColor);
    void pinButtonClicked();
    void cancelButtonClicked();

  private:
    QPushButton *deleteButton;
    QPushButton *changeColorButton;
    QPushButton *pinButton;
    QPushButton *cancelButton;
    QLabel *selectedNotesCountLabel;
    ColorPicker *colorPicker;
    QPushButton *createButton(const QString &iconPath);

  protected:
    void resizeEvent(QResizeEvent *event);
};

#endif // NOTELISTVIEWSELECTIONMENU_H
