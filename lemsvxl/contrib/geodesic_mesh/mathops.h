//-----------------------------------------------------
//Biliana Kaneva, Smith College
//mathops.h
//
// Class declaration of MathOps.
// Several useful math functions.
//-----------------------------------------------------

/*--------------------------------------------------------------------
This code is Copyright 2000 by Biliana Kaneva & Joseph O'Rourke.  
It may be freely redistributed in its entirety provided that this 
copyright notice is not removed.  Question to orourke@cs.smith.edu.
--------------------------------------------------------------------*/

#ifndef MATHOPS
#define MATHOPS

#include "structs.h"
#include <vcl_cmath.h>

class MathOps {

 public:
  Face Rotate2(Face face, double angle);
  Face Rotate(Face face, double sint, double cost);
  double abs(double n);
  double Cost(Point e0, Point e1, Point e2);
  double Sint(Point v0, Point v1, Point v2);
  int    Area2( Point a, Point b, Point c );
  double DotProduct(Point a, Point b);
  char SegSegInt(Point& a, Point& b, Point& c, Point& d, double &z);
  char SegSegInt(Point& a, Point& b, Point& c, Point& d, 
                 double &z1, double &z2);
  double Length(Point a, Point b);
};

#endif
