// This is brcv/shp/dbsk2d/dbsk2d_xshock_node.h
#ifndef dbsk2d_xshock_node_h_
#define dbsk2d_xshock_node_h_
//:
// \file
// \brief Extrinsic shock graph edge
// \author Amir Tamrakar
// \date 02/15/05
//
// 
// \verbatim
//  Modifications
//   Amir Tamrakar 07/12/2005    Created this class because the A-infinity 
//                               nodes that were recently added need to be sampled
//                               separately from the edges. Therefore generic
//                               shock_nodes are no longer enough to represent them
//
// \endverbatim

#include <vcl_vector.h>
#include <dbsk2d/dbsk2d_shock_node.h>
#include <dbsk2d/dbsk2d_shock_node_sptr.h>
#include <dbsk2d/dbsk2d_xshock_sample_sptr.h>
#include <dbsk2d/dbsk2d_xshock_sample.h>

//: Extrinsic shock graph edge
class dbsk2d_xshock_node : public dbsk2d_shock_node
{
public:

  //: Constructor
  dbsk2d_xshock_node(int id, bool bIO=true);

  //: Copy Constructor - does not copy the connectivity only properties intrinsic to the node
  dbsk2d_xshock_node(const dbsk2d_xshock_node& other);

  //: Constructor 2
  dbsk2d_xshock_node(int id, vcl_vector<dbsk2d_xshock_sample_sptr > samples, bool bIO=true);

  //: Destructor
  virtual ~dbsk2d_xshock_node();

  //access functions
  bool is_inside_shock() { return bIO_; }
  void set_bIO(bool state) { bIO_ = state; }

  //: return the number of samples on this node
  int num_samples() { return samples_.size(); }

  //: return sample by index
  dbsk2d_xshock_sample_sptr sample(int index) { return samples_[index]; }

  //: add a sample to this node
  void push_back(dbsk2d_xshock_sample_sptr sample) { samples_.push_back(sample); }

  //: first sample along the edge (for use with nodes)
  dbsk2d_xshock_sample_sptr first_sample() { return samples_.front(); }

  //: last sample along the edge (for use with nodes)
  dbsk2d_xshock_sample_sptr last_sample() { return samples_.back(); }

  //: form shock fragment from this edge
  virtual void form_shock_fragment();

protected:
  bool bIO_;   //inside=true, outside=false

  //: ordered list of samples on the shock node
  vcl_vector<dbsk2d_xshock_sample_sptr > samples_;

};

#endif //dbsk2d_xshock_node_h_
