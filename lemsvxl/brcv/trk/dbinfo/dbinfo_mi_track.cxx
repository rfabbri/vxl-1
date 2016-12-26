#include <dbinfo/dbinfo_observation.h>
#include <dbinfo/dbinfo_observation_matcher.h>
#include <dbinfo/dbinfo_mi_track.h>


//A numerical score measuring the compatibility of an observation with the track
float dbinfo_mi_track::score(dbinfo_observation_sptr const& obs,
                             bool verbose)
{
  unsigned k = observations_.size();
  if(!obs||!k)
    return 0;
  dbinfo_observation_sptr obs0 = dbinfo_track::observ(start_frame_);
  //  dbinfo_observation_sptr obs0 = observations_[k-1];
#if 0 //JLM DEBUG
  vcl_cout << "scoring : obs0 "  << obs0 << " vs " << obs << '\n';
#endif
  float info0 = dbinfo_observation_matcher::minfo(obs0, obs, verbose);
  //  float infok = dbinfo_observation_matcher::minfo(obs_k, obs, verbose);
  return info0;
}

// The most representative seed (not necessarily the highest score)
// When tracking it is possible to have a distribution of seed transformations
// The seed with highest mutual information may not be the most representative
// seed.  Instead, the distribution of seed transforms can be used to define
// a representative seed that is closest to the expected transformation.
// However there can be multiple modes, where a subset of the seeds is 
// attracted to other tracks. 
dbinfo_observation_sptr dbinfo_mi_track::representative_seed()
{
  return 0;
}
