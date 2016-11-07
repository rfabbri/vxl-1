#ifndef biosim_deformed_torus_h
#define biosim_deformed_torus_h
//: 
// \file     biosim_deformed_torus.h
// \brief    Generates a deformed torus based on the given paramaters. 
// \author   Gamze D. Tunali
// \date     Aug 10, 2006
// 

#include <vbl/vbl_array_3d.h>
#include <vil3d/vil3d_image_view.h>
#include <vcl_vector.h>

class biosim_deformed_torus

{
private:
    vbl_array_3d<double> volume_;
    vil3d_image_view<unsigned char> img_view_;
    
public:
   //: constructor.. it has default values for the required parameters
   biosim_deformed_torus(int xdim, int ydim, int zdim,
        double a=4.0, double b=2.0, double c=20,
        double mean=255.0, double gaussian_sigma=0.0);

    vbl_array_3d<double> volume(){return volume_;}

    vil3d_image_view<unsigned char> view() {return img_view_; }
     
    int dimx() { return volume_.get_row1_count(); }

    int dimy() { return volume_.get_row2_count(); }

    int dimz() { return volume_.get_row3_count(); }



};
#endif       
                         
