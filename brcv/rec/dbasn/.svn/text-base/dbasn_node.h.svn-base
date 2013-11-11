//---------------------------------------------------------------------
// This is brcv/rec/dbasn/dbasn_node.h
//:
// \file
// \brief a simple graph node structure
//
// \author
//  O.C. Ozcanli - January 21, 2004
//  MingChing Chang
//
// \verbatim
//  Modifications
//   modified class to hold tracked curves, January 30, 2004.
// \endverbatim
//
//-------------------------------------------------------------------------

#ifndef dbasn_node_h_
#define dbasn_node_h_

#include <vcl_vector.h>
#include <vcl_utility.h>
#include <vcl_map.h>
#include <vcl_cassert.h>

#include <vgl/vgl_point_3d.h>

//: A node structure for simple graphs
class dbasn_node
{
protected:
  int           nid_;             // node id for dbasn_gradasgn
  float         cost_;            // node cost
  
  //: To improve efficiency. Assume the connectivity is sparse.
  vcl_vector<int> conn_nids_;   

public:
  //####### Constructor/Destructor #######
  dbasn_node() {
    nid_          = 0;
    cost_         = 0.0;
  }
  dbasn_node (const int nid, const float cost) {
    nid_          = nid;
    cost_         = cost;
  }
  virtual ~dbasn_node () {
    conn_nids_.clear();
  }

  //####### Data access functions ####### 
  const float cost() const { 
    return cost_; 
  }
  void set_cost (const float c) { 
    cost_ = c; 
  }
  const int nid() const { 
    return nid_; 
  }
  void set_nid (const int i) {
    nid_ = i;
  }

  //access connected nodes.
  int n_conn_nids () const {
    return conn_nids_.size();
  }
  vcl_vector<int>& conn_nids() {
    return conn_nids_;
  }
  int conn_nids (const unsigned int i) const {
    return conn_nids_[i];
  }
  void _add_conn_nid (int id) {
    conn_nids_.push_back (id);
  }
};

//###################################################################

//Here is the customiable node types (mimic the shock types).
#define AN_TYPE                 char
#define AN_TYPE_BOGUS           '?'
#define AN_TYPE_RIB_END         '3'   //A1A3
#define AN_TYPE_AXIAL_END       '4'   //A14
#define AN_TYPE_DEGE_AXIAL_END  'D'   //Degenerate A1N (N>4)
#define AN_TYPE_DEGE_RIB_END    'E'   //Degenerate A1MA3 (M>1) or (A1A3)_k or A1A5.
#define AN_TYPE_LOOP_END        'L'   //Loop End

//: The augmented node with additional info.
class dbasn_node_aug : public dbasn_node
{
protected:
  AN_TYPE   type_;
  int       n_rib_;
  int       n_axial_;
  int       n_dege_;
  vgl_point_3d<double> pt_;

  float     grad_r_max_;
  float     grad_r_min_;
  float     corner_a_max_;
  float     corner_a_min_;

public:
  //###### Constructor/Destructor ######
  dbasn_node_aug () : dbasn_node () {
    type_ = AN_TYPE_BOGUS;
    n_rib_ = 0;
    n_axial_ = 0;
    n_dege_ = 0;
    grad_r_max_ = 0;
    grad_r_min_ = 0;
    corner_a_max_ = 0;
    corner_a_min_ = 0;
  }
  dbasn_node_aug (const int id, const float cost, const char type) 
    : dbasn_node (id, cost) {
    type_ = type;
    n_rib_ = 0;
    n_axial_ = 0;
    n_dege_ = 0;
    grad_r_max_ = 0;
    grad_r_min_ = 0;
    corner_a_max_ = 0;
    corner_a_min_ = 0;
  }
  dbasn_node_aug (const int id, const float cost, const char type,
                  const int n_rib, const int n_axial, const int n_dege,
                  vgl_point_3d<double> pt,
                  const float grad_r_max, const float grad_r_min,
                  const float corner_a_max, const float corner_a_min) 
    : dbasn_node (id, cost) {
    type_ = type;
    n_rib_ = n_rib;
    n_axial_ = n_axial;
    n_dege_ = n_dege;
    pt_ = pt;
    grad_r_max_ = grad_r_max;
    grad_r_min_ = grad_r_min;
    corner_a_max_ = corner_a_max;
    corner_a_min_ = corner_a_min;
  }

  virtual ~dbasn_node_aug () {
  }

  //###### Data access functions ######
  const AN_TYPE type () const {
    return type_;
  }
  void set_type (const AN_TYPE type) {    
    assert (type == AN_TYPE_RIB_END || type == AN_TYPE_AXIAL_END || 
            type == AN_TYPE_DEGE_RIB_END || type == AN_TYPE_DEGE_AXIAL_END || 
            type == AN_TYPE_LOOP_END);
    type_ = type;
  }
  int n_rib () const {
    return n_rib_;
  }
  int n_axial () const {
    return n_axial_;
  }
  int n_dege () const {
    return n_dege_;
  }
  vgl_point_3d<double> pt() const {
    return pt_;
  }
  void set_pt (vgl_point_3d<double>& pt) {
    pt_ = pt;
  }
  
  float grad_r_max () {
    return grad_r_max_;
  }
  void set_grad_r_max (const float max) {
    grad_r_max_ = max;
  }
  float grad_r_min () {
    return grad_r_min_;
  }
  void set_grad_r_min (const float min) {
    grad_r_min_ = min;
  }
  float corner_a_max () {
    return corner_a_max_;
  }
  void set_corner_a_max (const float max) {
    corner_a_max_ = max;
  }
  float corner_a_min () {
    return corner_a_min_;
  }
  void set_corner_a_min (const float min) {
    corner_a_min_ = min;
  }
};

#endif
