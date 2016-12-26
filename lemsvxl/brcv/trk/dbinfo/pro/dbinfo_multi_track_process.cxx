// This is brl/blem/brcv/trk/dbinfo/pro/dbinfo_multi_track_process.cxx
//:
// \file
#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsl/vsl_binary_io.h>
#include <dbinfo/dbinfo_region_geometry.h>
#include <dbinfo/dbinfo_observation.h>
#include <dbinfo/dbinfo_track.h>
#include <dbinfo/dbinfo_mi_track.h>
#include <dbinfo/pro/dbinfo_track_storage.h>
#include <dbinfo/pro/dbinfo_track_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <dbinfo/dbinfo_multi_tracker_params.h>
#include <dbinfo/pro/dbinfo_multi_track_process.h>


//: Constructor
dbinfo_multi_track_process::dbinfo_multi_track_process()
  : bpro1_process(), name_("track_storage"), first_frame_(true), 
    tracker_(dbinfo_multi_tracker(dbinfo_multi_tracker_params()))
{
  if( !parameters()->add( "Num Samples", "-n_samples",
                          tracker_.n_samples_)                              ||
      !parameters()->add( "Search Radius", "-search_radius",
                          tracker_.search_radius_)                          ||
      !parameters()->add( "Angle Range",  "-angle_range",
                          tracker_.angle_range_)                            ||
      !parameters()->add( "Scale Range",    "-scale_range",
                          tracker_.scale_range_)                            ||
      !parameters()->add( "Sigma",          "-sigma",
                          tracker_.sigma_)                                  ||
      !parameters()->add( "Intensity Info",      "-int_info",
                          tracker_.intensity_info_)                         ||
      !parameters()->add( "Gradient Info",      "-grad_info",
                          tracker_.gradient_info_)                          ||
      !parameters()->add( "Color Info",      "-color_info",
                          tracker_.color_info_)                             ||
      !parameters()->add( "Optimize ( aoemba )", "-optimize",
                          tracker_.optimize_)                               ||
      !parameters()->add( "Num Intensity Bins", "-n_int_bins", 
                          tracker_.intensity_hist_bins_)                    ||
      !parameters()->add( "Num Grad Dir Bins",  "-n_grad_dir_bins", 
                          tracker_.gradient_dir_hist_bins_)                 ||
      !parameters()->add( "Num Color Bins",  "-n_color_bins", 
                          tracker_.color_hist_bins_)                 ||
      !parameters()->add( "Save Tracks",            "-save_tracks", 
                          tracker_.save_tracks_) ||
       !parameters()->add( "Track Storage File" ,   "-ftrk" ,
                           bpro1_filepath("","*.*")) ||
      !parameters()->add( "Verbose",            "-v", 
                          tracker_.verbose_) )
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
} 


//: Copy Constructor
dbinfo_multi_track_process::
dbinfo_multi_track_process(const dbinfo_multi_track_process& other) :
  bpro1_process(other), first_frame_(true), tracker_(other.tracker_){}


void dbinfo_multi_track_process::set_tracker_parameters()
{
  parameters()->get_value( "-n_samples",         tracker_.n_samples_);
  parameters()->get_value( "-search_radius",       tracker_.search_radius_);
  parameters()->get_value( "-angle_range",  tracker_.angle_range_);
  parameters()->get_value( "-scale_range",         tracker_.scale_range_);
  parameters()->get_value( "-sigma",               tracker_.sigma_);
  parameters()->get_value( "-int_info",       tracker_.intensity_info_);
  parameters()->get_value( "-grad_info",       tracker_.gradient_info_);
  parameters()->get_value( "-color_info",       tracker_.color_info_);
  parameters()->get_value( "-optimize",       tracker_.optimize_);
  parameters()->get_value("-n_int_bins",tracker_.intensity_hist_bins_);
  parameters()->get_value("-n_grad_dir_bins",tracker_.gradient_dir_hist_bins_);
  parameters()->get_value("-n_color_bins",tracker_.gradient_dir_hist_bins_);
  parameters()->get_value( "-save_tracks",     tracker_.save_tracks_);
  parameters()->get_value( "-ftrk",     this->track_storage_filepath_);
  parameters()->get_value( "-v",             tracker_.verbose_);
}

//: Destructor
dbinfo_multi_track_process::~dbinfo_multi_track_process()
{
}


//: Clone the process
bpro1_process* 
dbinfo_multi_track_process::clone() const
{
  return new dbinfo_multi_track_process(*this);
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string > dbinfo_multi_track_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  to_return.push_back( "vsol2D" );
  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string > dbinfo_multi_track_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  return to_return;
}


