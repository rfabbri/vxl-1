#ifndef _dbrcl_compute_cameras_h_
#define _dbrcl_compute_cameras_h_
//:
// \file
// \brief ...
// \author Thomas Pollard
// \date 10/26/05
//

#include <vcl_vector.h>
#include <vtol/vtol_vertex_2d.h>
#include <vgel/vgel_multi_view_data.txx>
#include <vgel/vgel_kl.h>
#include <vpgl/vpgl_proj_camera.h>
#include <dbinfo/dbinfo_track_geometry.h>

//------------------------------------
class dbrcl_compute_cameras {

public:

  dbrcl_compute_cameras(){ verbose = false; }

  bool compute_cameras(
    vgel_multi_view_data_vertex_sptr& tracked_points,
    const vcl_vector< vpgl_proj_camera<double> >& known_cameras,
    const vcl_vector<int>& known_frames,
    vcl_vector< vpgl_proj_camera<double> >& cameras );

  bool compute_cameras(
    vcl_vector< dbinfo_track_geometry_sptr >& tracks,
    const vcl_vector< vpgl_proj_camera<double> >& known_cameras,
    const vcl_vector<int>& known_frames,
    vcl_vector< vpgl_proj_camera<double> >& cameras );

  bool verbose;


protected:


};


#endif // _dbrcl_compute_cameras_h_
