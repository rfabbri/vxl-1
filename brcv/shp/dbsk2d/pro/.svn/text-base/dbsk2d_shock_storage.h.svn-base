// This is brcv/shp/dbsk2d/pro/dbsk2d_shock_storage.h
#ifndef dbsk2d_shock_storage_h_
#define dbsk2d_shock_storage_h_
//:
// \file
// \brief Storage class for various class
// \author Mark Johnson
// \date Aug 28 2003
//
// \verbatim
//  Modifications
//
//    Amir Tamrakar  06/21/05   Renamed it from dbsk2d_ishock_storage
//                              because it contains more than just the 
//                              intrinsic shock graph
//    Amir Tamrakar  07/15/05   Also added the boundary class to the 
//                              storage class because it is an integral
//                              part of the shock data structure
//    Amir Tamrakar  07/20/05   Added the rich map to the shock storage.
//
//    Amir Tamrakar  07/24/05   Added an image object into the storage class.
//                              This image is the one associated with the current
//                              geometry.
//
// \endverbatim

#include <bpro1/bpro1_storage.h>
#include <dbsk2d/dbsk2d_boundary_sptr.h>
#include <dbsk2d/dbsk2d_ishock_graph_sptr.h>
#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbsk2d/dbsk2d_rich_map_sptr.h>
#include <vil/vil_image_resource_sptr.h>

#include "dbsk2d_shock_storage_sptr.h"

//: Storage class for dbsk2d_ishock
class dbsk2d_shock_storage : public bpro1_storage 
{
public:

  //: Constructor
  dbsk2d_shock_storage();

  //: Destructor
  virtual ~dbsk2d_shock_storage();

  virtual vcl_string type() const { return "shock"; }

  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual bpro1_storage* clone() const;
  
  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return "dbsk2d_shock_storage"; }

  //: get the boundary
  dbsk2d_boundary_sptr get_boundary() { return boundary_; }
  //: set the boundary
  void set_boundary( dbsk2d_boundary_sptr new_boundary ) { boundary_ = new_boundary; }

  //: get the intrinsic shock graph
  dbsk2d_ishock_graph_sptr get_ishock_graph() { return ishock_graph_; }
  //: set the intrinsic shock graph
  void set_ishock_graph( dbsk2d_ishock_graph_sptr new_ishock_graph ) { ishock_graph_ = new_ishock_graph; }

  //: get the coarse shock graph
  dbsk2d_shock_graph_sptr get_shock_graph() { return shock_graph_; }
  //: set the coarse shock graph
  void set_shock_graph( dbsk2d_shock_graph_sptr new_shock_graph ) { shock_graph_ = new_shock_graph; }

  //: get the rich map
  dbsk2d_rich_map_sptr get_rich_map() { return rich_map_; }
  //: set the rich map
  void set_rich_map( dbsk2d_rich_map_sptr new_rich_map ) { rich_map_ = new_rich_map; }

  //: get the image
  vil_image_resource_sptr get_image() { return image_; }
  //: set the image
  void set_image( const vil_image_resource_sptr img ) { image_ = img; }
  
private:
  dbsk2d_boundary_sptr boundary_;         ///< boundary class for the contours
  dbsk2d_ishock_graph_sptr ishock_graph_; ///< intrinsic shock graph
  dbsk2d_shock_graph_sptr shock_graph_;   ///< coarse shock graph (could be extrinsic shock graph)
  dbsk2d_rich_map_sptr rich_map_;         ///< various mappings from the image grid to the geometry
  //vil_image_view< unsigned char > image_; ///< This is the image associated with the current geometry
  vil_image_resource_sptr image_; ///< This is the image associated with the current geometry
  
};

//: Create a smart-pointer to a dbsk2d_shock_storage.
struct dbsk2d_shock_storage_new : public dbsk2d_shock_storage_sptr
{
  typedef dbsk2d_shock_storage_sptr base;

  //: Constructor - creates a default dbsk2d_shock_storage_sptr.
  dbsk2d_shock_storage_new() : base(new dbsk2d_shock_storage()) { }
};

#endif //dbsk2d_shock_storage_h_
