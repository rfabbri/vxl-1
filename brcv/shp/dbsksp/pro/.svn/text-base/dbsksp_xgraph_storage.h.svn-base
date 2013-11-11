// This is shp/dbsksp/pro/dbsksp_xgraph_storage.h
#ifndef dbsksp_xgraph_storage_h_
#define dbsksp_xgraph_storage_h_

//:
// \file
// \brief A storage class for a dbsksp_xshock_graph
// \author Nhon Trinh, (ntrinh@lems.brown.edu)
// \date Oct 20, 2009
//
//
// \verbatim
//  Modifications
// \endverbatim

#include "dbsksp_xgraph_storage_sptr.h"

#include <bpro1/bpro1_storage.h>
#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsksp/dbsksp_xshock_graph_sptr.h>
#include <dbsksp/dbsksp_xshock_fragment_sptr.h>

#include <vil/vil_image_view_base.h>

//: Storage class for dbsksp_xgraph
class dbsksp_xgraph_storage : public bpro1_storage 
{
public:
  // Initialization-------------------------------------------------------------
  
  //: Constructor
  dbsksp_xgraph_storage();
  
  //: Destructor
  virtual ~dbsksp_xgraph_storage(){}

  // Inherited from bpro1-------------------------------------------------------
  
  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual bpro1_storage* clone() const;
  
  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const 
  { return "dbsksp_xgraph_storage"; }


  // Data access----------------------------------------------------------------

  
  // EXTRINSIC SHOCK GRAPH PROPERTIES ------------------------------------------

  //: Get the extrinsic shock graph
  dbsksp_xshock_graph_sptr xgraph() const
  { return this->xgraph_; }

  //: Set the extrinsic shock graph
  void set_xgraph(const dbsksp_xshock_graph_sptr& xgraph)
  { this->xgraph_ = xgraph; }

  //: Get the active edge
  dbsksp_xshock_edge_sptr active_xedge() const
  { return this->active_xedge_; }

  //: Set the active edge
  void set_active_xedge(const dbsksp_xshock_edge_sptr& xe)
  { this->active_xedge_ = xe; }

  //: Get the active node
  dbsksp_xshock_node_sptr active_xnode() const
  {return this->active_xnode_; }

  //: Set the active node
  void set_active_xnode(const dbsksp_xshock_node_sptr& xv)
  { this->active_xnode_ = xv; }

  //: Reference to list of fragments
  const vcl_vector<dbsksp_xshock_fragment_sptr >& xfrag_list() const 
  { return this->list_xfrag_; }

  vcl_vector<dbsksp_xshock_fragment_sptr >& xfrag_list()
  { return this->list_xfrag_; }

  //: List of xsamples
  vcl_vector<dbsksp_xshock_node_descriptor >* xsamples()
  { return &(this->xsamples_); }

  const vcl_vector<dbsksp_xshock_node_descriptor >* xsamples() const
  { return &(this->xsamples_); }


  //: Get image
  vil_image_view_base_sptr image() const
  {return this->image_; }

  //: Set image
  void set_image(const vil_image_view_base_sptr& image)
  { this->image_ = image; }

  // Binary IO -----------------------------------------------------------------
  
  //: Returns the type string "dbsksp_skgraph"
  virtual vcl_string type() const { return "dbsksp_xgraph"; }

  //: Return IO version number;
  short version() const;

  
  //: Binary save self to stream.
  // Need rewrite
  void b_write(vsl_b_ostream &os) const;
  
  //: Binary load self from stream.
  // Need rewrite
  void b_read(vsl_b_istream &is);
  
protected:
  
  //: extrinsic shock graph
  dbsksp_xshock_graph_sptr xgraph_;
  dbsksp_xshock_edge_sptr active_xedge_;
  dbsksp_xshock_node_sptr active_xnode_;

  //: List of fragments
  vcl_vector<dbsksp_xshock_fragment_sptr > list_xfrag_;

  //: List of xshock samples
  vcl_vector<dbsksp_xshock_node_descriptor > xsamples_;

  //: Image associated with this xgraph
   vil_image_view_base_sptr image_;
  
};


//------------------------------------------------------------------------------
//: Create a smart-pointer to a dbsksp_xgraph_storage.
struct dbsksp_xgraph_storage_new : public dbsksp_xgraph_storage_sptr
{
  typedef dbsksp_xgraph_storage_sptr base;

  //: Constructor - creates a dbsksp_xgraph_storage_sptr.
  dbsksp_xgraph_storage_new(): base(new dbsksp_xgraph_storage()) { }
};


#endif //dbsksp_xgraph_storage_h_
