// This is brcv/mvg/dbvrl/pro/dbvrl_video_process.cxx

//:
// \file

#include "dbvrl_video_process.h"
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
#include <vil/vil_new.h>
#include <vimt/vimt_transform_2d.h>
#include <vimt/vimt_resample_bilin.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vul/vul_timer.h>
#include <vnl/vnl_matrix.h>


//: Constructor
dbvrl_video_process::dbvrl_video_process()
 : first_frame_(true)
{
  if( !parameters()->add( "Border Size (pixels)" , "-border" , (int)10 ) ||
      !parameters()->add( "First Frame" ,          "-first" ,  (bool)first_frame_ )) {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbvrl_video_process::~dbvrl_video_process()
{
}


//: Clone the process
bpro1_process*
dbvrl_video_process::clone() const
{
  return new dbvrl_video_process(*this);
}


//: Return the name of this process
vcl_string
dbvrl_video_process::name()
{
  return "Register Video";
}


//: Return the number of input frame for this process
int
dbvrl_video_process::input_frames()
{
  return 2;
}


//: Return the number of output frames for this process
int
dbvrl_video_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbvrl_video_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbvrl_video_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "bvrl" );
  to_return.push_back( "bvrl" );
  return to_return;
}


//: Execute the process
bool
dbvrl_video_process::execute()
{
  if ( input_data_.size() != 2 ){
    vcl_cout << "In dbvrl_video_process::execute() - not exactly two"
             << " input frames \n";
    return false;
  }
  clear_output();

  static dbvrl_region_sptr curr_region = NULL;

  if(input_data_[0][0]->frame() == 0){
    vidpro1_image_storage_sptr frame_image;
    frame_image.vertical_cast(input_data_[0][0]);
    vil_image_resource_sptr image = frame_image->get_image();
    vidpro1_image_storage_new output_image_storage;
    output_image_storage->set_image(image);
    output_data_[0].push_back(output_image_storage);

    curr_region = new dbvrl_region( image->ni(), image->nj() );
    dbvrl_transform_2d_sptr xform = new dbvrl_transform_2d();
    xform->set_zoom_only( image->ni(), image->nj(), 0.0, 0.0 );
    dbvrl_region_storage_new output_dbvrl_storage( curr_region, xform ); 
    output_data_[0].push_back(output_dbvrl_storage);
  
    return true;
  }

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

  unsigned int ni = curr_image.ni();
  unsigned int nj = curr_image.nj();
  assert(ni == last_image.ni() && nj == last_image.nj());

  vimt_transform_2d init_xform;
  vnl_matrix<double> A(2,3);
  A[0][0] = 1.0;  A[0][1] = 0.0;  A[0][2] = 0.0;
  A[1][0] = 0.0;  A[1][1] = 1.0;  A[1][2] = 0.0;
  init_xform.set_affine(A);
  
  
  if ( !curr_region ){
    curr_region = new dbvrl_region( ni, nj );
  }


  int border = 10;
  parameters()->get_value( "-border" , border );
  
  dbvrl_world_roi roi(ni- 2*border, nj- 2*border,
                     vgl_point_2d<double>(border,border),
                     vgl_vector_2d<double>(0.99,0.0),
                     vgl_vector_2d<double>(0.0,0.99));

  vimt_image_2d_of<float> last_img(last_image, vimt_transform_2d());
  vimt_image_2d_of<float> curr_img(curr_image, init_xform);
  dbvrl_minimizer minimizer(last_img, curr_img, roi);
  
  vul_timer time;
  minimizer.minimize(init_xform);

  static vimt_transform_2d total_xform;
  parameters()->get_value( "-first" , first_frame_ );
  if(first_frame_){
    total_xform.set_identity();
    parameters()->set_value( "-first" , false );
  }
  total_xform = total_xform * init_xform.inverse();

  int frame = input_data_[0][0]->frame()+1;
  vcl_cout << "rel_homog{"<<frame<<"} = [ " << init_xform.matrix() <<" ];"<< vcl_endl;
  vcl_cout << "abs_homog{"<<frame<<"} = [ " << total_xform.matrix() <<" ];"<< vcl_endl;
  vcl_cerr << " in " << time.real() << " msec" << vcl_endl;

  
  vimt_transform_2d default_xform;
  default_xform.set_zoom_only( ni, nj, 0.0, 0.0 );

  // create and store the dbvrl output
  dbvrl_transform_2d_sptr xform1 = new dbvrl_transform_2d(total_xform*default_xform);
  dbvrl_region_storage_new output_storage1( curr_region, xform1 ); 
  output_data_[0].push_back(output_storage1);

  // create and store the dbvrl output
  dbvrl_transform_2d_sptr xform2 = new dbvrl_transform_2d(init_xform*default_xform);
  dbvrl_region_storage_new output_storage2( curr_region, xform2 ); 
  output_data_[0].push_back(output_storage2);
  
  return true;
}



bool
dbvrl_video_process::finish()
{
  return true;
}



