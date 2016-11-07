//: This is brcv/shp/dbsk3d/algo/dbsk3d_fs_regul.h
//  MingChing Chang
//  Nov 30, 2005        Creation.

#ifndef dbsk3d_fs_regul_h_
#define dbsk3d_fs_regul_h_

#include <vcl_queue.h>
#include <vcl_list.h>
#include <vcl_algorithm.h>
#include <vcl_utility.h>

#include <dbsk3d/dbsk3d_fs_face.h>
#include <dbsk3d/dbsk3d_fs_mesh.h>
#include <dbsk3d/dbsk3d_fs_sheet_set.h>

//: class for shock regularization and surface smoothing
class dbsk3d_fs_regul
{
protected:
  //: Pointer to the set of shock sheets.
  dbsk3d_fs_sheet_set* fs_ss_;

  //: Priority queue of rank ordered shock sheets.
  vcl_multimap<float, dbsk3d_fs_sheet*>  fs_sheet_queue_;

public:
  //###### Constructor & Destructor ######
  dbsk3d_fs_regul (dbsk3d_fs_sheet_set* fs_sheetset) {
    fs_ss_ = fs_sheetset;
  }
  ~dbsk3d_fs_regul () {
    fs_sheet_queue_.clear();
  }

  //###### Data Accessing ######
  dbsk3d_fs_mesh* fs_mesh() const {
    return fs_ss_->fs_mesh();
  }
  dbmsh3d_mesh* bnd_mesh() const {
    return fs_ss_->fs_mesh()->bnd_mesh ();
  }

  void _add_FS_to_Q (const dbsk3d_fs_sheet* S) {
    assert (S->cost() >= 0);
    assert (S->type() == FS_TYPE_TAB || S->type() == FS_TYPE_UNBOUNDED);
    fs_sheet_queue_.insert (vcl_pair<float, dbsk3d_fs_sheet*> (S->cost(), (dbsk3d_fs_sheet*) S));
  }
  
  bool _remove_S_from_Q (const dbsk3d_fs_sheet* S) {
    float key = S->cost();  
    vcl_multimap<float, dbsk3d_fs_sheet*>::iterator lower = fs_sheet_queue_.lower_bound(key);
    vcl_multimap<float, dbsk3d_fs_sheet*>::iterator upper = fs_sheet_queue_.upper_bound(key);
    vcl_multimap<float, dbsk3d_fs_sheet*>::iterator it = lower;
    for (; it != upper; it++) {
      if ((*it).second == S) {
        fs_sheet_queue_.erase (it);
        return true;
      }
    }
    return false;
  }

  //###### Shock Regularization & Surface Smoothing Functions ######
  
  //: Trim out A12-2 fs_faces.  
  bool trim_bnd_A122_FFs (const float rmin_ratio);

  void run_shock_regul (const float reg_th, const bool reasgn_lost_genes);

  void init_regul_sheet_queue (const float reg_th);
  void greedy_shock_regul (const float reg_th);

  //: 3D Splice Transform on a fs_sheet.
  void S_3d_splice_xform (dbsk3d_fs_sheet* S, 
                          vcl_list<vcl_pair<int, int> >& S_to_splice,
                          vcl_vector<dbsk3d_fs_edge*>& C_Lset,
                          const float reg_th);
};

#endif
