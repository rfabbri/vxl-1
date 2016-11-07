#ifndef _bvaml_world_cxx_
#define _bvaml_world_cxx_

#include "bvaml_world.h"
#include "bvaml_log_writer.h"
#include "bvaml_ray.h"
#include "bvaml_world_viewer.h"
#include "vpgl_rays_to_voxels.h"
#include <baml/baml_prob_img.h>
#include <baml/baml_multi_mog.h>

#include <vcl_cmath.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_line_3d_2_points.h>
#include <vgl/vgl_intersection.h>
#include <vnl/vnl_matrix.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_resample_bilin.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_reg_exp.h>
#include <vul/vul_timer.h>
#include <vpgl/vpgl_perspective_camera.h>


//---------------------------------------------
bvaml_world::bvaml_world(
  bvaml_params* params ) :
    params_( params )
{
  // Create the supervoxels.
  for( int x = 0; x < params_->num_supervoxels.x(); x++ ){
    for( int y = 0; y < params_->num_supervoxels.y(); y++ ){
      for( int z = 0; z < params_->num_supervoxels.z(); z++ ){

        // Contruct this supervoxel's filename.
        vcl_stringstream new_filename;
        new_filename << params_->model_dir << "/sv_";
        if( x < 10 ) new_filename << "00";
        else if( x < 100 ) new_filename << "0";
        new_filename << x << '_';
        if( y < 10 ) new_filename << "00";
        else if( y < 100 ) new_filename << "0";
        new_filename << y << '_';
        if( z < 10 ) new_filename << "00";
        else if( z < 100 ) new_filename << "0";
        new_filename << z;

        bvaml_supervoxel new_supervoxel(
          params_,
          new_filename.str() );
        supervoxels_.push_back( new_supervoxel );
      }
    }
  }
};


