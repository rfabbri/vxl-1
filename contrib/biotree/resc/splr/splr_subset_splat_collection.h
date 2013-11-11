#if !defined(SPLR_SUBSET_SPLAT_COLLECTION_H_)
#define SPLR_SUBSET_SPLAT_COLLECTION_H_

#include <xmvg/xmvg_composite_filter_2d.h>
#include "splr_splat_collection.h"
#include <biob/biob_subset_worldpt_roster_sptr.h>
#include "splr_map.h"

template <class T,
   class filter_2d_class = xmvg_composite_filter_2d<T>  >
class splr_subset_splat_collection : public splr_splat_collection<T, filter_2d_class> {
  private:
  biob_subset_worldpt_roster_sptr subset_roster_;
  splr_splat_collection<T, filter_2d_class> * sub_splat_collection_;
  public:
  splr_subset_splat_collection(splr_splat_collection<T, filter_2d_class> * sub_splat_collection,
                               biob_worldpt_roster_sptr sub_roster);
  biob_subset_worldpt_roster_sptr subset_roster(){return subset_roster_;}
  const filter_2d_class & splat(orbit_index t, biob_worldpt_index pt) const;
  biob_worldpt_index_enumerator_sptr enumerator(biob_subset_worldpt_roster::const_which_points_t & which_points);
  biob_worldpt_index_enumerator_sptr enumerator();
  virtual ~splr_subset_splat_collection(){};
};

template <class T, class filter_2d_class = xmvg_composite_filter_2d<T> >
class splr_subset_splat_collection_enumerator :  public biob_worldpt_index_enumerator {
 private:
  biob_worldpt_index_enumerator_sptr sub_splat_collection_enumerator_;
  splr_map<biob_worldpt_index, biob_worldpt_index> map_;
 public:
  splr_subset_splat_collection_enumerator(biob_subset_worldpt_roster_sptr subset_roster,
  splr_splat_collection<T, filter_2d_class> * sub_splat_collection){
    biob_subset_worldpt_roster::which_points_t & which_points_in_subroster = subset_roster->which_points();

    for (unsigned int i = 0; i < which_points_in_subroster.size(); ++i){
      biob_worldpt_index pt_in_subroster = which_points_in_subroster[i];
      map_.insert(pt_in_subroster, biob_worldpt_index(i));
    }
    sub_splat_collection_enumerator_ = sub_splat_collection->enumerator(which_points_in_subroster);
  }

  biob_worldpt_index next(){
    biob_worldpt_index next_in_subcollection = sub_splat_collection_enumerator_->next();
    //find corresponding point in this collection
    assert(map_.has(next_in_subcollection));
    return map_.lookup(next_in_subcollection);
  }

  bool has_next(){
    return sub_splat_collection_enumerator_->has_next();
  }
};

#endif
