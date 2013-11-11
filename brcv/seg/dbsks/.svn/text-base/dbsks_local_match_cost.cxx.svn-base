// This is file seg/dbsks/dbsks_local_match_cost.cxx

//:
// \file

#include "dbsks_local_match_cost.h"


#include <dbsksp/dbsksp_shock_graph.h>



#include <dbsksp/dbsksp_xshock_node.h>
#include <dbsksp/algo/dbsksp_interp_two_xnodes.h>
#include <dbsks/dbsks_ocm_image_cost.h>
#include <dbsks/dbsks_utils.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_matrix.h>
#include <vcl_utility.h>

#include <dbgl/algo/dbgl_biarc.h>

// ============================================================================
// dbsks_local_match_cost
// ============================================================================


//: Constructor from a graph
dbsks_local_match_cost::
dbsks_local_match_cost(const dbsksp_shock_graph_sptr& graph) :
vnl_cost_function(), fmin_(1e20)
{
  this->set_graph(graph);
}




//: Set the reference graph
void dbsks_local_match_cost::
set_graph(const dbsksp_shock_graph_sptr& graph)
{
  this->graph_ = graph;

  // Compute the number of variable in the graph
  // 5 for each degree-2 node
  // 9 for each degree-3 node
  int graph_dim = 0;

  for (dbsksp_shock_graph::vertex_iterator vit = graph->vertices_begin();
    vit != graph->vertices_end(); ++vit)
  {
    switch ((*vit)->degree())
    {
    case 1:
      graph_dim += 0;
      break;
    case 2:
      graph_dim += 5;
      break;
    case 3:
      graph_dim += 9;
      break;
    default:
      vcl_cout << "ERROR: Invalid vertex degree.\n";
      assert(false);
    }
  }

  this->set_number_of_unknowns(graph_dim);  
}


// -----------------------------------------------------------------------------
//: Convert a set of fragments for each edge to a configuration vector for the graph
vnl_vector<double > dbsks_local_match_cost::
convert_to_graph_params(
  const vcl_map<dbsksp_shock_edge_sptr, dbsksp_shapelet_sptr >& shapelet_map)
{
  
  vcl_map<dbsksp_shock_node_sptr, dbsksp_xshock_node_descriptor > xnode_map;
  for (vcl_map<dbsksp_shock_edge_sptr, dbsksp_shapelet_sptr >::const_iterator it = 
    shapelet_map.begin(); it != shapelet_map.end(); ++it)
  {
    dbsksp_shock_edge_sptr e = it ->first;
    dbsksp_shapelet_sptr s_e = it->second;
    dbsksp_shock_node_sptr v_parent = e->parent_node();

    // xnode associated with the parent node
    vgl_vector_2d<double > t_parent = s_e->tangent_start();
    dbsksp_xshock_node_descriptor xnode_parent(
      s_e->start().x(), s_e->start().y(), 
      vcl_atan2(t_parent.y(), t_parent.x()),
      s_e->phi_start(), s_e->radius_start());
    xnode_map.insert(vcl_make_pair(v_parent, xnode_parent));

    // if the child node is connected to a terminal edge, then it is not
    // the parent node in any edge (for non-branching graphs). It xnode is thus
    // determined from the ``this'' edge
    if (e->child_node()->connected_to_terminal_edge())
    {
      dbsksp_shock_node_sptr v_child= e->child_node();
      vgl_vector_2d<double > t_child = s_e->tangent_end();
      dbsksp_xshock_node_descriptor xnode_child(
        s_e->end().x(), s_e->end().y(), 
        vcl_atan2(t_child.y(), t_child.x()),
        s_e->phi_end(), s_e->radius_end());
      xnode_map.insert(vcl_make_pair(v_child, xnode_child));
    }
  }


  vnl_vector<double > x(this->get_number_of_unknowns());
  int ind = 0;
  for (dbsksp_shock_graph::vertex_iterator vit = this->graph()->vertices_begin();
    vit != this->graph()->vertices_end(); ++vit)
  {
    dbsksp_shock_node_sptr v = *vit;
    if (v->degree() == 1)
      continue;

    // can't handle graph with junction yet
    if (v->degree() != 2)
    {
      vcl_cout << "ERROR: can't handle graph with junctions yet.\n";
      assert(false);
    }

    dbsksp_xshock_node_descriptor xnode = xnode_map.find(v)->second;
    vnl_vector<double > params = dbsks_convert_to_vector(xnode);

    x.update(params, ind);
    ind += params.size();
  }

  assert(ind == x.size());

  return x;
}



