#ifndef _dbmsh3d_mesh_bnd_h_
#define _dbmsh3d_mesh_bnd_h_
//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/dbmsh3d_mesh_bnd.h
//:
// \file
// \brief mesh boundary tracing
//
//
// \author
//  MingChing Chang  Dec 14, 2006
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------
#include <vcl_cassert.h>

#include <dbmsh3d/dbmsh3d_mesh.h>

class dbmsh3d_bnd_chain
{
protected:
  vcl_vector<dbmsh3d_halfedge*> HE_list_;

public:
  //=========== Constructor & Destructor ===========
  dbmsh3d_bnd_chain () {
  }
  virtual ~dbmsh3d_bnd_chain () {
    HE_list_.clear();
  }

  //=========== Data Accessing ===========
  vcl_vector<dbmsh3d_halfedge*>& HE_list() {
    return HE_list_;
  }  
  dbmsh3d_halfedge* HE_list (unsigned int i) const {
    return HE_list_[i];
  }
  unsigned int num_edges () const {
    return HE_list_.size();
  }

  //=========== Querying Functions ===========
  bool is_V_incident_via_HE (const dbmsh3d_vertex* V);
  void trace_polyline (vcl_vector<vgl_point_3d<double> >& polyline_vertices);

  //=========== Modification Functions ===========
  void trace_bnd_chain (dbmsh3d_halfedge* startHE);
};

// #########################################################

class dbmsh3d_bnd_chain_set
{
protected:
  dbmsh3d_mesh*     mesh_;
  vcl_vector<dbmsh3d_bnd_chain*> chainset_;

public:
  //=========== Constructor & Destructor ===========
  dbmsh3d_bnd_chain_set (dbmsh3d_mesh* mesh) {
    mesh_ = mesh;
  }
  virtual ~dbmsh3d_bnd_chain_set () {
    for (unsigned int i=0; i<chainset_.size(); i++) {
      _del_bnd_chain (chainset_[i]);
    }
  }

  //=========== Data Accessing ===========
  vcl_vector<dbmsh3d_bnd_chain*>& chainset() {
    return chainset_;
  }

  virtual dbmsh3d_bnd_chain* _new_bnd_chain () {
    return new dbmsh3d_bnd_chain ();
  }
  virtual void _del_bnd_chain (dbmsh3d_bnd_chain* BC) {
    delete BC;
  }

  //=========== Modification Functions ===========
  virtual void detect_bnd_chains ();

  void remove_large_bnd_chain (unsigned int th);
};

#endif

