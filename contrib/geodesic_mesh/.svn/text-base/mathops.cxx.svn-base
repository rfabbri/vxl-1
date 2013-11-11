//-----------------------------------------------------
//Biliana Kaneva, Smith College
//mathops.cpp
//
//This files contains the member functions of the class
//MathOps
//-----------------------------------------------------

/*--------------------------------------------------------------------
This code is Copyright 2000 by Biliana Kaneva & Joseph O'Rourke.  
It may be freely redistributed in its entirety provided that this 
copyright notice is not removed.  Question to orourke@cs.smith.edu.
--------------------------------------------------------------------*/

#include "mathops.h"


//--------------------------------------------------------------------
//abs - returns the absolute value of n
//--------------------------------------------------------------------
double MathOps::abs(double n)
{
  if (n < 0)
    return (0 - n);
  return n;
}

//--------------------------------------------------------------------
//DotProduct - returns the dot product of vectors a and b
//--------------------------------------------------------------------
double MathOps::DotProduct(Point a, Point b)
{
  return (a.x*b.x+a.y*b.y);
}


//--------------------------------------------------------------------
//SegSegInt - returns a 1 if segments ab and cd cross,0 if they don't
//            v if crosses at a vertex, and w along an edge.
//--------------------------------------------------------------------
char MathOps::SegSegInt(Point& a, Point& b, Point& c, Point& d, double &z )
{
  double s,t;
  double num, denom;
  char code ='?';

  denom= a.x*(d.y-c.y) + b.x*(c.y-d.y)+
         d.x*(b.y-a.y) + c.x*(a.y-b.y);

  if (denom == 0.0)
    {
      //    cout << "Oops!!! This can't happen!" << endl;
    z = denom;
    return '0';
      // parallel... what should we do here???
    }

  num = a.x*(d.y-c.y)+c.x*(a.y-d.y)+d.x*(c.y-a.y);
  if (num==0.0 || num == denom) code = 'v';

  s=num/denom;
  num=-(a.x*(c.y-b.y)+b.x*(a.y-c.y)+c.x*(b.y-a.y));
  if (num==0.0 || num == denom) code = 'v';
  t=num/denom;

  if ( (0.0 < s && s < 1.0) && ( 0.0 < t && t < 1.0))
    code='1';
  else  if ( (0.0 > s || s > 1.0) || ( 0.0 > t || t > 1.0))
    code='0';

  z=t;
  return code;
  
}

//--------------------------------------------------------------------
//SegSegInt - returns a 1 if segments ab and cd cross,0 if they don't
//            v if crosses at a vertex, and w along an edge.
//--------------------------------------------------------------------
char MathOps::SegSegInt(Point& a, Point& b, Point& c, Point& d, double &z1, 
                        double &z2)
{
  double s,t;
  double num, denom;
  char code ='?';

  denom= a.x*(d.y-c.y) + b.x*(c.y-d.y)+
         d.x*(b.y-a.y) + c.x*(a.y-b.y);

  if (denom == 0.0)
    {
      //    cout << "Oops!!! This can't happen!" << endl;
    z1 = denom;
    return '0';
      // parallel... what should we do here???
    }

  num = a.x*(d.y-c.y)+c.x*(a.y-d.y)+d.x*(c.y-a.y);
  if (num==0.0 || num == denom) code = 'v';

  s=num/denom;
  num=-(a.x*(c.y-b.y)+b.x*(a.y-c.y)+c.x*(b.y-a.y));
  if (num==0.0 || num == denom) code = 'v';
  t=num/denom;

  if ( (0.0 < s && s < 1.0) && ( 0.0 < t && t < 1.0))
    code='1';
  else  if ( (0.0 > s || s > 1.0) || ( 0.0 > t || t > 1.0))
    code='0';

  z1 = t;
  z2 = s;
  return code;
  
}

//--------------------------------------------------------------------
//Cost: Returns the cosine of the angle at vertex v0
//--------------------------------------------------------------------
double MathOps::Cost(Point v0, Point v1, Point v2)
{
  double a,b,c;

  a=Length(v0,v1);
  b=Length(v2,v0);
  c=Length(v1,v2);
  if (2*a*b == 0)
    return -2;
  else
    return (-c*c+a*a+b*b)/(2*a*b);
}

//--------------------------------------------------------------------
//Sint: Returns the sine of the angle at vertex v0
//--------------------------------------------------------------------
double MathOps::Sint(Point v0, Point v1, Point v2)
{
  double cost;

  cost=Cost(v0,v1,v2); 
  return (sqrt(1-cost*cost));
}

/*---------------------------------------------------------------------
Returns twice the signed area of the triangle determined by a,b,c.
The area is positive if a,b,c are oriented ccw, negative if cw,
and zero if the points are collinear.
---------------------------------------------------------------------*/
int  MathOps::Area2( Point a, Point b, Point c )
{
  double area = (b.x - a.x) * (c.y - a.y) -
    (c.x - a.x) * (b.y - a.y);

  if (area > 0.0000000001) return 1;
  else if (area < -0.0000000001) return -1;
  else return 0;

}



//--------------------------------------------------------------------
//Rotate2: Rotates the face in a clockwise direction by angle
//--------------------------------------------------------------------
Face MathOps::Rotate2(Face face, double angle)
{
  return Rotate(face, sin(angle), cos(angle));
}


//--------------------------------------------------------------------
//Rotate: Rotates the face in clockwise direction, given the cosine and
//        sine of an angle
//--------------------------------------------------------------------
Face MathOps::Rotate(Face face, double sint, double cost)
{
  Face f;

  for (int i=0; i < 3; i++)
    {
      f.v[i].p.x=(cost*face.v[i].p.x +sint*face.v[i].p.y);
      f.v[i].p.y=(-sint*face.v[i].p.x + cost*face.v[i].p.y);
      f.v[i].label=face.v[i].label; 
    }

  return f;
}

//--------------------------------------------------------------------
//Length: Returns the lengths between two points
//--------------------------------------------------------------------
double MathOps::Length(Point a, Point b)
{
  return sqrt((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y));
}
