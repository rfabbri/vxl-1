//-----------------------------------------------------
//Biliana Kaneva, Smith College
//llist.h
//
//Declaration of the NCone structure and the NList class
//
//-----------------------------------------------------

/*--------------------------------------------------------------------
This code is Copyright 2000 by Biliana Kaneva & Joseph O'Rourke.  
It may be freely redistributed in its entirety provided that this 
copyright notice is not removed.  Question to orourke@cs.smith.edu.
--------------------------------------------------------------------*/

#ifndef LLIST_H
#define LLIST_H

#include "cone.h"

#define NCONE 1
#define NCONEF 2
#define NCONEP 3

//-------------------------------------------------------------
// NCone - has a cone pointer, pointer to the next NCone and
//-------------------------------------------------------------

struct NCone{
  Cone *p;
  NCone *next;
  
  NCone()
    {
      p = NULL;
      next = NULL;
    }
  NCone(Cone *c, NCone *n)
    {
      p=c;
      next=n;
    };
  ~NCone()
    {
      p= NULL;
      next= NULL;
    }
};

//-------------------------------------------------------------
// NConeF - is a NCone with a fraction signifying where the
// path crosses edge 01 of the face p->f
//-------------------------------------------------------------
struct NConeF : public NCone{
  double fraction;


  NConeF(Cone *c, NCone *n)
    {
      p=c;
      next=n;
    };

};

//-------------------------------------------------------------
// NConeP - is an NCone containing the intersection points of 
// the cone with the face p->f it crosses
//-------------------------------------------------------------
struct NConeP : public NCone{
  double r1,r2,l1, l2;
  Point  p1[2],p2[2];  

  NConeP(Cone *c, NCone *n)
    {
      p=c;
      next=n;
    };

};


//-------------------------------------------------------------
//NList - list of NCones. Contains the different cones 
//        occupying a vertex via different edges/faces.
//-------------------------------------------------------------
class NList {

  NCone *head;

  public:
  NCone *getHead();
  void insert(Cone *p, int type);
  void insert(Cone *p, double fraction, int type);
  void insert(Cone *p, double r1, double r2, double l1, double l2);
  void findPoints(NConeP *np, Cone *p);
  int remove(int label, int type);
  int remove(Cone *c, int type);
  NCone *find(int label);
  NList();
  ~NList();
  
};

#endif
