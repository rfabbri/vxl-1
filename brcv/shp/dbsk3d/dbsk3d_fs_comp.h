//: This is dbsk3d_fs_comp.h
//  MingChing Chang
//  Nov 30, 2006.

#ifndef dbsk3d_fs_comp_h_
#define dbsk3d_fs_comp_h_

#include <vcl_set.h>

#include <dbsk3d/dbsk3d_fs_sheet.h>

#define S_UNVISITED_COST  -1

class dbsk3d_fs_comp
{
protected:
  vcl_vector<dbsk3d_fs_sheet*> fs_sheets_;
  int         id_;
  int         cost_;

public:
  //####### Constructor/Destructor #######
  dbsk3d_fs_comp (int id) { 
    id_     = id;
    cost_   = S_UNVISITED_COST;
  }
  virtual ~dbsk3d_fs_comp () {
    fs_sheets_.clear();
  }

  //####### Data Access #######
  const int id() const {
    return id_;
  }
  void set_id (const int id) {
    id_ = id;
  }
  const int cost () const {
    return cost_;
  }
  void set_cost (const int cost) {
    cost_ = cost;
  }

  vcl_vector<dbsk3d_fs_sheet*>& fs_sheets() {
    return fs_sheets_;
  }
  dbsk3d_fs_sheet* fs_sheets (const int i) {
    return fs_sheets_[i];
  }
  //: Add sheet S to this component.
  void add_sheet (const dbsk3d_fs_sheet* S) {
    assert (S->have_FFs());
    fs_sheets_.push_back ((dbsk3d_fs_sheet*) S);
  }

  //####### Connectivity Query Functions #######

  //####### Modification Functions #######
  void compute_cost ();

  void mark_all_FFs_valid ();
  void mark_all_FFs_invalid ();
};

#endif
