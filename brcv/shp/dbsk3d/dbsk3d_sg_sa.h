//: This is dbsk3d/dbsk3d_sg_sa.h
//  MingChing Chang
//  Nov 14, 2004.

#ifndef dbsk3d_sg_sa_h_
#define dbsk3d_sg_sa_h_

#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_utility.h>

#include <dbmsh3d/dbmsh3d_graph.h>
#include <dbsk3d/dbsk3d_fs_vertex.h>
#include <dbsk3d/dbsk3d_ms_node.h>
#include <dbsk3d/dbsk3d_ms_curve.h>

#define VALUE_NOT_DETERMINED -1.0f

class dbsk3d_sg_sa : public dbmsh3d_graph
{
protected:
  //: Datastructure for each fs_vertex
  vcl_map<int, dbsk3d_fs_vertex*> FV_map_;

  int     FV_id_counter_;
  float   median_edge_len_;

public:
  //###### Constructor & Destructor ######
  dbsk3d_sg_sa () : dbmsh3d_graph() {
    FV_id_counter_ = 0;
    median_edge_len_ = VALUE_NOT_DETERMINED;
  }

  void clear () {
    _clear_edgemap ();

    vcl_map<int, dbsk3d_fs_vertex*>::iterator it = FV_map_.begin();
    for (; it != FV_map_.end(); it++) {
      dbsk3d_fs_vertex* FV = (*it).second;

      //: tricky here. before deleting the FV (of a sg_sa),
      //  first clean up it's connectivity
      //  because the connectivity here is really
      //  not a tight dual-pointing structure
      FV->get_Gs_from_FFs().clear();
      _del_vertex (FV);
    }
    FV_map_.clear ();
  }

  virtual ~dbsk3d_sg_sa () {
    clear ();
  }

  //###### Data Access Functions ######
  int FV_id_counter () const {
    return FV_id_counter_;
  }
  void set_FV_id_counter (const int counter) {
    if (FV_id_counter_ < counter)
      FV_id_counter_ = counter;
  }
  void inc_FV_id_counter () {
    FV_id_counter_++;
  }

  float median_edge_len () const {
    return median_edge_len_;
  }

  vcl_map<int, dbsk3d_fs_vertex*>& FV_map () {
    return FV_map_;
  }

  //###### Internal Low-level Object creation/deletion ######
  //: new/delete function of the class hierarchy
  //  Because dbskr_gradasn_shock is a derived class for dbsk3d_scaffold_graph,
  //  and dbskr_ms_gradasn_vertex is a derived class for dbsk3d_ms_node,
  //  we need a virtual function for allocating new node.
  //  For dbsk3d_scaffold_graph, it's just new dbsk3d_ms_node()
  //  for dbskr_ms_gradasn_vertex, it's new dbskr_ms_gradasn_vertex()
  virtual dbmsh3d_vertex* _new_vertex () {
    return new dbsk3d_ms_node (vertex_id_counter_++);
  }
  virtual dbmsh3d_vertex* _new_vertex (const int id) {
    if (vertex_id_counter_ <= id)
      vertex_id_counter_ = id+1;
    return new dbsk3d_ms_node (id);
  }
  virtual void _del_vertex (dbmsh3d_vertex* V) {
    dbsk3d_ms_node* MN = (dbsk3d_ms_node*) V;
    delete MN;
  }
  virtual dbmsh3d_edge* _new_edge (dbmsh3d_vertex* sV, dbmsh3d_vertex* eV) {
    return new dbsk3d_ms_curve (sV, eV, edge_id_counter_++);
  }
  virtual dbmsh3d_edge* _new_edge (dbmsh3d_vertex* sv, dbmsh3d_vertex* ev, const int id) {
    return new dbsk3d_ms_curve (sv, ev, id);
  }

  dbsk3d_fs_vertex* FV_map (const int i) {
    vcl_map<int, dbsk3d_fs_vertex*>::iterator it = FV_map_.find (i);
    if (it == FV_map_.end())
      return NULL;    
    return (*it).second;
  }

  virtual dbsk3d_fs_vertex* _new_FV () {
    return new dbsk3d_fs_vertex (FV_id_counter_++);
  }
  virtual dbsk3d_fs_vertex* _new_FV (const int id) {
    if (FV_id_counter_ <= id)
      FV_id_counter_ = id+1;
    return new dbsk3d_fs_vertex (id);
  }
  void _add_FV (const dbsk3d_fs_vertex* FV) {
    int id = FV->id();
    ///assert (FV_map(id) == NULL); //Make sure there is no duplicate in the map. !!!
    FV_map_.insert (vcl_pair<int, dbsk3d_fs_vertex*> (FV->id(), (dbsk3d_fs_vertex*) FV));

    vcl_map<int, dbsk3d_fs_vertex*>::reverse_iterator rit = FV_map_.rbegin();
    int rid = (*rit).second->id();
    assert (id <= rid);
    if (FV_id_counter_ <= rid)
      FV_id_counter_ = rid + 1;
  }
  bool _del_FV (const dbsk3d_fs_vertex* FV) {
    //: no need to check connectivity.
    //  Here each FV just serves as a point.
    int n = FV_map_.erase (FV->id());
    assert (n==0 || n==1);
    return true;
  }

  //###### Other functions ######
  virtual bool check_integrity ();

  void compute_median_edge_len ();

};

void clone_sg_sa (dbsk3d_sg_sa* targetSG, dbsk3d_sg_sa* inputSG);

#endif
