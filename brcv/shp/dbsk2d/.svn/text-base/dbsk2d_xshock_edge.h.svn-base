// This is brcv/shp/dbsk2d/dbsk2d_xshock_edge.h
#ifndef dbsk2d_xshock_edge_h_
#define dbsk2d_xshock_edge_h_
//:
// \file
// \brief Extrinsic shock graph edge
// \author Amir Tamrakar
// \date 02/15/05
//
// 
// \verbatim
//  Modifications
//   Amir Tamrakar 02/02/2005    Initial version. Conversion to VXL standard.
//   Amir Tamrakar 02/20/2005    Subclassed it from dbgrl_edge since the
//                               shock graph was subclassed from dbgrl_graph
//   Amir Tamrakar 06/27/06      Subclassed it from the shock_edge because
//                               an extrinsic edge is a specific type of shock edge.
//
//   Anil Usumezbas 01/10/10     Added a method to change the samples
//                               Added a method to clear ex_pts
//
// \endverbatim

#include <vcl_vector.h>
#include <dbsk2d/dbsk2d_shock_edge.h>
#include <dbsk2d/dbsk2d_shock_node_sptr.h>
#include <dbsk2d/dbsk2d_xshock_sample_sptr.h>
#include <dbsk2d/dbsk2d_xshock_sample.h>

//: Extrinsic shock graph edge
class dbsk2d_xshock_edge : public dbsk2d_shock_edge
{
public:

  //: Constructor
  dbsk2d_xshock_edge(int id, dbsk2d_shock_node_sptr src_node, dbsk2d_shock_node_sptr tgt_node, bool bIO=true);

  //: Constructor 2
  dbsk2d_xshock_edge(int id, dbsk2d_shock_node_sptr src_node, dbsk2d_shock_node_sptr tgt_node,
                     vcl_vector<dbsk2d_xshock_sample_sptr > samples, bool bIO=true);

  //: Destructor
  virtual ~dbsk2d_xshock_edge();

  //access functions
  bool is_inside_shock() { return bIO_; }
  void set_bIO(bool state) { bIO_ = state; }

  //: return the number of samples in this edge
  int num_samples() { return samples_.size(); }

  //: return sample by index
  dbsk2d_xshock_sample_sptr sample(int index) { return samples_[index]; }

  //: change sample by index
  void set_sample(int index, dbsk2d_xshock_sample_sptr sample) {samples_[index] = sample; }

  //: add a sample to this edge
  void push_back(dbsk2d_xshock_sample_sptr sample) { samples_.push_back(sample); }

  //: first sample along the edge (for use with nodes)
  dbsk2d_xshock_sample_sptr first_sample() { return samples_.front(); }

  //: last sample along the edge (for use with nodes)
  dbsk2d_xshock_sample_sptr last_sample() { return samples_.back(); }

  //: clear the extrinsic points
  void clear_ex_pts() {ex_pts_.clear(); }

  //: form shock fragment from this edge
  virtual void form_shock_fragment();

  //: return the maximum intrinsic parameter
  virtual double psi_max() { return (double)(samples_.size()-1); }

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
  bool bIO_;   //inside=true, outside=false

  //: ordered list of samples along the shock edge
  vcl_vector<dbsk2d_xshock_sample_sptr > samples_;

};

#endif //dbsk2d_xshock_edge_h_
