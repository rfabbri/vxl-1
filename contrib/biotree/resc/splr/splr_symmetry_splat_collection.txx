#if !defined(SPLR_SYMMETRY_SPLAT_COLLECTION_TXX_)
#define SPLR_SYMMETRY_SPLAT_COLLECTION_TXX_

#include <splr/splr_symmetry_splat_collection.h>
#include <biob/biob_worldpt_index.h>

/*
splr_symmetry_splat_collection_enumerator::splr_symmetry_splat_collection_enumerator(biob_subset_worldpt_roster::const_which_points_t & which_points,
                                                                                     const vcl_vector<representees_t> & representees_vector, 
                                                                                     biob_worldpt_index_enumerator_sptr subsplat_enumerator)
  : representees_vector_(representees_vector), subsplat_enumerator_(subsplat_enumerator) {
  conditional_advance();
}


void splr_symmetry_splat_collection_enumerator::conditional_advance(){
  if (subsplat_enumerator_->has_next()){
    biob_worldpt_index representative_pti = subsplat_enumerator_->next();
    const representees_t & representees = representees_vector_[representative_pti.index()];
    representees_iterator_ = representees.begin();
    representees_end_ = representees.end();
    assert(representees_iterator_ != representees_end_);//otherwise, representative wouldn't have been on list
  }
}
  

biob_worldpt_index splr_symmetry_splat_collection_enumerator::next(){
  biob_worldpt_index to_return = *representees_iterator_;
  ++representees_iterator_;
  if (representees_iterator_ == representees_end_){
    conditional_advance();
  }
  assert(!subsplat_enumerator_->has_next() || representees_iterator_ != representees_end_);
}

bool splr_symmetry_splat_collection_enumerator::has_next(){
  return subsplat_enumerator_->has_next();
}
    
*/


  /* Build a roster using the symmetry.
     Build a per_orbit_index_splat_collection using that roster.
     Use the symmetry to filter requests for splats, and
     delegate the filtered requests to the per_orbit_index_splat_collection
   */

template<class T, class filter_2d_class>
splr_symmetry_splat_collection<T, filter_2d_class>::splr_symmetry_splat_collection(
           splr_symmetry * symmetry,  splr_splat_collection<T, filter_2d_class> * sub_splat_collection,
           unsigned int num_representatives)
  : num_representatives_(num_representatives),sub_splat_collection_(sub_splat_collection),symmetry_(symmetry){}

template<class T, class filter_2d_class>
const filter_2d_class & splr_symmetry_splat_collection<T, filter_2d_class>::splat(orbit_index t, biob_worldpt_index pti) const {
  //find the coset containing that element.
  unsigned int coset = pti.index() / num_representatives_;
  //find the representative within that coset
  biob_worldpt_index representative_pti(pti.index() % num_representatives_);
  splr_symmetry_struct result = symmetry_->apply(t, coset);
  return sub_splat_collection_->splat(result.representative_orbit_index_, representative_pti);
}


template<class T, class filter_2d_class>
biob_worldpt_index_enumerator_sptr splr_symmetry_splat_collection<T, filter_2d_class>::enumerator(biob_subset_worldpt_roster::const_which_points_t & which_points){
typedef vcl_list<biob_worldpt_index> representees_t;// list of members of sample_points() 
  vcl_vector<representees_t > representees_vector(num_representatives_);
  biob_subset_worldpt_roster::which_points_t which_representatives;
  for (unsigned int i = 0; i < which_points.size(); i++){
    biob_worldpt_index pti = which_points[i];
    biob_worldpt_index representative_pti(pti.index() % num_representatives_);
    representees_t & representees = representees_vector[representative_pti.index()];
    if (representees.begin() == representees.end()){//first occurence of this representative
      which_representatives.push_back(representative_pti);
    }
    representees.push_back(pti);
  }
  return new splr_symmetry_splat_collection_enumerator(representees_vector, sub_splat_collection_->enumerator(which_representatives));
}

template<class T, class filter_2d_class>
biob_worldpt_index_enumerator_sptr splr_symmetry_splat_collection<T, filter_2d_class>::enumerator(){
  vcl_cout << "(splr_symmetry_splat_collection.txx) no-args enumerator() method NOT IMPLEMENTED\n";
  assert(false);
  return 0;
}

#define SPLR_SYMMETRY_SPLAT_COLLECTION_WITH_ALL_ARGS_INSTANTIATE(T, filter_2d_class) \
template class splr_symmetry_splat_collection<T, filter_2d_class >;

#define SPLR_SYMMETRY_SPLAT_COLLECTION_INSTANTIATE(T) \
template class splr_symmetry_splat_collection<T >;



#endif
