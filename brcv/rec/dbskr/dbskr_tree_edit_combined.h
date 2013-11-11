//---------------------------------------------------------------------
// This is brcv/rec/dbskr/dbskr_tree_edit_combined.h
//:
// \file
// \brief This class inherits from the original edit distance and uses the combined width and orientation costs as the deformation cost
//        as proposed by Kimia, Ozcanli, Tamrakar in 2006. (see dbskr_dpmatch_combined.h for an explanation of the differences in the cost functions)
//
// \author
//  O.C. Ozcanli - May 17, 2007
//
// \verbatim
//   Modifications
//
// \endverbatim
//
//-------------------------------------------------------------------------

#ifndef _dbskr_tree_edit_combined_h
#define _dbskr_tree_edit_combined_h

#include <dbskr/dbskr_tree_edit.h>

class dbskr_tree_edit_combined : public dbskr_tree_edit
{
public:

  void clear();

  dbskr_tree_edit_combined(dbskr_tree_sptr tree1, dbskr_tree_sptr tree2, bool construct_circular_ends = true, bool localize_match = false) : dbskr_tree_edit(tree1, tree2, construct_circular_ends, localize_match) {}
  //virtual ~dbskr_tree_edit_combined() { clear(); };

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

protected:
  
  //: find and cache the cost of matching curve pair induced by given dart paths
  virtual float get_cost(int td1, int d1, int td2, int d2);

};

#endif // _dbskr_tree_edit_combined_h
