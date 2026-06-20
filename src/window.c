#include "window.h"

/**
 * @brief Check if a point exists inside a rectangle.
 *
 * @param x Mouse X.
 * @param y Mouse Y.
 * @param rect Rectangle to test.
 *
 * @return true if point is inside.
 */
static bool PointInRect(
    float x,
    float y,
    SDL_FRect* rect
)
{
    return (
        x >= rect->x &&
        x <= rect->x + rect->w &&
        y >= rect->y &&
        y <= rect->y + rect->h
    );
}

/**
 * @brief Move all child widgets recursively.
 *
 * Used while dragging a window so children
 * maintain their relative positions.
 *
 * @param panel Panel to update.
 * @param dx Horizontal offset.
 * @param dy Vertical offset.
 */
static void MoveChildren(
    Panel* panel,
    float dx,
    float dy
)
{
    for (
        int i = 0;
        i < panel->count;
        i++
    )
    {
        Widget* child =
            panel->children[i];

        child->rect.x += dx;
        child->rect.y += dy;

        //--------------------------------
        // Recursive nested panel support
        //--------------------------------

        if (
            child->Render ==
            panel->base.Render
        )
        {
            MoveChildren(
                (Panel*)child,
                dx,
                dy
            );
        }
    }
}

/**
 * @brief Render window and content.
 *
 * Draw order:
 * - Window body
 * - Title bar
 * - Content
 * - Overlays
 *
 * @param w Window widget.
 * @param r SDL renderer.
 */
static void Render(
    Widget* w,
    SDL_Renderer* r
)
{
    Window* window =
        (Window*)w;

    //--------------------------------
    // Window body
    //--------------------------------

    SDL_SetRenderDrawColor(
        r,
        40,
        40,
        40,
        255
    );

    SDL_RenderFillRect(
        r,
        &w->rect
    );

    //--------------------------------
    // Title bar
    //--------------------------------

    SDL_FRect title_bar =
    {
        w->rect.x,
        w->rect.y,
        w->rect.w,
        window->title_height
    };

    SDL_SetRenderDrawColor(
        r,
        60,
        100,
        180,
        255
    );

    SDL_RenderFillRect(
        r,
        &title_bar
    );

    //--------------------------------
    // Content
    //--------------------------------

    Widget_Render(
        (Widget*)&window->content,
        r
    );

    //--------------------------------
    // Overlay content
    //--------------------------------

    Widget_RenderOverlay(
        (Widget*)&window->content,
        r
    );
}

/**
 * @brief Handle window input.
 *
 * Supports:
 * - Window dragging
 * - Child event forwarding
 *
 * @param w Window widget.
 * @param e SDL event.
 */
static void Handle(
    Widget* w,
    SDL_Event* e
)
{
    Window* window =
        (Window*)w;

    float mouse_x;
    float mouse_y;

    SDL_GetMouseState(
        &mouse_x,
        &mouse_y
    );

    //--------------------------------
    // Begin drag
    //--------------------------------

    if (
        e->type ==
        SDL_EVENT_MOUSE_BUTTON_DOWN
    )
    {
        SDL_FRect title_bar =
        {
            w->rect.x,
            w->rect.y,
            w->rect.w,
            window->title_height
        };

        if (
            PointInRect(
                mouse_x,
                mouse_y,
                &title_bar
            )
        )
        {
            window->dragging =
                true;

            window->drag_x =
                mouse_x
                -
                w->rect.x;

            window->drag_y =
                mouse_y
                -
                w->rect.y;
        }
    }

    //--------------------------------
    // Stop drag
    //--------------------------------

    if (
        e->type ==
        SDL_EVENT_MOUSE_BUTTON_UP
    )
    {
        window->dragging =
            false;
    }

    //--------------------------------
    // Drag update
    //--------------------------------

    if (
        window->dragging &&
        e->type ==
        SDL_EVENT_MOUSE_MOTION
    )
    {
        float old_x =
            w->rect.x;

        float old_y =
            w->rect.y;

        w->rect.x =
            mouse_x
            -
            window->drag_x;

        w->rect.y =
            mouse_y
            -
            window->drag_y;

        float delta_x =
            w->rect.x
            -
            old_x;

        float delta_y =
            w->rect.y
            -
            old_y;

        window
        ->
        content
        .
        base
        .
        rect
        .
        x +=
            delta_x;

        window
        ->
        content
        .
        base
        .
        rect
        .
        y +=
            delta_y;

        MoveChildren(
            &window->content,
            delta_x,
            delta_y
        );
    }

    //--------------------------------
    // Forward events
    //--------------------------------

    Widget_Handle(
        (Widget*)&window->content,
        e
    );
}

/**
 * @brief Initialize a window widget.
 *
 * @param w Window.
 * @param x Left position.
 * @param y Top position.
 * @param width Width.
 * @param height Height.
 * @param title Window title.
 */
void Window_Init(
    Window* w,
    float x,
    float y,
    float width,
    float height,
    const char* title
)
{
    w->base.rect =
    (
        SDL_FRect
    )
    {
        x,
        y,
        width,
        height
    };

    w->base.Render =
        Render;

    w->base.Handle =
        Handle;

    w->base.has_overlay =
        false;

    w->title =
        title;

    w->title_height =
        40;

    w->dragging =
        false;

    Panel_Init(
        &w->content,
        x,
        y + 40,
        width,
        height - 40
    );
}

/**
 * @brief Add widget to window content.
 *
 * @param w Window.
 * @param child Widget to add.
 */
void Window_Add(
    Window* w,
    Widget* child
)
{
    Panel_Add(
        &w->content,
        child
    );
}