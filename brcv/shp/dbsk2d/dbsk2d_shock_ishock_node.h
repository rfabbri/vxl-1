// This is brcv/shp/dbsk2d/dbsk2d_shock_ishock_node.h
#ifndef dbsk2d_shock_ishock_node_h_
#define dbsk2d_shock_ishock_node_h_
//:
// \file
// \brief Subclass of shock node that links to ishock_node  
// \author Amir Tamrakar
// \date 06/20/05
//
// 
// \verbatim
//  Modifications
//   Amir Tamrakar 06/20/2005    Initial version.
// \endverbatim

#include <dbsk2d/dbsk2d_shock_node.h>
#include <dbsk2d/dbsk2d_ishock_node.h>

//: Subclass of shock node that links to ishock_node
class dbsk2d_shock_ishock_node : public dbsk2d_shock_node
{
public:
  //: Default Constructor
  dbsk2d_shock_ishock_node() : dbsk2d_shock_node(), ishock_node_(0) {}

  //: Constructor
  dbsk2d_shock_ishock_node(dbsk2d_ishock_node* ishock_node);

  //: Destructor
  virtual ~dbsk2d_shock_ishock_node(){}

  //: set the intrinsic shock node
  void set_ishock_node(dbsk2d_ishock_node* ishock_node) { ishock_node_ = ishock_node; }

  //: return the intrinsic shock node
  dbsk2d_ishock_node* ishock_node() { return ishock_node_; }

  //: return the extrinsic points for rendering this geometry
  virtual vcl_vector<vgl_point_2d<double> >& ex_pts() { return ishock_node_->ex_pts(); }

protected:
  dbsk2d_ishock_node* ishock_node_;

};

#endif // dbsk2d_shock_ishock_node_h_
