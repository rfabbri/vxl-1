// This is brl/blem/brcv/trk/dbinfo/pro/dbinfo_multi_poly_track_process.cxx
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
#include <dbinfo/dbinfo_object_matcher.h>
#include <dbinfo/pro/dbinfo_track_storage.h>
#include <dbinfo/pro/dbinfo_track_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <dbinfo/dbinfo_multi_tracker_params.h>
#include <dbinfo/pro/dbinfo_multi_poly_track_process.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_hungarian_algorithm.h>
#include <vul/vul_sprintf.h>
//#include <dbsta/dbsta_sinkhorn.h>

//: Constructor
dbinfo_multi_poly_track_process::dbinfo_multi_poly_track_process()
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
      !parameters()->add( "Score Threshold (stop a track)", "-scorethresh",
                          tracker_.scorethresh_)                            ||
      !parameters()->add( "Maximum Moion expected", "-expmotion",
                          (float)15)                                        ||
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
dbinfo_multi_poly_track_process::
dbinfo_multi_poly_track_process(const dbinfo_multi_poly_track_process& other) :
  bpro1_process(other), first_frame_(true), tracker_(other.tracker_){}


void dbinfo_multi_poly_track_process::set_tracker_parameters()
{
  parameters()->get_value( "-n_samples",           tracker_.n_samples_);
  parameters()->get_value( "-search_radius",       tracker_.search_radius_);
  parameters()->get_value( "-angle_range",         tracker_.angle_range_);
  parameters()->get_value( "-scale_range",         tracker_.scale_range_);
  parameters()->get_value( "-sigma",               tracker_.sigma_);
  parameters()->get_value( "-scorethresh",         tracker_.scorethresh_);
  
  parameters()->get_value( "-int_info",            tracker_.intensity_info_);
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
dbinfo_multi_poly_track_process::~dbinfo_multi_poly_track_process()
{
}


//: Clone the process
bpro1_process* 
dbinfo_multi_poly_track_process::clone() const
{
  return new dbinfo_multi_poly_track_process(*this);
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string > dbinfo_multi_poly_track_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  to_return.push_back( "vsol2D" );
  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string > dbinfo_multi_poly_track_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );
  return to_return;
}


//: Returns the number of input frames to this process
int
dbinfo_multi_poly_track_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
dbinfo_multi_poly_track_process::output_frames()
{
  return 1;
}


