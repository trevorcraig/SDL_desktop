#include "textbox.h"

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
    TextBox* tb =
        (TextBox*)w;

    SDL_Color bg =
        tb->focused
        ?
        (SDL_Color){60,90,150,255}
        :
        (SDL_Color){70,70,70,255};

    SDL_SetRenderDrawColor(
        r,
        bg.r,
        bg.g,
        bg.b,
        255
    );

    SDL_RenderFillRect(
        r,
        &w->rect
    );

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
    // Safe display text
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

    SDL_Surface* s =
        TTF_RenderText_Blended(
            tb->font,
            display,
            strlen(display),
            (SDL_Color)
            {
                255,
                255,
                255,
                255
            }
        );

    if (!s)
    {
        return;
    }

    SDL_Texture* t =
        SDL_CreateTextureFromSurface(
            r,
            s
        );

    if (!t)
    {
        SDL_DestroySurface(
            s
        );

        return;
    }

    SDL_FRect dst =
    {
        w->rect.x + 10,

        w->rect.y +
        (
            w->rect.h -
            s->h
        ) / 2,

        (float)s->w,
        (float)s->h
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

static void Handle(
    Widget* w,
    SDL_Event* e
)
{
    TextBox* tb =
        (TextBox*)w;

    if (
        e->type ==
        SDL_EVENT_MOUSE_BUTTON_DOWN
    )
    {
        float mx;
        float my;

        SDL_GetMouseState(
            &mx,
            &my
        );

        tb->focused =
            PointInRect(
                mx,
                my,
                &w->rect
            );
    }

    if (
        !tb->focused
    )
    {
        return;
    }

    if (
        e->type ==
        SDL_EVENT_TEXT_INPUT
    )
    {
        if (
            tb->length <
            TEXTBOX_MAX - 1
        )
        {
            strcat(
                tb->text,
                e->text.text
            );

            tb->length =
                strlen(
                    tb->text
                );
        }
    }

    if (
        e->type ==
        SDL_EVENT_KEY_DOWN
    )
    {
        if (
            e->key.key ==
            SDLK_BACKSPACE
        )
        {
            if (
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
}

void TextBox_Init(
    TextBox* tb,
    float x,
    float y,
    float w,
    float h,
    TTF_Font* font
)
{
    tb->base.rect =
        (SDL_FRect)
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

    tb->font =
        font;

    tb->focused =
        false;

    tb->text[0] = '\0';
    tb->length = 0;
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