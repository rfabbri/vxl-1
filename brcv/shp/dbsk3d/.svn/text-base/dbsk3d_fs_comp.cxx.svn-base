// This is dbsk3d_fs_comp.cxx
//: MingChing Chang
//  Dec 08, 2006        Creation

#include <dbsk3d/dbsk3d_fs_comp.h>

//: Compute the cost of this shock component.
//  Cost = # of total assigned genes.
void dbsk3d_fs_comp::compute_cost ()
{
  //Go through each shock sheet and count their assigned genes.
  vcl_set<const dbmsh3d_vertex*> genes;
  for (unsigned int i=0; i<fs_sheets_.size(); i++) {
    dbsk3d_fs_sheet* S = fs_sheets_[i];

    //Go through each shock patch-elm of S.
    for (dbmsh3d_ptr_node* cur = S->FF_list(); cur != NULL; cur = cur->next()) {
      dbsk3d_fs_face* FE = (dbsk3d_fs_face*) cur->ptr();
      genes.insert (FE->genes(0));
      genes.insert (FE->genes(1));
    }
  }
  cost_ = genes.size();
}

void dbsk3d_fs_comp::mark_all_FFs_valid ()
{
  for (unsigned int i=0; i<fs_sheets_.size(); i++) {
    dbsk3d_fs_sheet* S = fs_sheets_[i];
    //Mark all shock elements of S valid.
    S->mark_all_FFs_valid ();
  }
}

void dbsk3d_fs_comp::mark_all_FFs_invalid ()
{
  for (unsigned int i=0; i<fs_sheets_.size(); i++) {
    dbsk3d_fs_sheet* S = fs_sheets_[i];
    //Mark all shock elements of S invalid.
    S->mark_all_FFs_invalid ();
  }
}