//---------------------------------------------
void
bvaml_world::process(
  int mode,
  vcl_string namebase,
  const vil_image_view<vxl_byte>& img,
  const vpgl_proj_camera<double>& camera,
  const vnl_vector<float>& light,
  const vcl_vector< vgl_point_2d<int> >& inspect_pixels )
{
  bvaml_log_writer log( "bvaml_world::process" );

  // Check that the camera is good.
  double camera_z = camera.camera_center().z()/camera.camera_center().w();
  if( camera_z <= 0 )
    log.print_error( "camera has negative z center" );

  // Determine the pixel scaling to remap the image to an image
  // of approximately voxel sized pixels. NOT IMPLEMENTED.
  float image_scale = params_->image_scale;

  // Remap the image, and adjust camera.
  log.print_msg( "remapping image" );
  vnl_matrix_fixed<double,3,3> scale_matrix(0.0);
  scale_matrix(0,0) = scale_matrix(1,1) = image_scale; scale_matrix(2,2) = 1.0;
  vpgl_proj_camera<double> scale_camera = premultiply( camera, scale_matrix );

  vil_image_view<float> image_scaled( 
    (int)floor( img.ni()*image_scale ), (int)floor( img.nj()*image_scale ) );
  vil_resample_bilin( img, image_scaled, 
    (int)floor( img.ni()*image_scale ), (int)floor( img.nj()*image_scale ) );
  vil_image_view<vxl_byte> changes_scaled( image_scaled.ni(), image_scaled.nj() );
  vil_image_view<float> changes_scaled_full( image_scaled.ni(), image_scaled.nj() );

  // NEW set up prob_img_.
  baml_prob_img prob_img( 1, 1 );
  if( mode == 5 || mode == 6 ){
    prob_img.initialize( image_scaled.ni(), image_scaled.nj() );
  }

  // Get a map of all pixels to inspect.
  vnl_matrix<int> inspect_map( image_scaled.ni(), image_scaled.nj(), 0 );
  for( unsigned i = 0; i < inspect_pixels.size(); i++ )
    inspect_map( (int)floor( image_scale*inspect_pixels[i].x() ), 
      (int)floor( image_scale*inspect_pixels[i].y() ) ) = 1;
  bvaml_world_viewer viewer( params_ );
  if( inspect_pixels.size() != 0 )
    viewer.start_inspection_file();

  // Set up the ray calculation machinery.
  vpgl_perspective_camera<double> perspective_camera;
  if( !vpgl_perspective_decomposition( scale_camera.get_matrix(), perspective_camera ) )
    log.print_error( "camera is not perspective" );
  vpgl_rays_to_voxels rtsv( perspective_camera, params_->corner, 
    params_->voxel_length*params_->supervoxel_length,
    params_->num_supervoxels );
  vpgl_rays_to_voxels rtv( perspective_camera, params_->corner, 
    params_->voxel_length,
    params_->num_voxels() );

  // Divide the image into subimages of approximately supervoxel
  // size which will be trained on separately.
  vgl_vector_2d<int> num_subimgs( 
    (int)ceil( image_scaled.ni()/(double)params_->supervoxel_length ),
    (int)ceil( image_scaled.nj()/(double)params_->supervoxel_length ) );
  for( int subimg_x = 0; subimg_x < num_subimgs.x(); subimg_x++ ){
    for( int subimg_y = 0; subimg_y < num_subimgs.y(); subimg_y++ ){

      vcl_stringstream subimg_msg;
      subimg_msg << "processing subimage " << subimg_x << ' ' << subimg_y;
      log.print_msg( subimg_msg.str() );
      bvaml_log_writer subimg_log( "bvaml_world::process_subimg" );

      // Load into memory all supervoxels needed for this subimg.
      // NOTE: This doesn't work well currently and not all needed supervoxels are loaded.
      subimg_log.print_msg( "loading supervoxels" );
      bool no_supervoxels_loaded = true;
      for( int i = subimg_x*params_->supervoxel_length; i < (subimg_x+1)*params_->supervoxel_length; i++ ){
        for( int j = subimg_y*params_->supervoxel_length; j < (subimg_y+1)*params_->supervoxel_length; j++ ){
          if( i <= 0 || i >= (int)image_scaled.ni()-1 || j <= 0 || j >= (int)image_scaled.nj()-1 ) continue;
          vcl_vector< vgl_point_3d<int> > supervoxels_on_pixel_ray;
          rtsv.get_ray_voxels( vgl_point_2d<int>(i,j), supervoxels_on_pixel_ray, false );
          for( unsigned k = 0; k < supervoxels_on_pixel_ray.size(); k++ ){
            supervoxels_[supervoxel_index(supervoxels_on_pixel_ray[k])].load_local_appearance( light );
            supervoxels_[supervoxel_index(supervoxels_on_pixel_ray[k])].load_occupancies();
            if( mode == 3 || mode == 4 )
              supervoxels_[supervoxel_index(supervoxels_on_pixel_ray[k])].load_misc_storage();
            no_supervoxels_loaded = false;
          }
          changes_scaled(i,j) = 255;
          changes_scaled_full(i,j) = 0.0;
        }
      }
      if( no_supervoxels_loaded ){
        subimg_log.print_msg( "no supervoxels loaded" );
        continue;
      }

      // NOTE: This is where to begin any parallelization of the code.

      // Now process each pixel ray in this subimg. 
      subimg_log.print_msg( "updating rays" );
      for( int i = subimg_x*params_->supervoxel_length; i < (subimg_x+1)*params_->supervoxel_length; i++ ){
        for( int j = subimg_y*params_->supervoxel_length; j < (subimg_y+1)*params_->supervoxel_length; j++ ){
          if( i <= 0 || i >= (int)image_scaled.ni()-1 || j <= 0 || j >= (int)image_scaled.nj()-1 ) continue;

          float this_color = (float)( image_scaled(i,j)/255.0 ); 
          if( this_color < 0 ) this_color = 0; // Might happen with bicubic interpolation.

          vcl_vector< vgl_point_3d<int> > voxels_on_pixel_ray;
          bool unique_ray = true; if( mode == 4 || mode == 2 ) unique_ray = false;
          rtv.get_ray_voxels( vgl_point_2d<int>(i,j), voxels_on_pixel_ray, unique_ray );

          bool full_ray_intersection = true;
          if( voxels_on_pixel_ray.size() == 0 ) continue;
          if( voxels_on_pixel_ray[0].z() < (params_->num_voxels().z()-2) ||
            voxels_on_pixel_ray[voxels_on_pixel_ray.size()-1].z() > 1 ) full_ray_intersection = false;

          // Fill the ray with the voxel data.
          bvaml_ray ray( params_, voxels_on_pixel_ray.size() );
          for( unsigned k = 0; k < voxels_on_pixel_ray.size(); k++ ){
            bvaml_voxel* new_voxel = new bvaml_voxel( params_ );
            get_voxel( voxels_on_pixel_ray[k], *new_voxel, true, (mode==3 || mode==4), true, &light );
            ray.voxels.push_back( new_voxel );

            // Display inspection voxels.
            for( unsigned ivx = 0; ivx < params_->inspect_voxels.size(); ivx++ ){
              vgl_point_3d<int> voxel_to_inspect = params_->inspect_voxels[ivx];
              vgl_point_3d<int> this_voxel = voxels_on_pixel_ray[k];
              if( voxel_to_inspect == this_voxel ){
                vcl_stringstream vx_stream; vx_stream << voxels_on_pixel_ray[k];
                vcl_stringstream ivx_stream; ivx_stream << 
                  "Seen color: " << this_color << '\n' << 
                  "Occupancy prob: " << ray.voxels[k]->occupancy_prob[0] << '\n' <<
                  "Color prob: " << ray.voxels[k]->appearance->prob( this_color, light );
                log.print_block( "Printing voxel "+vx_stream.str(), ivx_stream.str() );
              }                  
            }
          }

          if( inspect_map(i,j) == 1 )
            viewer.add_inspection_ray( voxels_on_pixel_ray, ray, true );

          // Calculate the probability or expected color of this pixel.
          float color_prob = 0, pred_color = 0;
          if( mode == 0 )
            color_prob = ray.update( this_color, light );
          else if( mode == 1 )
            color_prob = ray.prob( this_color, light );
          else if( mode == 2 )
            pred_color = ray.expected_color( light );
          else if( mode == 3 )
            ray.store_color( this_color );
          else if( mode == 4 ){
            float prob_not_seen;
            ray.predicted_color( pred_color, prob_not_seen );
            //pred_color = prob_not_seen; // TEMP FOR EXP
          }
          else if( (mode == 5 || mode == 6) && full_ray_intersection ){
            baml_mog* new_mog = prob_img(i,j);
            ray.distribution( new_mog, light );
            delete new_mog;
          }
          if( mode == 0 || mode == 1 || mode == 3 ){
            int prob_intensity = (int)floor(128*color_prob);
            if( prob_intensity > 255 ) prob_intensity = 255;
            changes_scaled(i,j) = prob_intensity;
            changes_scaled_full(i,j) = color_prob;
          }
          else
            changes_scaled(i,j) = (int)floor(255*pred_color);

          if( inspect_map(i,j) == 1 )
            viewer.add_inspection_ray( voxels_on_pixel_ray, ray, false );

          for( unsigned v = 0; v < ray.voxels.size(); v++ )
            delete ray.voxels[v];
        }
      }

      // Save all supervoxels.
      subimg_log.print_msg( "saving supervoxels" );
      for( unsigned k = 0; k < supervoxels_.size(); k++ )
        supervoxels_[k].save();

    } // subimg_y
  } //subimg_x


  // Post processing code:-------------------------------------------------

  // NEED TO UNSCALE OUTPUT IMAGES
  
  vcl_string inspect_file = params_->output_dir + '/' + namebase + "_inspect.x3d";
  if( inspect_pixels.size() != 0 )
    viewer.save_inspection_file( inspect_file.c_str() );
  
  if( mode == 0 || mode == 1 ){

    // Save the probability map.
    unsigned light_bin = 0;
    if( params_->appearance_model == 1 ){
      baml_multi_mog_local am( params_->num_mixtures, light );
      light_bin = (unsigned)am.get_light_bin( light );
    }
    vcl_stringstream lb; lb << light_bin;
    //vcl_string prob_map_file = params_->output_dir + '/' + namebase + "_prob_map" + lb.str() + ".png";
    //vil_save( changes_scaled, prob_map_file.c_str() );
    vcl_string prob_map_file = params_->output_dir + '/' + namebase + "_prob_map" + lb.str() + ".tiff";
    vil_save( changes_scaled_full, prob_map_file.c_str() );

    // Mark changes as red.
    double red_thresh = .50; // 
    vil_image_view<vxl_byte> red_img( img.ni(), img.nj(), 3 );
    for( unsigned i = 0; i < red_img.ni(); i++ ){
      for( unsigned j = 0; j < red_img.nj(); j++ ){
        if( img.nplanes() == 1 )
          red_img(i,j,0) = red_img(i,j,1) = red_img(i,j,2) = img(i,j,0);
        else{
          red_img(i,j,0) = img(i,j,0); red_img(i,j,1) = img(i,j,1); red_img(i,j,2) = img(i,j,2);
        }
        vgl_point_2d<int> prob_img_pixel( 
          (int)floor( params_->image_scale*i ), (int)floor( params_->image_scale*j ) );
        if( prob_img_pixel.x() < 0 || prob_img_pixel.y() < 0 ||
          prob_img_pixel.x() >= (int)changes_scaled.ni() || prob_img_pixel.y() >= (int)changes_scaled.nj() ) continue;
        float this_prob = 1.0;
        if( changes_scaled(prob_img_pixel.x(), prob_img_pixel.y()) < 255*red_thresh ) 
          this_prob = (float)(changes_scaled( prob_img_pixel.x(), prob_img_pixel.y() )/(255*red_thresh));
        if( !(this_prob >= 0.0) || !(this_prob <= 1.0) ) this_prob = 0.0;
        red_img(i,j,0) = 
          (int)floor( red_img(i,j,0)*this_prob + 255*(1-this_prob) );
        red_img(i,j,1) = (int)floor( red_img(i,j,1)*this_prob );
        red_img(i,j,2) = (int)floor( red_img(i,j,2)*this_prob );
      }
    }
    vcl_string change_file = params_->output_dir + '/' + namebase + "_change.png";
    vil_save( red_img, change_file.c_str() );
  }

  else if( mode == 2 || mode == 4 ){
    vcl_string render_file = params_->output_dir + '/' + namebase + "_render.png";
    vil_save( changes_scaled, render_file.c_str() );
  }

  else if( mode == 5 ){
    vcl_string prob_img_file = params_->output_dir + '/' + namebase + "_prob_img.png";
    prob_img.write( prob_img_file );
  }

  else if( mode == 6 ){
    float this_a, this_b;
    prob_img.normalize_img( image_scaled, this_a, this_b );
    last_norm_img_.set_size( img.ni(), img.nj(), img.nplanes() );
  
    // Normalize the image.
    vcl_cerr << "\na: " << this_a << "  b: " << this_b << '\n';
    for( unsigned k = 0; k < img.nplanes(); k++ ){
      for( unsigned i = 0; i < img.ni(); i++ ){
        for( unsigned j = 0; j < img.nj(); j++ ){
          int p = (int)floor(this_a*img(i,j,k) + this_b);
          if( !(p >= 0) ) last_norm_img_(i,j,k) = 0;
          else if( p > 255 ) last_norm_img_(i,j,k) = 255;
          else last_norm_img_(i,j,k) = p;
        }
      }
    }
    vcl_string norm_img_file = params_->output_dir + '/' + namebase + "_norm_img.png";
    vil_save( last_norm_img_, norm_img_file.c_str() );
  }

};


