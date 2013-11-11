// This is contrib/ntrinh/en292/pro/vidpro1_compute_homography_2d_process.cxx

//:
// \file


#include "vidpro1_compute_homography_2d_process.h"


#include <vil/vil_image_resource.h>
#include <vil/vil_flip.h>
#include <vil/vil_new.h>
#include <vil/vil_convert.h>

#include <vgl/vgl_homg_point_2d.h>
#include <vgl/algo/vgl_h_matrix_2d.h>

#include <vnl/vnl_det.h>

#include <rrel/rrel_homography2d_est.h>
#include <rrel/rrel_ransac_obj.h>
#include <rrel/rrel_ran_sam_search.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>

#include <brip/brip_vil_float_ops.h>

#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

#include <en292/pro/vidpro1_homog_2d_storage.h>

//: Constructor
vidpro1_compute_homography_2d_process::vidpro1_compute_homography_2d_process() : bpro1_process()
{
}


//: Destructor
vidpro1_compute_homography_2d_process::~vidpro1_compute_homography_2d_process()
{
}


//: Return the name of this process
vcl_string
vidpro1_compute_homography_2d_process::name()
{
  return "Compute Plane Homography";
}


//: Return the number of input frame for this process
int
vidpro1_compute_homography_2d_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
vidpro1_compute_homography_2d_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > vidpro1_compute_homography_2d_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > vidpro1_compute_homography_2d_process::get_output_type()
{  
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "homog_2d" );
  return to_return;
}


//: Execute the process
bool
vidpro1_compute_homography_2d_process::execute()
{
  vcl_cout << "\n Perform Homography estimation between current frame and previous frame" << vcl_endl;
  if ( this->input_data_.size() != 1 )
  {
    vcl_cout << "In vidpro1_compute_homography_2d_process::execute() - not exactly one"
             << " input frames \n";
    return false;
  }
  clear_output();

  if (this->input_data_[0].size() != 3 )
  {
    vcl_cout << "In vidpro1_compute_homography_2d_process::execute() - not exactly 2 vsol2Ds and 1 image"
             << " in input frame \n";
  }
  // get input vsol points from storage class
  vidpro1_vsol2D_storage_sptr from_storage, to_storage;
  from_storage.vertical_cast(input_data_[0][0]);
  to_storage.vertical_cast(input_data_[0][1]);

  // set up input data for computing homography using rrel library
  vcl_vector< vgl_homg_point_2d< double > > from_pts;
  vcl_vector< vgl_homg_point_2d< double > > to_pts;

  // from_points - points in previous frame
  vcl_vector< vsol_spatial_object_2d_sptr > from_vsol = from_storage->all_data();
  vcl_vector< vsol_spatial_object_2d_sptr >::iterator vsol_it;
  for (vsol_it = from_vsol.begin(); vsol_it != from_vsol.end(); vsol_it ++)
  {
    if (! (*vsol_it)->cast_to_point())
      continue;
    vsol_point_2d_sptr pt = (*vsol_it)->cast_to_point();
    vgl_homg_point_2d< double > homg_pt(pt->x(), pt->y());
    from_pts.push_back(homg_pt);
  }

  // to-points - tracked points in current frame
  vcl_vector< vsol_spatial_object_2d_sptr > to_vsol = to_storage->all_data();
  for (vsol_it = to_vsol.begin(); vsol_it != to_vsol.end(); vsol_it ++)
  {
    if (! (*vsol_it)->cast_to_point())
      continue;
    vsol_point_2d_sptr pt = (*vsol_it)->cast_to_point();
    vgl_homg_point_2d< double > homg_pt(pt->x(), pt->y());
    to_pts.push_back(homg_pt);
  }


  // check input and output validity
  if (from_pts.size() != to_pts.size())
  {
    vcl_cout << "Number of from_pts is not the same as number of to_pts" << vcl_endl;
    return false;
  }
    
  vcl_cout << "Number of pairs of points = " << from_pts.size() << vcl_endl;

  //
  // Use RREL library to solve homography estimation problem
  //

  // Construct the problem
  rrel_homography2d_est hg( from_pts, to_pts );

  //  RANSAC
  double max_outlier_frac = 0.7;
  double desired_prob_good = 0.99;
  int max_pops = 1;
  int trace_level = 0;
  rrel_ransac_obj ransac = rrel_ransac_obj();
  hg.set_prior_scale( 1.0 );

  rrel_ran_sam_search ransam = rrel_ran_sam_search();
  ransam.set_trace_level(trace_level);
  ransam.set_sampling_params( max_outlier_frac, desired_prob_good, max_pops);

  /////////////////////////////////////////////////////////////
  bool success = ransam.estimate( &hg, &ransac );
  ////////////////////////////////////////////////////////////////

  if (! success)
  {
    vcl_cout << "RANSAC failed!!\n";
    return false;
  }
  
  vcl_cout << "RANSAC succeeded.\n"
            << "estimate = " << ransam.params() << vcl_endl
            << "scale = " << ransam.scale() << vcl_endl;

  // construct homography matrix
  vnl_vector< double > params = ransam.params();
  vnl_matrix_fixed<double,3, 3> M;
  M[0][0]= params.get(0);   M[0][1]= params.get(1);   M[0][2]= params.get(2);
  M[1][0]= params.get(3);   M[1][1]= params.get(4);   M[1][2]= params.get(5);
  M[2][0]= params.get(6);   M[2][1]= params.get(7); M[2][2]= params.get(8);
 
  vgl_h_matrix_2d< double > H(M);
  vcl_cout << "Homography matrix H = \n" << H << vcl_endl;
  vcl_cout << "Det(H) = " << vnl_det(H.get_matrix()) << vcl_endl;
 
  // create the output storage class
  vidpro1_homog_2d_storage_sptr output_storage = vidpro1_homog_2d_storage_new();
  output_storage->set_H(H);
  output_data_[0].push_back(output_storage);
  return true;
}


//: Clone the process
bpro1_process*
vidpro1_compute_homography_2d_process::clone() const
{
  return new vidpro1_compute_homography_2d_process(*this);
}
    
//: finish the process queue
bool
vidpro1_compute_homography_2d_process::finish()
{
  return true;
}




