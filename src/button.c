/**
 * @file button.c
 * @brief Button widget implementation.
 *
 * Provides rendering, mouse interaction,
 * hover states, pressed states, and callback
 * execution for clickable buttons.
 */

#include "button.h"

#include <string.h>

/// Default text color.
static const SDL_Color TEXT_COLOR =
{
    255,
    255,
    255,
    255
};

/**
 * @brief Check if a point lies inside a rectangle.
 *
 * @param x Mouse X position.
 * @param y Mouse Y position.
 * @param r Rectangle to test.
 *
 * @return true if inside.
 * @return false otherwise.
 */
static bool PointInRect(
    float x,
    float y,
    const SDL_FRect* r
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
 * @brief Render button text centered.
 *
 * @param btn Button instance.
 * @param r Renderer.
 */
static void RenderText(
    Button* btn,
    SDL_Renderer* r
)
{
    if (
        !btn->font ||
        !btn->text
    )
    {
        return;
    }

    SDL_Surface* surface =
        TTF_RenderText_Blended(
            btn->font,
            btn->text,
            strlen(
                btn->text
            ),
            TEXT_COLOR
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
        btn->base.rect.x +
        (
            btn->base.rect.w
            -
            surface->w
        ) / 2,

        btn->base.rect.y +
        (
            btn->base.rect.h
            -
            surface->h
        ) / 2,

        surface->w,
        surface->h
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
 * @brief Render a button.
 *
 * Draws the background color based
 * on hover and pressed states.
 *
 * @param w Widget instance.
 * @param r SDL renderer.
 */
static void Render(
    Widget* w,
    SDL_Renderer* r
)
{
    Button* btn =
        (Button*)w;

    SDL_Color color =
        btn->normal;

    if (
        btn->pressed
    )
    {
        color =
            btn->click;
    }
    else if (
        btn->hovered
    )
    {
        color =
            btn->hover;
    }

    SDL_SetRenderDrawColor(
        r,
        color.r,
        color.g,
        color.b,
        color.a
    );

    SDL_RenderFillRect(
        r,
        &w->rect
    );

    RenderText(
        btn,
        r
    );
}

/**
 * @brief Process button input.
 *
 * Updates hover state and executes
 * callback when clicked.
 *
 * @param w Widget instance.
 * @param e SDL event.
 */
static void Handle(
    Widget* w,
    SDL_Event* e
)
{
    Button* btn =
        (Button*)w;

    float mx;
    float my;

    SDL_GetMouseState(
        &mx,
        &my
    );

    btn->hovered =
        PointInRect(
            mx,
            my,
            &w->rect
        );

    switch (
        e->type
    )
    {
        case SDL_EVENT_MOUSE_BUTTON_DOWN:

            if (
                btn->hovered
            )
            {
                btn->pressed =
                    true;
            }

            break;

        case SDL_EVENT_MOUSE_BUTTON_UP:

            if (
                btn->pressed &&
                btn->hovered &&
                btn->callback
            )
            {
                btn->callback();
            }

            btn->pressed =
                false;

            break;

        default:
            break;
    }
}

/**
 * @brief Initialize a button widget.
 *
 * @param btn Button to initialize.
 * @param x Position X.
 * @param y Position Y.
 * @param w Width.
 * @param h Height.
 * @param text Display text.
 * @param font Font.
 * @param callback Click callback.
 */
void Button_Init(
    Button* btn,
    float x,
    float y,
    float w,
    float h,
    const char* text,
    TTF_Font* font,
    ButtonCallback callback
)
{
    btn->base.rect =
        (SDL_FRect)
        {
            x,
            y,
            w,
            h
        };

    btn->base.Render =
        Render;

    btn->base.Handle =
        Handle;

    btn->text =
        text;

    btn->font =
        font;

    btn->callback =
        callback;

    btn->hovered =
        false;

    btn->pressed =
        false;

    btn->normal =
        (SDL_Color)
        {
            70,
            70,
            70,
            255
        };

    btn->hover =
        (SDL_Color)
        {
            110,
            110,
            110,
            255
        };

    btn->click =
        (SDL_Color)
        {
            40,
            150,
            255,
            255
        };

    btn->base.fill_width =
        false;

    btn->base.fill_height =
        false;

    btn->base.flex =
        0;

    btn->base.h_align =
        ALIGN_LEFT;

    btn->base.v_align =
        ALIGN_TOP;

    btn->base.has_overlay =
        false;
}