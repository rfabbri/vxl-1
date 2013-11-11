// This is brl/vidpro1/process/vidpro1_homog_2d_transform_process.cxx

//:
// \file

//#include <vil/vil_math.h>
// brip_vil_float_ops.h
// #include <vgel/vgel_kl.h>
////#include <vil/vil_image_resource.h>
//#include <vil/vil_image_resource_sptr.h>
////#include <vil/vil_new.h>
////#include <vil/vil_image_view.h>
////#include <vil/vil_convert.h>
////#include <vil/vil_math.h>
#include<bpro1/bpro1_parameters.h>


#include "vidpro1_homog_2d_transform_process.h"

// #include <brip/brip_vil_float_ops.h>

//
//#include <vgl/vgl_homg_point_2d.h>
//#include <vgl/algo/vgl_h_matrix_2d.h>
//
//#include <vil/vil_image_resource.h>

//#include <vil/vil_new.h>
//#include <vil/vil_convert.h>
////
#include <vnl/vnl_det.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>


#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>


#include <bsol/bsol_algs.h>


//#include <vidpro1/storage/vidpro1_image_storage.h>
//#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
////
#include <en292/pro/vidpro1_homog_2d_storage.h>

//: Constructor
vidpro1_homog_2d_transform_process::vidpro1_homog_2d_transform_process() : bpro1_process()
{
  this->H_.set_identity();
  if( !parameters()->add( "Line x0=" , "-ht_x0" , (float)10) ||
      !parameters()->add( "Line y0=" , "-ht_y0" , (float)10) ||
      !parameters()->add( "Line x1=" , "-ht_x1" , (float)20) ||
      !parameters()->add( "Line y1=" , "-ht_y1" , (float)20)
    ) 
  {
    vcl_cerr << "ERROR: Adding parameters in vidpro1_klt_process::vidpro1_klt_process()" << vcl_endl;
  }
  else
  {    
  }
}


//: Destructor
vidpro1_homog_2d_transform_process::~vidpro1_homog_2d_transform_process()
{
}


//: Return the name of this process
vcl_string
vidpro1_homog_2d_transform_process::name()
{
  return "Homography 2D Transform";
}


//: Return the number of input frame for this process
int
vidpro1_homog_2d_transform_process::input_frames()
{
  return 1;
}



//: Return the number of output frames for this process
int
vidpro1_homog_2d_transform_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > vidpro1_homog_2d_transform_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  // to_return.push_back( "vsol2D" );
  to_return.push_back( "homog_2d" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > vidpro1_homog_2d_transform_process::get_output_type()
{  
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  return to_return;
}


//: Execute the process
bool
vidpro1_homog_2d_transform_process::execute()
{
  vcl_cout << "\nPerform homography transform on vsol2D objects" << vcl_endl;
  if ( this->input_data_.size() != 1 )
  {
    vcl_cout << "In vidpro1_homog_2d_transform_process::execute() - not exactly one"
             << " input frames \n";
    return false;
  }
  clear_output();

  // get input from storage classes
  vidpro1_homog_2d_storage_sptr homog_2d_storage;
  homog_2d_storage.vertical_cast(input_data_[0][0]);
  vgl_h_matrix_2d< double > H = homog_2d_storage->H();

  vcl_cout << "Homography matrix H = \n" << H << vcl_endl;
  vcl_cout << "Det(H) = " << vnl_det(H.get_matrix()) << vcl_endl;

  H = H * this->H();
  this->set_H(H);
  
  // Now we've got the homography matrix, apply it to the vsol2D line
  vcl_cout << "Apply homography to vsol2D line \n";
  
  // a hack for now
  // coordinate of a rectangle
  float x0=0, y0=0, x1=0, y1=0;
  this->parameters()->get_value("-ht_x0", x0);
  this->parameters()->get_value("-ht_y0", y0);
  this->parameters()->get_value("-ht_x1", x1);
  this->parameters()->get_value("-ht_y1", y1);

  vsol_point_2d_sptr p0(new vsol_point_2d(x0, y0));
  vsol_point_2d_sptr p1(new vsol_point_2d(x0, y1));
  vsol_point_2d_sptr p2(new vsol_point_2d(x1, y1));
  vsol_point_2d_sptr p3(new vsol_point_2d(x1, y0));

  vcl_vector< vsol_point_2d_sptr > pt_list;
  pt_list.push_back(p0);
  pt_list.push_back(p1);
  pt_list.push_back(p2);
  pt_list.push_back(p3);
  vsol_polygon_2d_sptr poly = new vsol_polygon_2d(pt_list);

  // apply homography on the polygon
  vsol_polygon_2d_sptr Hpoly = new vsol_polygon_2d();
  bsol_algs::homography(poly, H, Hpoly);

  // create the output storage class
  vidpro1_vsol2D_storage_sptr output_storage = vidpro1_vsol2D_storage_new();
  output_storage->add_object(Hpoly->cast_to_spatial_object());  
  output_data_[0].push_back(output_storage);


  //
  //// get input image from storage class
  //vidpro1_image_storage_sptr im_storage;
  //im_storage.vertical_cast(input_data_[0][0]);
  //vil_image_resource_sptr im_resource = im_storage->get_image();
  //vil_image_view< vxl_byte > im_view(im_resource->get_view());
  //// just for now
  //// convert to grey image
  //vil_image_view< float > im_view_float;
  //vil_convert_planes_to_grey< vxl_byte, float >(im_view, im_view_float);
  //vil_image_view< float > return_im_view;
  //return_im_view.deep_copy(im_view_float);

  //if (! brip_vil_float_ops::homography(im_view_float, H_inv, return_im_view, true, 0.0f))
  //{
  //  vcl_cout << "Homography transform failed" << vcl_endl;
  //  return false;
  //}
  //
  //vcl_cout << "Homography succeeded " << vcl_endl;
  //return_im_view.print(vcl_cout);
 
  //// create the output storage class
  //vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
  //vil_image_resource_sptr return_im_resource = vil_new_image_resource_of_view(return_im_view);
  //output_storage->set_image(return_im_resource);  
  //output_data_[0].push_back(output_storage);

  return true;
}


//: Clone the process
bpro1_process*
vidpro1_homog_2d_transform_process::clone() const
{
  vidpro1_homog_2d_transform_process* to_return = new vidpro1_homog_2d_transform_process(*this);
  to_return->H_.set_identity();
  return (bpro1_process*)to_return;
  //   return new vidpro1_homog_2d_transform_process(*this);
}
    
bool
vidpro1_homog_2d_transform_process::finish()
{
  return true;
}




