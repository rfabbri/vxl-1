// This is brcv/shp/dbsk2d/dbsk2d_xshock_node.cxx

//:
// \file

#include <dbsk2d/dbsk2d_xshock_node.h>
#include <dbsk2d/dbsk2d_xshock_sample_sptr.h>
#include <dbsk2d/dbsk2d_xshock_sample.h>

//: Constructor
dbsk2d_xshock_node::dbsk2d_xshock_node(int id, bool bIO) : 
  dbsk2d_shock_node(), bIO_(bIO)
{ 
  id_ = id; 
  samples_.clear();
}

//: Copy Constructor - does not copy the connectivity only properties intrinsic to the node
  dbsk2d_xshock_node::dbsk2d_xshock_node(const dbsk2d_xshock_node& other) : dbsk2d_shock_node(), 
    samples_(other.samples_)
{
  id_ = other.id_;
  radius_ = other.radius_;
  pt_ = other.pt_;
  bIO_ = other.bIO_;
}

//: Constructor 2
dbsk2d_xshock_node::dbsk2d_xshock_node(int id,vcl_vector<dbsk2d_xshock_sample_sptr > samples, 
                                       bool bIO):
  dbsk2d_shock_node(), bIO_(bIO), samples_(samples) 
{ 
  id_ = id; 
  radius_ = samples_.front()->radius;
  pt_ = samples_.front()->pt;
}

//: Destructor
dbsk2d_xshock_node::~dbsk2d_xshock_node() 
{
  samples_.clear();
}

//: form shock fragment from this edge
void dbsk2d_xshock_node::form_shock_fragment() 
{
  ////should this be done here or in the base class??
  ////1) instantiate the shock fragment
  //fragment_ = new dbsk2d_shock_fragment(this->cast_to_shock_node());

  ////2) Compile the polygon that represents this visual fragment

  ////2.1) add the center of the arc sector
  //fragment_->ex_pts().push_back(pt_);

  ////2.2) go along the contour
  ////  This needs to be obtained from reconstruction (TODO!!)
  //vcl_vector<dbsk2d_xshock_sample_sptr>::iterator s_itr = samples_.begin();
  //for( ; s_itr != samples_.end(); ++s_itr)
  //{
  //  dbsk2d_xshock_sample_sptr cur_sample = (*s_itr);
  //  fragment_->ex_pts().push_back(cur_sample->left_bnd_pt);
  //}
}
