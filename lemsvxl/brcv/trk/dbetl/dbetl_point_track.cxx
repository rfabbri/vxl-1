// This is /algo/dbetl/dbetl_point_track.cxx
//:
// \file

#include "dbetl_point_track.h"
#include "dbetl_point_2d.h"
#include "dbetl_camera.h"
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_svd_economy.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vgl/vgl_distance.h>


//: Constructor
dbetl_point_track::dbetl_point_track()
 : num_points_(0),
   error_(0)
{
}


//: Constructor from a point
dbetl_point_track::dbetl_point_track(const dbetl_point_2d_sptr& pt)
 : num_points_(0),
   error_(0)
{
  points_.push_back(pt);
  if(pt){
    ++num_points_;
    stats_near_ += pt->stats_near();
    stats_far_ += pt->stats_far();
  }
}


//: Copy Constructor that adds a new point
dbetl_point_track::dbetl_point_track(const dbetl_point_track& trk, 
                                   const dbetl_point_2d_sptr& pt)
 : points_(trk.points_), num_points_(trk.num_points_),
   stats_near_(trk.stats_near_),
   stats_far_(trk.stats_far_),
   mean_3d_(trk.mean_3d_),
   mean_3d_above_(trk.mean_3d_above_),
   mean_3d_below_(trk.mean_3d_below_),
   error_(trk.error_)
{
  points_.push_back(pt);
  if(pt){
    ++num_points_;

    if(num_points_>1){
      error_ = this->estimate_mean_3d();
      error_ += this->image_cost();
      error_ /= num_points_;
    }

    //double cost = stats_near_.intensity_cost(pt->stats_near().int_mean());
    //cost += stats_far_.intensity_cost(pt->stats_far().int_mean());

    //error_ += cost;

    // combine the image statistics
    //stats_near_ += pt->stats_near();
    //stats_far_ += pt->stats_far();
  }
}


//: Returns a cost function used to minimize reprojection error
dbetl_reproject_lsqr_cost 
dbetl_point_track::cost_func() const
{/*
  vnl_matrix<double> E(2*num_points_,4);
  vnl_matrix<double> P3(num_points_,4);
  vcl_vector<dbetl_point_2d_sptr>::const_iterator p_itr = points_.begin();
  for(int i=0; i<num_points_; ++i, ++p_itr){
    while(*p_itr == NULL) 
      ++p_itr;
    P3.set_row(i, (*(*p_itr)->camera())[2]);
    vnl_matrix<double> curr = (*p_itr)->proj_error_matrix();
    E.set_row(2*i,   curr[0]);
    E.set_row(2*i+1, curr[1]);
  }
  return dbetl_reproject_lsqr_cost(E,P3);
  */
  vcl_vector<vnl_double_3x4> cameras;
  vcl_vector<vgl_point_2d<double> > pts;
  vcl_vector<dbetl_point_2d_sptr>::const_iterator p_itr = points_.begin();
  for(int i=0; i<num_points_; ++i, ++p_itr){
    while(*p_itr == NULL) 
      ++p_itr;

    pts.push_back((*p_itr)->pt());
    cameras.push_back(*(*p_itr)->camera());
    
  }
  return dbetl_reproject_lsqr_cost(cameras,pts);
}