//: Returns the number of input frames to this process
int
dbinfo_multi_track_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
dbinfo_multi_track_process::output_frames()
{
  return 1;
}


//: Run the process on the current frame
bool
dbinfo_multi_track_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cerr << __FILE__ << " - not exactly one input frame" << vcl_endl;
    return false;
  }
  this->set_tracker_parameters();  

  vidpro1_image_storage_sptr frame_image;
  frame_image.vertical_cast(input_data_[0][0]);

  if(!frame_image)
    return false;

  unsigned frame = input_data_[0][0]->frame();
  vcl_cout << "MultiTrack: " << frame << '\n';

  //Set current frame image on the tracker
  tracker_.set_image(frame, frame_image->get_image());

  //Get the detections 
  vidpro1_vsol2D_storage_sptr detection_storage;
  detection_storage.vertical_cast(input_data_[0][1]);

  //put test here, change later when detections are on all frames
  if(first_frame_&&!detection_storage)
    return false;

 

  if(first_frame_)
    {
      name_ = detection_storage->name();
      vcl_cout << "MultiTrack: Executing First Frame\n";
      vcl_cout << (dbinfo_multi_tracker_params)tracker_ << '\n';
      vcl_vector< vsol_spatial_object_2d_sptr > detections = 
        detection_storage->all_data(); 
      if(!detections.size())
        {
          vcl_cout << "In dbinfo_multi_track_process::execute() -"
                   << " no detections in first frame\n";
          return false;
        }
      //Get each polygon and treat as a track initiation (for now)
      for(vcl_vector< vsol_spatial_object_2d_sptr >::iterator dit =
            detections.begin(); dit != detections.end(); ++dit)
        {
          vsol_spatial_object_2d* so = (*dit).ptr();
          assert(so);
          vsol_polygon_2d_sptr poly = so->cast_to_region()->cast_to_polygon();
          assert(poly);
          vcl_vector<vsol_polygon_2d_sptr> polys; polys.push_back(poly);
          tracker_.initiate_track(polys);
        }
      first_frame_ = false;
      return true;
    }
  //ignore detections on all other frames
  tracker_.extend_tracks_from_seeds();

  //create the storage for display
  vcl_vector<vsol_spatial_object_2d_sptr> polygons;
  unsigned n_trks = tracker_.n_tracks();
  for(unsigned i = 0; i<n_trks; ++i)
    {
      dbinfo_mi_track_sptr tr = tracker_.track(i);
      assert(tr);
      if(tr->n_seeds()==0)
        continue;
      dbinfo_observation_sptr seed = tr->seed(0);
      assert(seed);
      dbinfo_region_geometry_sptr geo = seed->geometry();
      assert(geo);

      //Assume for now 1 region per observation
      vsol_polygon_2d_sptr poly = geo->poly(0);
      polygons.push_back(poly->cast_to_spatial_object()); 
      vsol_point_2d_sptr cog = geo->cog();
      polygons.push_back(cog->cast_to_spatial_object()); 
#if 0
      vcl_vector<dbinfo_observation_sptr> seeds = tr->seeds();
      for(vcl_vector<dbinfo_observation_sptr>::iterator sit = seeds.begin();
          sit != seeds.end(); ++sit)
        {
          dbinfo_region_geometry_sptr geo = (*sit)->geometry();
          assert(geo);
          vsol_polygon_2d_sptr poly = geo->poly(0);
          polygons.push_back(poly->cast_to_spatial_object());
        }
#endif
    }

  vidpro1_vsol2D_storage_sptr output_storage = vidpro1_vsol2D_storage_new();
  output_storage->add_objects(polygons);
  output_data_[0].push_back(output_storage);
  return true;
}


//: Finish
bool
dbinfo_multi_track_process::finish()
{
  first_frame_ = true;
  if(tracker_.save_tracks_)
    {
      vcl_string path = track_storage_filepath_.path;
      // create the track storage class
      dbinfo_track_storage_sptr output_dbinfo = dbinfo_track_storage_new();
      vcl_vector<dbinfo_mi_track_sptr> mi_tracks = tracker_.tracks();

      vcl_vector<dbinfo_track_sptr> tracks;
      for(vcl_vector<dbinfo_mi_track_sptr>::iterator trit = mi_tracks.begin();
          trit != mi_tracks.end(); ++trit)
        tracks.push_back((dbinfo_track*)((dbinfo_mi_track*)(*trit).ptr()));
      
      //store the tracks
      output_dbinfo->set_tracks(tracks);
      output_dbinfo->set_name(name_);
      vsl_b_ofstream bp_out(path);
      output_dbinfo->b_write(bp_out);
      bp_out.close();
    }
  //Clear existing tracks
  tracker_.clear();
  return true;
}
