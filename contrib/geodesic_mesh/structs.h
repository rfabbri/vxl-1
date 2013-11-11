//------------------------------------------------------
//Biliana Kaneva, Smith College
//structs.h
//
//This file contains the declaration of the basic
//structures used in the shortest path program
// 
//Point
//Vertex
//Face
//------------------------------------------------------

/*--------------------------------------------------------------------
This code is Copyright 2000 by Biliana Kaneva & Joseph O'Rourke.  
It may be freely redistributed in its entirety provided that this 
copyright notice is not removed.  Question to orourke@cs.smith.edu.
--------------------------------------------------------------------*/


#ifndef STRUCTS_H
#define STRUCTS_H

#include <vcl_iostream.h>
#include <vcl_cstddef.h>

///#define MEM_COUNT

#ifdef MEM_COUNT
extern size_t maxmemory;
extern size_t maxm;
#endif

//------------------------------------------------------------
// structure for a point in 2D
class Point{
public:
  double x;
  double y;

  friend vcl_ostream& operator << (vcl_ostream& os, Point& p)
    {
      
      os << p.x << "\t" << p.y;
      return os;
    };

  friend void operator += (Point &p1, Point &p2)
    {
      p1.x+=p2.x;
      p1.y+=p2.y;
    };

  friend void operator -= (Point &p1, Point &p2)
    {
      p1.x-=p2.x;
      p1.y-=p2.y;
    };
  
  friend Point operator + (const Point &p1, const Point &p2)
    {
      Point temp;
      temp.x=p1.x+p2.x;
      temp.y=p1.y+p2.y;
      return temp;
    };

  /*friend Point operator + (Point p1, Point p2)
    {
      Point temp;
      temp.x=p1.x+p2.x;
      temp.y=p1.y+p2.y;
      return temp;
    };*/

  friend Point operator * (double s, const Point &p)
    {
      Point temp;
      temp.x = s*p.x;
      temp.y = s*p.y;
      return temp;
    }
  /*friend Point operator * (double s, Point p)
    {
      Point temp;
      temp.x = s*p.x;
      temp.y = s*p.y;
      return temp;
    }*/

  friend Point operator / (Point &p1, Point &p2)
    {
      Point temp;
      temp.x=p1.x/p2.x;
      temp.y=p1.y/p2.y;
      return temp;
    };

  friend Point operator - (Point &p1, Point &p2)
    {
      Point temp;
      temp.x=p1.x-p2.x;
      temp.y=p1.y-p2.y;
      return temp;
    };

  friend vcl_istream& operator >> (vcl_istream& is, Point &p)
    {
      is >> p.x >> p.y;
      return is;
    };

};

//-------------------------------------------------------------
//structure for a vertex - contains a point in 2D and a label
// (global index), corresponding to the label of the vertex 
// in the polytope
struct Vertex {
  Point p;
  int label;

  friend vcl_ostream& operator << (vcl_ostream& os, Vertex& v)
    {      
      os << v.label << "\t" << v.p;
      return os;
    };
};

//-------------------------------------------------------------
// structure for a triangular face - three vertices
struct Face{

  Vertex v[3];
  
  friend vcl_ostream& operator << (vcl_ostream& os, Face& f)
    {
      for (int i=0; i < 3; i++)
  os << f.v[i].label << " (" << f.v[i].p.x << "," << f.v[i].p.y  << ") ";
      os << vcl_endl;
      return os;
    };

  friend vcl_istream& operator >> (vcl_istream& is, Face &f)
    {
      for (int i=0; i < 3; i++)
    is >> f.v[i].p >> f.v[i].label;
      return is;
    }
    
};






#endif
