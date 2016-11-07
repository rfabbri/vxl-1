//---------------------------------------------------------------------
// This is brcv/rec/dbskr/dbskr_tree_edit_coarse.h
//:
// \file
// \brief The unrooted tree edit distance using a coarse matching cost between pairs of darts
//        In addition doing a coarse alignment another
//        difference between normal edit distance and this version is that:
//        all the shock curves are treated as non-leaf scurves so that circular completions 
//        at the end nodes are eliminated
//
// \author
//  O.C. Ozcanli - March 28, 2007
//
// \verbatim
//   Modifications
//
//  Ozge C Ozcanli March 29, 2007          Removed the scurve pair class and all related functionality
//                                         The dense one is almost always same as the coarse one
//                                         and the dense one is never used
//
// \endverbatim
//
//-------------------------------------------------------------------------

#ifndef _dbskr_tree_edit_coarse_h
#define _dbskr_tree_edit_coarse_h

#include <dbskr/dbskr_tree_edit.h>

class dbskr_tree_edit_coarse : public dbskr_tree_edit
{

public:

  dbskr_tree_edit_coarse(dbskr_tree_sptr tree1, dbskr_tree_sptr tree2, bool construct_circular_ends) : dbskr_tree_edit(tree1, tree2, construct_circular_ends) {}
  virtual ~dbskr_tree_edit_coarse() { clear(); };

  //: find and cache the cost of matching curve pair induced by given dart paths
  virtual float get_cost(int td1, int d1, int td2, int d2);

  virtual dbskr_sm_cor_sptr get_correspondence(bool save_pathtable = false);
  virtual dbskr_sm_cor_sptr get_correspondence_just_map();

  //: return the corresponding shock curves and their maps
  virtual bool get_correspondence(vcl_vector<dbskr_scurve_sptr>& curve_list1, 
                          vcl_vector<dbskr_scurve_sptr>& curve_list2, 
                          vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
                          vcl_vector< pathtable_key >& path_map); 

  //: return the corresponding shock curves and their maps
  virtual bool get_correspondence_just_map_helper(vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
                                   vcl_vector< pathtable_key >& path_map); 
};

#endif // _dbskr_tree_edit_coarse_h
