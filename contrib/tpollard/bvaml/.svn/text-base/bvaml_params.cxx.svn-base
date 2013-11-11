#ifndef _bvaml_params_cxx_
#define _bvaml_params_cxx_

#include "bvaml_params.h"
#include "bvaml_log_writer.h"

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_sstream.h>
#include <vnl/vnl_vector_fixed.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>


//-------------------------------------
bool bvaml_params::read_params( 
  vcl_string param_file )
{
  bvaml_log_writer log("bvaml_params::read_params");

  // Check that the file is good.
  log.print_msg( "parsing file " + param_file );
  vcl_ifstream file_stream( param_file.c_str() );
  if( !(file_stream.good()) ){
    log.print_error( "can't read file" );
    return false;
  }
  vul_awk awk( file_stream );
  vcl_stringstream first_line;
  for( int i = 0; i < awk.NF(); i++ )
    first_line << awk[i] << ' ';
  if( first_line.str() != "bvaml parameter file " ){
    log.print_error( "invalid file type" );
    return false;
  }

  // Parse the file a line at a time.
  int line_counter = 1;
  while( awk ){
    ++awk;
    line_counter++;
    if( awk.NF() == 0 ) continue;
    vcl_stringstream this_field;
    this_field << awk[0];
    if( this_field.str()[0] == '#' ) continue;

    if( this_field.str() == "model_dir" ){
      if( awk.NF() != 2 )
        log.print_error( "error on line " + line_counter );
      else
        model_dir = awk[1];
    }

    else if( this_field.str() == "output_dir" ){
      if( awk.NF() != 2 )
        log.print_error( "error on line " + line_counter );
      else
        output_dir = awk[1];
    }

    else if( this_field.str() == "training_images" || 
      this_field.str() == "detection_images" ||
      this_field.str() == "render_images" ||
      this_field.str() == "save_normalized_images" ){
      if( awk.NF() != 4 && awk.NF() != 5 ){
        log.print_error( "error on line " + line_counter );
        continue;
      }
      int this_mode = 0;
      if( this_field.str() == "detection_images" ) this_mode = 1;
      if( this_field.str() == "render_images" ) this_mode = 2;
      if( this_field.str() == "save_normalized_images" ) this_mode = 6;
      vcl_vector< vcl_string > new_images;
      vcl_vector< vpgl_proj_camera<double> > new_cameras;
      vcl_vector< vnl_vector<float> > new_lights;
      get_images( awk[1], new_images );
      get_cameras( awk[2], new_cameras );
      get_lights( awk[3], new_lights );
      vnl_vector_fixed<int,3> subset_range( 0, 1, new_images.size()-1 );
      if( awk.NF() == 5 )
        if( !get_range( awk[4], subset_range ) )
          log.print_error( "error in subset range" );

      if( new_images.size() != new_cameras.size() ||
         ( new_lights.size() != 0 && new_lights.size() != new_images.size() ) )
        log.print_error( "different numbers of images/cameras/lights" );

      vcl_vector< unsigned > new_order;
      if( order_by_date ) reorder_by_date( new_images, new_order );

      for( int i = subset_range[0]; i <= subset_range[2]; i+= subset_range[1] ){
        unsigned idx = (unsigned)i; if( order_by_date ) idx = new_order[i];
        images.push_back( new_images[idx] );
        cameras.push_back( new_cameras[idx] );
        if( new_lights.size() != 0 ) lights.push_back( new_lights[idx] );
        process_modes.push_back( this_mode );
        inspect_pixels.push_back( vcl_vector< vgl_point_2d<int> >() );
      }
    }

    else if( this_field.str() == "render_from_new_view" ){
      if( awk.NF() != 4 && awk.NF() != 5 ){
        log.print_error( "error on line " + line_counter );
        continue;
      }
      vcl_vector< vcl_string > new_images;
      vcl_vector< vpgl_proj_camera<double> > new_cameras, new_render_camera;
      get_images( awk[1], new_images );
      get_cameras( awk[2], new_cameras );
      get_cameras( awk[3], new_render_camera );
      vnl_vector_fixed<int,3> subset_range( 0, 1, new_cameras.size()-1 );
      if( awk.NF() == 5 )
        if( !get_range( awk[4], subset_range ) )
          log.print_error( "error in subset range" );

      if( new_images.size() != new_cameras.size() )
        log.print_error( "different numbers of images/cameras/lights" );
      if( new_render_camera.size() != 1 )
        log.print_error( "too many render cameras" );

      for( int i = subset_range[0]; i <= subset_range[2]; i+= subset_range[1] ){
        images.push_back( new_images[i] );
        cameras.push_back( new_cameras[i] );
        lights.push_back( vnl_vector<float>() );
        process_modes.push_back( 3 );
        inspect_pixels.push_back( vcl_vector< vgl_point_2d<int> >() );
        
        images.push_back( new_images[i] );
        cameras.push_back( new_render_camera[0] );
        lights.push_back( vnl_vector<float>() );
        process_modes.push_back( 4 );
        inspect_pixels.push_back( vcl_vector< vgl_point_2d<int> >() );
      }
      
    }

    else if( this_field.str() == "generate_prob_image" ){
      if( awk.NF() != 3 ){
        log.print_error( "error on line " + line_counter );
        continue;
      }
      vcl_vector< vpgl_proj_camera<double> > new_camera;
      get_cameras( awk[2], new_camera );

      images.push_back( awk[1] );
      cameras.push_back( new_camera[0] );
      lights.push_back( vnl_vector<float>() );
      process_modes.push_back( 5 );
      inspect_pixels.push_back( vcl_vector< vgl_point_2d<int> >() );
    }

    else if( this_field.str() == "inspect_frame" ){
      if( awk.NF() < 4 || (awk.NF() % 2) == 1 ){
        log.print_error( "error on line " + line_counter );
        continue;
      }
      int this_frame = atoi(awk[1]);
      for( int i = 2; i < awk.NF(); i+=2 ){
        inspect_pixels[this_frame].push_back( 
          vgl_point_2d<int>( atoi(awk[i]), atoi(awk[i+1]) ) );
      }
    }

    else if( this_field.str() == "inspect_point" ){
      if( awk.NF() < 4 || (awk.NF() % 3) != 1 ){
        log.print_error( "error on line " + line_counter );
        continue;
      }
      for( int i = 1; i < awk.NF(); i+=3 ){
        vgl_point_3d<float> insp_point( (float)atof(awk[i]), (float)atof(awk[i+1]), (float)atof(awk[i+2]) );
        inspect_voxels.push_back( vgl_point_3d<int>(
          (int)floor( (insp_point.x()-corner.x())/voxel_length ),
          (int)floor( (insp_point.y()-corner.y())/voxel_length ),
          (int)floor( (insp_point.z()-corner.z())/voxel_length ) ) );
      }
    }

    else if( this_field.str() == "image_scale" ){
      if( awk.NF() != 2 )
        log.print_error( "error on line " + line_counter );
      else
        image_scale = (float)atof(awk[1]);
    }

    else if( this_field.str() == "normalize_intensities" ){
      if( awk.NF() != 1 )
        log.print_error( "error on line " + line_counter );
      else
        normalize_intensities = true;
    }

    else if( this_field.str() == "order_by_date" ){
      if( awk.NF() != 1 )
        log.print_error( "error on line " + line_counter );
      else
        order_by_date = true;
    }

    else if( this_field.str() == "predict_appearance" ){
      if( awk.NF() != 4 )
        log.print_error( "error on line " + line_counter );
      else{
        vnl_vector<int> new_prediction(3);
        new_prediction(0) = atoi( awk[1] );
        new_prediction(1) = atoi( awk[2] );
        new_prediction(2) = atoi( awk[3] );
        prediction_jobs.push_back( new_prediction );
      }
    }

    else if( this_field.str() == "write_x3d_world" ){
      if( awk.NF() != 3 )
        log.print_error( "error on line " + line_counter );
      else{
        write_x3d_world.clear();
        write_x3d_world.push_back( (float)atof(awk[1]) );
        write_x3d_world.push_back( (float)atof(awk[2]) );
      }
    }

    else if( this_field.str() == "write_raw_world" ){
      if( awk.NF() != 1 )
        log.print_error( "error on line " + line_counter );
      else{
        write_raw_world = true;
      }
    }

    else if( this_field.str() == "ground_plane_file" ){
      if( awk.NF() != 2 )
        log.print_error( "error on line " + line_counter );
      else{
        ground_plane_file = awk[1];
      }
    }

    else if( this_field.str() == "corner" ){
      if( awk.NF() != 4 )
        log.print_error( "error on line " + line_counter );
      else
        corner = vgl_point_3d<double>( atof(awk[1]), atof(awk[2]), atof(awk[3]) );
    }

    else if( this_field.str() == "voxel_length" ){
      if( awk.NF() != 2 )
        log.print_error( "error on line " + line_counter );
      else
        voxel_length = atof(awk[1]);
    }

    else if( this_field.str() == "supervoxel_length" ){
      if( awk.NF() != 2 )
        log.print_error( "error on line " + line_counter );
      else
        supervoxel_length = atoi(awk[1]);
    }

    else if( this_field.str() == "num_supervoxels" ){
      if( awk.NF() != 4 )
        log.print_error( "error on line " + line_counter );
      else
        num_supervoxels = vgl_vector_3d<int>( atoi(awk[1]), atoi(awk[2]), atoi(awk[3]) );
    }

    else if( this_field.str() == "empty_thresh" ){
      if( awk.NF() != 2 )
        log.print_error( "error on line " + line_counter );
      else
        empty_thresh = (float)atof(awk[1]);
    }

    else if( this_field.str() == "min_prob_to_allow_empty_voxels" ){
      if( awk.NF() != 2 )
        log.print_error( "error on line " + line_counter );
      else
        min_prob_to_allow_empty_voxels = (float)atof(awk[1]);
    }

    else if( this_field.str() == "appearance_model" ){
      if( awk.NF() != 2 )
        log.print_error( "error on line " + line_counter );
      else
        appearance_model = atoi(awk[1]);
    }

    else if( this_field.str() == "num_mixtures" ){
      if( awk.NF() != 2 )
        log.print_error( "error on line " + line_counter );
      else
        num_mixtures = atoi(awk[1]);
    }
    else
      log.print_error( "error on line " + line_counter );
  }

  return true;
};

