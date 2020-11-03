#ifndef __NODE_H_INCLUDED
#define __NODE_H_INCLUDED

#define POINTER(p) (void*)p

typedef struct node node;

/*size of the structure: 48 bytes*/
struct node{
    /*links*/
    node *next,*previous,*last_insertion,*previous_insertion,*parent;
    /*value stored*/
    void* value;
};

/*Create a node with the value stored inside*/
extern node* CreateNode(void* value);

/*Return the last node*/
extern node* LastNode(node* n);

/*Insert the node at the end of the root node*/
extern void InsertNodeInside(node* parent,node* other);

/*Detach the node and all nodes inside*/
extern void DetachNode(node* n);

/*Push a node in the stack*/
extern void PushNode(node* root,node* other);

/*Free all node in the list*/
extern void FreeNode(node* root);

/*Print all node elements*/
extern void PrintNode(node* n);

/*Allocate 2D array of nodes with one malloc*/
extern node*** AllocNodeArray(int width,int height,void* default_value);

#endif