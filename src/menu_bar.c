/**
 * @file menu_bar.c
 * @brief Menu bar widget with dropdown support.
 */

#include "menu_bar.h"

#include <string.h>

#define MENU_WIDTH      110.0f
#define DROPDOWN_WIDTH  180.0f
#define DROPDOWN_HEIGHT 40.0f

/**
 * @brief Check whether a point is inside a rectangle.
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
 * @brief Draw text inside a rectangle.
 */
static void DrawText(
    SDL_Renderer* renderer,
    TTF_Font* font,
    const char* text,
    SDL_FRect rect
)
{
    if (
        !font ||
        !text
    )
    {
        return;
    }

    SDL_Surface* surface =
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

    if (!surface)
    {
        return;
    }

    SDL_Texture* texture =
        SDL_CreateTextureFromSurface(
            renderer,
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
        rect.x + 10,

        rect.y
        +
        (
            rect.h -
            surface->h
        )
        / 2,

        surface->w,
        surface->h
    };

    SDL_RenderTexture(
        renderer,
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
 * @brief Update all menu item rectangles.
 */
static void UpdateMenuRects(
    MenuBar* mb
)
{
    for (
        int i = 0;
        i < mb->count;
        i++
    )
    {
        mb->items[i].rect =
        (
            SDL_FRect
        )
        {
            mb->base.rect.x
            +
            i * MENU_WIDTH,

            mb->base.rect.y,

            MENU_WIDTH,

            mb->base.rect.h
        };
    }
}

/**
 * @brief Close every open menu.
 */
static void CloseMenus(
    MenuBar* mb
)
{
    for (
        int i = 0;
        i < mb->count;
        i++
    )
    {
        mb->items[i].open =
            false;
    }
}

/**
 * @brief Calculate dropdown row rectangle.
 */
static SDL_FRect GetDropdownRect(
    MenuItem* item,
    int index
)
{
    return
    (
        SDL_FRect
    )
    {
        item->rect.x,

        item->rect.y
        +
        item->rect.h
        +
        index
        *
        DROPDOWN_HEIGHT,

        DROPDOWN_WIDTH,

        DROPDOWN_HEIGHT
    };
}

/**
 * @brief Render menu bar.
 */
static void Render(
    Widget* w,
    SDL_Renderer* r
)
{
    MenuBar* mb =
        (MenuBar*)w;

    UpdateMenuRects(
        mb
    );

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
        int i = 0;
        i < mb->count;
        i++
    )
    {
        MenuItem* item =
            &mb->items[i];

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

/**
 * @brief Handle menu interaction.
 */
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
    // Hover
    //--------------------------------

    if (
        e->type ==
        SDL_EVENT_MOUSE_MOTION
    )
    {
        for (
            int i = 0;
            i < mb->count;
            i++
        )
        {
            mb->items[i].hover =
                PointInRect(
                    mx,
                    my,
                    &mb->items[i].rect
                );
        }

        return;
    }

    //--------------------------------
    // Click only
    //--------------------------------

    if (
        e->type !=
        SDL_EVENT_MOUSE_BUTTON_UP
    )
    {
        return;
    }

    //--------------------------------
    // Menu buttons
    //--------------------------------

    for (
        int i = 0;
        i < mb->count;
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
            CloseMenus(
                mb
            );

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
    // Dropdown rows
    //--------------------------------

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
                GetDropdownRect(
                    item,
                    d
                );

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

    CloseMenus(
        mb
    );
}

/**
 * @brief Render dropdown menus.
 */
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
                GetDropdownRect(
                    item,
                    d
                );

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

/**
 * @brief Initialize menu bar.
 */
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

/**
 * @brief Add a top-level menu item.
 */
void MenuBar_Add(
    MenuBar* mb,
    const char* text,
    MenuCallback cb
)
{
    MenuItem* item =
        &mb->items[
            mb->count++
        ];

    item->text =
        text;

    item->callback =
        cb;

    item->hover =
        false;

    item->open =
        false;

    item->dropdown_count =
        0;
}

/**
 * @brief Add dropdown item.
 */
void MenuBar_AddDropdown(
    MenuBar* mb,
    int menu,
    const char* text,
    MenuCallback cb
)
{
    MenuItem* item =
        &mb->items[
            menu
        ];

    item->dropdown[
        item->dropdown_count++
    ] =
    (
        DropdownItem
    )
    {
        text,
        cb
    };
}