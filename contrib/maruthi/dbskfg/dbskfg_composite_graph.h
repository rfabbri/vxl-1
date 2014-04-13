// This is brcv/shp/dbskfg/dbskfg_composite_graph.h
#ifndef dbskfg_composite_graph_h_
#define dbskfg_composite_graph_h_

//:
// \file
// \brief A class for a composite graph
//        
// \author Maruthi Narayanan ( mn@lems.brown.edu)
// \date July 07, 2010
//
// \verbatim
//  Modifications
// \endverbatim

#include <dbgrl/dbgrl_graph.h>
#include <dbskfg/dbskfg_composite_node.h>
#include <dbskfg/dbskfg_composite_link.h>
#include <dbskfg/dbskfg_composite_link_sptr.h>
#include <rsdl/rsdl_kd_tree_sptr.h>
#include <rsdl/rsdl_kd_tree.h>
#include <vcl_string.h>
#include <vnl/vnl_double_2.h>

//: 
class dbskfg_composite_graph : 
public dbgrl_graph<dbskfg_composite_node, dbskfg_composite_link>
{
public:

  // --------------------------------------------------------------------------
  // Constructors / Destructors
  // --------------------------------------------------------------------------
  //: Constructor
  dbskfg_composite_graph(bool locus = true );

  //: Copy constructor
  // Need to fix this constructor so that it also works when the argument is
  // "const dbskfg_composite_graph&"
  dbskfg_composite_graph(dbskfg_composite_graph& that);
  
  //: Destructor
  /* virtual */ ~dbskfg_composite_graph();
  
  // Get next available id
  //: increment the id counter and return new id
  unsigned int next_available_id();

  // Get current id
  unsigned int current_id(){ return next_available_id_;}

  // set curent id
  void set_current_id(unsigned int current_id){
      next_available_id_ = current_id;}

  // --------------------------------------------------------------------------
  // DATA ACCESS
  // --------------------------------------------------------------------------

  //: Return the type identifier string
  vcl_string is_a() const {return "dbskfg_composite_graph"; }

  //: Returns shock node based on id
  dbskfg_composite_node_sptr find_node(unsigned int id);

  //: Returns shock link based on id
  dbskfg_composite_link_sptr find_link(unsigned int id);

  //: Destroy maps
  void destroy_cache(){all_nodes_.clear(); all_links_.clear();}

  //: Get flag of whether locus should be constructed
  bool construct_locus(){return construct_locus_;}

  //: set outer shock kd tree
  void set_kd_tree(rsdl_kd_tree_sptr kd_tree){kd_tree_=kd_tree;}

  //: get outer shock kd tree
  rsdl_kd_tree_sptr get_kd_tree(){return kd_tree_;}

  //: set outer shock radisu
  void set_outer_shock_radius(vcl_vector<double>& radius)
  {outer_shock_radius_=radius;}

  // get nearest radius
  double get_nn_radius(vgl_point_2d<double> pt)
  {
      rsdl_point test_pt(vnl_double_2(pt.x(),pt.y()));

      vcl_vector<rsdl_point> closest_points;
      vcl_vector<int> indices;

      kd_tree_->n_nearest(test_pt,1,closest_points,indices);
      return outer_shock_radius_[indices.front()];
  }

protected:
  

private:

  unsigned int next_available_id_;

  bool construct_locus_;

  vcl_map<unsigned int,dbskfg_composite_node_sptr> all_nodes_;
  vcl_map<unsigned int,dbskfg_composite_link_sptr> all_links_;

  rsdl_kd_tree_sptr kd_tree_;

  vcl_vector<double> outer_shock_radius_;

};

#endif // brcv/shp/dbskfg/dbskfg_composite_graph.h


