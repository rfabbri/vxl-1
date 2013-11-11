#if !defined(SPLR_SUBSET_SPLAT_COLLECTION_TXX_)
#define SPLR_SUBSET_SPLAT_COLLECTION_TXX_

#include "splr_subset_splat_collection.h"

template<class T, class filter_2d_class>
splr_subset_splat_collection<T, filter_2d_class>::splr_subset_splat_collection(splr_splat_collection<T, filter_2d_class> * sub_splat_collection,
                                                                               biob_worldpt_roster_sptr sub_roster)
  : sub_splat_collection_(sub_splat_collection) {
  subset_roster_ = new biob_subset_worldpt_roster(sub_roster);
}


template<class T, class filter_2d_class>
const filter_2d_class & splr_subset_splat_collection<T, filter_2d_class>:: splat(orbit_index t, biob_worldpt_index pt) const {
  //just get the corresponding point and call splat on the sub_splat_collection
  return sub_splat_collection_->splat(t, subset_roster_->const_which_points()[pt.index()]);
}

//if no which_points is provided, want all the points comprising the subset
template<class T, class filter_2d_class>
biob_worldpt_index_enumerator_sptr splr_subset_splat_collection<T, filter_2d_class>::enumerator(){
  return new splr_subset_splat_collection_enumerator<T, filter_2d_class>(subset_roster_, sub_splat_collection_);
}

template<class T, class filter_2d_class>
biob_worldpt_index_enumerator_sptr splr_subset_splat_collection<T, filter_2d_class>::enumerator(biob_subset_worldpt_roster::const_which_points_t & which_points){
  vcl_cerr << "(splr_subset_splat_collection.txx) enumerator taking which_points argument has not been implemented.\n";
  assert(false);
  return 0;
}

#define SPLR_SUBSET_SPLAT_COLLECTION_INSTANTIATE(T) \
  template class splr_subset_splat_collection<T > ; \
  template class splr_subset_splat_collection_enumerator<T >;

#define SPLR_SUBSET_SPLAT_COLLECTION_WITH_ALL_ARGS_INSTANTIATE(T, filter_2d_class) \
  template class splr_subset_splat_collection<T, filter_2d_class > ; \
  template class splr_subset_splat_collection_enumerator<T, filter_2d_class >;

#endif
