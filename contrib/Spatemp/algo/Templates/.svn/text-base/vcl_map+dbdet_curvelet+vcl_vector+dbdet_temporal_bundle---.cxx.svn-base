#include <Spatemp/algo/dbdet_temporal_bundle.h>
#include <dbdet/sel/dbdet_curvelet.h>
#include <vcl_map.txx>

typedef vcl_vector< dbdet_temporal_bundle*> vec_dbdet_temporal_bundle_ptr;
#include "vcl_functional.h"
struct lessthan{
    bool operator()(dbdet_curvelet* c1,dbdet_curvelet* c2) const
    {
        return c1->edgel_chain.size()>c2->edgel_chain.size();
    }
};
VCL_MAP_INSTANTIATE(dbdet_curvelet*, vec_dbdet_temporal_bundle_ptr , lessthan);
