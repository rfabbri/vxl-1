// This is brl/brcv/trk/dbinfo/pro/dbinfo_refine_tracks_process.cxx

#include <bpro1/bpro1_storage.h>
#include <bpro1/bpro1_parameters.h>

#include <dbinfo/pro/dbinfo_refine_tracks_process.h>
#include <vcl_iostream.h>
#include <bvis1/bvis1_manager.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/vidpro1_repository.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <dbinfo/pro/dbinfo_track_storage.h>
#include <dbinfo/dbinfo_match_optimizer.h>
#include <dbinfo/dbinfo_track.h>
#include <dbinfo/dbinfo_observation.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_point_2d.h>

//: Constructor
dbinfo_refine_tracks_process::dbinfo_refine_tracks_process() : bpro1_process(), 
                                                               first_frame_(true), start_frame_(0), name_(""), opt_(0)
{
  if( !parameters()->add( "Translation increment" , "-dx", 1.0f) ||
      !parameters()->add( "Rotation increment" , "-dtheta", 0.0f) ||
      !parameters()->add( "Scale increment" , "-dscale", 0.0f) ||
      !parameters()->add( "Radius fraction" , "-frac", 0.2) ||
      !parameters()->add( "Max mutual info(kludge)" , "-maxinfo", 10.0f) ||
      !parameters()->add( "Save refined tracks" , "-save", false) ||
      !parameters()->add( "Track path" , "-path",  bpro1_filepath("","*") ))
    {
      vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
    }
}


//: Destructor
dbinfo_refine_tracks_process::~dbinfo_refine_tracks_process()
{
}


//: Clone the process
bpro1_process*
dbinfo_refine_tracks_process::clone() const
{
  return new dbinfo_refine_tracks_process(*this);
}


//: Return the name of the process
vcl_string dbinfo_refine_tracks_process::name()
{
  return "Refine Tracks";
}


//: Call the parent function and reset num_frames_
void
dbinfo_refine_tracks_process::clear_output(int resize)
{
  bpro1_process::clear_output(resize);  
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string >
dbinfo_refine_tracks_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  // no input type required
  to_return.clear();
  to_return.push_back( "image" );
  to_return.push_back("dbinfo_track_storage");
  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string >
dbinfo_refine_tracks_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );
  return to_return;
}


//: Returns the number of input frames to this process
int
dbinfo_refine_tracks_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
//  Note that no. frames is the number of frames for which output
//  data is produced, not necessarily frame images.
int
dbinfo_refine_tracks_process::output_frames()
{
  return 1;
}


