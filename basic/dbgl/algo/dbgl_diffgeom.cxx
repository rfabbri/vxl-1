#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include "dbgl_diffgeom.h"

void 
dbgl_compute_curvature(
    const vcl_vector< vgl_point_2d<double> > &vertices, 
    vcl_vector<double> *k
    )
{
    vcl_cout << "stub" << vcl_endl;
}

void dbgl_compute_normals(
    const vcl_vector< vgl_point_2d<double> > &vertices, 
    vcl_vector< vnl_vector_fixed<double, 2> > *n
    )
{
    int size = vertices.size();
    n->resize(size);
    for (int i = 0; i < size - 1; ++i)
    {
        double x = vertices[i].x() - vertices[i+1].x();
        double y = vertices[i].y() - vertices[i+1].y();
        double norm2 = (x * x + y * y);
        norm2 = (norm2 > DIFFGEOM_EPS) ? norm2 : DIFFGEOM_EPS;
        (*n)[i][0] = x / norm2;
        (*n)[i][1] = y / norm2;
    }
    (*n)[size - 1][0] = 0.0;
    (*n)[size - 1][1] = 0.0;

    //can be merged with first for later
    double lastx = 0.0;
    double lasty = 0.0;
    for (int i = 0; i < size; ++i)
    {
        double x = lastx;
        double y = lasty;
        lastx = (*n)[i][0];
        lasty = (*n)[i][1];

        x += lastx;
        y += lasty;
        double norm = vcl_sqrt(x * x + y * y);
        (*n)[i][0] = -y / norm;
        (*n)[i][1] = x / norm;
    }
}
