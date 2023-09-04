#ifndef FOLDERDATA_H
#define FOLDERDATA_H

#include <QString>
class FolderData
{
  public:
    FolderData();
    FolderData(int id, int parentId, int previousFolderId, const QString &name, int notesInsideCount = 0);
    void setId(int newId);
    void setParentId(int newParentId);
    void setName(const QString &newName);
    void setNotesInsideCount(int newNumberOfNotesInside);
    void setPreviousFolderId(int newPreviousFolderId);
    int getId() const;
    int getParentId() const;
    const QString &getName() const;
    int getNotesInsideCount() const;
    int getPreviousFolderId() const;

  private:
    int id;
    int parentId;
    int previousFolderId;
    QString name;
    int notesInsideCount;
};

#endif // FOLDERDATA_H
