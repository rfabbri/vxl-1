//: This is brcv/shp/dbsk3d/dbsk3d_fs_mesh.h.
//  Ming-Ching Chang
//  Apr 30, 2004        Creation.
//
//  An migration of Frederic Leymarie's surface segregation code.
//  Note that Frederic has 3 versions of surface code, all in
//  /vision/projects/kimia/shock3d/c/c/shocks-3dpts/FLOW_VERTICES/VISU
//  See the Makefile for more information.
//
//  The standalone compilable C++ code is in 
//  /vision/projects/kimia/shock3d/VXLMigration/SurfaceFlowVertices.
//
//  I first migrate the first method (Single seed: Iterate until no genes unmeshed),
//  in the new 3D visualizer. Eventually we will have all 3 of them.
//  After I understand and debug the code, new ideas would be easily developed and added.
//
//  Note:
//  1) Try to avoid re-definition of similar structure, i.e. integrate it as much as possible.
//  2) Try to make the process as integrated as possible, so that
//     we will have an all-in-one environment for 3d shocks.
//
//  Note on Fre's Program:
//  In MeshSurfFromRedShockData.c
//
//  May 19, 2004        Merge into the FullShock class
//
//  June 23, 2004: Start to design the new Shock data structure.
//  Re-organize the surface pruning data & functions:
//     Keep the necessary one, and 
//     move away the un-generic one into separate structure.

#ifndef dbsk3d_fs_mesh_h_
#define dbsk3d_fs_mesh_h_

#include <vcl_map.h>
#include <vcl_queue.h>
#include <vcl_utility.h>

#include <dbmsh3d/dbmsh3d_mesh.h>
#include <dbsk3d/dbsk3d_fs_vertex.h>
#include <dbsk3d/dbsk3d_fs_edge.h>
#include <dbsk3d/dbsk3d_fs_face.h>

// ####################################################################
//    THE FULL SHOCK STRUCTURE
// ####################################################################

#define VALUE_NOT_DETERMINED  -1.0f

class dbsk3d_ms_hypg;

class dbsk3d_fs_mesh : public dbmsh3d_mesh
{
protected:
  //: Pointer to the boundary point generators of this dbsk3d_fs_mesh.
  dbmsh3d_mesh*       bnd_mesh_;

  float               median_A122_dist_;

public:
  //###### Constructor & Destructor ######
  dbsk3d_fs_mesh (dbmsh3d_mesh* bnd);
  virtual ~dbsk3d_fs_mesh ();

  virtual void clear () {
    dbmsh3d_mesh::clear();
    bnd_mesh_->clear();
    median_A122_dist_ = VALUE_NOT_DETERMINED;
  }
  virtual bool is_modified() {
    return dbmsh3d_mesh::is_modified() || bnd_mesh_->is_modified();
  }

  //###### Data Access Functions ######
  const dbmsh3d_mesh* bnd_mesh() const {
    return bnd_mesh_;
  }
  dbmsh3d_mesh* bnd_mesh() {
    return bnd_mesh_;
  }
  void set_bnd_mesh (const dbmsh3d_mesh* mesh) {
    bnd_mesh_ = (dbmsh3d_mesh*) mesh;
  }
  float median_A122_dist() const {
    assert (median_A122_dist_ != VALUE_NOT_DETERMINED);
    return median_A122_dist_;
  }
  void set_median_A122_dist (const float d) {
    median_A122_dist_ = d;
  }

  //###### Connectivity Query Functions ######

  //###### Connectivity Modification Functions ######
  //: new/delete function of the class hierarchy
  virtual dbmsh3d_vertex* _new_vertex () {
    return new dbsk3d_fs_vertex (vertex_id_counter_++);
  }
  virtual void _del_vertex (dbmsh3d_vertex* V) {
    dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) V;
    delete FV;
  }

  virtual dbmsh3d_edge* _new_edge (dbmsh3d_vertex* sV, dbmsh3d_vertex* eV) {
    return new dbsk3d_fs_edge (sV, eV, edge_id_counter_++);
  }
  virtual void _del_edge (dbmsh3d_edge* E) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) E;
    delete FE;
  }

  virtual dbmsh3d_face* _new_face () {
    return new dbsk3d_fs_face (face_id_counter_++);
  }
  virtual void _del_face (dbmsh3d_face* F) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) F;
    delete FF;
  }

  virtual void remove_vertex (dbmsh3d_vertex* V) {
    dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) V;
    FV->clear_asgn_G_list();
    dbmsh3d_pt_set::remove_vertex (FV);
  }
  virtual void remove_vertex (const int id) {
    dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) vertexmap (id);
    remove_vertex (FV);
  }

  virtual void remove_edge (dbmsh3d_edge* E) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) E;
    FE->clear_asgn_G_list();
    dbmsh3d_mesh::remove_edge (FE);
  }
  virtual void remove_edge (const int id) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) edgemap (id);
    remove_edge (FE);
  }

  //###### Shock Segregation and Regularization Functions ######  
  float compute_median_A122_dist ();

  void detect_valid_FE_type ();
  void compute_all_FEs_FVs_type ();

  unsigned int del_invalid_FFs_complete ();

  //###### Functions to associate boundary and shocks ######    
  bool check_all_G_asgn (vcl_vector<dbmsh3d_vertex*>& unasgn_genes); 

  //###### Other Functions ######
  virtual bool check_integrity ();

  dbsk3d_fs_mesh* clone (dbmsh3d_mesh* BND2);
  virtual dbmsh3d_pt_set* clone ();  

  bool check_all_FFs_valid ();
  void summary_report ();
};

//###### Connectivity Recovery Functions ######

//: Output more detailed memory usage.
int IFS_to_HE_print_mem_usage (dbsk3d_fs_mesh* fs_mesh);
void IFS_to_HE_print_mem_usage (dbsk3d_fs_mesh* fs_mesh, int seid_edge_map_size, 
                                int V_F_incidence_size, int total);

//###### Gene. assoication functions ######

#define G_ASSO_NONE         '0'
#define G_ASSO_PATCH        'P'
#define G_ASSO_LINK         'L'
#define G_ASSO_NODE         'N'

// A vector to store the association cases of a generator. Used for
// checking if a generator is associated to a shock sheet or curve element.
// vcl_map<int, char>  G_asso_map_;
//
inline void mark_G_asso (const dbmsh3d_vertex* G, char asso,
                         vcl_map<int, char>& G_asso_map) {    
  vcl_map<int, char>::iterator it = G_asso_map.find (G->id());
  assert (it != G_asso_map.end());
  (*it).second = asso;
}

//###### Get associated boundary IFS mesh ######

void FFs_get_bnd_mesh_Fs (const vcl_vector<dbsk3d_fs_face*>& fs_faces, 
                          vcl_set<dbmsh3d_vertex*>& Gset, vcl_set<dbmsh3d_face*>& Gfaces,
                          vcl_set<dbmsh3d_face*>& Gfaces2, vcl_set<dbmsh3d_face*>& Gfaces1);

#endif
