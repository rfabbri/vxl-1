#ifndef dbdet_temporal_utils_h
#define dbdet_temporal_utils_h

#include <vgl/vgl_vector_2d.h>
#include <dbdet/sel/dbdet_edgel.h>

class dbdet_temporal_utils{


public:

    static double perp_distance(vgl_vector_2d<double> p, vgl_vector_2d<double> n);
    static bool compute_beta(dbdet_edgel* e, dbdet_curvelet * c, double & beta);
    static bool compute_backward_beta(dbdet_curvelet * c,dbdet_edgel* e,  double & beta, vgl_vector_2d<double> & p1);

    //
    static bool cartesian2spherical(double x, double y, double z, double & theta, double & phi, double & r);
};


#endif
