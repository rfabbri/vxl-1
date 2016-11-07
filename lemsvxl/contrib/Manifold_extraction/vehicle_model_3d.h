//---------------------------------------------------------------------
// \file
// \brief a class for representing three box models fitted to vehicle vrml files
//
//
// \author
//  Pradeep
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------


#ifndef vehicle_model_3d_h_
#define vehicle_model_3d_h_
#include <vgl/vgl_box_3d.h>
#include <vnl/vnl_matrix.h>

class vehicle_model_3d

    {
    public:
        // default constructor is n't meaningful !!
        vehicle_model_3d(){};
        vehicle_model_3d(vgl_box_3d<double> hood,vgl_box_3d <double> cab,vgl_box_3d <double> bed);


vgl_box_3d<double>  hood() const{return hood_;}
vgl_box_3d<double>  cab() const{return cab_;}
vgl_box_3d<double>  bed() const{return bed_;}
void transform_model(vnl_matrix<double>const& m1,vnl_matrix<double>const& m2,vnl_matrix<double>const& m3);

void print_vrml();

// void print_vrml(vcl_ofstream &ofstr);
    protected:
        vgl_box_3d <double> hood_,cab_,bed_;

    };

void transform_component(vnl_matrix<double>const & m,vgl_box_3d<double>& box);



#endif

