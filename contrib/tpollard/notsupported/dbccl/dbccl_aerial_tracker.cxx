#ifndef _dbccl_aerial_tracker_cxx_
#define _dbccl_aerial_tracker_cxx_

#include "dbccl_aerial_tracker.h"

#include <vnl/vnl_vector_fixed.h>
#include <vgl/vgl_distance.h>

#include <vil/vil_save.h> // DEBUGGING


//---------------------------------------------
dbccl_aerial_tracker::dbccl_aerial_tracker():params(dbccl_aerial_tracker_params())
{

};


//----------------------------------------
void 
dbccl_aerial_tracker::track_frame(
  const vil_image_view<vxl_byte>& new_frame,
  const vcl_vector< vgl_point_2d<int> >& new_tracks )
{
  bool debugging = false;
  dbccl_fast_mi fmi( &mi_params );

  // Compute MI in regions around the current live tracks.
  vcl_vector< vnl_matrix<float> > region_mi;
  int num_live_tracks = 0;
  for( int t = 0; t < tracks_.size(); t++ ){
    int tr1 = tracks_[t].x(); int tr2 = tracks_[t].y();
    if( track_masks_[t] == false ) 
      region_mi.push_back(vnl_matrix<float>(1,1,0));
    else{
      vnl_matrix<float> new_region_mi;
      fmi.compute_mi_region( new_frame, tracks_[t], mi_mask_[t], new_region_mi );
      region_mi.push_back( new_region_mi );
      num_live_tracks++;
    }
  }

  // DEBUGGING
  vil_image_view<vxl_byte> region_mi_img(
    tracks_.size()*mi_params.mi_region_diameter(), mi_params.mi_region_diameter() );
  if( debugging  ){
    float max_mi = 0;
    for( int t = 0; t < tracks_.size(); t++ ){
      if( track_masks_[t] == false ) continue;
      for( int i = 0; i < mi_params.mi_region_diameter(); i++ ){
        for( int j = 0; j < mi_params.mi_region_diameter(); j++ ){
          if( max_mi < region_mi[t](i,j) ) max_mi = region_mi[t](i,j);
        }
      }
    }
    for( int t = 0; t < tracks_.size(); t++ ){
      for( int i = 0; i < mi_params.mi_region_diameter(); i++ ){
        for( int j = 0; j < mi_params.mi_region_diameter(); j++ ){
          if( track_masks_[t] == true )
            region_mi_img(t*mi_params.mi_region_diameter()+i,j) = 
              (int)floor(255*region_mi[t](j,i)/max_mi);
          else
            region_mi_img(t*mi_params.mi_region_diameter()+i,j) = 0;
        }
      }
    }
    if( tracks_.size() > 0 )
      vil_save( region_mi_img, "f:\\testing\\output\\dbccl_regions.tif" );
  } // END DEBUGGING

  // Search a 3-dim subspace of fundamental matrices for a best match.
  vnl_matrix<float> best_fm;
  float best_total = 0;
  vcl_vector< vgl_point_2d<int> > best_tracks;
  vcl_vector<float> best_vals;
  int total_fms = 0, usable_fms = 0;
  for( int ts = 0; ts < params.num_fm_trans_searches; ts++ ){
    float tx = cos(3.14159*ts/(params.num_fm_trans_searches-1.0));
    float ty = sin(3.14159*ts/(params.num_fm_trans_searches-1.0));
      for( int rs = 0; rs < params.num_fm_rot_searches; rs++ ){
        float r = -params.max_rotation + 2*params.max_rotation*rs/
          (params.num_fm_rot_searches-1.0);
        total_fms++;

        // Construct the FM.
        float cosr = cos(r); float sinr = sin(r);
        //cosr = 1; sinr = 0; // DEBUGGING
        float a1 = tx*cosr + ty*sinr;
        float a2 = -tx*sinr + ty*cosr;
        float image_width = new_frame.ni(), image_height = new_frame.nj();
        vnl_matrix<float> m1(3,3,0), m2(3,3,0);
        m1(0,0) = cosr; m1(0,1) = -sinr; m1(1,0) = sinr; m1(1,1) = cosr; m1(2,2) = 1.0;
        m1(2,0) = -image_width*cosr/2.0 - image_height*sinr/2.0 + image_width/2.0;
        m1(2,1) = image_width*sinr/2.0 - image_height*cosr/2.0 + image_height/2.0;
        m2(2,0) = -a2; m2(2,1) = a1; m2(0,2) = a2; m2(1,2) = -a1;
        vnl_matrix<float> this_fm = m1*m2;

        // DEBUGGING
        vil_image_view<vxl_byte> fm_img;
        fm_img.deep_copy( region_mi_img );
        // END DEBUGGING

        // For each FM, find the best match for each live track in the next
        // image that agrees with the FM.
        float this_total = 0;
        vcl_vector< vgl_point_2d<int> > these_tracks;
        vcl_vector<float> these_vals;
        for( int t = 0; t < tracks_.size(); t++ ){

          if( track_masks_[t] == false ){
            these_tracks.push_back( vgl_point_2d<int>(-1,-1) );
            these_vals.push_back( 0.0 );
            continue;
          }

          vnl_vector<float> this_point(3);
          this_point[0] = tracks_[t].x();
          this_point[1] = tracks_[t].y(); this_point[2] = 1;
          vnl_vector<float> this_line = this_fm*this_point;

          // Find the end points of this line with the search region.
          float left_x = this_point[0]-mi_params.mi_region_rad;
          float right_x = this_point[0]+mi_params.mi_region_rad;
          float bot_y = this_point[1]-mi_params.mi_region_rad;
          float top_y = this_point[1]+mi_params.mi_region_rad;
          vgl_point_2d<float> p1, p2;
          int num_intersections = 0;
          for( int side = 0; side < 4; side++ ){
            vgl_point_2d<float> new_intersection(-1,-1);
            if( side == 0 && abs( this_line[1] ) > .001 ){
              float left_y = ( this_line[0]*left_x + this_line[2] )/(-this_line[1]);
              if( left_y >= bot_y && left_y <= top_y )
                new_intersection = vgl_point_2d<float>( left_x, left_y );
            }
            else if( side == 1 && abs( this_line[1] ) > .001 ){
              float right_y = ( this_line[0]*right_x + this_line[2] )/(-this_line[1]);
              if( right_y >= bot_y && right_y <= top_y )
                new_intersection = vgl_point_2d<float>( right_x, right_y );
            }
            else if( side == 2 && abs( this_line[0] ) > .001 ){
              float bot_x = ( this_line[1]*bot_y + this_line[2] )/(-this_line[0]);
              if( bot_x >= left_x && bot_x <= right_x )
                new_intersection = vgl_point_2d<float>( bot_x, bot_y );
            }
            else if( side == 3 && abs( this_line[0] ) > .001 ){
              float top_x = ( this_line[1]*top_y + this_line[2] )/(-this_line[0]);
              if( top_x >= left_x && top_x <= right_x )
                new_intersection = vgl_point_2d<float>( top_x, top_y );
            }
            if( new_intersection.x() != -1 ){
              if( num_intersections == 0 )
                p1 = new_intersection;
              else if( num_intersections == 1 ){
                if( vgl_distance( p1, new_intersection ) > 3 )
                  p2 = new_intersection;
                else num_intersections--;
              }
              num_intersections++;
              if( num_intersections == 2 ) break;
            }
          }
          
          // If this line doesn't intersect the search region, discard the FM.
          if( num_intersections != 2 ){ this_total = 0; break; }

          // Otherwise search along the line for the best match.
          float best_line_mi = 0;
          vgl_point_2d<int> best_line_point;
          vgl_vector_2d<float> v = p2-p1;
          float v_length = v.length(); normalize(v);
          for( float dv = 0; dv < v_length; dv+=.5 ){
            vgl_point_2d<float> start_coord = p1 + dv*v;
            for( int round = 0; round < 4; round++ ){
              vgl_point_2d<int> image_coord( 
                (int)floor(start_coord.x()), (int)floor(start_coord.y()) );
              if( round == 1 || round == 3 ) image_coord.x()++;
              if( round == 2 || round == 3 ) image_coord.y()++;
              vgl_point_2d<int> track_mask_coord =
                vgl_point_2d<int>( mi_params.mi_region_rad, mi_params.mi_region_rad ) + 
                ( image_coord - tracks_[t] );
              if( track_mask_coord.x() >= 0 && track_mask_coord.y() >= 0 &&
                  track_mask_coord.x() < mi_params.mi_region_diameter() && 
                  track_mask_coord.y() < mi_params.mi_region_diameter() ){
                float this_mi = region_mi[t]( track_mask_coord.y(), track_mask_coord.x() );
                if( this_mi > best_line_mi ){
                  best_line_mi = this_mi;
                  best_line_point = image_coord;
                }
                if( debugging ){ // DEBUGGING
                  fm_img( mi_params.mi_region_diameter()*t + track_mask_coord.x(), 
                    track_mask_coord.y() ) = 0;
                } // END DEBUGGING
              }
            }
          }
          this_total += best_line_mi;
          these_tracks.push_back( best_line_point );
          these_vals.push_back( best_line_mi );
        } // tracks

        if( this_total != 0 ) usable_fms++;
        if( this_total > best_total ){
          best_total = this_total;
          best_vals = these_vals;
          best_tracks = these_tracks;
          best_fm = this_fm;
          // DEBUGGING
          vil_save( fm_img, "f:\\testing\\output\\dbccl_regions.tif" ); 
          // END DEBUGGING
        }

      }
  }

  // Record the optimal tracks, remove tracks that don't match well.
  // TO BE DONE LATER
  for( int t = 0; t < tracks_.size(); t++ ){
    tracks_[t] = best_tracks[t];
    if( track_masks_[t] == false )
      track_masks_[t] = false;
    else if( best_tracks[t].x() == -47 )
      track_masks_[t] = false;
    else
      track_masks_[t] = true;
  }

  // Now find the optimal warping of the original image neihborhoods.
  for( int t = 0; t < tracks_.size(); t++ ){
    if( track_masks_[t] == false )
      continue;
    tracks_[t];
    // TO BE DONE LATER
  }
  
  // For a track starting this frame, copy a large niegborhood of the starting point
  // into nbhd_window and an initial unwarped small neighborhood into mi_mask.
  vnl_matrix<float> identity_transform(2,2,0.0);
  for( int i = 0; i < 2; i++ ) identity_transform(i,i) = 1.0;
  for( int t = 0; t < new_tracks.size(); t++ ){

    vnl_matrix<int> this_nbhd_window( 
      params.nbhd_diameter(), params.nbhd_diameter(), -1 );
    for( int i = 0; i < params.nbhd_diameter(); i++ ){
      for( int j = 0; j < params.nbhd_diameter(); j++ ){
        int image_x = new_tracks[t].x()+j-params.nbhd_rad;
        int image_y = new_tracks[t].y()+i-params.nbhd_rad;
        this_nbhd_window(i,j) = new_frame( image_x, image_y );
      }
    }

    vnl_matrix<int> this_mi_mask( 
      mi_params.mi_mask_diameter(), mi_params.mi_mask_diameter(), -1 );
    for( int i = 0; i < mi_params.mi_mask_diameter(); i++ ){
      for( int j = 0; j < mi_params.mi_mask_diameter(); j++ ){
        int image_x = new_tracks[t].x()+j-mi_params.mi_mask_rad;
        int image_y = new_tracks[t].y()+i-mi_params.mi_mask_rad;
        this_mi_mask(i,j) = new_frame( image_x, image_y );
      }
    }

    tracks_.push_back( new_tracks[t] );
    track_masks_.push_back( true );
    nbhd_transform_.push_back( identity_transform );
    mi_mask_.push_back( this_mi_mask );
    nbhd_window_.push_back( this_nbhd_window );
  }
};


