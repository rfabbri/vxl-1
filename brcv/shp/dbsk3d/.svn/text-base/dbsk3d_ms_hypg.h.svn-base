//: This is dbsk3d/dbsk3d_ms_hypg.h
//  MingChing Chang
//  Oct 20, 2004.

#ifndef dbsk3d_ms_hypg_h_
#define dbsk3d_ms_hypg_h_

#include <dbmsh3d/dbmsh3d_hypg.h>
#include <dbsk3d/dbsk3d_fs_mesh.h>
#include <dbsk3d/dbsk3d_ms_node.h>
#include <dbsk3d/dbsk3d_ms_curve.h>
#include <dbsk3d/dbsk3d_ms_sheet.h>

class dbsk3d_fs_mesh;

class dbsk3d_ms_hypg : public dbmsh3d_hypg
{
protected:
  int n_selected_ms_curves_;
  int n_selected_ms_nodes_;

public:
  //###### Constructor/Destructor ######
  dbsk3d_ms_hypg (const dbsk3d_fs_mesh* FS) : dbmsh3d_hypg (FS) {
    n_selected_ms_curves_ = 0;
    n_selected_ms_nodes_ = 0;
  }
  virtual ~dbsk3d_ms_hypg () {
  }

  virtual void clear () {
    _clear_sheetmap();
    dbmsh3d_graph::clear();
    fs_mesh()->clear();
  }
  virtual bool is_modified() {
    return dbmsh3d_hypg::is_modified() || fs_mesh()->is_modified();
  }

  //###### Data access functions ######
  const dbsk3d_fs_mesh* fs_mesh() const {
    return (dbsk3d_fs_mesh*) mesh_;
  }
  dbsk3d_fs_mesh* fs_mesh() {
    return  (dbsk3d_fs_mesh*) mesh_;
  }
  const dbmsh3d_mesh* bnd_mesh() const {
    return fs_mesh()->bnd_mesh();
  }
  dbmsh3d_mesh* bnd_mesh() {
    return fs_mesh()->bnd_mesh();
  }
  const int n_selected_ms_curves() const {
    return n_selected_ms_curves_;
  }
  const int n_selected_ms_nodes() const {
    return n_selected_ms_nodes_;
  }

  //###### Connectivity Query Functions ######

  //###### Connectivity Modification Functions ######

  //: new/delete function of the class hierarchy
  virtual dbmsh3d_vertex* _new_vertex () {
    return new dbsk3d_ms_node (vertex_id_counter_++);
  }  
  virtual void _del_vertex (dbmsh3d_vertex* V) {
    dbsk3d_ms_node* MN = (dbsk3d_ms_node*) V;
    delete MN;
  }

  virtual dbmsh3d_edge* _new_edge (dbmsh3d_vertex* sV, dbmsh3d_vertex* eV) {
    return new dbsk3d_ms_curve (sV, eV, edge_id_counter_++);
  }
  virtual void _del_edge (dbmsh3d_edge* E) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) E;
    delete MC;
  }

  virtual dbmsh3d_sheet* _new_sheet () {
    return new dbsk3d_ms_sheet (sheet_id_counter_++);
  }
  virtual void _del_sheet (dbmsh3d_sheet* S) {
    dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) S;
    delete MS;
  }

  void add_virtual_curve (dbmsh3d_vertex* sV, dbmsh3d_vertex* eV,
                          dbmsh3d_curve* sC, dbmsh3d_curve* eC);
  void add_virtual_curve (dbmsh3d_vertex* sV, dbmsh3d_vertex* eV,
                          vcl_vector<dbmsh3d_curve*>& sup_curves);

  //###### Graph traversing uses dbsk3d_fs_mesh's flag ###### 
  const unsigned int i_traverse_flag () const;

  void select_salient_ms_curves (const int min_elms, const int n_curves, const int verbose = 1);

  void select_all ();

  //###### Other functions ######
  virtual bool check_integrity ();

  virtual dbsk3d_ms_hypg* clone (dbsk3d_fs_mesh* FS2);
  virtual dbmsh3d_pt_set* clone ();  

  void print_MS_info ();
};


#endif
