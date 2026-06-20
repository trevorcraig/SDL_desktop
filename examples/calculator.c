/**
 * @file calculator.c
 * @brief Example calculator application using the SDL desktop toolkit.
 *
 * Demonstrates:
 * - Nested panels
 * - Horizontal + vertical layouts
 * - Button callbacks
 * - Dynamic label updates
 */

#include <stdio.h>
#include <string.h>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "panel.h"
#include "button.h"
#include "label.h"
#include "menu_bar.h"

/*----------------------------------------------------------
Constants
----------------------------------------------------------*/

#define WINDOW_WIDTH   450
#define WINDOW_HEIGHT  600

#define BUTTON_WIDTH   90
#define BUTTON_HEIGHT  70

#define ROW_COUNT      4
#define COL_COUNT      4

/*----------------------------------------------------------
Calculator State
----------------------------------------------------------*/

/** Output display widget */
static Label display;

/** Current calculator text */
static char calcText[64] =
    "0";

/*----------------------------------------------------------
Display Helpers
----------------------------------------------------------*/

/**
 * @brief Updates the calculator display.
 *
 * @param text New display text.
 */
static void SetDisplay(
    const char* text
)
{
    strncpy(
        calcText,
        text,
        sizeof(calcText)
    );

    calcText[
        sizeof(calcText)-1
    ] =
        '\0';

    display.text =
        calcText;
}

/**
 * @brief Appends text to the display.
 *
 * @param text Text to append.
 */
static void Append(
    const char* text
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

    strncat(
        calcText,
        text,
        sizeof(calcText)
        -
        strlen(calcText)
        -
        1
    );

    display.text =
        calcText;
}

/**
 * @brief Clears calculator input.
 */
static void Clear()
{
    SetDisplay(
        "0"
    );
}

/*----------------------------------------------------------
Button Callbacks
----------------------------------------------------------*/

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

/*----------------------------------------------------------
Expression Evaluation
----------------------------------------------------------*/

/**
 * @brief Evaluates simple expressions:
 *
 * Supported:
 *  a+b
 *  a-b
 *  a*b
 *  a/b
 */
static void Equal()
{
    float left =
        0;

    float right =
        0;

    char op =
        '\0';

    if (
        sscanf(
            calcText,
            "%f%c%f",
            &left,
            &op,
            &right
        )
        !=
        3
    )
    {
        SetDisplay(
            "ERROR"
        );

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
                left +
                right;
            break;

        case '-':
            result =
                left -
                right;
            break;

        case '*':
            result =
                left *
                right;
            break;

        case '/':

            if (
                right == 0
            )
            {
                SetDisplay(
                    "DIV0"
                );

                return;
            }

            result =
                left /
                right;

            break;

        default:

            SetDisplay(
                "ERROR"
            );

            return;
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

/*----------------------------------------------------------
Button Creation
----------------------------------------------------------*/

/**
 * @brief Initializes a calculator button.
 *
 * @param button Button to initialize.
 * @param text Button label.
 * @param font UI font.
 * @param callback Click callback.
 */
static void SetupButton(
    Button* button,
    const char* text,
    TTF_Font* font,
    MenuCallback callback
)
{
    Button_Init(
        button,
        0,
        0,
        BUTTON_WIDTH,
        BUTTON_HEIGHT,
        text,
        font,
        callback
    );

    Widget_SetAlign(
        (Widget*)button,
        ALIGN_LEFT,
        ALIGN_TOP
    );
}

/*----------------------------------------------------------
Main
----------------------------------------------------------*/

int main()
{
    SDL_Init(
        SDL_INIT_VIDEO
    );

    TTF_Init();

    SDL_Window* window =
        SDL_CreateWindow(
            "Calculator",
            WINDOW_WIDTH,
            WINDOW_HEIGHT,
            0
        );

    SDL_Renderer* renderer =
        SDL_CreateRenderer(
            window,
            NULL
        );

    //--------------------------------
    // Load font
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
    // Root layout
    //--------------------------------

    Panel root;

    Panel_Init(
        &root,
        0,
        0,
        WINDOW_WIDTH,
        WINDOW_HEIGHT
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

    Panel_Add(
        &root,
        (Widget*)&display
    );

    //--------------------------------
    // Create rows
    //--------------------------------

    Panel rows[
        ROW_COUNT
    ];

    for (
        int i=0;
        i<ROW_COUNT;
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

        rows[i].padding =
            0;

        rows[i].spacing =
            10;

        Panel_Add(
            &root,
            (Widget*)&rows[i]
        );
    }

    //--------------------------------
    // Create buttons
    //--------------------------------

    Button buttons[16];

    SetupButton(&buttons[0],"7",font,Num7);
    SetupButton(&buttons[1],"8",font,Num8);
    SetupButton(&buttons[2],"9",font,Num9);
    SetupButton(&buttons[3],"/",font,Div);

    SetupButton(&buttons[4],"4",font,Num4);
    SetupButton(&buttons[5],"5",font,Num5);
    SetupButton(&buttons[6],"6",font,Num6);
    SetupButton(&buttons[7],"*",font,Mul);

    SetupButton(&buttons[8],"1",font,Num1);
    SetupButton(&buttons[9],"2",font,Num2);
    SetupButton(&buttons[10],"3",font,Num3);
    SetupButton(&buttons[11],"-",font,Sub);

    SetupButton(&buttons[12],"C",font,Clear);
    SetupButton(&buttons[13],"0",font,Num0);
    SetupButton(&buttons[14],"=",font,Equal);
    SetupButton(&buttons[15],"+",font,Add);

    //--------------------------------
    // Place buttons
    //--------------------------------

    for (
        int row=0;
        row<ROW_COUNT;
        row++
    )
    {
        for (
            int col=0;
            col<COL_COUNT;
            col++
        )
        {
            Panel_Add(
                &rows[row],
                (Widget*)
                &buttons[
                    row
                    *
                    COL_COUNT
                    +
                    col
                ]
            );
        }
    }

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