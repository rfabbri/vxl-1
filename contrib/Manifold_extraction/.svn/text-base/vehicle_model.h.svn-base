//---------------------------------------------------------------------
// \file
// \brief a class for representing three box models fitted to vehicle images 
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

#ifndef vehicle_model_h_
#define vehicle_model_h_
#include <vgl/vgl_box_2d.h>
#include <vsol/vsol_rectangle_2d.h>
#include <vsol/vsol_rectangle_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vnl/vnl_matrix.h>

class vehicle_model

    {
    public:
        // default constructor is n't meaningful !!
        vehicle_model(){};
        vehicle_model(vsol_rectangle_2d engine,vsol_rectangle_2d  body,vsol_rectangle_2d  rear);


        vsol_rectangle_2d   engine(){return engine_;}
    vsol_rectangle_2d   body(){return body_;}
vsol_rectangle_2d   rear(){return rear_;}

vcl_ostream& print(vcl_ostream& out) const;
vcl_istream& read(vcl_istream& in) const;

void vrml_print(vcl_ostream& out);

void transform_model(vnl_matrix<double>const& m1,vnl_matrix<double>const& m2,vnl_matrix<double>const& m3);
void set_model(vsol_rectangle_2d engine,vsol_rectangle_2d  body,vsol_rectangle_2d  rear);

//draw the model into an image whose dimensions are ni,nj and return the vil_image_view of it 
//   vil_image_view<double> draw_model(unsigned int ni,unsigned int nj);

    protected:
        vsol_rectangle_2d  engine_,body_,rear_;

    };

void transform_component(vnl_matrix<double>const & m,vsol_rectangle_2d & box);

vcl_ostream&  operator<<(vcl_ostream& out,vehicle_model const& M);
vcl_istream&  operator>>(vcl_istream& in,vehicle_model &M);
#endif

