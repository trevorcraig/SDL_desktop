#include "button.h"

#include <string.h>

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

static void Render(
    Widget* w,
    SDL_Renderer* r
)
{
    Button* btn =
        (Button*)w;

    SDL_Color c =
        btn->normal;

    if (btn->pressed)
    {
        c =
            btn->click;
    }
    else if (
        btn->hovered
    )
    {
        c =
            btn->hover;
    }

    SDL_SetRenderDrawColor(
        r,
        c.r,
        c.g,
        c.b,
        255
    );

    SDL_RenderFillRect(
        r,
        &w->rect
    );

    if (
        btn->font &&
        btn->text
    )
    {
        SDL_Color white =
            {
                255,
                255,
                255,
                255
            };

        SDL_Surface* s =
            TTF_RenderText_Blended(
                btn->font,
                btn->text,
                strlen(
                    btn->text
                ),
                white
            );

        SDL_Texture* t =
            SDL_CreateTextureFromSurface(
                r,
                s
            );

        SDL_FRect dst =
        {
            w->rect.x +
            (
                w->rect.w -
                s->w
            ) / 2,

            w->rect.y +
            (
                w->rect.h -
                s->h
            ) / 2,

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
}

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

    if (
        e->type ==
        SDL_EVENT_MOUSE_BUTTON_DOWN
    )
    {
        if (
            btn->hovered
        )
        {
            btn->pressed =
                true;
        }
    }

    if (
        e->type ==
        SDL_EVENT_MOUSE_BUTTON_UP
    )
    {
        if (
            btn->hovered &&
            btn->pressed
        )
        {
            if (
                btn->callback
            )
            {
                btn->callback();
            }
        }

        btn->pressed =
            false;
    }
}

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
        (
            SDL_FRect
        )
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

    btn->hovered =
        false;

    btn->pressed =
        false;

    btn->text =
        text;

    btn->font =
        font;

    btn->callback =
        callback;

    btn->normal =
        (
            SDL_Color
        )
        {
            70,
            70,
            70,
            255
        };

    btn->hover =
        (
            SDL_Color
        )
        {
            110,
            110,
            110,
            255
        };

    btn->click =
        (
            SDL_Color
        )
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
    btn->base.has_overlay  =
        false;         
}