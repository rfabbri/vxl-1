// This is brl/bseg/dbinfo/dbinfo_multi_tracker.cxx
#include "dbinfo_multi_tracker.h"
//:
// \file
#include <vcl_cmath.h> // for vcl_fabs(double)
#include <vcl_algorithm.h>
#include <vcl_cassert.h>
#include <vul/vul_timer.h>
#include <vnl/vnl_math.h>
#include <vil/vil_image_resource.h>
#include <vsol/vsol_polygon_2d.h>
#include <dbinfo/dbinfo_feature_data.h>
#include <dbinfo/dbinfo_intensity_feature.h>
#include <dbinfo/dbinfo_gradient_feature.h>
#include "dbinfo_ihs_feature.h"
#include <dbinfo/dbinfo_observation.h>
#include <dbinfo/dbinfo_observation_generator.h>
#include <dbinfo/dbinfo_observation_matcher.h>
#include <dbinfo/dbinfo_mi_track.h>
#include <dbinfo/dbinfo_match_optimizer.h>


//Gives a sort on scores in decreasing order
static bool score_compare(dbinfo_observation_sptr const o1, dbinfo_observation_sptr const o2)
{
  assert(o1&&o2);
  return o1->score() > o2->score();//JLM Switched
}

//---------------------------------------------------------------
// Constructors
//
//----------------------------------------------------------------

//: constructor from a parameter block (the only way)
//
dbinfo_multi_tracker::dbinfo_multi_tracker(const dbinfo_multi_tracker_params& tp)
  : dbinfo_multi_tracker_params(tp)
{
  id_counter_ = 0;
}

//:Default Destructor
dbinfo_multi_tracker::~dbinfo_multi_tracker()
{

}
void 
dbinfo_multi_tracker::set_image(const unsigned frame, vil_image_resource_sptr const& image)
{
  frame_ = frame; 
  resc_ = image;
  id_counter_ = 100*frame;//allow 100 tracks per frame (make a parameter later)
}


//========================================================================
//                          tracking methods
//========================================================================

//=====   potentially start a new track  =========
//This method attempts to start a new track with the specified tracking regions.
//The regions may have been supplied by a motion or background segmenter
//It is possible that an existing track is already predicting essentially the
//same region as covered by the new track regions.  In this case it is assumed that the 
//existing track has dominance and the specified track regions are ignored.
//Otherwise a new track is initiated.
bool dbinfo_multi_tracker::
initiate_track(vcl_vector<vsol_polygon_2d_sptr> const& track_regions)
{
  // for now always initiate a track.
  
  if(!resc_||track_regions.size()==0)
    return false;

  if(!intensity_info_&&!gradient_info_&&!color_info_)
    {
      vcl_cout << "In dbinfo_multi_tracker::initiate_track - no information"
               << " channels\n";
      return false;
    }
  // Construct the observation
  vcl_vector<dbinfo_feature_base_sptr> features;
  if(intensity_info_)
    {
      dbinfo_feature_base_sptr intf = new dbinfo_intensity_feature();
      features.push_back(intf);
    }
  if(gradient_info_)
    {
      dbinfo_feature_base_sptr gradf = new dbinfo_gradient_feature();
     features.push_back(gradf);
    }  
  if(color_info_)
    {
  dbinfo_feature_data_base_sptr data;
      dbinfo_feature_base_sptr ihsf = new dbinfo_ihs_feature();
      features.push_back(ihsf);
    }  

  dbinfo_observation_sptr obs = new dbinfo_observation(frame_, resc_, track_regions, features);
  
  // Initiate the new track
  dbinfo_mi_track* new_track = new dbinfo_mi_track();
  new_track->init_track(obs);
  new_track->add_seed(obs);
  new_track->set_id(id_counter_);
  ++id_counter_;
  tracks_.push_back(new_track);
  vcl_cout << "Initiated Track " << new_track->id() << '\n';
  return true;
}
//Generate a set of track extension hypotheses from seeds.
vcl_vector<dbinfo_observation_sptr> 
dbinfo_multi_tracker::generate_hypotheses_from_seeds(dbinfo_mi_track_sptr const& track)
{
  vul_timer t;
  assert(track);
  vcl_vector<dbinfo_observation_sptr> seeds = track->seeds();
  vcl_vector<dbinfo_observation_sptr> hypos;  
  vcl_vector<dbinfo_observation_sptr> genobs;
  for(vcl_vector<dbinfo_observation_sptr>::iterator sit = seeds.begin();
      sit !=seeds.end(); ++sit)
    {
      vul_timer t1;
      //generate n_samples_ random samples around each seed
      dbinfo_observation_generator::uniform_about_seed(n_samples_, *sit, genobs,
                                                       search_radius_, search_radius_,
                                                       angle_range_, scale_range_);
      for(vcl_vector<dbinfo_observation_sptr>::iterator hit = genobs.begin();
          hit != genobs.end(); ++hit)
        {
          dbinfo_observation_sptr obs = *hit;
          dbinfo_region_geometry_sptr geom = obs->geometry();
          if(!obs->scan(frame_, resc_))
            {
              track->disable();
              continue;
            }
          //:rate the hypothesis compatibility with the track
          float s = track->score(*hit);
          (*hit)->set_score(s);
          hypos.push_back(*hit);
        }
    }
  //sort the hypotheses
  vcl_sort(hypos.begin(), hypos.end(), score_compare);

  vcl_cout << "Generated  " << hypos.size() << " hypotheses of area "
           << (hypos[0]->geometry()->size())/1000.0  
           << " Kpix in "  << t.real() << " msec\n";
  

  //Transfer the top winners to the output
  vcl_vector<dbinfo_observation_sptr> out;
  int n = 0;
  for(vcl_vector<dbinfo_observation_sptr>::iterator hit = hypos.begin();
      (hit !=hypos.end())&&(n<n_samples_); ++hit, ++n)
    out.push_back(*hit);
  
  return out;
}

