
//-----------------------------------------------------
//Biliana Kaneva, Smith College
//cone.h
//
//Class declaration of the cone data structure.
//Each cone is a node in a cone tree. It has a parent 
//left and right children, and potentially a list of
//middle children.
//Two bounding vertices - left and right
//Index of the current face 
//The coordinates of the current face.
//The angle_sum until the current roll out.
//The gl_edge of the lastcrossed face.
//Index of the level the cone is part of.
//Pointers to the next and previous cones in the level.
//
//-----------------------------------------------------

/*--------------------------------------------------------------------
This code is Copyright 2000 by Biliana Kaneva & Joseph O'Rourke.  
It may be freely redistributed in its entirety provided that this 
copyright notice is not removed.  Question to orourke@cs.smith.edu.
--------------------------------------------------------------------*/

#ifndef CONE_H
#define CONE_H

#include "structs.h"
#include <vcl_cstdio.h>



//-------------------------------------------------------------
class Cone {
 public:
  Cone *p;                              // parent
  Cone *l;                              // left child
  Cone *r;                              // right child 
  Cone *m;                              // list of children at reflex 
                                        // vertex or source vertex
  Cone *source;                         // pointer to the source cone
  Vertex left;                          // left bounding vertex
  Vertex right;                         // right bounding vertex
  Vertex s;                             // source vertex - if it is a 
                                        // source cone
  int gl_edge;                          // gluing edge of the last crossed face
  double angle_sum;                     // the angle sum of the rolled out 
                                        // faces so far
  int level;                            // the level in the tree the current 
                                        // cone is in
  Cone *next, *prev;                    // next, previous cones in the level
  Face f;                               // the coordinates of the current face
                                        //and the global labels of the vertices
  int currface;                         // index of the current face 
  int rotated;                          // 0 - rotate right, 1 rotate left, 
                                        // 3 - 0 but only translare
                                        // 4 - 1 but only translate
  int type;                             // reflex or not 

  //constructor
  Cone()
    {
      p=NULL;
      l=NULL;
      r=NULL; 
      m=NULL;
      gl_edge=0;
      angle_sum=0;
      level=0;
      next=NULL;
      prev=NULL;
      
      currface=0;
      rotated=0;
      type=0;
    };

  //constructor
  Cone(Cone *parent, Cone *s, Vertex lf, Vertex rt, int currf,
       int ge, double a_sum, int lv, Cone *n, Cone *pr, Face face,
       int rd, int t)
    {
      p=parent;
      l=NULL;
      r=NULL;
      m=NULL;
      source=s;
      left=lf;
      right=rt;
      gl_edge=ge;
      angle_sum=a_sum;
      level=lv;
      next=n;
      prev=pr;
      f=face;
      currface=currf;
      rotated=rd;
      type=t;
    };

  //destructor
  ~Cone()
    {
      p=NULL;
      source=NULL;


      if (l!= NULL)
        {
          delete l;
#ifdef MEM_COUNT
          maxmemory -= sizeof(Cone);
#endif
        }
      if (r != NULL)
        {
          delete r;
#ifdef MEM_COUNT
          maxmemory -= sizeof(Cone);
#endif
        }
      gl_edge=0;
      angle_sum=0;
      level=0;
      next=NULL;
      prev=NULL;

      currface=0;
      rotated=0;
    };

  void display();

};
#endif




