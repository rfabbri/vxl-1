// This is brcv/rec/dbskr/pro/dbskr_shock_match_storage.h
#ifndef dbskr_shock_match_storage_h_
#define dbskr_shock_match_storage_h_
//:
// \file
// \brief Storage class for shock matching
// \author Ozge Can Ozcanli
// \date Sep 30 2005
//
// \verbatim
//  Modifications
//
// \endverbatim

#include <bpro1/bpro1_storage.h>
#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbskr/dbskr_tree_sptr.h>
#include <dbskr/dbskr_tree.h>
#include <dbskr/dbskr_sm_cor_sptr.h>
//#include <dbsk2d/dbsk2d_rich_map_sptr.h>
//#include <vil/vil_image_resource_sptr.h>
//#include <vil/vil_image_view.h>

#include <dbskr/pro/dbskr_shock_match_storage_sptr.h>

typedef vcl_pair<vcl_pair<int, int>, vcl_pair<int, int> > pathtable_key;

//: Storage class for dbskr_edit_distance
class dbskr_shock_match_storage : public bpro1_storage 
{
public:

  //: Constructor
  dbskr_shock_match_storage();

  //: Destructor
  virtual ~dbskr_shock_match_storage();

  virtual vcl_string type() const { return "shock_match"; }

  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual bpro1_storage* clone() const;
  
  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return "dbskr_shock_match_storage"; }

  //: set the shock matching correspondence
  void set_sm_cor(dbskr_sm_cor_sptr sm_cor) { sm_cor_ = sm_cor; }

  //: get the shock matching correspondence
  dbskr_sm_cor_sptr get_sm_cor(void) { return sm_cor_; }

private:

  dbskr_sm_cor_sptr sm_cor_;
};

//: Create a smart-pointer to a dbskr_shock_match_storage.
struct dbskr_shock_match_storage_new : public dbskr_shock_match_storage_sptr
{
  typedef dbskr_shock_match_storage_sptr base;

  //: Constructor - creates a default dbskr_shock_match_storage_sptr.
  dbskr_shock_match_storage_new() : base(new dbskr_shock_match_storage()) { }
};

#endif //dbskr_shock_match_storage_h_
