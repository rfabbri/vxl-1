//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/dbmsh3d_face_mc.cxx
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

#include <vcl_cassert.h>
#include <vcl_iostream.h>

#include <dbmsh3d/dbmsh3d_face_mc.h>

/*dbmsh3d_face_mc::dbmsh3d_face_mc(const dbmsh3d_face_mc& face)
{
  vcl_map<int, dbmsh3d_halfedge*> set_he = face.set_he_;
  vcl_map<int, dbmsh3d_halfedge*>::iterator it = set_he.begin();
  while (it != set_he.end()) {
    dbmsh3d_halfedge* he = it->second;
    dbmsh3d_edge* edge = he->edge();
    this->_add_vertex(edge->sV());
    dbmsh3d_halfedge* new_he = new dbmsh3d_halfedge(edge, this);
  }
}*/

dbmsh3d_halfedge* dbmsh3d_face_mc::face_mc_map(int i) 
{
  vcl_map<int, dbmsh3d_halfedge*>::iterator it = set_he_.find (i);
  if (it == set_he_.end())
    return NULL;
  return (*it).second;
}

void dbmsh3d_face_mc::add_mc_halfedge(dbmsh3d_halfedge* inputHE)
{
  set_he_[id_counter_++] = inputHE;
}

bool dbmsh3d_face_mc::face_mc_id (dbmsh3d_halfedge* inputHE, int& id)
{
  vcl_map<int, dbmsh3d_halfedge*>::iterator it = set_he_.begin();
  while (it != set_he_.end()) {
    if (inputHE == it->second) {
      id = it->first;
      return true;
    }
  }
  return false;
}

void dbmsh3d_face_mc::get_mc_incident_halfedges (dbmsh3d_halfedge* inputHE, 
    vcl_vector<dbmsh3d_halfedge*>& incident_halfedges) const
{
  // first make sure that this halfedge head is in the set
  //int id;
  //if (face_mc_id(inputHE, id) == false)
   // vcl_cerr << "Halfedge is not included in set" << vcl_endl;

  dbmsh3d_face face(inputHE);
  face.get_bnd_HEs (incident_halfedges);
}

void dbmsh3d_face_mc::get_mc_incident_edges (dbmsh3d_halfedge* inputHE, 
    vcl_vector<dbmsh3d_edge*>& incident_edges) const
{
 // int id;
 // if (face_mc_id(inputHE, id) == false)
    //vcl_cerr << "Halfedge is not included in set" << vcl_endl;

  dbmsh3d_halfedge* HE = inputHE;
  do {
    incident_edges.push_back (HE->edge());
    HE = HE->next();
  }
  while (HE != inputHE);
}

//: reverse the orientation of this inner face,
//  i.e. reverse the chain of halfedges
void dbmsh3d_face_mc::reverse_mc_chain_of_halfedges (dbmsh3d_halfedge* inputHE) {
    
  
  vcl_vector<dbmsh3d_halfedge*> chain;
  dbmsh3d_halfedge* HE = inputHE;
  do {
    chain.push_back (HE);
    HE = HE->next();
  }
  while (HE != inputHE);

  //: build the reverse order
  HE = inputHE;
  for (int i=(int) chain.size()-1; i>=0; i--) {
    dbmsh3d_halfedge* next_he = chain[i];
    HE->set_next (next_he);
    HE = next_he;
  }
  assert (HE == inputHE);
}

void dbmsh3d_face_mc::print()
{
  unsigned i = 0;
  while (i < set_he_.size()) {
    dbmsh3d_halfedge* startHE = set_he_[i++];
    dbmsh3d_halfedge* HE = startHE;
    do {
      dbmsh3d_vertex* s = HE->edge()->sV();
      dbmsh3d_vertex* e = HE->edge()->eV();
      vcl_cout << "S=" << HE->edge()->sV() << " " << s->get_pt() << vcl_endl;
      vcl_cout << "E=" << HE->edge()->eV() << " " << e->get_pt() << vcl_endl;
      HE = HE->next();
    } while (HE != startHE);
  }
}
