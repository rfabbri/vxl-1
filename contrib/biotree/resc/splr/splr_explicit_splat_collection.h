#if !defined(SPLR_EXPLICIT_SPLAT_COLLECTION_H_)
#define SPLR_EXPLICIT_SPLAT_COLLECTION_H_

//#include <emulation/vcl_pair.h>
#include <xscan/xscan_scan.h>
#include <xmvg/xmvg_composite_filter_2d.h>
#include <xmvg/xmvg_composite_filter_3d.h>
#include <bio_defs.h>
#include <biob/biob_worldpt_roster_sptr.h>
#include <biob/biob_worldpt_index_enumerator_sptr.h>
#include <splr/splr_map.h>
#include <splr/splr_splat_collection.h>
#include <vcl_utility.h>

/*: \brief stores every splat explicitly
    \author P. N. Klein
*/

struct splr_populator {
  virtual void populate(biob_worldpt_index pti) = 0;
  virtual void forget(biob_worldpt_index pti) = 0;
  virtual ~splr_populator(){};
};

template<class T, class F, 
    class scan_class = xscan_scan,
    class filter_2d_class = xmvg_composite_filter_2d<T>,
    class filter_3d_class = xmvg_composite_filter_3d<T, F>,
    class camera_class = xmvg_perspective_camera<double> >
class splr_explicit_splat_collection : public splr_splat_collection<T, filter_2d_class>, splr_populator {

   private: //maybe needs to be public?
    typedef vcl_pair<orbit_index, biob_worldpt_index> key;
    typedef splr_map<key, filter_2d_class > map;
    map map_;
    
    scan_class scan_;
  
   unsigned int num_orbit_indices_;
   filter_3d_class * filter_3d_;
   const biob_worldpt_roster_sptr roster_;
   unsigned start_index_, period_;
   void forget(orbit_index t, biob_worldpt_index pti);
   //: calculate (using filter_3d_ and scan_) and store the splat for a specific orbit_index and worldpt
   void populate(orbit_index t, biob_worldpt_index pt);
  public:
   //: calculate and store the splat for specific worldpt and all orbit_indices
    void populate(biob_worldpt_index pt);
   void forget(biob_worldpt_index pt);
    const filter_2d_class & splat(orbit_index t, biob_worldpt_index pt) const;
    //: initialize the splat collection (does not populate it)
  splr_explicit_splat_collection(scan_class scan, unsigned int num_orbit_indices,
                                 filter_3d_class * filter,  biob_worldpt_roster_sptr roster, unsigned start_index=0, unsigned period=1);
   biob_worldpt_index_enumerator_sptr enumerator(biob_subset_worldpt_roster::const_which_points_t & which_points);
   biob_worldpt_index_enumerator_sptr enumerator();
   //added virtual destructor
   virtual ~splr_explicit_splat_collection(){}
};

    
class splr_explicit_splat_collection_enumerator : public biob_worldpt_index_enumerator {
 private:
  splr_populator & splr_populator_;
  biob_subset_worldpt_roster::const_which_points_t which_points_;
  unsigned int index_, period_;
  biob_worldpt_index oldest_remembered_pti_, second_oldest_remembered_pti_;
  unsigned int num_remembered_;
 public:
  splr_explicit_splat_collection_enumerator(splr_populator & splr_populator_arg, biob_subset_worldpt_roster::const_which_points_t & which_points,
                                            unsigned int start_index, unsigned int period)
    : splr_populator_(splr_populator_arg), which_points_(which_points), index_(start_index), period_(period), num_remembered_(0) {}

  bool has_next(){
    return index_ < which_points_.size();
  }

  /* Need to remember the splats for the last two points returned by next().
     One is not enough because of the way splr_symmetry_splat_collection works.
     Before splr_symmetry_splat_collection's next method is called,
     it asks for next() of the sub_splat_collection (e.g. explicit_splat_collection)
     in order to get the representees.
  */

  biob_worldpt_index next(){
    if (num_remembered_ >= 2){
      splr_populator_.forget(oldest_remembered_pti_);
      --num_remembered_;
    }
    //get the worldpt index
    biob_worldpt_index pti = which_points_[index_];
    index_ += period_;
    //calculate the splats for the given point
    splr_populator_.populate(pti);
    ++num_remembered_;
    oldest_remembered_pti_ = second_oldest_remembered_pti_;
    second_oldest_remembered_pti_ = pti;
    //return the point index
    return pti;
  }
};

#endif
