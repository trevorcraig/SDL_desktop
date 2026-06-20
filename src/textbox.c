/**
 * @file textbox.c
 * @brief Editable single-line text input widget.
 */

#include "textbox.h"

#include <string.h>

#define TEXT_PADDING 10

/**
 * @brief Determine if a point is inside a rectangle.
 *
 * @param x Mouse X position.
 * @param y Mouse Y position.
 * @param r Rectangle to test.
 *
 * @return true if point is inside rectangle.
 */
static bool PointInRect(
    float x,
    float y,
    SDL_FRect* r
)
{
    return (
        x >= r->x &&
        x <= r->x + r->w &&
        y >= r->y &&
        y <= r->y + r->h
    );
}

/**
 * @brief Render textbox contents.
 *
 * Draws:
 * - background
 * - border
 * - text
 * - caret while focused
 */
static void Render(
    Widget* w,
    SDL_Renderer* r
)
{
    TextBox* tb =
        (TextBox*)w;

    //--------------------------------
    // Background color
    //--------------------------------

    SDL_Color background =
        tb->focused
        ?
        (
            SDL_Color
        )
        {
            60,
            90,
            150,
            255
        }
        :
        (
            SDL_Color
        )
        {
            70,
            70,
            70,
            255
        };

    SDL_SetRenderDrawColor(
        r,
        background.r,
        background.g,
        background.b,
        background.a
    );

    SDL_RenderFillRect(
        r,
        &w->rect
    );

    //--------------------------------
    // Border
    //--------------------------------

    SDL_SetRenderDrawColor(
        r,
        220,
        220,
        220,
        255
    );

    SDL_RenderRect(
        r,
        &w->rect
    );

    //--------------------------------
    // Display string
    //
    // Adds cursor indicator while
    // textbox has focus.
    //--------------------------------

    char display[
        TEXTBOX_MAX + 2
    ];

    SDL_snprintf(
        display,
        sizeof(display),
        "%s%s",
        tb->text,
        tb->focused
        ?
        "|"
        :
        ""
    );

    //--------------------------------
    // Render text
    //--------------------------------

    SDL_Surface* surface =
        TTF_RenderText_Blended(
            tb->font,
            display,
            strlen(
                display
            ),
            (SDL_Color)
            {
                255,
                255,
                255,
                255
            }
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

    //--------------------------------
    // Center text vertically
    //--------------------------------

    SDL_FRect dst =
    {
        w->rect.x
        +
        TEXT_PADDING,

        w->rect.y
        +
        (
            w->rect.h
            -
            surface->h
        )
        / 2,

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
 * @brief Handle textbox input.
 *
 * Supports:
 * - focus
 * - text entry
 * - backspace
 */
static void Handle(
    Widget* w,
    SDL_Event* e
)
{
    TextBox* tb =
        (TextBox*)w;

    //--------------------------------
    // Focus handling
    //--------------------------------

    if (
        e->type ==
        SDL_EVENT_MOUSE_BUTTON_DOWN
    )
    {
        float mouseX;
        float mouseY;

        SDL_GetMouseState(
            &mouseX,
            &mouseY
        );

        tb->focused =
            PointInRect(
                mouseX,
                mouseY,
                &w->rect
            );
    }

    //--------------------------------
    // Ignore keyboard if not focused
    //--------------------------------

    if (
        !tb->focused
    )
    {
        return;
    }

    //--------------------------------
    // Text input
    //--------------------------------

    if (
        e->type ==
        SDL_EVENT_TEXT_INPUT
    )
    {
        size_t remaining =
            TEXTBOX_MAX
            -
            tb->length
            -
            1;

        strncat(
            tb->text,
            e->text.text,
            remaining
        );

        tb->length =
            strlen(
                tb->text
            );
    }

    //--------------------------------
    // Backspace
    //--------------------------------

    if (
        e->type ==
        SDL_EVENT_KEY_DOWN
    )
    {
        if (
            e->key.key ==
            SDLK_BACKSPACE
            &&
            tb->length > 0
        )
        {
            tb->length--;

            tb->text[
                tb->length
            ] =
                '\0';
        }
    }
}

/**
 * @brief Initialize textbox widget.
 *
 * @param tb Textbox instance.
 * @param x Left position.
 * @param y Top position.
 * @param w Width.
 * @param h Height.
 * @param font Text rendering font.
 */
void TextBox_Init(
    TextBox* tb,
    float x,
    float y,
    float w,
    float h,
    TTF_Font* font
)
{
    //--------------------------------
    // Widget base
    //--------------------------------

    tb->base.rect =
    (
        SDL_FRect
    )
    {
        x,
        y,
        w,
        h
    };

    tb->base.Render =
        Render;

    tb->base.Handle =
        Handle;

    //--------------------------------
    // Text state
    //--------------------------------

    tb->font =
        font;

    tb->focused =
        false;

    tb->text[0] =
        '\0';

    tb->length =
        0;

    //--------------------------------
    // Layout defaults
    //--------------------------------

    tb->base.fill_width =
        false;

    tb->base.fill_height =
        false;

    tb->base.flex =
        0;

    tb->base.h_align =
        ALIGN_LEFT;

    tb->base.v_align =
        ALIGN_TOP;

    tb->base.has_overlay =
        false;
}