void
bvaml_params::print_summary()
{
  bvaml_log_writer log("bvaml_params::print_summary");

  // Print world params.
  vcl_stringstream world_summary;
  world_summary
    << "corner " << corner.x() << " " << corner.y() << " " << corner.z() << '\n'
    << "voxel_length " << voxel_length << '\n'
    << "supervoxel_length " << supervoxel_length << '\n'
    << "num_supervoxels " << num_supervoxels.x() << ' ' << num_supervoxels.y() 
      << ' ' << num_supervoxels.z() << '\n'
    << "num_voxels " << num_voxels().x() << ' ' << num_voxels().y() 
      << ' ' << num_voxels().z() << '\n'
    << "empty_thresh " << empty_thresh << '\n'
    << "min_prob_to_allow_empty_voxels " << min_prob_to_allow_empty_voxels << '\n'
    << "appearance_model " << appearance_model << '\n'
    << "num_mixtures " << num_mixtures;
  log.print_block( "world summary", world_summary.str() );

  // Print job params.
  vcl_stringstream job_summary;
  job_summary
    << "model_dir " << model_dir << '\n'
    << "output_dir " << output_dir << '\n'
    << "image_scale " << image_scale << '\n'
    << "normalize intensities " << normalize_intensities << '\n'
    << "inspecting voxels " << '\n';
  for( unsigned i = 0; i < inspect_voxels.size(); i++ )
    job_summary << inspect_voxels[i] << '\n';
  for( unsigned i = 0; i < images.size(); i++ ){
    if( process_modes[i] == 0 )
      job_summary << "training on " << images[i] << '\n';
    else if( process_modes[i] == 1 )
      job_summary << "detecting changes on " << images[i] << '\n';
    else if( process_modes[i] == 2 )
      job_summary << "generating novel view\n";
    else if( process_modes[i] == 3 )
      job_summary << "projecting image onto world " << images[i] << '\n';
    else if( process_modes[i] == 4 )
      job_summary << "rendering image from projection\n";
    else if( process_modes[i] == 5 )
      job_summary << "generating prob image\n";
    else
      job_summary << "unknown process\n";
    if( appearance_model != 0 ) job_summary << " with light " << lights[i] << '\n';
    if( inspect_pixels[i].size() > 0 ){
      job_summary << ' ' << " inspecting pixels: ";
      for( unsigned j = 0; j < inspect_pixels[i].size(); j++ )
        job_summary << inspect_pixels[i][j].x() << ", " << inspect_pixels[i][j].y() << "   ";
      job_summary << '\n';
    }
  }
  if( ground_plane_file != "NONE" )
    job_summary << "setting ground planes from: " << ground_plane_file << '\n';
  for( int i = 0; i < prediction_jobs.size(); i++ ){
    job_summary << "predict appearance: " << prediction_jobs[i] << '\n';
  }

  if( write_x3d_world.size() > 0 )
    job_summary << "writing x3d world, threshold = " << write_x3d_world[0]
      << ", scaling = " << write_x3d_world[1] << '\n';
  if( write_raw_world )
    job_summary << "writing .raw world";
  log.print_block( "job summary", job_summary.str() );
};


