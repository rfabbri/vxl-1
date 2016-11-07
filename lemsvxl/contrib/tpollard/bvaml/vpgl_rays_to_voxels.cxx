#ifndef _vpgl_rays_to_voxels_cxx_
#define _vpgl_rays_to_voxels_cxx_

#include "vpgl_rays_to_voxels.h"

#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vgl/vgl_line_3d_2_points.h>
#include <vgl/vgl_distance.h>


//-----------------------------------------
vpgl_rays_to_voxels::vpgl_rays_to_voxels(
  const vpgl_perspective_camera<double>& camera,
  const vgl_point_3d<double>& volume_corner,
  double voxel_length,
  const vgl_vector_3d<int>& num_voxels ) :
    camera_( camera ),
    volume_lower_corner_( volume_corner ),
    voxel_length_( voxel_length ),
    num_voxels_( vgl_point_3d<int>( 
      num_voxels.x(), num_voxels.y(), num_voxels.z() ) )
{
  volume_upper_corner_ = vgl_point_3d<double>(
    volume_lower_corner_.x() + voxel_length_*num_voxels_.x(),
    volume_lower_corner_.y() + voxel_length_*num_voxels_.y(),
    volume_lower_corner_.z() + voxel_length_*num_voxels_.z() );
  camera_center_.set( camera_.camera_center().x(),
    camera_.camera_center().y(), camera_.camera_center().z() );

  // Determine the max amount of voxels in a ray.
  num_voxels_to_reserve_ = num_voxels.x();
  if( num_voxels_to_reserve_ < num_voxels.y() ) 
    num_voxels_to_reserve_ = num_voxels.y();
  if( num_voxels_to_reserve_ < num_voxels.z() ) 
    num_voxels_to_reserve_ = num_voxels.z();
  num_voxels_to_reserve_ *= 9; // somewhat arbitrary.

  // Get a rough clipping area of the voxel volume in the image.
  min_i_ = 100000; max_i_ = 0; min_j_ = 100000; max_j_ = 0;
  for( int i = 0; i < 8; i++ ){
    double px = volume_lower_corner_.x();
    double py = volume_lower_corner_.y();
    double pz = volume_lower_corner_.z();
    if( i == 1 || i == 3 || i == 5 || i == 7 ) px = volume_upper_corner_.x();
    if( i == 2 || i == 3 || i == 6 || i == 7 ) py = volume_upper_corner_.y();
    if( i == 4 || i == 5 || i == 6 || i == 7 ) pz = volume_upper_corner_.z();
    vgl_homg_point_2d<double> projp_homg = camera_.project( 
      vgl_homg_point_3d<double>( px, py, pz ) );
    vgl_point_2d<double> projp( 
      projp_homg.x()/projp_homg.w(), projp_homg.y()/projp_homg.w() );
    if( projp.x() < min_i_ ) min_i_ = (int)floor( projp.x() );
    if( projp.x() > max_i_ ) max_i_ = (int)ceil( projp.x() );
    if( projp.y() < min_j_ ) min_j_ = (int)floor( projp.y() );
    if( projp.y() > max_j_ ) max_j_ = (int)ceil( projp.y() );
  }
};


//----------------------------------------------
void 
vpgl_rays_to_voxels::get_ray_voxels(
  const vgl_point_2d<int>& pixel_index,
  vcl_vector< vgl_point_3d<int> >& ray_voxels,
  bool unique_assignment )
{
  ray_voxels.clear();
  ray_voxels.reserve( num_voxels_to_reserve_ );

  if( !unique_assignment ){
    get_ray_voxels( pixel_index, ray_voxels );
    return;
  }

  vcl_vector< vgl_point_3d<int> > pre_voxels;
  pre_voxels.reserve( num_voxels_to_reserve_ );
  get_ray_voxels( pixel_index, pre_voxels );

  // Project each voxel into the image and see if its closest to this pixel
  for( unsigned i = 0; i < pre_voxels.size(); i++ ){
    vgl_point_3d<double> voxel_coords = volume_lower_corner_ + voxel_length_*
      vgl_vector_3d<double>( pre_voxels[i].x(), pre_voxels[i].y(), pre_voxels[i].z() );
    vgl_homg_point_2d<double> proj = camera_.project( 
      vgl_homg_point_3d<double>( voxel_coords ) );
    if( pixel_index.x() - proj.x()/proj.w() <= .5 &&
        pixel_index.x() - proj.x()/proj.w() > -.5 &&
        pixel_index.y() - proj.y()/proj.w() <= .5 &&
        pixel_index.y() - proj.y()/proj.w() > -.5 ) 
      ray_voxels.push_back( pre_voxels[i] );
  }
};


