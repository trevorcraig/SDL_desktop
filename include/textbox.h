#ifndef TEXTBOX_H
#define TEXTBOX_H

#include "widget.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#define TEXTBOX_MAX 256

typedef struct
{
    Widget base;

    char text[TEXTBOX_MAX];

    int length;

    bool focused;

    TTF_Font* font;

} TextBox;

void TextBox_Init(
    TextBox*,
    float,
    float,
    float,
    float,
    TTF_Font*
);

#endif