//----------------------------------------------
bool
dbccl_aerial_tracker::transform_window(
  const vnl_matrix<int>& full_window,
  const vnl_matrix<float>& transform,
  vnl_matrix<int>& warped_window )
{
  float trans_volume = 
    abs( transform(0,0)*transform(1,1)-transform(0,1)*transform(1,0) );
  if( trans_volume < .5 || trans_volume > 2 ) return false;

  assert( full_window.rows() == 1+2*params.nbhd_rad &&
          full_window.cols() == 1+2*params.nbhd_rad );
  warped_window = vnl_matrix<int>( 1+2*mi_params.mi_mask_rad, 1+2*mi_params.mi_mask_rad );

  for( int wx = 0; wx < 1+2*mi_params.mi_mask_rad; wx++ ){
    for( int wy = 0; wy < 1+2*mi_params.mi_mask_rad; wy++ ){
      float x = wx-mi_params.mi_mask_rad, y = wy-mi_params.mi_mask_rad;
      float tx = x*transform(0,0) + y*transform(0,1);
      float ty = x*transform(1,0) + y*transform(1,1);
      tx += params.nbhd_rad;
      ty += params.nbhd_rad;
      int ftx = floor(tx), fty = floor(ty);
      float w00 = (ftx+1-tx)*(fty+1-ty), w01 = (ftx+1-tx)*(ty-fty),
        w10 = (tx-ftx)*(fty+1-ty), w11 = (tx-ftx)*(ty-fty);
      warped_window(wx,wy) = (int)floor( w00*full_window(ftx,fty)
        +w01*full_window(ftx,fty+1)
        +w10*full_window(ftx+1,fty)+w11*full_window(ftx+1,fty+1) );
    }
  }
  return true;
};


#endif // _dbccl_tracker_cxx_
