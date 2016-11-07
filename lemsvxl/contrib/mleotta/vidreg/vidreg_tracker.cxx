// This is contrib/mleotta/vidreg/vidreg_tracker.cxx

//:
// \file


#include "vidreg_tracker.h"
#include <rgrl/rgrl_match_set.h>


//: Constructor
vidreg_tracker::vidreg_tracker()
  : next_track_id_(1)
{
}


//: Destructor
vidreg_tracker::~vidreg_tracker()
{
}


//: Returns a vector of track ID / feature pairs from the last frame
vcl_vector<vcl_pair<unsigned long, rgrl_feature_sptr> >
vidreg_tracker::current_tracks() const
{
  vcl_vector<vcl_pair<unsigned long, rgrl_feature_sptr> > tracks;

  typedef vcl_map<rgrl_feature_sptr, unsigned long> Tmap;
  for(Tmap::const_iterator ti = last_features_map_.begin();
      ti!=last_features_map_.end(); ++ti)
  {
    tracks.push_back(vcl_pair<unsigned long, rgrl_feature_sptr>(ti->second,ti->first));
  }
  return tracks;
}


void
vidreg_tracker::process_frame(const rgrl_match_set_sptr& match_set,
                              const vcl_map<rgrl_feature_sptr, double>& weight_map)
{
  typedef vcl_map<rgrl_feature_sptr, unsigned long> Tmap;
  Tmap curr_features_map;

  typedef rgrl_match_set::from_iterator from_iter;
  typedef from_iter::to_iterator        to_iter;

  //  for each from image feature being matched
  for ( from_iter fitr = match_set->from_begin();
        fitr != match_set->from_end(); ++fitr )
  {
    if ( fitr.empty() )  continue;
    rgrl_feature_sptr from_feature = fitr.from_feature();

    vcl_map<rgrl_feature_sptr, double>::const_iterator f = weight_map.find(from_feature);
    if(f == weight_map.end() || f->second < 1.0)
      continue;

    unsigned long t_id = 0;
    for ( to_iter titr = fitr.begin(); titr != fitr.end(); ++titr )
    {
      rgrl_feature_sptr to_feature = titr.to_feature();
      Tmap::const_iterator ti = last_features_map_.find(to_feature);
      if(ti != last_features_map_.end()){
        t_id = ti->second;
      }
    }
    if(t_id > 0)
      curr_features_map[from_feature] = t_id;
    else
      curr_features_map[from_feature] = next_track_id_++;
  }

  last_features_map_ = curr_features_map;
}


//=============================================================================

//: Constructor
vidreg_track_filter::vidreg_track_filter(unsigned int n)
  : buffer_size_(n), next_track_id_(1), ready_(false)
{
}


//: Destructor
vidreg_track_filter::~vidreg_track_filter()
{
}


//: Add a labelled set of tracks
void
vidreg_track_filter::push_frame(const Tvec& tracks)
{
  queue_.push_back(tracks);
  vcl_vector<bool> observed(counts_.size(),false);
  for(Tvec::const_iterator i=tracks.begin(); i!=tracks.end(); ++i){
    if(i->first >= counts_.size()){
      observed.resize(i->first+1,false);
      counts_.resize(i->first+1,0);
    }
    if(!observed[i->first])
      ++counts_[i->first];
    observed[i->first] = true;
  }
  if(queue_.size() == buffer_size_)
    ready_ = true;
}


//: Remove a relabelled subset of tracks
vidreg_track_filter::state
vidreg_track_filter::pop_frame(Tvec& tracks)
{
  if(queue_.empty())
    return DONE;
  if(!ready_)
    return NOT_READY;

  tracks.clear();
  const Tvec& all_tracks = queue_.front();
  for(Tvec::const_iterator i=all_tracks.begin(); i!=all_tracks.end(); ++i){
    if(counts_[i->first] < buffer_size_)
      continue;

    if(i->first >= new_track_ids_.size())
      new_track_ids_.resize(i->first+1,0);
    if(new_track_ids_[i->first] == 0)
      new_track_ids_[i->first] = next_track_id_++;

    tracks.push_back(vcl_pair<unsigned long, rgrl_feature_sptr>
                           (new_track_ids_[i->first], i->second));
  }

  queue_.pop_front();
  return VALID;
}


