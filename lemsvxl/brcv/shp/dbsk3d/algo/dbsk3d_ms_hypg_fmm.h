//: This is dbsk3d_ms_hypg_fmm.h
//  Medial Scaffold Hypergraph Transitions Regularization
//  MingChing Chang 2006, 11/17 

#ifndef dbsk3d_hypg_fmm_h_
#define dbsk3d_hypg_fmm_h_

#include <vcl_map.h>
#include <dbmsh3d/algo/dbmsh3d_fmm.h>
#include <dbsk3d/dbsk3d_ms_hypg.h>


class dbsk3d_ms_hypg_fmm
{
protected:
  dbsk3d_ms_hypg*       ms_hypg_;
  
  //: map between each shock sheet id and the corresponding fmm-mesh
  vcl_map <int, dbmsh3d_fmm_mesh*> sheet_fmm_mesh_map_;

public:
  //###### Constructor/Destructor ######
  dbsk3d_ms_hypg_fmm (dbsk3d_ms_hypg* ms_hypg) {
    ms_hypg_ = ms_hypg;
  }
  virtual ~dbsk3d_ms_hypg_fmm () {
    vcl_map<int, dbmsh3d_fmm_mesh*>::iterator it = sheet_fmm_mesh_map_.begin();
    for (; it != sheet_fmm_mesh_map_.end(); it++) {
      dbmsh3d_fmm_mesh* fmm_mesh = (*it).second;
      delete fmm_mesh;
    }
    sheet_fmm_mesh_map_.clear();
  }

  //###### Data access functions ######
  dbsk3d_ms_hypg* ms_hypg () {
    return ms_hypg_;
  }
  dbsk3d_fs_mesh* fs_mesh() {
    return ms_hypg_->fs_mesh();
  }
  void set_ms_hypg (dbsk3d_ms_hypg* ms_hypg) {
    ms_hypg_ = ms_hypg;
  }

  vcl_map <int, dbmsh3d_fmm_mesh*>& sheet_fmm_mesh_map() {
    return sheet_fmm_mesh_map_;
  }

  //###### Transition Functions ######

  void build_sheets_fmm_mesh ();
  
  void run_fmm_on_sheet_meshes ();

};

#endif
