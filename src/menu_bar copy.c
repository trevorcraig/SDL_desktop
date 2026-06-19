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
        x>=r->x &&
        x<=r->x+r->w &&
        y>=r->y &&
        y<=r->y+r->h
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
        return;

    SDL_Texture* t =
        SDL_CreateTextureFromSurface(
            r,
            s
        );

    SDL_FRect dst =
    {
        rect.x+10,

        rect.y
        +
        (
            rect.h
            -
            s->h
        )
        /
        2,

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

    //--------------------------------
    // Bar background
    //--------------------------------

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

    //--------------------------------
    // Menu buttons only
    //--------------------------------

    for (
        int i = 0;
        i < mb->count;
        i++
    )
    {
        MenuItem* item =
            &mb->items[i];

        item->rect =
        (
            SDL_FRect
        )
        {
            w->rect.x
            +
            i * 110,

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

    if (
        e->type
        !=
        SDL_EVENT_MOUSE_BUTTON_UP
    )
    {
        return;
    }

    bool hit =
        false;

    for (
        int i=0;
        i<mb->count;
        i++
    )
    {
        MenuItem* item =
            &mb->items[i];

        item->hover =
            PointInRect(
                mx,
                my,
                &item->rect
            );

        if (
            item->hover
        )
        {
            hit =
                true;

            for (
                int j=0;
                j<mb->count;
                j++
            )
            {
                mb
                ->
                items[j]
                .open=
                false;
            }

            item->open=
                true;
        }

        if (
            item->open
        )
        {
            for (
                int d=0;
                d<
                item
                ->
                dropdown_count;
                d++
            )
            {
                SDL_FRect row=
                {
                    item
                    ->
                    rect.x,

                    item
                    ->
                    rect.y
                    +
                    item
                    ->
                    rect.h
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
                        ->
                        dropdown[d]
                        .callback;

                    //--------------------------------
                    // Close menu FIRST
                    //--------------------------------

                    item
                    ->
                    open =
                        false;

                    //--------------------------------
                    // Execute callback AFTER
                    //--------------------------------

                    if (
                        cb
                    )
                    {
                        cb();
                    }

                    return;
                }
                // if (
                //     PointInRect(
                //         mx,
                //         my,
                //         &row
                //     )
                // )
                // {
                //     if (
                //         item
                //         ->
                //         dropdown[d]
                //         .callback
                //     )
                //     {
                //         item
                //         ->
                //         dropdown[d]
                //         .callback();
                //     }

                //     item
                //     ->
                //     open=
                //     false;
                // }
            }
        }
    }

    if (!hit)
    {
        for (
            int i=0;
            i<mb->count;
            i++
        )
        {
            mb
            ->
            items[i]
            .open=
            false;
        }
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
                item->dropdown[d].text,
                row
            );
        }
    }
}

// static void RenderOverlay(
//     Widget* w,
//     SDL_Renderer* r
// )
// {
//     MenuBar* mb =
//         (MenuBar*)w;

//     for (
//         int i=0;
//         i<mb->count;
//         i++
//     )
//     {
//         MenuItem* item =
//             &mb->items[i];

//         if (
//             !item->open
//         )
//         {
//             continue;
//         }

//         for (
//             int d=0;
//             d<item->dropdown_count;
//             d++
//         )
//         {
//             SDL_FRect row =
//             {
//                 item->rect.x,
//                 item->rect.y
//                 +
//                 item->rect.h
//                 +
//                 d*40,
//                 180,
//                 40
//             };

//             SDL_SetRenderDrawColor(
//                 r,
//                 55,
//                 55,
//                 55,
//                 255
//             );

//             SDL_RenderFillRect(
//                 r,
//                 &row
//             );

//             DrawText(
//                 r,
//                 mb->font,
//                 item
//                 ->
//                 dropdown[d]
//                 .text,
//                 row
//             );
//         }
//     }
// }

void MenuBar_Init(
    MenuBar* mb,
    float x,
    float y,
    float w,
    float h,
    TTF_Font* font
)
{
    mb->base.rect=
    (
        SDL_FRect
    )
    {
        x,
        y,
        w,
        h
    };

    mb->base.Render=
        Render;

    mb->base.Handle=
        MenuBar_Handle;

    mb->font=
        font;

    mb->count=
        0;
    mb->base.has_overlay =
        true;
    mb->base.RenderOverlay =
        RenderOverlay;
}

void MenuBar_Add(
    MenuBar* mb,
    const char* text,
    MenuCallback cb
)
{
    MenuItem* item=
        &mb
        ->
        items[
            mb
            ->
            count++
        ];

    item->text=
        text;

    item->callback=
        cb;

    item->open=
        false;

    item->dropdown_count=
        0;
}

void MenuBar_AddDropdown(
    MenuBar* mb,
    int menu,
    const char* text,
    MenuCallback cb
)
{
    MenuItem* item=
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
        dropdown_count
    ]
    =
    (
        DropdownItem
    )
    {
        text,
        cb
    };

    item
    ->
    dropdown_count++;
}

