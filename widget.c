#include "widget.h"
#include <stdlib.h>

#define MAX(a,b) (a>b?a:b)
#define MIN(a,b) (a<b?a:b)

static widget* root_widget=NULL;
static widget* next_widget=NULL;
static widget* current_widget=NULL;

widget* GetRootWidget(void);
widget* GetParentWidget(widget* w);
widget* NewContainterWidget(uint8_t width,uint8_t height);
widget* NewRectWidget(uint8_t width,uint8_t height,color background);
void SetRectWidgetColor(widget* w,color c);
void PrintSurface(widget* parent,rect r);
void NewWindow(widget* w);
void WindowRefresh(void);
void WindowInsert(widget* w,int x,int y);
void WindowClear(void);
short WindowGetWidth(void);
short WindowGetHeight(void);
void WindowCheckWidget(widget* w);
void WidgetInformation(FILE* stream,widget* w);
void RectInformation(FILE* stream,rect r);
void FreeWidget(widget* w);
void DestroyWidget(widget* w);
void WindowDestroyWidget(widget* w);
void ClearWidget(widget* w);
void WindowClearWidget(widget* w);
void DestroyWindow(void);
void PrintWidget(widget* root,widget* other);
void ForEachWidgetInside(widget* parent,void (*func)(widget*));
rect GetPrintedRect(widget* w);
void WindowRefreshWidget(widget* w);
/*return the first child if the widget has children otherwise return NULL*/
widget* WidgetHasChildren(widget* w);
/*return the next widget. return NULL when arriving to the last widget*/
widget* NextWidget(widget* w);
/*return the next widget. return the first widget when arriving to the last widget*/
widget* NextWidgetAuto(widget* w);
widget* PreviousWidget(widget* w);
widget* PreviousWidgetAuto(widget* w);
void LockSurface(widget* parent,widget* other,rect surface);
void LockWidget(widget* parent,widget* other);
void WindowLockWidget(widget* parent,widget* other);
void MoveWidget(widget* w,int dx,int dy);
void WindowMoveWidget(widget* w,int x,int y);
void PlaceWidget(widget* parent,widget* other,int x,int y);
void SetWidgetPosition(widget* w,int x,int y);
void InsertWidget(widget* parent,widget* other,int x,int y);
void WindowInsertWidget(widget* parent,widget* other,int x,int y);
void RefreshWidget(widget* w);
void UnlockSurface(widget* w,rect surface);
void UnlockWidget(widget* w);
void WindowRefreshRect(rect r);
void WindowUnlockWidget(widget* w);
void HideWidget(widget* w);
void ShowWidget(widget* w);
void WindowHideWidget(widget* w);
void WindowShowWidget(widget* w);
void ReloadWidget(widget* w);
void WindowReloadWidget(widget* w);
widget* SelectWidget(widget* w);
widget* StandardWidgetSelection(widget* w);
widget* StandardContainerSelection(widget* w);
void SetWidgetSelectionFunction(widget* w,widget* (*selection_function)(widget*));
//end of list---------------------------!

__WidgetClass Widget={
    .init=NewContainterWidget,
    .parent=GetParentWidget,
    .print_surface=PrintSurface,
    .info=WidgetInformation,
    .del=FreeWidget,
    .destroy=DestroyWidget,
    .clear=ClearWidget,
    .print=PrintWidget,
    .browse=ForEachWidgetInside,
    .printed_rect=GetPrintedRect,
    .children=WidgetHasChildren,
    .next=NextWidget,
    .next_auto=NextWidgetAuto,
    .previous=PreviousWidget,
    .previous_auto=PreviousWidgetAuto,
    .lock=LockWidget,
    .move=MoveWidget,
    .place=PlaceWidget,
    .select=SelectWidget,
    .reload=ReloadWidget,
    .position=SetWidgetPosition,
    .insert=InsertWidget,
    .refresh=RefreshWidget,
    .unlock=UnlockWidget,
    .hide=HideWidget,
    .show=ShowWidget,
    .standard_selection=StandardWidgetSelection,
    .set_selection_function=SetWidgetSelectionFunction
};

