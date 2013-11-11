// This is brcv/shp/dbsk2d/dbsk2d_xshock_edge.cxx

//:
// \file

#include <dbsk2d/dbsk2d_xshock_edge.h>
#include <dbsk2d/dbsk2d_xshock_sample_sptr.h>
#include <dbsk2d/dbsk2d_xshock_sample.h>

//: Constructor
dbsk2d_xshock_edge::dbsk2d_xshock_edge(int id, 
                                       dbsk2d_shock_node_sptr src_node, 
                                       dbsk2d_shock_node_sptr tgt_node, 
                                       bool bIO) : 
  dbsk2d_shock_edge(src_node, tgt_node), bIO_(bIO)
{ 
  id_=id; 
  samples_.clear();
}

//: Constructor 2
dbsk2d_xshock_edge::dbsk2d_xshock_edge(int id, 
                                       dbsk2d_shock_node_sptr src_node, 
                                       dbsk2d_shock_node_sptr tgt_node,
                                       vcl_vector<dbsk2d_xshock_sample_sptr > samples, 
                                       bool bIO):
  dbsk2d_shock_edge(src_node, tgt_node), bIO_(bIO), samples_(samples) 
{ 
  id_ = id; 
}

//: Destructor
dbsk2d_xshock_edge::~dbsk2d_xshock_edge() 
{
  samples_.clear();
}

//: compute the extrinsic locus of this element for easier rendering
void dbsk2d_xshock_edge::compute_extrinsic_locus() 
{
  for(unsigned int i=0; i<samples_.size(); i++)
    ex_pts_.push_back(samples_[i]->pt);
}

//: form shock fragment from this edge
void dbsk2d_xshock_edge::form_shock_fragment() 
{
  //should this be done here or in the base class??
  //1) instantiate the shock fragment
  fragment_ = new dbsk2d_shock_fragment(this->cast_to_shock_edge());

  //2) Compile the polygon that represents this visual fragment

  //2.1) add the first extrinsic point along the shock edge
  fragment_->ex_pts().push_back(ex_pts_.front());

  //2.2) go along left contour
  //  This needs to be obtained from reconstruction (TODO!!)
  vcl_vector<dbsk2d_xshock_sample_sptr>::iterator s_itr = samples_.begin();
  for( ; s_itr != samples_.end(); ++s_itr)
  {
    dbsk2d_xshock_sample_sptr cur_sample = (*s_itr);
    fragment_->ex_pts().push_back(cur_sample->left_bnd_pt);
  }

  //2.3) add the last extrinsic point along the shock edge
  fragment_->ex_pts().push_back(ex_pts_.back());

  //2.4) Go along the right contour
  vcl_vector<dbsk2d_xshock_sample_sptr>::reverse_iterator rs_itr = samples_.rbegin();
  for( ; rs_itr != samples_.rend(); ++rs_itr)
  {
    dbsk2d_xshock_sample_sptr cur_sample = (*rs_itr);
    fragment_->ex_pts().push_back(cur_sample->right_bnd_pt);
  }
}

//: return the extrinsic point on the shock
vgl_point_2d<double> dbsk2d_xshock_edge::pt(double psi)
{
  return samples_[(int)vcl_floor(psi)]->pt;
}

//: return the radius
double dbsk2d_xshock_edge::r (double psi) 
{ 
  return samples_[(int)vcl_floor(psi)]->radius; 
}

//: return the tangent
double dbsk2d_xshock_edge::tangent (double psi)
{
  return samples_[(int)vcl_floor(psi)]->theta;
}

//: return the velocity
double dbsk2d_xshock_edge::v  (double psi)
{
  return samples_[(int)vcl_floor(psi)]->speed;
}

//: return the phi parameter
double dbsk2d_xshock_edge::phi (double psi)
{
  return vcl_acos(-1/samples_[(int)vcl_floor(psi)]->speed);
}

