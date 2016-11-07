#ifndef Delaunay_H
#define Delaunay_H

#include <vcl_iostream.h>
#include <stdlib.h> // for C qsort 
#include <cmath>
#include <time.h> // for random

const int MaxVertices = 500;
const int MaxTriangles = 1000;
const int n_MaxPoints = 10; // for the test programm
const double EPSILON = 0.000001;
struct XYZ{
  double x, y, z,fx,fy;
  void out(){
    vcl_cout<<"("<<x<<","<<y<<","<<z<<") "<<"("<<fx<<","<<fy<<")";
  }
};

struct ITRIANGLE{
  int p1, p2, p3;
  double f12n,f23n,f31n;

  
};

struct IEDGE{
  int p1, p2 ;
};



int XYZCompare(const void *v1, const void *v2);
int Triangulate(int nv, XYZ pxyz[], ITRIANGLE v[], int &ntri);
int CircumCircle(double, double, double, double, double, double, double, 
double, double&, double&, double&);
void outputtriangle(int &nv, XYZ p[], ITRIANGLE v[], int &ntri);

#endif


