// This is Spatemp/pro/dbdet_temporal_map_storage.h
#ifndef dbdet_temporal_map_storage_h_
#define dbdet_temporal_map_storage_h_



#include <bpro1/bpro1_storage.h>
#include "dbdet_temporal_map_storage_sptr.h"

#include <vcl_vector.h>
#include <vbl/vbl_array_2d.h>

#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/sel/dbdet_curvelet_map.h>
#include <dbdet/sel/dbdet_edgel_link_graph.h>
#include <dbdet/sel/dbdet_curve_fragment_graph.h>

#include <Spatemp/algo/dbdet_temporal_map_sptr.h>
//: Storage class for various edge linking data structures
//  One idiosyncracy of this design: Since the various structures are
//  instantiated in this class, the storage class will need to be instantiated
//  before any algorithm can be called
class dbdet_temporal_map_storage : public bpro1_storage 
{
public:

  //: Constructor
  dbdet_temporal_map_storage() {}

  //: Destructor
  virtual ~dbdet_temporal_map_storage(){}

  //: Returns the type string "sel"
  virtual vcl_string type() const { return "temporalmap"; }

  //: Return IO version number;
  short version() const;

  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual bpro1_storage* clone() const;
  
  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return "dbdet_temporal_map_storage"; }

  // Data Access Functions

  void set_temporalmap(dbdet_temporal_map_sptr tmap) { tmap_ = tmap; }

    dbdet_temporal_map_sptr  get_temporalmap() { return tmap_ ; }

private:

  dbdet_temporal_map_sptr tmap_; ///< The edgemap (EM) (without this structure none of the other structures here are defined)

};


//: Create a smart-pointer to a dbdet_temporal_map_storage.
struct dbdet_temporal_map_storage_new : public dbdet_temporal_map_storage_sptr
{
  typedef dbdet_temporal_map_storage_sptr base;

  //: Constructor - creates a default dbdet_temporal_map_storage_sptr.
  dbdet_temporal_map_storage_new() : base(new dbdet_temporal_map_storage()) { }

};


#endif //dbdet_temporal_map_storage_h_
