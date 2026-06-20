#include "panel.h"

/**
 * @brief Render a panel and all child widgets.
 *
 * Draws the panel background, renders normal child widgets
 * inside the clipping region, then renders overlays without
 * clipping (dropdowns, menus, etc.).
 *
 * @param w Panel widget.
 * @param r SDL renderer.
 */
static void Render(
    Widget* w,
    SDL_Renderer* r
)
{
    Panel* panel =
        (Panel*)w;

    //--------------------------------
    // Panel background
    //--------------------------------

    SDL_SetRenderDrawColor(
        r,
        60,
        60,
        60,
        255
    );

    SDL_RenderFillRect(
        r,
        &w->rect
    );

    //--------------------------------
    // Clip normal children
    //--------------------------------

    SDL_Rect clip =
    {
        (int)w->rect.x,
        (int)w->rect.y,
        (int)w->rect.w,
        (int)w->rect.h
    };

    SDL_SetRenderClipRect(
        r,
        &clip
    );

    for (
        int i = 0;
        i < panel->count;
        i++
    )
    {
        Widget_Render(
            panel->children[i],
            r
        );
    }

    //--------------------------------
    // Render overlays unclipped
    //--------------------------------

    SDL_SetRenderClipRect(
        r,
        NULL
    );

    for (
        int i = 0;
        i < panel->count;
        i++
    )
    {
        Widget_RenderOverlay(
            panel->children[i],
            r
        );
    }
}

/**
 * @brief Handle panel events.
 *
 * Supports scrolling and forwards events
 * to child widgets.
 *
 * @param w Panel widget.
 * @param e SDL event.
 */
static void Handle(
    Widget* w,
    SDL_Event* e
)
{
    Panel* panel =
        (Panel*)w;

    //--------------------------------
    // Scroll support
    //--------------------------------

    if (
        panel->scroll_enabled &&
        e->type ==
        SDL_EVENT_MOUSE_WHEEL
    )
    {
        panel->scroll_y -=
            e->wheel.y * 40;

        if (
            panel->scroll_y < 0
        )
        {
            panel->scroll_y =
                0;
        }

        float max_scroll =
            panel->content_height
            -
            panel->base.rect.h;

        if (
            max_scroll < 0
        )
        {
            max_scroll =
                0;
        }

        if (
            panel->scroll_y >
            max_scroll
        )
        {
            panel->scroll_y =
                max_scroll;
        }

        Panel_PerformLayout(
            panel
        );
    }

    //--------------------------------
    // Forward event
    //--------------------------------

    for (
        int i = 0;
        i < panel->count;
        i++
    )
    {
        Widget_Handle(
            panel->children[i],
            e
        );
    }
}

/**
 * @brief Initialize panel.
 *
 * @param p Panel.
 * @param x Left position.
 * @param y Top position.
 * @param w Width.
 * @param h Height.
 */
void Panel_Init(
    Panel* p,
    float x,
    float y,
    float w,
    float h
)
{
    p->base.rect =
    (
        SDL_FRect
    )
    {
        x,
        y,
        w,
        h
    };

    p->base.Render =
        Render;

    p->base.Handle =
        Handle;

    p->count =
        0;

    p->padding =
        20;

    p->spacing =
        20;

    p->layout =
        PANEL_VERTICAL;

    p->scroll_y =
        0;

    p->scroll_enabled =
        false;

    p->content_height =
        0;

    //--------------------------------
    // Widget defaults
    //--------------------------------

    p->base.fill_width =
        false;

    p->base.fill_height =
        false;

    p->base.flex =
        0;

    p->base.h_align =
        ALIGN_LEFT;

    p->base.v_align =
        ALIGN_TOP;

    p->base.has_overlay =
        false;
}

/**
 * @brief Enable vertical scrolling.
 *
 * @param p Panel.
 */
void Panel_EnableScroll(
    Panel* p
)
{
    p->scroll_enabled =
        true;
}

/**
 * @brief Set panel layout mode.
 *
 * @param p Panel.
 * @param layout Horizontal or vertical.
 */
void Panel_SetLayout(
    Panel* p,
    PanelLayout layout
)
{
    p->layout =
        layout;

    Panel_PerformLayout(
        p
    );
}

/**
 * @brief Recalculate child positions.
 *
 * Applies:
 * - alignment
 * - spacing
 * - flex sizing
 * - scrolling
 *
 * @param p Panel.
 */
void Panel_PerformLayout(
    Panel* p
)
{
    float cursor_x =
        p->padding;

    float cursor_y =
        p->padding;

    float available_w =
        p->base.rect.w
        -
        (
            p->padding * 2
        );

    float available_h =
        p->base.rect.h
        -
        (
            p->padding * 2
        );

    for (
        int i = 0;
        i < p->count;
        i++
    )
    {
        Widget* child =
            p->children[i];

        //--------------------------------
        // Fill sizing
        //--------------------------------

        if (
            child->fill_width
        )
        {
            child->rect.w =
                available_w;
        }

        if (
            child->flex > 0.0f
        )
        {
            child->rect.h =
                available_h
                *
                child->flex;
        }

        if (
            child->rect.h < 1
        )
        {
            child->rect.h =
                36;
        }

        //--------------------------------
        // Position
        //--------------------------------

        child->rect.x =
            p->base.rect.x
            +
            cursor_x;

        child->rect.y =
            p->base.rect.y
            +
            cursor_y
            -
            p->scroll_y;

        //--------------------------------
        // Advance cursor
        //--------------------------------

        if (
            p->layout ==
            PANEL_VERTICAL
        )
        {
            cursor_y +=
                child->rect.h
                +
                p->spacing;
        }
        else
        {
            cursor_x +=
                child->rect.w
                +
                p->spacing;
        }

        //--------------------------------
        // Nested panel layout
        //--------------------------------

        if (
            child->Render ==
            Render
        )
        {
            Panel_PerformLayout(
                (Panel*)child
            );
        }
    }

    p->content_height =
        cursor_y
        +
        p->padding;
}

/**
 * @brief Add widget to panel.
 *
 * Automatically refreshes layout.
 *
 * @param p Panel.
 * @param w Child widget.
 */
void Panel_Add(
    Panel* p,
    Widget* w
)
{
    if (
        p->count >=
        PANEL_MAX
    )
    {
        return;
    }

    p->children[
        p->count++
    ] =
        w;

    Panel_PerformLayout(
        p
    );
}