// -----------------------------------------------------------------------------
//: Convert a configuration vector to a list of twoshapelets associated with the edges
void dbsks_local_match_cost::
convert_to_twoshapelet_map(const vnl_vector<double >& x, 
   vcl_map<dbsksp_shock_edge_sptr, dbsksp_twoshapelet_sptr >& twoshapelet_map)
{
  twoshapelet_map.clear();

  vcl_map<dbsksp_shock_node_sptr, dbsksp_xshock_node_descriptor > xnode_map;
  this->convert_to_xnode_map(x, xnode_map);
  
  // Compute the cost by summing the cost of all fragments
  for (dbsksp_shock_graph::edge_iterator eit = this->graph()->edges_begin();
    eit != this->graph()->edges_end(); ++eit)
  {
    dbsksp_shock_edge_sptr e = *eit;
    if (e->is_terminal_edge())
      continue;

    // Determine the geometric shape of the edge
    dbsksp_shock_node_sptr v_parent = e->parent_node();
    dbsksp_shock_node_sptr v_child = e->child_node();

    dbsksp_xshock_node_descriptor xnode_parent = xnode_map.find(v_parent)->second;
    dbsksp_xshock_node_descriptor xnode_child = xnode_map.find(v_child)->second;

    dbsksp_twoshapelet_sptr ss_e = 0;
    dbsksp_optimal_interp_two_xnodes interpolator(xnode_parent, xnode_child);
    ss_e = interpolator.optimize();

    twoshapelet_map.insert(vcl_make_pair(e, ss_e));
  }


  return;
}


// -----------------------------------------------------------------------------
//: Convert a configuration vector to a list of xnodes associated with the vertices
void dbsks_local_match_cost::
convert_to_xnode_map(const vnl_vector<double >& x,
  vcl_map<dbsksp_shock_node_sptr, dbsksp_xshock_node_descriptor >& xnode_map)
{
  // Translate components of x into a configuration for the graph
  //vcl_map<dbsksp_shock_node_sptr, dbsksp_xshock_node_descriptor > xnode_map;
  xnode_map.clear();
  int ind = 0;
  for (dbsksp_shock_graph::vertex_iterator vit = this->graph()->vertices_begin();
    vit != this->graph()->vertices_end(); ++vit)
  {
    dbsksp_shock_node_sptr v = *vit;
    if (v->degree() == 1)
      continue;

    if (v->degree() > 2)
    {
      vcl_cout << "ERROR: Can't handle degree-3 nodes.\n";
      assert(false);
    }

    // construct a xnode from the 5 numbers of `x'
    vnl_vector<double > xnode_params = x.extract(5, ind);
    ind += 5;

    dbsksp_xshock_node_descriptor xnode = dbsks_convert_to_xnode(xnode_params);
    xnode_map.insert(vcl_make_pair(v, xnode));
  }
  return;
}



