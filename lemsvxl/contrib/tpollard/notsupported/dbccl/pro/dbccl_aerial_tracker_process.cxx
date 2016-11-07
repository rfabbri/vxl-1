#include "dbccl_aerial_tracker_process.h"

#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <vil1/vil1_vil.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vsol/vsol_point_2d.h>


//-------------------------------------------
dbccl_aerial_tracker_process::dbccl_aerial_tracker_process() : bpro1_process()
{
  if( !parameters()->add( 
    "Search Radius" , "-dbcclsr" , tracker.mi_params.mi_region_rad ) ){
    vcl_cerr << "ERROR: Adding parameters in " __FILE__<< vcl_endl;
  } 
  parameters()->get_value( "-dbcclsr" , tracker.mi_params.mi_region_rad );
}


//---------------------------------------------
dbccl_aerial_tracker_process::~dbccl_aerial_tracker_process()
{
}


//--------------------------------------------
bpro1_process* 
dbccl_aerial_tracker_process::clone() const
{
  return new dbccl_aerial_tracker_process(*this);
}


//------------------------------------------
vcl_string
dbccl_aerial_tracker_process::name()
{
  return "Aerial Tracker";
}


//-----------------------------------------------
vcl_vector< vcl_string > dbccl_aerial_tracker_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  to_return.push_back( "vsol2D" );
  return to_return;
}


//----------------------------------------------------
vcl_vector< vcl_string > dbccl_aerial_tracker_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  return to_return;
}


//: Run the process on the current frame
bool
dbccl_aerial_tracker_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbccl_aerial_tracker_process::execute() - not exactly one"
          << " input image \n";
    return false;
  }
  clear_output();

  // Get your hands on any new tracks at this frame.
  vidpro1_vsol2D_storage_sptr new_tracks_storage;
  new_tracks_storage.vertical_cast(input_data_[0][1]);
  vcl_vector< vsol_spatial_object_2d_sptr > new_tracks_vsol = 
    new_tracks_storage->all_data();
  vcl_vector< vgl_point_2d<int> > new_tracks;
  for( int t = 0; t < new_tracks_vsol.size(); t++ )
    new_tracks.push_back( vgl_point_2d<int>( 
      (int)floor( new_tracks_vsol[t]->cast_to_point()->x() ),
      (int)floor( new_tracks_vsol[t]->cast_to_point()->y() ) ) );

  // Get your hands on the new image.
  vidpro1_image_storage_sptr frame_image;
  frame_image.vertical_cast(input_data_[0][0]);
  vil_image_resource_sptr image_sptr = frame_image->get_image();
  vil_image_view<vxl_byte> image_view = image_sptr->get_view();//0, image_sptr->ni(), 0, image_sptr->nj() );
  vil_image_view<vxl_byte> greyscale_view;

  if( image_view.nplanes() == 3 ) {
    vil_convert_planes_to_grey( image_view , greyscale_view );
  }
  else if ( image_view.nplanes() == 1 ) {
    greyscale_view = image_view;
  } 
  else {
    vcl_cerr << "Returning false. nplanes(): " << image_view.nplanes() << vcl_endl;
    return false;
  }

  // Track points in the new image.
  tracker.track_frame( greyscale_view, new_tracks );
  new_tracks = tracker.get_tracks();

  // Store the new tracks.
  new_tracks_vsol.clear();
  for( unsigned int t = 0; t < new_tracks.size(); t++ ) {
    vsol_spatial_object_2d_sptr new_point_obj = 
      new vsol_point_2d( (double)new_tracks[t].x(), (double)new_tracks[t].y() );
    new_tracks_vsol.push_back( new_point_obj );
  }
  vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
  output_vsol->add_objects(new_tracks_vsol, "dbccl tracks");
  output_data_[0].push_back(output_vsol);

  return true;
}


//------------------------------------------
bool
dbccl_aerial_tracker_process::finish()
{
  return true;
}


//-------------------------------------------------
int
dbccl_aerial_tracker_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
dbccl_aerial_tracker_process::output_frames()
{
  return 1;
}