//: Run the process on the current frame
bool
dbinfo_refine_tracks_process::execute()
{
  //Set the refinement parameters
  float dx=0, dtheta=0, dscale=0, maxinfo=0;
  parameters()->get_value( "-dx" , dx );
  parameters()->get_value( "-dtheta" , dtheta );
  parameters()->get_value( "-dscale" , dscale );
  parameters()->get_value( "-maxinfo" , maxinfo );
  double frac;
  parameters()->get_value( "-frac" , frac );
  opt_ = new dbinfo_match_optimizer(maxinfo, dx, dtheta, dscale);
  opt_->set_debug_level(0);
  vidpro1_image_storage_sptr image_storage;
  image_storage.vertical_cast(input_data_[0][0]);
  int f = image_storage->frame();
  unsigned frame = 0;
  if(frame>=0)
    frame = static_cast<unsigned>(f);
  else
    return false;
  //Get the track storage (a global quantity)
  dbinfo_track_storage_sptr track_storage;
  track_storage.vertical_cast(input_data_[0][1]);
  if(!track_storage)
    return false;
  name_ = track_storage->name();
  vcl_vector<dbinfo_track_sptr> tracks = track_storage->tracks();
  if(!tracks.size())
    return false;
  //Don't do any refinement of the first frame since the tracking polygon
  //is assumed to be perfect.
  if(first_frame_)
    {
      start_frame_ = tracks[0]->observ(frame)->frame();
      int track_frame = start_frame_,
        storage_frame = image_storage->frame();
      if(track_frame>storage_frame)
        return true;
      //initialize the new optimized tracks
      unsigned kt = 0;
      optimized_tracks_.resize(tracks.size());
      for(vcl_vector<dbinfo_track_sptr>::iterator trit = tracks.begin();
          trit != tracks.end(); trit++, kt++)
        {
          dbinfo_track_sptr new_track = new dbinfo_track();
          new_track->init_track((*trit)->observ(frame));
          new_track->set_id((*trit)->id());
          optimized_tracks_[kt]=new_track;
        }
      first_frame_ = false;
      return true;
    }

  //The normal execute code 
  opt_->set_frame(frame);
  vil_image_resource_sptr resc = image_storage->get_image();
  opt_->set_resource(resc);

  //generate views of the track
  vcl_vector<vsol_spatial_object_2d_sptr> polygons;
  vcl_vector<vsol_spatial_object_2d_sptr> optimized_polygons;
  vidpro1_vsol2D_storage_sptr optimized_cog = vidpro1_vsol2D_storage_new();
  unsigned k = 0;
  for(vcl_vector<dbinfo_track_sptr>::iterator trit = tracks.begin();
      trit != tracks.end(); trit++, k++)
    {
      dbinfo_observation_sptr obs = (*trit)->observ(frame);
      if(!obs)
        continue;
      dbinfo_region_geometry_sptr geo = obs->geometry();
      vsol_polygon_2d_sptr p = geo->poly(0);
      vsol_spatial_object_2d_sptr so = (vsol_polygon_2d*)(p->cast_to_polygon());
      polygons.push_back(so);
      // now optimize the geometry
      dbinfo_observation_sptr obs0 = (*trit)->observ(start_frame_);
      vcl_cout << "Optimizing Frame " << frame << " Track Id "<< (*trit)->id()
               << "\n"<< vcl_flush;
      opt_->set_obs0(obs0);
      opt_->set_best_seed(obs);
      if(!opt_->optimize())
        continue;
      double c = opt_->current_cost();
      dbinfo_observation_sptr optimized_obs = opt_->optimized_obs();
      optimized_obs->set_score(static_cast<float>(c));
      optimized_tracks_[k]->extend_track(optimized_obs);
#if 0
      vcl_vector<double> par =  opt_->current_params();
      vcl_cout <<"c["<< k << "]("<< par[0] << ' ' << par[1] << ' ' 
               << par[2] <<")= " << c << '\n';
#endif
      dbinfo_region_geometry_sptr geoo = optimized_obs->geometry();
      vsol_point_2d_sptr cog = geoo->cog();
      optimized_cog->add_object(cog->cast_to_point());
      vsol_polygon_2d_sptr po = geoo->poly(0);
      vsol_spatial_object_2d_sptr soo = 
        (vsol_polygon_2d*)(po->cast_to_polygon());
      optimized_polygons.push_back(soo);
    }
  vidpro1_vsol2D_storage_sptr output_storage = vidpro1_vsol2D_storage_new();
  output_storage->add_objects(polygons);
  
  vidpro1_vsol2D_storage_sptr opt_output_storage = vidpro1_vsol2D_storage_new();
  opt_output_storage->add_objects(optimized_polygons);

  output_data_[0].push_back(output_storage);
  output_data_[0].push_back(opt_output_storage);
  output_data_[0].push_back(optimized_cog);

  return true;
}

//: Finish by saving the optimized tracks
bool
dbinfo_refine_tracks_process::finish() 
{
  first_frame_ = true;
  bool save_tracks=false; 
  parameters()->get_value( "-save" , save_tracks);
  if(!save_tracks)
    {
      optimized_tracks_.clear();
      return true;
    }
  bpro1_filepath track_path;
  parameters()->get_value( "-path" , track_path );
  vcl_string path = track_path.path;
  // create the track storage class
  dbinfo_track_storage_sptr output_dbinfo = dbinfo_track_storage_new();
  //insert the tracks into storage
  output_dbinfo->set_tracks(optimized_tracks_);
  output_dbinfo->set_name(name_+"-optimized");

  //save the tracks into storage
  vsl_b_ofstream bp_out(path);
  output_dbinfo->b_write(bp_out);
  bp_out.close();
  optimized_tracks_.clear();
  return true;
}


