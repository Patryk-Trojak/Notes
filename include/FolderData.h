#ifndef FOLDERDATA_H
#define FOLDERDATA_H

#include <QString>

class FolderData
{
  public:
    FolderData();
    void setId(int newId);
    void setParentId(int newParentId);
    void setName(const QString &newName);
    int getId() const;
    int getParentId() const;
    const QString &getName() const;

  private:
    int id;
    QString name;
    int parentId;
};

#endif // FOLDERDATA_H
