// This is brcv/seg/dbkpr/pro/dbkpr_reconstruct_process.cxx

//:
// \file

#include "dbkpr_reconstruct_process.h"
#include <dbdet/dbdet_lowe_keypoint.h>
#include <dbdet/dbdet_keypoint_corr3d.h>
#include <dbkpr/dbkpr_view_span_tree.h>
#include <vpgl/algo/vpgl_bundle_adjust.h>
#include <vpgl/algo/vpgl_ba_fixed_k_lsqr.h>
#include <vnl/algo/vnl_sparse_lm.h>
#include <vnl/vnl_quaternion.h>
#include <vcl_algorithm.h>
#include <vgl/vgl_distance.h>

#include <dbdet/pro/dbdet_keypoint_storage.h>
#include <dbkpr/pro/dbkpr_corr3d_storage_sptr.h>
#include <dbkpr/pro/dbkpr_corr3d_storage.h>

#include <bpro1/bpro1_parameters.h>


//: Constructor
dbkpr_reconstruct_process::dbkpr_reconstruct_process()
{
  if( !parameters()->add( "VRML File",          "-vrmlfile",  bpro1_filepath("","wrl")) ||
      !parameters()->add( "CS224 File",         "-cs224file", bpro1_filepath("","txt")) ||
      !parameters()->add( "Minimum Projections","-minproj",   (int)2) ||
      !parameters()->add( "Maximum Iterations", "-maxitr",    (int)200) ||
      !parameters()->add( "Gradient Tolerence", "-gtol",      (float)1e-6) ||
      !parameters()->add( "Use Adaptive Weights", "-usewgt",  (bool)true) ||
      !parameters()->add( "Verbose",            "-verbose",   (bool)true)) {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbkpr_reconstruct_process::~dbkpr_reconstruct_process()
{
}


//: Clone the process
bpro1_process*
dbkpr_reconstruct_process::clone() const
{
  return new dbkpr_reconstruct_process(*this);
}


//: Return the name of this process
vcl_string
dbkpr_reconstruct_process::name()
{
  return "Reconstruct Keypoints";
}


//: Return the number of input frame for this process
int
dbkpr_reconstruct_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbkpr_reconstruct_process::output_frames()
{
  return 0;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbkpr_reconstruct_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "keypoints_corr3d" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbkpr_reconstruct_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "keypoints_corr3d" );
  return to_return;
}


//: Execute the process
bool
dbkpr_reconstruct_process::execute()
{
  return true;
}


// write the output for use my CS224 project
static void write_cs224(const vcl_string& filename,
                        vcl_vector<vpgl_perspective_camera<double> >& cameras,
                        vcl_vector<vgl_point_3d<double> >& world_points)
{
  vcl_ofstream os(filename.c_str());

  os << cameras.size() << '\n'
     << "\"/u/imagephysics/data/views/ambient2/\"\n\n";
  for(unsigned int i=0; i<cameras.size(); ++i){
    os <<"\"00000.png\"\n";
    vnl_double_3x3 K = cameras[i].get_calibration().get_matrix();
    vgl_rotation_3d<double> R = cameras[i].get_rotation();
    R = R * vgl_rotation_3d<double>(3.141592653,0,0);
    vgl_point_3d<double> ctr = cameras[i].get_camera_center();
    vnl_vector<double> r = R.as_rodrigues();
    double fov = 2.0*vcl_min(vcl_atan(K[1][2]/K[1][1]), vcl_atan(K[0][2]/K[0][0])) * 180.0/3.141592653;
    os  << '('<< ctr.x() << ", " << ctr.y() << ", " << ctr.z() << ") ("
        << r[0] << ", " << r[1] << ", " << r[2] <<") "
        << fov << "\n\n";
  }

  for(unsigned int j=0; j<world_points.size(); ++j){
    os  << "("<< world_points[j].x() << ", "
        << world_points[j].y() << ", "
        << world_points[j].z() << ")\n";
  }

  os.close();
}


bool
dbkpr_reconstruct_process::finish()
{

  // cast the storage classes
  vcl_vector<dbdet_keypoint_corr3d_sptr> all_corr3d;
  vcl_vector<vpgl_perspective_camera<double> > cameras;
  cameras.reserve(input_data_.size());
  for(unsigned int i=0; i<input_data_.size(); ++i){
    dbkpr_corr3d_storage_sptr frame_corr3d;
    frame_corr3d.vertical_cast(input_data_[i][0]);
    if(i==0)
      all_corr3d = frame_corr3d->correspondences();

    const vpgl_perspective_camera<double> *pcam;
    if (frame_corr3d->get_camera()->type_name() != "vpgl_perspective_camera") {
      vcl_cerr << "Error: requires a perspective  camera" << vcl_endl;
      return false;
    }
    pcam = static_cast<const vpgl_perspective_camera<double> *> (frame_corr3d->get_camera());
    cameras.push_back(*pcam);
  }

  
  bpro1_filepath vrml_file,cs224_file;
  int maxitr = 0, minproj = 0;
  bool verbose = true, usewgt = true;
  float gtol = 0.0;
  parameters()->get_value( "-vrmlfile" , vrml_file );
  parameters()->get_value( "-cs224file" , cs224_file );
  parameters()->get_value( "-minproj" , minproj );
  parameters()->get_value( "-maxitr" , maxitr );
  parameters()->get_value( "-usewgt" , usewgt );

  vcl_cerr << "Warning: implementation uses new vpgl and treats use_weight to use_m_estimator\n";
  vcl_cerr << "Warning: implementation uses new vpgl and has not been thoroughly tested\n";

  parameters()->get_value( "-verbose" , verbose );
  parameters()->get_value( "-gtol" , gtol );

  // collect the subset of points with more than 2 projections
  vcl_vector<dbdet_keypoint_corr3d_sptr> some_corr3d;
  for(unsigned int i=0; i<all_corr3d.size(); ++i){
    if(all_corr3d[i]->size() >= (unsigned int)minproj)
      some_corr3d.push_back(new dbdet_keypoint_corr3d(*all_corr3d[i]));
  }

  vcl_vector<vgl_point_2d<double> > image_points;
  vcl_vector<vgl_point_3d<double> > world_points;
  vcl_vector<vcl_vector<bool> > mask(cameras.size(),
                                     vcl_vector<bool>(some_corr3d.size(),false));
  for(unsigned int j=0; j<some_corr3d.size(); ++j)
    world_points.push_back(*some_corr3d[j]);
  for(unsigned int i=0; i<cameras.size(); ++i){
    for(unsigned int j=0; j<some_corr3d.size(); ++j){
      dbdet_keypoint_sptr kp = some_corr3d[j]->in_view(i);
      if(kp){
        mask[i][j] = true;
        image_points.push_back(*kp);
      }
    }
  }

  vcl_cout << "==========================================\n";
  vcl_cout << "Calibration BEFORE bundle adjustment:\n";
  // This is a customized version of what is already implemented inside the
  // bpgl_bundle_adjust class
  vcl_vector<vpgl_calibration_matrix<double> > Ks;
  vnl_vector<double> init_a = vpgl_ba_fixed_k_lsqr::create_param_vector(cameras);
  vnl_vector<double> init_b = vpgl_ba_fixed_k_lsqr::create_param_vector(world_points);
  for(unsigned int i=0; i<cameras.size(); ++i){
    Ks.push_back(cameras[i].get_calibration());
    vcl_cout << "\nK" << i << ":\n" << Ks.back().get_matrix() << vcl_endl;
  }
  
  vnl_vector<double> a(init_a), b(init_b), c(0);
  // do the bundle adjustment
  vpgl_ba_fixed_k_lsqr ba_func_fixed(Ks,image_points,mask);
  vpgl_bundle_adjust_lsqr &ba_func = ba_func_fixed;
  vnl_sparse_lm lm(ba_func);
  lm.set_max_function_evals(maxitr);
  lm.set_g_tolerance(gtol);
  lm.set_trace(true);
  lm.set_verbose(verbose);
  if(!lm.minimize(a,b,c, true, usewgt))
    return false;
  lm.diagnose_outcome();

  double error1 = lm.get_end_error();
 
  if(error1 > 1.0)
  {
    // flip points
    vnl_vector<double> b2(b);
    for(unsigned int j=0; j<b.size()/3; ++j){
      b2[3*j+2] *= -1.0; // flip over XY plane
    }
    // reset cameras
    vnl_vector<double> a2(init_a);

    ba_func.reset();
    // do the bundle adjustment again
    if(!lm.minimize(a2,b2,c,true,usewgt))
      return false;
    lm.diagnose_outcome();

    double error2 = lm.get_end_error();
    if(error2 < error1){
      a = a2;
      b = b2;
      error1 = error2;
    }
  }
  
  // Update the camera parameters
  for(unsigned int i=0; i<cameras.size(); ++i)
    cameras[i] = ba_func.param_to_cam(i,a,c);
    
  // Update the world points
  for(unsigned int j=0; j<world_points.size(); ++j){
    world_points[j] = ba_func.param_to_point(j,b,c);
    some_corr3d[j]->set(world_points[j].x(),world_points[j].y(),world_points[j].z());
  }
  
  if(usewgt)
  {
    vcl_vector<double> weights = vcl_vector<double>(lm.get_weights().begin(), lm.get_weights().end());

    const vnl_crs_index& crs = ba_func.residual_indices();
    typedef vnl_crs_index::sparse_vector::iterator sv_itr;
    for(unsigned int i=0; i<cameras.size(); ++i)
    {
      vnl_crs_index::sparse_vector row = crs.sparse_row(i);
      for(sv_itr r_itr=row.begin(); r_itr!=row.end(); ++r_itr)
      {
        unsigned int j = r_itr->second;
        unsigned int k = r_itr->first;
        if(weights[k] < 1.0){
          some_corr3d[j]->remove_correspondence(i);
        }
      }
    }
  }
  
  
  double avg_dist = 0.0;
  for(unsigned int i=0; i<cameras.size(); ++i){
    vgl_point_3d<double> c1 = cameras[i].camera_center();
    for(unsigned int j=i+1; j<cameras.size(); ++j){
      vgl_point_3d<double> c2 = cameras[j].camera_center();
      avg_dist += vgl_distance(c1,c2);
    }
  }
  avg_dist /= (cameras.size()*(cameras.size()-1))/2.0;
  
  vcl_vector<int> num_meaningful(some_corr3d.size());
  for(unsigned int k=0; k<some_corr3d.size(); ++k){
    num_meaningful[k] = some_corr3d[k]->size();
  }
  for(unsigned int i=0; i<cameras.size(); ++i){
    vgl_point_3d<double> c1 = cameras[i].camera_center();
    vcl_vector<bool> meaningful(some_corr3d.size(),true);
    for(unsigned int j=i+1; j<cameras.size(); ++j){
      vgl_point_3d<double> c2 = cameras[j].camera_center();
      if(vgl_distance(c1,c2) < avg_dist*.1){
        for(unsigned int k=0; k<some_corr3d.size(); ++k){
          if(some_corr3d[k]->in_view(i) && some_corr3d[k]->in_view(j))
            meaningful[k] = false;
        }
      }
    }
    for(unsigned int k=0; k<some_corr3d.size(); ++k)
      if(!meaningful[k])
        --num_meaningful[k];
  }
  
  // remove all points without at least minproj meaningful projections
  vcl_vector<dbdet_keypoint_corr3d_sptr> old_corr3d(some_corr3d); some_corr3d.clear(); 
  vcl_vector<vgl_point_3d<double> > old_world_points(world_points); world_points.clear();
  for(unsigned int j=0; j<old_corr3d.size(); ++j)
    if(num_meaningful[j] >= minproj){
      some_corr3d.push_back(old_corr3d[j]);
      world_points.push_back(old_world_points[j]);
    } 

/*
  vnl_vector<double> b2(b);
  // reset cameras
  vnl_vector<double> a2(init_a);

  ba_func.reset();
  // do the bundle adjustment again
  if(!lm.minimize(a2,b2))
    return false;
  lm.diagnose_outcome();

  double error2 = lm.get_end_error();
  if(error2 < error1){
    a = a2;
    b = b2;
  }
*/
  


  if(vrml_file.path != "") 
    vpgl_bundle_adjust::write_vrml(vrml_file.path, cameras, world_points);

  if(cs224_file.path != "")
    write_cs224(cs224_file.path, cameras, world_points);

  // pack up the results into storage classes
  dbkpr_corr3d_storage_sptr in_storage;
  for(unsigned int i=0; i<input_data_.size(); ++i){
    in_storage.vertical_cast(input_data_[i][0]);
    dbkpr_corr3d_storage_sptr out_storage = dbkpr_corr3d_storage_new(some_corr3d, in_storage->ni(),in_storage->nj());
    out_storage->set_camera(new vpgl_perspective_camera<double> (cameras[i]));
    output_data_[i].push_back(out_storage);
  }

  // XXX 
  //
  vcl_cout << "==========================================\n";
  vcl_cout << "Calibration after bundle adjustment:\n";
  for (unsigned i=0; i < cameras.size(); ++i) {
    vcl_cout << "\nK" << i << ":\n" << cameras[i].get_calibration().get_matrix() << vcl_endl;
  }


  return true;
}





