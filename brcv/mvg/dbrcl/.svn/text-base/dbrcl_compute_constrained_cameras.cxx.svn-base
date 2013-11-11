#ifndef _dbrcl_compute_constrained_cameras_cxx_
#define _dbrcl_compute_constrained_cameras_cxx_

#include "dbrcl_compute_constrained_cameras.h"

#include <vgl/vgl_distance.h>
#include <vcl_cstdlib.h>


//---------------------------------------------
bool 
dbrcl_compute_constrained_cameras::read_constraint_file( 
  vcl_string constraint_file )
{
  // Declare some needed variables.
  char line_buffer[256];
  char number_buffer[16];
  int line_counter = 2;

  // Check that the file is good.
  vcl_cerr << "\ndbrcl_compute_constrained_cameras::read_constraint_file\n"
    << " parsing constraint file\n";
  vcl_ifstream constraints_stream( constraint_file.c_str() );
  if( constraints_stream.bad() ){
    vcl_cerr << "  can't read file: " << constraint_file.c_str() << '\n';
    return false;
  }
  constraints_stream.getline(line_buffer,256);
  vcl_stringstream first_line;
  first_line << line_buffer;
  if( first_line.str() != "dbrcl camera constraint file" ){
    vcl_cerr << "  invalid file: " << constraint_file.c_str() << '\n';
    return false;
  }

  // Parse the file a line at a time.
  dbrcl_camera_constraints* current_constraint = NULL;
  while( constraints_stream.eof() == 0 ){
    constraints_stream.getline(line_buffer,256);
    vcl_stringstream this_line;
    this_line << line_buffer;

    // Make sure this line doesn't begin with a number.
    char c = this_line.str().c_str()[0];
    if( c == '0' || c == '1' || c == '2' || c == '3' || c == '4' || 
        c == '5' || c == '6' || c == '7' || c == '8' || c == '9'  ){
      vcl_cerr << " error in constraint file on line " << line_counter << '\n';
      line_counter++;
    }

    // Case 0: specifies a new frame.
    else if( this_line.str() == "frame" ){
      constraints_stream.getline(number_buffer,16);
      int frame = vcl_atoi( number_buffer );
      constraints_.push_back( dbrcl_camera_constraints() );
      current_constraint = &( constraints_[ constraints_.size() -1 ] );
      current_constraint->frame_number = frame;
      if( verbose )
        vcl_cerr << "\n frame: " << frame << '\n'; 
      line_counter+=2;
    }

    // Case 1: world-image point correspondence.
    else if( this_line.str() == "world image correspondence" ){

      constraints_stream.getline(number_buffer,16,' ');
      double x = atof( number_buffer );
      constraints_stream.getline(number_buffer,16,' ');
      double y = atof( number_buffer );
      constraints_stream.getline(number_buffer,16);
      double z = atof( number_buffer );
      vgl_point_3d<double> world_point(x,y,z);
      current_constraint->world_points.push_back( world_point );

      constraints_stream.getline(number_buffer,16,' ');
      x = atof( number_buffer );
      constraints_stream.getline(number_buffer,16);
      y = atof( number_buffer );
      vgl_point_2d<double> image_point(x,y);
      current_constraint->image_points.push_back( image_point );

      if( verbose ){
        vcl_cerr << " world image correspondence:\n" 
          << "  world point: " << world_point << '\n' 
          << "  image point: " << image_point << '\n';
      }
      line_counter+=3;
    }

    // Case 2: height.
    else if( this_line.str() == "height" ){
    
      constraints_stream.getline(number_buffer,16,' ');
      double x = atof( number_buffer );
      constraints_stream.getline(number_buffer,16);
      double y = atof( number_buffer );
      vgl_point_2d<double> bot_point(x,y);
      current_constraint->height_bot_points.push_back( bot_point );
      constraints_stream.getline(number_buffer,16,' ');
      x = atof( number_buffer );
      constraints_stream.getline(number_buffer,16);
      y = atof( number_buffer );
      vgl_point_2d<double> top_point(x,y);
      current_constraint->height_top_points.push_back( top_point );

      if( verbose ){
        vcl_cerr << " height:\n"
          << "  bottom point: " << bot_point << '\n' 
          << "  top point: " << top_point << '\n';
      }
      line_counter+=3;
    }

    // Case 3: world point.
    else if( this_line.str() == "world point" ){
      constraints_stream.getline(number_buffer,16,' ');
      double x = atof( number_buffer );
      constraints_stream.getline(number_buffer,16,' ');
      double y = atof( number_buffer );
      constraints_stream.getline(number_buffer,16);
      double z = atof( number_buffer );
      vgl_point_3d<double> world_point(x,y,z);
      unmatched_world_points_.push_back( world_point );

      if( verbose ){
        vcl_cerr << " world point:\n" 
          << "  world point: " << world_point << '\n';
      }
      line_counter+=2;
    }

    // Default Case: unknown constraint so skip.
    else{
      line_counter++;
    }

  } // iteration over file lines
  vcl_cerr << "  success\n";
  return true;
}


