//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/dbmsh3d_textured_mesh_mc.cxx
//:
// \file
// \brief Mesh
//
//
// \author
//  Daniel Crispell  March 20, 2007
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vcl_cstdio.h>

#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_list.h>
#include <vnl/vnl_math.h>


#include <dbmsh3d/dbmsh3d_textured_mesh_mc.h>
#include <dbmsh3d/dbmsh3d_mesh_mc.h>
#include <dbmsh3d/dbmsh3d_textured_face_mc.h>
#include <dbmsh3d/dbmsh3d_vertex.h>

#include <vcl_iostream.h>

//: create a textured mesh from a non-textured mesh
dbmsh3d_textured_mesh_mc::dbmsh3d_textured_mesh_mc(const dbmsh3d_mesh_mc* mesh_in): dbmsh3d_mesh_mc()
{
  dbmsh3d_mesh_mc* mesh = const_cast<dbmsh3d_mesh_mc*>(mesh_in);

  // shallow copy the vertices
  vcl_map<int, dbmsh3d_vertex* > vertices = mesh->vertexmap();
  vcl_map<int, dbmsh3d_vertex* >::iterator v_it;
  for (v_it = vertices.begin(); v_it != vertices.end(); v_it++) {
    dbmsh3d_vertex* vertex = v_it->second;
    this->_add_vertex(vertex);
  }

  // shallow copy the edgemap
  vcl_map<int, dbmsh3d_edge* > edgemap = mesh->edgemap();
  vcl_map<int, dbmsh3d_edge* >::iterator edge_it;
  for (edge_it = edgemap.begin(); edge_it != edgemap.end(); edge_it++) {
    // create new edges
    dbmsh3d_edge* edge = edge_it->second;
    this->add_edge_incidence (edge);
  }

  // deep copy the faces (need to create textured faces)
  vcl_map<int, dbmsh3d_face* > fmap = mesh->facemap();
  vcl_map<int, dbmsh3d_face* >::iterator face_it;
  for (face_it = fmap.begin(); face_it != fmap.end(); face_it++) {

    dbmsh3d_face_mc* face = (dbmsh3d_face_mc*) face_it->second;
    face->_sort_bnd_HEs_chain();
    face->_ifs_clear_vertices ();

    dbmsh3d_textured_face_mc* f = this->_new_face();
    dbmsh3d_halfedge* he = face->halfedge();
    dbmsh3d_halfedge* HE = he;

    // clone the half edges
    do {
      dbmsh3d_edge* edge = HE->edge();
      //vcl_cout << edge->id() << vcl_endl;
      dbmsh3d_halfedge* new_he = new dbmsh3d_halfedge(this->edgemap(edge->id()), f);
      f->connect_bnd_E_end (new_he->edge());
      HE = HE->next();
    }while (HE != he);
    
    this->_add_face(f);
    // clone the inner faces
    if (face->size() > 0) {
      vcl_map<int, dbmsh3d_halfedge*> inner_faces = face->get_mc_halfedges();
      vcl_map<int, dbmsh3d_halfedge*>::iterator it = inner_faces.begin();
      while (it != inner_faces.end()) {
        vcl_vector<dbmsh3d_edge*> incident_edges;
        face->get_mc_incident_edges(it->second, incident_edges);
        dbmsh3d_face* inner_face = copy_inner_face(incident_edges, mesh);
        f->add_mc_halfedge((dbmsh3d_halfedge *)inner_face->halfedge());
        it++;
      }
    }
  }
}


dbmsh3d_textured_mesh_mc* dbmsh3d_textured_mesh_mc::clone() const
{
  dbmsh3d_textured_mesh_mc* mesh = new dbmsh3d_textured_mesh_mc();
  
  // deep copy the vertices
  vcl_map<int, dbmsh3d_vertex* > vertices = this->vertexmap_;
  vcl_map<int, dbmsh3d_vertex* >::iterator v_it = vertices.begin();
  while (v_it != vertices.end()) {
    dbmsh3d_vertex* vertex = (dbmsh3d_vertex*) v_it->second;
    dbmsh3d_vertex* v = (dbmsh3d_vertex*) mesh->_new_vertex(); 
    v->set_pt(vertex->get_pt());
    mesh->_add_vertex(v);
    v_it++;
  }

 // deep copy the edgemap
  vcl_map<int, dbmsh3d_edge* > edgemap = this->edgemap_;
  vcl_map<int, dbmsh3d_edge* >::iterator edge_it = edgemap.begin();
  while (edge_it != edgemap.end()) {
    // create new edges
    dbmsh3d_edge* edge = edge_it->second;
    //vcl_cout << "old edge id=" << edge->id() << vcl_endl;
    dbmsh3d_edge* new_edge = /*mesh->_new_edge*/new dbmsh3d_edge((dbmsh3d_vertex*) mesh->vertexmap(edge->sV()->id()), 
                                    (dbmsh3d_vertex*) mesh->vertexmap(edge->eV()->id()), edge_it->first);
    //vcl_cout << "  new edge id=" << new_edge->id() << vcl_endl;
    //vcl_cout << " v1=" << ((dbmsh3d_vertex*)new_edge->sV())->get_pt() << 
    //  " v2=" << ((dbmsh3d_vertex*)new_edge->eV())->get_pt() << vcl_endl;
    mesh->add_edge_incidence (new_edge);
    edge_it++;
  }

  // deep copy the faces
  vcl_map<int, dbmsh3d_face* > fmap = this->facemap_;
  vcl_map<int, dbmsh3d_face* >::iterator face_it = fmap.begin();
  while (face_it != fmap.end()) {
    dbmsh3d_textured_face_mc* face = (dbmsh3d_textured_face_mc*) face_it->second;
    //vcl_cout << "old face id=" << face->id() << vcl_endl;
    face->_sort_bnd_HEs_chain();
    face->_ifs_clear_vertices ();
    dbmsh3d_textured_face_mc* f = mesh->_new_face();
    dbmsh3d_halfedge* he = face->halfedge();
    dbmsh3d_halfedge* HE = he;

    // copy the texture coordinates

    f->tex_coords().insert(face->tex_coords().begin(), face->tex_coords().end());

    // clone the half edges
    do {
      dbmsh3d_edge* edge = HE->edge();
      //vcl_cout << edge->id() << vcl_endl;
      dbmsh3d_halfedge* new_he = new dbmsh3d_halfedge(mesh->edgemap(edge->id()), f);
      f->connect_bnd_E_end (new_he->edge());
      HE = HE->next();
    }while (HE != he);
    
    mesh->_add_face(f);
    // clone the inner faces
    if (face->size() > 0) {
      vcl_map<int, dbmsh3d_halfedge*> inner_faces = face->get_mc_halfedges();
      vcl_map<int, dbmsh3d_halfedge*>::iterator it = inner_faces.begin();
      while (it != inner_faces.end()) {
        vcl_vector<dbmsh3d_edge*> incident_edges;
        face->get_mc_incident_edges(it->second, incident_edges);
        dbmsh3d_face* inner_face = copy_inner_face(incident_edges, mesh);
        f->add_mc_halfedge((dbmsh3d_halfedge *)inner_face->halfedge());
        it++;
      }
    }
    face_it++;
  }
  return mesh;
}

