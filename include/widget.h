#ifndef WIDGET_H
#define WIDGET_H

#include <SDL3/SDL.h>

typedef enum
{
    ALIGN_LEFT,
    ALIGN_CENTER,
    ALIGN_RIGHT,
    ALIGN_FILL

} HorizontalAlign;

typedef enum
{
    ALIGN_TOP,
    ALIGN_MIDDLE,
    ALIGN_BOTTOM,
    ALIGN_VFILL

} VerticalAlign;

typedef struct Widget Widget;

struct Widget
{
    SDL_FRect rect;

    bool fill_width;

    bool fill_height;

    bool has_overlay;

    float flex;

    HorizontalAlign h_align;

    VerticalAlign v_align;

    void (*Render)(
        Widget*,
        SDL_Renderer*
    );

    void (*RenderOverlay)(
        struct Widget*,
        SDL_Renderer*
    );

    void (*Handle)(
        Widget*,
        SDL_Event*
    );

    void* data;
};

void Widget_RenderOverlay(
    Widget*,
    SDL_Renderer*
);

void Widget_SetAlign(
    Widget*,
    HorizontalAlign,
    VerticalAlign
);

void Widget_Render(
    Widget*,
    SDL_Renderer*
);

void Widget_Handle(
    Widget*,
    SDL_Event*
);


void Widget_SetFillWidth(
    Widget*
);

void Widget_SetFlex(
    Widget*,
    float
);
#endif