//extend all existing tracks (if possible)
// A set of hypotheses are generated for the extension of each track
// The hypotheses are evaluated and then if the score of the best hypotheses is above a 
// specified threshold the track is extended.  The M highest ranking hypotheses are
// added as seeds to the track for the next track iteration
bool dbinfo_multi_tracker::extend_tracks_from_seeds()
{
  bool at_least_one_track_is_extended = false;
  for(vcl_vector<dbinfo_mi_track_sptr>::iterator ti = tracks_.begin();
      ti != tracks_.end(); ++ti)
    {
      dbinfo_mi_track_sptr mi_trk = (*ti);
      if(mi_trk->is_disabled())
        continue;
      //Generate a sorted list of the top M track extension hypotheses for track ti.
      vcl_vector<dbinfo_observation_sptr> obsvs =
        generate_hypotheses_from_seeds(mi_trk);
      if(obsvs.size()==0)
        continue;

      dbinfo_observation_sptr seed = obsvs[0];
      //Optionally refine the top candidate 
      if(optimize_)
        {
          dbinfo_match_optimizer opt(10.0f, search_radius_, angle_range_,
                                     scale_range_);
          opt.set_frame(frame_);
          opt.set_resource(resc_);
          opt.set_obs0(mi_trk->observ(0));
          opt.set_best_seed(seed);
          if(opt.optimize())
            {
              seed  = opt.optimized_obs();
              obsvs[0]=seed;
              float s = mi_trk->score(seed);
              seed->set_score(s);
            }
        }
    if(seed->score()>scorethresh_ )
        {
        //Extend with the top candidate
        if(!mi_trk->extend_track(seed))
            continue;
        vcl_cout << "Extending Track " << mi_trk->id() 
            << " with observation score "<< seed->score() <<'\n'
            << vcl_flush;
        //Add the candidates as seeds for the next iteration
        mi_trk->set_seeds(obsvs);
        at_least_one_track_is_extended = true;
        }
    else
        {
        mi_trk->disable();
        }

    }
  return at_least_one_track_is_extended;
}

void dbinfo_multi_tracker::clear()
{
  resc_  = 0;
  id_counter_ = 0;
  tracks_.clear();
}
