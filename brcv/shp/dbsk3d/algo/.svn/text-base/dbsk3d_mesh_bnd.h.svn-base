//: dbsk3d/dbsk3d_mesh_bnd.h
//  MingChing Chang
//  Dec 14, 2006        Creation.

#ifndef _dbsk3d_mesh_bnd_h_
#define _dbsk3d_mesh_bnd_h_

#include <vcl_set.h>
#include <dbmsh3d/algo/dbmsh3d_mesh_bnd.h>
#include <dbsk3d/dbsk3d_fs_face.h>
#include <dbsk3d/dbsk3d_fs_mesh.h>

class dbsk3d_bnd_chain : public dbmsh3d_bnd_chain
{
protected:
  vcl_set<dbsk3d_fs_face*>  P_set_;

public:
  //=========== Constructor & Destructor ===========
  dbsk3d_bnd_chain () : dbmsh3d_bnd_chain () {
  }
  virtual ~dbsk3d_bnd_chain () {
    P_set_.clear();
  }

  //=========== Data Accessing ===========
  dbsk3d_fs_edge* find_L_for_hole (const dbmsh3d_vertex* Vo, 
                                   const dbmsh3d_edge* E);

  //=========== Modification Functions ===========
  void _add_fs_patch (const dbsk3d_fs_face* FF) {
    P_set_.insert ((dbsk3d_fs_face*) FF);
  }
};

// #########################################################

class dbsk3d_bnd_chain_set : public dbmsh3d_bnd_chain_set
{
protected:
  dbsk3d_fs_mesh* fs_mesh_;

public:
  //=========== Constructor & Destructor ===========
  dbsk3d_bnd_chain_set (dbsk3d_fs_mesh* fs_mesh) :
    dbmsh3d_bnd_chain_set (fs_mesh->bnd_mesh()) {
    fs_mesh_ = fs_mesh;
  }
  virtual ~dbsk3d_bnd_chain_set () {}

  //=========== Data Accessing ===========
  virtual dbmsh3d_bnd_chain* _new_bnd_chain () {
    return new dbsk3d_bnd_chain ();
  }
  virtual void _del_bnd_chain (dbmsh3d_bnd_chain* BC) {
    dbsk3d_bnd_chain* BCs = (dbsk3d_bnd_chain*) BC;
    delete BCs;
  }

  //=========== Modification Functions ===========
  void set_fs_mesh (dbsk3d_fs_mesh* fs_mesh) {
    fs_mesh_ = fs_mesh;
    mesh_ = fs_mesh_->bnd_mesh();
  }
  bool detect_bnd_chains_th (const unsigned int th, const unsigned int skip_chain_th);

  void add_P_to_bnd_chain (const dbmsh3d_vertex* G, const dbsk3d_fs_face* FF);
};

#endif
