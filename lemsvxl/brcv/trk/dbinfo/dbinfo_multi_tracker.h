// This is brl/bseg/dbinfo/dbinfo_multi_tracker.h
#ifndef dbinfo_multi_tracker_h_
#define dbinfo_multi_tracker_h_
//---------------------------------------------------------------------
//:
// \file
// \brief A multi-object tracker
//
// Maintains a set of tracks and is responsible for starting and killing tracks
// A set of observations for each track is generated and evaluated to find the most
// suitable match based on the compatiblity score.  
//
// \author
//  J.L. Mundy - May 9, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vcl_cassert.h>
#include <vbl/vbl_ref_count.h>
#include <vil/vil_image_resource_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <dbinfo/dbinfo_multi_tracker_params.h>
#include <dbinfo/dbinfo_mi_track_sptr.h>
#include <dbinfo/dbinfo_observation_sptr.h>

class dbinfo_multi_tracker : public dbinfo_multi_tracker_params
{

 public:
  //: Constructors/destructor
  dbinfo_multi_tracker(const dbinfo_multi_tracker_params& tp);

  //:default destructor
  ~dbinfo_multi_tracker();

  //:accessors

  void set_image(const unsigned frame, vil_image_resource_sptr const& resc);

  vil_image_resource_sptr image(){return  resc_;}


  dbinfo_mi_track_sptr track(unsigned i) {assert(i<n_tracks()); return tracks_[i];}

  vcl_vector<dbinfo_mi_track_sptr> tracks()
    { return tracks_;}  

  unsigned n_tracks() {return tracks_.size();}  

  void add_track(dbinfo_mi_track_sptr new_track){tracks_.push_back(new_track);}
  //:tracking methods
  //:potentially start a new track
  bool initiate_track(vcl_vector<vsol_polygon_2d_sptr> const& track_regions);

  //:extend all existing tracks using randomly generated hypotheses
  bool extend_tracks_from_seeds();

  //:flush tracks that have not been extended for the last k frames
  bool prune_tracks(const unsigned k)
    {return true;}

    //local utility methods
  vcl_vector<dbinfo_observation_sptr> generate_hypotheses_from_seeds(dbinfo_mi_track_sptr const& track);

  //: clear all data
  void clear();
 protected:
  //:constructor from parameters only. Default constructor not used
  dbinfo_multi_tracker();


  //:current frame being tracked
  unsigned frame_;
  //:current frame image
  vil_image_resource_sptr resc_;

  //: track id counter
  unsigned id_counter_;

  //:existing tracks
  vcl_vector<dbinfo_mi_track_sptr> tracks_;

};


#endif // dbinfo_multi_tracker_h_