//------------------------------------------------------
bool
bvaml_params::get_cameras(
  vcl_string camera_file,
  vcl_vector< vpgl_proj_camera<double> >& cameras )
{
  bvaml_log_writer log( "bvaml_params::get_cameras" );
  cameras.clear();
  vcl_ifstream camera_stream( camera_file.c_str() );
  char line_buffer[256];
  if( !(camera_stream.good()) ){
    log.print_error( "bad camera file " + camera_file );
    return false;
  }
  while( camera_stream.eof() == 0 ){
    char nc = camera_stream.peek();
    if( nc == '-' || nc == '0' || nc == '1' || nc == '2' || nc == '3' || nc == '4' || 
        nc == '5' || nc == '6' || nc == '7' || nc == '8' || nc == '9' ){
      vnl_matrix_fixed<double,3,4> new_camera_matrix;
      camera_stream >> new_camera_matrix;
      cameras.push_back( vpgl_proj_camera<double>( new_camera_matrix)  );
    }
    else
      camera_stream.getline(line_buffer,256);
  }
  return true;
};


//----------------------------------------------------
bool 
bvaml_params::get_images(
  vcl_string image_dir,
  vcl_vector< vcl_string >& images )
{
  bvaml_log_writer log( "bvaml_params::get_images" );
  images.clear();

  image_dir += "/*.*";
  for( vul_file_iterator fit = image_dir; fit; ++fit ){
    if( vul_file::is_directory(fit()) )
      continue;
    vcl_string image_name = fit();
    if( image_name.find( ".jpg" ) > 1000 &&
        image_name.find( ".png" ) > 1000 &&
        image_name.find( ".tif" ) > 1000 &&
        image_name.find( ".JPG" ) > 1000 ){
      log.print_error( "can't read image " + image_name );
      continue;
    }
    images.push_back( image_name );
  }
  return true;
};


