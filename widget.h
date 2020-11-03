#ifndef __WIDGET_H_INCLUDED
#define __WIDGET_H_INCLUDED

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "node.h"

typedef struct{
    uint8_t r,g,b;
}color;

typedef struct{
    color fg,bg;
    char character,style;
}pixel;

#define BLUE (color){0x00,0x00,0xFF} 
#define BLACK (color){0x00,0x00,0x00}
#define WHITE (color){0xFF,0xFF,0xFF}
#define RED (color){0xFF,0x00,0x00}
#define GREEN (color){0x00,0xFF,0x00}
#define BROWN (color){0x58,0x29,0x00}
#define LIGHT_BLUE (color){0x77,0xB5,0xFE}
#define LIGHT_RED (color){0xC0,0x40,0x40}
#define ORANGE (color){0xFC,0x93,0x03}
#define LIGHT_VIOLET (color){0xB0,0x9D,0xB9}
#define GREY (color){0x78,0x78,0x78}

#define SET_TERMINAL_DIMENSION(width,height) printf("\e[8;%d;%dt",height,width) 
#define GOTOXY(x,y) printf("\e[%d;%dH",y,x) 
#define HIDE_CURSOR() printf("\e[?25l"); 
#define SHOW_CURSOR() printf("\e[?25h");
#define CLEAR_SCREEN() printf("\e[2J");
#define SET_TERMINAL_TITLE(title) printf("\e]2;"title"\a")

typedef struct{
    short x,y,width,height;
}rect;

/*widget structure*/
typedef struct widget widget;

/*widget structure*/
struct widget{
    /*real surface*/
    rect surface;
    /*the part of the widget printed*/
    rect printed_surface;   
    /*array of node*/
    node ***pixels;
    /*the value of this node is NULL*/
    node *link;
    /*parent link*/
    node *parent_link;
    /*pixel array*/
    pixel* pixel_array;
    /*lenght of the pixel array*/
    size_t nb_pixels;
    /*parent widget*/
    widget* parent;
    /*when the widget is selected, it launches this function*/
    widget* (*select)(widget*);
    /*if the value is not NULL, launch the function with reserved pointer as argument*/
    void (*free_reserved)(void*);
    /*see free reserved comment for more informations*/
    void (*free_reserved1)(void*);
    /*reserved data pointer*/
    void* reserved;
    /*reserved data pointer*/
    void* reserved1;

};

/*Special keys*/
typedef enum{
    RIGHT_ARROW=0x435B1B,
    LEFT_ARROW=0x445B1B,
    UP_ARROW=0x415B1B,
    DOWN_ARROW=0x425B1B,
    BACKSPACE=0x7F,
    SPACE=0x20,
    DELETE=0x7E335B1B,
    ESCAPE=0x1B,
    ENTER=0xD,
    TAB=0x9,
    QUIT=0x3
}key;

