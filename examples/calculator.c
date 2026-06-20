#include <stdio.h>
#include <string.h>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "../include/panel.h"
#include "../include/button.h"
#include "../include/label.h"
#include "menu_bar.h"

static Label display;

static char calcText[64] =
    "0";

static void Append(
    const char* t
)
{
    if (
        strcmp(
            calcText,
            "0"
        ) == 0
    )
    {
        calcText[0] =
            '\0';
    }

    strcat(
        calcText,
        t
    );

    display.text =
        calcText;
}

void Num0(){Append("0");}
void Num1(){Append("1");}
void Num2(){Append("2");}
void Num3(){Append("3");}
void Num4(){Append("4");}
void Num5(){Append("5");}
void Num6(){Append("6");}
void Num7(){Append("7");}
void Num8(){Append("8");}
void Num9(){Append("9");}

void Add(){Append("+");}
void Sub(){Append("-");}
void Mul(){Append("*");}
void Div(){Append("/");}

void Clear()
{
    strcpy(
        calcText,
        "0"
    );

    display.text =
        calcText;
}

void Equal()
{
    float a =
        0;

    float b =
        0;

    char op =
        '\0';

    if (
        sscanf(
            calcText,
            "%f%c%f",
            &a,
            &op,
            &b
        ) != 3
    )
    {
        strcpy(
            calcText,
            "ERROR"
        );

        display.text =
            calcText;

        return;
    }

    float result =
        0;

    switch (
        op
    )
    {
        case '+':
            result =
                a + b;
            break;

        case '-':
            result =
                a - b;
            break;

        case '*':
            result =
                a * b;
            break;

        case '/':
        {
            if (
                b == 0
            )
            {
                strcpy(
                    calcText,
                    "DIV0"
                );

                display.text =
                    calcText;

                return;
            }

            result =
                a / b;

            break;
        }

        default:
        {
            strcpy(
                calcText,
                "ERROR"
            );

            display.text =
                calcText;

            return;
        }
    }

    SDL_snprintf(
        calcText,
        sizeof(calcText),
        "%.2f",
        result
    );

    display.text =
        calcText;
}

static void SetupButton(
    Button* b,
    const char* text,
    TTF_Font* font,
    MenuCallback cb
)
{
    Button_Init(
        b,
        0,
        0,
        90,
        70,
        text,
        font,
        cb
    );

    //--------------------------------
    // IMPORTANT
    //--------------------------------

    ((Widget*)b)->fill_width =
        false;

    ((Widget*)b)->flex =
        0;
}

int main()
{
    SDL_Init(
        SDL_INIT_VIDEO
    );

    TTF_Init();

    SDL_Window* window =
        SDL_CreateWindow(
            "Calculator",
            450,
            600,
            0
        );

    SDL_Renderer* renderer =
        SDL_CreateRenderer(
            window,
            NULL
        );

    //--------------------------------

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
            28
        );

    //--------------------------------
    // Root
    //--------------------------------

    Panel root;

    Panel_Init(
        &root,
        0,
        0,
        450,
        600
    );

    Panel_SetLayout(
        &root,
        PANEL_VERTICAL
    );

    //--------------------------------
    // Display
    //--------------------------------

    Label_Init(
        &display,
        0,
        0,
        calcText,
        font
    );

    ((Widget*)&display)->fill_width =
        false;

    Panel_Add(
        &root,
        (Widget*)&display
    );

    //--------------------------------
    // Rows
    //--------------------------------

    Panel rows[4];

    for (
        int i=0;
        i<4;
        i++
    )
    {
        Panel_Init(
            &rows[i],
            0,
            0,
            420,
            80
        );

        Panel_SetLayout(
            &rows[i],
            PANEL_HORIZONTAL
        );

        rows[i].spacing =
            10;

        rows[i].padding =
            0;

        ((Widget*)&rows[i])->fill_width =
            false;

        Panel_Add(
            &root,
            (Widget*)&rows[i]
        );
    }

    //--------------------------------
    // Buttons
    //--------------------------------

    Button b[16];

    SetupButton(&b[0],"7",font,Num7);
    SetupButton(&b[1],"8",font,Num8);
    SetupButton(&b[2],"9",font,Num9);
    SetupButton(&b[3],"/",font,Div);

    SetupButton(&b[4],"4",font,Num4);
    SetupButton(&b[5],"5",font,Num5);
    SetupButton(&b[6],"6",font,Num6);
    SetupButton(&b[7],"*",font,Mul);

    SetupButton(&b[8],"1",font,Num1);
    SetupButton(&b[9],"2",font,Num2);
    SetupButton(&b[10],"3",font,Num3);
    SetupButton(&b[11],"-",font,Sub);

    SetupButton(&b[12],"C",font,Clear);
    SetupButton(&b[13],"0",font,Num0);
    SetupButton(&b[14],"=",font,Equal);
    SetupButton(&b[15],"+",font,Add);

    //--------------------------------
    // Add to rows
    //--------------------------------

    for (
        int i=0;
        i<4;
        i++
    )
    {
        for (
            int j=0;
            j<4;
            j++
        )
        {
            Panel_Add(
                &rows[i],
                (Widget*)&b[
                    i*4+j
                ]
            );
        }
    }

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