__WindowClass Window={
    .root=GetRootWidget,
    .init=NewWindow,
    .destroy_widget=WindowDestroyWidget,
    .clear_widget=WindowClearWidget,
    .destroy=DestroyWindow,
    .refresh_widget=WindowRefreshWidget,
    .lock_widget=WindowLockWidget,
    .move_widget=WindowMoveWidget,
    .insert_widget=WindowInsertWidget,
    .unlock_widget=WindowUnlockWidget,
    .hide_widget=WindowHideWidget,
    .show_widget=WindowShowWidget,
    .reload_widget=WindowReloadWidget,
    .refresh=WindowRefresh,
    .insert=WindowInsert,
    .get_width=WindowGetWidth,
    .get_height=WindowGetHeight,
    .check_widget=WindowCheckWidget,
    .clear=WindowClear
};

__RectWidgetClass RectWidget={
    .init=NewRectWidget,
    .set_color=SetRectWidgetColor
};

__ContainerWidgetClass ContainerWidget={
    .init=NewContainterWidget,
    .standard_selection=StandardContainerSelection
};



rect InsidePart(rect root,rect other){
    /*This function calculate the intersection between two rectangle.
    It return the part of the other rectangle inside the root rectangle.*/
    rect r;
    r.x=MAX(root.x,other.x)-other.x;
    r.y=MAX(root.y,other.y)-other.y;
    r.width=MIN(root.x+root.width,other.x+other.width)-(r.x+other.x);
    r.height=MIN(root.y+root.height,other.y+other.height)-(r.y+other.y);
    if (r.width<0 || r.height<0) return (rect){0,0,0,0};
    return r;
}

key GetKey(void){
    int k=0;
    char* p=(char*)&k;
    stdin->_IO_read_ptr=stdin->_IO_read_end; //empty buffer
    getchar(); //wait for key...
    stdin->_IO_read_ptr--; //last key pressed
    while (stdin->_IO_read_ptr!=stdin->_IO_read_end){ //support combinations of characters
        *p=*stdin->_IO_read_ptr;
        p++;
        stdin->_IO_read_ptr++;
    };
    return k;
}

widget* NewContainterWidget(uint8_t width,uint8_t height){
    widget* w=calloc(1,sizeof(widget)); //initialize with 0
    w->surface=(rect){0,0,width,height};
    w->printed_surface=w->surface;
    w->link=CreateNode(NULL);
    w->parent_link=CreateNode(w);
    w->select=StandardContainerSelection;
    return w;
}

widget* NewRectWidget(uint8_t width,uint8_t height,color background){
    widget* w=NewContainterWidget(width,height);
    pixel p={
        .bg=background,
        .fg=WHITE,
        .character=' ',
        .style=0
    };
    w->nb_pixels=1;
    w->pixel_array=malloc(sizeof(pixel));
    w->pixel_array[0]=p;
    w->pixels=AllocNodeArray(width,height,w->pixel_array);
    w->select=StandardWidgetSelection;
    return w;
}

void SetRectWidgetColor(widget* w,color c){
    if (w->nb_pixels==1) w->pixel_array->bg=c;
}

widget* GetRootWidget(void){
    return root_widget;
}

widget* GetParentWidget(widget* w){
    return w->parent;
}

void PrintSurface(widget* parent,rect r){
    pixel* p;
    color fg,bg;
    int dx=parent->surface.x+r.x,dy=parent->surface.y;
    int ex=r.x+r.width;
    for (int i=r.y+r.height-1;i>=r.y;i--){
        GOTOXY(dx,i+dy);
        for (int j=r.x;j<ex;j++){
            p=parent->pixels[i][j]->previous->value;
            fg=p->fg;
            bg=p->bg;
            printf("\e[%hhu;38;2;%hhu;%hhu;%hhu;48;2;%hhu;%hhu;%hhum%c",p->style,fg.r,fg.g,fg.b,bg.r,bg.g,bg.b,p->character);
        }
    }
    GOTOXY(1,1);
    putchar('\n');
}

