#if !defined(SPLR_EXPLICIT_SPLAT_COLLECTION_TXX_)
#define SPLR_EXPLICIT_SPLAT_COLLECTION_TXX_

#include <splr/splr_explicit_splat_collection.h>
#include <vcl_utility.h>
#include <biob/biob_worldpt_index.h>
#include <splr/splr_map.txx>
#include <vsl/vsl_pair_io.txx>
#include <xmvg/xmvg_composite_filter_2d.h>
#include <xmvg/io/xmvg_io_composite_filter_2d.h>
#include <xmvg/algo/xmvg_ramp_compensation.h>

template<class T, class F, class scan_class, class filter_2d_class, class filter_3d_class, class camera_class>
splr_explicit_splat_collection<T,F, scan_class, filter_2d_class, filter_3d_class, camera_class>::splr_explicit_splat_collection(scan_class scan, 
    unsigned int num_orbit_indices, filter_3d_class * filter_3d, biob_worldpt_roster_sptr roster, unsigned start_index, unsigned period)
  : scan_(scan), num_orbit_indices_(num_orbit_indices), filter_3d_(filter_3d), roster_(roster) , start_index_(start_index), period_(period) {}

template<class T, class F, class scan_class, class filter_2d_class, class filter_3d_class, class camera_class>
void splr_explicit_splat_collection<T,F, scan_class, filter_2d_class, filter_3d_class, camera_class>::populate(orbit_index t, biob_worldpt_index pti){
  //ask filter_3d_ for the splat, and put it into the hash map
  camera_class camera = scan_(t);
  key key_to_insert(t, pti);
  //filter_3d splat method returns a composite filter,
  xmvg_composite_filter_2d<T> uncomp_filter = filter_3d_->splat(camera, roster_->point(pti));
  /////// Second attempt at compensating for the 1/r effect -- JLM
  xmvg_composite_filter_2d<T> filter;
  xmvg_ramp_compensation<T>(uncomp_filter, filter);
  ////////
  map_.insert(key_to_insert, filter);
}

template<class T, class F, class scan_class, class filter_2d_class, class filter_3d_class, class camera_class>
void splr_explicit_splat_collection<T,F, scan_class, filter_2d_class, filter_3d_class, camera_class>::forget(orbit_index t, biob_worldpt_index pti){
  map_.forget(key(t, pti));
}

template<class T, class F, class scan_class, class filter_2d_class, class filter_3d_class, class camera_class>
void splr_explicit_splat_collection<T,F, scan_class, filter_2d_class, filter_3d_class, camera_class>::populate(biob_worldpt_index pti){
  key key_to_insert(0, pti);
  if (!map_.has(key_to_insert)){
    for (orbit_index t = 0; t < num_orbit_indices_; ++t){
      populate(t, pti);
    }
  }
}

template<class T, class F, class scan_class, class filter_2d_class, class filter_3d_class, class camera_class>
void splr_explicit_splat_collection<T,F, scan_class, filter_2d_class, filter_3d_class, camera_class>::forget(biob_worldpt_index pti){
    for (orbit_index t = 0; t < num_orbit_indices_; ++t){
      forget(t, pti);
    }
}

#if 0
template<class T, class F, class scan_class, class filter_2d_class, class filter_3d_class, class camera_class>
void splr_explicit_splat_collection<T,F, scan_class, filter_2d_class, filter_3d_class, camera_class>::populate(orbit_index t){
  for (unsigned i = 0; i < roster_->num_points(); ++i){
   if (i % 100 == 0){
      vcl_cout << "(splr_explicit_splat_collection.txx) populating: point number " << i << "\n";
   }
   populate(t, biob_worldpt_index(i));
  }
}
#endif


template<class T, class F, class scan_class, class filter_2d_class, class filter_3d_class, class camera_class>
const filter_2d_class & splr_explicit_splat_collection<T,F, scan_class, filter_2d_class, filter_3d_class, camera_class>::splat(orbit_index t, biob_worldpt_index pt) const {
  key lookup_pair(t, pt);
  return map_.lookup(lookup_pair);
}                                            

typedef vcl_pair<orbit_index, biob_worldpt_index> splr_key_type;

template<class T, class F, class scan_class, class filter_2d_class, class filter_3d_class, class camera_class>
biob_worldpt_index_enumerator_sptr splr_explicit_splat_collection<T,F, scan_class, filter_2d_class, filter_3d_class, camera_class>::enumerator(biob_subset_worldpt_roster::const_which_points_t & which_points){
  return new splr_explicit_splat_collection_enumerator(static_cast<splr_populator&>(*this), which_points, start_index_, period_);
}

template<class T, class F, class scan_class, class filter_2d_class, class filter_3d_class, class camera_class>
biob_worldpt_index_enumerator_sptr splr_explicit_splat_collection<T,F, scan_class, filter_2d_class, filter_3d_class, camera_class>::enumerator(){
  vcl_cerr << "(splr_explicit_splat_collection.txx) enumerate() with no arg not implemented.\n";
  assert(false);
  return 0;
}

//Code for easy instantiation
#undef SPLR_EXPLICIT_SPLAT_COLLECTION_INSTANTIATE
#define SPLR_EXPLICIT_SPLAT_COLLECTION_INSTANTIATE(T, F) \
template class splr_explicit_splat_collection<T, F >;

#undef SPLR_EXPLICIT_SPLAT_COLLECTION_WITH_ALL_ARGS_INSTANTIATE
#define SPLR_EXPLICIT_SPLAT_COLLECTION_WITH_ALL_ARGS_INSTANTIATE(T, F, scan_class, filter_2d_class, filter_3d_class, camera_class) \
template class splr_explicit_splat_collection<T, F, scan_class, filter_2d_class, filter_3d_class, camera_class>;

#endif
