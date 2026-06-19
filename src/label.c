#include "label.h"
#include <string.h>

static void Render(
    Widget* w,
    SDL_Renderer* r
)
{
    Label* label =
        (Label*)w;

    SDL_Surface* s =
        TTF_RenderText_Blended(
            label->font,
            label->text,
            strlen(
                label->text
            ),
            label->color
        );

    SDL_Texture* t =
        SDL_CreateTextureFromSurface(
            r,
            s
        );

    SDL_FRect dst =
    {
        w->rect.x,
        w->rect.y,
        s->w,
        s->h
    };

    SDL_RenderTexture(
        r,
        t,
        NULL,
        &dst
    );

    SDL_DestroyTexture(
        t
    );

    SDL_DestroySurface(
        s
    );
}

void Label_Init(
    Label* label,
    float x,
    float y,
    const char* text,
    TTF_Font* font
)
{
    int tw = 0;
    int th = 0;

    TTF_GetStringSize(
        font,
        text,
        strlen(text),
        &tw,
        &th
    );

    label->base.rect =
    (
        SDL_FRect
    )
    {
        x,
        y,
        (float)tw,
        (float)th
    };

    label->base.Render =
        Render;

    label->base.Handle =
        NULL;

    label->text =
        text;

    label->font =
        font;

    label->color =
        (SDL_Color)
        {
            255,
            255,
            255,
            255
        };
    label->base.fill_width =
        false;

    label->base.fill_height =
        false;

    label->base.flex =
        0;

    label->base.h_align =
        ALIGN_LEFT;

    label->base.v_align =
        ALIGN_TOP;
    label->base.has_overlay  =
        false; 
}