// -----------------------------------------------------------------------------
//: Convert a configuration vector to a list of xnodes associated with the vertices
void dbsks_local_match_cost::
convert_to_circ_arc_list(const vnl_vector<double >& x,
                         vcl_vector<dbgl_circ_arc >& arc_list)
{
  arc_list.clear();
  vcl_map<dbsksp_shock_node_sptr, dbsksp_xshock_node_descriptor > xnode_map;
  this->convert_to_xnode_map(x, xnode_map);

  // Compute the cost by summing the cost of all fragments
  for (dbsksp_shock_graph::edge_iterator eit = this->graph()->edges_begin();
    eit != this->graph()->edges_end(); ++eit)
  {
    dbsksp_shock_edge_sptr e = *eit;
    if (e->is_terminal_edge())
      continue;

    // Determine the geometric shape of the edge
    dbsksp_shock_node_sptr v_parent = e->parent_node();
    dbsksp_shock_node_sptr v_child = e->child_node();

    dbsksp_xshock_node_descriptor xnode_parent = xnode_map.find(v_parent)->second;
    dbsksp_xshock_node_descriptor xnode_child = xnode_map.find(v_child)->second;

    // Since the biar interpolation codes are very sensity to bad input,
    // we'll be protective, avoid giving it bad inputs

    dbgl_biarc biarc_left;
    if ( biarc_left.compute_biarc_params(
      xnode_parent.bnd_pt_left(), 
      vcl_atan2(xnode_parent.bnd_tangent_left().y(), xnode_parent.bnd_tangent_left().x()),
      xnode_child.bnd_pt_left(), 
      vcl_atan2(xnode_child.bnd_tangent_left().y(), xnode_child.bnd_tangent_left().x()) ))
    {
      dbgl_circ_arc arc_l0(biarc_left.start(), biarc_left.mid_pt(), biarc_left.k1());
      dbgl_circ_arc arc_l1(biarc_left.mid_pt(), biarc_left.end(), biarc_left.k2());
      arc_list.push_back(arc_l0);
      arc_list.push_back(arc_l1);
    }

    dbgl_biarc biarc_right;
    if (biarc_right.compute_biarc_params(xnode_parent.bnd_pt_right(), 
      vcl_atan2(xnode_parent.bnd_tangent_right().y(), xnode_parent.bnd_tangent_right().x()),
      xnode_child.bnd_pt_right(), 
      vcl_atan2(xnode_child.bnd_tangent_right().y(), xnode_child.bnd_tangent_right().x()) ))
    {
      dbgl_circ_arc arc_r0(biarc_right.start(), biarc_right.mid_pt(), biarc_right.k1());
      dbgl_circ_arc arc_r1(biarc_right.mid_pt(), biarc_right.end(), biarc_right.k2());
      arc_list.push_back(arc_r0);
      arc_list.push_back(arc_r1);
    }    
  }
  return;
}












// ----------------------------------------------------------------------------
//: Cost value for each configuration of the graph
double dbsks_local_match_cost::
f(const vnl_vector<double>& x)
{

  // be protective of other functions
  if ( vnl_math_abs(x.min_value()) > 1e4 || vnl_math_abs(x.max_value()) > 1e4 )
  {
    return 1e10;
  }


  double total_len = 0;
  double total_cost = 0;

  vcl_vector<dbgl_circ_arc > arc_list;
  this->convert_to_circ_arc_list(x, arc_list);

  // When some biarc interpolation failed ...
  if (arc_list.size() != (this->get_number_of_unknowns()/5 -1) * 4)
  {
    return 2e10;
  };

  for (unsigned i =0; i < arc_list.size(); ++i)
  {
    dbgl_circ_arc arc = arc_list[i];

    // sample the two boundary curves
    double ds = this->ds_shapelet_;

    // length of the arc
    double len = arc.length();

    vcl_vector<vgl_point_2d<double > > pts;
    vcl_vector<vgl_vector_2d<double > > tangents;
    for (double s = 0; s < len; s += ds)
    {
      pts.push_back(arc.point_at(s));
      tangents.push_back(arc.tangent_at(s));
    }

    // image costs
    double norm_cost = this->oriented_chamfer()->f(pts, tangents);
    double cost = norm_cost * len;

    total_len += len;
    total_cost += cost;
  }

  double final = (total_cost - this->lambda() * total_len);

  if (final < this->fmin_)
  {
    this->fmin_ = final;
    this->xmin_ = x;
  }
  return final;
  //return (total_cost - this->lambda() * total_len);
}







// ----------------------------------------------------------------------------
//: convert a vector to an xnode
dbsksp_xshock_node_descriptor dbsks_convert_to_xnode_1(const vnl_vector<double >& x)
{
  vgl_point_2d<double > pt_left(x(0), x(1));
  vgl_point_2d<double > pt_right(x(2), x(3));
  double d = x(4);

  vgl_vector_2d<double > n = normalized(pt_left - pt_right);
  vgl_vector_2d<double > t(n.y(), -n.x());
  vgl_point_2d<double > pt = midpoint(pt_left, pt_right) + d * t;
  
  return dbsksp_xshock_node_descriptor(pt, pt_left, pt_right);
}



// ----------------------------------------------------------------------------
//: convert an xnode to a vector
vnl_vector<double > dbsks_convert_to_vector_1(const dbsksp_xshock_node_descriptor& xnode)
{
  vnl_vector<double > params(5, 0);
  params(0) = xnode.bnd_pt_left().x();
  params(1) = xnode.bnd_pt_left().y();
  params(2) = xnode.bnd_pt_right().x();
  params(3) = xnode.bnd_pt_right().y();
  params(4) = -xnode.radius_ * vcl_cos(xnode.phi_);
  return params;
}