//--------------------------------------------------
void
bvaml_world::process_job()
{
  bvaml_log_writer log( "bvaml_world::process_job" );

  // Process the images.
  for( unsigned i = 0; i < params_->images.size(); i++ ){
    log.print_msg( "training on image " + params_->images[i] );
    vnl_vector<float> this_light; 
    if( params_->appearance_model != 0 ) this_light = params_->lights[i];
    vcl_string namebase = vul_file::strip_extension(vul_file::basename( params_->images[i] ));
    vil_image_view<vxl_byte> this_img(1280,720);
    if( params_->process_modes[i] != 4 ) this_img = vil_load(params_->images[i].c_str());
    vcl_string this_image_name = params_->images[i];
    
    // Normalize if needed.
    if( params_->normalize_intensities && params_->process_modes[i] != 6 ){
      process( 6, namebase, this_img, params_->cameras[i], 
        this_light, params_->inspect_pixels[i] );
      process( params_->process_modes[i], namebase, last_norm_img_, params_->cameras[i], 
        this_light, params_->inspect_pixels[i] );
    }
    else
      process( params_->process_modes[i], namebase, this_img, params_->cameras[i], 
        this_light, params_->inspect_pixels[i] );


    // Refine the world if needed.
    //if( params_->process_modes[i] == 0 ) refine_surface();
  }

  // Prediction jobs.
  for( unsigned i = 0; i < params_->prediction_jobs.size(); i++ ){
    predict_appearance( params_->prediction_jobs[i](0), params_->prediction_jobs[i](1), 
      params_->prediction_jobs[i](2) );
  }

  // Set ground plane if needed.
  if( params_->ground_plane_file != "NONE" )
    set_ground_plane( params_->ground_plane_file );

  // Write world if needed.
  if( params_->write_x3d_world.size() != 0 ){
    vcl_string world_file = params_->output_dir + "/world.x3d";
    //bvaml_world_viewer viewer( params_ );
    //viewer.write_x3d_world( this, world_file, 
      //params_->write_x3d_world[0], params_->write_x3d_world[1] );
    refine_surface();
  }
  if( params_->write_raw_world ){
    vcl_string world_file = params_->output_dir + "/world.raw";
    bvaml_world_viewer viewer( params_ );
    viewer.write_raw_world( this, world_file );
  }
};


