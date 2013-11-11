// This is brcv/rec/dbkpr/pro/dbkpr_span_match_process.cxx

//:
// \file

#include "dbkpr_span_match_process.h"
#include <dbdet/dbdet_lowe_keypoint.h>
#include <dbkpr/dbkpr_view_span_tree.h>

#include <dbdet/pro/dbdet_keypoint_storage.h>
#include <dbkpr/pro/dbkpr_corr3d_storage.h>

#include <bpro1/bpro1_parameters.h>
#include <vpgl/vpgl_perspective_camera.h>


//: Constructor
dbkpr_span_match_process::dbkpr_span_match_process()
{
  if( !parameters()->add( "Max bins searched"        , "-max_bins"  , (int)50 ) ||
      !parameters()->add( "Number of RANSAC samples" , "-ran_sam"   , (int)20 ) ||
      !parameters()->add( "F constraint std. dev."   , "-std"       , 1.0f ) ) {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbkpr_span_match_process::~dbkpr_span_match_process()
{
}


//: Clone the process
bpro1_process*
dbkpr_span_match_process::clone() const
{
  return new dbkpr_span_match_process(*this);
}


//: Return the name of this process
vcl_string
dbkpr_span_match_process::name()
{
  return "Global Match Keypoints";
}


//: Return the number of input frame for this process
int
dbkpr_span_match_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbkpr_span_match_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbkpr_span_match_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "keypoints" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbkpr_span_match_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "keypoints" );
  to_return.push_back( "keypoints_corr3d" );
  return to_return;
}


//: Execute the process
bool
dbkpr_span_match_process::execute()
{
  return true;
}




bool
dbkpr_span_match_process::finish()
{

  // cast the storage classels
  vcl_vector<vcl_vector< dbdet_keypoint_sptr > > all_keypoints;
  for(unsigned int i=0; i<input_data_.size(); ++i){
    dbdet_keypoint_storage_sptr frame_keypoints;
    frame_keypoints.vertical_cast(input_data_[i][0]);
    all_keypoints.push_back(frame_keypoints->keypoints());
  }


  int max_bins = 0, num_samples = 0;
  float std = 0.0f;
  parameters()->get_value( "-max_bins" , max_bins );
  parameters()->get_value( "-ran_sam"  , num_samples );
  parameters()->get_value( "-std"      , std );
  dbkpr_view_span_tree_sptr span_ptr = new dbkpr_view_span_tree(all_keypoints, max_bins, std, num_samples);

  dbkpr_view_span_tree &span = *span_ptr;

  //vcl_cout << span.match_matrix() << vcl_endl;

  // compute the global correspondences
  vcl_vector<vcl_vector<dbdet_keypoint_sptr> > matches = span.global_correspondence();

  // compute the global correspondence points
  vcl_vector<dbdet_keypoint_corr3d_sptr> corr_pts = span.global_corr3d_points();

  vpgl_calibration_matrix<double> K(2000.0,vgl_homg_point_2d<double>(512,384));
  vgl_rotation_3d<double> R;
  vgl_point_3d<double> center(0,0,-10);

  // pack up the results into storage classes
  dbdet_keypoint_storage_sptr in_storage;
  for(unsigned int i=0; i<input_data_.size(); ++i){
    vpgl_perspective_camera<double> *default_cam = new vpgl_perspective_camera<double>(K,center,R);
    in_storage.vertical_cast(input_data_[i][0]);
    dbdet_keypoint_storage_sptr out_storage1 = dbdet_keypoint_storage_new(matches[i],in_storage->ni(),in_storage->nj());
    output_data_[i].push_back(out_storage1);
    dbkpr_corr3d_storage_sptr out_storage2 = dbkpr_corr3d_storage_new(corr_pts, in_storage->ni(),in_storage->nj());
    out_storage2->set_camera(default_cam);
    out_storage2->set_tree(span_ptr);
    output_data_[i].push_back(out_storage2);
  }


  return true;
}