// ----------------------------------------------------------------------------
//: convert a vector to an xnode - method 2
// parameters: (x,y) of midpoint, (x,y) shock point
// half distance between two shock point
dbsksp_xshock_node_descriptor dbsks_convert_to_xnode_2(const vnl_vector<double >& x)
{
  vgl_point_2d<double > mid_pt(x(0), x(1));
  vgl_point_2d<double > shock_pt(x(2), x(3));
  double len = x(4); // half distance between 2 boundary point

  vgl_vector_2d<double > t = normalized(shock_pt - mid_pt);
  vgl_vector_2d<double > n(-t.y(), t.x());
  
  return dbsksp_xshock_node_descriptor(shock_pt, mid_pt + len*n, mid_pt - len*n); 
}



// ----------------------------------------------------------------------------
//: convert an xnode to a vector
vnl_vector<double > dbsks_convert_to_vector_2(const dbsksp_xshock_node_descriptor& xnode)
{
  vnl_vector<double > params(5, 0);
  params(2) = xnode.pt().x();
  params(3) = xnode.pt().y();

  vgl_point_2d<double > pt_left = xnode.bnd_pt_left();
  vgl_point_2d<double > pt_right = xnode.bnd_pt_right();
  vgl_point_2d<double > mid_pt = centre(pt_left, pt_right);
  double len = xnode.radius_ * vcl_sin(xnode.phi_);

  params(0) = mid_pt.x();
  params(1) = mid_pt.y();
  params(4) = len;

  return params;
}


// ----------------------------------------------------------------------------
//: convert a vector to an xnode
dbsksp_xshock_node_descriptor dbsks_convert_to_xnode(const vnl_vector<double >& x)
{
  return dbsks_convert_to_xnode_1(x);
}



// ----------------------------------------------------------------------------
//: convert an xnode to a vector
vnl_vector<double > dbsks_convert_to_vector(const dbsksp_xshock_node_descriptor& xnode)
{
  return dbsks_convert_to_vector_1(xnode);
}


// ============================================================================
// dbsks_a12_xnode_cost
// ============================================================================



// -----------------------------------------------------------------------------
//: Set necessary internal variables for the cost function
void dbsks_a12_xnode_cost::
set(const dbsks_ocm_image_cost_sptr ocm, 
    float lambda,
    float ds,
    const dbsksp_xshock_node_descriptor& xnode_parent,
    const dbsksp_xshock_node_descriptor& xnode_child,
    const dbsksp_shapelet_sptr& ref_shapelet_parent,
    const dbsksp_shapelet_sptr& ref_shapelet_child)
{
  this->ocm_ = ocm;
  this->lambda_ = lambda;
  this->ds_ = ds;
  this->xnode_parent_ = xnode_parent;
  this->xnode_child_ = xnode_child;

  this->ref_shapelet_parent_ = ref_shapelet_parent;
  this->ref_shapelet_child_ = ref_shapelet_child;
}


// -----------------------------------------------------------------------------
//: Cost value for each configuration of the graph
double dbsks_a12_xnode_cost::
f(const vnl_vector<double>& x)
{
  if (this->ref_shapelet_child_ == 0 && this->ref_shapelet_parent_ == 0)
  {
    return this->f_regularized_using_len(x);
  }
  else
  {
    return this->f_regularized_using_shock_edit(x);
  }
}


