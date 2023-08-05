#ifndef FOLDERDATA_H
#define FOLDERDATA_H

#include <QString>
class FolderData
{
  public:
    FolderData();
    FolderData(int id, int parentId, const QString &name, int notesInsideCount = 0);
    void setId(int newId);
    void setParentId(int newParentId);
    void setName(const QString &newName);
    void setNotesInsideCount(int newNumberOfNotesInside);
    int getId() const;
    int getParentId() const;
    const QString &getName() const;
    int getNotesInsideCount() const;

  private:
    int id;
    QString name;
    int parentId;
    int notesInsideCount;
};

#endif // FOLDERDATA_H
