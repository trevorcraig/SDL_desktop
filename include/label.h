#ifndef LABEL_H
#define LABEL_H

#include "widget.h"
#include <SDL3_ttf/SDL_ttf.h>

typedef struct
{
    Widget base;

    const char* text;

    TTF_Font* font;

    SDL_Color color;

} Label;

void Label_Init(
    Label*,
    float,
    float,
    const char*,
    TTF_Font*
);

#endif