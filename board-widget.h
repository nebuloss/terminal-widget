#ifndef __BOARD_WIDGET_H_INCLUDED
#define __BOARD_WIDGET_H_INCLUDED
#include "widget.h"

typedef struct{
    widget* (*init)(int width,int height,color background);
    widget* (*init_surface)(int width,int height,color background);
    void (*set_brush_color)(widget* w,color fg,color bg);
    void (*wipe)(widget* w);
    void (*insert_text)(widget* w,char* text);
    void (*insert_widget)(widget* bw,widget* other,rect widget_part,int x,int y);
    void (*insert_widget_auto)(widget* bw,widget* other,int x,int y);
    /*a faire*/
    void (*draw_rect)(widget* w,rect r,color c);
    /*à faire*/
    void (*draw_circle)(widget* w,int ox,int oy,int radius);
    /*à faire*/
    void (*draw_round)(widget* w,int ox,int oy,int radius);
}__BoardWidgetClass;

extern __BoardWidgetClass BoardWidget;

#endif