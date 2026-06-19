#ifndef WINDOW_H
#define WINDOW_H

#include "panel.h"

typedef struct
{
    Widget base;

    Panel content;

    const char* title;

    float title_height;

    bool dragging;

    float drag_x;

    float drag_y;

} Window;

void Window_Init(
    Window*,
    float,
    float,
    float,
    float,
    const char*
);

void Window_Add(
    Window*,
    Widget*
);

#endif