typedef struct{
    /*init widget. Allocate only requisite data. It returns a container*/
    widget* (*init)(uint8_t width,uint8_t height);
    /*return the parent of a widget*/
    widget* (*parent)(widget* w);
    /*print a part of the widget on the screen*/
    void (*print_surface)(widget* parent,rect r);
    /*print widget information in the stream*/
    void (*info)(FILE* stream,widget* w);
    /*free memory allocated by a widget*/
    void (*del)(widget* w);
    /*unlock and free widget*/
    void (*destroy)(widget* w);
    /*destroy all widgets inside the given widget*/
    void (*clear)(widget* w);
    /*print a widget on the screen. need a root widget*/
    void (*print)(widget* root,widget* other);
    /*launch the function for each widget inside the parent*/
    void (*browse)(widget* parent,void (*func)(widget*));
    /*return the rectangle printed on the screen*/
    rect (*printed_rect)(widget* w);
    /*if the widget has children,return the first child otherwise return NULL*/ 
    widget* (*children)(widget* w);
    /*return the next widget (widget's brother)*/
    widget* (*next)(widget* w);
    /*return the next widget. if the next widget is NULL it will skip it*/
    widget* (*next_auto)(widget* w);
    /*return the next widget (widget's brother)*/
    widget* (*previous)(widget* w);
    /*return the previous widget. if the previous widget is NULL it will skip it*/
    widget* (*previous_auto)(widget* w);
    /*lock the other widget inside the parent*/
    void (*lock)(widget* parent,widget* other);
    /*move a widget*/
    void (*move)(widget* w,int dx,int dy);
    /*place the widget in comparison with the parent widget*/
    void (*place)(widget* parent,widget* other,int x,int y);
    /*launch the select function*/
    widget* (*select)(widget* w);
    /*unlock and lock the widget => reload it*/
    void (*reload)(widget* w);
    /*set the widget position*/
    void (*position)(widget* w,int x,int y);
    /*insert a widget inside another*/
    void (*insert)(widget* parent,widget* other,int x,int y);
    /*refresh a widget*/
    void (*refresh)(widget* w);
    /*unlock a widget*/
    void (*unlock)(widget* w);
    /*hide a widget*/
    void (*hide)(widget* w);
    /*show widget*/
    void (*show)(widget* w);
    /*standard widget selection*/
    widget* (*standard_selection)(widget*);
    /*set widget selection function*/
    void (*set_selection_function)(widget* w,widget* (*selection_function)(widget* w));
}__WidgetClass;

typedef struct{
    /*return the root widget*/
    widget* (*root)(void);
    /*init the window*/
    void (*init)(widget* w);
    /*destroy a widget on the window*/
    void (*destroy_widget)(widget* w);
    /*clear a widget on the window*/
    void (*clear_widget)(widget* w);
    /*destroy the window and all widgets inside*/
    void (*destroy)(void);
    /*refresh the surface of the widget on the window*/
    void (*refresh_widget)(widget* w);
    /*lock a widget on the window*/
    void (*lock_widget)(widget* parent,widget* other);
    /*unlock a widget on the window*/
    void (*unlock_widget)(widget* w);
    /*move a widget on the window*/
    void (*move_widget)(widget* w,int dx,int dy);
    /*insert a widget inside another*/
    void (*insert_widget)(widget* parent,widget* other,int x,int y);
    /*hide a widget on the window*/
    void (*hide_widget)(widget* w);
    /*show a widget on the window*/
    void (*show_widget)(widget* w);
    /*reload a widget on the window (unlock and lock)*/
    void (*reload_widget)(widget* w);
    /*refresh the window*/
    void (*refresh)(void);
    /*insert a widget in the root widget*/
    void (*insert)(widget* w,int x,int y);
    /*return the window width*/
    short (*get_width)(void);
    /*return the window height*/
    short (*get_height)(void);
    /*check if a widget is inside the window. If not, it will modify his printed surface*/
    void (*check_widget)(widget* w);
    /*clear all widgets inside the window*/
    void (*clear)(void);
}__WindowClass;

typedef struct{
    /*create a rectangle widget*/
    widget* (*init)(uint8_t width,uint8_t height,color background);
    /*set the rectangle color*/
    void (*set_color)(widget* w,color c);
}__RectWidgetClass;

typedef struct{
    /*create a container widget*/
    widget* (*init)(uint8_t width,uint8_t height);
    /*standard container selection function*/
    widget* (*standard_selection)(widget*);
}__ContainerWidgetClass;

/*Widget operations*/
extern __WidgetClass Widget;
/*Window operations*/
extern __WindowClass Window;
extern __RectWidgetClass RectWidget;
extern __ContainerWidgetClass ContainerWidget;

/*Return the key pressed*/
extern key GetKey(void);

extern rect InsidePart(rect root,rect other);

extern void NEXT_SELECTION(widget* w);

extern void END_SELECTION();

extern widget* StandardKeySelection(widget* w);

#include "bmp-widget.h"
#include "board-widget.h"

#endif