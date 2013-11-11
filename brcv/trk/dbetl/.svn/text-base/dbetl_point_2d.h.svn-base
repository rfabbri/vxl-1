// This is /algo/dbetl/dbetl_point_2d.h
#ifndef dbetl_point_2d_h_
#define dbetl_point_2d_h_

//:
// \file
// \brief  A 2D point along an epipolar track.
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date   11/12/2004


#include <dbecl/dbecl_episeg_point.h>
#include <dbetl/dbetl_point_2d_sptr.h>
#include <dbetl/dbetl_camera_sptr.h>
#include <dbetl/dbetl_image_stats.h>
#include <vnl/vnl_matrix.h>


//: A 2D point along an epipolar track.
class dbetl_point_2d : public dbecl_episeg_point 
{
public:
  //: Constructor
  dbetl_point_2d( const dbecl_episeg_sptr& episeg, double index, 
                 const dbetl_camera_sptr& camera = NULL );
  
  //: Destructor
  virtual ~dbetl_point_2d();

  //: Return the distance to the epipole
  double dist() const;

  //: Return the angle with the epipole
  double angle() const;

  //: Return the orientation
  double orientation() const { return orientation_; }

  //: Return the camera
  dbetl_camera_sptr camera() const { return camera_; }

  void set_camera(const dbetl_camera_sptr& camera) { camera_ = camera; }

  //: Returns a 2 by 4 matrix used to calculate the x-y projection errors
  vnl_matrix<double> proj_error_matrix() const;

  //: Set the image statistics going toward the epipole
  void set_stats_near(const dbetl_image_stats& stats) { stats_near_ = stats; }

  //: Set the image statistics going away from the epipole
  void set_stats_far(const dbetl_image_stats& stats) { stats_far_ = stats; }

  //: Return the mean intensity going toward the epipole
  dbetl_image_stats stats_near() const { return stats_near_; }

  //: Return the mean intensity going away from the epipole
  dbetl_image_stats stats_far() const { return stats_far_; }

  //: Return the next point
  dbetl_point_2d_sptr next() const { return next_; }
  
  //: Return the previous point
  dbetl_point_2d_sptr prev() const { return prev_; }

  //: Set the next point
  void set_next(const dbetl_point_2d_sptr& n) { next_ = n.ptr(); }
  
  //: Set the previous point
  void set_prev(const dbetl_point_2d_sptr& p) { prev_ = p.ptr(); }

private: 
  //: Image statistics going towards the epipole
  dbetl_image_stats stats_near_;
  //: Image statistics going away from the epipole
  dbetl_image_stats stats_far_;

  //: Next point in the doubly-linked list
  dbetl_point_2d* next_;
  //: Previous point in the doubly-linked list
  dbetl_point_2d* prev_;

  //: The camera for this point
  dbetl_camera_sptr camera_;
  
  //: the angle of the tagent direction from the x-axis
  double orientation_;

  double dist_;
  double angle_;
};



#endif // dbetl_point_2d_h_
