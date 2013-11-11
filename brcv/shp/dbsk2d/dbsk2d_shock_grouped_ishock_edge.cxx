// This is brcv/shp/dbsk2d/dbsk2d_shock_grouped_ishock_edge.cxx

//:
// \file

#include <dbsk2d/dbsk2d_shock_grouped_ishock_edge.h>
#include <dbsk2d/dbsk2d_shock_ishock_node_sptr.h>
#include <dbsk2d/dbsk2d_shock_ishock_node.h>

//: Constructor
dbsk2d_shock_grouped_ishock_edge::
dbsk2d_shock_grouped_ishock_edge(dbsk2d_shock_node_sptr src_node, 
                                 dbsk2d_shock_node_sptr tgt_node,
                                 vcl_list<dbsk2d_ishock_edge*> shock_edges) : 
  dbsk2d_shock_edge(src_node, tgt_node), 
  edges_(shock_edges) 
{ 
  compute_extrinsic_locus(); 
  if (tgt_node)  //don't form visual fragments if this edge does not terminate
    form_shock_fragment();
}

//: Destructor
dbsk2d_shock_grouped_ishock_edge::~dbsk2d_shock_grouped_ishock_edge() 
{
  edges_.clear();
}

//: compute the extrinsic locus of this element for easier rendering
void dbsk2d_shock_grouped_ishock_edge::compute_extrinsic_locus()
{ 
  //traverse the list of intrinsic shock edges and collect the extrinsic points in the correct order
  dbsk2d_ishock_node* cur_node = ((dbsk2d_shock_ishock_node*)source().ptr())->ishock_node();

  for(ishock_edge_list::iterator curE = edges_.begin();
      curE != edges_.end(); 
      ++curE)
  {
    dbsk2d_ishock_edge* cur_edge = (*curE);

    if (!cur_edge) //A1-Ainf node
      continue;

    if (cur_node == cur_edge->source()){
      //going forward
      for (unsigned i=0; i<cur_edge->ex_pts().size(); i++)
        ex_pts_.push_back(cur_edge->ex_pts()[i]);

      //move to the next node
      cur_node = cur_edge->target();
    }
    else {
      //going backward
      for (unsigned i=cur_edge->ex_pts().size()-1; i>=0; i--)
        ex_pts_.push_back(cur_edge->ex_pts()[i]);

      //move to the next node
      cur_node = cur_edge->source();
    }
  }
}

//: form shock fragment from this edge
void dbsk2d_shock_grouped_ishock_edge::form_shock_fragment() 
{
  //should this be done here or in the base class??
  //) 1. instantiate the shock fragment
  fragment_ = new dbsk2d_shock_fragment(this->cast_to_shock_edge());

  //2) Compile the polygon that represents this visual fragment
  dbsk2d_ishock_node* src_inode = ((dbsk2d_shock_ishock_node*)source().ptr())->ishock_node();
  dbsk2d_ishock_node* tgt_inode = ((dbsk2d_shock_ishock_node*)target().ptr())->ishock_node();

  //2) add the first extrinsic point along the shock edge
  vgl_point_2d<double> fpt = src_inode->origin();
  fragment_->ex_pts().push_back(fpt);

  //3) go along left contour
  dbsk2d_ishock_node* cur_node = src_inode;
  for(ishock_edge_list::iterator curE = edges_.begin();
      curE != edges_.end(); 
      ++curE)
  {
    dbsk2d_ishock_edge* cur_edge = (*curE);

    if (!cur_edge) //A1-Ainf node
      continue;

    if (cur_node == cur_edge->source()){
      //going forward
      fragment_->ex_pts().push_back(cur_edge->getLFootPt(cur_edge->sTau()));

      //move to the next node
      cur_node = cur_edge->target();

      // if last edge in the list, include the end point projection too
      if (cur_node == tgt_inode)
        fragment_->ex_pts().push_back(cur_edge->getLFootPt(cur_edge->eTau()));
    }
    else {
      //going backward
      fragment_->ex_pts().push_back(cur_edge->getRFootPt(cur_edge->eTau()));

      //move to the next node
      cur_node = cur_edge->source();

      // if last edge in the list, include the end point projection too
      if (cur_node == tgt_inode)
        fragment_->ex_pts().push_back(cur_edge->getRFootPt(cur_edge->sTau()));
    }
  }

  //4) add the last extrinsic point along the shock edge
  vgl_point_2d<double> lpt = tgt_inode->origin();
  fragment_->ex_pts().push_back(lpt);

  //5) Go along the right contour
  cur_node = tgt_inode;
  for(ishock_edge_list::reverse_iterator curRE = edges_.rbegin();
      curRE != edges_.rend(); 
      ++curRE)
  {
    dbsk2d_ishock_edge* cur_edge = (*curRE);

    if (!cur_edge) //A1-Ainf node
      continue;

    if (cur_node == cur_edge->source()){
      //going forward
      fragment_->ex_pts().push_back(cur_edge->getLFootPt(cur_edge->sTau()));

      //move to the next node
      cur_node = cur_edge->target();

      // if last edge in the list, include the end point projection too
      if (cur_node == src_inode)
        fragment_->ex_pts().push_back(cur_edge->getLFootPt(cur_edge->eTau()));
    }
    else {
      //going backward
      fragment_->ex_pts().push_back(cur_edge->getRFootPt(cur_edge->eTau()));

      //move to the next node
      cur_node = cur_edge->source();

      // if last edge in the list, include the end point projection too
      if (cur_node == src_inode)
        fragment_->ex_pts().push_back(cur_edge->getRFootPt(cur_edge->sTau()));
    }
  }

}

//: return the extrinsic point on the shock
vgl_point_2d<double> dbsk2d_shock_grouped_ishock_edge::pt(double psi)
{
  return vgl_point_2d<double>(0,0);
}

//: return the radius
double dbsk2d_shock_grouped_ishock_edge::r (double psi) 
{ 
  return 0; 
}

//: return the tangent
double dbsk2d_shock_grouped_ishock_edge::tangent (double psi)
{
  return 0;
}

//: return the velocity
double dbsk2d_shock_grouped_ishock_edge::v  (double psi)
{
  return 0;
}

//: return the phi parameter
double dbsk2d_shock_grouped_ishock_edge::phi (double psi)
{
  return 0;
}

