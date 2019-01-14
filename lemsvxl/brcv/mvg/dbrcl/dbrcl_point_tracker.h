#ifndef _dbrcl_point_tracker_h_
#define _dbrcl_point_tracker_h_
//:
// \file
// \brief ...
// \author Thomas Pollard
// \date 10/26/05
//

#include <vector>
#include <vil/vil_image_resource.h>
#include <vgel/vgel_multi_view_data.hxx>
#include <vgel/vgel_kl.h>


//------------------------------------
class dbrcl_point_tracker {

public:

  dbrcl_point_tracker(
    double max_adjacent_frame_motion = 3 ) :
      verbose( false ),max_adjacent_frame_motion_( max_adjacent_frame_motion ){};

  bool track_points(
    std::vector< vil_image_resource_sptr >& images,
    vgel_multi_view_data_vertex_sptr& tracked_points );

  bool verbose;


protected:

  // Maximum pixel movement for corresponding points between adjacent frames.
  double max_adjacent_frame_motion_;

};

#endif // _dbrcl_point_tracker_h_