//-----------------------------------------
void 
vpgl_rays_to_voxels::get_ray_voxels(
  const vgl_point_2d<int>& pixel_index,
  vcl_vector< vgl_point_3d<int> >& ray_voxels )
{
  ray_voxels.clear();
  ray_voxels.reserve( num_voxels_to_reserve_ );

  // Clip pixel against rough voxel volume bounds.
  if( pixel_index.x() < min_i_ || pixel_index.x() > max_i_ ||
    pixel_index.y() < min_j_ || pixel_index.y() > max_j_ ) return;

  // Get the ray for this pixel.
  vgl_line_3d_2_points<double> ray = camera_.backproject(
    vgl_point_2d<double>( pixel_index.x(), pixel_index.y() ) );
  vgl_vector_3d<double> ray_dir = ray.direction();
  normalize( ray_dir );
  if( ray_dir.z() > 0 )
    return; // Z-ASSUMPTION

  // For the time being start the ray search at the top z plane.
  vgl_point_3d<double> search_point;
  double search_distance = 0;
  double start_factor = ( volume_upper_corner_.z() - camera_center_.z() )/
    ray_dir.z();
  search_point = vgl_point_3d<double>( 
    camera_center_.x() + start_factor*ray_dir.x(), 
    camera_center_.y() + start_factor*ray_dir.y(), 
    volume_upper_corner_.z() );

  // Search along the ray at voxel increments.
  while( search_point.z() > volume_lower_corner_.z() ){

    vgl_vector_3d<double> search_voxel_coords = (1.0/voxel_length_)*
      (search_point - volume_lower_corner_);
    vgl_point_3d<int> search_voxel( (int)floor( search_voxel_coords.x() ), 
      (int)floor( search_voxel_coords.y() ), (int)floor( search_voxel_coords.z() ) );

    // Check the 27 voxels around the search voxel. Put all voxels that haven't
    // been seen by this ray that are sufficiently close to the ray in new_voxels.
    vcl_vector< vgl_point_3d<int> > new_voxels; new_voxels.reserve( 27 );
    vcl_vector<double> new_voxel_distances; new_voxel_distances.reserve( 27 );
    for( int i = -1; i <= 1; i++ ){
      for( int j = -1; j <= 1; j++ ){
        for( int k = -1; k <= 1; k++ ){

          vgl_point_3d<int> this_voxel = search_voxel + vgl_vector_3d<int>(i,j,k);
          if( this_voxel.x() < 0 || this_voxel.y() < 0 || this_voxel.z() < 0 ||
            this_voxel.x() >= num_voxels_.x() || this_voxel.y() >= num_voxels_.y() ||
            this_voxel.z() >= num_voxels_.z() ) continue;
          vgl_point_3d<double> v = volume_lower_corner_ + voxel_length_*
            vgl_vector_3d<double>( this_voxel.x(), this_voxel.y(), this_voxel.z() );

          double this_distance = dot_product( ray_dir, 
            v - vgl_point_3d<double>( camera_.camera_center() ) );
          if( this_distance < search_distance ) continue;

          vgl_point_3d<double> closest_point_on_ray = camera_center_ +
            this_distance*ray_dir;
          double distance_to_ray = vgl_distance( v, closest_point_on_ray );
          if( distance_to_ray > voxel_length_ )
            continue;

          new_voxels.push_back( this_voxel );
          new_voxel_distances.push_back( this_distance );
        }
      }
    }

    // Add the discovered voxels to the list in the order of closeness to camera.
    bool all_new_voxels_added = false;
    while( all_new_voxels_added == false ){

      double smallest_distance = 10000000;
      int smallest_distance_index = -1;
      for( unsigned i = 0; i < new_voxels.size(); i++ ){
        if( new_voxel_distances[i] <= search_distance )
          continue;
        if( new_voxel_distances[i] <= smallest_distance ){
          smallest_distance = new_voxel_distances[i];
          smallest_distance_index = i;
        }
      }

      if( smallest_distance_index == -1 )
        break;

      search_distance = smallest_distance;
      ray_voxels.push_back( new_voxels[ smallest_distance_index ] );
    }

    search_point = search_point + voxel_length_*ray_dir;
  }

};



#endif // _vpgl_rays_to_voxels_cxx_