// -----------------------------------------------------------------------------
//: Cost value for each configuration of the graph
// TODO: convert this function to use dbsks_xfrag_cost
double dbsks_a12_xnode_cost::
f_regularized_using_len(const vnl_vector<double>& x)
{
  assert(x.size() == 5);
  dbsksp_xshock_node_descriptor xnode = dbsks_convert_to_xnode(x);

  // use biarc to interpolate between the three xnodes
  dbgl_biarc biarc[4];


  // return a huge cost if interpolation fails
  if ( !biarc[0].compute_biarc_params(this->xnode_parent_.bnd_pt_left(), 
    this->xnode_parent_.bnd_tangent_left(),
    xnode.bnd_pt_left(),
    xnode.bnd_tangent_left()) ||
    
    !biarc[1].compute_biarc_params(this->xnode_parent_.bnd_pt_right(),
    this->xnode_parent_.bnd_tangent_right(),
    xnode.bnd_pt_right(),
    xnode.bnd_tangent_right()) ||
    
    !biarc[2].compute_biarc_params(xnode.bnd_pt_left(),
    xnode.bnd_tangent_left(),
    this->xnode_child_.bnd_pt_left(),
    this->xnode_child_.bnd_tangent_left()) ||

    !biarc[3].compute_biarc_params(xnode.bnd_pt_right(),
    xnode.bnd_tangent_right(),
    this->xnode_child_.bnd_pt_right(),
    this->xnode_child_.bnd_tangent_right())
    )
  {
    return 1e20;
  }

  if (!biarc[0].is_consistent() ||
    !biarc[1].is_consistent() ||
    !biarc[2].is_consistent() ||
    !biarc[3].is_consistent()
    )
  {
    return 1e20;
  }


  // Compute cost and length of each component
  double accum_len = 0;
  double accum_cost = 0;
  for (unsigned i =0; i <4; ++i)
  {
    double biarc_len = biarc[i].len();
    vcl_vector<vgl_point_2d<double > > pts;
    vcl_vector<vgl_vector_2d<double > > tangents;

    if (biarc_len < this->ds())
    {
      pts.push_back(biarc[i].point_at(biarc_len / 2));
      tangents.push_back(biarc[i].tangent_at(biarc_len / 2));
    }
    else
    {
      for (double s = this->ds() / 2; s < biarc_len; s += this->ds())
      {
        pts.push_back(biarc[i].point_at(s));
        tangents.push_back(biarc[i].tangent_at(s));
      }
    }

    accum_cost += biarc_len * this->ocm()->f(pts, tangents);
    accum_len += biarc_len;
  }

  // save for debugging purpose
  this->accum_image_cost_ = accum_cost;
  this->accum_len_ = accum_len;

  return accum_cost - this->lambda() * accum_len;
}


// -----------------------------------------------------------------------------
//: 
double dbsks_a12_xnode_cost::
f_regularized_using_shock_edit(const vnl_vector<double>& x)
{
  assert(x.size() == 5);
  dbsksp_xshock_node_descriptor xnode = dbsks_convert_to_xnode(x);

  // image costs of the parent and child fragment
  dbsks_xfrag_cost xfrag_cost;
  xfrag_cost.set(this->ocm(), this->lambda(), this->ds());

  double image_cost_parent = xfrag_cost.f_image(this->xnode_parent_, xnode);
  double image_cost_child = xfrag_cost.f_image(xnode, this->xnode_child_);

  double shock_edit_cost_parent = 
    xfrag_cost.f_shock_edit(this->xnode_parent_, xnode, this->ref_shapelet_parent_);
  double shock_edit_cost_child = 
    xfrag_cost.f_shock_edit(xnode, this->xnode_child_, this->ref_shapelet_child_);


  this->accum_image_cost_ = (image_cost_parent + image_cost_child);
  this->accum_shape_cost_ = (shock_edit_cost_parent + shock_edit_cost_child);

  return (this->accum_image_cost_ + this->lambda() * this->accum_shape_cost_);
}








// ============================================================================
// dbsks_ainfty_xnode_cost
// ============================================================================

// -----------------------------------------------------------------------------
//: Set necessary internal variables for the cost function
void dbsks_ainfty_xnode_cost::
set(const dbsks_ocm_image_cost_sptr ocm,
    float lambda,
    float ds,
    const dbsksp_xshock_node_descriptor& xnode_parent,
    const dbsksp_shapelet_sptr& ref_shapelet_parent)
{
  this->ocm_ = ocm;
  this->lambda_ = lambda;
  this->ds_ = ds;
  this->xnode_parent_ = xnode_parent;
  this->ref_shapelet_parent_ = ref_shapelet_parent;
}



// -----------------------------------------------------------------------------
//: Cost value for each configuration of the graph
double dbsks_ainfty_xnode_cost::
f(const vnl_vector<double>& x)
{
  if (this->ref_shapelet_parent_ == 0)
  {
    return this->f_regularized_using_len(x);
  }
  else
  {
    return this->f_regularized_using_shock_edit(x);
  }
}



