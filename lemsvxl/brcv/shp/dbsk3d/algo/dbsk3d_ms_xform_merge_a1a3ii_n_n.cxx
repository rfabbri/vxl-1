//: dbsk3d_ms_xform_merge_a1a3ii_n_n.cxx
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
//  A1A3-II Node-Node Merge xform on ms_nodes MN and MN2
//#############################################################

//: Given a ms_node MN, try finding a ms_node MN2 for the transform.
//  Goal is to find the transform candidate with smallest cost (closest event).
//
bool find_A1A3II_n_n_merge_N (const dbsk3d_ms_node* MN1, dbsk3d_ms_node*& closest_MN2, 
                              dbsk3d_ms_sheet*& baseMS, 
                              vcl_vector<dbmsh3d_edge*>& shortest_Evec)
{
  //The resulting MN2 and the shortest path.
  closest_MN2 = NULL;
  baseMS = NULL;
  shortest_Evec.clear();
  unsigned int n_shortest_Evec = INT_MAX;

  //1) If MN or MN2 not A1A3, return false.
  //MN and MN2 could be either A1A3 or degenerate AmAn (with incident A3).
  if (MN1->n_type() != N_TYPE_RIB_END && MN1->has_rib_C()==false)
    return false;

  //The source for searching MN2 is the MN1.
  vcl_set<dbmsh3d_vertex*> srcV_set;
  srcV_set.insert (MN1->V());

  //2) Search for all ms_sheets incident to any of N's incident curves.
  //   Candidate MS that qualifies the xform: 
  //     - any of MN1's incident A13 (or A1n) tabMC1 is an i-curve of MS.
  //     - any of MN2's incident A13 (or A1n) tabMC2 is an i-curve of MS.
  vcl_set<dbmsh3d_face*> incident_MS_set;
  MN1->get_incident_Fs (incident_MS_set);
  
  vcl_set<dbmsh3d_face*>::iterator fit = incident_MS_set.begin();
  for (; fit != incident_MS_set.end(); fit++) {
    dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) (*fit);
    
    //Given MN1 on MS, find the set of curves MC1_set from MS's i-curves.
    vcl_set<dbmsh3d_edge*> MC1_set;
    MS->get_ICpairs_inc_N (MN1, MC1_set);
    if (MC1_set.empty())
      continue; //tabMC1 not found, skip this MS.

    if (MN1->n_incident_Es() != 2)
      continue; //skip this MN1 if more than one tabMC1 incident to MN1.

    //Put all MS's other icurve-pairs not in MC1_set as candidate MC2_cand.
    vcl_set<dbmsh3d_edge*> MC2_cand;
    MS->get_icurve_pairs (MC2_cand);

    vcl_set<dbmsh3d_edge*>::iterator eit = MC1_set.begin();
    for (; eit != MC1_set.end(); eit++)
      MC2_cand.erase (*eit);

    if (MC2_cand.empty())
      continue; //tabMC2 not found, skip this MS.

    //Create a temporary mesh MSM (no need to destruct elements when deleting).
    dbmsh3d_mesh* MSM = new dbmsh3d_mesh (MS->facemap(), false);
    
    //Only search for MS's interior fine-scale A12 edge-elements 
    //  - exclude all bordering A3 and A13 curves (and their end points).
    //  - exclude all interior anchor curves (and their end points).
    vcl_set<dbmsh3d_edge*> MS_incident_FE_set;
    vcl_set<dbmsh3d_vertex*> MS_incident_FV_set;
    MS->get_incident_FEs (MS_incident_FE_set);
    MS->get_incident_FVs (MS_incident_FV_set);

    //Collect the set of candidate MN2's in MN2_set
    //together with their fine-scale V's in FV2_set.
    vcl_set<dbsk3d_ms_node*> MN2_set;
    vcl_set<dbmsh3d_vertex*> V2_set;

    //Loop through all MC2_cand and check each's starting/ending vertex as MN2.
    eit = MC2_cand.begin();
    for (; eit != MC2_cand.end(); eit++) {
      dbsk3d_ms_curve* tabMC2 = (dbsk3d_ms_curve*) (*eit);
      //Skip tabMC2 if shares starting/ending node with any tabMC1.
      bool skip_MC2 = false;
      vcl_set<dbmsh3d_edge*>::iterator eit1 = MC1_set.begin();
      for (; eit1 != MC1_set.end(); eit1++) {
        dbsk3d_ms_curve* tabMC1 = (dbsk3d_ms_curve*) (*eit1);
        if (tabMC2->is_V_incident (tabMC1->other_V (MN1))) {
          skip_MC2 = true;
          break;
        }
      }
      if (skip_MC2)
        continue;

      for (int i=0; i<2; i++) {
        dbsk3d_ms_node* MN2 = (dbsk3d_ms_node*) tabMC2->vertices(i);
        if (MN2->n_type() != N_TYPE_RIB_END && MN2->has_rib_C() == false)
          continue; //skip this MN2 of invalid type.
        
        if (MN2->n_incident_Es() != 2)
          continue; //skip this MN2 if more than one tabMC1 incident to MN1.

        MN2_set.insert (MN2);
        V2_set.insert (MN2->V());
      }
    }

    //Dijkstra search for the closet MN2 in MN2_set.
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

    //Find the the shortest path from MN1 for the destV (in the V2_set).
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

      //Find the closest_MN2 from the destV.
      vcl_set<dbsk3d_ms_node*>::iterator nit = MN2_set.begin();
      for (; nit != MN2_set.end(); nit++) {
        dbsk3d_ms_node* MN2 = (*nit);
        if (MN2->V() == destV) {
          closest_MN2 = MN2;
          baseMS = MS;
          break;
        }
      }
      assert (closest_MN2);
    }
    delete MSM;
  }

  //Return the result.
  if (n_shortest_Evec != INT_MAX) {
    assert (shortest_Evec.empty() == false);
    assert (closest_MN2);
    assert (baseMS);
    return true;
  }
  else
    return false;
}



