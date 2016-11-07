//: dbsk3d/dbsk3d_ms_hypg_build.h
//  040330 MingChing Chang
//  May 05, 2005
//

#ifndef dbsk3d_ms_hypg_build_h_
#define dbsk3d_ms_hypg_build_h_

#include <dbsk3d/dbsk3d_fs_comp_set.h>
#include <dbsk3d/dbsk3d_ms_hypg.h>

class dbsk3d_ms_hypg_build
{
protected:
  dbsk3d_fs_comp_set*   fs_comp_set_;
  dbsk3d_ms_hypg*       ms_hypg_;

public:
  //: ====== Constructor/Destructor ======
  dbsk3d_ms_hypg_build (dbsk3d_fs_comp_set* fs_comp_set,
                        dbsk3d_ms_hypg* ms_hypg) {
    fs_comp_set_ = fs_comp_set;
    ms_hypg_ = ms_hypg;
  }

  //: ====== Data access functions ======
  dbsk3d_fs_mesh* fs_mesh () {
    return fs_comp_set_->fs_ss()->fs_mesh();
  }
  void set_ms_hypg (dbsk3d_ms_hypg* ms_hypg) {
    ms_hypg_ = ms_hypg;
  }

  //: ====== Modification functions ======
  //: Build the coarse-scale shock scaffold hypergraph.
  dbsk3d_ms_hypg* build_ms_hypg (const int MS_topo_opt);
  
  //: Build the coarse-scale shock scaffold vertices.
  void build_ms_nodes ();

  //: Build the coarse-scale shock scaffold curves.
  void build_ms_curves ();

  //: Build the coarse-scale shock scaffold loops
  void build_ms_loops ();
    void trace_build_A3_curves (dbsk3d_ms_node* A1A3SV);
    void trace_build_A13_curves (dbsk3d_ms_node* A14SV);
    void trace_build_Dege_curves (dbsk3d_ms_node* DegeSV);

  //: Build the coarse-scale shock scaffold sheets.
  void build_ms_sheets (const int SS_topo_option);
    bool prop_build_sheet_comp (dbsk3d_ms_sheet* MS, dbsk3d_fs_face* FF,
                                const int SS_topo_option);
};

void prop_add_Fs_from_seed (dbsk3d_ms_sheet* MSn, dbsk3d_fs_face* seedFF);


#endif

