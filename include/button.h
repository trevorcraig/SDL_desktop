#ifndef BUTTON_H
#define BUTTON_H

#include "widget.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

typedef void (*ButtonCallback)(void);

typedef struct
{
    Widget base;

    bool hovered;

    bool pressed;

    const char* text;

    TTF_Font* font;

    SDL_Color normal;

    SDL_Color hover;

    SDL_Color click;

    ButtonCallback callback;

} Button;

void Button_Init(
    Button* btn,
    float x,
    float y,
    float w,
    float h,
    const char* text,
    TTF_Font* font,
    ButtonCallback callback
);

#endif