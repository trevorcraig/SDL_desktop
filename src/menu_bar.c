#include "menu_bar.h"

#include <stdio.h>
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

static void DrawText(
    SDL_Renderer* r,
    TTF_Font* font,
    const char* text,
    SDL_FRect rect
)
{
    SDL_Surface* s =
        TTF_RenderText_Solid(
            font,
            text,
            strlen(text),
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

    SDL_FRect dst =
    {
        rect.x + 10,

        rect.y +
        (
            rect.h -
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

static void Render(
    Widget* w,
    SDL_Renderer* r
)
{
    
    MenuBar* mb =
        (MenuBar*)w;

    SDL_SetRenderDrawColor(
        r,
        45,
        45,
        45,
        255
    );

    SDL_RenderFillRect(
        r,
        &w->rect
    );

    for (
        int i=0;
        i<mb->count;
        i++
    )
    {
        MenuItem* item =
            &mb->items[i];

        //--------------------------------
        // ALWAYS update rect
        //--------------------------------
        // printf(
        //     "item %d %.0f %.0f %.0f %.0f\n",
        //     i,
        //     item->rect.x,
        //     item->rect.y,
        //     item->rect.w,
        //     item->rect.h
        // );

        item->rect =
        (
            SDL_FRect
        )
        {
            w->rect.x
            +
            i*110,

            w->rect.y,

            110,

            w->rect.h
        };

        if (
            item->hover
        )
        {
            SDL_SetRenderDrawColor(
                r,
                70,
                90,
                160,
                255
            );

            SDL_RenderFillRect(
                r,
                &item->rect
            );
        }

        DrawText(
            r,
            mb->font,
            item->text,
            item->rect
        );
    }
}

void MenuBar_Handle(
    Widget* w,
    SDL_Event* e
)
{
    MenuBar* mb =
        (MenuBar*)w;

    float mx;
    float my;

    SDL_GetMouseState(
        &mx,
        &my
    );


    //--------------------------------
    // Hover updates every frame
    //--------------------------------

    if (
        e->type ==
        SDL_EVENT_MOUSE_MOTION
    )
    {
        for (
            int i=0;
            i<mb->count;
            i++
        )
        {
            mb
            ->items[i]
            .hover =
                PointInRect(
                    mx,
                    my,
                    &mb
                    ->items[i]
                    .rect
                );
        }

        return;
    }

    //--------------------------------
    // Only click logic below
    //--------------------------------

    if (
        e->type
        !=
        SDL_EVENT_MOUSE_BUTTON_UP
    )
    {
        return;
    }

    //--------------------------------
    // First check menu buttons
    //--------------------------------

    for (
        int i=0;
        i<mb->count;
        i++
    )
    {
        MenuItem* item =
            &mb->items[i];

        if (
            PointInRect(
                mx,
                my,
                &item->rect
            )
        )
        {
            for (
                int j=0;
                j<mb->count;
                j++
            )
            {
                mb
                ->items[j]
                .open =
                    false;
            }

            item->open =
                true;

            if (
                item->callback
            )
            {
                item->callback();
            }

            return;
        }
    }

    //--------------------------------
    // Then dropdowns
    //--------------------------------

    for (
        int i=0;
        i<mb->count;
        i++
    )
    {
        MenuItem* item =
            &mb->items[i];

        if (
            !item->open
        )
        {
            continue;
        }

        for (
            int d=0;
            d<
            item
            ->dropdown_count;
            d++
        )
        {
            SDL_FRect row =
            {
                item->rect.x,
                item->rect.y
                +
                item->rect.h
                +
                d*40,
                180,
                40
            };

            if (
                PointInRect(
                    mx,
                    my,
                    &row
                )
            )
            {
                MenuCallback cb =
                    item
                    ->dropdown[d]
                    .callback;

                item->open =
                    false;

                if (
                    cb
                )
                {
                    cb();
                }

                return;
            }
        }
    }

    //--------------------------------
    // Click outside closes menus
    //--------------------------------

    for (
        int i=0;
        i<mb->count;
        i++
    )
    {
        mb
        ->items[i]
        .open =
            false;
    }
}

static void RenderOverlay(
    Widget* w,
    SDL_Renderer* r
)
{
    MenuBar* mb =
        (MenuBar*)w;

    for (
        int i = 0;
        i < mb->count;
        i++
    )
    {
        MenuItem* item =
            &mb->items[i];

        if (
            !item->open
        )
        {
            continue;
        }

        for (
            int d = 0;
            d < item->dropdown_count;
            d++
        )
        {
            SDL_FRect row =
            {
                item->rect.x,

                item->rect.y
                +
                item->rect.h
                +
                d * 40,

                180,

                40
            };

            SDL_SetRenderDrawColor(
                r,
                55,
                55,
                55,
                255
            );

            SDL_RenderFillRect(
                r,
                &row
            );

            SDL_SetRenderDrawColor(
                r,
                100,
                100,
                100,
                255
            );

            SDL_RenderRect(
                r,
                &row
            );

            DrawText(
                r,
                mb->font,
                item
                ->
                dropdown[d]
                .text,
                row
            );
        }
    }
}

void MenuBar_Init(
    MenuBar* mb,
    float x,
    float y,
    float w,
    float h,
    TTF_Font* font
)
{
    mb->base.rect =
    (
        SDL_FRect
    )
    {
        x,
        y,
        w,
        h
    };

    mb->base.Render =
        Render;

    mb->base.Handle =
        MenuBar_Handle;

    mb->base.RenderOverlay =
        RenderOverlay;

    mb->base.has_overlay =
        true;

    mb->font =
        font;

    mb->count =
        0;
}

void MenuBar_Add(
    MenuBar* mb,
    const char* text,
    MenuCallback cb
)
{
    MenuItem* item =
        &mb
        ->
        items[
            mb
            ->
            count++
        ];

    item->text =
        text;

    item->callback =
        cb;

    item->open =
        false;

    item->hover =
        false;

    item->dropdown_count =
        0;
}

void MenuBar_AddDropdown(
    MenuBar* mb,
    int menu,
    const char* text,
    MenuCallback cb
)
{
    MenuItem* item =
        &mb
        ->
        items[
            menu
        ];

    item
    ->
    dropdown[
        item
        ->
        dropdown_count++
    ]
    =
    (
        DropdownItem
    )
    {
        text,
        cb
    };
}