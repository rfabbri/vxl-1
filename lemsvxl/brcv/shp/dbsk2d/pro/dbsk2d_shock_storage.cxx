// This is brcv/shp/dbsk2d/pro/dbsk2d_shock_storage.cxx

//:
// \file

#include "dbsk2d_shock_storage.h"
#include <dbsk2d/dbsk2d_ishock_graph.h>
#include <dbsk2d/dbsk2d_shock_graph.h>

//: Constructor
dbsk2d_shock_storage::dbsk2d_shock_storage():
  boundary_(0), ishock_graph_(0), shock_graph_(0), rich_map_(0), image_(0)
{
}

//: Destructor
dbsk2d_shock_storage::~dbsk2d_shock_storage() 
{
  //delete the rich map
  rich_map_ = 0;

  //delete the coarse shock graph first and then the 
  //intrinsic shock graph
  if (shock_graph_)
    shock_graph_->clear();
  shock_graph_= 0;

  // The ishock graph and boundary both point to each other so we have a 
  // circular reference, we need to break the link of one of them

  if ( ishock_graph_)
  {
      ishock_graph_->remove_boundary();
  }

  if(ishock_graph_)
    ishock_graph_->clear();
  ishock_graph_ = 0;

  //delete the boundary last
  boundary_ = 0;

  //delete the reference to the image
  image_ = 0;
}

//: Create a copy of the object on the heap.
// The caller is responsible for deletion
bpro1_storage* dbsk2d_shock_storage::clone() const
{
  return new dbsk2d_shock_storage(*this);
}



