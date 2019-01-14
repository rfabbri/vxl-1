#ifndef _dbrcl_compute_constrained_cameras_h_
#define _dbrcl_compute_constrained_cameras_h_
//:
// \file
// \brief ...
// \author Thomas Pollard
// \date 10/26/05
//

#include <vector>
#include <vgl/vgl_point_2d.h>
#include <vpgl/vpgl_proj_camera.h>


//: A class storing a set of constraints on a single projective camera.
class dbrcl_camera_constraints {

public:

  dbrcl_camera_constraints(){ frame_number = 0; }

  int frame_number;

  int num_constraints()
  { return 2*image_points.size() + height_top_points.size(); }

  // Corresponding world and image points.
  std::vector< vgl_point_3d<double> > world_points;
  std::vector< vgl_point_2d<double> > image_points;

  // Tops and bottoms of objects in the world that point "up", i.e. parallel 
  // to the z axis.
  std::vector< vgl_point_2d<double> > height_top_points;
  std::vector< vgl_point_2d<double> > height_bot_points;

};


//------------------------------------
class dbrcl_compute_constrained_cameras {

public:

  dbrcl_compute_constrained_cameras()
  { verbose = false; }

  bool read_constraint_file( std::string constraint_file );

  bool compute_cameras(
    std::vector< vpgl_proj_camera<double> >& cameras,
    std::vector<int>& frames,
    bool affine_camera = false );

  bool compute_cameras_separately(
    std::vector< vpgl_proj_camera<double> >& cameras,
    std::vector<int>& frames,
    bool affine_camera = false );


  void get_world_points(
    std::vector< vgl_point_3d<double> >& world_points );

  void add_constraint( 
    dbrcl_camera_constraints new_constraint )
  { constraints_.push_back( new_constraint ); }

  bool verbose;


protected:

  std::vector< dbrcl_camera_constraints > constraints_;

  // Other world points with no image correspondences, for visualization.
  std::vector< vgl_point_3d<double> > unmatched_world_points_;

};

#endif // _dbrcl_compute_constrained_cameras_h_
