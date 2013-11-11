// This is brcv/shp/dbmsh3d/dbmsh3d_mesh.cxx
//:
// \file
// \brief Mesh with additional info at vertices and faces, e.g. color, normals
//
//
// \author Nhon Trinh(ntrinh@lems.brown.edu)
// \date August 14, 2006
//
// \verbatim
//  Modifications
//    Mar 27, 2007  Nhon Trinh   added handling for richface (face with exta properties)
// \endverbatim
//
//-------------------------------------------------------------------------


#include <vcl_iostream.h>
#include <dbmsh3d/dbmsh3d_richmesh.h>
//#include <vcl_cstring.h>

// ================= Constructors / Destructors ==========================



// =========== Other functions ==============
// Print a summary of mesh properties
void dbmsh3d_richmesh::
print_summary(vcl_ostream& str)
{
  dbmsh3d_mesh::print_summary(str);

  dbmsh3d_richvertex* v = (dbmsh3d_richvertex*)this->vertexmap().begin()->second;
  str << "List of properties at each vertex: ";
  vcl_vector<vcl_string > vertex_prop_list = v->scalar_property_list();
  for (unsigned int i=0; i<vertex_prop_list.size(); ++i)
  {
    str << " " << vertex_prop_list[i];
  }
  str << "\n";

  str << "List of properties at each face: ";
  dbmsh3d_richface* f = (dbmsh3d_richface*) this->facemap().begin()->second;
  vcl_vector<vcl_string > face_prop_list = f->scalar_property_list();
  for (unsigned int i=0; i< face_prop_list.size(); ++i)
  {
    str << " " << face_prop_list[i];
  }
  str << "\n";
}

dbmsh3d_richmesh* clone_richmesh_ifs (dbmsh3d_mesh* M, 
                                      const vcl_vector<vcl_string >& bkt_vplist,
                                      const vcl_vector<vcl_string >& bkt_fplist)
{
  //Assume M is in data structure mode of IFS, no isolated edge.
  dbmsh3d_richmesh* newRM = new dbmsh3d_richmesh ();

  //Clone all vertices of M.
  //Note: use _new_vertex() to create a new vertex.
  vcl_map<int, dbmsh3d_vertex*>::iterator vit = M->vertexmap().begin();
  for (; vit != M->vertexmap().end(); vit++) {
    dbmsh3d_vertex* V = (*vit).second;
    dbmsh3d_richvertex* newRV = (dbmsh3d_richvertex*) newRM->_new_vertex (V->id());
    newRV->set_pt (V->pt());    
    //Put the orig_id into the property of V in the richmesh.
    newRV->add_scalar_property ("id", newRV->id());
    newRM->_add_vertex (newRV);
  }
  newRM->set_vertex_id_counter (M->vertex_id_counter());

  //Clone all faces of M.
  vcl_map<int, dbmsh3d_face*>::iterator fit = M->facemap().begin();
  for (; fit != M->facemap().end(); fit++) {
    dbmsh3d_face* F = (*fit).second;
    dbmsh3d_face* newF = newRM->_new_face (F->id());
    for (unsigned int i=0; i<F->vertices().size(); i++) {
      int vid = F->vertices(i)->id();
      dbmsh3d_vertex* newV = newRM->vertexmap (vid);
      newF->_ifs_add_bnd_V (newV);
    }
    newRM->_add_face (newF);
  }
  newRM->set_face_id_counter (M->face_id_counter());

  return newRM;
}
