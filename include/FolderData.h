#ifndef FOLDERDATA_H
#define FOLDERDATA_H

#include <QColor>
#include <QString>
class FolderData
{
  public:
    FolderData();
    FolderData(int id, int parentId, int previousFolderId, const QString &name, const QColor &color = QColor(0, 0, 0),
               int notesInsideCount = 0);
    void setId(int newId);
    void setParentId(int newParentId);
    void setName(const QString &newName);
    void setNotesInsideCount(int newNumberOfNotesInside);
    void setPreviousFolderId(int newPreviousFolderId);
    void setColor(const QColor &newColor);
    int getId() const;
    int getParentId() const;
    const QString &getName() const;
    int getNotesInsideCount() const;
    int getPreviousFolderId() const;
    QColor getColor() const;

  private:
    int id;
    int parentId;
    int previousFolderId;
    QString name;
    QColor color;
    int notesInsideCount;
};

#endif // FOLDERDATA_H
