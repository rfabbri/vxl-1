// This is brcv/shp/dbmsh3d/dbmsh3d_richmesh.h

#ifndef dbmsh3d_richmesh_h_
#define dbmsh3d_richmesh_h_

//---------------------------------------------------------------------
//:
// \file
// \brief Mesh with additional info at vertices and faces, e.g. colors, normals
// \author Nhon Trinh(ntrinh@lems.brown.edu)
// \date August 14, 2006
//
// \verbatim
//  Modifications
//
// \endverbatim
//-------------------------------------------------------------------------


///#include <dbmsh3d/dbmsh3d_property.h>
#include <dbmsh3d/dbmsh3d_mesh.h>

#include <dbmsh3d/dbmsh3d_richvertex.h>
#include <dbmsh3d/dbmsh3d_richface.h>


// a mesh class with additional information at the vertices and faces
// e.g. normals, colors.
class dbmsh3d_richmesh : public dbmsh3d_mesh
{
public:
  // ========== Constructors / Destructors ============================
  dbmsh3d_richmesh() : dbmsh3d_mesh(){
  }

  virtual ~dbmsh3d_richmesh(){
    if (del_elm_destruct_)
      clear ();
  }

  virtual void clear () {
    clear_faces (); //delete all faces first. 
    clear_edges (); //delete all edges.
    clear_vertices ();  //delete all vertices.
    i_traverse_flag_ = 0;
    b_watertight_ = false;
  }

  // ============ New / Edit / Remove rich vertices and rich faces
  //: new/delete function of the class hierarchy
  virtual dbmsh3d_vertex* _new_vertex () {
    return new dbmsh3d_richvertex(vertex_id_counter_++);
  }
  virtual dbmsh3d_vertex* _new_vertex (const int id) {
    return new dbmsh3d_richvertex (id);
    if (vertex_id_counter_ <= id)
      vertex_id_counter_ = id+1;
  }

  virtual void _del_vertex (dbmsh3d_vertex* V) {
    dbmsh3d_richvertex* RV = (dbmsh3d_richvertex*) V;
    delete RV;
  }
  
  virtual void clear_vertices () {
    vcl_map<int, dbmsh3d_vertex*>::iterator it = vertexmap_.begin();
    while (it != vertexmap_.end()) {
      dbmsh3d_richvertex* RV = (dbmsh3d_richvertex*) (*it).second;
      remove_vertex (RV);
      it = vertexmap_.begin();
    }
    vertex_id_counter_ = 0;
  }

  //: new/delete function of the class hierarchy
  virtual dbmsh3d_face* _new_face () {
    return new dbmsh3d_richface (face_id_counter_++);
  }
  virtual dbmsh3d_face* _new_face (const int id) {
    if (face_id_counter_ <= id)
      face_id_counter_ = id+1;
    return new dbmsh3d_richface (id);
  }
  
  virtual void _del_face (dbmsh3d_face* F) {
    dbmsh3d_richface* RF = (dbmsh3d_richface*) F;
    delete RF;
  }
  
  //: function to disconnect & remove all faces from the facemap
  virtual void clear_faces () {
    vcl_map<int, dbmsh3d_face*>::iterator it = facemap_.begin();
    while (it != facemap_.end()) {
      dbmsh3d_richface* RF = (dbmsh3d_richface*) (*it).second;
      remove_face (RF);      
      it = facemap_.begin();
    }
    face_id_counter_ = 0;
  }

  // =========== Other functions ==============
  // Print a summary of mesh properties
  virtual void print_summary(vcl_ostream& str);  

};

dbmsh3d_richmesh* clone_richmesh_ifs (dbmsh3d_mesh* M, 
                                      const vcl_vector<vcl_string >& bkt_vplist,
                                      const vcl_vector<vcl_string >& bkt_fplist);

#endif //dbmsh3d_richmesh_h_


