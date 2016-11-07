// This is shp/dbsksp/pro/dbsksp_average_xgraph_storage.h
#ifndef dbsksp_average_xgraph_storage_h_
#define dbsksp_average_xgraph_storage_h_

//:
// \file
// \brief A storage class to store an averaged dbsksp_xshock_graph
// \author Nhon Trinh, (ntrinh@lems.brown.edu)
// \date Mar 2, 2010
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <dbsksp/pro/dbsksp_average_xgraph_storage_sptr.h>
#include <dbsksp/pro/dbsksp_xgraph_storage.h>

//: Storage class for dbsksp_xgraph
class dbsksp_average_xgraph_storage : public dbsksp_xgraph_storage
{
public:
  //: Constructor
  dbsksp_average_xgraph_storage();
  
  //: Destructor
  virtual ~dbsksp_average_xgraph_storage(){}

  // Inherited from bpro1-------------------------------------------------------
  
  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual bpro1_storage* clone() const;
  
  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const 
  { return "dbsksp_average_xgraph_storage"; }


  // Data access----------------------------------------------------------------

  //: Get distance between parent shapes
  double distance_btw_parents() const
  {
    return this->distance_btw_parents_;
  }
  
  //: Set distance between parent shapes
  void set_distance_btw_parents(double distance)
  {
    this->distance_btw_parents_ = distance;
  }


  //: Get distance to parent shapes
  double distance_to_parent(unsigned index) const
  {
    return this->distance_to_parent_[index];
  }


  //: Set distance from `this' average xgraph to parent xgraph
  void set_distance_to_parent(unsigned index, double distance)
  {
    this->distance_to_parent_[index] = distance;
  }


  //: Get parent xgraph
  dbsksp_xshock_graph_sptr parent_xgraph(int index) const
  {
    return this->parent_xgraph_[index];
  }

  //: Set parent xgraph
  void set_parent_xgraph(int index, const dbsksp_xshock_graph_sptr& xgraph)
  {
    this->parent_xgraph_[index] = xgraph;
  }


  // Binary IO -----------------------------------------------------------------
  
  //: Returns the type string "dbsksp_skgraph"
  virtual vcl_string type() const { return "dbsksp_average_xgraph"; }

  //: Return IO version number;
  short version() const;

  
  //: Binary save self to stream.
  // Need rewrite
  void b_write(vsl_b_ostream &os) const;
  
  //: Binary load self from stream.
  // Need rewrite
  void b_read(vsl_b_istream &is);
  
protected:
  dbsksp_xshock_graph_sptr parent_xgraph_[2];
  double distance_to_parent_[2];
  double distance_btw_parents_;
};


//------------------------------------------------------------------------------
//: Create a smart-pointer to a dbsksp_average_xgraph_storage.
struct dbsksp_average_xgraph_storage_new : public dbsksp_average_xgraph_storage_sptr
{
  typedef dbsksp_average_xgraph_storage_sptr base;

  //: Constructor - creates a dbsksp_average_xgraph_storage_sptr.
  dbsksp_average_xgraph_storage_new(): base(new dbsksp_average_xgraph_storage()) { }
};


#endif //dbsksp_average_xgraph_storage_h_
