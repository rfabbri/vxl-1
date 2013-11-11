// This is brcv/mvg/dbvrl/pro/dbvrl_region_process.cxx

//:
// \file

#include "dbvrl_region_process.h"
#include "dbvrl_region_storage.h"
#include <vcl_algorithm.h>
#include <dbvrl/dbvrl_minimizer.h>
#include <dbvrl/dbvrl_world_roi.h>
#include <dbvrl/dbvrl_transform_2d.h>
#include <dbvrl/dbvrl_region.h>
#include <dbvrl/dbvrl_region_sptr.h>
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vimt/vimt_transform_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_distance.h>
#include <vul/vul_timer.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_identity_3x3.h>


//: Constructor
dbvrl_region_process::dbvrl_region_process()
{
  if( !parameters()->add( "Transformation Type" , "-type" , (int)5 ) ) {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbvrl_region_process::~dbvrl_region_process()
{
}


//: Clone the process
bpro1_process*
dbvrl_region_process::clone() const
{
  return new dbvrl_region_process(*this);
}


//: Return the name of this process
vcl_string
dbvrl_region_process::name()
{
  return "Register Region";
}


//: Return the number of input frame for this process
int
dbvrl_region_process::input_frames()
{
  return 2;
}


//: Return the number of output frames for this process
int
dbvrl_region_process::output_frames()
{
  return 0;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbvrl_region_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  to_return.push_back( "bvrl" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbvrl_region_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;
  return to_return;
}


//: Execute the process
bool
dbvrl_region_process::execute()
{
  if ( input_data_.size() != 2 ){
    vcl_cout << "In dbvrl_region_process::execute() - not exactly two"
             << " input frames \n";
    return false;
  }
  clear_output();

  static dbvrl_region_sptr curr_region = NULL;

  if(input_data_[0][0]->frame() == 0){
    // Do nothing
    return true;
  }

  static vimt_transform_2d delta_xform;

  // get images from the storage class
  vidpro1_image_storage_sptr frame_image;
  frame_image.vertical_cast(input_data_[0][0]);
  vil_image_resource_sptr image_sptr = frame_image->get_image();
  vil_image_view<float> curr_image = 
    vil_convert_to_grey_using_rgb_weighting(
      vil_convert_cast(float(0.0), image_sptr->get_view()) );

  frame_image.vertical_cast(input_data_[1][0]);
  image_sptr = frame_image->get_image();
  vil_image_view<float> last_image = 
    vil_convert_to_grey_using_rgb_weighting(
      vil_convert_cast(float(0.0), image_sptr->get_view()) );

  
  dbvrl_region_storage_sptr curr_dbvrl_stg;
  curr_dbvrl_stg.vertical_cast(input_data_[0][1]);

  dbvrl_region_storage_sptr last_dbvrl_stg;
  last_dbvrl_stg.vertical_cast(input_data_[1][1]);
  
  if( !curr_dbvrl_stg && !delta_xform.is_identity() ){
    dbvrl_transform_2d_sptr est_xform;
    est_xform = new dbvrl_transform_2d(delta_xform* (*last_dbvrl_stg->transform()));
    curr_dbvrl_stg = dbvrl_region_storage_new(last_dbvrl_stg->region(), est_xform);
    input_data_[0][1] = curr_dbvrl_stg;
  }
 

  vimt_transform_2d l_xform = *last_dbvrl_stg->transform();
  vgl_point_2d<double> points[4];
  points[0] = l_xform(vgl_point_2d<double>(0,0));
  points[1] = l_xform(vgl_point_2d<double>(1,0));
  points[2] = l_xform(vgl_point_2d<double>(1,1));
  points[3] = l_xform(vgl_point_2d<double>(0,1));
  int ni = (int) vcl_floor(vcl_max( vgl_distance(points[0],points[1]),
                                    vgl_distance(points[2],points[3]) )+0.5);
  
  int nj = (int) vcl_floor(vcl_max( vgl_distance(points[0],points[3]),
                                    vgl_distance(points[1],points[2]) )+0.5);

  vcl_cout << "max box = ("<<ni<<","<<nj<<")"<< vcl_endl;
  dbvrl_world_roi roi(ni,nj,vgl_point_2d<double>(0.0, 0.0),
                     vgl_vector_2d<double>(0.99,0.0),
                     vgl_vector_2d<double>(0.0,0.99));
  dbvrl_region reg(ni,nj);

  vimt_image_2d_of<float> last_img(last_image, *last_dbvrl_stg->transform() * reg.xform());
  vimt_image_2d_of<float> curr_img(curr_image, *curr_dbvrl_stg->transform() * reg.xform());
  dbvrl_minimizer minimizer(last_img, curr_img, roi);
  
  vimt_transform_2d init_xform;
  
  int xform_type=0;
  parameters()->get_value( "-type" , xform_type );
  switch(xform_type){
  case vimt_transform_2d::ZoomOnly:
    init_xform.set_zoom_only(1.0,1.0,0.0,0.0);
    assert(init_xform.form() == vimt_transform_2d::ZoomOnly);
    break;

  case vimt_transform_2d::RigidBody: 
    init_xform.set_rigid_body(1.0, 0.0, 0.0);
    init_xform = init_xform * init_xform.inverse();
    assert(init_xform.form() == vimt_transform_2d::RigidBody);
    break;

  case vimt_transform_2d::Similarity:
    init_xform.set_similarity( vgl_point_2d<double>(1.0,0.0),
                               vgl_point_2d<double>(0.0,0.0) );
    assert(init_xform.form() == vimt_transform_2d::Similarity);
    break;

  case vimt_transform_2d::Affine:
    {
      vnl_matrix<double> A(2,3);
      A[0][0] = 1.0;  A[0][1] = 0.0;  A[0][2] = 0.0;
      A[1][0] = 0.0;  A[1][1] = 1.0;  A[1][2] = 0.0;
      init_xform.set_affine(A);
    }
    assert(init_xform.form() == vimt_transform_2d::Affine);
    break;

  case vimt_transform_2d::Projective:
    init_xform.set_projective(vnl_identity_3x3());
    assert(init_xform.form() == vimt_transform_2d::Projective);
    break;

  default:
    init_xform.set_identity();
    init_xform.set_origin(vgl_point_2d<double>(0.0,0.0));
    assert(init_xform.form() == vimt_transform_2d::Translation);
    break;
  }
  assert(init_xform.matrix() == vnl_identity_3x3());


  vul_timer time;
  minimizer.minimize(init_xform);

  vcl_cout << init_xform.matrix() << vcl_endl;
  vcl_cout << " in " << time.real() << " msec" << vcl_endl;
  

  vimt_transform_2d new_xform = init_xform * (*curr_dbvrl_stg->transform());
  vnl_vector< double > params;
  new_xform.params(params);
  curr_dbvrl_stg->transform()->set(params, new_xform.form());

  delta_xform = new_xform * last_dbvrl_stg->transform()->inverse();
  
  return true;
}



bool
dbvrl_region_process::finish()
{
  return true;
}



