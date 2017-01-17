// This is brl/bseg/dbinfo/dbinfo_mi_track.h
#ifndef dbinfo_mi_track_h_
#define dbinfo_mi_track_h_
//---------------------------------------------------------------------
//:
// \file
// \brief Track construction based on mutual information and optional seeds
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
#include <vbl/vbl_ref_count.h>
#include <vcl_cassert.h>
#include <dbinfo/dbinfo_track.h>
#include <dbinfo/dbinfo_observation_sptr.h>

class dbinfo_mi_track : public dbinfo_track
{

 public:
  //: Constructors/destructor
  dbinfo_mi_track(){}

  //:default destructor
  ~dbinfo_mi_track(){}

  //: A numerical score measuring the compatibility of an observation with the track
  //  Assumed to be monotonically increasing according to compatibility
  virtual float score(dbinfo_observation_sptr const& obs,
                      bool verbose = false);

  // methods associated with seeds
  //: get all seeds
  vcl_vector<dbinfo_observation_sptr> seeds() const {return seeds_;}

  //: set all seeds

  void set_seeds(vcl_vector<dbinfo_observation_sptr> const& seeds)
    {seeds_.clear(); seeds_ = seeds;}

  //: add a single seed
  void add_seed(dbinfo_observation_sptr const& seed){seeds_.push_back(seed);}

  //: total number of seeds
  unsigned n_seeds() const {return seeds_.size();}

  //: The seed with index i
  dbinfo_observation_sptr seed(const unsigned i) const
    {assert( i<n_seeds()); return seeds_[i];}

  //: The most representative seed (not necessarily the highest score)
  dbinfo_observation_sptr representative_seed();
  
  //: Safe downcasting methods
  virtual dbinfo_mi_track* cast_to_mi_track() { return this; }
  virtual dbinfo_mi_track const* cast_to_mi_track() const { return this; }

 protected:
  //: A set of observations that were ranked highest in compatibility with the track
  vcl_vector<dbinfo_observation_sptr> seeds_;
};
#include <dbinfo/dbinfo_mi_track_sptr.h>
#endif // dbinfo_mi_track_h_