//--------------------------------------------------
void
bvaml_world::refine_surface()
{
  bvaml_log_writer log( "bvaml_world::refine_surface" );
  log.print_msg( "refining surface" );

  int fit_rad = 2;
  vil_image_view<vxl_byte> hmap( params_->num_voxels().x(), params_->num_voxels().y() );

  // Get heights in misc0 and copy into misc1.
  occupancy_to_heights(0);
  for( int x = 0; x < params_->num_voxels().x(); x++ ){
    if( (x % params_->supervoxel_length) == 0 )
      for( unsigned sv = 0; sv < supervoxels_.size(); sv++ )
        supervoxels_[sv].save();
    for( int y = 0; y < params_->num_voxels().y(); y++ ){
      for( int z = 0; z < params_->num_voxels().z(); z++ ){
        bvaml_voxel v( params_ );
        get_voxel( vgl_point_3d<int>(x,y,z), v, false, true, false, NULL );
        v.misc_storage[1] = v.misc_storage[0];
      }
    }
  }
  for( unsigned sv = 0; sv < supervoxels_.size(); sv++ )
    supervoxels_[sv].save();

  for( int x = 1; x < params_->num_voxels().x()-1; x++ ){
    //vcl_cerr << x << ' ';
    if( (x % params_->supervoxel_length) == 1 )
      for( unsigned sv = 0; sv < supervoxels_.size(); sv++ )
        supervoxels_[sv].save();
    for( int y = 1; y < params_->num_voxels().y()-1; y++ ){

      // Compute the neighborhood cost.
      vnl_vector<float> ph( params_->num_voxels().z(), 1.0 );
      for( int d = 0; d < 4; d++ ){
        int dx, dy;
        if( d == 0 ){ dx = 0; dy = 1; }
        if( d == 1 ){ dx = 0; dy = -1; }
        if( d == 2 ){ dx = 1; dy = 0; }
        if( d == 3 ){ dx = -1; dy = 0; }

        vnl_vector<float> sum_pv( params_->num_voxels().z() );
        for( int z = params_->num_voxels().z()-1; z >= 0; z-- ){
          bvaml_voxel v( params_ );
          get_voxel( vgl_point_3d<int>(x+dx,y+dy,z), v, false, true, false, NULL );
          if( z == (params_->num_voxels().z()-1) ) sum_pv(z) = v.misc_storage[0];
          else sum_pv(z) = sum_pv(z+1) + v.misc_storage[0];
        }

        for( int z = params_->num_voxels().z()-1; z >= 0; z-- ){
          int zmin = z-fit_rad; if( zmin < 0 ) zmin = 0;
          int zmax = z+fit_rad; if( zmax >= params_->num_voxels().z() ) zmax = (params_->num_voxels().z()-1);
          float psum = sum_pv(zmin) - sum_pv(zmax);
          ph(z) *= (float)(.8 + .2*psum );
        }
      }

      // Make the heights sum to one.
      float ph_sum = 0.0;
      for( int z = 0; z < params_->num_voxels().z(); z++ ){
        bvaml_voxel v( params_ );
        get_voxel( vgl_point_3d<int>(x,y,z), v, false, true, false, NULL );
        ph(z) *= v.misc_storage[0];
        ph_sum += ph(z);
      }
      float best_height = 0.0;
      for( int z = 0; z < params_->num_voxels().z(); z++ ){
        bvaml_voxel v( params_ );
        get_voxel( vgl_point_3d<int>(x,y,z), v, false, true, false, NULL );
        v.misc_storage[1] = ph(z)/ph_sum;
        if( ph(z) > best_height ){ best_height = ph(z); hmap(x,y) = 2*z; }
      }

    }
  }
  for( unsigned sv = 0; sv < supervoxels_.size(); sv++ )
    supervoxels_[sv].save();

  heights_to_occupancy(1);
  vil_save( hmap, "D:/results/hmap.png" );
};


