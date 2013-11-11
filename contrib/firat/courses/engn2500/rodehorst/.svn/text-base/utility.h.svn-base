/*******************************************************************************
* 
*******************************************************************************/

#ifndef UTILITY_H
#define UTILITY_H

/******************************************************************************/
/* Global Constants */

const unsigned int DIM = 3;
const unsigned int TRI_SIZE = 3;

/******************************************************************************/
/* Function Prototypes */

// in utility.cxx:
double DotProduct(const double v1[DIM],
                  const double v2[DIM]);

void CrossProduct(const double v1[DIM],
                  const double v2[DIM],
                  double out[DIM]);

void Normalize(double v[DIM]);

double Distance(const double v1[DIM],
                const double v2[DIM]);

double Magnitude(const double v[DIM]);

// in make_normals.cxx
void GetTriangleNormal(const double * coords,
                       const unsigned int * poly_idxs,
                       double normal[DIM]);

double * MakeVertexNormals(const double * vertex_coords,
                           const unsigned int num_vertices,
                           const unsigned int * polygons,
                           const unsigned int num_polygons,
                           const double * poly_normals);

#endif
