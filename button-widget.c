#include "button-widget.h"
#include "board-widget.h"

typedef struct{
    widget* (*init)(int width,int height,char* text);
}__ButtonWidgetClass;

typedef struct{
    void (*action)(void*);
    void* arg;
    widget* extension;
    key extension_key;
}widget_button;

#include "widget.h"