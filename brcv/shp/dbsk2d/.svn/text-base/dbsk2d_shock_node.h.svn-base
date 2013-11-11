// This is brcv/shp/dbsk2d/dbsk2d_shock_node.h
#ifndef dbsk2d_shock_node_h_
#define dbsk2d_shock_node_h_
//:
// \file
// \brief Base class for all shock nodes 
// \author Amir Tamrakar
// \date 06/08/05
//
// 
// \verbatim
//  Modifications
//   Amir Tamrakar 06/08/2005    Initial version.
// \endverbatim

#include <vcl_list.h>
#include <vbl/vbl_smart_ptr.h>

#include <dbgrl/dbgrl_vertex.h>
#include <dbsk2d/dbsk2d_base_gui_geometry.h>
#include <dbsk2d/dbsk2d_bnd_contour_sptr.h>
#include <dbsk2d/dbsk2d_shock_fragment_sptr.h>
#include <dbsk2d/dbsk2d_shock_fragment.h>

class dbsk2d_shock_edge;

//: This class stores intrinsic and extrinsic information
//  corresponding to a shock edge adjacent to this node
//
// Regular shock nodes only have one (or a pair if you consider both +- phi) 
// contact point per shock edge but degenerate shock nodes with A-inf segments
// have a range of contact points. Therefore a tangent and phi range.
//
// This also means that degenerate descriptors have shock fragments defined on them.
//
// This node is therefore of the form A1^n - A-inf^k.
//
class dbsk2d_shock_node_descriptor
{
public:

  //: Default Constructor
  dbsk2d_shock_node_descriptor():edge(0), tangent(-1), tangent2(-1), 
    phi(-1), contour(0), sEta(-1.0), eEta(-1.0), fragment(0) {}

  //: Constructor
  dbsk2d_shock_node_descriptor(vbl_smart_ptr<dbsk2d_shock_edge> sedge, 
    double tan_vec, double phi_val, double tan_vec2=0) :
  edge(sedge), tangent(tan_vec), tangent2(tan_vec2), phi(phi_val),  
  contour(0), sEta(-1.0), eEta(-1.0), fragment(0) {}

  //: Destructor
  ~dbsk2d_shock_node_descriptor() { contour=0; fragment=0; }

public:

  //: The shock edge corresponding to this parameter 
  // (Null if this is for a virtual edge corresponding to A-infinity)
  vbl_smart_ptr<dbsk2d_shock_edge> edge; 

  //: outgoing tangent from the node
  double tangent;

  //: outgoing tangent on the other side for an A-infinity node
  double tangent2;

  //: angle of the contant point wrt to the tangent
  double phi;

  //the following fields are only required if this is a virtual A-infinity edge
  dbsk2d_bnd_contour_sptr contour;    ///< boundary curve
  double sEta;                        ///< The starting arclength along the curve
  double eEta;                        ///< The ending arclength along the curve
  dbsk2d_shock_fragment_sptr fragment; ///< shock fragment formed by this edge

};


//: Base class for all shock node classes
//  This is the class that is to be used by people who
//  don't need to know the detailed workings of shock computation
class dbsk2d_shock_node : public dbsk2d_base_gui_geometry, 
                          public dbgrl_vertex<dbsk2d_shock_edge> 
{
public:
  enum shock_node_type { 
    SOURCE, SINK, A3, JUNCT, TERMINAL           
  };
 
  //: Constructor
  dbsk2d_shock_node();

  //: Destructor
  virtual ~dbsk2d_shock_node();

  //---------------------------------------
  // Casting functions
  //---------------------------------------

  //: cast to dbsk2d_shock_node
  virtual dbsk2d_shock_node* cast_to_shock_node() { return this; }
  virtual dbsk2d_shock_node const* cast_to_shock_node() const{return this;}

  //Accessing functions
  int id(){return id_;}
  void set_id(int id){id_ = id;}

  //: return the extrinsic locaiton of this point
  vgl_point_2d<double> pt() { return pt_; }
  void set_pt(vgl_point_2d<double> pt) { pt_ = pt; }

  //: return the radius
  double radius() { return radius_; }
  void set_radius(double radius) { radius_ = radius; }

  //: return ordered list of intrinsic parameters at this node
  vcl_list<dbsk2d_shock_node_descriptor>& descriptor_list() { return descriptor_list_; }

  //: form shock fragment from this edge
  virtual void form_shock_fragments();

  //: clear the shock fragment on this edge
  void clear_shock_fragment() {}

  // useful functions
  bool is_a_source() { return (in_degree()==0 && out_degree()==2); }
  bool is_a_sink() { return (in_degree()>=1 && out_degree()==0); }
  bool is_a_junction() { return (in_degree()>1 && out_degree()==1); }
  bool is_an_a3() { return (in_degree()==0 && out_degree()==1); }

  //: return the type of the node depending on its connectivity
  shock_node_type type();

  //: compute the extrinsic locus of this element for easier rendering
  virtual void compute_extrinsic_locus(){}

  //: Return some information about the element
  virtual void getInfo (vcl_ostream& ostrm=vcl_cout);

protected:

  int id_;  ///< unique id of this node
  vgl_point_2d<double> pt_; ///< The extrinsic location of this node
  double radius_; ///< The radius of the contact circle 
  
  //: ordered list of node descriptors at this node
  vcl_list<dbsk2d_shock_node_descriptor> descriptor_list_;  

};

#endif // dbsk2d_shock_node_h_
