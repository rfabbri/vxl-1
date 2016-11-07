//-----------------------------------------------------
//Biliana Kaneva, Smith College
//ctree.cpp
//
//This file contains the member function of the CTree
//class
//
//display - displays one level of the tree
//displaytree - displays the whole tree
//remove - deletes a subtree
//-----------------------------------------------------

/*--------------------------------------------------------------------
This code is Copyright 2000 by Biliana Kaneva & Joseph O'Rourke.  
It may be freely redistributed in its entirety provided that this 
copyright notice is not removed.  Question to orourke@cs.smith.edu.
--------------------------------------------------------------------*/

#include "ctree.h"


//-----------------------------------------------------
// display - displays one level of the tree, given 
//           the head of the level list
//-----------------------------------------------------
void CTree::display(Cone *head)
{
  Cone *q;

  for (q=head; q!=NULL; q=q->next)
  q->display();
}

//-----------------------------------------------------
//displaytree - displays the whole tree, given the root
//-----------------------------------------------------
void CTree::displaytree(Cone *p)
{
  if (p->l!=NULL) 
    displaytree(p->l);
  if (p->r!=NULL)
    displaytree(p->r);

  p->display();
}

//-----------------------------------------------------
//remove - deletes a subtree rooted at c and each
//deleted cone is deleted from the vertex list v, 
//containing the cones occupying each vertex, and from
//the list of sources if it is a source cone, and from 
//the list of cones crossing face c->currface
//-----------------------------------------------------
void CTree::remove(Cone *c, NList **v, int numv, NList *s, NList **f)
{
  Cone *p, *q;

  // shouldn't be in this function. nothing to delete
  if (c==NULL)
    return;
  
  // remove left child      
  if (c->l!=NULL)
    remove(c->l,v,numv,s,f);
  
  // remove right child
  if (c->r!=NULL)
      remove(c->r,v,numv,s,f);

  // remove middle childrenn if any
  if (c->m !=NULL)
    {
      for (p=c->m; p->next!=NULL && p->next->p == c; p=p->next);
      q=p;
      
      while (q!=c->m)
        {
          q=p->prev;
          remove(p,v,numv,s,f);
          p=q;
    }
      remove(q,v,numv,s,f);
      c->m=NULL;
    }
  
  
  // readjust the pointers in the level from which the cone was
  // deleted
  if (c->l==NULL && c->r==NULL && c->m == NULL)
    {
      if (c->prev!=NULL)
  c->prev->next=c->next;
      if (c->next!=NULL)
  c->next->prev=c->prev;
      
      if (c->p!=NULL)
  if (c->p->l==c)
    c->p->l=NULL;
  else if (c->p->r==c)
    c->p->r=NULL;
      
      // remove cone from vertex, source, and face list
      if (v != NULL)
        {
          for (int i=0; i < numv; i++)
            {
              v[i]->remove(c, NCONE);
            }
          
          f[c->currface]->remove(c, NCONEP);
     
          s->remove(c, NCONE);
        }

      // delete cone
      delete (Cone *)c;    
      c= NULL;
#ifdef MEM_COUNT
      maxmemory -= sizeof(Cone);
#endif
    }
}




