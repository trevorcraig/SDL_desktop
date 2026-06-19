#include <stdio.h>
#include <stdbool.h>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "../include/widget.h"
#include "../include/window.h"
#include "../include/label.h"
#include "../include/button.h"
#include "../include/textbox.h"
#include "../include/menu_bar.h"
#include "file_dialog.h"

void OnButtonPress()
{
    printf(
        "BUTTON CLICKED\n"
    );
}

void FileClick()
{
    printf(
        "File\n"
    );
}

void EditClick()
{
    printf(
        "Edit\n"
    );
}

void OpenClick()
{
    printf(
        "Save\n"
    );
    char* file =
        FileDialog_Open();

    if (
        file
        &&
        file[0]
    )
    {
        printf(
            "OPEN:\n%s\n",
            file
        );
    }
    else
    {
        printf(
            "Dialog cancelled\n"
        );
    }
}

void SaveClick()
{
    // printf(
    //     "Save\n"
    // );
    char* file =
        FileDialog_Save();

    if (file)
    {
        printf(
            "SAVE:\n%s\n",
            file
        );
    }
}

void ExitClick()
{
    printf(
        "EXIT\n"
    );
}

int main(
    int argc,
    char* argv[]
)
{
    //--------------------------------
    // Init SDL
    //--------------------------------

    if (
        !SDL_Init(
            SDL_INIT_VIDEO
        )
    )
    {
        printf(
            "SDL Error: %s\n",
            SDL_GetError()
        );

        return 1;
    }

    if (
        !TTF_Init()
    )
    {
        printf(
            "TTF Error: %s\n",
            SDL_GetError()
        );

        return 1;
    }

    //--------------------------------
    // Window
    //--------------------------------

    SDL_Window* window =
        SDL_CreateWindow(
            "SDL Desktop",
            1200,
            800,
            0
        );

    if (!window)
    {
        printf(
        "Window Error: %s\n",
        SDL_GetError()
        );

        return 1;
    }

    SDL_Renderer* renderer =
        SDL_CreateRenderer(
            window,
            NULL
        );

    if (!renderer)
    {
        printf(
        "Renderer Error: %s\n",
        SDL_GetError()
        );

        return 1;
    }

    SDL_StartTextInput(
        window
    );

    //--------------------------------
    // Load Font
    //--------------------------------

    char fontPath[512];

    const char* base =
        SDL_GetBasePath();

    SDL_snprintf(
        fontPath,
        sizeof(fontPath),
        "%sPTC55F.ttf",
        base
    );


    printf(
        "Loading font:\n%s\n",
        fontPath
    );

    TTF_Font* font =
        TTF_OpenFont(
            fontPath,
            30
        );

    if (!font)
    {
        printf(
            "Font Error: %s\n",
            SDL_GetError()
        );

        return 1;
    }

    //--------------------------------
    // Desktop Window
    //--------------------------------

    Window desktop;

    Window_Init(
        &desktop,
        100,
        100,
        800,
        600,
        "Settings"
    );

    MenuBar menu;

    MenuBar_Init(
        &menu,
        0,
        0,
        700,
        36,
        font
    );

    MenuBar_Add(
        &menu,
        "File",
        NULL
        // FileClick
    );

    MenuBar_Add(
        &menu,
        "Edit",
        EditClick
    );

    MenuBar_Add(
        &menu,
        "View",
        NULL
    );

    MenuBar_Add(
        &menu,
        "Help",
        NULL
    );

    MenuBar_AddDropdown(
        &menu,
        0,
        "Open",
        OpenClick
    );

    MenuBar_AddDropdown(
        &menu,
        0,
        "Save",
        SaveClick
    );

    MenuBar_AddDropdown(
        &menu,
        0,
        "Exit",
        ExitClick
    );

    Window_Add(
        &desktop,
        (Widget*)&menu
    );

    //--------------------------------
    // Title
    //--------------------------------

    Label title;

    Label_Init(
        &title,
        0,
        0,
        "SDL Desktop Toolkit",
        font
    );

    Window_Add(
        &desktop,
        (Widget*)&title
    );

    //--------------------------------
    // Button
    //--------------------------------

    Button button;

    Button_Init(
        &button,
        0,
        0,
        300,
        80,
        "PRESS",
        font,
        // OpenClick
        OnButtonPress
    );

    Widget_SetAlign(
        (Widget*)&button,
        ALIGN_CENTER,
        ALIGN_TOP
    );

    Window_Add(
        &desktop,
        (Widget*)&button
    );

    //--------------------------------
    // Textbox
    //--------------------------------

    TextBox textbox;

    TextBox_Init(
        &textbox,
        0,
        0,
        400,
        60,
        font
    );

    Widget_SetAlign(
        (Widget*)&textbox,
        ALIGN_FILL,
        ALIGN_TOP
    );

    Window_Add(
        &desktop,
        (Widget*)&textbox
    );

    //--------------------------------
    // Main Loop
    //--------------------------------

    bool running =
        true;

    while (
        running
    )
    {
        SDL_Event e;

        while (
            SDL_PollEvent(
                &e
            )
        )
        {
            if (
                e.type ==
                SDL_EVENT_QUIT
            )
            {
                running =
                    false;
            }

            Widget_Handle(
                (Widget*)&desktop,
                &e
            );
        }

        //--------------------------------
        // Draw
        //--------------------------------

        SDL_SetRenderDrawColor(
            renderer,
            20,
            20,
            20,
            255
        );

        SDL_RenderClear(
            renderer
        );

        Widget_Render(
            (Widget*)&desktop,
            renderer
        );

        SDL_RenderPresent(
            renderer
        );
    }

    //--------------------------------
    // Cleanup
    //--------------------------------

    SDL_StopTextInput(
        window
    );

    TTF_CloseFont(
        font
    );

    SDL_DestroyRenderer(
        renderer
    );

    SDL_DestroyWindow(
        window
    );

    TTF_Quit();

    SDL_Quit();

    return 0;
}