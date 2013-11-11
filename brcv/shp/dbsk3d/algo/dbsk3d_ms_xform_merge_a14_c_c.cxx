//: dbsk3d_ms_xform_merge_a14_c_c.cxx
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
//  A14 Curve-Curve Merge xform on ms_curves MC and MC2
//#############################################################

//: Given an A13 curve C, search for an A13 curve C2 to merge.
//  Goal is to find the transform candidate with smallest cost (closest event).
//
bool find_A14_c_c_merge_C (const dbsk3d_ms_curve* MC1, dbsk3d_ms_curve*& closest_MC2, 
                           dbsk3d_ms_sheet*& baseMS, 
                           dbmsh3d_vertex*& closest_V1, dbmsh3d_vertex*& closest_V2, 
                           vcl_vector<dbmsh3d_edge*>& shortest_Evec,
                           const int cmxth)
{
  //The resulting MC2 (at cloeset_V2) and the shortest path.
  closest_MC2 = NULL;
  baseMS = NULL;
  closest_V1 = NULL;
  closest_V2 = NULL;
  shortest_Evec.clear();
  unsigned int n_shortest_Evec = INT_MAX;

  //If MC1 is not A13 or dege. A1n, return false.
  if (MC1->c_type() != C_TYPE_AXIAL && MC1->c_type() != C_TYPE_DEGE_AXIAL)
    return false;
  //If MC1 is swallow-tail 3-incidence, return.
  if (is_HE_3_incidence (MC1->halfedge()))
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

  //Search for all sheet MS's incident to MC1, 
  dbmsh3d_halfedge* HE = MC1->halfedge();
  assert (HE->pair());
  do {
    dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) HE->face();

    // - Excluding MC1's starting/ending nodes from the solution.
    // - Find the node MN2 closest to MC1 (at a fine-scale vertex Vc) 
    //   where MN2's incident A13 (or A1n) MC2 is an i-curve of the MS.

    //Put all MS's other A13/A1n bnd or icurve (except 3-incidence and MC1) as candidate MC2_set.
    vcl_set<dbmsh3d_edge*> MC2_set;
    MS->get_axial_nonsw (MC2_set);
    MC2_set.erase ((dbsk3d_ms_curve*) MC1);
    if (MC2_set.empty()) {
      HE = HE->pair();
      continue; //MC2 not found, skip this MS.
    }

    //Collect the candidate fine-scale V's of MN2's in V2_set.
    vcl_set<dbmsh3d_vertex*> V2_set;

    vcl_set<dbmsh3d_edge*>::iterator eit = MC2_set.begin();
    for (; eit != MC2_set.end(); eit++) {
      dbsk3d_ms_curve* MC2 = (dbsk3d_ms_curve*) (*eit);
      assert (MC2->c_type() == C_TYPE_AXIAL || MC2->c_type() == C_TYPE_DEGE_AXIAL);
      if (is_HE_3_incidence (MC2->halfedge()))
        continue; //Skip if MC2 is swallow-tail 3-incidence.
      //Skip MC2 if shares starting/ending node with any MC1.
      if (Es_sharing_V (MC1, MC2))
        continue;

      //Add the interior fine-scale V's of MC2 to V2_set.      
      vcl_vector<dbmsh3d_vertex*> Vvec;
      MC2->get_V_vec (Vvec);
      //Skip the starting/ending cmxth ones.
      for (int i=cmxth; i<int(Vvec.size())-cmxth; i++) { 
        dbmsh3d_vertex* V = Vvec[i];
        V2_set.insert (V);
      }
    }

    if (V2_set.empty()) {
      HE = HE->pair();
      continue; //V2_set empty, skip this MS.
    }

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

      //Find the closest_MC2 from the destV.
      eit = MC2_set.begin();
      for (; eit != MC2_set.end(); eit++) {
        dbsk3d_ms_curve* MC2 = (dbsk3d_ms_curve*) (*eit);
        if (MC2->contain_V (destV)) {
          closest_MC2 = MC2;
          baseMS = MS;
          closest_V1 = srcV;
          closest_V2 = destV;
          break;
        }
      }
      assert (closest_MC2);
    }
    delete MSM;
    HE = HE->pair();
  }
  while (HE != MC1->halfedge());

  //Return the result.
  if (n_shortest_Evec != INT_MAX) {
    assert (shortest_Evec.empty() == false);
    assert (closest_MC2);
    assert (baseMS);
    assert (closest_V1);
    assert (closest_V2);
    return true;
  }
  else
    return false;
}

