// This is file seg/dbsks/dbsks_det_desc_xgraph.cxx

//:
// \file

#include "dbsks_det_desc_xgraph.h"
#include <dbsksp/dbsksp_xshock_graph.h>
#include <vsol/vsol_box_2d.h>




// ============================================================================
// dbsks_det_desc_xgraph
// ============================================================================

// -----------------------------------------------------------------------------
//: Return bounding box of the detection
vsol_box_2d_sptr dbsks_det_desc_xgraph::
bbox()
{
  if (!this->bbox_)
    this->compute_bbox();
  return this->bbox_;
}


// -----------------------------------------------------------------------------
//: Compute bounding box from xgraph_
bool dbsks_det_desc_xgraph::
compute_bbox()
{
  if (!this->xgraph()) return false;
  this->xgraph()->update_bounding_box();
  this->bbox_ = new vsol_box_2d(*this->xgraph()->bounding_box());
  return true;
}








//==============================================================================

//: Return true if two detection are in decreasing confidence level
bool dbsks_decreasing_confidence(const dbsks_det_desc_xgraph_sptr& a, 
                                 const dbsks_det_desc_xgraph_sptr& b)
{
  return dborl_decreasing_confidence(static_cast<dborl_det_desc* >(a.ptr()), 
    static_cast<dborl_det_desc* >(b.ptr()));
}

//: Return true if two detection are in increasing confidence level
bool dbsks_increasing_confidence(const dbsks_det_desc_xgraph_sptr& a, 
                                 const dbsks_det_desc_xgraph_sptr& b)
{
  return dborl_increasing_confidence(static_cast<dborl_det_desc* >(a.ptr()), 
    static_cast<dborl_det_desc* >(b.ptr()));
}





