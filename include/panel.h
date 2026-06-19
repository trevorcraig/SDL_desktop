#ifndef PANEL_H
#define PANEL_H

#include "widget.h"

#define PANEL_MAX 64

typedef enum
{
    PANEL_VERTICAL,
    PANEL_HORIZONTAL

} PanelLayout;

typedef struct
{
    Widget base;

    Widget* children[PANEL_MAX];

    int count;

    float padding;

    float spacing;

    PanelLayout layout;

    float scroll_y;

    bool scroll_enabled;

    float content_height;

} Panel;



void Panel_Init(
    Panel*,
    float,
    float,
    float,
    float
);

void Panel_SetLayout(
    Panel*,
    PanelLayout
);

void Panel_Add(
    Panel*,
    Widget*
);

void Panel_PerformLayout(
    Panel*
);

void Panel_EnableScroll(
    Panel*
);

#endif