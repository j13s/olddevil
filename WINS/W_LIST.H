/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Wins - A GUI for DOS
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. */

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#ifndef W_LISTH
#define W_LISTH
#include "w_system.h"

#ifdef NDEBUG
#define my_assert(x)
#define my_assertval(x) (x);
#else
#define my_assert(x) \
 { if(!(x)) { fprintf(errf,\
    "Assert-error in file %s line %d\n",__FILE__,__LINE__); my_exit(); } }
#define my_assertval(x) my_assert(x)
#endif
#define checkmem(x) { \
 if((x)==NULL) { \
  fprintf(errf,"No mem in file %s line %d\n",__FILE__,__LINE__);  \
  my_exit(); } }
struct node 
 { 
 struct node *next,*prev; 
 int no;
 union {
  struct wi_window *w_w;
  struct ws_bitmap *w_bm;
  struct wi_menu *w_m;
  struct wi_button *w_b;
  struct w_description *w_d;
#include "../nodeuni.h"
  struct node *n;
  void *v; } d;
 };
struct list
 {
 struct node *head,*dummy,*tail;
 int size,maxnum;
 };
 
void initlist(struct list *l);
struct node *insertnode(struct list *l,struct node *nprev,int no,void *data);
struct node *addnode(struct list *l,int no,void *data);
struct node *addheadnode(struct list *l,int no,void *data);
struct node *findnode(struct list *l,int no);
void copylisthead(struct list *d,struct list *s);
void unlistnode(struct list *l,struct node *n);
void listnode_tail(struct list *l,struct node *n);
void killnode(struct list *l,struct node *n);
void killlist(struct list *l);
#endif
