// This is contrib/mleotta/vidreg/vidreg_reconstructor.cxx

//:
// \file


#include "vidreg_reconstructor.h"
#include <vidreg/vidreg_salient_group.h>
#include <rgrl/rgrl_match_set.h>
#include <rgrl/rgrl_view.h>
#include <rgrl/rgrl_cast.h>
#include <rgrl/rgrl_trans_similarity.h>
#include <vnl/vnl_vector_fixed.h>
#include <vgl/vgl_line_3d_2_points.h>
#include <vpgl/algo/vpgl_bundle_adjust.h>
#include <vnl/algo/vnl_sparse_lm.h>


//: Constructor
vidreg_reconstructor::vidreg_reconstructor(const vidreg_salient_group_sptr& group)
  : K_( 12500.0, vgl_point_2d<double>(640,360) )
{
  groups_.push_front(group);
  vidreg_salient_group_sptr curr_group = group;
  while(curr_group = curr_group->previous_group()){
    groups_.push_front(curr_group);
  }
  if(groups_.size() == 20)
    reconstruct();
}


//: Destructor
vidreg_reconstructor::~vidreg_reconstructor()
{
}


void
vidreg_reconstructor::reconstruct()
{
  vcl_cout << "reconstructing" <<vcl_endl;

  init_cameras();

  init_points();

  vpgl_bundle_adjust::write_vrml("initial_scene.wrl",cameras_,world_points_);

  vcl_vector<vpgl_calibration_matrix<double> > Ks;
  vnl_vector<double> init_a = vpgl_bundle_adj_lsqr::create_param_vector(cameras_);
  vnl_vector<double> init_b = vpgl_bundle_adj_lsqr::create_param_vector(world_points_);
  for(unsigned int i=0; i<cameras_.size(); ++i){
    Ks.push_back(cameras_[i].get_calibration());
  }
  
  vnl_vector<double> a(init_a), b(init_b);
  // do the bundle adjustment
  vpgl_bundle_adj_lsqr ba_func(Ks,image_points_,mask_,true);
  vnl_sparse_lm lm(ba_func);
  lm.set_max_function_evals(100);
  lm.set_g_tolerance(1e-6);
  //lm.set_trace(true);
  lm.set_verbose(true);
  if(!lm.minimize(a,b))
    vcl_cout << "didn't converge" <<vcl_endl;
  lm.diagnose_outcome();

  //double error1 = lm.get_end_error();

  // Update the camera parameters
  for(unsigned int i=0; i<cameras_.size(); ++i)
    cameras_[i] = ba_func.param_to_cam(i,a);

  // Update the world points
  for(unsigned int j=0; j<world_points_.size(); ++j){
    world_points_[j] = ba_func.param_to_point(j,b);
  }

  vpgl_bundle_adjust::write_vrml("final_scene.wrl",cameras_,world_points_);

}


void
vidreg_reconstructor::init_cameras()
{
  double x=0, y=0;
  vgl_rotation_3d<double> R;
  cameras_.push_back(vpgl_perspective_camera<double>(K_, vgl_point_3d<double>(x,y,0), R));

  typedef vcl_deque<vidreg_salient_group_sptr>::const_iterator Gitr;
  for(Gitr g = groups_.begin(); g != groups_.end(); ++g)
  {
    rgrl_transformation_sptr xform = (*g)->view()->xform_estimate();
    rgrl_trans_similarity* sim = rgrl_cast<rgrl_trans_similarity*>(xform);
    x -= sim->t()[0];
    y -= sim->t()[1];
    double scale = sim->A().get_row(0).magnitude();
    vnl_matrix_fixed<double,3,3> rot_2d(0.0);
    rot_2d(2,2) = 1.0;
    rot_2d(0,0) = sim->A()(0,0)/scale;
    rot_2d(0,1) = sim->A()(1,0)/scale;
    rot_2d(1,0) = sim->A()(0,1)/scale;
    rot_2d(1,1) = sim->A()(1,1)/scale;
    R = R * vgl_rotation_3d<double>(rot_2d);
    // Ignoring scale change, should this factor into focal length?
    cameras_.push_back(vpgl_perspective_camera<double>(K_, vgl_point_3d<double>(x,y,0), R));
  }
}


