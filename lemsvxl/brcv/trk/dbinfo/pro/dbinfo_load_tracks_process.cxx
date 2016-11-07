
// This is brl/brcv/trk/dbinfo/pro/dbinfo_load_tracks_process.cxx


#include <dbinfo/pro/dbinfo_load_tracks_process.h>
#include <vcl_iostream.h>
#include <bpro1/bpro1_storage.h>
#include <bpro1/bpro1_parameters.h>
#include <bvis1/bvis1_manager.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/vidpro1_repository.h>
#include <dbinfo/pro/dbinfo_track_storage.h>
#include <vsol/vsol_polyline_2d.h>

//: Constructor
dbinfo_load_tracks_process::dbinfo_load_tracks_process() : bpro1_process(), num_frames_(0), track_storage_(0)
{
  if( !parameters()->add( "Track file <filename...>" , "-track_filename", bpro1_filepath("","*") ))
    {
      vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
    }
}


//: Destructor
dbinfo_load_tracks_process::~dbinfo_load_tracks_process()
{
}


//: Clone the process
bpro1_process*
dbinfo_load_tracks_process::clone() const
{
  return new dbinfo_load_tracks_process(*this);
}


//: Return the name of the process
vcl_string dbinfo_load_tracks_process::name()
{
  return "Load Tracks";
}


//: Call the parent function and reset num_frames_
void
dbinfo_load_tracks_process::clear_output(int resize)
{
  num_frames_ = 0;
  bpro1_process::clear_output(resize);  
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string >
dbinfo_load_tracks_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  // no input type required
  to_return.clear();

  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string >
dbinfo_load_tracks_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  //no output type required
  to_return.clear();
#if 0
  to_return.push_back( "vsol2D" );
#endif
  return to_return;
}


//: Returns the number of input frames to this process
int
dbinfo_load_tracks_process::input_frames()
{
  return 0;
}


//: Returns the number of output frames from this process
int
dbinfo_load_tracks_process::output_frames()
{
#if 0
  return num_frames_;
#endif
  return 0;
}


//: Run the process on the current frame
bool
dbinfo_load_tracks_process::execute()
{
  // read the tracks from the storage file  
  bpro1_filepath track_path;
  parameters()->get_value( "-track_filename" , track_path );
  vcl_string path = track_path.path;
  vsl_b_ifstream is(path);
  track_storage_ = new dbinfo_track_storage();
  track_storage_->b_read(is);
  track_storage_->set_frame(-2);//for global storage
  //get the geometry for each track
  vcl_vector<dbinfo_track_sptr> tracks = track_storage_->tracks();
  vcl_vector<dbinfo_track_geometry_sptr> track_geoms;
  for(vcl_vector<dbinfo_track_sptr>::iterator trit = tracks.begin();
      trit != tracks.end(); trit++)
    track_geoms.push_back((*trit)->track_geometry());
  vcl_cout << "Reached end of getting track geometry\n";
  //find the start and end frames for the full set of tracks
  //that is, the start of the earliest track and the end of the latest track
  unsigned start_frame = 0, end_frame = 0;
  for(vcl_vector<dbinfo_track_geometry_sptr>::iterator git=track_geoms.begin();
      git != track_geoms.end(); ++git)
    {
      if((*git)->start_frame()<start_frame)
        start_frame = (*git)->start_frame();
      if((*git)->end_frame()>end_frame)
        end_frame = (*git)->end_frame();
    }
  if(start_frame>end_frame)
    {
      vcl_cout << "In dbinfo_load_tracks_process::execute() -"
               << " corrupt frame sequence\n";
      return false;
    }

  //The total number of frames holding tracks
  //  num_frames_ = end_frame-start_frame +1;
  num_frames_ = end_frame+1;
  //construct the storage for each frame
  //each track at a given frame is inserted into the storage element 
  //constructed for the frame
  for(int frame = end_frame; frame>=static_cast<int>(start_frame); frame--)
    {
      //the storage element
      vidpro1_vsol2D_storage* str = new vidpro1_vsol2D_storage();
      //insert the tracks for this frame
      for(unsigned j = 0; j<track_geoms.size(); ++j)
        {
            if(track_geoms[j]->start_frame()<=frame && track_geoms[j]->end_frame()>=frame)
            {
                vsol_polyline_2d_sptr poly = 
                    track_geoms[j]->incremental_track(track_geoms[j]->start_frame(), frame);
                vsol_curve_2d* c = poly->cast_to_curve();
                vsol_spatial_object_2d_sptr so = c;
                str->add_object(so);
            }
        }
      vcl_vector< bpro1_storage_sptr > stores;
      stores.push_back(str);
      //save the vsol2D tracks into the output data for this frame
      output_data_.push_back(stores);
    }
  if(start_frame>0)
    for(int frame=static_cast<int>(start_frame)-1; frame>=0; frame--)
      output_data_.push_back(vcl_vector< bpro1_storage_sptr >(0));

  this->finish();//Not called by the execute menu macro.
  return true;
}

//: Finish
bool
dbinfo_load_tracks_process::finish() 
{
  if(!track_storage_)
    return false;
 vidpro1_repository_sptr res = bvis1_manager::instance()->repository();
   if(!res)
     return false;
   res->initialize_global();
   //more official way
   track_storage_->mark_global();
   //Old way of enforcing global
#if 0
   // frame index of -2 should indicate global storage
   if(!res->store_data_at(track_storage_, -2))
#endif
   if(!res->store_data(track_storage_))
    {
      vcl_cout << "In dbinfo_track_display_tool::display_tracks() "
               << "store to repository failed\n";
      return false;
    }
  return true;
}



