//: dbsk3d_ms_xform_merge_a1a5_n_c.cxx
//: MingChing Chang
//  Jan. 29, 2008

#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <dbmsh3d/dbmsh3d_curve.h>
#include <dbmsh3d/algo/dbmsh3d_sheet_algo.h>
#include <dbmsh3d/algo/dbmsh3d_mesh_algos.h>
#include <dbmsh3d/algo/dbmsh3d_graph_sh_path.h>
#include <dbsk3d/algo/dbsk3d_fs_algo.h>
#include <dbsk3d/algo/dbsk3d_ms_xform.h>

//#############################################################
//  A1A5 Node-Curve Merge xform on ms_node MN and ms_curve MC
//#############################################################

//: Given an A1A3 (or AmAn) node N, search for an A3 curve C to merge.
//  Goal is to find the transform candidate with smallest cost (closest event).
//
bool find_A1A5_n_c_merge_C (const dbsk3d_ms_node* MN1, dbsk3d_ms_curve*& closest_MC2, 
                            dbsk3d_ms_sheet*& baseMS, dbmsh3d_vertex*& cloeset_V, 
                            vcl_vector<dbmsh3d_edge*>& shortest_Evec,
                            const int cmxth)
{
  //The resulting MC2 (at cloeset_V) and the shortest path.
  closest_MC2 = NULL;
  baseMS = NULL;
  cloeset_V = NULL;
  shortest_Evec.clear();
  unsigned int n_shortest_Evec = INT_MAX;

  //If MN is not A1A3 or AmAn (with incident A3), return false.
  if (MN1->n_type() != N_TYPE_RIB_END && MN1->has_rib_C()==false)
    return false;

  //Skip the difficult merging case of AmAn.
  if (MN1->n_E_incidence() != 2)
    return false;

  //The source for searching MC is the MN1.
  vcl_set<dbmsh3d_vertex*> srcV_set;
  srcV_set.insert (MN1->V());
  
  //Search for all ms_sheets incident to any of N's incident curves.
  //Candidate MS that qualifies the xform: 
  // - any of MN1's incident A13 (or A1n) MC1 is a curve of MS.
  // - MC2 is an A3 rib of MS.
  vcl_set<dbmsh3d_face*> incident_MS_set;
  MN1->get_incident_Fs (incident_MS_set);
  
  vcl_set<dbmsh3d_face*>::iterator fit = incident_MS_set.begin();
  for (; fit != incident_MS_set.end(); fit++) {
    dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) (*fit);
    
    //Given MN1 on MS, find the set of curves MC1_set from MS's i-curves.
    vcl_set<dbmsh3d_edge*> MC1_set;
    MS->get_axial_inc_N (MN1, MC1_set);
    if (MC1_set.empty())
      continue; //MC1 not found, skip this MS.

    //Put all MS's A3 rib as candidate MC2_set.
    vcl_set<dbmsh3d_edge*> MC2_set;
    MS->get_incident_A3ribs (MC2_set);
    vcl_set<dbmsh3d_edge*>::iterator eit = MC1_set.begin();
    for (; eit != MC1_set.end(); eit++)
      MC2_set.erase (*eit);

    if (MC2_set.empty())
      continue; //MC2 not found, skip this MS.

    //Collect the set of interior fine-scale V's of candidate MC2's in V2_set.
    vcl_set<dbmsh3d_vertex*> V2_set;

    eit = MC2_set.begin();
    for (; eit != MC2_set.end(); eit++) {
      dbsk3d_ms_curve* MC2 = (dbsk3d_ms_curve*) (*eit);
      assert (MC2->c_type() == C_TYPE_RIB);

      //Add the interior fine-scale V's of MC2 to V2_set.      
      vcl_vector<dbmsh3d_vertex*> Vvec;
      MC2->get_V_vec (Vvec);
      //Skip the starting/ending cmxth ones.
      for (int i=cmxth; i<int(Vvec.size())-cmxth; i++) { 
        dbmsh3d_vertex* V = Vvec[i];
        V2_set.insert (V);
      }
    }

    if (V2_set.empty())
      continue; //V2_set empty, skip this MS.

    //Create a temporary mesh MSM (no need to destruct elements when deleting).
    dbmsh3d_mesh* MSM = new dbmsh3d_mesh (MS->facemap(), false);
    
    //Only search for MS's interior fine-scale A12 edge-elements 
    //  - exclude all bordering A3 and A13 curves (and their end points).
    //  - exclude all interior anchor curves (and their end points).
    vcl_set<dbmsh3d_edge*> MS_incident_FE_set;
    vcl_set<dbmsh3d_vertex*> MS_incident_FV_set;
    MS->get_incident_FEs (MS_incident_FE_set);
    MS->get_incident_FVs (MS_incident_FV_set);

    //Dijkstra search for the closetV in V2_set.
    MSM->reset_face_traversal ();
    vcl_set<dbmsh3d_edge*> avoid_Eset;
    avoid_Eset.insert (MS_incident_FE_set.begin(), MS_incident_FE_set.end());
    vcl_set<dbmsh3d_vertex*> avoid_Vset;
    avoid_Vset.insert (MS_incident_FV_set.begin(), MS_incident_FV_set.end());

    avoid_Vset.erase (MN1->V());
    vcl_set<dbmsh3d_vertex*>::iterator vit = V2_set.begin();
    for (; vit != V2_set.end(); vit++) {
      dbmsh3d_vertex* V = (*vit);
      avoid_Vset.erase (V);
    }

    //Find the the shortest path from MN1 for the cloeset_V (in the V2_set).
    vcl_vector<dbmsh3d_edge*> Evec_path;
    dbmsh3d_vertex *srcV, *destV;
    bool result = find_shortest_Es_on_M_restrained_targets (MSM, srcV_set, V2_set,
                                                            avoid_Eset, avoid_Vset, 
                                                            Evec_path, srcV, destV);
    if (result && Evec_path.size() < n_shortest_Evec) {
      //keep the result of the shortest path.
      n_shortest_Evec = Evec_path.size();
      shortest_Evec.clear();
      shortest_Evec.insert (shortest_Evec.begin(), Evec_path.begin(), Evec_path.end());

      //Find the closest_MC2 from the clstV.
      eit = MC2_set.begin();
      for (; eit != MC2_set.end(); eit++) {
        dbsk3d_ms_curve* MC2 = (dbsk3d_ms_curve*) (*eit);
        //exclude case that MN1 is end_pt of MC2.
        if (MC2->contain_V (destV) && MC2->is_V_incident (MN1) == false) {
          closest_MC2 = MC2;
          baseMS = MS;
          cloeset_V = destV;
          break;
        }
      }
    }
    delete MSM;
  }

  //Return the result.
  if (closest_MC2) {
    assert (baseMS);
    assert (cloeset_V);
    assert (shortest_Evec.empty() == false);
    return true;
  }
  else
    return false;
}

