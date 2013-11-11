//: dbsk3d/dbsk3d_fs_inflow_prop.h
//  Surface In-Flow Generator Propagation on the Shocks.
//  MingChing Chang
//  June 26, 2007        Creation.

#ifndef dbsk3d_fs_inflow_prop_h_
#define dbsk3d_fs_inflow_prop_h_

#include <vcl_map.h>
#include <dbsk3d/dbsk3d_fs_mesh.h>

class dbsk3d_fs_inflow_prop
{
protected:
  dbsk3d_fs_mesh* fs_mesh_;

  //: The priority queue of shock curve or shock sheet elements.  
  vcl_multimap<double, vispt_elm*>  PQ_;

public:
  //====== Constructor & Destructor ======
  dbsk3d_fs_inflow_prop (dbsk3d_fs_mesh* fs) {
    fs_mesh_          = fs;
  }
  ~dbsk3d_fs_inflow_prop () {
    PQ_.clear();
  }

  //====== Data Accessing ======
  dbmsh3d_mesh* bnd_mesh() {
    return fs_mesh_->bnd_mesh();
  }
  void set_fs_mesh (dbsk3d_fs_mesh* fs_mesh) {
    fs_mesh_ = fs_mesh;
  } 

  vcl_multimap<double, vispt_elm*>& PQ () {
    return PQ_;
  }

  void add_FF_to_PQ (const double r, const dbsk3d_fs_face* FF) {
    bool result = _brute_force_find_in_PQ (FF);
    assert (result == false);
    PQ_.insert (vcl_pair<double, dbsk3d_fs_face*> (r, (dbsk3d_fs_face*) FF));
  } 
  //: Note that identical FE can be added with different r in PQ several times.
  //  We only process FE if all of its incoming FF's are visited.
  void add_FE_to_PQ (const double r, const dbsk3d_fs_edge* FE) {
    PQ_.insert (vcl_pair<double, dbsk3d_fs_edge*> (r, (dbsk3d_fs_edge*) FE));
  }  

  bool _brute_force_find_in_PQ (const vispt_elm* E) {
    vcl_multimap<double, vispt_elm*>::iterator it = PQ_.begin();
    for (; it != PQ_.end(); it++) {
      if ((*it).second == E)
        return true;
    }
    return false;
  }

  //====== Surface In-Flow Generator Propagation ======
  void compute_surface_inflow ();

  void surface_inflow_gene_init ();
  void surface_inflow_gene_prop ();

  //Save the reconstructed surface mesh.
  void build_surface_mesh ();

};


#endif
