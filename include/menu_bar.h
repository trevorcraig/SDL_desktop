#ifndef MENU_BAR_H
#define MENU_BAR_H

#include "widget.h"

#include <SDL3_ttf/SDL_ttf.h>

#define MENU_MAX 16
#define DROPDOWN_MAX 16

typedef void (*MenuCallback)(void);

typedef struct
{
    const char* text;

    MenuCallback callback;

} DropdownItem;

typedef struct
{
    const char* text;

    SDL_FRect rect;

    bool hover;

    bool open;

    MenuCallback callback;

    DropdownItem dropdown[
        DROPDOWN_MAX
    ];

    int dropdown_count;

} MenuItem;

typedef struct
{
    Widget base;

    TTF_Font* font;

    MenuItem items[
        MENU_MAX
    ];

    int count;

} MenuBar;

void MenuBar_Init(
    MenuBar*,
    float,
    float,
    float,
    float,
    TTF_Font*
);

void MenuBar_Add(
    MenuBar*,
    const char*,
    MenuCallback
);

void MenuBar_AddDropdown(
    MenuBar*,
    int menu,
    const char*,
    MenuCallback
);

void MenuBar_Handle(
    Widget*,
    SDL_Event*
);

#endif