//: Estimated the mean 3D reconstruction
double
dbetl_point_track::estimate_mean_3d()
{
  double error = 0.0;

  int steps_above = 10;
  int steps_below = 10;
  vcl_vector<vnl_double_3x4> cameras;
  vcl_vector<vgl_point_2d<double> > pts;
  vcl_vector<vcl_vector<vgl_point_2d<double> > > pts_above(steps_above);
  vcl_vector<vcl_vector<vgl_point_2d<double> > > pts_below(steps_below);
  vcl_vector<double> errors;
  vcl_vector<dbetl_point_2d_sptr>::const_iterator p_itr = points_.begin();
  for(int i=0; i<num_points_; ++i, ++p_itr){
    while(*p_itr == NULL) 
      ++p_itr;
    cameras.push_back(*(*p_itr)->camera());
    pts.push_back((*p_itr)->pt());

    dbetl_point_2d_sptr temp_pt = *p_itr;
    for( int j=0; j<steps_above; ++j){
      temp_pt = temp_pt->next();
      if(temp_pt)
        pts_above[j].push_back(temp_pt->pt());
      else{
        steps_above = j;
        break;
      }
    }
    temp_pt = *p_itr;
    for( int j=0; j<steps_below; ++j){
      temp_pt = temp_pt->prev();
      if(temp_pt)
        pts_below[j].push_back(temp_pt->pt());
      else{
        steps_below = j;
        break;
      }
    }
  }
  //vcl_cout << "above = " << steps_above << " below = " << steps_below << vcl_endl;

  // for more than 2 points
  if(num_points_>2){
    dbetl_reproject_lsqr_cost func(cameras,pts);
    vnl_levenberg_marquardt minimizer(func);
    vnl_double_3 min_pt(mean_3d_.x(), mean_3d_.y(), mean_3d_.z());
    minimizer.minimize(min_pt);
    double rms_error = minimizer.get_end_error();
    error = rms_error*rms_error*2*num_points_;
    mean_3d_ = vgl_point_3d<double>(min_pt.data_block()); 

    for(int j=0; j<steps_above; ++j){
      dbetl_reproject_lsqr_cost func(cameras,pts_above[j]);
      vnl_levenberg_marquardt minimizer(func);
      vnl_double_3 min_pt(mean_3d_above_[j].x(), mean_3d_above_[j].y(), mean_3d_above_[j].z());
      minimizer.minimize(min_pt);
      double rms_error = minimizer.get_end_error();
      errors.push_back(rms_error*rms_error*2*num_points_);
      mean_3d_above_[j] = vgl_point_3d<double>(min_pt.data_block());
    }
    for(int j=0; j<steps_below; ++j){
      dbetl_reproject_lsqr_cost func(cameras,pts_below[j]);
      vnl_levenberg_marquardt minimizer(func);
      vnl_double_3 min_pt(mean_3d_below_[j].x(), mean_3d_below_[j].y(), mean_3d_below_[j].z());
      minimizer.minimize(min_pt);
      double rms_error = minimizer.get_end_error();
      errors.push_back(rms_error*rms_error*2*num_points_);
      mean_3d_below_[j] = vgl_point_3d<double>(min_pt.data_block());
    }
    
  }
  // for exactly 2 points
  else{
    vnl_matrix<double> A(4,4);
    A.set_row(0, cameras[0].get_row(0) - pts[0].x()*cameras[0].get_row(2));
    A.set_row(1, cameras[0].get_row(1) - pts[0].y()*cameras[0].get_row(2));
    A.set_row(2, cameras[1].get_row(0) - pts[1].x()*cameras[1].get_row(2));
    A.set_row(3, cameras[1].get_row(1) - pts[1].y()*cameras[1].get_row(2));
    vnl_vector<double> v = vnl_svd_economy<double>(A).nullvector();
    v /= v[3];
    mean_3d_ = vgl_point_3d<double>(v.data_block());

    if((int)mean_3d_above_.size() < steps_above)
      mean_3d_above_.resize(steps_above);
    for(int j=0; j<steps_above; ++j){
      A.set_row(0, cameras[0].get_row(0) - pts_above[j][0].x()*cameras[0].get_row(2));
      A.set_row(1, cameras[0].get_row(1) - pts_above[j][0].y()*cameras[0].get_row(2));
      A.set_row(2, cameras[1].get_row(0) - pts_above[j][1].x()*cameras[1].get_row(2));
      A.set_row(3, cameras[1].get_row(1) - pts_above[j][1].y()*cameras[1].get_row(2));
      vnl_vector<double> v = vnl_svd_economy<double>(A).nullvector();
      v /= v[3];
      mean_3d_above_[j] = vgl_point_3d<double>(v.data_block());
    }
    if((int)mean_3d_below_.size() < steps_below)
      mean_3d_below_.resize(steps_below);
    for(int j=0; j<steps_below; ++j){
      A.set_row(0, cameras[0].get_row(0) - pts_below[j][0].x()*cameras[0].get_row(2));
      A.set_row(1, cameras[0].get_row(1) - pts_below[j][0].y()*cameras[0].get_row(2));
      A.set_row(2, cameras[1].get_row(0) - pts_below[j][1].x()*cameras[1].get_row(2));
      A.set_row(3, cameras[1].get_row(1) - pts_below[j][1].y()*cameras[1].get_row(2));
      vnl_vector<double> v = vnl_svd_economy<double>(A).nullvector();
      v /= v[3];
      mean_3d_below_[j] = vgl_point_3d<double>(v.data_block());
    }

    error = 0.0;
  }

  //vcl_cout << "Error = " <<error << " -- ";
  for(unsigned int i=0; i<errors.size(); ++i){
    error += errors[i];
    //vcl_cout << errors[i] << ", ";
  }
  error /= (errors.size()+1);
  //vcl_cout << "avg = " << error << vcl_endl;

  vcl_vector<vgl_point_3d<double> > cpts = this->curve_points();
  double distance = 0.0;
  if(cpts.size() > 1){
    vgl_point_3d<double> last_pt = cpts.front();
    for(unsigned int i=1; i<cpts.size(); ++i){
      distance += vgl_distance(last_pt, cpts[i]);
      last_pt = cpts[i];
    }
    distance /= cpts.size()-1;
  }
  //vcl_cout << "error = " << error << " \t distance = " << distance << vcl_endl;

  error += distance;

  return error + num_points_*vcl_log(2*vnl_math::pi);
}


