/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    list.c - List management
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program (file COPYING); if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. */

#include <stdio.h>
#include "w_system.h" /* Because of mem-functions */
#include "w_list.h"

extern FILE *errf;

/* list functions */
void initlist(struct list *l)
 {
 l->dummy=NULL;
 l->head=(struct node *)&l->dummy;
 l->tail=(struct node *)&l->head;
 l->size=l->maxnum=0;
 }
 
struct node *insertnode(struct list *l,struct node *nprev,int no,void *data)
 {
 struct node *n;
 if((n=MALLOC(sizeof(struct node)))==NULL) return NULL;
 n->d.v=data;
 n->next=nprev->next;
 n->prev=nprev;
 n->no=(no==-1 ? l->maxnum : no);
 if(n->no>=l->maxnum) l->maxnum=n->no+1;
 nprev->next->prev=n;
 nprev->next=n;
 l->size++;
 return n;
 }

struct node *addnode(struct list *l,int no,void *data)
 { return insertnode(l,l->tail,no,data); }
 
struct node *addheadnode(struct list *l,int no,void *data)
 { return insertnode(l,(struct node *)&l->head,no,data); }
 
struct node *findnode(struct list *l,int no)
 {
 struct node *n;
 for(n=l->head;n->next!=NULL;n=n->next)
  if(n->no==no) return n;
 return NULL;
 }

void copylisthead(struct list *d,struct list *s)
 {
 if(s->head->next==NULL) initlist(d);
 else
  {
  d->dummy=NULL; d->size=s->size; d->maxnum=s->maxnum;
  d->head=s->head; d->tail=s->tail;
  d->head->prev=(struct node *)&d->head;
  d->tail->next=(struct node *)&d->dummy;
  }
 }
 
void unlistnode(struct list *l,struct node *n)
 {
 n->prev->next=n->next;
 n->next->prev=n->prev;
 l->size--;
 }

void listnode_tail(struct list *l,struct node *n)
 {
 n->prev=l->tail; n->next=l->tail->next;
 n->prev->next=n; n->next->prev=n;
 n->no=l->maxnum++;
 l->size++; 
 }

void killnode(struct list *l,struct node *n)
 { unlistnode(l,n); free(n); }
 
void killlist(struct list *l)
 {
 struct node *n;
 for(n=l->head->next;n!=NULL;n=n->next) free(n->prev);
 l->size=0; l->maxnum=0;
 }