// -----------------------------------------------------------------------------
//: regularized the energy by length of the boundary
double dbsks_ainfty_xnode_cost::
f_regularized_using_len(const vnl_vector<double>& x)
{
  assert(x.size() == 5);
  dbsksp_xshock_node_descriptor xnode = dbsks_convert_to_xnode(x);

  // use biarc to interpolate between the three xnodes
  dbgl_biarc biarc[3];

  // return a huge cost if interpolation fails
  if ( !biarc[0].compute_biarc_params(this->xnode_parent_.bnd_pt_left(), 
    this->xnode_parent_.bnd_tangent_left(),
    xnode.bnd_pt_left(),
    xnode.bnd_tangent_left()) ||
    
    !biarc[1].compute_biarc_params(this->xnode_parent_.bnd_pt_right(),
    this->xnode_parent_.bnd_tangent_right(),
    xnode.bnd_pt_right(),
    xnode.bnd_tangent_right())
    )
  {
    return 1e20;
  }

  if (!biarc[0].is_consistent() ||
    !biarc[1].is_consistent()
    )
  {
    return 1e20;
  }

  if (!biarc[2].compute_biarc_params(xnode.bnd_pt_left(), 
    xnode.bnd_tangent_left(), 
    xnode.bnd_pt_right(), 
    -xnode.bnd_tangent_right()) ||

    !biarc[2].is_consistent()
    )
  {
    vcl_cout << "ERROR: something wrong with biarc interpolation.\n";
    return 1e20;
  }

  // Compute cost and length of each component
  double accum_len = 0;
  double accum_cost = 0;
  for (unsigned i =0; i <3; ++i)
  {
    double biarc_len = biarc[i].len();
    vcl_vector<vgl_point_2d<double > > pts;
    vcl_vector<vgl_vector_2d<double > > tangents;

    if (biarc_len < this->ds())
    {
      pts.push_back(biarc[i].point_at(biarc_len / 2));
      tangents.push_back(biarc[i].tangent_at(biarc_len / 2));
    }
    else
    {
      for (double s = this->ds() / 2; s < biarc_len; s += this->ds())
      {
        pts.push_back(biarc[i].point_at(s));
        tangents.push_back(biarc[i].tangent_at(s));
      }
    }
    accum_cost += biarc_len * this->ocm()->f(pts, tangents);
    accum_len += biarc_len;
  }

  // save for debugging purpose
  this->accum_image_cost_ = accum_cost;
  this->accum_len_ = accum_len;

  return accum_cost - this->lambda() * accum_len;
}


// -----------------------------------------------------------------------------
//: regularized the energy by shock-edit distance to the reference shapelet
double dbsks_ainfty_xnode_cost::
f_regularized_using_shock_edit(const vnl_vector<double>& x)
{
  assert(x.size() == 5);
  dbsksp_xshock_node_descriptor xnode = dbsks_convert_to_xnode(x);

  dbsks_xfrag_cost xfrag_cost;
  xfrag_cost.set(this->ocm(), this->lambda(), this->ds());

  // image costs
  double image_cost_1 = xfrag_cost.f_image(this->xnode_parent_, xnode);
  double image_cost_2 = xfrag_cost.f_image(xnode);

  // shape cost
  // TODO for now ignore the effect of the A_\infty node
  double shape_cost = xfrag_cost.f_shock_edit(this->xnode_parent_, xnode,
    this->ref_shapelet_parent_);

  // sum up
  double total_cost = (image_cost_1 + image_cost_2) + 
    this->lambda() * shape_cost;


  // cache cost values
  this->accum_image_cost_ = image_cost_1 + image_cost_2;
  this->accum_shape_cost_ = shape_cost;

  return total_cost;
}





// ============================================================================
// dbsks_xfrag_cost
// ============================================================================


//: Set necessary internal variables for the cost function
void dbsks_xfrag_cost::
set(const dbsks_ocm_image_cost_sptr ocm, float lambda, float ds)
{
  this->ocm_ = ocm;
  this->lambda_ = lambda;
  this->ds_ = ds;
}


// -----------------------------------------------------------------------------
//: Cost value for each configuration of the graph
double dbsks_xfrag_cost::
f_regularized_using_len(const dbsksp_xshock_node_descriptor& xnode_parent, 
  const dbsksp_xshock_node_descriptor& xnode_child)
{
  return this->f_image(xnode_parent, xnode_child) - 
    this->lambda() * this->f_bnd_length(xnode_parent, xnode_child);
}







