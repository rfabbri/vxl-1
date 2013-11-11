// This is brcv/shp/dbsk2d/dbsk2d_shock_edge.h
#ifndef dbsk2d_shock_edge_h_
#define dbsk2d_shock_edge_h_
//:
// \file
// \brief Base class for all shock edges
// \author Amir Tamrakar
// \date 06/08/05
//
// 
// \verbatim
//  Modifications
//   Amir Tamrakar 06/08/2005    Initial version.
// \endverbatim

#include <dbgrl/dbgrl_edge.h>
#include <dbsk2d/dbsk2d_base_gui_geometry.h>

#include <dbsk2d/dbsk2d_shock_node.h>
#include <dbsk2d/dbsk2d_shock_node_sptr.h>

#include <dbsk2d/dbsk2d_bnd_contour.h>
#include <dbsk2d/dbsk2d_shock_fragment.h>
#include <dbsk2d/dbsk2d_shock_fragment_sptr.h>

//: Base class for all shock edge classes
class dbsk2d_shock_edge : public dbsk2d_base_gui_geometry, public dbgrl_edge<dbsk2d_shock_node> 
{
public:

  //: Constructor 
  dbsk2d_shock_edge(dbsk2d_shock_node_sptr src_node, dbsk2d_shock_node_sptr tgt_node);

  //: Destructor
  virtual ~dbsk2d_shock_edge() {}
  
  //---------------------------------------
  // Casting functions
  //---------------------------------------

  //: cast to dbsk2d_shock_node
  virtual dbsk2d_shock_edge* cast_to_shock_edge() { return this; }
  virtual dbsk2d_shock_edge const* cast_to_shock_edge() const{return this;}

  // member variable acessing functions
  int id(){return id_;}
  void set_id(int id){id_ = id;}

  //: return the left contour
  dbsk2d_bnd_contour_sptr left_contour() { return left_contour_; }

  //: set the left contour
  void set_left_contour(dbsk2d_bnd_contour_sptr contour, double lseta, double leeta);

  //: return the right contour
  dbsk2d_bnd_contour_sptr right_contour() { return right_contour_; }

  //: set the right contour
  void set_right_contour(dbsk2d_bnd_contour_sptr contour, double rseta, double reeta);

  double LsEta() { return LsEta_; }
  double LeEta() { return LeEta_; }
  double RsEta() { return RsEta_; }
  double ReEta() { return ReEta_; }

  //: return the shock fragment formed by this edge
  dbsk2d_shock_fragment_sptr shock_fragment() { return fragment_; }

  //: form shock fragment from this edge
  virtual void form_shock_fragment() {}

  //: clear the shock fragment on this edge
  void clear_shock_fragment() { fragment_ = 0; }

  //: return the maximum intrinsic parameter
  virtual double psi_max() { return 0; }

  //: map from intrinsic to extrinsic coordinates
  vgl_point_2d<double> get_ex_coords(double psi, double t);

  // functions to compute the geometry and dynamics of the shock edge
  //: return the extrinsic point on the shock
  virtual vgl_point_2d<double> pt(double psi) { return vgl_point_2d<double>(0,0); }
  //: return the radius
  virtual double r (double psi) { return 0; }
  //: return the tangent vector
  virtual double tangent (double psi) { return 0; }
  //: return the velocity
  virtual double v  (double psi) { return 0; }
  //: return the phi parameter
  virtual double phi (double psi) { return 0; }
  
  //: compute the extrinsic locus of this element for easier rendering
  virtual void compute_extrinsic_locus(){}

  //: Return some information about the element
  virtual void getInfo (vcl_ostream& ostrm=vcl_cout);

protected:

  int id_;  ///< unique id of this edge

  dbsk2d_bnd_contour_sptr left_contour_;    ///< the left boundary curve
  dbsk2d_bnd_contour_sptr right_contour_;   ///< the right boundary curve

  double LsEta_;  ///< The starting arclength along the left curve
  double LeEta_;  ///< The ending arclength along the left curve
  double RsEta_;  ///< The starting arclength along the right curve
  double ReEta_;  ///< The ending arclength along the right curve

  dbsk2d_shock_fragment_sptr fragment_;  ///< shock fragment formed by this edge

};

#endif // dbsk2d_shock_edge_h_
