/**
 * @file widget.c
 * @brief Core widget utility functions.
 *
 * Provides common widget operations:
 * - rendering
 * - event handling
 * - layout configuration
 * - overlay rendering
 */

#include "widget.h"

/**
 * @brief Render a widget.
 *
 * Calls the widget's render callback
 * if one exists.
 *
 * @param w Widget instance.
 * @param r SDL renderer.
 */
void Widget_Render(
    Widget* w,
    SDL_Renderer* r
)
{
    if (
        !w ||
        !w->Render
    )
    {
        return;
    }

    w->Render(
        w,
        r
    );
}

/**
 * @brief Send an event to a widget.
 *
 * Calls the widget's event handler
 * if one exists.
 *
 * @param w Widget instance.
 * @param e SDL event.
 */
void Widget_Handle(
    Widget* w,
    SDL_Event* e
)
{
    if (
        !w ||
        !w->Handle
    )
    {
        return;
    }

    w->Handle(
        w,
        e
    );
}

/**
 * @brief Enable horizontal expansion.
 *
 * Allows the parent layout system
 * to stretch the widget width.
 *
 * @param w Widget instance.
 */
void Widget_SetFillWidth(
    Widget* w
)
{
    if (!w)
    {
        return;
    }

    w->fill_width =
        true;
}

/**
 * @brief Set widget flex value.
 *
 * Flex controls how much available
 * layout space a widget receives.
 *
 * Typical values:
 * - 0.0 → fixed size
 * - 1.0 → equal growth
 * - 2.0 → double growth
 *
 * @param w Widget instance.
 * @param flex Relative growth factor.
 */
void Widget_SetFlex(
    Widget* w,
    float flex
)
{
    if (!w)
    {
        return;
    }

    w->flex =
        flex;
}

/**
 * @brief Configure widget alignment.
 *
 * Controls how a widget positions
 * itself inside parent layouts.
 *
 * @param w Widget instance.
 * @param h Horizontal alignment.
 * @param v Vertical alignment.
 */
void Widget_SetAlign(
    Widget* w,
    HorizontalAlign h,
    VerticalAlign v
)
{
    if (!w)
    {
        return;
    }

    w->h_align =
        h;

    w->v_align =
        v;
}

/**
 * @brief Render widget overlay.
 *
 * Overlay rendering happens after
 * normal widget rendering and is
 * intended for menus, popups,
 * dropdowns, and floating UI.
 *
 * @param w Widget instance.
 * @param r SDL renderer.
 */
void Widget_RenderOverlay(
    Widget* w,
    SDL_Renderer* r
)
{
    if (
        !w ||
        !w->has_overlay ||
        !w->RenderOverlay
    )
    {
        return;
    }

    w->RenderOverlay(
        w,
        r
    );
}