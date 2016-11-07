//: This is dbsk3d_fs_comp_set.h
//  MingChing Chang
//  Dec 08, 2006.

#ifndef _dbsk3d_fs_comp_set_h_
#define _dbsk3d_fs_comp_set_h_

#include <vcl_vector.h>
#include <vcl_queue.h>

#include <dbsk3d/dbsk3d_fs_comp.h>
#include <dbsk3d/dbsk3d_fs_sheet_set.h>

class dbsk3d_fs_comp_set
{
protected:
  vcl_vector<dbsk3d_fs_comp*> comp_list_;
  dbsk3d_fs_sheet_set*        fs_ss_;
  int                         fs_comp_id_counter_;

public:
  //####### Constructor/Destructor #######
  dbsk3d_fs_comp_set (dbsk3d_fs_sheet_set* fs_ss) { 
    fs_ss_ = fs_ss;
    fs_comp_id_counter_ = 0;
  }

  void clear() {
    for (unsigned int i=0; i<comp_list_.size(); i++) {
      delete comp_list_[i];
    }  
    comp_list_.clear();
    fs_comp_id_counter_ = 0;
  }

  ~dbsk3d_fs_comp_set () {
    clear();
  }

  //####### Data Access #######  
  vcl_vector<dbsk3d_fs_comp*>& comp_list() {
    return comp_list_;
  }  
  dbsk3d_fs_comp* comp_list (unsigned int id) {
    return comp_list_[id];
  }
  const int id_from_sorted_idx (unsigned int sorted_idx) const {
    assert (sorted_idx < comp_list_.size());
    return comp_list_[sorted_idx]->id();
  }

  dbsk3d_fs_sheet_set* fs_ss () const {
    return fs_ss_;
  }
  void set_fs_sheet_set (dbsk3d_fs_sheet_set* fs_ss) {
    fs_ss_ = fs_ss;
  }

  void add_fs_comp (const dbsk3d_fs_comp* C) {
    comp_list_.push_back ((dbsk3d_fs_comp*) C);
  }

  //####### Shock Component Query #######
  unsigned int n_valid_comps () {
    unsigned int count = 0;
    for (unsigned int i=0; i<comp_list_.size(); i++)
      if (comp_list_[i] != NULL)
        count++;
    return count;
  }
  unsigned int n_total_fs_sheets () {
    return fs_ss_->sheetmap().size();
  }

  //####### Shock Component Extraction #######
  unsigned int label_shock_components ();
    void propagate_label_C (dbsk3d_fs_comp* C, dbsk3d_fs_sheet* inputS);
    void put_neighbor_S_to_queue (dbsk3d_fs_sheet* S, 
                                  vcl_queue<dbsk3d_fs_sheet*>& sheet_queue);

  //####### Sheet Component Operations #######
  void sort_shock_components ();

  unsigned int delete_unspecified_comps (vcl_set<int>& ith_comp_list);
};

#endif
