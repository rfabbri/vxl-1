#include <vcl_string.h>
#include <vcl_vector.h>
#include<vcl_cstdio.h>
#include <vbl/vbl_bounding_box.h>
#include <vcl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vpgl/algo/vpgl_list.h>
#include <dbinfo/dbinfo_track.h>
#include <dbinfo/pro/dbinfo_track_storage.h>
#include <dbinfo/pro/dbinfo_track_storage_sptr.h>
#include <dbrcl/dbrcl_point_tracker.h>
#include <dbrcl/dbrcl_compute_constrained_cameras.h>
#include <dbrcl/dbrcl_compute_cameras.h>


//-------------------------------------------
int main( int argc, char* argv[] )
{
  if(argc!=4 && argc!=3){
    vcl_cout<<"Usage : dbrcl_app constraint_file camera_file track_file\n";
    return -1;
  }

  vcl_string constraint_file_name(argv[1]);
  vcl_string camera_file_name(argv[2]);
  vcl_string track_file_name;
  if( argc == 4 ) track_file_name = vcl_string(argv[3]);

  // STEP 1: Compute the constrained cameras.
  dbrcl_compute_constrained_cameras ccc;
  ccc.verbose = true;
  if( !ccc.read_constraint_file( constraint_file_name ) )
    return -1;
  vcl_vector< vpgl_proj_camera<double> > constrained_cameras;
  vcl_vector<int> constrained_frames;
  if( argc == 3 ){
    if( !ccc.compute_cameras_separately( constrained_cameras, constrained_frames ) )
      return -1;
    if( !vpgl_write_list( constrained_cameras, camera_file_name ) )
      return -1;
    else return 0;
  }
  if( !ccc.compute_cameras( constrained_cameras, constrained_frames ) )
    return -1;

  // STEP 2: Retrieve the tracks
  vcl_vector< dbinfo_track_geometry_sptr > tracks;
  vsl_b_ifstream is( track_file_name );
  dbinfo_track_storage_sptr track_storage = new dbinfo_track_storage();
  track_storage->b_read( is );
  vcl_vector< dbinfo_track_sptr > track_list = track_storage->tracks();
  for( int i = 0; i < static_cast<int>(track_list.size()); i++ )
    tracks.push_back( track_list[i]->track_geometry() );

  // STEP 3: Compute the rest of the cameras.
  dbrcl_compute_cameras cc;
  //cc.verbose = true;
  vcl_vector< vpgl_proj_camera<double> > cameras;
  if( !cc.compute_cameras( tracks, 
    constrained_cameras, constrained_frames, cameras ) )
    return -1;

  // Record the cameras in a file.
  if( !vpgl_write_list( cameras, camera_file_name ) )
    return -1;

  return 0;
}