//--------------------------------------------------
void
bvaml_world::refine_surface2()
{
  int fit_rad = 2;

  vnl_vector<float> light(3);
  light(0) = -0.307174; light(1) = 0.34471; light(2) = 0.887028;
  vnl_matrix<int> h( params_->num_voxels().x(), params_->num_voxels().y(), 0 );
  vnl_matrix<float> hc( params_->num_voxels().x(), params_->num_voxels().y(), 0.0 );
  vil_image_view<vxl_byte> pvimg( params_->num_voxels().x(), params_->num_voxels().y(), 1 );

  for( int x = 1; x < params_->num_voxels().x()-1; x++ ){
    vcl_cerr << x << ' ';
    if( (x % params_->supervoxel_length) == 1 )
      for( unsigned sv = 0; sv < supervoxels_.size(); sv++ )
        supervoxels_[sv].clear();
    for( int y = 1; y < params_->num_voxels().y()-1; y++ ){

      vnl_vector<float> pv( params_->num_voxels().z() );
      float pv_max = 0.0;
      int pv_max_index = 0;
      float hard_mult = 1.0;
      for( int z = params_->num_voxels().z()-1; z >= 0; z-- ){
        bvaml_voxel v( params_ );
        get_voxel( vgl_point_3d<int>(x,y,z), v, true, false, false, NULL );
        pv(z) = hard_mult*v.occupancy_prob[0];
        hard_mult *= (1-v.occupancy_prob[0]);
        if( pv(z) > pv_max ){ pv_max = pv(z); pv_max_index = z; }
      }
      pvimg(x,y) = 2*pv_max_index;

      vnl_vector<float> ph( params_->num_voxels().z(), 0.0 );
      for( int d = 0; d < 4; d++ ){
        int dx, dy;
        if( d == 0 ){ dx = 0; dy = 1; }
        if( d == 1 ){ dx = 0; dy = -1; }
        if( d == 2 ){ dx = 1; dy = 0; }
        if( d == 3 ){ dx = -1; dy = 0; }

        vnl_vector<float> pv2( params_->num_voxels().z() );
        vnl_vector<float> sum_pv2( params_->num_voxels().z() );
        hard_mult = 1.0;
        for( int z = params_->num_voxels().z()-1; z >= 0; z-- ){
          bvaml_voxel v( params_ );
          get_voxel( vgl_point_3d<int>(x+dx,y+dy,z), v, true, false, false, NULL );
          pv2(z) = hard_mult*v.occupancy_prob[0];
          hard_mult *= (1-v.occupancy_prob[0]);
          if( z == (params_->num_voxels().z()-1) ) sum_pv2(z) = pv2(z);
          else sum_pv2(z) = sum_pv2(z+1) + pv2(z);
        }

        for( int z = params_->num_voxels().z()-1; z >= 0; z-- ){
          int zmin = z-fit_rad; if( zmin < 0 ) zmin = 0;
          int zmax = z+fit_rad; if( zmax >= params_->num_voxels().z() ) zmax = (params_->num_voxels().z()-1);
          float psum = sum_pv2(zmin) - sum_pv2(zmax);
          ph(z) += (.1 + .5*psum );
        }
      }

      float best_height_prob = 0.0;
      for( int z = params_->num_voxels().z()-1; z >= 0; z-- )
        if( ph(z) > best_height_prob ){
          h(x,y) = z;
          best_height_prob = ph(z);
        }

    }
  }

  for( int x = 0; x < params_->num_voxels().x(); x++ ){
    if( (x % params_->supervoxel_length) == 0 )
      for( unsigned sv = 0; sv < supervoxels_.size(); sv++ )
        supervoxels_[sv].clear();
    for( int y = 0; y < params_->num_voxels().y(); y++ ){
      bvaml_voxel v( params_ );
      get_voxel( vgl_point_3d<int>(x,y,h(x,y)), v, true, false, true, &light );
      hc(x,y) = v.appearance->expected_color(light);
    }
  }

  vil_image_view<vxl_byte> himg( params_->num_voxels().x(), params_->num_voxels().y(), 1 );
  for( int x = 0; x < params_->num_voxels().x(); x++ )
    for( int y = 0; y < params_->num_voxels().y(); y++ )
      himg(x,y) = h(x,y)*2;
  vil_save( himg, "D:/results/refined_heights.png" );
  vil_save( pvimg, "D:/results/pv_heights.png" );


  vcl_ofstream ofs( "D:/results/refined_surface.x3d" );
  ofs <<
    "<X3D version='3.0' profile='Immersive'>\n" <<
    " <Scene>\n" <<
    " <Background skyColor='0 0 1' />\n";
/*
  for( int x = 0; x < params_->num_voxels().x(); x++ ){
    if( (x % params_->supervoxel_length) == 0 )
      for( unsigned sv = 0; sv < supervoxels_.size(); sv++ )
        supervoxels_[sv].clear();
    for( int y = 0; y < params_->num_voxels().y(); y++ ){

      float best_occ = 0.0;
      for( int z = 0; z < params_->num_voxels().z(); z++ ){
        bvaml_voxel v( params_ );
        get_voxel( vgl_point_3d<int>(x,y,z), v, true, false, true, &light );
        if( v.occupancy_prob[0] > best_occ ){
          best_occ = v.occupancy_prob[0];
          h(x,y) = z;
          hc(x,y) = v.appearance->expected_color(light);
        }
      }

      ofs << " <Transform translation='" << x << ' ' << y << ' ' << h(x,y) << "'>\n" <<
        "  <Shape>\n" <<
        "   <Appearance>\n" << 
        "    <Material diffuseColor='" << hc(x,y) << ' ' << hc(x,y) << ' ' << hc(x,y) << "'/>\n" <<
        "   </Appearance>\n" <<
        "   <Box size='1 1 1'/>\n" <<
        "  </Shape>\n" <<
        " </Transform>\n";
    }
  }*/

  ofs << 
    "  <Shape>\n" <<
    "   <IndexedFaceSet colorPerVertex='false' coordIndex=' ";

  int c = 0;
  for( int x = 0; x < params_->num_voxels().x(); x++ )
    for( int y = 0; y < params_->num_voxels().y(); y++ ){
      ofs << c << ' ' << c+1 << ' ' << c+2 << ' ' << c+3 << " -1 ";
      c+=4;
    }
  
  ofs << "' >\n" <<
    "    <Color color=' ";
  for( int x = 0; x < params_->num_voxels().x(); x++ )
    for( int y = 0; y < params_->num_voxels().y(); y++ )
      ofs << hc(x,y) << ' ' << hc(x,y) << ' ' << hc(x,y) << ", "; 

  ofs << "' />\n" <<
    "    <Coordinate point=' ";
  for( int x = 0; x < params_->num_voxels().x(); x++ ){
    for( int y = 0; y < params_->num_voxels().y(); y++ ){
      ofs << x << ' ' << y << ' ' << h(x,y) << ", " <<
        x+1 << ' ' << y << ' ' << h(x,y) << ", " <<
        x+1 << ' ' << y+1 << ' ' << h(x,y) << ", " <<
        x << ' ' << y+1 << ' ' << h(x,y) << ", ";
    }
  }
  ofs << "' />\n" <<
    "   </IndexedFaceSet>\n" <<
    "  </Shape>\n";

  ofs <<
    " </Scene>\n" <<
    "</X3D>";
};


