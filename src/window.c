#include "window.h"

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

static void MoveChildren(
    Panel* p,
    float dx,
    float dy
)
{
    for (
        int i = 0;
        i < p->count;
        i++
    )
    {
        Widget* child =
            p->children[i];

        child->rect.x += dx;
        child->rect.y += dy;

        Panel* nested =
            (Panel*)child;

        if (
            nested->base.Render ==
            p->base.Render
        )
        {
            MoveChildren(
                nested,
                dx,
                dy
            );
        }
    }
}

static void Render(
    Widget* w,
    SDL_Renderer* r
)
{
    Window* win =
        (Window*)w;

    //--------------------------------
    // Body
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
    // Title
    //--------------------------------

    SDL_FRect title =
    {
        w->rect.x,
        w->rect.y,
        w->rect.w,
        win->title_height
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
        &title
    );

    //--------------------------------
    // Content
    //--------------------------------

    Widget_Render(
        (Widget*)&win->content,
        r
    );

    //--------------------------------
    // ADD THIS
    //--------------------------------

    Widget_RenderOverlay(
        (Widget*)&win->content,
        r
    );
}

static void Handle(
    Widget* w,
    SDL_Event* e
)
{
    Window* win =
        (Window*)w;

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

        SDL_FRect title =
        {
            w->rect.x,
            w->rect.y,
            w->rect.w,
            win->title_height
        };

        if (
            PointInRect(
                mx,
                my,
                &title
            )
        )
        {
            win->dragging =
                true;

            win->drag_x =
                mx -
                w->rect.x;

            win->drag_y =
                my -
                w->rect.y;
        }
    }

    if (
        e->type ==
        SDL_EVENT_MOUSE_BUTTON_UP
    )
    {
        win->dragging =
            false;
    }

    if (
        e->type ==
        SDL_EVENT_MOUSE_MOTION
        &&
        win->dragging
    )
    {
        float mx;
        float my;

        SDL_GetMouseState(
            &mx,
            &my
        );

        float oldX =
            w->rect.x;

        float oldY =
            w->rect.y;

        w->rect.x =
            mx -
            win->drag_x;

        w->rect.y =
            my -
            win->drag_y;

        float dx =
            w->rect.x -
            oldX;

        float dy =
            w->rect.y -
            oldY;

        win->content.base.rect.x += dx;
        win->content.base.rect.y += dy;

        MoveChildren(
            &win->content,
            dx,
            dy
        );
    }

    Widget_Handle(
        (Widget*)&win->content,
        e
    );
}

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

    w->title =
        title;

    w->title_height =
        40;

    w->dragging =
        false;

    w->base.has_overlay =
    false;

    Panel_Init(
        &w->content,
        x,
        y + 40,
        width,
        height - 40
    );
}

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