#include <stdio.h>
#include <stdbool.h>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "widget.h"
#include "window.h"
#include "button.h"
#include "label.h"
#include "textbox.h"
#include "menu_bar.h"

#include "file_dialog.h"

#define WINDOW_WIDTH   1200
#define WINDOW_HEIGHT  800
#define FONT_SIZE      30

/**
 * @brief Generic test button callback.
 */
static void OnButtonPress()
{
    printf(
        "BUTTON CLICKED\n"
    );
}

/**
 * @brief Menu → File → Open.
 */
static void OpenClick()
{
    char* file =
        FileDialog_Open();

    if (
        file &&
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

/**
 * @brief Menu → File → Save.
 */
static void SaveClick()
{
    char* file =
        FileDialog_Save();

    if (
        file
    )
    {
        printf(
            "SAVE:\n%s\n",
            file
        );
    }
}

/**
 * @brief Menu → File → Exit.
 */
static void ExitClick()
{
    printf(
        "EXIT\n"
    );
}

/**
 * @brief Load application font.
 *
 * @return Loaded font or NULL.
 */
static TTF_Font* LoadFont()
{
    char path[512];

    SDL_snprintf(
        path,
        sizeof(path),
        "%sPTC55F.ttf",
        SDL_GetBasePath()
    );

    printf(
        "Loading font:\n%s\n",
        path
    );

    return
        TTF_OpenFont(
            path,
            FONT_SIZE
        );
}

/**
 * @brief Build UI hierarchy.
 *
 * Window
 * ├── MenuBar
 * ├── Label
 * ├── Button
 * └── TextBox
 */
static void BuildUI(
    Window* desktop,
    TTF_Font* font
)
{
    //--------------------------------
    // Desktop
    //--------------------------------

    Window_Init(
        desktop,
        100,
        100,
        800,
        600,
        "Settings"
    );

    //--------------------------------
    // Menu
    //--------------------------------

    static MenuBar menu;

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
    );

    MenuBar_Add(
        &menu,
        "Edit",
        NULL
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
        desktop,
        (Widget*)&menu
    );

    //--------------------------------
    // Title
    //--------------------------------

    static Label title;

    Label_Init(
        &title,
        0,
        0,
        "SDL Desktop Toolkit",
        font
    );

    Window_Add(
        desktop,
        (Widget*)&title
    );

    //--------------------------------
    // Button
    //--------------------------------

    static Button button;

    Button_Init(
        &button,
        0,
        0,
        300,
        80,
        "PRESS",
        font,
        OnButtonPress
    );

    Widget_SetAlign(
        (Widget*)&button,
        ALIGN_CENTER,
        ALIGN_TOP
    );

    Window_Add(
        desktop,
        (Widget*)&button
    );

    //--------------------------------
    // Textbox
    //--------------------------------

    static TextBox textbox;

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
        desktop,
        (Widget*)&textbox
    );
}

/**
 * @brief Application entry point.
 */
int main(
    int argc,
    char* argv[]
)
{
    //--------------------------------
    // Initialize systems
    //--------------------------------

    if (
        !SDL_Init(
            SDL_INIT_VIDEO
        )
    )
    {
        return 1;
    }

    if (
        !TTF_Init()
    )
    {
        return 1;
    }

    //--------------------------------
    // Create SDL objects
    //--------------------------------

    SDL_Window* window =
        SDL_CreateWindow(
            "SDL Desktop",
            WINDOW_WIDTH,
            WINDOW_HEIGHT,
            0
        );

    SDL_Renderer* renderer =
        SDL_CreateRenderer(
            window,
            NULL
        );

    SDL_StartTextInput(
        window
    );

    //--------------------------------
    // Assets
    //--------------------------------

    TTF_Font* font =
        LoadFont();

    if (!font)
    {
        return 1;
    }

    //--------------------------------
    // UI
    //--------------------------------

    Window desktop;

    BuildUI(
        &desktop,
        font
    );

    //--------------------------------
    // Main loop
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

        SDL_SetRenderDrawColor(
            renderer,
            20,
            20,
            20,
            255
        );

        SDL_RenderClear(
            renderer);

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