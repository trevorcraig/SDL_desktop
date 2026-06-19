#include "panel.h"
#include "menu_bar.h"

static void Render(
    Widget* w,
    SDL_Renderer* r
)
{
    Panel* p =
        (Panel*)w;

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

    //--------------------------------
    // Render all children normally
    //--------------------------------
    //--------------------------------
    // Normal widgets
    //--------------------------------

    for (
        int i=0;
        i<p->count;
        i++
    )
    {
        Widget_Render(
            p->children[i],
            r
        );
    }

    //--------------------------------
    // Overlay widgets
    //--------------------------------

    SDL_SetRenderClipRect(
        r,
        NULL
    );

    for (
        int i=0;
        i<p->count;
        i++
    )
    {
        Widget_RenderOverlay(
            p->children[i],
            r
        );
    }
    // for (
    //     int i = 0;
    //     i < p->count;
    //     i++
    // )
    // {
    //     if (
    //         p->children[i]
    //     )
    //     {
    //         Widget_Render(
    //             p->children[i],
    //             r
    //         );
    //     }
    // }

    SDL_SetRenderClipRect(
        r,
        NULL
    );
}

static void Handle(
    Widget* w,
    SDL_Event* e
)
{
    Panel* p =
        (Panel*)w;

    //--------------------------------
    // Mouse wheel
    //--------------------------------

    if (
        p->scroll_enabled &&
        e->type ==
        SDL_EVENT_MOUSE_WHEEL
    )
    {
        p->scroll_y -=
            e->wheel.y
            * 40;

        if (
            p->scroll_y < 0
        )
        {
            p->scroll_y =
                0;
        }

        float maxScroll =
            p->content_height
            -
            p->base.rect.h;

        if (
            maxScroll < 0
        )
        {
            maxScroll =
                0;
        }

        if (
            p->scroll_y >
            maxScroll
        )
        {
            p->scroll_y =
                maxScroll;
        }

        Panel_PerformLayout(
            p
        );
    }

    for (
        int i = 0;
        i < p->count;
        i++
    )
    {
        Widget_Handle(
            p->children[i],
            e
        );
    }
}


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
    p->scroll_y = 0;

    p->scroll_enabled = false;

    p->content_height = 0;
    p->base.has_overlay =
    false;
        
}

void Panel_EnableScroll(
    Panel* p
)
{
    p->scroll_enabled =
        true;
}

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

void Panel_PerformLayout(
    Panel* p
)
{
    float cursorX =
        p->padding;

    float cursorY =
        p->padding;

    for (
        int i = 0;
        i < p->count;
        i++
    )
    {
        Widget* child =
            p->children[i];

        float availableW =
            p->base.rect.w
            -
            (
                p->padding
                * 2
            );

        float availableH =
            p->base.rect.h
            -
            (
                p->padding
                * 2
            );

        float childX =
            cursorX;

        float childY =
            cursorY;

        switch (
            child->h_align
        )
        {
        case ALIGN_LEFT:
            break;

        case ALIGN_CENTER:

            childX +=
                (
                    availableW
                    -
                    child->rect.w
                )
                / 2;

            break;

        case ALIGN_RIGHT:

            childX +=
                availableW
                -
                child->rect.w;

            break;

        case ALIGN_FILL:

            child->rect.w =
                availableW;

            break;
        }

        switch (
            child->v_align
        )
        {
        case ALIGN_TOP:
            break;

        case ALIGN_MIDDLE:

            childY +=
                (
                    availableH
                    -
                    child->rect.h
                )
                / 2;

            break;

        case ALIGN_BOTTOM:

            childY +=
                availableH
                -
                child->rect.h;

            break;

        case ALIGN_VFILL:

            child->rect.h =
                availableH;

            break;
        }

        child->rect.x =
            p->base.rect.x
            +
            childX;

        child->rect.y =
            p->base.rect.y
            +
            childY
            -
            p->scroll_y;

        if (
            child->fill_width
        )
        {
            child->rect.w =
                p->base.rect.w
                -
                (
                    p->padding
                    * 2
                );
        }
        if (
            p->layout ==
            PANEL_VERTICAL
        )
        {
            //--------------------------------
            // Only flex if explicitly set
            //--------------------------------

            if (
                child->flex > 0.0f
            )
            {
                child->rect.h =
                    (
                        p->base.rect.h
                        -
                        (
                            p->padding
                            * 2
                        )
                    )
                    *
                    child->flex;
            }

            //--------------------------------
            // Preserve existing height
            //--------------------------------

            if (
                child->rect.h < 1
            )
            {
                child->rect.h =
                    36;
            }

            cursorY +=
                child->rect.h
                +
                p->spacing;
        }
        // if (
        //     p->layout ==
        //     PANEL_VERTICAL
        // )
        // {
        //     if (
        //         child->flex > 0
        //     )
        //     {
        //         child->rect.h =
        //             (
        //                 p->base.rect.h
        //                 -
        //                 (
        //                     p->padding
        //                     * 2
        //                 )
        //             )
        //             *
        //             child->flex;
        //     }

        //     cursorY +=
        //         child->rect.h
        //         +
        //         p->spacing;
        // }
        else
        {
            cursorX +=
                child->rect.w +
                p->spacing;
        }

    //     Panel* nested =
    //         (Panel*)child;

    //     if (
    //         nested->base.Render ==
    //         Render
    //     )
    //     {
    //         Panel_PerformLayout(
    //             nested
    //         );
    //     }
    // }

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
    cursorY
    +
    p->padding;
}

void Panel_Add(
    Panel* p,
    Widget* w
)
{
    if (
        p->count <
        PANEL_MAX
    )
    {
        p->children[
            p->count++
        ] =
            w;

        Panel_PerformLayout(
            p
        );
    }
}