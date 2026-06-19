#include <stdio.h>
#include <stdbool.h>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "widget.h"
#include "panel.h"
#include "label.h"
#include "button.h"
#include "textbox.h"

void OnButtonPress()
{
    printf(
        "HELLO BUTTON\n"
    );
}

int main(
    int argc,
    char* argv[]
)
{
    //--------------------------------
    // SDL Init
    //--------------------------------

    if (
        !SDL_Init(
            SDL_INIT_VIDEO
        )
    )
    {
        printf(
            "SDL Init Error: %s\n",
            SDL_GetError()
        );

        return 1;
    }

    if (
        !TTF_Init()
    )
    {
        printf(
            "TTF Init Error: %s\n",
            SDL_GetError()
        );

        return 1;
    }

    //--------------------------------
    // Window
    //--------------------------------

    SDL_Window* window =
        SDL_CreateWindow(
            "SDL Desktop Toolkit",
            1000,
            700,
            0
        );

    if (!window)
    {
        printf(
            "Window Error\n"
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
            "Renderer Error\n"
        );

        return 1;
    }

    //--------------------------------
    // Enable keyboard text input
    //--------------------------------

    SDL_StartTextInput(
        window
    );

    //--------------------------------
    // Load Font
    //--------------------------------

    char fontPath[512];

    SDL_snprintf(
        fontPath,
        sizeof(fontPath),
        "%sPTC55F.ttf",
        SDL_GetBasePath()
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
    // Root
    //--------------------------------

    Panel root;

    Panel_Init(
        &root,
        100,
        100,
        800,
        500
    );

    Panel_SetLayout(
        &root,
        PANEL_VERTICAL
    );

    Panel_EnableScroll(
    &root
    );

    // New scrollable list of things 
    Label rows[20];

    for (
        int i = 0;
        i < 20;
        i++
    )
    {
        char* txt =
            SDL_malloc(
                64
            );

        SDL_snprintf(
            txt,
            64,
            "Row %d",
            i
        );

        Label_Init(
            &rows[i],
            0,
            0,
            txt,
            font
        );

        Panel_Add(
            &root,
            (Widget*)&rows[i]
        );
    }

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
    Widget_SetAlign(
    (Widget*)&title,
    ALIGN_CENTER,
    ALIGN_TOP
    );

    Panel_Add(
        &root,
        (Widget*)&title
    );

    //--------------------------------
    // Button Row
    //--------------------------------

    Panel row;

    Panel_Init(
        &row,
        0,
        0,
        700,
        100
    );

    Panel_SetLayout(
        &row,
        PANEL_HORIZONTAL
    );

    //--------------------------------
    // Buttons
    //--------------------------------

    Button b1;

    Button_Init(
        &b1,
        0,
        0,
        160,
        60,
        "Open",
        font,
        OnButtonPress
    );

    Panel_Add(
        &row,
        (Widget*)&b1
    );

    Button b2;

    Button_Init(
        &b2,
        0,
        0,
        160,
        60,
        "Save",
        font,
        OnButtonPress
    );

    Panel_Add(
        &row,
        (Widget*)&b2
    );

    Button b3;

    Button_Init(
        &b3,
        0,
        0,
        160,
        60,
        "Export",
        font,
        OnButtonPress
    );

    Panel_Add(
        &row,
        (Widget*)&b3
    );

    //--------------------------------
    // Add row to root
    //--------------------------------

    Panel_Add(
        &root,
        (Widget*)&row
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

    Widget_SetFillWidth(
        (Widget*)&textbox
    );

    Panel_Add(
        &root,
        (Widget*)&textbox
    );

    Widget_SetAlign(
    (Widget*)&row,
    ALIGN_RIGHT,
    ALIGN_TOP
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
                (Widget*)&root,
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
            (Widget*)&root,
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