//---------------------------------------------
bool 
dbrcl_compute_constrained_cameras::compute_cameras(
 vcl_vector< vpgl_proj_camera<double> >& cameras,
  vcl_vector<int>& frames,
  bool affine_camera )
{
  vcl_cerr << "\ndbrcl_compute_constrained_cameras::compute_known_cameras\n"
    << " computing " << constraints_.size() << " cameras.\n";
  frames.clear();
  cameras.clear();
  for( int i = 0; i < constraints_.size(); i++ )
    frames.push_back( constraints_[i].frame_number );

  // Find the dimensions of the solution matrix.
  int num_params = 8;
  if( affine_camera ) num_params = 6;
  int num_constraints = 0;
  for( int i = 0; i < constraints_.size(); i++ ){
    num_params += 2;
    num_constraints += 2*constraints_[i].image_points.size();
    num_constraints += constraints_[i].height_top_points.size();
  }
  vnl_matrix<double> S( num_constraints, num_params, 0 );
  vnl_vector<double> B( num_constraints, 0 );
  vnl_vector<double> V( num_params );

  // Fill in the solution matrix.
  int cstart = 8;
  if( affine_camera ) cstart = 6;
  int k = 0;
  for( int c = 0; c < constraints_.size(); c++ ){
    for( int i = 0; i < constraints_[c].image_points.size(); i++ ){
      vgl_point_2d<double> image_point = constraints_[c].image_points[i];
      vgl_point_3d<double> world_point = constraints_[c].world_points[i];
      S(k,0) = world_point.x();
      S(k,1) = world_point.y();
      S(k,cstart+2*c+0) = world_point.z();
      S(k,2) = 1;
      if( !affine_camera ){
        S(k,6) = -world_point.x()*image_point.x();
        S(k,7) = -world_point.y()*image_point.x();
      }
      B(k) = image_point.x();
      k++;
      S(k,3) = world_point.x();
      S(k,4) = world_point.y();
      S(k,cstart+2*c+1) = world_point.z();
      S(k,5) = 1;
      if( !affine_camera ){
        S(k,6) = -world_point.x()*image_point.y();
        S(k,7) = -world_point.y()*image_point.y();
      }
      B(k) = image_point.y();
      k++;
    }
    for( int i = 0; i < constraints_[c].height_top_points.size(); i++ ){
      vgl_point_2d<double> top = constraints_[c].height_top_points[i];
      vgl_point_2d<double> bot = constraints_[c].height_bot_points[i];
      S(k,cstart+2*c+0) = top.y()-bot.y();
      S(k,cstart+2*c+1) = -( top.x()-bot.x() );
      k++;
    }
  }
  vnl_svd<double> Ssvd(S);
  V = Ssvd.solve( B );
  // Reform the cameras.
  vcl_vector< vnl_matrix_fixed<double,3,4> > init_cameras;
  for( int i = 0; i < constraints_.size(); i++ ){
    vnl_matrix_fixed<double,3,4> P;
    P(0,0) = V(0); P(0,1) = V(1); P(0,3) = V(2);
    P(1,0) = V(3); P(1,1) = V(4); P(1,3) = V(5);
    if( affine_camera ){
      P(2,0) = 0; P(2,1) = 0; P(2,2) = 0; P(2,3) = 1; }
    else{
      P(2,0) = V(6); P(2,1) = V(7); P(2,2) = 0; P(2,3) = 1; }
    P(0,2) = V(cstart+2*i+0); P(1,2) = V(cstart+2*i+1); 
    cameras.push_back( vpgl_proj_camera<double>( P ) );
  }
  vcl_cerr << "  success\n";
  return true;
}