//: Given an A3 curve C, search for an A1A3 (or AmAn) node N to merge.
//  Goal is to find the transform candidate with smallest cost (closest event).
//
bool find_A1A5_n_c_merge_N (const dbsk3d_ms_curve* MC1, dbsk3d_ms_node*& closest_MN2, 
                            dbsk3d_ms_sheet*& baseMS, dbmsh3d_vertex*& cloeset_V, 
                            vcl_vector<dbmsh3d_edge*>& shortest_Evec,
                            const int cmxth)
{
  //The resulting MC2 (at cloeset_V) and the shortest path.
  closest_MN2 = NULL;
  baseMS = NULL;
  cloeset_V = NULL;
  shortest_Evec.clear();
  unsigned int n_shortest_Evec = INT_MAX;

  //If MC1 is not A3 rib, return false.
  if (MC1->c_type() != C_TYPE_RIB)
    return false;
    
  //Use the interior fine-scale elements {Vi} of MC1 
  vcl_vector<dbmsh3d_vertex*> Vvec;
  MC1->get_V_vec (Vvec);
  vcl_set<dbmsh3d_vertex*> srcV_set;
  //Skip the starting/ending cmxth ones.
  for (int i=cmxth; i<int(Vvec.size())-cmxth; i++) { 
    dbmsh3d_vertex* V = Vvec[i];
    srcV_set.insert (V);
  }
  if (srcV_set.empty())
    return false; //no valid source vertex, return.

  //Search on the sheet MS incident to MC1.
  assert (MC1->halfedge()->pair() == NULL);
  dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) MC1->halfedge()->face();

  //find the set of curves MC2_set from MS's i-curves.
  vcl_set<dbmsh3d_edge*> MC2_set;
  MS->get_axial_nonsw (MC2_set);
  if (MC2_set.empty())
    return false; //MC2 not found, skip this MS.
  MC2_set.erase ((dbsk3d_ms_curve*) MC1);

  //Collect the candidate fine-scale V's of MN2's in V2_set.
  // - Excluding MC1's starting/ending nodes from the solution.
  // - Find the node MN2 closest to MC1 (at a fine-scale vertex Vc) 
  //   where MN2's incident A13 (or A1n) MC2 is an i-curve of the MS.
  vcl_set<dbmsh3d_vertex*> V2_set;

  //Loop through each candidate MC2 and prepare to search for MN2.
  vcl_set<dbmsh3d_edge*>::iterator eit = MC2_set.begin();
  for (; eit != MC2_set.end(); eit++) {
    dbsk3d_ms_curve* MC2 = (dbsk3d_ms_curve*) (*eit);
    if (Es_sharing_V (MC1, MC2))
      continue; //Skip MC2 if it shares starting/ending node with MC1.

    //Only look for MN is that is A1A3 or AmAn (with incident A3).
    if (MC2->s_MN()->n_type() == N_TYPE_RIB_END || MC2->s_MN()->has_rib_C())
      V2_set.insert (MC2->s_MN()->V());
    if (MC2->e_MN()->n_type() == N_TYPE_RIB_END || MC2->e_MN()->has_rib_C())      
      V2_set.insert (MC2->e_MN()->V());
  }

  if (V2_set.empty())
    return false; //MN2 candidate fine-scale element not found.

  //Create a temporary mesh MSM (no need to destruct elements when deleting).
  dbmsh3d_mesh* MSM = new dbmsh3d_mesh (MS->facemap(), false);

  //Only search for MS's interior fine-scale A12 edge-elements 
  //  - exclude all bordering A3 and A13 curves (and their end points).
  //  - exclude all interior anchor curves (and their end points).
  vcl_set<dbmsh3d_edge*> MS_incident_FE_set;
  vcl_set<dbmsh3d_vertex*> MS_incident_FV_set;
  MS->get_incident_FEs (MS_incident_FE_set);
  MS->get_incident_FVs (MS_incident_FV_set);

  //Dijkstra search for the closetV in V2_set.
  MSM->reset_face_traversal ();
  vcl_set<dbmsh3d_edge*> avoid_Eset;
  avoid_Eset.insert (MS_incident_FE_set.begin(), MS_incident_FE_set.end());
  vcl_set<dbmsh3d_vertex*> avoid_Vset;
  avoid_Vset.insert (MS_incident_FV_set.begin(), MS_incident_FV_set.end());

  vcl_set<dbmsh3d_vertex*>::iterator vit = srcV_set.begin();
  for (; vit != srcV_set.end(); vit++) {
    dbmsh3d_vertex* V = (*vit);
    avoid_Vset.erase (V);
  }
  vit = V2_set.begin();
  for (; vit != V2_set.end(); vit++) {
    dbmsh3d_vertex* V = (*vit);
    avoid_Vset.erase (V);
  }

  //Find the the shortest path from MN1 for the cloeset_V (in the V2_set).
  vcl_vector<dbmsh3d_edge*> Evec_path;
  dbmsh3d_vertex *srcV, *destV;
  bool result = find_shortest_Es_on_M_restrained_targets (MSM, srcV_set, V2_set,
                                                          avoid_Eset, avoid_Vset, 
                                                          Evec_path, srcV, destV);
  if (result && Evec_path.size() < n_shortest_Evec) {
    //keep the result of the shortest path.
    n_shortest_Evec = Evec_path.size();
    shortest_Evec.clear();
    shortest_Evec.insert (shortest_Evec.begin(), Evec_path.begin(), Evec_path.end());

    //Find the closest_MN2 from the clstV.
    eit = MC2_set.begin();
    for (; eit != MC2_set.end(); eit++) {
      dbsk3d_ms_curve* MC2 = (dbsk3d_ms_curve*) (*eit);
        //exclude case that MN2 is end_pt of MC1.
      if (MC2->s_MN()->V() == destV && MC1->is_V_incident(MC2->s_MN()) == false) {
        closest_MN2 = MC2->s_MN();
        baseMS = MS;
        cloeset_V = destV;
        break;
      }
      else if (MC2->e_MN()->V() == destV && MC1->is_V_incident(MC2->e_MN()) == false) {
        closest_MN2 = MC2->e_MN();
        baseMS = MS;
        cloeset_V = destV;
        break;
      }
    }
  }
  delete MSM;

  //Return the result.
  if (closest_MN2) {
    assert (baseMS);
    assert (cloeset_V);
    assert (shortest_Evec.empty() == false);
    return true;
  }
  else
    return false;
}


