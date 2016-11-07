#ifndef dbmsh3d_graph_h_
#define dbmsh3d_graph_h_
//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/dbmsh3d_graph.h
//:
// \file
// \brief Graph
//
//
// \author
//  MingChing Chang  July 05, 2005
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

#include <dbmsh3d/dbmsh3d_vertex.h>
#include <dbmsh3d/dbmsh3d_edge.h>

#include <dbmsh3d/dbmsh3d_pt_set.h>

class dbmsh3d_graph : public dbmsh3d_pt_set
{
protected:
  vcl_map<int, dbmsh3d_edge*> edgemap_;
  int   edge_id_counter_;

public:
  //###### Constructor/Destructor ######
  dbmsh3d_graph () : dbmsh3d_pt_set () {
    edge_id_counter_ = 0;
  }

  void _clear_edgemap () {
    vcl_map<int, dbmsh3d_edge*>::iterator it = edgemap_.begin();
    while (it != edgemap_.end()) {
      dbmsh3d_edge* E = (*it).second;
      remove_edge (E);
      it = edgemap_.begin();
    }
  }

  virtual ~dbmsh3d_graph () {
    _clear_edgemap ();
  }

  virtual void clear () {
    _clear_edgemap();
    edge_id_counter_ = 0;
    dbmsh3d_pt_set::clear();
  }
  virtual bool is_modified() {
    return (edgemap_.size() != 0) || dbmsh3d_pt_set::is_modified();
  }

  //###### Data access functions ######
  vcl_map<int, dbmsh3d_edge*>& edgemap() {
    return edgemap_;
  }
  dbmsh3d_edge* edgemap (const int i) {
    vcl_map<int, dbmsh3d_edge*>::iterator it = edgemap_.find (i);
    if (it == edgemap_.end())
      return NULL;
    return (*it).second;
  }

  const int edge_id_counter() const {
    return edge_id_counter_;
  }
  void set_edge_id_counter (int counter) {
    edge_id_counter_ = counter;
  }

  //###### Connectivity Modification Functions ######
  
  virtual void _del_vertex (dbmsh3d_vertex* V) {
    delete V;
  }

  //: new/delete function of the class hierarchy
  virtual dbmsh3d_edge* _new_edge (dbmsh3d_vertex* sv, dbmsh3d_vertex* ev) {
    return new dbmsh3d_edge (sv, ev, edge_id_counter_++);
  }
  virtual dbmsh3d_edge* _new_edge (dbmsh3d_vertex* sv, dbmsh3d_vertex* ev, const int id) {
    return new dbmsh3d_edge (sv, ev, id);
  }
  virtual void _del_edge (dbmsh3d_edge* E) {
    delete E;
  }

  void _add_edge (dbmsh3d_edge* E) {
    edgemap_.insert (vcl_pair<int, dbmsh3d_edge*>(E->id(), E));
  }

  void add_edge_incidence (dbmsh3d_edge* E) {
    //Add edge.
    _add_edge (E);
    //Add the two edge-vertex incidences.
    dbmsh3d_vertex* sV = E->sV();
    sV->add_incident_E (E);
    dbmsh3d_vertex* eV = E->eV();
    eV->add_incident_E (E);
  }

  void add_edge_incidence_check (dbmsh3d_edge* E) {
    edgemap_.insert (vcl_pair<int, dbmsh3d_edge*>(E->id(), E));
    dbmsh3d_vertex* sV = E->sV();
    sV->check_add_incident_E (E);
    dbmsh3d_vertex* eV = E->eV();
    eV->check_add_incident_E (E);
  }

  //: Create and add the new edge to the mesh
  dbmsh3d_edge* add_new_edge (dbmsh3d_vertex* V1, dbmsh3d_vertex* V2) {    
    dbmsh3d_edge* E = _new_edge (V1, V2);
    add_edge_incidence (E);
    return E;
  }

  void remove_edge (dbmsh3d_edge* E) {    
    //The edge can be deleted only when there's no incident faces (or halfedges).
    assert (E->halfedge() == NULL);
    //Disconnect E from the two vertices
    E->_disconnect_V_idx (0);
    //For loop-edge, the two vertices are the same.
    //In this case the eV here is already NULL.
    if (E->eV())
      E->_disconnect_V_idx (1);
    edgemap_.erase (E->id());
    //Delete the edge using the virtual del function.
    _del_edge (E);
  }
  void remove_edge (int id) {
    dbmsh3d_edge* E = edgemap (id);
    remove_edge (E);
  }

  void remove_edge_complete (dbmsh3d_edge* E) {
    dbmsh3d_vertex* sV = E->sV();
    dbmsh3d_vertex* eV = E->eV();
    remove_edge (E->id());

    try_remove_vertex (sV);
    try_remove_vertex (eV);
  }

  bool try_remove_edge (dbmsh3d_edge* E) {
    if (E->halfedge() == NULL) {
      remove_edge (E->id());
      return true;
    }
    else
      return false;
  }

  //###### Other functions ######
  virtual bool check_integrity ();
  void summary_report ();
};

//###### Graph Processing ######

//: Remove all loops of the input graph G.
void remove_graph_loops (dbmsh3d_graph* G);

void clone_graph (dbmsh3d_graph* targetG, dbmsh3d_graph* inputG);

#endif