void NewWindow(widget* w){
    if (root_widget) DestroyWindow();
    system("stty raw -echo");
    HIDE_CURSOR();
    root_widget=w;
    SetWidgetPosition(w,1,1);
    root_widget->printed_surface=(rect){0,0,root_widget->surface.width,root_widget->surface.height};
    SET_TERMINAL_DIMENSION(w->surface.width,w->surface.height);
    PrintSurface(root_widget,root_widget->printed_surface);
}

void DestroyWindow(void){
    FreeWidget(root_widget);
    root_widget=NULL;
    puts("\e[0m"); //reset color
    system("clear");
    system("stty cooked echo");
    SHOW_CURSOR();
}

void WindowRefresh(void){
    PrintSurface(root_widget,root_widget->printed_surface);
}

void WindowInsert(widget* w,int x,int y){
    WindowInsertWidget(root_widget,w,x,y);
}

short WindowGetWidth(void){
    return root_widget->surface.width;
}

short WindowGetHeight(void){
    return root_widget->surface.height;
}

void WindowCheckWidget(widget* w){
    w->printed_surface=InsidePart(root_widget->surface,GetPrintedRect(w));
}

void RectInformation(FILE* stream,rect r){
    fprintf(stream,"x=%hhu y=%hhu width=%hhu height=%hhu\n",r.x,r.y,r.width,r.height);
}

void WidgetInformation(FILE* stream,widget* w){
    fprintf(stream,"Widget at %p\n",w);
    fputs("Surface:\n",stream);
    RectInformation(stream,w->surface);
    fputs("Printed Surface:\n",stream);
    RectInformation(stream,w->printed_surface);
    fputs("Printed Rect:\n",stream);
    RectInformation(stream,GetPrintedRect(w)); 
}

void ForEachWidgetInside(widget* parent,void (*func)(widget*)){
    node *n1=parent->link->next,*n2;
    while (n1->value){
        n2=n1;
        n1=n1->next;
        func(n2->value);
    }
}

void FreeWidget(widget* w){
    //printf("free widget %p\n",w);
    ForEachWidgetInside(w,FreeWidget);

    free(w->link); 
    free(w->parent_link);
    if (w->free_reserved) w->free_reserved(w->reserved);
    if (w->free_reserved1) w->free_reserved1(w->reserved1);
    if (w->pixel_array) free(w->pixel_array);
    if (w->pixels) free(w->pixels);
    free(w);
}

void DestroyWidget(widget* w){
    UnlockWidget(w);
    FreeWidget(w);
}

void WindowDestroyWidget(widget* w){
    WindowUnlockWidget(w);
    FreeWidget(w);
}

void ClearWidget(widget* w){
    ForEachWidgetInside(w,DestroyWidget);
}

void WindowClearWidget(widget* w){
    ForEachWidgetInside(w,WindowDestroyWidget);
}

void WindowClear(void){
    ForEachWidgetInside(root_widget,WindowDestroyWidget);
}

rect GetPrintedRect(widget* w){
    return (rect){w->surface.x+w->printed_surface.x,w->surface.y+w->printed_surface.y,w->printed_surface.width,w->printed_surface.height};
}

void WindowRefreshRect(rect r){
    r.x--;
    r.y--;
    PrintSurface(root_widget,r);
}

void WindowRefreshWidget(widget* w){
    WindowRefreshRect(GetPrintedRect(w));
}

widget* WidgetHasChildren(widget* w){
    return w->link->next->value;
}

widget* NextWidget(widget* w){
    return w->parent_link->next->value;
}

widget* NextWidgetAuto(widget* w){
    node* n=w->parent_link->next;
    if (n->value) return n->value;
    return n->next->value;
}

widget* PreviousWidget(widget* w){
    return w->parent_link->previous->value;
}

