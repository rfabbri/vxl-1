// This is shp/dbsksp/pro/dbsksp_shock_storage.h
#ifndef dbsksp_shock_storage_h_
#define dbsksp_shock_storage_h_

//:
// \file
// \brief The vidpro1 storage class for a shock
// \author Nhon Trinh, (ntrinh@lems.brown.edu)
// \date Sep 28, 2006
//
//
// \verbatim
//  Modifications
// \endverbatim

#include "dbsksp_shock_storage_sptr.h"

#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>

#include <bpro1/bpro1_storage.h>
#include <dbsksp/dbsksp_shock_graph_sptr.h>
#include <dbsksp/dbsksp_shock_edge_sptr.h>
#include <dbsksp/dbsksp_shock_node_sptr.h>
#include <dbsksp/dbsksp_shapelet_sptr.h>
#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsksp/dbsksp_xshock_graph_sptr.h>



//: Storage class for dbsksp_skgraph
class dbsksp_shock_storage : public bpro1_storage 
{
public:
  // ------------ Initialization  ------------
  //: Constructor
  dbsksp_shock_storage();
  
  //: Destructor
  virtual ~dbsksp_shock_storage(){}

  // ------------ Data access --------------------------------------------------


  // INSTRINC SHOCK GRAPH PROPERTIES ------------------------------------------
  //: Return smart pointer to the dbsksp_skgraph object
  dbsksp_shock_graph_sptr shock_graph(){ return this->shock_graph_; }

  //: set the dbsksp_skgraph object
  void set_shock_graph(const dbsksp_shock_graph_sptr& shock_graph)
  { this->shock_graph_ = shock_graph; }

  //: Get and set the active edge
  dbsksp_shock_edge_sptr active_edge(){ return this->active_edge_; }
  void set_active_edge(const dbsksp_shock_edge_sptr& e)
  { this->active_edge_ = e; }

  //: Get and set the active node
  dbsksp_shock_node_sptr active_node(){return this->active_node_; }
  void set_active_node(const dbsksp_shock_node_sptr& node)
  {this->active_node_ = node; }

  //: Get and set the active shapelet
  dbsksp_shapelet_sptr active_shapelet() const {return this->active_shapelet_; }
  void set_active_shapelet( const dbsksp_shapelet_sptr& shapelet )
  { this->active_shapelet_ = shapelet; }

  dbsksp_shock_edge_sptr edge0() const {return this->e0_ ; }
  void set_edge0(const dbsksp_shock_edge_sptr& e)
  { this->e0_ = e; }

  dbsksp_shock_edge_sptr edge1() const {return this->e1_ ; }
  void set_edge1(const dbsksp_shock_edge_sptr& e)
  { this->e1_ = e; }


  // list of shapelet
  vcl_vector<dbsksp_shapelet_sptr > shapelet_list() const 
  { return shapelet_list_; }

  void add_shapelet(const dbsksp_shapelet_sptr& s)
  {this->shapelet_list_.push_back(s); }
  
  // clear all the shapelet
  void clear_shapelet_list() 
  { this->shapelet_list_.clear(); }

  //: Get and set the target point
  vsol_point_2d_sptr target_point() const { return this->target_point_; }
  void set_target_point(const vsol_point_2d_sptr& pt)
  { this->target_point_ = pt; }

  //: Get the extrinsic shock graph
  dbsksp_xshock_graph_sptr xshock_graph() const
  { return this->xshock_graph_; }

  // EXTRINSIC SHOCK GRAPH PROPERTIES ------------------------------------------

  //: Set the extrinsic shock graph
  void set_xshock_graph(const dbsksp_xshock_graph_sptr& xshock_graph)
  { this->xshock_graph_ = xshock_graph; }

  //: Get and set the active edge
  dbsksp_xshock_edge_sptr active_xedge() { return this->active_xedge_; }
  void set_active_xedge(const dbsksp_xshock_edge_sptr& xe)
  { this->active_xedge_ = xe; }

  //: Get and set the active node
  dbsksp_xshock_node_sptr active_xnode() {return this->active_xnode_; }
  void set_active_xnode(const dbsksp_xshock_node_sptr& xv)
  {this->active_xnode_ = xv; }


  //: return reference to the vsol list
  vcl_vector<vsol_spatial_object_2d_sptr >& vsol_list()
  { return this->vsol_list_; }

  //: add vsol object
  void add_vsol_object(const vsol_spatial_object_2d_sptr& obj)
  { this->vsol_list_.push_back(obj); }

  //: Clear vsol list
  void clear_vsol_list()
  { this->vsol_list_.clear(); }


  // ------  Binary IO ----------------------
  //: Returns the type string "dbsksp_skgraph"
  virtual vcl_string type() const { return "dbsksp_shock"; }

  //: Return IO version number;
  short version() const;
  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;
  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  // ---------- bpro1 ---------------------
  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual bpro1_storage* clone() const;
  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return "dbsksp_shock_storage"; }

  
  
protected:
  //: intrinsic shock graph
  dbsksp_shock_graph_sptr shock_graph_;
  dbsksp_shock_edge_sptr active_edge_;
  dbsksp_shock_node_sptr active_node_;

  dbsksp_shock_edge_sptr e0_;
  dbsksp_shock_edge_sptr e1_;

  // active shapelet
  dbsksp_shapelet_sptr active_shapelet_;
  vcl_vector<dbsksp_shapelet_sptr > shapelet_list_;

  vsol_point_2d_sptr target_point_;

  //: extrinsic shock graph
  dbsksp_xshock_graph_sptr xshock_graph_;
  dbsksp_xshock_edge_sptr active_xedge_;
  dbsksp_xshock_node_sptr active_xnode_;

  vcl_vector<vsol_spatial_object_2d_sptr > vsol_list_;
};


//: Create a smart-pointer to a dbsksp_shock_storage.
struct dbsksp_shock_storage_new : public dbsksp_shock_storage_sptr
{
  typedef dbsksp_shock_storage_sptr base;

  //: Constructor - creates a dbsksp_shock_storage_sptr.
  dbsksp_shock_storage_new() 
    : base(new dbsksp_shock_storage()) { }
};


#endif //dbsksp_shock_storage_h_
