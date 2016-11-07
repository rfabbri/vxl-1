// This is pro/edge_corr_storage.h
#ifndef edge_corr_storage_h_
#define edge_corr_storage_h_

//:
// \file
// \brief Storage class for edge map correlations
// \author Amir Tamrakar
// \date 12/14/07
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_storage.h>
#include "edge_corr_storage_sptr.h"
#include <dbdet/edge/dbdet_edgemap_sptr.h>

//: 
class edge_corr_storage : public bpro1_storage {
public:

  //: Constructor
  edge_corr_storage(){}

  //: Destructor
  virtual ~edge_corr_storage(){}

  //: Returns the type string "edge_map_corr"
  virtual vcl_string type() const { return "edge_map_corr"; }

  //: Return IO version number;
  short version() const;

  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual bpro1_storage* clone() const;
  
  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return "edge_corr_storage"; }

public:

  //: edge map1 
  dbdet_edgemap_sptr edge_map1_;

  //: edge map2
  dbdet_edgemap_sptr edge_map2_;

  //: mapping between the two edgemaps
  vcl_vector<int> match1;
  vcl_vector<int> match2;

};


//: Create a smart-pointer to a edge_corr_storage.
struct edge_corr_storage_new : public edge_corr_storage_sptr
{
  typedef edge_corr_storage_sptr base;

  //: Constructor - creates a default edge_corr_storage_sptr.
  edge_corr_storage_new() : base(new edge_corr_storage()) { }

};


#endif //edge_corr_storage_h_