//----------------------------------------------------
bool 
bvaml_params::get_lights(
  vcl_string light_file,
  vcl_vector< vnl_vector<float> >& lights )
{
  bvaml_log_writer log( "bvaml_params::get_lights" );
  if( light_file == "NONE" ) return true;
  lights.clear();

  vcl_ifstream light_stream( light_file.c_str() );
  //vul_awk lawk( light_file.c_str() );
  //while( lawk ){
  for( vul_awk lawk( light_stream ); lawk; ++lawk ){
    if( lawk.NF() == 0 ) continue; //{ ++lawk; continue; }
    vnl_vector<float> new_light( lawk.NF() );
    for( int i = 0; i < lawk.NF(); i++ ){
      new_light(i) = (float)atof( lawk[i] );
    }
    lights.push_back( new_light );
    //++lawk
  }
/*
  vcl_ifstream light_stream( light_file.c_str() );
  if( !(light_stream.good()) ){
    log.print_error( "bad light file " + light_file );
    return false;
  }
  char line_buffer[256];
  while( light_stream.eof() == 0 ){
    char nc = light_stream.peek();
    if( nc == '-' || nc == '0' || nc == '1' || nc == '2' || nc == '3' || nc == '4' || 
        nc == '5' || nc == '6' || nc == '7' || nc == '8' || nc == '9' ){
      vnl_vector<float> new_light;
      light_stream >> new_light;
      lights.push_back( new_light  );
    }
    else
      light_stream.getline(line_buffer,256);
  } */
 return true;
};


