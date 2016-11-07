// This is contrib/mleotta/vidreg/vidreg_tracker.h
#ifndef vidreg_tracker_h_
#define vidreg_tracker_h_

//:
// \file
// \brief An object to label matches with track IDs
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 10/17/06
//
// \verbatim
//  Modifications
// \endverbatim


#include <rgrl/rgrl_match_set_sptr.h>
#include <rgrl/rgrl_feature_sptr.h>
#include <vcl_map.h>
#include <vcl_vector.h>
#include <vcl_list.h>
#include <vcl_utility.h>


//: An object to label matches with track IDs
class vidreg_tracker
{
  public:
    //: Constructor
    vidreg_tracker();
    //: Destructor
    ~vidreg_tracker();

    //: Label the features at this frame with track IDs given matches
    void process_frame(const rgrl_match_set_sptr& matches,
                       const vcl_map<rgrl_feature_sptr, double>& weight_map);

    //: Returns a vector of track ID / feature pairs from the last frame
    vcl_vector<vcl_pair<unsigned long, rgrl_feature_sptr> >
        current_tracks() const;

  private:
    unsigned long next_track_id_;
    vcl_map<rgrl_feature_sptr, unsigned long> last_features_map_;

};


//: Filter out tracks that last less then \p n frames
class vidreg_track_filter
{
  public:
    //: Constructor
    vidreg_track_filter(unsigned int n);
    //: Destructor
    ~vidreg_track_filter();

    enum state { VALID = 0, NOT_READY = 1, DONE = 2 };

    typedef vcl_vector<vcl_pair<unsigned long, rgrl_feature_sptr> > Tvec;

    //: Add a labelled set of tracks
    void push_frame(const Tvec& tracks);

    //: Remove a relabelled subset of tracks
    state pop_frame(Tvec& tracks);

  private:
    unsigned int buffer_size_;
    unsigned long next_track_id_;
    vcl_vector<unsigned long> new_track_ids_;
    vcl_vector<unsigned int> counts_;
    bool ready_;

    vcl_list<Tvec> queue_;
};


#endif // vidreg_tracker_h_
