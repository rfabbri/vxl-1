//---------------------------------------------------------------------
// This is brcv/rec/dbskr/dbskr_tree_edit_pmi.h
//:
// \file
// \brief This version of edit distance algorithm derives from the original
//        and it uses normalized mutual information scores as path to path matching deformation cost
//        instead of elastic curve matching cost
//
// \author
//  O.C. Ozcanli - December 10, 2005
//
// \verbatim
//   
// \endverbatim
//
//-------------------------------------------------------------------------
#ifndef _dbskr_tree_edit_pmi_h
#define _dbskr_tree_edit_pmi_h

#include <dbskr/dbskr_tree_edit.h>
#include <dbinfo/dbinfo_observation_sptr.h>
#include <dbru/dbru_rcor_sptr.h>
#include <dbskr/dbskr_sm_cor_sptr.h>

class dbskr_tree_edit_pmi : public dbskr_tree_edit
{
protected:
  //Data
  dbinfo_observation_sptr obs1_;
  dbinfo_observation_sptr obs2_;
  dbru_rcor_sptr rcor_;
  dbskr_sm_cor_sptr sm_cor_;
  
public:

  dbskr_tree_edit_pmi(dbskr_tree_sptr tree1, 
                      dbskr_tree_sptr tree2, 
                      dbinfo_observation_sptr obs1,
                      dbinfo_observation_sptr obs2);

protected:
  // REMOVE ALL SPLICE AND CONTRACT COSTS FROM THE FOLLOWING METHODS
  //: if next(d1) is down, there is a left branch of head(d1), try splicing it
  virtual float maybe_left_splice_out_tree1(int d1, int d2);
  //: version returning paths
  virtual float maybe_left_splice_out_tree1_vec(int d1, int d2);

  //: if next(d2) is down, there is a left branch of head(d2), try splicing it
  virtual float maybe_left_splice_out_tree2(int d1, int d2);
  //: version returning paths
  virtual float maybe_left_splice_out_tree2_vec(int d1, int d2);

  virtual float merge_down_tree1(int a1, int a2, int d1, int d2, int td1, int td2, vcl_vector<pathtable_key>& vec);
  //: for speed do the operations without vectors first
  virtual float merge_down_tree1(int a1, int a2, int d1, int d2, int td1, int td2);
  
  virtual float merge_down_tree2(int a1, int a2, int d1, int d2, int td1, int td2, vcl_vector<pathtable_key>& vec);
  //: for speed do the operations without vectors first
  virtual float merge_down_tree2(int a1, int a2, int d1, int d2, int td1, int td2);

  virtual vcl_vector<pathtable_key>& maybe_right_splice_out_tree1_vec(int a1, int d1, int d2);
  virtual float maybe_right_splice_out_tree1(int a1, int d1, int d2);
  virtual vcl_vector<pathtable_key>& maybe_right_splice_out_tree2_vec(int a2, int d2, int d1);
  virtual float maybe_right_splice_out_tree2(int a2, int d2, int d1);


  //: find and cache the cost of matching curve pair induced by given dart paths
  //  modify this function so that it returns #of corresponding pixels in two regions x mutual info of two regions 
  virtual float get_cost(int td1, int d1, int td2, int d2);

  // parameters and inline functions
  //: "is less than" function with a precision argument
  //  change this to "isLarger than" to maximize the cost function instead of minimizing it
  virtual bool isL(float A, float B, float E) { return (((A)-E) > (B) ) ? true : false; }
};

#endif // _dbskr_tree_edit_pmi_h
