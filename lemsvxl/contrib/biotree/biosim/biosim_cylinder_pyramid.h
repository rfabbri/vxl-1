#ifndef biosim_cylinder_pyramid_h
#define biosim_cylinder_pyramid_h
//: 
// \file  biosim_cylinder_pyramid.h
// \brief  
// \author   Pradeep
// \date     2006-07-20
// 

#include <vbl/vbl_array_3d.h>
#include <vcl_vector.h>

class biosim_cylinder_pyramid

{
private:
    vbl_array_3d<double> volume_;
    vcl_vector<double> radii_;


public:

    biosim_cylinder_pyramid(double density,double gaussian_sigma);

    vbl_array_3d<double> volume(){return volume_;}
     
    void radii_range(double &min_radius,double &max_radius);

    int dim1() { return volume_.get_row1_count(); }

    int dim2() { return volume_.get_row2_count(); }

    int dim3() { return volume_.get_row3_count(); }



};
#endif       
                         
