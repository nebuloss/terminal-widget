#include "widget.h"
#include <unistd.h>
#include <locale.h>
#include <wchar.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
//-istrip -inlcr
#define MAX(a,b) (a>b?a:b)
#define MIN(a,b) (a<b?a:b)

typedef struct{
    color fg,bg;
    short style;
    wchar_t character;
}pixel_test;

int main(){
    widget* root=RectWidget.init(90,30,WHITE);
    widget* img1=BMPWidget.init("right.bmp");
    widget* img2=BMPWidget.init("right.bmp");
    BMPWidget.reverse(img2);
    Widget.insert(root,img1,0,0);
    Widget.insert(root,img2,60,0);
    Window.init(root);
    getchar();
    Window.insert_widget(img1,RectWidget.init(10,10,RED),10,10);
    getchar();
    Window.clear_widget(img1);
    getchar();
    widget* w1=RectWidget.init(40,30,GREEN);
    Window.insert(w1,30,0);
    Window.insert(RectWidget.init(20,15,BROWN),35,10);
    getchar();
    Window.destroy_widget(w1);
    getchar();
    Window.clear();
    getchar();
    widget* container=ContainerWidget.init(30,30);
    widget* w;
    for (int i=0;i<30;i++){
        w=BoardWidget.init(30,1,LIGHT_VIOLET);
        BoardWidget.insert_text(w,"test");
        Widget.insert(container,w,0,i);
    }
    Window.insert(container,0,0);
    getchar();
    Widget.select(container);
    getchar();
    Window.destroy();
    
    return 0;
}
/*

import tkinter as tk

root=tk.Tk()
label=tk.Label(root,text="test")
label.pack()
root.mainloop()
*/
