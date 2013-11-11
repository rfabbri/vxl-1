//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/dbmsh3d_edge.cxx
//:
// \file
// \brief Basic 3d edge
//
//
// \author
//  MingChing Chang  Apr 22, 2005
//
// \verbatim
//  Modifications
//     Jan 22, 2007 Gamze Tunali - added sV() and eV() methods      
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <dbmsh3d/dbmsh3d_halfedge.h>
#include <dbmsh3d/dbmsh3d_edge.h>
#include <dbmsh3d/dbmsh3d_face.h>
#include <dbmsh3d/dbmsh3d_sheet.h>

//: warning: this vertex may be different than the he->edge's sV
dbmsh3d_vertex* dbmsh3d_halfedge::s_vertex() const
{
  dbmsh3d_halfedge* next = this->next();
  dbmsh3d_vertex* e = Es_sharing_V (edge_, next->edge());
  dbmsh3d_vertex* s = edge_->other_V (e);
  return s;
}

//: warning: this vertex may be different than the he->edge's eV
dbmsh3d_vertex* dbmsh3d_halfedge::e_vertex() const
{
  dbmsh3d_halfedge* next = this->next();
  dbmsh3d_vertex* e = Es_sharing_V (edge_, next->edge());
  return e;
}

//######################################################################

dbmsh3d_halfedge* _find_prev_in_next_chain (const dbmsh3d_halfedge* inputHE)
{
  if (inputHE == NULL)
    return NULL;
  if (inputHE->next() == NULL)
    return NULL;
  dbmsh3d_halfedge* HE = (dbmsh3d_halfedge*) inputHE;
  while (HE->next() != inputHE)
    HE = HE->next();
  return HE;
}

dbmsh3d_halfedge* _find_prev2_in_next_chain (const dbmsh3d_halfedge* inputHE)
{
  if (inputHE == NULL)
    return NULL;
  if (inputHE->next() == NULL)
    return NULL;
  dbmsh3d_halfedge* HE = (dbmsh3d_halfedge*) inputHE;
  while (HE->next()->next() != inputHE)
    HE = HE->next();
  return HE;
}

dbmsh3d_halfedge* _find_prev_in_pair_chain (const dbmsh3d_halfedge* inputHE)
{
  if (inputHE == NULL)
    return NULL;
  if (inputHE->pair() == NULL)
    return NULL;
  dbmsh3d_halfedge* HE = (dbmsh3d_halfedge*) inputHE;
  while (HE->pair() != inputHE)
    HE = HE->pair();
  return HE;
}


dbmsh3d_halfedge* _find_prevHE_in_chain (const dbmsh3d_halfedge* headHE, 
                                         const dbmsh3d_halfedge* inputHE)
{
  if (headHE == NULL)
    return NULL;
  
  dbmsh3d_halfedge* HE = (dbmsh3d_halfedge*) headHE;
  do {
    if (HE->next() == inputHE)
      return HE;
    HE = HE->next();
  }
  while (HE != headHE && HE != NULL);
  return NULL;
}


//: return the num of halfedges in the circular chain.
unsigned int count_HEs_in_next_loop (const dbmsh3d_halfedge* headHE)
{
  if (headHE == NULL)
    return 0;
  //Count until reaching the end
  unsigned int count = 0;
  dbmsh3d_halfedge* HE = (dbmsh3d_halfedge*) headHE;
  do {
    count++;
    HE = HE->next();
  }
  while (HE != headHE && HE != NULL);
  return count;
}

void get_HEvec_next_chain (const dbmsh3d_halfedge* headHE, vcl_vector<dbmsh3d_halfedge*>& HEvec)
{
  HEvec.clear();
  if (headHE == NULL)
    return;
  if (headHE->next() == NULL) {
    HEvec.push_back ((dbmsh3d_halfedge*) headHE);
    return;
  }

  dbmsh3d_halfedge* HE = (dbmsh3d_halfedge*) headHE;
  do {
    HEvec.push_back (HE);
    HE = HE->next();
  }
  while (HE != headHE);
}

void get_HEset_next_chain (const dbmsh3d_halfedge* headHE, vcl_set<dbmsh3d_halfedge*>& HEset)
{
  HEset.clear();
  if (headHE == NULL)
    return;
  if (headHE->next() == NULL) {
    HEset.insert ((dbmsh3d_halfedge*) headHE);
    return;
  }

  dbmsh3d_halfedge* HE = (dbmsh3d_halfedge*) headHE;
  do {
    HEset.insert (HE);
    HE = HE->next();
  }
  while (HE != headHE);
}

void get_HEvec_pair_chain (const dbmsh3d_halfedge* headHE, vcl_vector<dbmsh3d_halfedge*>& HEvec)
{
  HEvec.clear();
  if (headHE == NULL)
    return;
  if (headHE->pair() == NULL) {
    HEvec.push_back ((dbmsh3d_halfedge*) headHE);
    return;
  }

  dbmsh3d_halfedge* HE = (dbmsh3d_halfedge*) headHE;
  do {
    HEvec.push_back (HE);
    HE = HE->pair();
  }
  while (HE != headHE);
}

void add_HE_chain_end (dbmsh3d_halfedge* headHE, dbmsh3d_halfedge* inputHE)
{  
  assert (headHE != NULL);
  if (headHE->next() == NULL) {
    headHE->set_next (inputHE);
    inputHE->set_next (headHE);
  }
  else {
    //Trace to the end of the headHE circular list.
    dbmsh3d_halfedge* prevHE = headHE;
    while (prevHE->next() != headHE)
      prevHE = prevHE->next();
    //Add inputHE to the end.
    prevHE->set_next (inputHE);
    inputHE->set_next (headHE);
  }
}