//: The cost associated with the image statistics
double
dbetl_point_track::image_cost() const
{
  double cost = this->image_cost(points_);
  int size = 1;
  vcl_vector<dbetl_point_2d_sptr> pts(points_);
  for(unsigned int i=0; i<mean_3d_above_.size(); ++i){
    for(unsigned int j=0; j<pts.size(); ++j){
      if(pts[j] != NULL) 
        pts[j] = pts[j]->next();
    }
    cost += this->image_cost(pts);
    ++size;
  }
  pts = points_;
  for(unsigned int i=0; i<mean_3d_below_.size(); ++i){
    for(unsigned int j=0; j<pts.size(); ++j){
      if(pts[j] != NULL) 
        pts[j] = pts[j]->prev();
    }
    cost += this->image_cost(pts);
    ++size;
  }
  return cost/size;
}


//: The cost associated with the image statistics
double
dbetl_point_track::image_cost(const vcl_vector<dbetl_point_2d_sptr>& pts) const
{
  double error = 0.0;
  double mean_near = 0.0;
  double mean_far = 0.0;
  int num = 0;
  for( vcl_vector<dbetl_point_2d_sptr>::const_iterator p_itr = pts.begin();
       p_itr != pts.end();  ++p_itr ){
    if(*p_itr == NULL) 
      continue;
    mean_near += (*p_itr)->stats_near().int_mean();
    mean_far  += (*p_itr)->stats_far().int_mean();
    ++num;
  }
  mean_near /= num;
  mean_far /= num;

  for( vcl_vector<dbetl_point_2d_sptr>::const_iterator p_itr = pts.begin();
       p_itr != pts.end();  ++p_itr ){
    if(*p_itr == NULL) 
      continue;
    error += (*p_itr)->stats_near().intensity_cost(mean_near);
    error += (*p_itr)->stats_far().intensity_cost(mean_far);
  }

  return error;
}


//: Return the vector of 3D points making up a local curve fragment
vcl_vector<vgl_point_3d<double> > 
dbetl_point_track::curve_points() const
{
  vcl_vector<vgl_point_3d<double> > pts;
  for(int i=int(mean_3d_below_.size())-1; i>0; --i)
    pts.push_back(mean_3d_below_[i]);
  pts.push_back(mean_3d_);
  for(int i=0; i<int(mean_3d_above_.size()); ++i)
    pts.push_back(mean_3d_above_[i]);

  return pts;
}
