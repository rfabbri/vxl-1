// This is brcv/seg/dbdet/pro/dbdet_lowe_keypoint_process.cxx

//:
// \file

#include "dbdet_lowe_keypoint_process.h"
#include "dbdet_keypoint_storage.h"
#include <dbdet/dbdet_lowe_keypoint.h>
#include <dbdet/dbdet_scale_space_peaks.h>
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>


//: Constructor
dbdet_lowe_keypoint_process::dbdet_lowe_keypoint_process()
{
  if( !parameters()->add( "Prior Smoothing Sigma" , "-prior_sigma"     , 1.6f ) ||
      !parameters()->add( "Max Curvature Ratio"   , "-max_curve_ratio" , 10.0f ) ||
      !parameters()->add( "Contrast Threshold"    , "-contrast_thresh" , 0.03f ) ||
      !parameters()->add( "Orientation Bins"      , "-orient_bins"     , 36    ) ||
      !parameters()->add( "Orientation Spread"    , "-orient_sigma"    , 1.5f  )) {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbdet_lowe_keypoint_process::~dbdet_lowe_keypoint_process()
{
}


//: Clone the process
bpro1_process*
dbdet_lowe_keypoint_process::clone() const
{
  return new dbdet_lowe_keypoint_process(*this);
}


//: Return the name of this process
vcl_string
dbdet_lowe_keypoint_process::name()
{
  return "Lowe Keypoints";
}


//: Return the number of input frame for this process
int
dbdet_lowe_keypoint_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbdet_lowe_keypoint_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbdet_lowe_keypoint_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbdet_lowe_keypoint_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "keypoints" );
  return to_return;
}


//: Execute the process
bool
dbdet_lowe_keypoint_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbdet_lowe_keypoint_process::execute() - not exactly one"
             << " input images \n";
    return false;
  }
  clear_output();

  // get image from the storage class
  vidpro1_image_storage_sptr frame_image;
  frame_image.vertical_cast(input_data_[0][0]);

  vil_image_resource_sptr image_sptr = frame_image->get_image();
  vil_image_view<vxl_byte> image = vil_convert_to_grey_using_rgb_weighting(image_sptr->get_view());

  float prior_sigma = 0.0f, max_curve_ratio = 0.0f, contrast_thresh = 0.0f, orient_sigma = 0.0f;
  int orient_bins = 0;
  parameters()->get_value( "-prior_sigma"     , prior_sigma     );
  parameters()->get_value( "-max_curve_ratio" , max_curve_ratio );
  parameters()->get_value( "-contrast_thresh" , contrast_thresh );
  parameters()->get_value( "-orient_bins"     , orient_bins     );
  parameters()->get_value( "-orient_sigma"    , orient_sigma    );


  // Cast into float and upsample by 2x
  vil_image_view<float> image2x;
  float dummy=0.0;
  vil_image_resource_sptr image_rsc = vil_new_image_resource_of_view(image);
  vil_image_view_base_sptr imagef = vil_convert_stretch_range(dummy, image_rsc->get_view());

  // determine the number of scale octaves
  int min_size = (image.ni() < image.nj())?image.ni():image.nj();
  unsigned num_octaves = 2;
  while( (min_size/=2) >= 8) ++num_octaves;

  // build the gaussian and difference of gaussian scale images
  bil_scale_image<float> gauss(3,num_octaves,prior_sigma,-1), dog(3,num_octaves,prior_sigma,-1);
  gauss.build_gaussian(imagef,&dog);
  // build the gradient scale space images
  bil_scale_image<float> g_dir, g_mag;
  gauss.compute_gradients(g_dir, g_mag);

  // detect peaks in the scale space
  vcl_vector<vgl_point_3d<float> > peak_pts;
  dbdet_scale_space_peaks(dog, peak_pts, max_curve_ratio, contrast_thresh);

  // compute orientations and descriptors at each scale peak to make a Lowe keypoint
  vcl_vector< dbdet_keypoint_sptr > keypoints;
  dbdet_ssp_orientation_params o_params(g_dir, g_mag, orient_bins, orient_sigma);//, float thresh=0.8f);
  for(unsigned int i=0; i<peak_pts.size(); ++i)
  {
    vgl_point_3d<float>& pt = peak_pts[i];
    vcl_vector<float> orientations = dbdet_ssp_orientations(pt, o_params);
    for(unsigned int j=0; j<orientations.size(); ++j)
    {
      dbdet_lowe_keypoint* kp = new dbdet_lowe_keypoint(pt.x(), pt.y(), pt.z(),
                                                        orientations[j]);
      kp->compute_descriptor(g_dir, g_mag);
      keypoints.push_back(kp);
    }
  }

  // create the output storage class
  dbdet_keypoint_storage_sptr output_storage = dbdet_keypoint_storage_new( keypoints,
                                                                           image.ni(), image.nj() );
  output_data_[0].push_back(output_storage);
  
  return true;
}



bool
dbdet_lowe_keypoint_process::finish()
{
  return true;
}



