#include "dbdet_sel_base.h"

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cassert.h>
#include <vcl_deque.h>
#include <vcl_map.h>
#include <vcl_set.h>
#include <vcl_algorithm.h>

//********************************************************************//
// Functions for fitting C^1 polyarcs to edgel chains
//********************************************************************//

//: fit C^1 polyarcs to all the unambiguous edgel chains in the image
void dbdet_sel_base::fit_polyarcs_to_all_edgel_chains()
{
  //go over each edgel chain in the link grpah and try to fit polyarcs to them
  dbdet_edgel_chain_list_iter f_it = curve_frag_graph_.frags.begin();
  for (; f_it != curve_frag_graph_.frags.end(); f_it++)
  {
    dbdet_edgel_chain* chain = (*f_it);

    fit_polyarc_to_chain(chain);
  }

}

double compute_BC_saliency(dbdet_CC_curve_model_3d* cm1, dbdet_CC_curve_model_3d* cm2)
{
  double vol = 0.0;

  for (unsigned i=0; i<cm1->Kmax.rows(); i++)
    for (unsigned j=0; j<cm1->Kmax.cols(); j++)
      if (cm1->Kmax(i,j)>cm1->Kmin(i,j) && cm2->Kmax(i,j)>cm2->Kmin(i,j))
        vol += 1.0;

  return vol;
}

//: attempt to fit polyarcs to edgel chains (knot-based algorithm)
void dbdet_sel_base::fit_polyarc_to_chain(dbdet_edgel_chain* chain)
{
  //Note:
  //   Assume that the basic ENO type curvelets have already been formed at every edgel
  //   To be safe, we might need to check for this
  //
  //   HACK::Also assume that the curve model is CC3d!

  // The knot-based algorithm:
  //
  //   A) data structures: 
  //      1) knot structure : contains a BC bundle + domain indicator
  //      2) 
  //   
  //   B) Algo:
  //      1) add end points as virtual knots
  //      2) Find candidate knot points (BC but not CC)
  //      3) Pick knot points from the candidates and mark the domains
  //      4) Expand each unused point as well as the knot points
  //      5) Repeat from 2) until all edgels are claimed

  unsigned N = chain->edgels.size();

  vcl_vector<bool> bKnots (N, false);
  vcl_vector<bool> cand_knots(N, false);
  vcl_vector<vcl_list<int> > knot_labels(N);
  vcl_vector<dbdet_sel_knot > knots(N);

  vcl_vector<dbdet_CC_curve_model_3d* > CBsb(N), CBsa(N);
  vcl_vector<int> Db(N, 0), Da(N, 0); //domain extents 

  // PrP)initialize the before and after curve bundles from the current curvelets
  for (unsigned i=0; i<N; i++)
  {
    //go over all the curvelets formed at this edgel
    dbdet_edgel* eA = chain->edgels[i];
    curvelet_list_iter cv_it = curvelet_map_.curvelets(eA->id).begin();
    for ( ; cv_it!=curvelet_map_.curvelets(eA->id).end(); cv_it++){
      dbdet_curvelet* cvlet = (*cv_it);

      if (i==0)
        CBsb[i] = new dbdet_CC_curve_model_3d(eA, dpos_, dtheta_, token_len_, max_k_, max_gamma_, badap_uncer_); //default bundle
      else if (cvlet->edgel_chain.back()==eA){
        CBsb[i]= new dbdet_CC_curve_model_3d(*(dbdet_CC_curve_model_3d*)cvlet->curve_model);
        Db[i] = cvlet->edgel_chain.size();
      }

      if (i==N-1)
        CBsa[i]= new dbdet_CC_curve_model_3d(eA, dpos_, dtheta_, token_len_, max_k_, max_gamma_, badap_uncer_);// default bundle at the other end
      else if (cvlet->edgel_chain.front()==eA){
        CBsa[i]= new dbdet_CC_curve_model_3d(*(dbdet_CC_curve_model_3d*)cvlet->curve_model);
        Da[i] = cvlet->edgel_chain.size();
      }
    }
  }

  // 1) add end points as virtual knot points
  bKnots[0] = true;
  bKnots[N-1] = true;

  // 1b) mark the domains of the end points
  if (CBsa[0]){
    for (int j=0; j<Da[0]; j++)
      knot_labels[j].push_back(0);//add the label of the first point
  }

  if (CBsb[N-1]){
    for (int j=0; j<Db[N-1]; j++)
      knot_labels[N-1-j].push_back(N-1);//add the label of the first point
  }

  // repeat the following loop until all edges are marked

  // 2) Find the candidate knot points
  for (unsigned i=0; i<N; i++){
    //cand knot point = not already a knot, not in the domain of a knot AND (!CC && BC)
    if (!bKnots[i] && knot_labels[i].size()==0 && 
        !CBsb[i]->is_C2_with(CBsa[i]) && CBsb[i]->is_C1_with(CBsa[i]))
    {
      cand_knots[i]= true;
      //knots[i] = dbdet_sel_knot();
    }
  }

  // 2b) Check for conflict between candidate knot points
  for (int i=0; i<N; i++)
  {
    vcl_vector<unsigned> conflict;
    if (cand_knots[i]){ //if this is a candidate check its domains for other candidates
      for (int j=-Db[i]+1; j<Da[i]-1; j++) {
        if (i+j>=0 && i+j<N && j!=0 && cand_knots[i+j])
          conflict.push_back(i+j);
      }
    }

    //if there are any conflicts, resolve them right away (using the saliency heuristic)
    if (conflict.size()){
      //find the most salient and delete the others
      double max_sal = compute_BC_saliency(CBsb[i], CBsa[i]);
      unsigned sal_ind = i;
      for (unsigned k=0; k<conflict.size(); k++){
        double sal = compute_BC_saliency(CBsb[conflict[k]], CBsa[conflict[k]]);
        if (sal>max_sal){
          max_sal = sal;
          sal_ind = conflict[k];
        }
      }

      //now delete the other candidate knots but the most salient
      if (sal_ind != i)
        cand_knots[i]= false;

      for (unsigned k=0; k<conflict.size(); k++){
        if (sal_ind != conflict[k])
          cand_knots[conflict[k]] = false;
      }
    }

  }

  // 3) Mark the remaining as new knot points and mark their domains
  for (unsigned i=0; i<N; i++)
  {
    if (cand_knots[i]){
      bKnots[i]= true;

      // also mark their domains
      for (int j=0; j<Da[i]; j++) knot_labels[i+j].push_back(i);//add the label
      for (int j=0; j<Db[i]; j++) knot_labels[i-j].push_back(i);//add the label
    }  
  }

  // 4) Expand the curvelets at each of the knot points and the unclaimed points


  // PoP) Clear the curvelets at the non-knot points and replace the curvelets at the new knot points
  for (unsigned i=0; i<N; i++)
  {
    dbdet_edgel* eA = chain->edgels[i];

    if (bKnots[i]){
      //replace the curvelets with the new ones formed here
    }
    else { 
      //clear the curvelets from here  
      curvelet_list_iter cv_it = curvelet_map_.curvelets(eA->id).begin();
      for ( ; cv_it!=curvelet_map_.curvelets(eA->id).end(); cv_it++)
        delete (*cv_it);
      curvelet_map_.curvelets(eA->id).clear();
    }
  }

  // Extract best polyarc curve from the bundle and set it as the best fits for each of the curvelets




}


