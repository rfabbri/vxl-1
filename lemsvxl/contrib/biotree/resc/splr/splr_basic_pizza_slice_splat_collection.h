#if !defined(SPLR_BASIC_PIZZA_SLICE_SPLAT_COLLECTION_H_)
#define SPLR_BASIC_PIZZA_SLICE_SPLAT_COLLECTION_H_

#include "splr_subset_splat_collection.h"
#include <bio_defs.h>
#include <xscan/xscan_scan.h>
#include <xmvg/xmvg_composite_filter_2d.h>
#include <biob/biob_worldpt_box.h>
#include <xscan/xscan_uniform_orbit.h>
#include "splr_pizza_slice_symmetry.h"
#include <biob/biob_explicit_worldpt_roster_sptr.h>
#include "splr_explicit_splat_collection.h"
#include "splr_subset_splat_collection.h"
#include "splr_symmetry_splat_collection.h"
#include <biob/biob_worldpt_field.h>

template <class T, class F, 
    class scan_class = xscan_scan,
    class filter_2d_class = xmvg_composite_filter_2d<T>,
    class filter_3d_class = xmvg_composite_filter_3d<T, F>,
    class camera_class = xmvg_perspective_camera<double> >
class splr_basic_pizza_slice_splat_collection {
  private: 
  splr_subset_splat_collection<T, filter_2d_class> * subset_splat_collection_;
  splr_splat_collection<T, filter_2d_class> * symmetry_splat_collection_;
  splr_explicit_splat_collection<T, F, scan_class, filter_2d_class, filter_3d_class, camera_class> * explicit_splat_collection_;
  biob_worldpt_field<vnl_quaternion<double> > rotation_field_;
  public:
  ~splr_basic_pizza_slice_splat_collection(){
    delete symmetry_splat_collection_;
    delete subset_splat_collection_;
    delete explicit_splat_collection_;
  }
splr_basic_pizza_slice_splat_collection(scan_class scan, filter_3d_class * filter,
                                        biob_worldpt_box box, double spacing,
                                        unsigned start_index=0, unsigned period=1);
  splr_subset_splat_collection<T, filter_2d_class> * collection();
  biob_worldpt_roster_sptr roster(){return *(reinterpret_cast<biob_worldpt_roster_sptr*>(&(subset_splat_collection_->subset_roster())));}
    const biob_worldpt_field<vnl_quaternion<double> > & rotation_field();
};
#endif
