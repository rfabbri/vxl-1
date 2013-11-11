#ifndef dbrl_clough_tocher_patch_h_
#define dbrl_clough_tocher_patch_h_

#include<vcl_iostream.h>
#include<vgl/vgl_point_3d.h>
#include<vgl/vgl_point_2d.h>
#include <vcl_cmath.h>

class dbrl_clough_tocher_patch
{
public:   
    dbrl_clough_tocher_patch();
    ~dbrl_clough_tocher_patch();

    void set_points(vgl_point_3d<double> pt1,
                    vgl_point_3d<double> pt2,
                    vgl_point_3d<double> pt3);

    void set_partials(vgl_vector_2d<double> fxy1,
                      vgl_vector_2d<double> fxy2,
                      vgl_vector_2d<double> fxy3);
    
    void set_cross_bnd_derivative(double f1nxy,
                                  double f2nxy,
                                  double f3nxy);


    //; p1p2c -tri 1
    //; p2p3c -tri 2
    //; p3p1c -tri 3

    bool compute_cubic_control_points();

    double interpolate(vgl_point_2d<double> p) const;

    bool intriangle(vgl_point_2d<double> p) const;

    //: inout is the set of ten coefficients and barcyentric coordinates of the point 
    //: under consideration
    double compute_value(const double coeffs[10],vgl_vector_3d<double> bcp) const;

    //: store 3 cubic polynomials 
    //: each of the polynomial has ten coefficients

protected:
        vgl_point_3d<double> p1;
    vgl_point_3d<double> p2;
    vgl_point_3d<double> p3;
    //: partial derivatives w.r.t x and y.
    vgl_vector_2d<double> f1;
    vgl_vector_2d<double> f2;
    vgl_vector_2d<double> f3;

    double f1n,f2n,f3n;
    //: centroid
    vgl_point_3d<double> c;

    double coeffs1[10];
    double coeffs2[10];
    double coeffs3[10];

    vgl_vector_3d<double> convert_to_barycentric(vgl_point_2d<double> p,
                                                    vgl_point_2d<double> p1,
                                                    vgl_point_2d<double> p2,
                                                    vgl_point_2d<double> p3) const;

        int find_mini_triangle(vgl_vector_3d<double> p) const;




};
#endif
