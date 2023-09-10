#ifndef SPECIALFOLDERID_H
#define SPECIALFOLDERID_H

struct SpecialFolderId
{
    enum Id
    {
        InvalidId = -10,
        RootFolder = 1,
        AllNotesFolder = 2,
        TrashFolder = 3,
        UserFolder = 4 // All folders added by user will have id bigger than this
    };
};

#endif // SPECIALFOLDERID_H
