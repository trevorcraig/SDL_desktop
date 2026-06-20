/**
 * @file label.c
 * @brief Text rendering widget implementation.
 */

#include "label.h"

#include <string.h>

/**
 * @brief Render the label text.
 *
 * Creates a temporary text surface and texture,
 * then draws the text at the widget position.
 *
 * @param w Widget to render.
 * @param r SDL renderer.
 */
static void Render(
    Widget* w,
    SDL_Renderer* r
)
{
    Label* label =
        (Label*)w;

    if (
        !label ||
        !label->font ||
        !label->text
    )
    {
        return;
    }

    SDL_Surface* surface =
        TTF_RenderText_Blended(
            label->font,
            label->text,
            strlen(
                label->text
            ),
            label->color
        );

    if (!surface)
    {
        return;
    }

    SDL_Texture* texture =
        SDL_CreateTextureFromSurface(
            r,
            surface
        );

    if (!texture)
    {
        SDL_DestroySurface(
            surface
        );

        return;
    }

    SDL_FRect dst =
    {
        w->rect.x,
        w->rect.y,

        (float)surface->w,
        (float)surface->h
    };

    SDL_RenderTexture(
        r,
        texture,
        NULL,
        &dst
    );

    SDL_DestroyTexture(
        texture
    );

    SDL_DestroySurface(
        surface
    );
}

/**
 * @brief Initialize a label widget.
 *
 * Automatically sizes the widget
 * based on the rendered text dimensions.
 *
 * @param label Label instance.
 * @param x Left position.
 * @param y Top position.
 * @param text Label text.
 * @param font Font used for rendering.
 */
void Label_Init(
    Label* label,
    float x,
    float y,
    const char* text,
    TTF_Font* font
)
{
    int textWidth =
        0;

    int textHeight =
        0;

    if (
        font &&
        text
    )
    {
        TTF_GetStringSize(
            font,
            text,
            strlen(text),
            &textWidth,
            &textHeight
        );
    }

    //--------------------------------
    // Widget base
    //--------------------------------

    label->base.rect =
    (
        SDL_FRect
    )
    {
        x,
        y,
        (float)textWidth,
        (float)textHeight
    };

    label->base.Render =
        Render;

    label->base.Handle =
        NULL;

    //--------------------------------
    // Content
    //--------------------------------

    label->text =
        text;

    label->font =
        font;

    label->color =
    (
        SDL_Color
    )
    {
        255,
        255,
        255,
        255
    };

    //--------------------------------
    // Layout defaults
    //--------------------------------

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

    label->base.has_overlay =
        false;
}