//-----------------------------------------------------------
void 
bvaml_world::predict_appearance(
  int a1, int a2, int a3 )
{
  bvaml_log_writer log( "bvaml_world::predict_appearance" );


  vnl_vector<float> a1light(3); a1light(0) = a1light(1) = a1light(2) = (float)a1;
  vnl_vector<float> a2light(3); a2light(0) = a2light(1) = a2light(2) = (float)a2;
  vnl_vector<float> a3light(3); a3light(0) = a3light(1) = a3light(2) = (float)a3;

  for( unsigned sv = 0; sv < supervoxels_.size(); sv++ ){
    vcl_stringstream ss; ss << "predicting supervoxel " << sv;
    log.print_msg( ss.str() );
    float* a1_expval = new float[params_->supervoxel_length*params_->supervoxel_length*params_->supervoxel_length];
    for( int i = 0; i < params_->supervoxel_length; i++ ){
      for( int j = 0; j < params_->supervoxel_length; j++ ){
        for( int k = 0; k < params_->supervoxel_length; k++ ){
          bvaml_voxel v( params_ );
          supervoxels_[sv].get_voxel( vgl_point_3d<int>(i,j,k), v, false, false, true, &a1light );
          a1_expval[k+j*params_->supervoxel_length+i*params_->supervoxel_length*params_->supervoxel_length] =
            v.appearance->expected_color( a1light );
        }
      }
    }
    supervoxels_[sv].clear();

    float* a2_expval = new float[params_->supervoxel_length*params_->supervoxel_length*params_->supervoxel_length];
    for( int i = 0; i < params_->supervoxel_length; i++ ){
      for( int j = 0; j < params_->supervoxel_length; j++ ){
        for( int k = 0; k < params_->supervoxel_length; k++ ){
          bvaml_voxel v( params_ );
          supervoxels_[sv].get_voxel( vgl_point_3d<int>(i,j,k), v, false, false, true, &a2light );
          a2_expval[k+j*params_->supervoxel_length+i*params_->supervoxel_length*params_->supervoxel_length] =
            v.appearance->expected_color( a1light );
        }
      }
    }
    supervoxels_[sv].clear();

    for( int i = 0; i < params_->supervoxel_length; i++ ){
      for( int j = 0; j < params_->supervoxel_length; j++ ){
        for( int k = 0; k < params_->supervoxel_length; k++ ){
          bvaml_voxel v( params_ );
          supervoxels_[sv].get_voxel( vgl_point_3d<int>(i,j,k), v, false, false, true, &a3light );
          float a1val = a1_expval[k+j*params_->supervoxel_length+i*params_->supervoxel_length*params_->supervoxel_length];
          float a2val = a2_expval[k+j*params_->supervoxel_length+i*params_->supervoxel_length*params_->supervoxel_length];
          v.appearance->update( (float)0.5*(a1val+a2val), (float).6, a3light );
          v.appearance->update( a1val, (float).2, a3light );
          v.appearance->update( a2val, (float).2, a3light );
        }
      }
    }
    supervoxels_[sv].save();

  }
};


