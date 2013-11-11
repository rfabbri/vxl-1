#ifndef _dbrcl_point_tracker_
#define _dbrcl_point_tracker_

#include "dbrcl_point_tracker.h"

#include <vgl/vgl_distance.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vtol/vtol_vertex_2d.h>



//--------------------------------------
bool 
dbrcl_point_tracker::track_points(
  vcl_vector< vil_image_resource_sptr >& images,
  vgel_multi_view_data_vertex_sptr& tracked_points )
{
  vcl_cerr << "dbrcl_point_tracker::track_points:\n" 
    << " tracking points\n";

  // Use KLT to get image correspondences.
  vgel_kl_params kl_params;
  kl_params.replaceLostPoints = true;
  kl_params.numpoints = static_cast<int>((images[0]->ni()+images[0]->nj())/4.0);
  kl_params.search_range = (int)(1+2*max_adjacent_frame_motion_);
  tracked_points = new vgel_multi_view_data<vtol_vertex_2d_sptr>(images.size());
  vgel_kl kl_points(kl_params);
  kl_points.match_sequence( images, tracked_points );

  // If the images have been registered we'll want to eliminate points along the 
  // boundary. If the images are not registered, the following should do nothing.
  for( int i = 0; i < tracked_points->get_nb_views(); i++ ){
    vcl_vector< vtol_vertex_2d_sptr > points;
    tracked_points->get( i, points );
    vil_image_view<double> this_image = 
      vil_convert_to_grey_using_average(
        vil_convert_cast(double(), images[i]->get_view()) );
    for( int j = 0; j < static_cast<int>(points.size()); j++ ){
      int x = (int)floor( points[j]->x() );
      int y = (int)floor( points[j]->y() );
      bool bad_point = false;
      for( int dx = -5; dx <= 5; dx++ ){
        for( int dy = -5; dy <= 5; dy++ ){
          if( (double)this_image(x+dx,y+dy) <= 1 )
            bad_point = true; // Might want to bound this.
        }
      }
      if( bad_point ){
      tracked_points->remove( i, points[j] ); }
    }
  }

  vcl_cerr << "dbrcl_point_tracker::track_points:\n" 
    << "  success\n";
  return true;
}


#endif // _dbrcl_point_tracker_cxx_
