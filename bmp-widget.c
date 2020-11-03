#include "bmp-widget.h"
#include <stdlib.h>

widget* NewBMPWidget(char* filename);
void BMPWidgetReverse(widget* bmpw);
void BMPWidgetLeftRotation(widget* bmpw);
void BMPWidgetRightRotation(widget* bmpw);
void BMPWidgetLeftRotationCentered(widget* bmpw);
void BMPWidgetRightRotationCentered(widget* bmpw);

__BMPWidgetClass BMPWidget={
    .init=NewBMPWidget,
    .reverse=BMPWidgetReverse,
    .left_rotation=BMPWidgetLeftRotation,
    .right_rotation=BMPWidgetRightRotation,
    .left_rotation_centered=BMPWidgetLeftRotationCentered,
    .right_rotation_centered=BMPWidgetRightRotationCentered
};

typedef struct BitMapHeader{ //bitmap header (size: 12 bytes)
    uint32_t FileSize; 
    uint16_t Reserved1;
    uint16_t Reserved2;
    uint32_t Offset;
}BitMapHeader;

typedef struct BitMapInfoHeader{//info header. It contains image properties (size: 40 bytes)
    uint32_t Size;
    uint32_t Width;
    uint32_t Height;
    uint16_t Planes;
    uint16_t BitsPerPixel;
    uint32_t Compression;
    uint32_t ImageSize;
    uint32_t HorizontalResolution;
    uint32_t VerticalResolution;
    uint32_t ColorUsed;
    uint32_t ImportantColors;
}BitMapInfoHeader;

void BMPWidgetFreeReserved(void* reserved){
    if (reserved!=NULL) free(reserved);
}

widget* NewBMPWidget(char* filename){
    uint16_t magic,pitch,width;
    BitMapHeader header;
    BitMapInfoHeader info;
    widget* w=NULL;

    FILE* file=fopen(filename,"rb");
    if (file==NULL) return NULL;

    fread(&magic,2,1,file); //read the first 2 characters. It must be BM
    if (magic!=0x4d42) goto end;

    fread(&header,sizeof(BitMapHeader),1,file); //read headers
    fread(&info,sizeof(BitMapInfoHeader),1,file);

    width=info.Width<<1;

    if (width>0x7F || info.Height>0xFF) goto end; //widget max dimension

    uint8_t BytePerPixel=info.BitsPerPixel/8; //support only 24 and 32 bits pixels 
    if (BytePerPixel<3) goto end;

    uint8_t current_pixel[4];
    pitch=info.ImageSize/info.Height;
    long offset=header.Offset+info.ImageSize-pitch;

    w=Widget.init(width,info.Height);
    w->pixels=AllocNodeArray(width,info.Height,NULL);
    w->nb_pixels=info.Height*info.Width;
    w->pixel_array=malloc(sizeof(pixel)*w->nb_pixels);
    w->select=Widget.standard_selection;
    w->free_reserved=BMPWidgetFreeReserved;

    pixel* p=w->pixel_array;

    for (int i=0;i<info.Height;i++,offset-=pitch){
        fseek(file,offset,SEEK_SET);
        for (int j=0;j<width;j+=2,p++){
            fread(current_pixel,BytePerPixel,1,file);
            *p=(pixel){
                .fg=WHITE,
                .bg=(color){current_pixel[2],current_pixel[1],current_pixel[0]},
                .character=' ',
                .style=0
            };
            w->pixels[i][j]->value=p;
            w->pixels[i][j+1]->value=p;
        }
    }

    end:
       fclose(file);
       return w;
}

int BMPWidgetInvertCoordinate(int x,int lenght){
    return lenght-x-1;
}

int BMPWidgetIdentityCoordinate(int x,int lenght){
    return x;
}

void BMPWidgetRotation(widget* bmpw,int (*func1)(int,int),int (*func2)(int,int),bool center){
    int width=bmpw->surface.width>>1,height=bmpw->surface.height<<1;
    node*** new_array=bmpw->reserved;
    if (new_array==NULL) new_array=AllocNodeArray(height,width,NULL);

    for (int i=0,i2;i<width;i++){
        i2=func1(i,width)<<1;
        for (int j=0,j2;j<height;j++){
            j2=func2(j,height)>>1;
            new_array[i][j]->value=bmpw->pixels[j2][i2]->value;
            j++;
            new_array[i][j]->value=bmpw->pixels[j2][i2]->value;
        }
    }
    if (center){
        bmpw->surface.x+=width-bmpw->surface.height;
        bmpw->surface.y+=(bmpw->surface.height>>1)-(width>>1);
    }

    bmpw->reserved=bmpw->pixels;
    bmpw->pixels=new_array;
    bmpw->surface.width=height;
    bmpw->surface.height=width;
    
    if (bmpw->parent){
        bmpw->printed_surface=InsidePart(Widget.printed_rect(bmpw->parent),bmpw->surface);
    }else{
        bmpw->printed_surface.width=height;
        bmpw->printed_surface.height=width;
    }
}

void BMPWidgetLeftRotation(widget* bmpw){
    BMPWidgetRotation(bmpw,BMPWidgetInvertCoordinate,BMPWidgetIdentityCoordinate,false);
}

void BMPWidgetRightRotation(widget* bmpw){
    BMPWidgetRotation(bmpw,BMPWidgetIdentityCoordinate,BMPWidgetInvertCoordinate,false);
}

void BMPWidgetLeftRotationCentered(widget* bmpw){
    BMPWidgetRotation(bmpw,BMPWidgetInvertCoordinate,BMPWidgetIdentityCoordinate,true);
}

void BMPWidgetRightRotationCentered(widget* bmpw){
    BMPWidgetRotation(bmpw,BMPWidgetIdentityCoordinate,BMPWidgetInvertCoordinate,true);
}

void BMPWidgetReverse(widget* bmpw){
    pixel p;
    for (int i=bmpw->nb_pixels-1,i1=0;i>i1;i--,i1++){
        p=bmpw->pixel_array[i];
        bmpw->pixel_array[i]=bmpw->pixel_array[i1];
        bmpw->pixel_array[i1]=p;
    }
}

