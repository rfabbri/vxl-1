//-----------------------------------------------------
//Biliana Kaneva, Smith College
//llist.cpp
//
//This file contains the member functions of NList class
//
//getHead - returns a pointer to the head of the list
//find    - returns a pointer to a cone with a face index 
//          equal to label. If none found, returns NULL.
//insert  - insert a node in the list
//remove  - delete a cone given the face index or a 
//          pointer to the cone
//findPoints - computes the intersection points of the cone
//             with the face it is crossing
//-----------------------------------------------------

/*--------------------------------------------------------------------
This code is Copyright 2000 by Biliana Kaneva & Joseph O'Rourke.  
It may be freely redistributed in its entirety provided that this 
copyright notice is not removed.  Question to orourke@cs.smith.edu.
--------------------------------------------------------------------*/

#include "llist.h"
#include "mathops.h"

//-----------------------------------------------------
// getHead - returns a pointer to the beginning of the
//           list
//-----------------------------------------------------
NCone *NList::getHead()
{
  return head;
}

//-----------------------------------------------------
//find - returns a pointer to the NCone with a cone 
//       crossing face with the given label
//-----------------------------------------------------
NCone * NList::find(int label)
{
  NCone *p;
  
  for (p=head->next; p!=NULL; p=p->next)
    if (p->p->currface==label)
      return p;

  return NULL;
  
}

//-----------------------------------------------------
//insert - inserts a NConeP in the list
//-----------------------------------------------------
void NList::insert(Cone *p, double r1, double r2, double l1, double l2)
{
  NConeP *q;
  q=new NConeP(p,head->next);
  q->r1 =r1;
  q->r2 =r2;
  q->l1 =l1;
  q->l2 =l2;
  findPoints(q,p); 
  head->next=q;
}

//-----------------------------------------------------
//findPoints - finds the intersection points of the cone
//with the face it is crossing
//-----------------------------------------------------
void NList::findPoints(NConeP *np, Cone *p)
{
  if (p->l == NULL)
    {
      np->p1[0] = p->f.v[0].p + (np->r1*(p->f.v[1].p - p->f.v[0].p));
      np->p1[1] = p->f.v[0].p + (np->l1*(p->f.v[1].p - p->f.v[0].p));
      np->p2[0] = p->f.v[1].p + (np->r2*(p->f.v[2].p - p->f.v[1].p));
      np->p2[1] = p->f.v[1].p + (np->l2*(p->f.v[2].p - p->f.v[1].p));
    }
  else if (p->r == NULL)
    {
      np->p1[0] = p->f.v[0].p + (np->r1*(p->f.v[1].p - p->f.v[0].p));
      np->p1[1] = p->f.v[0].p + (np->l1*(p->f.v[1].p - p->f.v[0].p));
      np->p2[0] = p->f.v[2].p + (np->r2*(p->f.v[0].p - p->f.v[2].p));
      np->p2[1] = p->f.v[2].p + (np->l2*(p->f.v[0].p - p->f.v[2].p));
    }
}

//-----------------------------------------------------
//insert - inserts NCone or NConeF in the list
//-----------------------------------------------------
void NList::insert(Cone *p, double fraction, int type)
{

  if (type == NCONE)
    {
      NCone *q;
      q=new NCone(p,head->next);
      head->next=q;
    }
  else 
    {
      NConeF *q;
      q=new NConeF(p,head->next);
      q->fraction=fraction;
      head->next=q;
    }

}

//-----------------------------------------------------
//insert - inserts NCone in the list and the list any
//NCone pointing to a cone crossing the same face as p
//-----------------------------------------------------
void NList::insert(Cone *p, int type)
{
  NCone *q;
  remove(p->currface, type); // delete the previous existing cone 
                             // going through the same face
  if (type == NCONE)
    q=new NCone(p,head->next);
  else
    q=new NConeF(p,head->next);
  head->next=q;
}

//-----------------------------------------------------
//remove - deletes NCone from the list that is pointing
// to cone c
//-----------------------------------------------------
int NList::remove(Cone *c, int type)
{
  NCone *p, *q;
  for (p=head; p->next!=NULL && p->next->p!=c ; p=p->next);
  if (p->next!=NULL)
    {
      q=p->next;
      p->next=p->next->next;
      delete q;
      q = NULL;

#ifdef MEM_COUNT
      if (type == NCONE)
        maxmemory -= sizeof(NCone);
      else if (type == NCONEF)
        maxmemory -= sizeof(NConeF);
      else
        maxmemory -= sizeof(NConeP);
#endif
      return 1;
    }
  else
    return 0;
}

//-----------------------------------------------------
//remove - deletes NCone from the list whose cone is 
//pointing to a face with the given label
//-----------------------------------------------------
int NList::remove(int label, int type)
{
  NCone *p, *q;
  for (p=head; p->next!=NULL && p->next->p->currface!=label ; p=p->next);
  if (p->next!=NULL)
    {
      q=p->next;
      p->next=p->next->next;
      delete q;
      q = NULL;

#ifdef MEM_COUNT
      if (type == NCONE)
        maxmemory -= sizeof(NCone);
      else if (type == NCONEF)
        maxmemory -= sizeof(NConeF);
      else
        maxmemory -= sizeof(NConeP);
#endif
      return 1;
    }
  else
    return 0;
}

//-----------------------------------------------------
//constructor - create a dummy node
//-----------------------------------------------------
NList::NList()
{
  head=new NCone(NULL,NULL);
}

//-----------------------------------------------------
//destructor - delete the list
//-----------------------------------------------------
NList::~NList()
{

  NCone *q, *p;

  for (p=head; p != NULL; )
    {
      q = p->next;
      delete p;
      p = NULL;
      p = q;
    }

}