//: Run the process on the current frame
bool
dbinfo_multi_poly_track_process::execute()
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

  vcl_vector< vsol_spatial_object_2d_sptr > detections = detection_storage->all_data(); 

  if(!detections.size())
  {
      vcl_cout << "no detections in frame no. "<<detection_storage->frame()<<"\n";
      return true;
  }

  vcl_vector<dbinfo_observation_sptr> curr_obs;
  //: obtain the detected polygons
  for(vcl_vector< vsol_spatial_object_2d_sptr >::iterator dit =
      detections.begin(); dit != detections.end(); ++dit)
  {
      vsol_spatial_object_2d* so = (*dit).ptr();
      assert(so);
      vsol_polygon_2d_sptr poly = so->cast_to_region()->cast_to_polygon();
      assert(poly);
      vsol_box_2d_sptr b2d=poly->get_bounding_box();
      if(b2d->get_max_x()<tracker_.image()->ni()-10 && b2d->get_max_y()<tracker_.image()->nj()-10 &&
          b2d->get_min_x()>10 && b2d->get_min_y()>10 )
      {
          dbinfo_observation_sptr new_obs=new dbinfo_observation(frame,tracker_.image(),poly,
                                                                 tracker_.intensity_info_,
                                                                 tracker_.gradient_info_,
                                                                 tracker_.color_info_);
          curr_obs.push_back(new_obs);
      }
  }

  //create the storage for display
  vcl_vector<vsol_spatial_object_2d_sptr> polygons;
  unsigned n_trks = tracker_.n_tracks();

  vcl_vector<dbinfo_mi_track_sptr> enabled_tracks;
  for(unsigned i = 0; i<n_trks; ++i)
  {
      dbinfo_mi_track_sptr tr = tracker_.track(i);
      assert(tr);
      if(!tr->is_disabled())
            enabled_tracks.push_back(tr);
  }

  vnl_matrix<double> cost_matrix(curr_obs.size(),enabled_tracks.size(),0.0);
  float expmotion=0.0;
  parameters()->get_value( "-expmotion",  expmotion);

  unsigned track_counter=0;
  for(unsigned i = 0; i<n_trks; ++i)
  {
      dbinfo_mi_track_sptr tr = tracker_.track(i);
      assert(tr);
      if(tr->is_disabled())
         continue;
      dbinfo_observation_sptr prev_obs=tr->seed(0);
      vcl_vector<dbinfo_observation_sptr>::iterator iter;
      float max_score=0.0;
      unsigned curr_obs_counter=0;
      for(iter=curr_obs.begin();iter!=curr_obs.end();iter++)
      {

          vil_image_resource_sptr img;

          dbinfo_region_geometry_sptr geo_db = (*iter)->geometry();
          dbinfo_region_geometry_sptr geo_q = prev_obs->geometry();
          vsol_point_2d_sptr cent_db = geo_db->centroid();
          vsol_point_2d_sptr cent_q = geo_q->centroid();

          double dist=vcl_sqrt((cent_db->x()-cent_q->x())*(cent_db->x()-cent_q->x())+(cent_db->y()-cent_q->y())*(cent_db->y()-cent_q->y()));
          if(dist<expmotion)
          {
           vgl_h_matrix_2d<float> h;
           float score=dbinfo_object_matcher ::minfo_rigid_alignment_rand(*iter,prev_obs,h,
                                                             tracker_.search_radius_,
                                                             tracker_.angle_range_,
                                                             tracker_.scale_range_,
                                                             0.0,1.0,0.8,tracker_.n_samples_,img,
                                                             true,1.0,tracker_.intensity_info_,
                                                             tracker_.gradient_info_);
          if(score<   tracker_.scorethresh_ )
            cost_matrix(curr_obs_counter, track_counter)=  100;
          else
            cost_matrix(curr_obs_counter, track_counter)=  10-score;
          }
          else
              cost_matrix(curr_obs_counter, track_counter)=  200;
          
          //if(score>tracker_.scorethresh_  )
          //{
          //    //Extend with the top candidate
          //    tr->add_seed(*iter);
          //    vcl_cout << "Extending Track " << tr->id()<< " with observation score "<< score <<'\n'<< vcl_flush;
          //    //Add the candidates as seeds for the next iteration
          //    //tr->set_seeds(obsvs);
          //}
          curr_obs_counter++;
      }
      //if(tr->n_seeds()==0)
      //{
      //    tr->disable();
      //    continue;
      //}

      
        
      //dbinfo_observation_sptr seed = tr->seed(0);
      //assert(seed);
      //dbinfo_region_geometry_sptr geo = seed->geometry();
      //assert(geo);

      ////Assume for now 1 region per observation
      //vsol_polygon_2d_sptr poly = geo->poly(0);
      //polygons.push_back(poly->cast_to_spatial_object()); 
      //vsol_point_2d_sptr cog = geo->cog();
      //polygons.push_back(cog->cast_to_spatial_object()); 
    track_counter++;
    tr->disable();
    }

  //dbsta_sinkhorn<float> sinkhornopt(cost_matrix,0.01);
  //sinkhornopt.set_maxiter(5);
  //sinkhornopt.normalize(1e-5);


  //cost_matrix=sinkhornopt.M();


  //for(unsigned i=0;i<cost_matrix.rows();i++)
  //{
  //    bool curr_obs_taken=false;
  //    for(unsigned j=0;j<cost_matrix.cols();j++)
  //    {
  //        if(cost_matrix(i,j)>0.51)
  //            curr_obs_taken=true;
  //    }
  //}

  vcl_vector<unsigned> assignment=vnl_hungarian_algorithm<double>(cost_matrix);
  for(unsigned i=0;i<assignment.size();i++)
  {
        if(assignment[i]!=unsigned(-1))
        {
            if(cost_matrix(i,assignment[i])<100)
            {
            enabled_tracks[assignment[i]]->enable();
            dbinfo_observation_sptr p_obs=enabled_tracks[assignment[i]]->seed(0);
            vgl_h_matrix_2d<float> f;
                      vil_image_resource_sptr img;

            float s=dbinfo_object_matcher ::minfo_rigid_alignment_rand(curr_obs[i],p_obs,f,
                                                                         tracker_.search_radius_,
                                                                         tracker_.angle_range_,
                                                                         tracker_.scale_range_,
                                                                         0.0,1.0,0.8,tracker_.n_samples_,img,
                                                                         true,1.0,tracker_.intensity_info_,
                                                                         tracker_.gradient_info_);
            enabled_tracks[assignment[i]]->extend_track(curr_obs[i],f);
            enabled_tracks[assignment[i]]->set_seeds(vcl_vector<dbinfo_observation_sptr>(1,curr_obs[i]));
            curr_obs[i]=NULL;
            }
        }

  }

  vidpro1_vsol2D_storage_sptr output_track_storage = vidpro1_vsol2D_storage_new();
  vcl_vector<vsol_spatial_object_2d_sptr> tracks;
  for(unsigned i = 0; i<n_trks; ++i)
  {
      dbinfo_mi_track_sptr tr = tracker_.track(i);
      assert(tr);

      dbinfo_track_geometry_sptr trk=tr->track_geometry();
      vul_sprintf s("Track%d",i);
      output_track_storage->add_object(trk->current_track()->cast_to_spatial_object(),s);
      if(tr->is_disabled())
          continue;
      if(tr->n_seeds()==0)
      {
          tr->disable();
          continue;
      }
      dbinfo_observation_sptr seed = tr->seed(0);
      assert(seed);
      dbinfo_region_geometry_sptr geo = seed->geometry();
      assert(geo);

      ////Assume for now 1 region per observation
      vsol_polygon_2d_sptr poly = geo->poly(0);
      polygons.push_back(poly->cast_to_spatial_object()); 
      vsol_point_2d_sptr cog = geo->cog();
      polygons.push_back(cog->cast_to_spatial_object()); 
  }


  for(unsigned i = 0; i<curr_obs.size(); ++i)
  {
      if(curr_obs[i])
      {
          dbinfo_mi_track_sptr new_track = new dbinfo_mi_track();
          
          new_track->init_track(curr_obs[i]);
          new_track->set_id(tracker_.n_tracks());
          new_track->set_seeds(vcl_vector<dbinfo_observation_sptr>(1,curr_obs[i]));
          tracker_.add_track(new_track);
      }
  }

  vidpro1_vsol2D_storage_sptr output_storage = vidpro1_vsol2D_storage_new();
  output_storage->add_objects(polygons);
  output_data_[0].push_back(output_storage);

  
  output_data_[0].push_back(output_track_storage);
  return true;
}


//: Finish
bool
dbinfo_multi_poly_track_process::finish()
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
