#ifndef _dbmsh3d_face_mc_h_
#define _dbmsh3d_face_mc_h_

//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/dbmsh3d_halfedge.h
//:
// \file
// \brief Multiply Connected Face Class. A face contains inner faces.
//
//
// \author
//  Gamze Tunali  Dec 28, 2006
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------
#include <vcl_map.h>

#include <dbmsh3d/dbmsh3d_face.h>
#include <dbmsh3d/dbmsh3d_halfedge.h>

class dbmsh3d_face_mc : public dbmsh3d_face
{

protected:
  vcl_map<int, dbmsh3d_halfedge*> set_he_;
  int id_counter_;

public:
  //: ====== Constructor/Destructor ======
  dbmsh3d_face_mc (const int id) 
    : dbmsh3d_face(id), id_counter_(0) 
  { set_he_.clear(); }

  dbmsh3d_face_mc (const dbmsh3d_face* face) 
    : dbmsh3d_face(face->id()),  id_counter_(0) 
  { set_he_.clear(); halfedge_ = face->halfedge();}

  //: copy constructor
  //dbmsh3d_face_mc (const dbmsh3d_face_mc& face);

  virtual ~dbmsh3d_face_mc () {}

  dbmsh3d_halfedge* face_mc_map(int i);

  bool face_mc_id(dbmsh3d_halfedge* he, int& i);

  int size() {return set_he_.size(); }

  vcl_map<int, dbmsh3d_halfedge*> get_mc_halfedges() { return set_he_; }

  void add_mc_halfedge(dbmsh3d_halfedge* he);
  
  void remove_mc_halfedge(dbmsh3d_halfedge* he) {};

  void get_mc_incident_halfedges (dbmsh3d_halfedge* he, 
    vcl_vector<dbmsh3d_halfedge*>& incident_edges) const;

  //: given halfedge head, returns the set of incident edges 
  // of the inner face
  void get_mc_incident_edges (dbmsh3d_halfedge* he, 
    vcl_vector<dbmsh3d_edge*>& incident_edges) const;

  void reverse_mc_chain_of_halfedges (dbmsh3d_halfedge* he);

  void print();
};

#endif