widget* PreviousWidgetAuto(widget* w){
    node* n=w->parent_link->previous;
    if (n->value) return n->value;
    return n->previous->value;
}

void LockSurface(widget* parent,widget* other,rect surface){
    int dx=other->surface.x-parent->surface.x,dy=other->surface.y-parent->surface.y;
    int sx=surface.x+surface.width-1;

    for (int i=surface.y+surface.height-1;i>=surface.y;i--){
        for (int j=sx;j>=surface.x;j--){
           InsertNodeInside(parent->pixels[i+dy][j+dx],other->pixels[i][j]);
        }
    }
}

void LockWidget(widget* parent,widget* other){
    if (other->parent==NULL){
        other->parent=parent;
        other->printed_surface=InsidePart(parent->surface,other->surface);
        PushNode(parent->link,other->parent_link);
    }
    if (other->pixels){
        if (parent->pixels){
            LockSurface(parent,other,other->printed_surface);
        }else if (parent->parent){
            other->printed_surface=InsidePart(parent->parent->surface,GetPrintedRect(other));
            LockWidget(parent->parent,other);
        }
    }else{
        widget *w1=WidgetHasChildren(other);
        while (w1){
            w1->printed_surface=InsidePart(parent->surface,GetPrintedRect(w1));
            LockWidget(parent,w1);
            w1=NextWidget(w1); 
        }
    }
}

void WindowLockWidget(widget* parent,widget* other){
    LockWidget(parent,other);
    WindowRefreshWidget(other);
}

void MoveWidget(widget* w,int dx,int dy){
    w->surface.x+=dx;
    w->surface.y+=dy;
    widget *w1=WidgetHasChildren(w);
    while(w1){
        MoveWidget(w1,dx,dy);
        w1=NextWidget(w1);
    }
    
}

void WindowMoveWidget(widget* w,int x,int y){
    widget* parent=w->parent;
    WindowUnlockWidget(w);
    MoveWidget(w,x,y);
    WindowLockWidget(parent,w);
}

void PlaceWidget(widget* parent,widget* other,int x,int y){
    int nx=parent->surface.x+x,ny=parent->surface.y+y;
    MoveWidget(other,nx-other->surface.x,ny-other->surface.y);
}

void SetWidgetPosition(widget* w,int x,int y){
    MoveWidget(w,x-w->surface.x,y-w->surface.y);
}

void InsertWidget(widget* parent,widget* other,int x,int y){
    PlaceWidget(parent,other,x,y);
    LockWidget(parent,other);
}

void WindowInsertWidget(widget* parent,widget* other,int x,int y){
    InsertWidget(parent,other,x,y);
    WindowRefreshWidget(other);
}

void UnlockSurface(widget* w,rect surface){
    for (int i=surface.y+surface.height-1;i>=surface.y;i--){
        for (int j=surface.x+surface.width-1;j>=surface.x;j--){
            DetachNode(w->pixels[i][j]);
        }
    }
}

void HideWidget(widget* w){
    if (w->pixels) UnlockSurface(w,w->printed_surface);
    else ForEachWidgetInside(w,HideWidget);
}

void UnlockWidget(widget* w){
    DetachNode(w->parent_link);
    w->parent=NULL;
    HideWidget(w);
    w->printed_surface=(rect){0,0,w->surface.width,w->surface.height};
}

void WindowUnlockWidget(widget* w){
    rect r=GetPrintedRect(w);   
    UnlockWidget(w);
    WindowRefreshRect(r);
}

void RefreshWidget(widget* w){
    widget *parent=w,*parent1=NULL;
    while (parent){
        if (parent->pixels) parent1=parent;
        parent=parent->parent;
    }
    PrintWidget(parent1,w);
}

