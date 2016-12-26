// This is /algo/dbetl/dbetl_tracker.cxx
//:
// \file

#include "dbetl_tracker.h"
#include <dbetl/dbetl_point_2d.h>
#include <dbetl/dbetl_point_track.h>
#include <dbetl/dbetl_epiprofile.h>
#include <dbetl/dbetl_camera.h>
#include <vcl_algorithm.h>
#include <vcl_set.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_det.h>

//: Constructor
dbetl_tracker::dbetl_tracker(double min_angle, double max_angle, unsigned int num)
: camera_(NULL), tracks_(num),
  min_angle_(min_angle), d_angle_((max_angle-min_angle)/num)
{
}


//: Combine all new points with existing tracks
void 
dbetl_tracker::track()
{
  this->create_points();
  for(unsigned int i=0; i<tracks_.size(); ++i)
  {
    vcl_cout << "epipole " << i << vcl_endl;
    this->track(i);
  }
}


static bool trk_cmp_err_less(const dbetl_point_track_sptr& lhs,
                             const dbetl_point_track_sptr& rhs)
{
  return lhs->error() < rhs->error();
}


static bool trk_cmp_size_gt(const dbetl_point_track_sptr& lhs,
                            const dbetl_point_track_sptr& rhs)
{
  return lhs->num_points() > rhs->num_points();
}


static bool pt_cmp_less(const dbetl_point_2d_sptr& lhs,
                        const dbetl_point_2d_sptr& rhs)
{
  return lhs->dist() < rhs->dist();
}

//: Combine all new points with existing tracks
void 
dbetl_tracker::track(unsigned int index)
{
  vcl_vector<dbetl_point_track_sptr> new_tracks;

  // create new tracks if none exist
  if(tracks_[index].empty()){
    for( vcl_vector<dbetl_point_2d_sptr>::const_iterator p_itr = curr_pts_[index].begin();
    p_itr != curr_pts_[index].end();  ++p_itr )
      tracks_[index].push_back(new dbetl_point_track(*p_itr));
    return;
  }

  // a vector of unmatched points
  vcl_set<dbetl_point_2d_sptr> unmatched(curr_pts_[index].begin(), curr_pts_[index].end());

  // match each existing track to several points
  
  double sign_det_m = (vnl_det(vnl_double_3x3(camera_->extract(3,3))) > 0)?1:-1;
  vnl_vector<double> p3 = camera_->get_row(2);
  
  // order by the number of points
  vcl_sort(tracks_[index].begin(), tracks_[index].end(), trk_cmp_size_gt);
  for( vcl_vector<dbetl_point_track_sptr>::const_iterator t_itr = tracks_[index].begin();
       t_itr != tracks_[index].end();  ++t_itr )
  {
    dbetl_point_2d_sptr last_pt = (*t_itr)->points().back();
    for( vcl_vector<dbetl_point_2d_sptr>::const_iterator p_itr 
      = vcl_lower_bound(curr_pts_[index].begin(), curr_pts_[index].end(), last_pt, pt_cmp_less); 
    p_itr != curr_pts_[index].end();  ++p_itr )
    {
      if( (*p_itr)->dist() > last_pt->dist() + 100)
        break;
      dbetl_point_track_sptr new_trk = new dbetl_point_track(**t_itr, *p_itr);
      
      vgl_point_3d<double> pt = new_trk->mean_3d();
      double w = pt.x()*p3[0] + pt.y()*p3[1] + pt.z()*p3[2] + p3[3];
      if( w*sign_det_m < 0){ // behind camera
        //vcl_cout << "behind camera" << vcl_endl;
        continue;
      }
      new_tracks.push_back(new_trk);
    }

  }
  vcl_cout << "unmatched: " << unmatched.size();

  vcl_sort(new_tracks.begin(), new_tracks.end(), trk_cmp_err_less);

  tracks_[index].clear();
  const double thresh = 2.5;
  for( vcl_vector<dbetl_point_track_sptr>::const_iterator t_itr = new_tracks.begin();
       t_itr != new_tracks.end();  ++t_itr)
  {
    //vcl_cout << "error = " << (*t_itr)->error() << vcl_endl;
    if( (*t_itr)->error() > thresh 
       && tracks_[index].size() > 1)
      break;

    if((*t_itr)->curve_points().size() < 5)
      continue;

    if( (*t_itr)->num_points() < 3 ){
      tracks_[index].push_back(*t_itr);
      continue;
    }
    
    vcl_set<dbetl_point_2d_sptr>::iterator u_itr = unmatched.find((*t_itr)->last());
    if(u_itr == unmatched.end())
      continue;
    unmatched.erase(u_itr);
    tracks_[index].push_back(*t_itr);
    
  }

  vcl_cout << "  after: " << unmatched.size() << vcl_endl;

  // create new tracks for unmatched points
  
  for( vcl_set<dbetl_point_2d_sptr>::const_iterator p_itr = unmatched.begin();
       p_itr != unmatched.end();  ++p_itr ){
    tracks_[index].push_back(new dbetl_point_track(*p_itr));
  }
       
}


//: Create all the points 
void
dbetl_tracker::create_points()
{
  curr_pts_ = dbetl_epiprofile(episegs_, image_, min_angle_, d_angle_, tracks_.size());

  //: Set the camera to all points
  for( vcl_vector<vcl_vector<dbetl_point_2d_sptr> >::iterator itr1 = curr_pts_.begin();
       itr1 != curr_pts_.end();  ++itr1 )
    for( vcl_vector<dbetl_point_2d_sptr>::iterator itr2 = itr1->begin();
       itr2 != itr1->end();  ++itr2 )
      (*itr2)->set_camera(camera_);
}


//: Create all the points at a given angle
void
dbetl_tracker::create_points(double angle, 
                            vcl_vector<dbetl_point_2d_sptr>& points) const
{
  points = dbetl_epiprofile(episegs_, image_, angle);
  for( vcl_vector<dbetl_point_2d_sptr>::const_iterator itr = points.begin();
       itr != points.end();  ++itr )
  {
    (*itr)->set_camera(camera_);
  }
}
