#include "file_dialog.h"

#ifdef _WIN32

#include <windows.h>
#include <commdlg.h>
#include <stdio.h>

static char filename[512];

char* FileDialog_Open()
{
    OPENFILENAMEA ofn =
    {
        0
    };

    filename[0] =
        '\0';

    ofn.lStructSize =
        sizeof(ofn);

    ofn.hwndOwner =
        NULL;

    ofn.lpstrFile =
        filename;

    ofn.nMaxFile =
        sizeof(filename);

    ofn.lpstrFilter =
        "All Files\0*.*\0";

    ofn.Flags =
        OFN_FILEMUSTEXIST
        |
        OFN_PATHMUSTEXIST
        |
        OFN_EXPLORER;

    BOOL result =
        GetOpenFileNameA(
            &ofn
        );

    if (
        result
    )
    {
        return filename;
    }

    printf(
        "Dialog Error: %lu\n",
        CommDlgExtendedError()
    );

    return NULL;
}

char* FileDialog_Save()
{
    OPENFILENAMEA ofn =
    {
        0
    };

    filename[0] =
        '\0';

    ofn.lStructSize =
        sizeof(ofn);

    ofn.hwndOwner =
        NULL;

    ofn.lpstrFile =
        filename;

    ofn.nMaxFile =
        sizeof(filename);

    ofn.lpstrFilter =
        "All Files\0*.*\0";

    ofn.Flags =
        OFN_OVERWRITEPROMPT
        |
        OFN_EXPLORER;

    BOOL result =
        GetSaveFileNameA(
            &ofn
        );

    if (
        result
    )
    {
        return filename;
    }

    printf(
        "Dialog Error: %lu\n",
        CommDlgExtendedError()
    );

    return NULL;
}

#endif