// -----------------------------------------------------------------------------
//: Cost value for a terminal fragment, characterized by an xnode
double dbsks_xfrag_cost::
f_regularized_using_len(const dbsksp_xshock_node_descriptor& terminal_xnode)
{
  // length
  dbgl_biarc biarc;
  if (!biarc.compute_biarc_params(terminal_xnode.bnd_pt_left(), 
    terminal_xnode.bnd_tangent_left(), 
    terminal_xnode.bnd_pt_right(), 
    -terminal_xnode.bnd_tangent_right()) ||

    !biarc.is_consistent()
    )
  {
    vcl_cout << "ERROR: something wrong with biarc interpolation.\n";
    return 1e20;
  }
  double frag_len = biarc.len();

  // image cost
  dbsks_xfrag_cost xfrag_cost;
  xfrag_cost.set(this->ocm(), this->lambda(), this->ds());
  double frag_image_cost = xfrag_cost.f_image(terminal_xnode);

  // final
  return (frag_image_cost - this->lambda() * frag_len);
}






// -----------------------------------------------------------------------------
//: Image cost for an extrinsic fragment (integrate ocm cost
// along the boundary)
double dbsks_xfrag_cost::
f_image(const dbsksp_xshock_node_descriptor& xnode_parent, 
        const dbsksp_xshock_node_descriptor& xnode_child)
{
  // use biarc to interpolate between the three xnodes
  dbgl_biarc biarc[2];

  // return a huge cost if interpolation fails
  if ( !biarc[0].compute_biarc_params(xnode_parent.bnd_pt_left(), 
    xnode_parent.bnd_tangent_left(),
    xnode_child.bnd_pt_left(),
    xnode_child.bnd_tangent_left()) ||
    
    !biarc[1].compute_biarc_params(xnode_parent.bnd_pt_right(),
    xnode_parent.bnd_tangent_right(),
    xnode_child.bnd_pt_right(),
    xnode_child.bnd_tangent_right())
    )
  {
    return 1e20;
  }

  if (!biarc[0].is_consistent() ||
    !biarc[1].is_consistent()
    )
  {
    return 1e20;
  }

  // Compute cost and length of each component
  double accum_cost = 0;
  for (unsigned i =0; i <2; ++i)
  {
    double biarc_len = biarc[i].len();
    vcl_vector<vgl_point_2d<double > > pts;
    vcl_vector<vgl_vector_2d<double > > tangents;

    if (biarc_len < this->ds())
    {
      pts.push_back(biarc[i].point_at(biarc_len / 2));
      tangents.push_back(biarc[i].tangent_at(biarc_len / 2));
    }
    else
    {
      for (double s = this->ds() / 2; s < biarc_len; s += this->ds())
      {
        pts.push_back(biarc[i].point_at(s));
        tangents.push_back(biarc[i].tangent_at(s));
      }
    }
    accum_cost += biarc_len * this->ocm()->f(pts, tangents);
  }

  return accum_cost;
}






// -----------------------------------------------------------------------------
//: Image cost for an A_infty fragment
double dbsks_xfrag_cost::
f_image(const dbsksp_xshock_node_descriptor& terminal_xnode)
{
  dbgl_biarc biarc;
  if (!biarc.compute_biarc_params(terminal_xnode.bnd_pt_left(), 
    terminal_xnode.bnd_tangent_left(), 
    terminal_xnode.bnd_pt_right(), 
    -terminal_xnode.bnd_tangent_right()) ||

    !biarc.is_consistent()
    )
  {
    vcl_cout << "ERROR: something wrong with biarc interpolation.\n";
    return 1e20;
  }


  double biarc_len = biarc.len();
  vcl_vector<vgl_point_2d<double > > pts;
  vcl_vector<vgl_vector_2d<double > > tangents;

  if (biarc_len < this->ds())
  {
    pts.push_back(biarc.point_at(biarc_len / 2));
    tangents.push_back(biarc.tangent_at(biarc_len / 2));
  }
  else
  {
    for (double s = this->ds() / 2; s < biarc_len; s += this->ds())
    {
      pts.push_back(biarc.point_at(s));
      tangents.push_back(biarc.tangent_at(s));
    }
  }
  
  double frag_cost = biarc_len * this->ocm()->f(pts, tangents);
  return frag_cost;
}

