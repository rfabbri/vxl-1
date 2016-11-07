#include <iostream>
using namespace std;
#include <stdlib.h>

#include <math.h>
#include "utility.h"

/******************************************************************************/

void GetTriangleNormal(const double * coords,
                       const unsigned int * poly_idxs,
                       double normal[DIM])
{
  double a[DIM];
  double b[DIM];
  
  for(unsigned int dim = 0; dim < DIM; dim++)
  {
    a[dim] = coords[DIM*poly_idxs[0] + dim] - 
             coords[DIM*poly_idxs[1] + dim];
    b[dim] = coords[DIM*poly_idxs[2] + dim] -
             coords[DIM*poly_idxs[1] + dim];
  }
  
  CrossProduct(b, a, normal);
  
  Normalize(normal);
}




/******************************************************************************/

double * MakePolyNormals(const double * vertex_coords,
                         const unsigned int * polygons,
                         const unsigned int num_polygons)
{
  double * poly_normals = new double[DIM*num_polygons];
  
  for(int poly = 0; poly < num_polygons; poly++)
  {
    GetTriangleNormal(vertex_coords,
                      polygons + poly*DIM,
                      poly_normals + poly*DIM);
  }
  
  return poly_normals;
}




/******************************************************************************/

double * MakeVertexNormals(const double * vertex_coords,
                           const unsigned int num_vertices,
                           const unsigned int * polygons,
                           const unsigned int num_polygons,
                           const double * poly_normals)
{
  unsigned int vert, poly, pnt, coord;
  double a[DIM], b[DIM];
  double angle;
  unsigned int u, w;
  
  double * vertex_normals = new double[DIM*num_vertices];
  for(vert = 0; vert < DIM*num_vertices; vert++)
    vertex_normals[vert] = 0.0;
  
  bool * normal_valid = new bool[num_vertices];
  for(vert = 0; vert < num_vertices; vert++)
    normal_valid[vert] = 0;
  
  for(poly = 0; poly < num_polygons*TRI_SIZE; poly+=TRI_SIZE)
  {
    for(pnt = 0; pnt < TRI_SIZE; pnt++)
    {
      u = polygons[poly + (pnt+2)%TRI_SIZE];
      vert = polygons[poly + pnt];
      w = polygons[poly + (pnt+1)%TRI_SIZE];
      
      for(coord = 0; coord < DIM; coord++)
      {
        a[coord] = vertex_coords[DIM*u + coord] - 
                   vertex_coords[DIM*vert + coord];
        b[coord] = vertex_coords[DIM*w + coord] -
                   vertex_coords[DIM*vert + coord];
      }
      
      angle = acos(DotProduct(a, b) / (Magnitude(a) * Magnitude(b)));
      
      for(coord = 0; coord < DIM; coord++)
      {
        vertex_normals[DIM*vert + coord] +=
          angle*poly_normals[poly + coord];
      }
      normal_valid[vert] = 1;
      
/************************************
      if((vert == 142631) || (vert == 142632))
      {
        cout << "Bad Polygon Found: " << poly/TRI_SIZE << endl;
        for(unsigned int p = 0; p < DIM; p++)
        {
          cout << polygons[poly+p] << " (";
          cout << vertex_coords[DIM*polygons[poly+p]+0] << ", "
               << vertex_coords[DIM*polygons[poly+p]+1] << ", "
               << vertex_coords[DIM*polygons[poly+p]+2] << ")" << endl;
        }
        cout << "Normal: ("
             << poly_normals[poly + 0] << ", "
             << poly_normals[poly + 1] << ", "
             << poly_normals[poly + 2] << ") " << endl;
      }
************************************/
    }
  }

  // Normalize all valid vertex normals
  for(vert = 0; vert < num_vertices; vert++)
  {
    if(normal_valid[vert])
    {
      if(Magnitude(&vertex_normals[DIM*vert]) == 0.0)
      {
        cout << "Normal for vertex " << vert << " found to be zero." << endl
             << "Coords: " << vertex_coords[DIM*vert+0] << " "
             << vertex_coords[DIM*vert+1] << " "
             << vertex_coords[DIM*vert+2] << endl
             << "Setting to unit Z vector." << endl;
        vertex_normals[DIM*vert+0] = 0.0;
        vertex_normals[DIM*vert+1] = 0.0;
        vertex_normals[DIM*vert+2] = 1.0;
      }
      else
      {
        Normalize(&vertex_normals[DIM*vert]);
      }
    }
  }
  
  delete[](normal_valid);
  
  return vertex_normals;
}




/******************************************************************************/

