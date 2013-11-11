/*
 * dbdet_third_order_3d_edge_storage.h
 *
 *  Created on: Oct 16, 2011
 *      Author: firat
 */

#ifndef DBDET_THIRD_ORDER_3D_EDGE_STORAGE_H_
#define DBDET_THIRD_ORDER_3D_EDGE_STORAGE_H_

//:
// \file
// \brief A storage class for vil3d image view
// \author Firat Kalaycilar, (firat_kalaycilar@brown.edu)
// \date Oct 11, 2011
//
//
// \verbatim
//  Modifications
// \endverbatim

#include "dbdet_third_order_3d_edge_storage_sptr.h"
#include "../algo/dbdet_third_order_3d_edge_detector.h"

#include <bpro1/bpro1_storage.h>


//: Storage class for dbsksp_xgraph
class dbdet_third_order_3d_edge_storage : public bpro1_storage
{
public:
  // Initialization-------------------------------------------------------------


  //: Destructor
  virtual ~dbdet_third_order_3d_edge_storage(){}

  // Inherited from bpro1-------------------------------------------------------

  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual bpro1_storage* clone() const;

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const
  { return "dbdet_third_order_3d_edge_storage"; }


  // Data access----------------------------------------------------------------
  //: Get image
  vcl_vector<dbdet_3d_edge_sptr>& edgemap()
  {return this->edgemap_; }

  //: Set image
  void set_edgemap(vcl_vector<dbdet_3d_edge_sptr>& edg)
  { this->edgemap_ = edg;}

  // Binary IO -----------------------------------------------------------------

  //: Returns the type string
  virtual vcl_string type() const { return "3d_edges"; }

  //: Return IO version number;
  short version() const;

  //: Binary save self to stream.
  // Need rewrite
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  // Need rewrite
  void b_read(vsl_b_istream &is);

protected:
  //: 3d Image
  vcl_vector<dbdet_3d_edge_sptr> edgemap_;

};


//------------------------------------------------------------------------------
//: Create a smart-pointer to a dbdet_third_order_3d_edge_storage.
struct dbdet_third_order_3d_edge_storage_new : public dbdet_third_order_3d_edge_storage_sptr
{
  typedef dbdet_third_order_3d_edge_storage_sptr base;

  //: Constructor - creates a dbdet_third_order_3d_edge_storage_sptr.
  dbdet_third_order_3d_edge_storage_new(): base(new dbdet_third_order_3d_edge_storage()) { }
};



#endif /* DBDET_THIRD_ORDER_3D_EDGE_STORAGE_H_ */