void
vidreg_reconstructor::init_points()
{
  vcl_vector<vcl_vector<rgrl_feature_sptr> > tracks(cameras_.size());
  typedef vcl_map<rgrl_feature_sptr,unsigned> Tmap;
  Tmap track_map;

  const double depth = -K_.focal_length();

  typedef rgrl_match_set::from_iterator from_iter;
  typedef from_iter::to_iterator        to_iter;

  typedef vcl_deque<vidreg_salient_group_sptr>::const_iterator Gitr;
  unsigned int frame = 0;
  unsigned int num_tracks = 0;
  for(Gitr g = groups_.begin(); g != groups_.end(); ++g, ++frame)
  {

    rgrl_match_set_sptr match_set = (*g)->matches()[0];
    //  for each from image feature being matched
    for ( from_iter fitr = match_set->from_begin();
          fitr != match_set->from_end(); ++fitr )
    {
      if ( fitr.empty() )  continue;
      rgrl_feature_sptr from_feature = fitr.from_feature();

      double weight = (*g)->weight(from_feature);
      if(weight < 1.0) continue;

      for ( to_iter titr = fitr.begin(); titr != fitr.end(); ++titr )
      {
        unsigned t_index = 0;
        rgrl_feature_sptr to_feature = titr.to_feature();
        Tmap::const_iterator ti = track_map.find(to_feature);
        if(ti != track_map.end()){
          t_index = ti->second;
        }
        else{
          t_index = num_tracks++;
          for(unsigned f=0; f<cameras_.size(); ++f)
            tracks[f].push_back(NULL);
          tracks[frame][t_index] = to_feature;
        }
        track_map[from_feature] = t_index;
        tracks[frame+1][t_index] = from_feature;
      }
    }
  }

  // prune short tracks
  {
    vcl_vector<vcl_vector<rgrl_feature_sptr> > good_tracks(cameras_.size());
    unsigned num_good_tracks = 0;
    for(unsigned int j=0; j<num_tracks; ++j){
      unsigned num_corrs = 0;
      for(unsigned int i=0; i<cameras_.size(); ++i){
        if(tracks[i][j])
          ++num_corrs;
      }
      if(num_corrs > 5){
        ++num_good_tracks;
        for(unsigned int i=0; i<cameras_.size(); ++i){
          good_tracks[i].push_back(tracks[i][j]);
        }
      }
    }
    num_tracks = num_good_tracks;
    tracks = good_tracks;
  }

  image_points_.clear();
  world_points_.clear();
  mask_.clear();
  world_points_.resize(num_tracks, vgl_point_3d<double>(0,0,0));
  mask_.resize(cameras_.size(), vcl_vector<bool>(num_tracks,false));
  vcl_vector<unsigned> observation_count(num_tracks,0);

  for(unsigned int i=0; i<cameras_.size(); ++i){
    const vpgl_perspective_camera<double>& P = cameras_[i];
    vgl_point_3d<double> c(P.camera_center());
    double d = depth - c.z();
    for(unsigned int j=0; j<num_tracks; ++j){
      if(tracks[i][j]){
        mask_[i][j] = true;
        vnl_vector<double> loc = tracks[i][j]->location();
        vgl_point_2d<double> pt2d(loc[0],loc[1]);
        image_points_.push_back(pt2d);
        vgl_vector_3d<double> ray = P.backproject(pt2d).direction();
        double a = d/ray.z();
        world_points_[j] += vgl_vector_3d<double>(c.x() + a*ray.x(),
                                                c.y() + a*ray.y(), depth);
        ++observation_count[j];
      }
    }
  }
  for(unsigned int j=0; j<num_tracks; ++j){
    double n = observation_count[j];
    for(int i=0; i<cameras_.size(); ++i)
      vcl_cout << mask_[i][j] << " ";
    vcl_cout << vcl_endl;
    world_points_[j].set(world_points_[j].x()/n,
                         world_points_[j].y()/n,
                         world_points_[j].z()/n);
  }
}

