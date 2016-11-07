#if !defined(SPLR_SYMMETRY_SPLAT_COLLECTION_H_)
#define SPLR_SYMMETRY_SPLAT_COLLECTION_H_

#include <vsl/vsl_binary_io.h>
#include <splr/splr_symmetry.h>
#include <splr/splr_splat_collection.h>
#include <biob/biob_worldpt_index_enumerator_sptr.h>

/*: \brief Uses symmetry to decide which splats need to be stored
    \author P. N. Klein

    This splat collection finds splats for all points
    represented by the symmetry object.
*/

//class splr_symmetry_splat_collection_enumerator;

template<class T,
    class filter_2d_class = xmvg_composite_filter_2d<T> >
class splr_symmetry_splat_collection : public splr_splat_collection<T, filter_2d_class> {
  //  friend class splr_symmetry_splat_collection_enumerator;
  private:
  unsigned int num_representatives_;
  //  unsigned int num_representatives(){return num_representatives_;}
  public: //should be private:
   splr_splat_collection<T, filter_2d_class>
       * sub_splat_collection_;
   splr_symmetry * symmetry_;
   
 public:
  splr_symmetry_splat_collection(splr_symmetry * symmetry,
                                 splr_splat_collection<T, filter_2d_class> * sub_splat_collection,
                                 unsigned int num_representatives);
  const filter_2d_class & splat(orbit_index t, biob_worldpt_index pt) const;
  biob_worldpt_index_enumerator_sptr enumerator(biob_subset_worldpt_roster::const_which_points_t & which_points);
  biob_worldpt_index_enumerator_sptr enumerator();
  virtual ~splr_symmetry_splat_collection(){};
};

class splr_symmetry_splat_collection_enumerator : public biob_worldpt_index_enumerator {
 private:
  typedef vcl_list<biob_worldpt_index> representees_t;// list of members of sample_points() 
  const vcl_vector<representees_t> representees_vector_; //entry i is points of sample_points() represented by representative i
  biob_worldpt_index_enumerator_sptr subsplat_enumerator_;
  representees_t::const_iterator representees_iterator_, representees_end_;
  void conditional_advance(){
    if (subsplat_enumerator_->has_next()){
      biob_worldpt_index representative_pti = subsplat_enumerator_->next();
      const representees_t & representees = representees_vector_[representative_pti.index()];
      representees_iterator_ = representees.begin();
      representees_end_ = representees.end();
      assert(representees_iterator_ != representees_end_);//otherwise, representative wouldn't have been on list
    }
  }

 public:
  biob_worldpt_index next(){
    biob_worldpt_index to_return = *representees_iterator_;
    ++representees_iterator_;
    if (representees_iterator_ == representees_end_){
      conditional_advance();
    }
    assert(!subsplat_enumerator_->has_next() || representees_iterator_ != representees_end_);
    return to_return;
  }
  bool has_next(){
    return representees_iterator_ != representees_end_ || subsplat_enumerator_->has_next();
  }
  splr_symmetry_splat_collection_enumerator(const vcl_vector<representees_t> & representees_vector,                                            
                                            biob_worldpt_index_enumerator_sptr subsplat_enumerator)
    : representees_vector_(representees_vector), subsplat_enumerator_(subsplat_enumerator) {
    conditional_advance();
  }
};


#endif