//---------------------------------------------
bool 
dbrcl_compute_constrained_cameras::compute_cameras_separately(
  vcl_vector< vpgl_proj_camera<double> >& cameras,
  vcl_vector<int>& frames,
  bool affine_camera )
{
  vcl_cerr << "\ndbrcl_compute_constrained_cameras::compute_individual_cameras\n"
    << " computing " << constraints_.size() << " cameras.\n";
  frames.clear();
  cameras.clear();
  for( int i = 0; i < constraints_.size(); i++ ){
    frames.push_back( constraints_[i].frame_number );

    // Find the dimensions of the solution matrix.
    int num_params = 11;
    if( affine_camera ) num_params = 8;
    int num_constraints = 2*constraints_[i].image_points.size() +
      constraints_[i].height_top_points.size();
    
    vnl_matrix<double> S( num_constraints, num_params, 0 );
    vnl_vector<double> B( num_constraints, 0 );
    vnl_vector<double> V( num_params );

    // Fill in the solution matrix.
    int k = 0;
    for( int c = 0; c < constraints_[i].image_points.size(); c++ ){
      vgl_point_2d<double> image_point = constraints_[i].image_points[c];
      vgl_point_3d<double> world_point = constraints_[i].world_points[c];
      S(k,0) = world_point.x();
      S(k,1) = world_point.y();
      S(k,2) = world_point.z();
      S(k,3) = 1;
      if( !affine_camera ){
        S(k,8) = -world_point.x()*image_point.x();
        S(k,9) = -world_point.y()*image_point.x();
        S(k,10) = -world_point.z()*image_point.x();
      }
      B(k) = image_point.x();
      k++;
      S(k,4) = world_point.x();
      S(k,5) = world_point.y();
      S(k,6) = world_point.z();
      S(k,7) = 1;
      if( !affine_camera ){
        S(k,8) = -world_point.x()*image_point.y();
        S(k,9) = -world_point.y()*image_point.y();
        S(k,10) = -world_point.z()*image_point.y();
      }
      B(k) = image_point.y();
      k++;
    }
    for( int c = 0; c < constraints_[i].height_top_points.size(); c++ ){
      vgl_point_2d<double> top = constraints_[i].height_top_points[c];
      vgl_point_2d<double> bot = constraints_[i].height_bot_points[c];
      S(k,2) = top.y()-bot.y();
      S(k,6) = -( top.x()-bot.x() );
      if( !affine_camera )
        S(k,10) = top.y()*( top.x()-bot.x() )-top.x()*( top.y()-bot.y() );
      k++;
    }
  
    vnl_svd<double> Ssvd(S);
    V = Ssvd.solve( B );
    // Reform the camera.
    vnl_matrix_fixed<double,3,4> P;
    P(0,0) = V(0); P(0,1) = V(1); P(0,2) = V(2); P(0,3) = V(3);
    P(1,0) = V(4); P(1,1) = V(5); P(1,2) = V(6); P(1,3) = V(7);
    if( affine_camera ){
      P(2,0) = 0; P(2,1) = 0; P(2,2) = 0; P(2,3) = 1; }
    else{
      P(2,0) = V(8); P(2,1) = V(9); P(2,2) = V(10); P(2,3) = 1; }
    cameras.push_back( vpgl_proj_camera<double>( P ) );
  }
  vcl_cerr << "  success\n";
  return true;
};


//---------------------------------------------
void 
dbrcl_compute_constrained_cameras::get_world_points(
  vcl_vector< vgl_point_3d<double> >& world_points )
{
  world_points = unmatched_world_points_;
}


#endif // _dbrcl_compute_constrained_cameras_cxx_
