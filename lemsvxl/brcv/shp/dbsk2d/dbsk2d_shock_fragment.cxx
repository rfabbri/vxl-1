// This is brcv/shp/dbsk2d/dbsk2d_shock_fragment.cxx

//:
// \file

#include <dbsk2d/dbsk2d_shock_fragment.h>

//: Constructor for regular shock fragments
dbsk2d_shock_fragment::dbsk2d_shock_fragment(dbsk2d_shock_edge_sptr shock_edge): 
  vbl_ref_count(), dbsk2d_base_gui_geometry(),
  id_(-1), shock_edge_(shock_edge), shock_node_(0), avg_i_(0), std_dev_i_(0)
{
}

//: Constructor for degenerate shock fragments
dbsk2d_shock_fragment::dbsk2d_shock_fragment(dbsk2d_shock_node_sptr shock_node): 
  vbl_ref_count(), dbsk2d_base_gui_geometry(),
  id_(-1), shock_edge_(0), shock_node_(shock_node), avg_i_(0), std_dev_i_(0) 
{
}

//: compute the extrinsic locus of this element for easier rendering
void dbsk2d_shock_fragment::compute_extrinsic_locus()
{
  //very very temporary
}

