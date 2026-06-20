#include "file_dialog.h"

#ifdef _WIN32

#include <windows.h>
#include <commdlg.h>
#include <stdio.h>
#include <stdbool.h>

/**
 * @brief Persistent dialog filename buffer.
 *
 * Windows common dialogs require the caller
 * to provide storage for the selected path.
 *
 * Returned pointers remain valid until the
 * next dialog call.
 */
static char filename[512];

/**
 * @brief Display a Windows file dialog.
 *
 * Shared implementation for Open/Save.
 *
 * @param save_mode
 * true  = Save dialog
 * false = Open dialog
 *
 * @return Selected path or NULL.
 */
static char* ShowDialog(
    bool save_mode
)
{
    OPENFILENAMEA dialog =
    {
        0
    };

    //--------------------------------
    // Reset output buffer
    //--------------------------------

    filename[0] =
        '\0';

    //--------------------------------
    // Dialog configuration
    //--------------------------------

    dialog.lStructSize =
        sizeof(dialog);

    dialog.hwndOwner =
        NULL;

    dialog.lpstrFile =
        filename;

    dialog.nMaxFile =
        sizeof(filename);

    dialog.lpstrFilter =
        "All Files\0*.*\0";

    dialog.Flags =
        OFN_EXPLORER;

    //--------------------------------
    // Mode specific flags
    //--------------------------------

    if (
        save_mode
    )
    {
        dialog.Flags |=
            OFN_OVERWRITEPROMPT;
    }
    else
    {
        dialog.Flags |=
            OFN_FILEMUSTEXIST
            |
            OFN_PATHMUSTEXIST;
    }

    //--------------------------------
    // Show dialog
    //--------------------------------

    BOOL success =
        save_mode
        ?
        GetSaveFileNameA(
            &dialog
        )
        :
        GetOpenFileNameA(
            &dialog
        );

    if (
        success
    )
    {
        return filename;
    }

    //--------------------------------
    // Print actual errors only
    //--------------------------------

    DWORD error =
        CommDlgExtendedError();

    if (
        error != 0
    )
    {
        printf(
            "Dialog Error: %lu\n",
            error
        );
    }

    return NULL;
}

/**
 * @brief Open file picker dialog.
 *
 * @return Selected file path or NULL.
 */
char* FileDialog_Open()
{
    return
        ShowDialog(
            false
        );
}

/**
 * @brief Open save file dialog.
 *
 * @return Selected save path or NULL.
 */
char* FileDialog_Save()
{
    return
        ShowDialog(
            true
        );
}

#endif