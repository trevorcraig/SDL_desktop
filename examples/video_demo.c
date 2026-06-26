#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#include "panel.h"
#include "video_player.h"
#include "menu_bar.h"
#include "file_dialog.h"

#include <stdio.h>

#define FONT_SIZE 30

static VideoPlayer* gPlayer =
    NULL;
static SDL_Renderer* gRenderer =
    NULL;

void OpenClick()
{
    char* file =
        FileDialog_Open();

    if (
        file
        &&
        file[0]
    )
    {
        printf(
            "Loading:\n%s\n",
            file
        );

        VideoPlayer_Load(
            gPlayer,
            gRenderer,
            file
        );
    }
}


/**
 * @brief Resume playback.
 */
void PlayClick()
{
    VideoPlayer_Play(
        gPlayer
    );
}


/**
 * @brief Pause playback.
 */
void StopClick()
{
    VideoPlayer_Stop(
        gPlayer
    );
}


/**
 * @brief Double playback speed.
 */
void FastClick()
{
    VideoPlayer_SetSpeed(
        gPlayer,
        2.0f
    );
}


/**
 * @brief Return to normal speed.
 */
void NormalClick()
{
    VideoPlayer_SetSpeed(
        gPlayer,
        1.0f
    );
}


/**
 * @brief Load UI font.
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


int main()
{
    SDL_Init(
        SDL_INIT_VIDEO
    );

    TTF_Init();


    //--------------------------------
    // Window
    //--------------------------------

    SDL_Window* win =
        SDL_CreateWindow(
            "Video Demo",
            1280,
            720,
            0
        );

    SDL_Renderer* r =
        SDL_CreateRenderer(
            win,
            NULL
        );
    gRenderer =r;

    //--------------------------------
    // Root panel
    //--------------------------------

    Panel root;

    Panel_Init(
        &root,
        0,
        0,
        1280,
        720
    );

    Panel_SetLayout(
        &root,
        PANEL_VERTICAL
    );

    root.padding =
        0;

    root.spacing =
        0;


    //--------------------------------
    // Font
    //--------------------------------

    TTF_Font* font =
        LoadFont();


    //--------------------------------
    // Menu
    //--------------------------------

    static MenuBar menu;

    MenuBar_Init(
        &menu,
        0,
        0,
        1280,
        36,
        font
    );

    Widget_SetFillWidth(
        (Widget*)&menu
    );
    MenuBar_Add(
        &menu,
        "Open",
        OpenClick
    );
    MenuBar_Add(
        &menu,
        "Stop",
        StopClick
    );

    MenuBar_Add(
        &menu,
        "Play",
        PlayClick
    );

    MenuBar_Add(
        &menu,
        "Fast",
        FastClick
    );

    MenuBar_Add(
        &menu,
        "Normal",
        NormalClick
    );

    Panel_Add(
        &root,
        (Widget*)&menu
    );


    //--------------------------------
    // Video
    //--------------------------------

    VideoPlayer player =
        {0};

    VideoPlayer_Init(
        &player,
        r,
        0,
        36,
        1280,
        684,
        NULL
    );

    gPlayer =
        &player;

    Panel_Add(
        &root,
        (Widget*)&player
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
                (Widget*)&root,
                &e
            );
        }


        //--------------------------------
        // Draw
        //--------------------------------

        SDL_SetRenderDrawColor(
            r,
            20,
            20,
            20,
            255
        );

        SDL_RenderClear(
            r
        );

        Widget_Render(
            (Widget*)&root,
            r
        );

        SDL_RenderPresent(
            r
        );
    }


    //--------------------------------
    // Cleanup
    //--------------------------------

    VideoPlayer_Destroy(
        &player
    );

    TTF_CloseFont(
        font
    );

    SDL_DestroyRenderer(
        r
    );

    SDL_DestroyWindow(
        win
    );

    TTF_Quit();

    SDL_Quit();

    return 0;
}