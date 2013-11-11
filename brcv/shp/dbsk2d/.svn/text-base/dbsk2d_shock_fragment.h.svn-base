// This is brcv/shp/dbsk2d/dbsk2d_shock_fragment.h
#ifndef dbsk2d_shock_fragment_h_
#define dbsk2d_shock_fragment_h_
//:
// \file
// \brief Shock Fragment class
// \author Amir Tamrakar
// \date 06/21/05
//
// 
// \verbatim
//  Modifications
//   Amir Tamrakar 06/21/2005    Initial version.
// \endverbatim

#include <vbl/vbl_ref_count.h>
#include <dbsk2d/dbsk2d_base_gui_geometry.h>

#include <dbsk2d/dbsk2d_shock_node_sptr.h>
#include <dbsk2d/dbsk2d_shock_node.h>
#include <dbsk2d/dbsk2d_shock_edge_sptr.h>
#include <dbsk2d/dbsk2d_shock_edge.h>


//: Shock Fragment class
//
// The extrinsic points defining this polygonal region is
// stored as ex_pts in the base_gui_geometry
class dbsk2d_shock_fragment : public vbl_ref_count, public dbsk2d_base_gui_geometry
{
public:

  //: Constructor for regular shock fragments
  dbsk2d_shock_fragment(dbsk2d_shock_edge_sptr shock_edge);

  //: Constructor for degenerate shock fragments
  dbsk2d_shock_fragment(dbsk2d_shock_node_sptr shock_node);

  //: Destructor
  virtual ~dbsk2d_shock_fragment() {}

  int id(){return id_;}
  void set_id(int id){id_ = id;}

  dbsk2d_shock_edge_sptr shock_edge() { return shock_edge_; }
  dbsk2d_shock_node_sptr shock_node() { return shock_node_; }

  //various set/get methods
  double avg_i() { return avg_i_; }
  void set_avg_i (double avg_i) { avg_i_ = avg_i; }
  double std_dev_i() { return std_dev_i_; }
  void set_std_dev_i (double stddev_i) { std_dev_i_ = stddev_i; }

  //: compute the extrinsic locus of this element for easier rendering
  virtual void compute_extrinsic_locus();

protected:

  int id_;

  dbsk2d_shock_edge_sptr  shock_edge_;    ///< 
  dbsk2d_shock_node_sptr  shock_node_;    ///< for degenerate fragments 

  //various regional properties of this fragment
  double avg_i_;
  double std_dev_i_;
};

#endif // dbsk2d_shock_fragment_h_
