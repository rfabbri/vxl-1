#if !defined(SPLR_SPLAT_COLLECTION_H_)
#define SPLR_SPLAT_COLLECTION_H_

#include <bio_defs.h>
#include <biob/biob_worldpt_index.h>
//#include <xmvg/xmvg_atomic_filter_2d.h>
#include <xmvg/xmvg_composite_filter_2d.h>
#include <biob/biob_subset_worldpt_roster.h>
#include <biob/biob_worldpt_index_enumerator_sptr.h>
/*: \brief interface for a collection of splats
    \author P. N. Klein
*/

template <class T,
   class filter_2d_class = xmvg_composite_filter_2d<T>  >
class splr_splat_collection {
  public:
   //: retrieve a splat for a particular orbit_index and point
    virtual const filter_2d_class & splat(orbit_index t, biob_worldpt_index pt) const = 0;
  virtual biob_worldpt_index_enumerator_sptr enumerator() = 0;
  virtual biob_worldpt_index_enumerator_sptr enumerator(biob_subset_worldpt_roster::const_which_points_t & which_points) = 0;
  virtual ~splr_splat_collection(){};
};
#endif
