#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vcl_cassert.h>
#include "dbgl_diffgeom.h"

void inverse3(double Ta, double Tb, double (&inv)[3][3])
{
    //Should remove operations taking 1.0s and 0.0s later
    double m[9] = {1.0, -Ta, Ta * Ta, 1.0, 0.0, 0.0, 1.0, -Tb, Tb * Tb};
    inv[0][0] = m[4] * m[8] - m[7] * m[5];
    inv[0][1] = -(m[3] * m[8] - m[6]* m[5]);
    inv[0][2] = m[3] * m[7] - m[6] * m[4];
    inv[1][0] = -(m[1] * m[8] - m[7] * m[2]);
    inv[1][1] = m[0] * m[8] - m[6] *m[2];
    inv[1][2] = -(m[0] * m[7] - m[6] * m[1]);
    inv[2][0] = m[1] * m[5] - m[4] * m[2];
    inv[2][1] = -(m[0] * m[5] - m[3] * m[2]);
    inv[2][2] = m[0] * m[4] - m[3] * m[1];

    double invDet = m[0] * m[4] * m[8] - m[0] * m[7] * m[5] - m[3] * m[1] * m[8] + m[3] * m[7] * m[2] + m[6] * m[1] * m[5] - m[6] * m[4] * m[2];
    invDet = 1.0 / invDet;
   
    inv[0][0] *= invDet;
    inv[0][1] *= invDet;
    inv[0][2] *= invDet;
    inv[1][0] *= invDet;
    inv[1][1] *= invDet;
    inv[1][2] *= invDet;
    inv[2][0] *= invDet;
    inv[2][1] *= invDet;
    inv[2][2] *= invDet;
}

double compute_k(double xa, double x, double xb,
                 double ya, double y, double yb,
                 double inv[3][3])
{
    double a1 = inv[0][0] * xa + inv[1][0] * x + inv[2][0] * xb;
    double a2 = inv[0][1] * xa + inv[1][1] * x + inv[2][1] * xb;
    double a3 = inv[0][2] * xa + inv[1][2] * x + inv[2][2] * xb;
    double b1 = inv[0][0] * ya + inv[1][0] * y + inv[2][0] * yb;
    double b2 = inv[0][1] * ya + inv[1][1] * y + inv[2][1] * yb;
    double b3 = inv[0][2] * ya + inv[1][2] * y + inv[2][2] * yb;

    return 2.0 * (a2 * b3 - a3 * b2 ) / vcl_pow((a2 * a2 + b2 * b2), 3.0/2.0);
}

double compute_dist(double x1, double x2, double y1, double y2)
{
    return vcl_sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

void 
dbgl_compute_curvature(
    const vcl_vector< vgl_point_2d<double> > &vertices, 
    vnl_vector<double> *k
    )
{
    int size = vertices.size();
    assert(size >= 2);

    k->set_size(size);

    double inv[3][3];
    double xa, x, xb, ya, y, yb, ta, tb;

    xb = vertices[0].x(); x = vertices[1].x(); xa = vertices[2].x();
    yb = vertices[0].y(); y = vertices[1].y(); ya = vertices[2].y();
    
    ta = compute_dist(xb, x, yb, y);
    tb = compute_dist(xa, xb, ya, yb);

    inverse3(-ta, -tb, inv);
    (*k)[0] = compute_k(vertices[1].x(), vertices[0].x(), vertices[2].x(),
                        vertices[1].y(), vertices[0].y(), vertices[2].y(),
                        inv);

    for (int i = 1; i < size - 1; ++i)
    {
        xa = vertices[i+1].x();
        ya = vertices[i+1].y();
        
        tb = ta;
        ta = compute_dist(xa, x, ya, y);

        inverse3(-ta, tb, inv);
        (*k)[i] = compute_k(vertices[i+1].x(), vertices[i].x(), vertices[i-1].x(),
                            vertices[i+1].y(), vertices[i].y(), vertices[i-1].y(),
                            inv);
        x = xa; y = ya;
    }
    
    tb = ta;
    ta = compute_dist(x, vertices[size-3].x(), y, vertices[size-3].y());

    inverse3(ta, tb, inv);
    (*k)[size-1] = compute_k(vertices[size-3].x(), vertices[size-1].x(), vertices[size-2].x(),
                        vertices[size-3].y(), vertices[size-1].y(), vertices[size-2].y(),
                        inv);
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
