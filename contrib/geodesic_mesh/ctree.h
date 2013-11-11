//-----------------------------------------------------
//Biliana Kaneva, Smith College
//ctree.h
//
// Class declaration of the CTree class
//
// Ctree is a tree of cones, where each cone have left,
// right and/or list of middle children
//-----------------------------------------------------

/*--------------------------------------------------------------------
This code is Copyright 2000 by Biliana Kaneva & Joseph O'Rourke.  
It may be freely redistributed in its entirety provided that this 
copyright notice is not removed.  Question to orourke@cs.smith.edu.
--------------------------------------------------------------------*/

#ifndef CTREE_H
#define CTREE_H

#include "cone.h"
#include "llist.h"

class CTree {

 public:
  Cone *root;

  void display(Cone *head);
  void remove(Cone *c, NList **v, int numv, NList *s, NList **f);
  void displaytree(Cone *p);
  CTree(){root = NULL;};
  ~CTree(){ remove(root, NULL, 0, NULL, NULL);};
};

#endif
