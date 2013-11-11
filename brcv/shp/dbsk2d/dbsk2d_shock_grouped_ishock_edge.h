// This is brcv/shp/dbsk2d/dbsk2d_shock_grouped_ishock_edge.h
#ifndef dbsk2d_shock_grouped_ishock_edge_h_
#define dbsk2d_shock_grouped_ishock_edge_h_
//:
// \file
// \brief Shock edge formed by grouping ishock_edges
// \author Amir Tamrakar
// \date 06/20/05
//
// 
// \verbatim
//  Modifications
//   Amir Tamrakar 06/20/2005    Initial version.
// \endverbatim

#include <vcl_list.h>
#include <dbsk2d/dbsk2d_shock_edge.h>
#include <dbsk2d/dbsk2d_shock_ishock_node_sptr.h>
#include <dbsk2d/dbsk2d_ishock_edge.h>


//: Shock edge formed by grouping ishock_edges
class dbsk2d_shock_grouped_ishock_edge : public dbsk2d_shock_edge
{
public:

  //: Constructor
  dbsk2d_shock_grouped_ishock_edge(dbsk2d_shock_node_sptr src_node, 
                                   dbsk2d_shock_node_sptr tgt_node,
                                   vcl_list<dbsk2d_ishock_edge*> shock_edges);

  //: Destructor
  virtual ~dbsk2d_shock_grouped_ishock_edge();

  //: form shock fragment from this edge
  virtual void form_shock_fragment();

  //: return a reference to the intrinsic shock edges
  vcl_list<dbsk2d_ishock_edge*>& edges() { return edges_; }

  //: return the maximum intrinsic parameter
  virtual double psi_max() { return 0; }

  // functions to compute the geometry and dynamics of the shock edge
  //: return the extrinsic point on the shock
  virtual vgl_point_2d<double> pt(double psi);
  //: return the radius
  virtual double r (double psi);
  //: return the tangent
  virtual double tangent (double psi);
  //: return the velocity
  virtual double v  (double psi);
  //: return the phi parameter
  virtual double phi (double psi);

  //: compute the extrinsic locus of this element for easier rendering
  virtual void compute_extrinsic_locus();

protected:

  vcl_list<dbsk2d_ishock_edge*> edges_;

};

#endif // dbsk2d_shock_grouped_ishock_edge_h_