// -----------------------------------------------------------------------------
//: Boundary length of the extrinsic fragment, approximated with biarcs
double dbsks_xfrag_cost::
f_bnd_length(const dbsksp_xshock_node_descriptor& xnode_parent, 
           const dbsksp_xshock_node_descriptor& xnode_child)
{
  // use biarc to interpolate between the three xnodes
  dbgl_biarc biarc[2];

  // return a huge cost if interpolation fails
  if ( !biarc[0].compute_biarc_params(xnode_parent.bnd_pt_left(), 
    xnode_parent.bnd_tangent_left(),
    xnode_child.bnd_pt_left(),
    xnode_child.bnd_tangent_left()) ||
    
    !biarc[1].compute_biarc_params(xnode_parent.bnd_pt_right(),
    xnode_parent.bnd_tangent_right(),
    xnode_child.bnd_pt_right(),
    xnode_child.bnd_tangent_right())
    )
  {
    return 1e20;
  }

  if (!biarc[0].is_consistent() ||
    !biarc[1].is_consistent()
    )
  {
    return 1e20;
  }

  // Compute cost and length of each component
  double accum_len = 0;
  for (unsigned i =0; i <2; ++i)
  {
    accum_len += biarc[i].len();
  }

  return accum_len;
}



// -----------------------------------------------------------------------------
double dbsks_xfrag_cost::
f_shock_edit(const dbsksp_xshock_node_descriptor& xnode_parent, 
             const dbsksp_xshock_node_descriptor& xnode_child,
             const dbsksp_shapelet_sptr& s_ref)
{
  // first thing first - LEGALITY CHECK
  if (!dbsks_is_legal_xfrag(xnode_parent, xnode_child))
    return 1e20;

  // use biarc to interpolate between the three xnodes
  dbgl_biarc biarc[2];

  // return a huge cost if interpolation fails
  if ( !biarc[0].compute_biarc_params(xnode_parent.bnd_pt_left(), 
    xnode_parent.bnd_tangent_left(),
    xnode_child.bnd_pt_left(),
    xnode_child.bnd_tangent_left()) ||
    
    !biarc[1].compute_biarc_params(xnode_parent.bnd_pt_right(),
    xnode_parent.bnd_tangent_right(),
    xnode_child.bnd_pt_right(),
    xnode_child.bnd_tangent_right())
    )
  {
    return 1e20;
  }

  if (!biarc[0].is_consistent() ||
    !biarc[1].is_consistent()
    )
  {
    return 1e20;
  }

  

  dbgl_circ_arc arc_left_ref = s_ref->bnd_arc_left();
  dbgl_circ_arc arc_right_ref = s_ref->bnd_arc_right();


  dbgl_biarc biarc_left_target = biarc[0];
  dbgl_biarc biarc_right_target = biarc[1];


  // Components of shock-edit deformation
  // 1. boundary length difference
  double len_diff_left = vnl_math_abs(arc_left_ref.length() - biarc_left_target.len());
  double len_diff_right = vnl_math_abs(arc_right_ref.length() - biarc_right_target.len());
  double len_diff = len_diff_left + len_diff_right;



  // 2. bending angle difference
  double bending_diff_left = vnl_math_abs(arc_left_ref.length()*arc_left_ref.k() -
    signed_angle(xnode_parent.bnd_tangent_left(), xnode_child.bnd_tangent_left()));

  double bending_diff_right = vnl_math_abs(arc_right_ref.length()*arc_right_ref.k() -
    signed_angle(xnode_parent.bnd_tangent_right(), xnode_child.bnd_tangent_right()));

  double bending_diff = bending_diff_left + bending_diff_right;

  // 3. starting radius difference
  double start_radius_diff = vnl_math_abs(s_ref->radius_start() - xnode_parent.radius_);

  // 4. radius increment difference
  double radius_increment_diff = vnl_math_abs(
    s_ref->radius_increment() - (xnode_child.radius_ - xnode_parent.radius_) );

  // 5. phiA difference
  double start_phi_diff = vnl_math_abs(
    s_ref->phi_start() - xnode_parent.phi_);


  // 6. phi increment difference
  double phi_increment_diff = vnl_math_abs(
    (s_ref->phi_end() - s_ref->phi_start()) - 
    (xnode_child.phi_ - xnode_parent.phi_) 
    );

  // adjust R to be proportional to the size of the fragment
  double average_length = (arc_left_ref.length() + arc_right_ref.length() + 
    biarc_left_target.len() + biarc_right_target.len() ) / 2;

  // weighing coefficient (experience from shock matching, with shape size 120)
  double R = 6;

  R = (6.0/120) * average_length/3;
  
  // Combining everything together
  double total_deform_cost = len_diff + R*bending_diff + 2*start_radius_diff + 
    2 * radius_increment_diff + 2 * R * start_phi_diff + 2*R * phi_increment_diff;

  return total_deform_cost;
}





