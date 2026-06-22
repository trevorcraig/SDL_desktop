#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "panel.h"
#include "button.h"
#include "label.h"
#include "textbox.h"
#include "menu_bar.h"

#define FILE_COUNT 50

//--------------------------------------
// Globals
//--------------------------------------

static char selected[128] =
    "Nothing selected";

static Label selectedLabel;

//--------------------------------------
// Callbacks
//--------------------------------------

void SelectFile0(){ selectedLabel.text="Clicked File_00.txt"; }
void SelectFile1(){ selectedLabel.text="Clicked File_01.txt"; }
void SelectFile2(){ selectedLabel.text="Clicked File_02.txt"; }
void SelectFile3(){ selectedLabel.text="Clicked File_03.txt"; }
void SelectFile4(){ selectedLabel.text="Clicked File_04.txt"; }

static void Empty()
{
}

//--------------------------------------
// Main
//--------------------------------------

int main()
{
    SDL_Init(
        SDL_INIT_VIDEO
    );

    TTF_Init();

    SDL_Window* window =
        SDL_CreateWindow(
            "Scrolling File Browser",
            700,
            700,
            0
        );

    SDL_Renderer* renderer =
        SDL_CreateRenderer(
            window,
            NULL
        );

    //----------------------------------
    // Font
    //----------------------------------

    char fontPath[512];

    SDL_snprintf(
        fontPath,
        sizeof(fontPath),
        "%sPTC55F.ttf",
        SDL_GetBasePath()
    );

    TTF_Font* font =
        TTF_OpenFont(
            fontPath,
            24
        );

    //----------------------------------
    // Root Panel
    //----------------------------------

    Panel root;

    Panel_Init(
        &root,
        0,
        0,
        700,
        700
    );

    Panel_SetLayout(
        &root,
        PANEL_VERTICAL
    );

    Panel_EnableScroll(
        &root
    );

    //----------------------------------
    // Header
    //----------------------------------

    Label title;

    Label_Init(
        &title,
        0,
        0,
        "File Browser Example",
        font
    );

    Panel_Add(
        &root,
        (Widget*)&title
    );

    //----------------------------------
    // Selected Label
    //----------------------------------

    Label_Init(
        &selectedLabel,
        0,
        0,
        selected,
        font
    );

    Panel_Add(
        &root,
        (Widget*)&selectedLabel
    );

    //----------------------------------
    // File Buttons
    //----------------------------------

    static Button files[
        FILE_COUNT
    ];

    MenuCallback callbacks[5] =
    {
        SelectFile0,
        SelectFile1,
        SelectFile2,
        SelectFile3,
        SelectFile4
    };

    static char names
    [
        FILE_COUNT
    ][64];

    for (
        int i=0;
        i<FILE_COUNT;
        i++
    )
    {
        SDL_snprintf(
            names[i],
            sizeof(
                names[i]
            ),
            "File_%02d.txt",
            i
        );

        Button_Init(
            &files[i],
            0,
            0,
            620,
            60,
            names[i],
            font,
            (
                i < 5
            )
            ?
            callbacks[i]
            :
            Empty
        );

        Widget_SetAlign(
            (Widget*)
            &files[i],
            ALIGN_FILL,
            ALIGN_TOP
        );

        Panel_Add(
            &root,
            (Widget*)
            &files[i]
        );
    }

    //----------------------------------
    // Main Loop
    //----------------------------------

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
                e.type
                ==
                SDL_EVENT_QUIT
            )
            {
                running =
                    false;
            }

            Widget_Handle(
                (Widget*)
                &root,
                &e
            );
        }

        SDL_SetRenderDrawColor(
            renderer,
            25,
            25,
            25,
            255
        );

        SDL_RenderClear(
            renderer
        );

        Widget_Render(
            (Widget*)
            &root,
            renderer
        );

        SDL_RenderPresent(
            renderer
        );
    }

    //----------------------------------
    // Cleanup
    //----------------------------------

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