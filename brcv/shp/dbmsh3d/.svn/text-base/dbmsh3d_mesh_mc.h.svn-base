#ifndef _dbmsh3d_mesh_mc_h_
#define _dbmsh3d_mesh_mc_h_
//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/dbmsh3d_mesh.h
//:
// \file
// \brief multiple connected (mc) mesh for
//        representing mesh face with internal holes or curves.
//
// \author
//  Gamze Tunali  Jan 2, 2007
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------
#include <vcl_cassert.h>

#include <vcl_map.h>
#include <vcl_utility.h>

#include <dbmsh3d/dbmsh3d_mesh.h>
#include <dbmsh3d/dbmsh3d_face_mc.h>

//: The mesh class that handles indexed face set.
class dbmsh3d_mesh_mc : public dbmsh3d_mesh
{
public:
  dbmsh3d_mesh_mc() : dbmsh3d_mesh(), id_counter_(0) {}

  dbmsh3d_mesh_mc(dbmsh3d_mesh*);

  //: new/delete function of the class hierarchy
  virtual dbmsh3d_face_mc* _new_face () { 
    return new dbmsh3d_face_mc (face_id_counter_++);
  }

  virtual dbmsh3d_mesh_mc* clone() const;

  void orient_face_normals();

  void add_inner_block(dbmsh3d_face_mc* face) { set_face_[id_counter_++]=face; } 
  
  vcl_map<int, dbmsh3d_face_mc*> get_inner_blocks() { return set_face_; }

private:
  vcl_map<int, dbmsh3d_face_mc*> set_face_;
  int id_counter_;
};

//: breaks a face into two on edges E1 and E2 and returns the two 
// new faces F1 and F2
void mesh_break_face (dbmsh3d_mesh_mc* M, dbmsh3d_face_mc* F, 
                      dbmsh3d_edge* E1, dbmsh3d_edge* E2, 
                      dbmsh3d_vertex* V1, dbmsh3d_vertex* V2,
                      dbmsh3d_face_mc* &F1, dbmsh3d_face_mc* &F2);

//: merges two meshes M1 amd M2 into M1. M1 and and M2 do not 
// have any intersection
void merge_mesh (dbmsh3d_mesh_mc* M1, dbmsh3d_mesh_mc* M2);
dbmsh3d_face* copy_inner_face(vcl_vector<dbmsh3d_edge*> incident_edges,
                          dbmsh3d_mesh_mc* mesh);
#endif