//--------------------------------------------------------
void 
bvaml_world::occupancy_to_heights( unsigned m )
{
  for( int x = 0; x < params_->num_voxels().x(); x++ ){
    if( (x % params_->supervoxel_length) == 0 )
      for( unsigned sv = 0; sv < supervoxels_.size(); sv++ )
        supervoxels_[sv].save();
    for( int y = 0; y < params_->num_voxels().y(); y++ ){

      float hard_mult = 1.0;
      for( int z = params_->num_voxels().z()-1; z >= 0; z-- ){
        bvaml_voxel v( params_ );
        get_voxel( vgl_point_3d<int>(x,y,z), v, true, true, false, NULL );
        v.misc_storage[m] = hard_mult*v.occupancy_prob[0];
        hard_mult *= (1-v.occupancy_prob[0]);
      }
    }
  }
  for( unsigned sv = 0; sv < supervoxels_.size(); sv++ )
    supervoxels_[sv].save();
};


//----------------------------------------------------------
void 
bvaml_world::heights_to_occupancy( unsigned m )
{
  float height_thresh = (float)0.05;

  for( int x = 0; x < params_->num_voxels().x(); x++ ){
    if( (x % params_->supervoxel_length) == 0 )
      for( unsigned sv = 0; sv < supervoxels_.size(); sv++ )
        supervoxels_[sv].save();
    for( int y = 0; y < params_->num_voxels().y(); y++ ){

      float height_sum = 1.0;
      for( int z = params_->num_voxels().z()-1; z >= 0; z-- ){
        if( height_sum < height_thresh ) continue;
        bvaml_voxel v( params_ );
        get_voxel( vgl_point_3d<int>(x,y,z), v, true, true, false, NULL );
        v.occupancy_prob[0] = v.misc_storage[m]/height_sum;
        height_sum -= v.misc_storage[m];
      }
    }
  }
  for( unsigned sv = 0; sv < supervoxels_.size(); sv++ )
    supervoxels_[sv].save();
};


