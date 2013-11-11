#ifndef dbmsh3d_hypg_h_
#define dbmsh3d_hypg_h_
//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/dbmsh3d_hypg.h
//:
// \file
// \brief Hypergraph
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

#include <dbmsh3d/dbmsh3d_node.h>
#include <dbmsh3d/dbmsh3d_curve.h>
#include <dbmsh3d/dbmsh3d_sheet.h>
#include <dbmsh3d/dbmsh3d_graph.h>
#include <dbmsh3d/dbmsh3d_mesh.h>

//: Definition of a geometric hypergraph on top of an underlying (non-manifold) mesh.

class dbmsh3d_hypg : public dbmsh3d_graph
{
protected:
  dbmsh3d_mesh* mesh_;
  
  vcl_map<int, dbmsh3d_sheet*> sheetmap_;
  int   sheet_id_counter_;

public:
  //###### Constructor/Destructor ######
  dbmsh3d_hypg (const dbmsh3d_mesh* M) : dbmsh3d_graph () {
    mesh_ = (dbmsh3d_mesh*) M;
    sheet_id_counter_ = 0;
  }
  
  void _clear_sheetmap () {
    vcl_map<int, dbmsh3d_sheet*>::iterator it = sheetmap_.begin();
    while (it != sheetmap_.end()) {
      dbmsh3d_sheet* S = (*it).second;
      remove_sheet (S);
      it = sheetmap_.begin();
    }
    sheet_id_counter_ = 0;
  }
  void _clear_all_S_shared_F () {
    //Clear the shared_F_list for all sheets.
    vcl_map<int, dbmsh3d_sheet*>::iterator it = sheetmap_.begin();
    for (; it != sheetmap_.end(); it++) {
      dbmsh3d_sheet* S = (*it).second;
      S->clear_shared_F_list();
    }
  }
  void _clear_all_C_shared_E () {
    //Clear the shared_E_list for all curves.
    vcl_map<int, dbmsh3d_edge*>::iterator it = edgemap_.begin();
    for (; it != edgemap_.end(); it++) {
      dbmsh3d_curve* C = (dbmsh3d_curve*) (*it).second;
      C->clear_shared_E_list();
    }
  }

  virtual ~dbmsh3d_hypg () {
    mesh_ = NULL;
    _clear_all_S_shared_F ();
    _clear_sheetmap ();
    _clear_all_C_shared_E ();
  }

  virtual void clear () {
    _clear_sheetmap();
    dbmsh3d_graph::clear();
    mesh_->clear ();
  }
  virtual bool is_modified() {
    return (sheetmap_.size() != 0) || dbmsh3d_graph::is_modified();
  }

  //###### Data access functions ######
  const dbmsh3d_mesh* mesh() const {
    return mesh_;
  }
  dbmsh3d_mesh* mesh() {
    return mesh_;
  }
  void set_mesh (dbmsh3d_mesh* M) {
    mesh_ = M;
  }
  const int sheet_id_counter() const {
    return sheet_id_counter_;
  }
  void set_sheet_id_counter (int counter) {
    sheet_id_counter_ = counter;
  }

  vcl_map<int, dbmsh3d_sheet*>& sheetmap () {
    return sheetmap_;
  }
  dbmsh3d_sheet* sheetmap (const int i) {
    vcl_map<int, dbmsh3d_sheet*>::iterator it = sheetmap_.find (i);
    if (it == sheetmap_.end())
      return NULL;    
    return (*it).second;
  }
  
  //###### Connectivity Query Functions ######

  //###### Connectivity Modification Functions ######

  //: new/delete function of the class hierarchy
  virtual dbmsh3d_vertex* _new_vertex () {
    return new dbmsh3d_node (vertex_id_counter_++);
  }  
  virtual void _del_vertex (dbmsh3d_vertex* V) {
    dbmsh3d_node* N = (dbmsh3d_node*) V;
    delete N;
  }

  virtual dbmsh3d_edge* _new_edge (dbmsh3d_vertex* sV, dbmsh3d_vertex* eV) {
    return new dbmsh3d_curve (sV, eV, edge_id_counter_++);
  }
  virtual void _del_edge (dbmsh3d_edge* E) {
    dbmsh3d_curve* C = (dbmsh3d_curve*) E;
    delete C;
  }

  //: new/delete function of the class hierarchy
  virtual dbmsh3d_sheet* _new_sheet () {
    return new dbmsh3d_sheet (sheet_id_counter_++);
  }
  virtual void _del_sheet (dbmsh3d_sheet* S) {
    delete S;
  }

  void _add_sheet (dbmsh3d_sheet* S) {
    sheetmap_.insert (vcl_pair<int, dbmsh3d_sheet*>(S->id(), S));
  }

  //: Disconnect all associated halfedges 
  //  from their incident edges and delete them.
  bool remove_sheet (int id) {
    dbmsh3d_sheet* S = sheetmap (id);
    if (S == NULL)
      return false;
    S->disconnect_all_incident_Es ();
    sheetmap_.erase (id);
    _del_sheet (S);
    return true;
  }
  bool remove_sheet (dbmsh3d_sheet* S) {
    return remove_sheet (S->id());
  }

  //: Delete the sheet S, also remove all curves and vertices only incident to S.
  //  This ensures the hypergraph is a complete geometric hypergraph.
  void remove_S_complete_hypg (dbmsh3d_sheet* S);

  void remove_S_complete_hypg (dbmsh3d_sheet* S, 
                               vcl_set<dbmsh3d_edge*>& remaining_Cs,
                               vcl_set<dbmsh3d_vertex*>& remaining_Ns);

  void remove_S_complete_fix (vcl_set<dbmsh3d_edge*>& remaining_Cs, vcl_set<dbmsh3d_vertex*>& remaining_Ns,
                              vcl_set<dbmsh3d_edge*>& removed_Cs, vcl_set<dbmsh3d_vertex*>& removed_Ns);
  
  //: Merge two adjacent curves C1 and C2 sharing a node N.
  //  Keep C1 and merge all edge elements of C2 to C1.
  void merge_Cs_sharing_N (dbmsh3d_node* N, dbmsh3d_curve* C1, dbmsh3d_curve* C2);
  
  //: Merge two adjacent edges sharing a vertex V.
  void merge_Es_sharing_V (const dbmsh3d_vertex* V, dbmsh3d_edge* E1, dbmsh3d_edge* E2);

  //###### Other functions ######
  //: Loop through all elements and check integrity
  virtual bool check_integrity ();
  
  virtual dbmsh3d_hypg* clone (dbmsh3d_mesh* M2);
  virtual dbmsh3d_pt_set* clone ();  
};

void _merge_E_vec_C2_to_C1 (dbmsh3d_node* N, dbmsh3d_curve* C1, dbmsh3d_curve* C2);

void dbmsh3d_hypg_print_object_size ();

#endif

