#ifndef FOLDERTREEMODELROLE_H
#define FOLDERTREEMODELROLE_H
#include <qnamespace.h>

struct FolderTreeModelRole
{ // We don't want use enum class, because we would need to make explicit conversion to int every time we use it, since
  // Qt treats role as int in models. We still want scoped enum though.
    enum Role
    {
        DisplayRole = Qt::DisplayRole,
        EditRole = Qt::EditRole,
        Id = Qt::UserRole,
        ParentId,
        PreviousFolderId,
        Name,
        NotesInsideCount
    };
};

#endif // FOLDERTREEMODELROLE_H