void PrintWidget(widget* root,widget* other){
    /*This function prints the other widget.
    root widget must be the base of the pixel stack.
    For example root_widget can be used as root widget because
    it is the base of all window widget.
    Do not use a container widget as root widget. 
    It will trigger a segmentation fault because it has no pixels.*/
    rect r;
    widget* w;
    if (root==NULL){
        if (other->pixels) PrintSurface(other,other->printed_surface);
        /*if there is not root widget and other widget has pixels,
        this function take other widget as root widget*/
        else{
            /*if other widget has no pixels, the function browses
            the children of the other widget. Recursive part of the function*/
            w=WidgetHasChildren(other);
            while (w){
                PrintWidget(NULL,w);
                w=NextWidget(w);
            }
        }
    }else{
        /*if there is a root widget, just refresh a part of it*/
        r=GetPrintedRect(other);
        r.x-=root->surface.x;
        r.y-=root->surface.y;
        /*we calculate the other widget's coordinates in relation to
        the root widget*/
        PrintSurface(root,r);
    }
}

void WindowRefreshPixel(uint8_t x,uint8_t y){
    /*Print a pixel of the root_widget*/
    pixel* p=root_widget->pixels[y][x]->previous->value;
    color fg=p->fg,bg=p->bg;
    printf("\e[%hhu;38;2;%hhu;%hhu;%hhu;48;2;%hhu;%hhu;%hhum%c",p->style,fg.r,fg.g,fg.b,bg.r,bg.g,bg.b,p->character);
}

void WidgetInvertColor(widget* w){
    /*Invert the color the widget*/
    if (w->pixels==NULL) return;
    color c;
    pixel* p;
    /*Browse all pixels and invert background and foreground*/
    for (int i=0;i<w->nb_pixels;i++){
        p=w->pixel_array+i;
        c=p->bg;
        p->bg=p->fg;
        p->fg=c;
    }
}

void WindowHideWidget(widget* w){
    /*Hide the widget and refresh on the screen the widget surface*/
    HideWidget(w);
    WindowRefreshWidget(w);
}

void WindowShowWidget(widget* w){
    /*Show the widget and refresh on the screen the widget surface*/
    LockWidget(w->parent,w);
    WindowRefreshWidget(w);
}

void ShowWidget(widget* w){
    LockWidget(w->parent,w);
    RefreshWidget(w);
}

void ReloadWidget(widget* w){
    widget* parent=w->parent;
    if (parent==NULL) return;
    UnlockWidget(w);
    LockWidget(parent,w);
}

void WindowReloadWidget(widget* w){
    widget* parent=w->parent;
    if (parent==NULL) return;
    WindowUnlockWidget(w);
    WindowLockWidget(parent,w);
}

widget* SelectWidget(widget* w){
    if (w==NULL) return NULL;
    widget* return_value;
    next_widget=NULL;
    do{
        return_value=w->select(w);
        w=next_widget;
    }while(next_widget!=NULL);
    return return_value;
}

void NEXT_SELECTION(widget* w){
    next_widget=w;
}

void END_SELECTION(){
    next_widget=NULL;
}

widget* StandardKeySelection(widget* w){
    key k;
    while (1){
        k=GetKey();
        if (k==LEFT_ARROW || k==UP_ARROW){
            NEXT_SELECTION(PreviousWidgetAuto(w));
            return NULL;
        }
        if (k==RIGHT_ARROW || k==DOWN_ARROW){
            NEXT_SELECTION(NextWidgetAuto(w));
            return NULL;
        }
        if (k==ENTER){
            END_SELECTION();
            return w;
        }
        if (k==ESCAPE){
            END_SELECTION();
            return NULL;
        }
    }
}

widget* StandardWidgetSelection(widget* w){
    WidgetInvertColor(w);
    RefreshWidget(w);
    widget* return_value=StandardKeySelection(w);
    WidgetInvertColor(w);
    RefreshWidget(w);
    return return_value;
}

widget* StandardContainerSelection(widget* w){
    return SelectWidget(WidgetHasChildren(w));
}

void SetWidgetSelectionFunction(widget* w,widget* (*selection_function)(widget*)){
    w->select=selection_function;
}