//-----------------------------------------------------
bool 
bvaml_params::get_range(
  vcl_string range_str,
  vnl_vector_fixed<int,3>& range_indices )
{
  int first_colon = -1, second_colon = -1;
  for( unsigned c = 0; c < range_str.size(); c++ ){
    if( range_str[c] == ':' ){
      if( first_colon == -1 )
        first_colon = c;
      else if( second_colon == -1 )
        second_colon = c;
      else return false;
    }
  }
  if( first_colon == -1 || second_colon == -1 ) return false;

  vcl_string start_index_str, index_inc_str, end_index_str;
  for( int c = 0; c < first_colon; c++ )
    start_index_str += range_str[c];
  for( int c = first_colon+1; c < second_colon; c++ )
    index_inc_str += range_str[c];
  for( unsigned c = second_colon+1; c < range_str.size(); c++ )
    end_index_str += range_str[c];
  range_indices[0] = atoi( start_index_str.c_str() )-1 ;
  range_indices[1] = atoi( index_inc_str.c_str() );
  range_indices[2] = atoi( end_index_str.c_str() )-1 ;
  return true;
};


//----------------------------------------------------
void bvaml_params::reorder_by_date(
  const vcl_vector< vcl_string >& file_list,
  vcl_vector< unsigned >& new_order )
{
  new_order.resize( file_list.size() );
  vcl_vector<double> dates;
  for( unsigned i = 0; i < file_list.size(); i++ ){
    vcl_string filename = vul_file::strip_directory( file_list[i] );
    vcl_string year; year += filename[0]; year += filename[1];
    vcl_string month; month += filename[2]; month += filename[3]; month += filename[4]; 
    vcl_string day; day += filename[5]; day += filename[6];
    double date = atoi(year.c_str())/100.0;
    if( month == "JAN" ) date += .0001;
    else if( month == "FEB" ) date += .0002;
    else if( month == "MAR" ) date += .0003;
    else if( month == "APR" ) date += .0004;
    else if( month == "MAY" ) date += .0005;
    else if( month == "JUN" ) date += .0006;
    else if( month == "JUL" ) date += .0007;
    else if( month == "AUG" ) date += .0008;
    else if( month == "SEP" ) date += .0009;
    else if( month == "OCT" ) date += .0010;
    else if( month == "NOV" ) date += .0011;
    else if( month == "DEC" ) date += .0012;
    else vcl_cerr << "ERROR: UNKNOWN DATE: " << month << '\n';
    date += atoi(day.c_str())/1000000.0;
    dates.push_back( date );
  }

  // Brute force n^2 sorting.
  double smallest_date;
  unsigned smallest_index;
  for( unsigned i = 0; i < file_list.size(); i++ ){
    smallest_date = 1.0;
    for( unsigned j = 0; j < file_list.size(); j++ ){
      if( dates[j] < smallest_date ){ smallest_date = dates[j]; smallest_index = j; }
    }
    new_order[i] = smallest_index;
    dates[smallest_index] = 1.0;
  }

};

#endif // _bvaml_params_cxx_
