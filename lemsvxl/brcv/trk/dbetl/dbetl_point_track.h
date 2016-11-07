// This is /algo/dbetl/dbetl_point_track.h
#ifndef dbetl_point_track_h_
#define dbetl_point_track_h_

//:
// \file
// \brief  A track of 2D points along an epipolar line over time.
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date   11/22/2004


#include <dbetl/dbetl_point_2d_sptr.h>
#include <dbetl/dbetl_image_stats.h>
#include <dbetl/dbetl_reproject_lsqr_cost.h>
#include <vcl_vector.h>
#include <vbl/vbl_ref_count.h>
#include <vgl/vgl_point_3d.h>


//: A track of 2D points along an epipolar line over time.
class dbetl_point_track : public vbl_ref_count{
public:
  //: Constructor
  dbetl_point_track();

  //: Constructor from a point
  dbetl_point_track(const dbetl_point_2d_sptr& pt);

  //: Copy Constructor that adds a new point
  dbetl_point_track(const dbetl_point_track& trk, 
                   const dbetl_point_2d_sptr& pt);

  //: Return the current estimate of the 3D mean reconstruction
  vgl_point_3d<double> mean_3d() const { return mean_3d_; }

  //: Return the vector of 3D points making up a local curve fragment
  vcl_vector<vgl_point_3d<double> > curve_points() const;

  //: Return the combined error of the estimated 3D mean with image stats
  double error() const { return error_; }

  //: The cost associated with the image statistics
  double image_cost() const;

  //: The cost associated with the image statistics
  double image_cost(const vcl_vector<dbetl_point_2d_sptr>& pts) const;

  //: Return the vector of points
  vcl_vector<dbetl_point_2d_sptr> points() const { return points_; }

  //: Return a single point
  dbetl_point_2d_sptr point(unsigned int index) { return points_[index]; }

  //: Return the last point
  dbetl_point_2d_sptr last() { return points_.back(); }

  //: Return the number of points
  double num_points() const { return num_points_; }

  //: Returns a cost function used to minimize reprojection error
  dbetl_reproject_lsqr_cost cost_func() const;


private:
  //: Estimated the mean 3D reconstruction
  // \return the sum of squared reprojection errors
  double estimate_mean_3d();

  //----------- Data -------------

  //: The points at each frame
  vcl_vector<dbetl_point_2d_sptr> points_;
  //: The number of non-null points in points_
  int num_points_;

  //: Image statistics going towards the epipole
  dbetl_image_stats stats_near_;
  //: Image statistics going away from the epipole
  dbetl_image_stats stats_far_;

  //: The current estimate of the 3D mean reconstruction
  vgl_point_3d<double> mean_3d_;

  //: The 3D point above (larger angle)
  vcl_vector<vgl_point_3d<double> > mean_3d_above_;
  
  //: The 3D point below (smaller angle)
  vcl_vector<vgl_point_3d<double> > mean_3d_below_;

  //: The error associated with the mean estimation
  double error_;
};



#endif // dbetl_point_track_h_
