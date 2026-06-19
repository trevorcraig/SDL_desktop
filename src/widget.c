#include "widget.h"

void Widget_Render(
    Widget* w,
    SDL_Renderer* r
)
{
    if (
        w &&
        w->Render
    )
    {
        w->Render(
            w,
            r
        );
    }
}

void Widget_Handle(
    Widget* w,
    SDL_Event* e
)
{
    if (
        w &&
        w->Handle
    )
    {
        w->Handle(
            w,
            e
        );
    }
}

void Widget_SetFillWidth(
    Widget* w
)
{
    w->fill_width =
        true;
}

void Widget_SetFlex(
    Widget* w,
    float flex
)
{
    w->flex =
        flex;
}

void Widget_SetAlign(
    Widget* w,
    HorizontalAlign h,
    VerticalAlign v
)
{
    w->h_align =
        h;

    w->v_align =
        v;
}

void Widget_RenderOverlay(
    Widget* w,
    SDL_Renderer* r
)
{
    if (
        w &&
        w->has_overlay &&
        w->RenderOverlay
    )
    {
        w->RenderOverlay(
            w,
            r
        );
    }
}