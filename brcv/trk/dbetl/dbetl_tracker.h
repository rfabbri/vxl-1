// This is /algo/dbetl/dbetl_tracker.h
#ifndef dbetl_tracker_h_
#define dbetl_tracker_h_

//:
// \file
// \brief  A class to perform the tracking
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date   11/22/2004


#include <dbetl/dbetl_point_2d_sptr.h>
#include <dbetl/dbetl_point_track_sptr.h>
#include <dbetl/dbetl_camera_sptr.h>
#include <vil/vil_image_resource_sptr.h>
#include <dbecl/dbecl_episeg_sptr.h>
#include <vcl_vector.h>
 
//: A class to perform the tracking
class dbetl_tracker{
public:
  //: Constructor
  dbetl_tracker(double min_alpha, double max_alpha, unsigned int num);

  //: Set the current camera
  void set_camera(const dbetl_camera_sptr& camera) { camera_ = camera; }
  //: Set the current image
  void set_image(const vil_image_resource_sptr& image) { image_ = image; }
  //: Set the current episegs
  void set_episegs(const vcl_vector<dbecl_episeg_sptr>& episegs) { episegs_ = episegs; }

  //: Combine all new points with existing tracks
  void track();
  //: Combine all new points with existing tracks on one epipolar line
  void track(unsigned int index);

  //: Return the current tracks
  vcl_vector<vcl_vector<dbetl_point_track_sptr> > tracks() const { return tracks_; }

  //: Create all the points 
  void create_points();

  //: Create all the points at a given angle
  void create_points(double angle, vcl_vector<dbetl_point_2d_sptr>& points) const;

private:
  //: The current camera to associate with new points
  dbetl_camera_sptr camera_;

  //: The current image used in image statistics gathering
  vil_image_resource_sptr image_;

  //: The current vector of episegs to extract points from
  vcl_vector<dbecl_episeg_sptr> episegs_;

  //: The ordered set of tracks
  vcl_vector<vcl_vector<dbetl_point_track_sptr> > tracks_;

  //: The ordered set of epipolar profiles
  vcl_vector<vcl_vector<dbetl_point_2d_sptr> > curr_pts_;

  //: The minimum angle for a track
  double min_angle_;

  //: The change in angle between adjacent tracks
  double d_angle_;
};



#endif // dbetl_tracker_h_
