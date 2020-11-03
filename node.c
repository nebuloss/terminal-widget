#include "node.h"
#include <stdlib.h>
#include <stdio.h>

node* CreateNode(void* value){
    node* n=malloc(sizeof(node));
    *n=(node){n,n,n,n,n,value};
    return n;
}

node* LastNode(node* n){
    while (n!=n->last_insertion) n=n->last_insertion;
    return n;
}

void InsertNodeInside(node* parent,node* other){
    node *n1=parent,*n2;
    while (n1!=n1->last_insertion) n1=n1->last_insertion;
    n2=n1->next;
    n2->previous=other->previous;
    other->previous->next=n2;
    n1->next=other;
    other->previous=n1;
    other->previous_insertion=parent->last_insertion;
    parent->last_insertion=other;
    other->parent=parent;
}

void DetachNode(node* n){
    node *n1=n;
    while (n1!=n1->last_insertion) n1=n1->last_insertion;
    n1->next->previous=n->previous;
    n->previous->next=n1->next;
    n->previous=n1;
    n1->next=n;
    if (n->parent->last_insertion==n) n->parent->last_insertion=n->previous_insertion;
    else n1->previous_insertion=n->previous_insertion;
    n->parent=n;
    n->last_insertion=n;
    n->previous_insertion=n;
}

void PushNode(node* root,node* other){
    node* n1=root->previous;
    other->previous->next=root;
    root->previous=other->previous;
    n1->next=other;
    other->previous=n1;
    if (root->parent!=root){
        other->parent=root->parent;
        other->previous_insertion=root->previous_insertion;
        root->previous_insertion=other;
    }
}


void FreeNode(node* root){
    node *n1=root,*n2;
    do{
        n2=n1;
        n1=n1->next;
        free(n2);
    }while(n1!=root);
}

void PrintNode(node* n){
    node* n1=n;
    do{
        printf("%p\n",n1->value);
        n1=n1->next;
    }while(n1!=n);
}

node*** AllocNodeArray(int width,int height,void* default_value){
    size_t m1=height*sizeof(void*),m2=m1*width,m4=m1+m2;
    void *arr=malloc(m4+m2*6);
    node ***d=arr,**p1=arr+m1,*p2=arr+m4;
    for (int i=0;i<height;i++){
        d[i]=p1;
        p1+=width;
        for (int j=0;j<width;j++){
            d[i][j]=p2;
            *p2=(node){p2,p2,p2,p2,p2,default_value};
            p2++;
        }
    }
    return d;
}