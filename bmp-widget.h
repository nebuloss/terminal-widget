#ifndef BMP_WIDGET_H_INCLUDED
#define BMP_WIDGET_H_INCLUDED
#include "widget.h"

typedef struct{
    widget* (*init)(char* filename);
    void (*reverse)(widget* bmpw);
    void (*left_rotation)(widget* bmpw);
    void (*right_rotation)(widget* bmpw);
    void (*left_rotation_centered)(widget* bmpw);
    void (*right_rotation_centered)(widget* bmpw);
}__BMPWidgetClass;

extern __BMPWidgetClass BMPWidget;

#endif