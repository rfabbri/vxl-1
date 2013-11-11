//: dbsk3d_mesh_bnd.cxx
//: MingChing Chang
//  Dec 14, 2006

#include <vcl_iostream.h>
#include <vul/vul_printf.h>
#include <dbsk3d/algo/dbsk3d_mesh_bnd.h>
#include <dbsk3d/dbsk3d_fs_mesh.h>

//: find the shock link element filling the hole.
dbsk3d_fs_edge* dbsk3d_bnd_chain::find_L_for_hole (const dbmsh3d_vertex* Vo, 
                                                       const dbmsh3d_edge* E)
{
  //Loop through all shock patch elms.
  vcl_set<dbsk3d_fs_face*>::iterator it = P_set_.begin();
  for (; it != P_set_.end(); it++) {
    dbsk3d_fs_face* FF = (*it);
    
    dbmsh3d_halfedge* HE = FF->halfedge();
    do {
      dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) HE->edge();
      //Check each incident shock link elm.    
      if (FE->check_fit_hole (Vo, E))
        return FE;
      HE = HE->next();
    }
    while (HE != FF->halfedge());
  }
  return NULL;
}

// #################################################################

bool dbsk3d_bnd_chain_set::detect_bnd_chains_th (const unsigned int th, 
                                                 const unsigned int skip_chain_th)
{
  dbmsh3d_bnd_chain_set::detect_bnd_chains ();

  //Remove bnd_chain larger than the threshold.
  remove_large_bnd_chain (th);

  //If number of bnd_chain too large, return.
  if (chainset_.size() > skip_chain_th) {
    vul_printf (vcl_cout, "\t!! Skip for too many bnd chains (%u, th = %u).\n", 
                chainset_.size(), skip_chain_th);
    return false;
  }

  //Brute-forcely prepare the associated shock sheet elements for each chain.
  
  //Go through all shock patch elements and insert valid ones to bnd_chain.
  vcl_map<int, dbmsh3d_face*>::iterator pit = fs_mesh_->facemap().begin();
  for (; pit != fs_mesh_->facemap().end(); pit++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*pit).second;
    if (FF->b_valid() == false)
      continue;

    //Check if generator on mesh boundary, if so add the shock sheet to bnd_chain.
    const dbmsh3d_vertex* G0 = FF->genes (0);
    VTOPO_TYPE type = G0->detect_vtopo_type();
    if (type == VTOPO_2_MANIFOLD)
      add_P_to_bnd_chain (G0, FF);

    const dbmsh3d_vertex* G1 = FF->genes (1);
    type = G1->detect_vtopo_type();
    if (type == VTOPO_2_MANIFOLD)
      add_P_to_bnd_chain (G1, FF);
  }  

  return true;
}

void dbsk3d_bnd_chain_set::add_P_to_bnd_chain (const dbmsh3d_vertex* G, 
                                               const dbsk3d_fs_face* FF)
{
  //Put FF into the bnd_chain containing G
  vcl_vector<dbmsh3d_bnd_chain*>::iterator it = chainset_.begin();
  for (; it != chainset_.end(); it++) {
    dbsk3d_bnd_chain* BCs = (dbsk3d_bnd_chain*) (*it);
    if (BCs->is_V_incident_via_HE (G)) {
      BCs->_add_fs_patch (FF);
      return;
    }
  }
}

