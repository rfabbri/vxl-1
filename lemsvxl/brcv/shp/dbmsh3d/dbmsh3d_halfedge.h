#ifndef dbmsh3d_halfedge_h_
#define dbmsh3d_halfedge_h_
//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/dbmsh3d_halfedge.h
//:
// \file
// \brief Basic 3d halfedge on a mesh
//
//
// \author
//  MingChing Chang  May 30, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vcl_vector.h>

#include <dbmsh3d/dbmsh3d_utils.h>
#include <dbmsh3d/dbmsh3d_vertex.h>

//#######################################################
//     The Modified Half-Edge Data Structure
//#######################################################

class dbmsh3d_edge;
class dbmsh3d_face;
class dbmsh3d_sheet;

//: the pointer to two ending vertex are in the dbmsh3d_edge class.
class dbmsh3d_halfedge
{
protected:
  //: For manifold-mesh, the pair's pair is itself
  //  For non-manifold mesh, the pairs form a circular list
  dbmsh3d_halfedge*  pair_;

  //: The nexts form a circular list bounding a face
  dbmsh3d_halfedge*  next_;

  dbmsh3d_face* face_;

  //: pointer to the dbmsh3d_edge
  dbmsh3d_edge* edge_;

public:
  //###### Constructor/Destructor ######
  dbmsh3d_halfedge (dbmsh3d_halfedge* pair, dbmsh3d_halfedge* next,
                    dbmsh3d_edge* edge, dbmsh3d_face* face)
  {
    pair_ = pair;
    next_ = next;
    face_ = face;
    edge_ = edge;
  }

  dbmsh3d_halfedge (dbmsh3d_edge* edge, dbmsh3d_face* face)
  {
    face_ = face;
    edge_ = edge;
    pair_ = NULL;
    next_ = NULL;
  }

  virtual ~dbmsh3d_halfedge () {
    //: make sure that its pair_ is properly disconnected!
    assert (pair_ == NULL);

    //: don't need to worry about the next_ here
    //  assume the topological consistency is handled
    //  when a face is deleted!

    //: make sure that no empty halfedge exists
    assert (face_ != NULL);

    //: make sure that it is already disconnected from the edge
    //  You should use dbmsh3d::remove_face() and
    //  dbmsh3d::remove_edge() to ensure topological consistency.
    assert (edge_ == NULL);
  }

  //###### Data access functions ######
  dbmsh3d_halfedge* pair() const {
    assert (pair_ != this);
    return pair_;
  }
  void set_pair (dbmsh3d_halfedge* pair) {
    assert (pair != this);
    pair_ = pair;
  }

  dbmsh3d_halfedge* next() const {
    assert (next_ != this);
    return next_;
  }
  void set_next (dbmsh3d_halfedge* next) {
    assert (next != this);
    next_ = next;
  }

  dbmsh3d_face* face() const {
    return face_;
  }
  void set_face (dbmsh3d_face* face) {
    face_ = face;
  }

  dbmsh3d_edge* edge() const {
    return edge_;
  }
  void set_edge (dbmsh3d_edge* edge) {
    edge_ = edge;
  }

  //: Returns the starting vertex for this halfedge based on the direction 
  dbmsh3d_vertex* s_vertex() const;

  //: Returns the ending vertex for this halfedge based on the direction 
  dbmsh3d_vertex* e_vertex() const;
};


inline bool is_icurve_pair_HE (const dbmsh3d_halfedge* headHE) 
{
  //should check both the pair and next pointer.
  assert (headHE);
  if (headHE->next() == NULL)
    return false;
  if (headHE->next()->next() != headHE)
    return false;
  if (headHE->edge() != headHE->next()->edge())
    return false;
  return true;
}

inline bool is_HE_3_incidence (const dbmsh3d_halfedge* HE1)
{
  //should check both the pair and next pointer.
  assert (HE1);
  if (HE1->next() == NULL)
    return false;
  dbmsh3d_halfedge* HE2 = HE1->next();
  if (HE2->next() == HE1)
    return false; //Loop of 2 HEs.
  dbmsh3d_halfedge* HE3 = HE2->next();
  dbmsh3d_halfedge* HE4 = HE3->next();
  if (HE1->edge() == HE2->edge() && HE1->edge() == HE3->edge()) {
    //No 4-incidence allowed. No single E of 3-incidence allowed.
    assert (HE1->edge() != HE4->edge());
    assert (HE1->face() == HE2->face());
    assert (HE2->face() == HE3->face());
    return true; 
  }
  return false;
}

//: return true if the HE1 is triply incident to one curve inside only one sheet.
//
inline bool is_HE_3_inc_sheet (const dbmsh3d_halfedge* HE1)
{
  //should check both the pair and next pointer.
  assert (HE1);
  if (HE1->next() == NULL)
    return false;
  dbmsh3d_halfedge* HE2 = HE1->next();
  if (HE2->next() == HE1)
    return false; //Loop of 2 HEs.
  if (HE2->face() != HE1->face())
    return false;
  dbmsh3d_halfedge* HE3 = HE2->next();
  if (HE3->face() != HE1->face())
    return false;
  dbmsh3d_halfedge* HE4 = HE3->next();
  if (HE1->edge() == HE2->edge() && HE1->edge() == HE3->edge()) {
    //No 4-incidence allowed. No single E of 3-incidence allowed.
    assert (HE1->edge() != HE4->edge()); 
    return true; 
  }
  return false;
}

//: Return if the pair loop contains 3 or more halfedges with the same edge.
inline bool is_HE_pair_3p_inc (const dbmsh3d_halfedge* HE1)
{
  //should check both the pair and next pointer.
  assert (HE1);
  if (HE1->pair() == NULL)
    return false;
  dbmsh3d_halfedge* HE2 = HE1->pair();
  if (HE2->pair() == HE1)
    return false; //Loop of 2 HEs.
  dbmsh3d_halfedge* HE3 = HE2->pair();
  if (HE1->edge() == HE2->edge() && HE1->edge() == HE3->edge())
    return true; 
  else
    return false;
}

dbmsh3d_halfedge* _find_prev_in_next_chain (const dbmsh3d_halfedge* inputHE);
dbmsh3d_halfedge* _find_prev2_in_next_chain (const dbmsh3d_halfedge* inputHE);
dbmsh3d_halfedge* _find_prev_in_pair_chain (const dbmsh3d_halfedge* inputHE);

dbmsh3d_halfedge* _find_prevHE_in_chain (const dbmsh3d_halfedge* headHE, 
                                         const dbmsh3d_halfedge* inputHE);

unsigned int count_HEs_in_next_loop (const dbmsh3d_halfedge* headHE);

void get_HEvec_next_chain (const dbmsh3d_halfedge* headHE, vcl_vector<dbmsh3d_halfedge*>& HEvec);
void get_HEset_next_chain (const dbmsh3d_halfedge* headHE, vcl_set<dbmsh3d_halfedge*>& HEset);

void get_HEvec_pair_chain (const dbmsh3d_halfedge* headHE, vcl_vector<dbmsh3d_halfedge*>& HEvec);

void add_HE_chain_end (dbmsh3d_halfedge* headHE, dbmsh3d_halfedge* inputHE);

#endif