//--------------------------------------------------
void 
bvaml_world::set_ground_plane(
  vcl_string ground_file )
{
  bvaml_log_writer log( "bvaml_world::set_ground_plane" );

  // Check that the file is good.
  log.print_msg( "parsing file " + ground_file );
  vcl_ifstream file_stream( ground_file.c_str() );
  if( !(file_stream.good()) ){
    log.print_error( "can't read file" );
    return;
  }

  // Read the file.
  vul_awk awk( file_stream );
  vcl_vector< vcl_vector< vgl_point_3d<float> > > planes;
  vcl_vector< vgl_point_3d<float> > new_plane;
  while( awk ){
    if( awk.NF() == 0 ){
      if( new_plane.size() > 3 )
        planes.push_back( new_plane );
      else if( new_plane.size() == 1 || new_plane.size() == 2 )
        log.print_error( "too few points in ground plane" );
      new_plane.clear();
    }
    else if( awk.NF() == 3 ){
      new_plane.push_back( vgl_point_3d<float>( 
        (float)atof(awk[0]), (float)atof(awk[1]), (float)atof(awk[2]) ) );
    }
    else
      log.print_error( "bad line in ground file" );
    ++awk;
  }

  // Update the world model for each plane.
  log.print_msg( "updating ground plane" );
  for( unsigned plane = 0; plane < planes.size(); plane++ ){

    vcl_vector< vgl_point_2d<float> > this_plane_2d;
    for( unsigned p = 0; p < planes[plane].size(); p++ )
      this_plane_2d.push_back( vgl_point_2d<float>( 
        planes[plane][p].x(), planes[plane][p].y() ) );
    vgl_polygon<float> this_poly( this_plane_2d );
    vgl_plane_3d<float> this_plane( planes[plane][0],
      planes[plane][1], planes[plane][2] );

    // Get bounds on the plane area.
    int min_vx = params_->num_voxels().x()-1;
    int min_vy = params_->num_voxels().y()-1;
    int max_vx = 0, max_vy = 0;
    for( unsigned p = 0; p < this_plane_2d.size(); p++ ){
      vgl_point_3d<int> this_voxel_index;
      get_voxel_index( planes[plane][p], this_voxel_index );
      if( this_voxel_index.x() < min_vx ) min_vx = this_voxel_index.x();
      if( this_voxel_index.x() > max_vx ) max_vx = this_voxel_index.x();
      if( this_voxel_index.y() < min_vy ) min_vy = this_voxel_index.y();
      if( this_voxel_index.y() > max_vy ) max_vy = this_voxel_index.y();
    }

    // Set all voxels in this bounded region.
    for( int vx = min_vx; vx <= max_vx; vx++ ){
      for( int vy = min_vy; vy <= max_vy; vy++ ){

        // If the voxels on this z-column don't lie in the polygon, ignore.
        vgl_point_3d<float> bot_world_point, top_world_point;
        get_world_coord( vgl_point_3d<int>(vx,vy,0), bot_world_point );
        get_world_coord( vgl_point_3d<int>(vx,vy,params_->num_voxels().z()), top_world_point );
        if( !this_poly.contains( vgl_point_2d<float>(
          bot_world_point.x(), bot_world_point.y() ) ) ) continue;

        // If they do set all probabilities to 0 except for the voxel that lies 
        // in the plane.
        vgl_point_3d<float> plane_point = vgl_intersection(
          vgl_line_3d_2_points<float>( bot_world_point, top_world_point ),
          this_plane );
        vgl_point_3d<int> plane_voxel;
        get_voxel_index( plane_point, plane_voxel );
        for( int vz = 0; vz < params_->num_voxels().z(); vz++ ){
          bvaml_voxel this_voxel( params_ );
          get_voxel( vgl_point_3d<int>(vx,vy,vz), this_voxel, true, false, false, NULL );
          if( vz == plane_voxel.z() ) this_voxel.occupancy_prob[0] = 1.0;
          else this_voxel.occupancy_prob[0] = 0.0;
        }
      }
    }
    // Save all changed supervoxels.
    for( unsigned k = 0; k < supervoxels_.size(); k++ )
      supervoxels_[k].save();
  }

};


//--------------------------------------------------
bool 
bvaml_world::get_voxel_location(
  const vgl_point_3d<int>& voxel_index,
  vgl_point_3d<int>& supervoxel,
  vgl_point_3d<int>& voxel_in_supervoxel )
{
  supervoxel.set(
    (int)floor( voxel_index.x()/(double)params_->supervoxel_length ),
    (int)floor( voxel_index.y()/(double)params_->supervoxel_length ),
    (int)floor( voxel_index.z()/(double)params_->supervoxel_length ) );
  voxel_in_supervoxel.set(
    voxel_index.x() - supervoxel.x()*params_->supervoxel_length,
    voxel_index.y() - supervoxel.y()*params_->supervoxel_length,
    voxel_index.z() - supervoxel.z()*params_->supervoxel_length );
  if( supervoxel.x() < 0 || supervoxel.x() >= params_->num_supervoxels.x() ||
    supervoxel.y() < 0 || supervoxel.y() >= params_->num_supervoxels.y() ||
    supervoxel.z() < 0 || supervoxel.z() >= params_->num_supervoxels.z() )
      return false;

  return true;
};


//-------------------------------------------------
void
bvaml_world::get_voxel(
  const vgl_point_3d<int>& voxel_index,
  bvaml_voxel& v,
  bool load_occupancy,
  bool load_misc,
  bool load_appearance,
  const vnl_vector<float>* local_light )
{
  bvaml_log_writer log( "bvaml_world::get_voxel" );
  vgl_point_3d<int> supervoxel, voxel_in_supervoxel;
  if( !get_voxel_location( voxel_index, supervoxel, voxel_in_supervoxel ) ){
    log.print_error( "voxel index out of bounds" );
    return;
  }
  supervoxels_[ supervoxel_index( supervoxel ) ].get_voxel( voxel_in_supervoxel, v,
    load_occupancy, load_misc, load_appearance, local_light );
};


//------------------------------------------------
void
bvaml_world::get_voxel_index(
  const vgl_point_3d<float>& p,
  vgl_point_3d<int>& voxel_index )
{
  voxel_index.set(
    (int)floor( ( p.x() - params_->corner.x() )/params_->voxel_length ),
    (int)floor( ( p.y() - params_->corner.y() )/params_->voxel_length ),
    (int)floor( ( p.z() - params_->corner.z() )/params_->voxel_length ) );
};


//------------------------------------------------
void 
bvaml_world::get_world_coord(
  const vgl_point_3d<int>& voxel_index,
  vgl_point_3d<float>& p )
{
  p.set(
    (float)( params_->corner.x() + voxel_index.x()*params_->voxel_length ),
    (float)( params_->corner.y() + voxel_index.y()*params_->voxel_length ),
    (float)( params_->corner.z() + voxel_index.z()*params_->voxel_length ) );
};


#endif // _bvaml_world_cxx_
