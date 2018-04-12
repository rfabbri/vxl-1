#include "dbdet_sel_base.h"

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cassert.h>
#include <vcl_deque.h>
#include <vcl_map.h>
#include <vcl_set.h>
#include <vcl_algorithm.h>
#include <vcl_queue.h>
#include <vnl_cross.h>


//********************************************************************//
// Functions for constructing hypothesis trees and
// tracing between contour end points.
//********************************************************************//


#define SM_TH 0.6

#define Theta_1 0.6
#define Theta_2 0.0
#define Strength_Diff1 2.0
#define Strength_Diff2 4.0
#define INF_COST 0xffffff

dbdet_EHT* dbdet_sel_base::construct_hyp_tree(dbdet_edgel* edge)
{
  if (edge_link_graph_.cLinks.size()==0){
    vcl_cout << "No Link Graph !" <<vcl_endl;
    return 0;
  }


 //construct 2 HTs: one in the forward direction and one in the reverse direction ????  by yuliang no forword
//Modify: for each node, consider all the child links and parent links(exact the one linking parent node), as its child node
  vcl_queue<dbdet_EHT_node*> BFS_queue;

  //forward HT
  dbdet_EHT* HTF = new dbdet_EHT();

  dbdet_EHT_node* root1 = new dbdet_EHT_node(edge);
  HTF->root = root1;
  BFS_queue.push(root1);

  int depth = 0; // comment by Yuliang, this is not the depth of the tree, but number of nodes actually

  //How far do we wanna go (if we don't hit a node)?
  while (!BFS_queue.empty() && vcl_log10(double(depth))<3)
  {
    dbdet_EHT_node* cur_node = BFS_queue.front();
    BFS_queue.pop();

    //are we at a CFG node? if we are we don't need to go any further
    if (cur_node!= root1 &&
        (curve_frag_graph_.pFrags[cur_node->e->id].size()>0 ||
         curve_frag_graph_.cFrags[cur_node->e->id].size()>0))
      continue;

    //also if we hit an edgel that is already linked no need to go further (this might ensure planarity)
    if (cur_node!= root1 && edge_link_graph_.linked[cur_node->e->id])
      continue;

    //propagate this node
    dbdet_link_list_iter lit = edge_link_graph_.cLinks[cur_node->e->id].begin();
    for (; lit != edge_link_graph_.cLinks[cur_node->e->id].end(); lit++)
    {
      if (edge_link_graph_.linked[(*lit)->ce->id]) //don't go tracing in linked contours
        continue;
  
      if (cur_node->parent) {
        if((*lit)->ce->id == cur_node->parent->e->id)// if this child link the same from parent node, don't trace
          continue;
        //make a simple consistency check
        double dx1 = cur_node->e->pt.x() - cur_node->parent->e->pt.x();
        double dy1 = cur_node->e->pt.y() - cur_node->parent->e->pt.y();
        double dx2 = (*lit)->ce->pt.x() - cur_node->e->pt.x();
        double dy2 = (*lit)->ce->pt.y() - cur_node->e->pt.y();

        if (((dx1*dx2 + dy1*dy2)/vcl_sqrt(dx1*dx1+dy1*dy1)/vcl_sqrt(dx2*dx2+dy2*dy2))<SM_TH) //not consistent, but with lower TH to keep more Hypothesis by Yuliang ////////// Cosine Formula
          continue;
      }

      //else extend the tree to this edgel
      dbdet_EHT_node* new_node = new dbdet_EHT_node((*lit)->ce);

      cur_node->add_child(new_node);
      BFS_queue.push(new_node);
      depth++;
    }
    // by Yuliang
    // explore in pLinks
    lit = edge_link_graph_.pLinks[cur_node->e->id].begin();
    for (; lit != edge_link_graph_.pLinks[cur_node->e->id].end(); lit++)
    {
      if (edge_link_graph_.linked[(*lit)->pe->id]) //don't go tracing in linked contours
        continue;

      if (cur_node->parent) {
        if((*lit)->pe->id == cur_node->parent->e->id)// if this parent link the same from parent node, don't trace
		  continue;
		//make a simple consistency check
		double dx1 = cur_node->e->pt.x() - cur_node->parent->e->pt.x();
		double dy1 = cur_node->e->pt.y() - cur_node->parent->e->pt.y();
		double dx2 = (*lit)->pe->pt.x() - cur_node->e->pt.x();
		double dy2 = (*lit)->pe->pt.y() - cur_node->e->pt.y();

		if (((dx1*dx2 + dy1*dy2)/vcl_sqrt(dx1*dx1+dy1*dy1)/vcl_sqrt(dx2*dx2+dy2*dy2))<SM_TH) //not consistent, but with lower TH to keep more Hypothesis by Yuliang
		  continue;
      }

      //else extend the tree to this edgel
      dbdet_EHT_node* new_node = new dbdet_EHT_node((*lit)->pe);

      cur_node->add_child(new_node);
      BFS_queue.push(new_node);
      depth++;
    }
  }

  //empty the bfs queue
  while (!BFS_queue.empty())
    BFS_queue.pop();

  return HTF;
}

//: construct all possible EHTs from the terminal nodes and find legal contour paths
void dbdet_sel_base::construct_all_path_from_EHTs()
{
  // modify by Yuliang, use a local filter for contours size 3 and 4 to prune some, and merge some continuous regular contours first
  regular_contour_filter();
  //go over the contour fragment graph and form an EHT from every terminal node
  //validate each of the paths in the EHT

  vcl_vector<dbdet_edgel_chain*> new_frags;

  //going over the edgemap instead so that an EHT only starts once from a node when there are two
  //contour fragments terminating there
  for (unsigned i=0; i<edgemap_->edgels.size(); i++)
  {
    dbdet_edgel* eA = edgemap_->edgels[i];

    if (curve_frag_graph_.pFrags[i].size()==0 &&
        curve_frag_graph_.cFrags[i].size()==0)
      continue; //no terminal nodes here

    //1) Terminal node found, construct an EHT from here
    dbdet_EHT* EHT1 = construct_hyp_tree(eA);

    //2) traverse each path and determine its validity
    // In fact, this algorithm iteratively finds a leaf node, and its path to root
    if (EHT1)
    {
      //traverse the EHT and test all the paths
      dbdet_EHT::path_iterator pit = EHT1->path_begin();
      for (; pit != EHT1->path_end(); pit++)
      {

    	// edgel_chain: a path starting from the root and end at current node
		vcl_vector<dbdet_edgel*>& edgel_chain = pit.get_cur_path();

		dbdet_edgel* le = edgel_chain.back();

		if (curve_frag_graph_.pFrags[le->id].size()==0 && curve_frag_graph_.cFrags[le->id].size()==0)
		{
		  //not a valid termination node
		  //delete the node associated  with this path ( it will delete the entire path, by definition)
		  EHT1->delete_subtree(pit);
		  continue;
		}

		//test this path to see if it is valid
		if (!is_EHT_path_legal(edgel_chain)){
		  EHT1->delete_subtree(pit);
		  continue;
		}

		//copy this chain
		dbdet_edgel_chain* new_chain = new dbdet_edgel_chain();
		new_chain->append(edgel_chain);
		new_chain->temp = true; //make sure that the new frags are initialized with temp flags


		curve_frag_graph_.CFTG.insert_fragment(new_chain);

		//now that its added delete the path
		EHT1->delete_subtree(pit); // trace pit up to where this path bifurcates
      }
      //finally delete the EHT
      delete EHT1;
    }
  }
  vcl_cout<<"Finish constructing all hypothesis trees"<<vcl_endl;
  ////Now add all the new curve fragments into the CFG (as tentative fragments)
  //for (unsigned i=0; i<new_frags.size(); i++)
  //  curve_frag_graph_.insert_fragment(new_frags[i]);
}

//: perform a geometric consistency check to determine whether a given temp path is valid
bool dbdet_sel_base::is_EHT_path_legal(vcl_vector<dbdet_edgel*>& edgel_chain)
{
  //what makes a path legal?
  if(edgel_chain.size()==1)
	  return false;
  else if(edgel_chain.size()==2)   //remove direct link of two edges over 5 pixels
  {
      dbdet_edgel* eS = edgel_chain.front();
      dbdet_edgel* eE = edgel_chain.back();
      double dx1 = eE->pt.x() - eS->pt.x();
      double dy1 = eE->pt.y() - eS->pt.y();
      double dist= vcl_sqrt(dx1*dx1+dy1*dy1);
      if(dist>5)
         return false;
   }
  else // the first step should not jump too far, this is for junction resolving stage
  {
	  int sz = edgel_chain.size();
	  dbdet_edgel* eS = edgel_chain.front();
	  dbdet_edgel* eE = edgel_chain[1];
	  double dx1 = eE->pt.x() - eS->pt.x();
	  double dy1 = eE->pt.y() - eS->pt.y();
	  double dist= vcl_sqrt(dx1*dx1+dy1*dy1);
	  if(dist>2)
	    return false;
  }

  // by Yuliang, construct two lists of end nodes from other Curve Fragments end points linking the end points of the path
  vcl_vector<dbdet_edgel*> S_link_end_nodes, E_link_end_nodes;

  // (a) if a c1 polyarc bundle can form within it
  // (b) if it is c1 compatible with the end points

  //For now, just check for...

  //1) continuity consistency at the end points
  //1a) at the start point
  dbdet_edgel* eS = edgel_chain.front();
  dbdet_edgel* e2 = edgel_chain[1];
  double dx1 = e2->pt.x() - eS->pt.x();
  double dy1 = e2->pt.y() - eS->pt.y();
  bool cons = false;


  //Is the starting point consistent with at least one of the connecting unambiguous chains? : angel < 90 degree
  dbdet_edgel_chain_list_iter pcit = curve_frag_graph_.pFrags[eS->id].begin();
  for ( ; pcit != curve_frag_graph_.pFrags[eS->id].end(); pcit++)
  {
    dbdet_edgel* pe = (*pcit)->edgels[(*pcit)->edgels.size()-2];
    S_link_end_nodes.push_back((*pcit)->edgels.front());
 
    //make a simple consistency check for child Frags
    double dx2 = eS->pt.x() - pe->pt.x();
    double dy2 = eS->pt.y() - pe->pt.y();

    cons = cons || ((dx1*dx2 + dy1*dy2)/vcl_sqrt(dx1*dx1+dy1*dy1)/vcl_sqrt(dx2*dx2+dy2*dy2))>0;

    if((*pcit)->edgels.size()>=3) // a little more globle check, in case of local zig-zag
    {
    	pe = (*pcit)->edgels[(*pcit)->edgels.size()-3];
    	dx2 = eS->pt.x() - pe->pt.x();
    	dy2 = eS->pt.y() - pe->pt.y();
        cons = cons || ((dx1*dx2 + dy1*dy2)/vcl_sqrt(dx1*dx1+dy1*dy1)/vcl_sqrt(dx2*dx2+dy2*dy2))>0;
    }
  }
  dbdet_edgel_chain_list_iter ccit = curve_frag_graph_.cFrags[eS->id].begin();
  for ( ; ccit != curve_frag_graph_.cFrags[eS->id].end(); ccit++)
  {
    dbdet_edgel* ce = (*ccit)->edgels[1];
    S_link_end_nodes.push_back((*ccit)->edgels.back());

    //make a simple consistency check for parent Frags
    double dx2 = eS->pt.x() - ce->pt.x();
    double dy2 = eS->pt.y() - ce->pt.y();

    cons = cons || ((dx1*dx2 + dy1*dy2)/vcl_sqrt(dx1*dx1+dy1*dy1)/vcl_sqrt(dx2*dx2+dy2*dy2))>0;
    if((*ccit)->edgels.size()>=3) // a little more globle check, in case of local zig-zag
    {
    	ce = (*ccit)->edgels[2];
    	dx2 = eS->pt.x() - ce->pt.x();
    	dy2 = eS->pt.y() - ce->pt.y();
        cons = cons || ((dx1*dx2 + dy1*dy2)/vcl_sqrt(dx1*dx1+dy1*dy1)/vcl_sqrt(dx2*dx2+dy2*dy2))>0;
    }
  }
  if (!cons) return false; //no good at the start point

  //1b) The end point of the edgel_chain be constrained to locate at unambiguous fragments?
  // Do not apply to include free end case
  dbdet_edgel* eE = edgel_chain.back();

  // When the end of the path reach an unambiguous chain
  if(curve_frag_graph_.pFrags[eE->id].size() + curve_frag_graph_.cFrags[eE->id].size()>=1)
  {
	  e2 = edgel_chain[edgel_chain.size()-2];
	  dx1 = eE->pt.x() - e2->pt.x();
	  dy1 = eE->pt.y() - e2->pt.y();
	  cons = false;
	  //Is the ending point consistent with at least one of the connecting unambiguous chains? : angel < 90 degree
	  pcit = curve_frag_graph_.pFrags[eE->id].begin();
	  for ( ; pcit != curve_frag_graph_.pFrags[eE->id].end(); pcit++)
	  {
		dbdet_edgel* pe = (*pcit)->edgels[(*pcit)->edgels.size()-2];
		E_link_end_nodes.push_back((*pcit)->edgels.front());

		//make a simple consistency check
		double dx2 = pe->pt.x() - eE->pt.x();
		double dy2 = pe->pt.y() - eE->pt.y();

		cons = cons || ((dx1*dx2 + dy1*dy2)/vcl_sqrt(dx1*dx1+dy1*dy1)/vcl_sqrt(dx2*dx2+dy2*dy2))>0;

	    if((*pcit)->edgels.size()>=3) // a little more globle check, in case of local zig-zag
	    {
	    	pe = (*pcit)->edgels[(*pcit)->edgels.size()-3];
	    	dx2 = pe->pt.x() - eE->pt.x();
	    	dy2 = pe->pt.y() - eE->pt.y();
	    	cons = cons || ((dx1*dx2 + dy1*dy2)/vcl_sqrt(dx1*dx1+dy1*dy1)/vcl_sqrt(dx2*dx2+dy2*dy2))>0;
	    }
	  }
	  ccit = curve_frag_graph_.cFrags[eE->id].begin();
	  for ( ; ccit != curve_frag_graph_.cFrags[eE->id].end(); ccit++)
	  {
		dbdet_edgel* ce = (*ccit)->edgels[1];
		E_link_end_nodes.push_back((*ccit)->edgels.back());

		//make a simple consistency check
		double dx2 = ce->pt.x() - eE->pt.x();
		double dy2 = ce->pt.y() - eE->pt.y();

		cons = cons || ((dx1*dx2 + dy1*dy2)/vcl_sqrt(dx1*dx1+dy1*dy1)/vcl_sqrt(dx2*dx2+dy2*dy2))>0;

	    if((*ccit)->edgels.size()>=3) // a little more globle check, in case of local zig-zag
	    {
	    	ce = (*ccit)->edgels[2];
	    	dx2 = ce->pt.x() - eE->pt.x();
	    	dy2 = ce->pt.y() - eE->pt.y();
			cons = cons || ((dx1*dx2 + dy1*dy2)/vcl_sqrt(dx1*dx1+dy1*dy1)/vcl_sqrt(dx2*dx2+dy2*dy2))>0;
	    }
	  }
	  if (!cons) return false; //no good at the end point


	  //2) use the two lists, check if it is a path between which unambiguous contours linked,
	  // this is only necessuary if both ends are at end points
	  for(int i =0; i<S_link_end_nodes.size(); i++)
	  {
	      dbdet_edgel* S_l_e = S_link_end_nodes[i];
	    if(S_l_e == eE)
	        return false;
	      for(int j=0; j<E_link_end_nodes.size(); j++)
	        {
	        dbdet_edgel* E_l_e = E_link_end_nodes[j];
	        if(E_l_e == eS)
	            return false;
	        // the case two connected contours filling the path
	        if(E_l_e== S_l_e)
	            return false;
	        }
	  }
  }

  // comment by Yuliang, in most cases, it is no need, because of short paths.
  //fit_polyarc_to_chain(&edgel_chain);
  return true;
}

//: New Quality Metric by Naman Kumar :: compute a path metric based on the Gap, Orientation, Strength and Size of the chain
double dbdet_sel_base::compute_path_metric2(vcl_vector<dbdet_edgel*>& Pchain,
                                           vcl_vector<dbdet_edgel*>& Tchain,
                                           vcl_vector<dbdet_edgel*>& Cchain)
{
  double cost = 0.0;double ds=0;double dt=0;

  //construct an edgel chain out of all three chains
  vcl_vector<dbdet_edgel*> chain;
  if (Pchain.size())
    for (unsigned i=0; i<Pchain.size(); i++) chain.push_back(Pchain[i]);
  if (Tchain.size())
    for (unsigned i=0; i<Tchain.size(); i++) chain.push_back(Tchain[i]);
  if (Cchain.size())
    for (unsigned i=0; i<Cchain.size(); i++) chain.push_back(Cchain[i]);

  //now compute the metric
  dbdet_edgel *eA=0, *eP=0;
  double dsp = 0, thp = 0, total_ds =0.0, a=0.0,s1=0,s2=0,s=0,size=chain.size();
  for (unsigned i=1; i<chain.size(); i++)
  {
    // computing difference in strength
    eA = chain[i];
    eP = chain[i-1];
    s1=(eA)->strength;
    s2=(eP)->strength;
    s=vcl_fabs(s1-s2);
    //computing ds
    ds = vgl_distance(eA->pt, eP->pt);
    if(ds>1.0) a=2.0; else a=1.0;
    total_ds += ds;
    //computing dtheta
    double thc = dbdet_vPointPoint(eP->pt, eA->pt);
    dt = vcl_fabs(thc-thp);
    dt = (dt>vnl_math::pi)? 2*vnl_math::pi-dt : dt;
    cost += vcl_pow((s+dt + a*ds)/size, 2.0); 
    thp = thc;//saving the current vector for the next iteration
    dsp = ds;
  }
  return cost;
}

double dbdet_sel_base::compute_path_len(vcl_deque<dbdet_edgel*>& chain)
{
	double total_ds = 0;
	dbdet_edgel *eA=0, *eP=0;
	for (unsigned i=1; i<chain.size(); i++)
	  {
	    eA = chain[i];
	    eP = chain[i-1];

	    total_ds += vgl_distance(eA->pt, eP->pt);
	  }
	  return total_ds;
}

double dbdet_sel_base::compute_path_len(vcl_vector<dbdet_edgel*>& chain)
{
	double total_ds = 0;
	dbdet_edgel *eA=0, *eP=0;
	for (unsigned i=1; i<chain.size(); i++)
	  {
	    eA = chain[i];
	    eP = chain[i-1];

	    total_ds += vgl_distance(eA->pt, eP->pt);
	  }
	  return total_ds;
}
//: New Quality Metric by Yuliang Guo :: compute a path metric based on the Gap, Orientation normorlized by Size of the chain, weights trained by grid search
double dbdet_sel_base::compute_path_metric3(vcl_vector<dbdet_edgel*>& Pchain,
                                           vcl_vector<dbdet_edgel*>& Tchain,
                                           vcl_vector<dbdet_edgel*>& Cchain)
{
  double cost = 0.0;double ds=0;double dt=0;

  //construct an edgel chain out of all three chains
  vcl_vector<dbdet_edgel*> chain;
  if (Pchain.size())
    for (unsigned i=0; i<Pchain.size(); i++) chain.push_back(Pchain[i]);
  if (Tchain.size())
    for (unsigned i=0; i<Tchain.size(); i++) chain.push_back(Tchain[i]);
  if (Cchain.size())
    for (unsigned i=0; i<Cchain.size(); i++) chain.push_back(Cchain[i]);

  //now compute the metric
  dbdet_edgel *eA=0, *eP=0;
  double thp = 0, total_ds =0.0, size=chain.size();
  for (unsigned i=1; i<chain.size(); i++)
  {
    eA = chain[i];
    eP = chain[i-1];

    //compute ds
    ds = vgl_distance(eA->pt, eP->pt);
    double a;
    if(ds<=1.0) a=1.0; else a=ds;
    total_ds += ds;

    //compute dt: dir(i, i-1) - dir(i-1, i-2)
    double thc = dbdet_vPointPoint(eP->pt, eA->pt); // arc_tan2
    dt = vcl_pow((vcl_cos(thc) - vcl_cos(thp)), 2) + vcl_pow((vcl_sin(thc) - vcl_sin(thp)), 2);

    if(i==1)
    	dt=0;
    cost += dt + a*vcl_pow(ds,2);
    //compute dtheta: tangent(i) - tangent(i-1)
    //double dtheta = vcl_pow((vcl_cos(eA->tangent) - vcl_cos(eP->tangent)), 2) + vcl_pow((vcl_sin(eA->tangent) - vcl_sin(eP->tangent)), 2);
    //cost += dtheta;

    thp = thc;//save the current vector for the next iteration
  }
  cost /= (size-1);
  return cost;
}

double dbdet_sel_base::compute_path_metric3(vcl_deque<dbdet_edgel*>& chain)
{
  double cost = 0.0;double ds=0;double dt=0;

  //now compute the metric
  dbdet_edgel *eA=0, *eP=0;
  double thp = 0, total_ds =0.0, size=chain.size();
  for (unsigned i=1; i<chain.size(); i++)
  {
    eA = chain[i];
    eP = chain[i-1];

    //compute ds
    ds = vgl_distance(eA->pt, eP->pt);
    double a;
    if(ds<=1.0) a=1.0; else a=ds;
    total_ds += ds;

    //compute dt: dir(i, i-1) - dir(i-1, i-2)
    double thc = dbdet_vPointPoint(eP->pt, eA->pt); // arc_tan2
    dt = vcl_pow((vcl_cos(thc) - vcl_cos(thp)), 2) + vcl_pow((vcl_sin(thc) - vcl_sin(thp)), 2);

    if(i==1)
    	dt=0;
    cost += dt + a*vcl_pow(ds,2);
    //compute dtheta: tangent(i) - tangent(i-1)
    //double dtheta = vcl_pow((vcl_cos(eA->tangent) - vcl_cos(eP->tangent)), 2) + vcl_pow((vcl_sin(eA->tangent) - vcl_sin(eP->tangent)), 2);
    //cost += dtheta;

    thp = thc;//save the current vector for the next iteration
  }
  cost /= (size-1);
  return cost;
}


bool link_cost_less(dbdet_CFTG_link* link1, dbdet_CFTG_link* link2)
{return link1->cost < link2->cost;}


//: disambiguate the CFG, basically to produce a disjoint set
void dbdet_sel_base::disambiguate_the_CFTG()
{
  //At the moment, I cannot verify that the CFTG is topologically sound (i.e., a planar graph)
  //so within this limit, the goal is to break the links at junctions

  //Alternatively, it is possible to splice the two contours and mark the connection with the others as a junction
  //these others might be pruned off at a postprocessing stage if desired

  //Note: the temp flag on the contours distinguish it from the unambiguous contours

  //A) disambiguate the links first : Only keep the best path
  //   Note: remember to search in both directions

  //go over all the links of the CFTG
  vcl_vector<dbdet_edgel*> dummy_chain;
   
  dbdet_CFTG_link_list_iter l_it = curve_frag_graph_.CFTG.Links.begin();
  for (; l_it != curve_frag_graph_.CFTG.Links.end(); l_it++)
  {
    dbdet_CFTG_link* cur_Link = (*l_it);
 

    //is this an ambiguous link?
    if (cur_Link->cCFs.size()>1)
    {
      //needs disambiguation
      double min_cost = 10000;
      dbdet_edgel_chain* best_chain = 0;
    dbdet_edgel_chain_list_iter f_it = cur_Link->cCFs.begin();
    for(; f_it != cur_Link->cCFs.end(); f_it++)
      {
        dbdet_edgel_chain* edgel_chain = (*f_it);
        vcl_vector<dbdet_edgel*> chain(edgel_chain->edgels.begin(), edgel_chain->edgels.end());

        double path_cost = compute_path_metric2(dummy_chain, chain, dummy_chain);
        if (path_cost < min_cost){
          min_cost = path_cost;
          best_chain = edgel_chain;
        }
 
      }

      //remove all except the best chain
      if (best_chain){
        dbdet_edgel_chain_list_iter f_it = cur_Link->cCFs.begin();
        for(; f_it != cur_Link->cCFs.end(); f_it++)
          if ((*f_it) != best_chain)
            delete (*f_it);

        cur_Link->cCFs.clear();
        cur_Link->cCFs.push_back(best_chain);
        cur_Link->cost = min_cost;
}
    }
    else { //just comptue cost for this path

      dbdet_edgel_chain* edgel_chain = cur_Link->cCFs.front();
      vcl_vector<dbdet_edgel*> chain(edgel_chain->edgels.begin(), edgel_chain->edgels.end());
      cur_Link->cost = compute_path_metric2(dummy_chain, chain, dummy_chain);
    }
  }
  //B) disambiguate between duplicates (A->B vs B->A)
  l_it = curve_frag_graph_.CFTG.Links.begin();
  for (; l_it != curve_frag_graph_.CFTG.Links.end(); l_it++)
  {
    dbdet_CFTG_link* cur_Link = (*l_it);

    if (cur_Link->cCFs.size()==0)
      continue;

    //look for the link from the other direction
    dbdet_CFTG_link_list_iter l_it2 = curve_frag_graph_.CFTG.cLinks[cur_Link->eE->id].begin();
    for (; l_it2 != curve_frag_graph_.CFTG.cLinks[cur_Link->eE->id].end(); l_it2++){
      if ((*l_it2)->eE == cur_Link->eS){
        //duplicate found

        if ((*l_it2)->cCFs.size()==0)
          continue;

        dbdet_edgel_chain* edgel_chain1 = cur_Link->cCFs.front();
        dbdet_edgel_chain* edgel_chain2 = (*l_it2)->cCFs.front();

        vcl_vector<dbdet_edgel*> chain1(edgel_chain1->edgels.begin(), edgel_chain1->edgels.end());
        vcl_vector<dbdet_edgel*> chain2(edgel_chain2->edgels.begin(), edgel_chain2->edgels.end());

        double path_cost1 = compute_path_metric2(dummy_chain, chain1, dummy_chain);
        double path_cost2 = compute_path_metric2(dummy_chain, chain2, dummy_chain);

        if (path_cost1<path_cost2){
          //keep current link and delete the other
          delete edgel_chain2;
          (*l_it2)->cCFs.clear();
          (*l_it2)->cost = 1000;
        }
        else{
          delete edgel_chain1;
          cur_Link->cCFs.clear();
          cur_Link->cost = 1000;
        }
      }
    }
  }

  //C) Gradient descent to prune bifurcations from the CFTG

  //go over list of Links and find any with degree > 1
  //these need to be disambiguated (gradient descent)
  vcl_list<dbdet_CFTG_link*> GD_list;

  //populate the map
  l_it = curve_frag_graph_.CFTG.Links.begin();
  for (; l_it != curve_frag_graph_.CFTG.Links.end(); l_it++)
  {
    //compute degree at each end
    int deg_S = curve_frag_graph_.CFTG.cLinks[(*l_it)->eS->id].size() + curve_frag_graph_.CFTG.pLinks[(*l_it)->eS->id].size();
    int deg_E = curve_frag_graph_.CFTG.cLinks[(*l_it)->eE->id].size() + curve_frag_graph_.CFTG.pLinks[(*l_it)->eE->id].size();

    if (deg_S>1){
      GD_list.push_back(*l_it);
      continue;
    }

    if (deg_E>1){
      GD_list.push_back(*l_it);
      continue;
    }
 
  }

 
  //sort the cost list
  GD_list.sort(link_cost_less);

  //gradient descent
  while (GD_list.size()>0)
  {
    dbdet_CFTG_link* cur_Link = GD_list.front();
    GD_list.pop_front();

    //now remove the other links connected to the end points of this link
    //clinks from eS
    vcl_vector<dbdet_CFTG_link*> links_to_del;
    l_it = curve_frag_graph_.CFTG.cLinks[cur_Link->eS->id].begin();
    for (; l_it != curve_frag_graph_.CFTG.cLinks[cur_Link->eS->id].end(); l_it++){
 
      if ((*l_it) != cur_Link)
    links_to_del.push_back((*l_it));
    }

    //by yuliang, also consider plinks from eS
    l_it = curve_frag_graph_.CFTG.pLinks[cur_Link->eS->id].begin();
    for (; l_it != curve_frag_graph_.CFTG.pLinks[cur_Link->eS->id].end(); l_it++){

      if ((*l_it) != cur_Link)
        links_to_del.push_back((*l_it));
    }

    for (unsigned j=0; j<links_to_del.size(); j++){   
      GD_list.remove(links_to_del[j]);//also remove it from the GD list
      curve_frag_graph_.CFTG.remove_link(links_to_del[j]);
    }
    links_to_del.clear();

    //plinks from eE
    l_it = curve_frag_graph_.CFTG.pLinks[cur_Link->eE->id].begin();
    for (; l_it != curve_frag_graph_.CFTG.pLinks[cur_Link->eE->id].end(); l_it++){

      if ((*l_it) != cur_Link)
        links_to_del.push_back((*l_it));
    }

    //by yuliang, also consider clinks from eE
    l_it = curve_frag_graph_.CFTG.cLinks[cur_Link->eE->id].begin();
    for (; l_it != curve_frag_graph_.CFTG.cLinks[cur_Link->eE->id].end(); l_it++){
 
      if ((*l_it) != cur_Link)
        links_to_del.push_back((*l_it));
    }

    for (unsigned j=0; j<links_to_del.size(); j++){
      GD_list.remove(links_to_del[j]);//also remove it from the GD list
      curve_frag_graph_.CFTG.remove_link(links_to_del[j]);
    }
    links_to_del.clear();
  }

  //D) Add it all back to the CFG (clear the CFTG in the process)
  l_it = curve_frag_graph_.CFTG.Links.begin();
  for (; l_it != curve_frag_graph_.CFTG.Links.end(); l_it++)
  {
    if ((*l_it)->cCFs.size()==1)
      curve_frag_graph_.insert_fragment((*l_it)->cCFs.front());
  }
  curve_frag_graph_.CFTG.clear();
  curve_frag_graph_.CFTG.resize(edgemap_->edgels.size());
   vcl_cout<<"Finish disambiguating the CFTG"<<vcl_endl;
}

// Following part by Yuliang Guo.
static bool is_continue (const dbdet_edgel_chain *c1, const dbdet_edgel_chain *c2); // test genenal/local continuity

static bool is_longer (const dbdet_edgel_chain *c1, const dbdet_edgel_chain *c2){ // whether contour 1 is longer
    if (c1->edgels.size()>c2->edgels.size()){
        return true;
    }
    return false;
 
}

static double get_continuity (const dbdet_edgel_chain *c1, const dbdet_edgel_chain *c2);
//: correct the CFG topology to produce a disjoint set


void dbdet_sel_base::merge_extreme_short_curve_frags()
{

for (unsigned i=0; i<edgemap_->edgels.size(); i++)
  {
    dbdet_edgel_chain *c1=0, *c2=0;
    dbdet_edgel* eA = edgemap_->edgels[i];

    int deg = curve_frag_graph_.pFrags[i].size()+ curve_frag_graph_.cFrags[i].size();	
    if (deg<2)
      continue; //nodes

    if (deg==2){ //degree 2 segments will trigger a splice
   
      //standard operation: extract them from the graph, reorder them, either merge or put them back

      //segments need to meet continuity criteria (simple one for now)
      if (curve_frag_graph_.pFrags[i].size()>1){
        dbdet_edgel_chain_list_iter fit = curve_frag_graph_.pFrags[i].begin();
        c1 =  (*fit); fit++;
        c2 =  (*fit);

        curve_frag_graph_.extract_fragment(c1);
        curve_frag_graph_.extract_fragment(c2);

        //reverse the sequence of edgels
        vcl_reverse(c2->edgels.begin(), c2->edgels.end());
        curve_frag_graph_.insert_fragment(c1);
        curve_frag_graph_.insert_fragment(c2);
      }
      else if (curve_frag_graph_.pFrags[i].size()==1){
        c1 =  curve_frag_graph_.pFrags[i].front();
        c2 =  curve_frag_graph_.cFrags[i].front();
		//for the closed contour case
		if(c1==c2)
			continue;
      }
      else {
        dbdet_edgel_chain_list_iter fit = curve_frag_graph_.cFrags[i].begin();
        c1 =  (*fit); fit++;
        c2 =  (*fit);

        //add the second one to the first one and delete it from the graph
        curve_frag_graph_.extract_fragment(c1);
        curve_frag_graph_.extract_fragment(c2);

        //reverse the sequence of edgels
        vcl_reverse(c1->edgels.begin(), c1->edgels.end());
        curve_frag_graph_.insert_fragment(c1);
        curve_frag_graph_.insert_fragment(c2);
      }

      if ((c1->edgels.size()<=5 || c2->edgels.size()<=5) && is_continue(c1,c2)){ //if two contours are all very short < 5 edges and are continuous, merge them anyway
        //merge the two contours
    	curve_frag_graph_.extract_fragment(c1);
        curve_frag_graph_.extract_fragment(c2);
        c1->append(c2->edgels);
        curve_frag_graph_.insert_fragment(c1);
        //when it makes a closed contour, just count as the child frag rather than parent frag
        if(c1->edgels.front()==c1->edgels.back())
        	curve_frag_graph_.pFrags[c1->edgels.front()->id].remove(c1);
        delete c2;
      }
	  else if (c1->edgels.size()<=2 || c2->edgels.size()<=2)
	  {
		//merge the two contours
    	curve_frag_graph_.extract_fragment(c1);
        curve_frag_graph_.extract_fragment(c2);
        c1->append(c2->edgels);
        curve_frag_graph_.insert_fragment(c1);
    	//when it makes a closed contour, just count as the child frag rather than parent frag
        if(c1->edgels.front()==c1->edgels.back())
        	curve_frag_graph_.pFrags[c1->edgels.front()->id].remove(c1);
        delete c2;		
	  }
     }
  }

    // use the filter to prun out local problems again
    regular_contour_filter();
}

void dbdet_sel_base::correct_CFG_topology()
{
  //D) Final T-junction type disambiguation can be done on the CFG
  // Basically, go over all the nodes of the CFG and operate on the ones with degree>2
  // also merge all segments that are adjacent

  // going over the edgemap instead so that a node is visited only once and so that I
  // don't have to deal with iterator issues

  for (unsigned i=0; i<edgemap_->edgels.size(); i++)
  {
    dbdet_edgel_chain *c1=0, *c2=0;
    dbdet_edgel* eA = edgemap_->edgels[i];

    int deg = curve_frag_graph_.pFrags[i].size()+ curve_frag_graph_.cFrags[i].size();	
    if (deg<2)
      continue; //nodes

    if (deg==2)
    { //degree 2 segments will trigger a splice
   
      //standard operation: extract them from the graph, reorder them, either merge or put them back

      //segments need to meet continuity criteria (simple one for now)
      if (curve_frag_graph_.pFrags[i].size()>1){
        dbdet_edgel_chain_list_iter fit = curve_frag_graph_.pFrags[i].begin();
        c1 =  (*fit); fit++;
        c2 =  (*fit);

        curve_frag_graph_.extract_fragment(c1);
        curve_frag_graph_.extract_fragment(c2);

        //reverse the sequence of edgels
        vcl_reverse(c2->edgels.begin(), c2->edgels.end());
        curve_frag_graph_.insert_fragment(c1);
        curve_frag_graph_.insert_fragment(c2);
      }
      else if (curve_frag_graph_.pFrags[i].size()==1)
      {
        c1 =  curve_frag_graph_.pFrags[i].front();
        c2 =  curve_frag_graph_.cFrags[i].front();
		//for the closed contour case
		if(c1==c2)
			continue;
      }
      else {
        dbdet_edgel_chain_list_iter fit = curve_frag_graph_.cFrags[i].begin();
        c1 =  (*fit); fit++;
        c2 =  (*fit);

        //add the second one to the first one and delete it from the graph
        curve_frag_graph_.extract_fragment(c1);
        curve_frag_graph_.extract_fragment(c2);

        //reverse the sequence of edgels
        vcl_reverse(c1->edgels.begin(), c1->edgels.end());
        curve_frag_graph_.insert_fragment(c1);
        curve_frag_graph_.insert_fragment(c2);
      }

      if (is_continue(c1,c2)){ //if two contours are generally/local continue based on a frag
        //merge the two contours
    	curve_frag_graph_.extract_fragment(c1);
        curve_frag_graph_.extract_fragment(c2);
        c1->append(c2->edgels);
        curve_frag_graph_.insert_fragment(c1);
        //when it makes a closed contour, just count as the child frag rather than parent frag
        if(c1->edgels.front()==c1->edgels.back())
        curve_frag_graph_.pFrags[c1->edgels.front()->id].remove(c1);
        delete c2;
      }
     }
  }
/*
  // deal with junction with another loop after dealing with deg 2

  for (unsigned i=0; i<edgemap_->edgels.size(); i++)
  {
    dbdet_edgel_chain *c1=0, *c2=0;
    dbdet_edgel* eA = edgemap_->edgels[i];

    int deg = curve_frag_graph_.pFrags[i].size()+ curve_frag_graph_.cFrags[i].size();
    //degree 3 is a junction (T-junction or Y-junction)
    if (deg>2) //  Make length of contour as the first priority
    {
        //goal is to see if any two will produce smooth continuation
    dbdet_edgel_chain_list node_frags;
        if(curve_frag_graph_.pFrags[i].size()!=0){
        dbdet_edgel_chain_list_iter p_fit = curve_frag_graph_.pFrags[i].begin();
            for(;p_fit!=curve_frag_graph_.pFrags[i].end();p_fit++)
        {
            node_frags.push_back(*p_fit);
        }
    }

        if(curve_frag_graph_.cFrags[i].size()!=0){
        dbdet_edgel_chain_list_iter c_fit = curve_frag_graph_.cFrags[i].begin();
            for(;c_fit!=curve_frag_graph_.cFrags[i].end();c_fit++)
        {
            node_frags.push_back(*c_fit);
        }
    }
 
    node_frags.sort(is_longer); //sort all the pfrags and cfrags in length

        //compare each pair to decide merge or not
        dbdet_edgel_chain_list_iter fit_1=node_frags.begin();
    for (;fit_1!=--node_frags.end();){
        c1= *fit_1;
 
        if(c1->edgels.back()!= eA){ 
            curve_frag_graph_.extract_fragment(c1); 
            vcl_reverse(c1->edgels.begin(), c1->edgels.end());
            curve_frag_graph_.insert_fragment(c1);             
        }
        fit_1++;
        dbdet_edgel_chain_list_iter fit_2 = fit_1, max_fit = fit_1;
        double max_SM = 0;
        for (;fit_2!=node_frags.end();fit_2++){
            c2=*fit_2;
            if(c2->edgels.back()== eA){
                curve_frag_graph_.extract_fragment(c2);     
                vcl_reverse(c2->edgels.begin(), c2->edgels.end());
                curve_frag_graph_.insert_fragment(c2);             
            }

            double SM_0 = get_continuity(c1,c2);
            if(SM_0>max_SM){
                max_SM = SM_0;
                max_fit = fit_2;
            }
        }
        if(max_SM>=0.9){
            c2=*max_fit;
            curve_frag_graph_.extract_fragment(c1);
            curve_frag_graph_.extract_fragment(c2);
            c1->append(c2->edgels);
            curve_frag_graph_.insert_fragment(c1); 
            break;
        }
         
        //if(fit_2!=node_frags.end())
        //    break;
        }
    }
  }
*/
    // use the filter to prune out local problems again
    regular_contour_filter();
    vcl_cout<<"Finish correcting the CFG topology"<<vcl_endl;
}

//June, 2012:: Make decision whether to merge two contours or not (by Naman Kumar) 
static bool is_continue (const dbdet_edgel_chain *c1, const dbdet_edgel_chain *c2)
{
	int nbr_limit = 3;
	int sz1 = (c1->edgels.size()<nbr_limit)?c1->edgels.size():nbr_limit;
	int sz2 = (c2->edgels.size()<nbr_limit)?c2->edgels.size():nbr_limit;
 	dbdet_edgel* e1;dbdet_edgel* e4;
	dbdet_edgel* e2 = c1->edgels.back(); // Last edge of c1
	dbdet_edgel* e3 = c2->edgels.front(); // Front edge of c2
	double dx1=0,dy1=0,dy2=0,dx2=0,s1=0,s2=0,s=0,SM_1=0;

	for(int i=1;i<sz1;i++)
	{
		e1 = c1->edgels[c1->edgels.size()-1-i];
		dx1 = e2->pt.x()-e1->pt.x();
		dy1 = e2->pt.y()-e1->pt.y();
		for(int j=1;j<sz2 ;j++)
		{
			e4 = c2->edgels[j];
			dx2 = e4->pt.x()-e3->pt.x();
			dy2 = e4->pt.y()-e3->pt.y();
			SM_1 = (dx1*dx2 + dy1*dy2)/vcl_sqrt(dx1*dx1+dy1*dy1)/vcl_sqrt(dx2*dx2+dy2*dy2);

			if(SM_1>=Theta_1)
				return true;
			//else if(SM_1<Theta_2)
				//return false;
			//else
				//continue;
		}
	}
	return false;
}

static bool is_not_continue (const dbdet_edgel_chain *c1, const dbdet_edgel_chain *c2)
{
	int nbr_limit = 3;
	int sz1 = (c1->edgels.size()<nbr_limit)?c1->edgels.size():nbr_limit;
	int sz2 = (c2->edgels.size()<nbr_limit)?c2->edgels.size():nbr_limit;
 	dbdet_edgel* e1;dbdet_edgel* e4;
	dbdet_edgel* e2 = c1->edgels.back(); // Last edge of c1
	dbdet_edgel* e3 = c2->edgels.front(); // Front edge of c2
	double dx1=0,dy1=0,dy2=0,dx2=0,s1=0,s2=0,s=0,SM_1=0;

	for(int i=1;i<sz1;i++)
	{
		e1 = c1->edgels[c1->edgels.size()-1-i];
		dx1 = e2->pt.x()-e1->pt.x();
		dy1 = e2->pt.y()-e1->pt.y();
		for(int j=1;j<sz2 ;j++)
		{
			e4 = c2->edgels[j];
			dx2 = e4->pt.x()-e3->pt.x();
			dy2 = e4->pt.y()-e3->pt.y();
			SM_1 = (dx1*dx2 + dy1*dy2)/vcl_sqrt(dx1*dx1+dy1*dy1)/vcl_sqrt(dx2*dx2+dy2*dy2);

			if(SM_1<0)
				return true;
			//else if(SM_1<Theta_2)
				//return false;
			//else
				//continue;
		}
	}
	return false;
}


static double get_max_continuity (const dbdet_edgel_chain *c1, const dbdet_edgel_chain *c2)
{
	int nbr_limit = 3;
	int sz1 = (c1->edgels.size()<nbr_limit)?c1->edgels.size():nbr_limit;
	int sz2 = (c2->edgels.size()<nbr_limit)?c2->edgels.size():nbr_limit;
 	dbdet_edgel* e1;dbdet_edgel* e4;
	dbdet_edgel* e2 = c1->edgels.back(); // Last edge of c1
	dbdet_edgel* e3 = c2->edgels.front(); // Front edge of c2
	double dx1=0,dy1=0,dy2=0,dx2=0,s1=0,s2=0,s=0,SM_1=0;
	double max_con = -1;

	for(int i=1;i<sz1;i++)
	{
		e1 = c1->edgels[c1->edgels.size()-1-i];
		dx1 = e2->pt.x()-e1->pt.x();
		dy1 = e2->pt.y()-e1->pt.y();
		for(int j=1;j<sz2 ;j++)
		{
			e4 = c2->edgels[j];
			dx2 = e4->pt.x()-e3->pt.x();
			dy2 = e4->pt.y()-e3->pt.y();
			SM_1 = (dx1*dx2 + dy1*dy2)/vcl_sqrt(dx1*dx1+dy1*dy1)/vcl_sqrt(dx2*dx2+dy2*dy2);

			if(SM_1>=max_con)
				max_con = SM_1;
			//else if(SM_1<Theta_2)
				//return false;
			//else
				//continue;
		}
	}
	return max_con;
}

static double get_min_continuity (const dbdet_edgel_chain *c1, const dbdet_edgel_chain *c2)
{
	int nbr_limit = 3;
	int sz1 = (c1->edgels.size()<nbr_limit)?c1->edgels.size():nbr_limit;
	int sz2 = (c2->edgels.size()<nbr_limit)?c2->edgels.size():nbr_limit;
 	dbdet_edgel* e1;dbdet_edgel* e4;
	dbdet_edgel* e2 = c1->edgels.back(); // Last edge of c1
	dbdet_edgel* e3 = c2->edgels.front(); // Front edge of c2
	double dx1=0,dy1=0,dy2=0,dx2=0,s1=0,s2=0,s=0,SM_1=0;
	double min_con = 1;

	for(int i=1;i<sz1;i++)
	{
		e1 = c1->edgels[c1->edgels.size()-1-i];
		dx1 = e2->pt.x()-e1->pt.x();
		dy1 = e2->pt.y()-e1->pt.y();
		for(int j=1;j<sz2 ;j++)
		{
			e4 = c2->edgels[j];
			dx2 = e4->pt.x()-e3->pt.x();
			dy2 = e4->pt.y()-e3->pt.y();
			SM_1 = (dx1*dx2 + dy1*dy2)/vcl_sqrt(dx1*dx1+dy1*dy1)/vcl_sqrt(dx2*dx2+dy2*dy2);

			if(SM_1<=min_con)
				min_con = SM_1;
			//else if(SM_1<Theta_2)
				//return false;
			//else
				//continue;
		}
	}
	return min_con;
}

static double get_continuity (const dbdet_edgel_chain *c1, const dbdet_edgel_chain *c2)
{
        // using the median global continuity
        dbdet_edgel* e1=0;
        dbdet_edgel* e2=0;
        dbdet_edgel* e3=0;
        dbdet_edgel* e4=0;
        if(c1->edgels.size()>=5){ 
            e1 = c1->edgels[c1->edgels.size()-5];
            e2 = c1->edgels.back();
        }
        else{
            e1 = c1->edgels.front();
            e2 = c1->edgels.back();
        }
        if(c2->edgels.size()>=5){
            e3 = c2->edgels.front();
            e4 = c2->edgels[4];
        }
        else{
            e3 = c2->edgels.front();
            e4 = c2->edgels.back();
        }
        double dx1 = e2->pt.x()-e1->pt.x();
        double dy1 = e2->pt.y()-e1->pt.y();
        double dx2 = e4->pt.x()-e3->pt.x();
        double dy2 = e4->pt.y()-e3->pt.y();
        return (dx1*dx2 + dy1*dy2)/vcl_sqrt(dx1*dx1+dy1*dy1)/vcl_sqrt(dx2*dx2+dy2*dy2);
}

static bool share_same_ends(dbdet_edgel_chain *c1, dbdet_edgel_chain *c2)
{
    if((c1->edgels.front()==c2->edgels.front()&&c1->edgels.back()==c2->edgels.back())
        || (c1->edgels.front()==c2->edgels.back()&&c1->edgels.back()==c2->edgels.front()))
        return true;
    return false;
}



void dbdet_sel_base::regular_contour_filter(){
// first, deal with contours with length 3, which cause a lot of local problems
    dbdet_edgel_chain_list Size_3_chain_list;
    dbdet_edgel_chain_list_iter fit = curve_frag_graph_.frags.begin();
    for(;fit!=curve_frag_graph_.frags.end();fit++)
    {
        dbdet_edgel_chain *c1=*fit;
		// (a) for size 4 frags, git rid of the small closed triangle
		if(c1->edgels.size()==4 && (c1->edgels.front()==c1->edgels.back()))
		{
			c1->edgels.pop_back();
			curve_frag_graph_.pFrags[c1->edgels.back()->id].push_back(c1);
		}
		// push size 3 frags's pointer into a seperate list to same computation for next step
		if(c1->edgels.size()==3)
			Size_3_chain_list.push_back(c1);
    }


    dbdet_edgel_chain_list_iter fit_1 = Size_3_chain_list.begin();
    while(fit_1!=Size_3_chain_list.end())
    {
        // (b) change 3 edgels sharp path to be a direct line link
		dbdet_edgel_chain *c1=*fit_1;
		double dx1 = c1->edgels[1]->pt.x() - c1->edgels.front()->pt.x();
		double dy1 = c1->edgels[1]->pt.y() - c1->edgels.front()->pt.y();
		double dx2 = c1->edgels.back()->pt.x() - c1->edgels[1]->pt.x();
		double dy2 = c1->edgels.back()->pt.y() - c1->edgels[1]->pt.y();
		double SM = (dx1*dx2 + dy1*dy2)/vcl_sqrt(dx1*dx1+dy1*dy1)/vcl_sqrt(dx2*dx2+dy2*dy2);
		double length = vcl_sqrt(dx1*dx1+dy1*dy1) + vcl_sqrt(dx2*dx2+dy2*dy2);
		if(length>10)// only consider local problems
			continue;
		if(SM<=0)
		{
			dbdet_edgel_list_iter eit = c1->edgels.begin();
			eit++;
			c1->edgels.erase(eit);
		}
		// (c) if two frags share same end nodes, change them into one direct line link
		dbdet_edgel_chain_list_iter fit_2 = ++fit_1;
		while(fit_2!=Size_3_chain_list.end())
		{
			dbdet_edgel_chain *c2=*fit_2;
			if(share_same_ends(c1, c2))
			{
				// check if c1 is modifid in (b), if no, change it to a direct line link
				if(c1->edgels.size()==3)
				{
					dbdet_edgel_list_iter eit = c1->edgels.begin();
					eit++;
					c1->edgels.erase(eit);
				}
				// remove c2 from CFG
				curve_frag_graph_.extract_fragment(c2);
				// only deal with one frag has same ends, if there are more, leave for iterations afterward
				break;
			}
			fit_2++;
		}
    }
}


//New construction of Hypothesis Tree by Naman Kumar
void dbdet_sel_base::Construct_Hypothesis_Tree()
{
	int n1=0;
	double d=0,dis=0,distance=0;
	vcl_vector<dbdet_edgel*> new_chain0,new_chain2,new_chain3,new_chain6,new_chain33;
	dbdet_edgel_chain* new_chain1=new dbdet_edgel_chain();dbdet_edgel_chain* new_chain4=new dbdet_edgel_chain();
	dbdet_edgel_chain* test1=new dbdet_edgel_chain();dbdet_edgel_chain *chains=new dbdet_edgel_chain();
	dbdet_edgel_chain* new_chain44=new dbdet_edgel_chain();
	double gap_thres=gap_;
	vcl_cout << "Construction of Hypothesis Tree is in Progress!! " << vcl_endl;
	//Calculating number of edges which are having degree 1   	
	for (int i=0; i<edgemap_->edgels.size(); i++)
	{
        	dbdet_edgel* eA1 = edgemap_->edgels[i];
        	new_chain0.push_back(eA1); // new_chain0: saves all the edges in edgemap
        	if ((curve_frag_graph_.pFrags[eA1->id].size() + curve_frag_graph_.cFrags[eA1->id].size()) ==1)
        		new_chain1->edgels.push_back(eA1);
        	else new_chain2.push_back(eA1);
        	// new_chain1: save all edges having degree 1
        	// new_chain2; save all edges having degree not 1
	}

	//Calculating number of edges which are part of the contours and which are unused
	dbdet_edgel_chain_list_iter fit = curve_frag_graph_.frags.begin();
	for(;fit!=curve_frag_graph_.frags.end();fit++)
	{
        	dbdet_edgel_chain *test1=*fit;
        	for(int d=0;d<test1->edgels.size();d++) {new_chain3.push_back(test1->edgels[d]);new_chain33.push_back(test1->edgels[d]);} 
	}
	// new_chain3, new_chain33: saves all the edges participated in curve fragments
	for(int i=0;i<new_chain0.size();i++)
	{
        	for(int j=0;j<new_chain3.size();j++)
        	{
        		if(new_chain0[i]!=new_chain3[j]) continue;
                	else {n1=5; break;}
        	}
     		if(n1==0) {new_chain4->edgels.push_back(new_chain0[i]);}
     		else {n1=0; continue;}
	}
	// new_chain4: save all edges NOT used in any curve fragment

	//Constucting the tree from end of an unambiguous chain and extending it till the end of edge chain
	double cost1=gap_,cost2=10.0,cost3=gap_,d1=0.0,d2=0.0,d3=0,dx=0.0,dy=0.0,cost=1000.0,costc=0.0;int m1=0,m2=0,m3=0,m4=0,m5=0,m7=0,m8=0,m9=0;
	dbdet_edgel* ce=0;dbdet_edgel* pe=0;dbdet_edgel* ed=0;dbdet_edgel* imp=0;dbdet_edgel* im=0;
	dbdet_edgel_chain *c11=new dbdet_edgel_chain();dbdet_edgel_chain* xx=new dbdet_edgel_chain();dbdet_edgel_chain* end=new dbdet_edgel_chain();
	while(new_chain1->edgels.size()>0) // go over all terminations, that is, edge with degree 1
	{
		a: ; // spot marker a:
		if(new_chain1->edgels.size()==0)
			break;
		ed=new_chain1->edgels[0];
		new_chain1->edgels.pop_front();
		// skip the termination that is included in 'end', some previous searched path reached a terminal, that is saved in 'end'
		for(int z=0;z<end->edgels.size();z++)
		{
			if(ed==end->edgels[z])
				goto a;
			else
				continue;
		}
		dbdet_edgel_chain* new_chain5 = new dbdet_edgel_chain();
		new_chain5->edgels.push_back(ed);xx->edgels.push_back(ed);
		m4=0;m7=0;
		double dis=0, distance=0;
		//Forming the tree from the edge
		// c11: hold the connected fragment to ed. Because degree of ed is 1, c11 is either the cfrag or pfrag adjuncted with ed
		// distance: max pairwise dist between edges in c11. it is used to update gap_.
		while(1)
		{
			dbdet_edgel_list_iter eit1=new_chain4->edgels.begin();
			dbdet_edgel_list_iter eit2=new_chain4->edgels.begin();
			if(m4==0) // indicate the initial termination
			{
				if(curve_frag_graph_.cFrags[ed->id].size()==1) // if number of child fragments is 1
				{
					dbdet_edgel_chain_list_iter ccit = curve_frag_graph_.cFrags[ed->id].begin();
					ce = (*ccit)->edgels[1];
					c11=*ccit;
					pe=ce;
					m7=1;
					for (int j=1;j<c11->edgels.size();j++)
					{
						dis=vgl_distance(c11->edgels[j]->pt,c11->edgels[j-1]->pt);
						if(dis>distance)
							distance=dis;
					}
					distance=distance + 0.25;
					if(distance <=1)
						gap_=1;
					else if(distance <gap_thres)
						gap_=distance;
					else
						gap_=gap_thres;
				}
				else if(curve_frag_graph_.pFrags[ed->id].size()==1) // if number of parent fragments is 1
				{
					dbdet_edgel_chain_list_iter pcit = curve_frag_graph_.pFrags[ed->id].begin();
					ce = (*pcit)->edgels[(*pcit)->edgels.size()-2];
					c11=*pcit;
					pe=ce;
					m7=2;
					for (int j=1;j<c11->edgels.size();j++)
					{
						dis=vgl_distance(c11->edgels[j]->pt,c11->edgels[j-1]->pt);
						if(dis>distance)
							distance=dis;
					}
					distance=distance + 0.25;
					if(distance <=1)
						gap_=1;
					else if(distance <gap_thres)
						gap_=distance;
					else
						gap_=gap_thres;
				}
				m4=1;
			}
			// ce: second edge from the connected fragment
			// m7: use cfrag =1, use pfrag = 2
			// m8:
			// m9; size of cll
			if(m7==2)
			{
				m8=c11->edgels.size()-5;
				if(m8<0)
					m8=0;
				m9=c11->edgels.size();
			}
			else if(m7==1)
			{
				m8=0;
				m9=5;
				if(m9>c11->edgels.size())
					m9=c11->edgels.size();
			}

			// Finding the closest unused edge, saved in 'imp', prepare to extend the current path
			costc=0.0;cost=10000.0;cost1=gap_;
			for(int j=0;j<new_chain4->edgels.size(); j++)
			{
				d1= vgl_distance(ed->pt,new_chain4->edgels[j]->pt);
				//Checking Localization, Orientation,etc..
				if(d1<cost1) // extended step need to satisfy cost threshold
				{
					vcl_vector<dbdet_edgel*> dummy_chain;
					// copy previous found path new_chain5 into temp edgel_chain
					dbdet_edgel_chain* edgel_chain = new dbdet_edgel_chain();
					for(int i=0;i<new_chain5->edgels.size();i++)
						edgel_chain->edgels.push_back(new_chain5->edgels[i]);
					edgel_chain->edgels.push_back(new_chain4->edgels[j]);
					vcl_vector<dbdet_edgel*> chain(edgel_chain->edgels.begin(),edgel_chain->edgels.end());
					costc = compute_path_metric2(dummy_chain, chain, dummy_chain);
					// check if the extended chain satisfy cost threshold
					if(costc<cost)
					{
						double d8=vgl_distance(new_chain4->edgels[j]->pt,ce->pt);
						double d9=vgl_distance(new_chain4->edgels[j]->pt,ed->pt);
						double d0=vgl_distance(new_chain4->edgels[j]->pt,pe->pt); //pe is always the initial one
						double dx1 = ce->pt.x() - ed->pt.x();
						double dy1 = ce->pt.y() - ed->pt.y();
						double dx2 = ed->pt.x() - new_chain4->edgels[j]->pt.x();
						double dy2 = ed->pt.y() - new_chain4->edgels[j]->pt.y();
						double angle=((dx1*dx2 + dy1*dy2)/vcl_sqrt(dx1*dx1+dy1*dy1)/vcl_sqrt(dx2*dx2+dy2*dy2));
						if(d0<d9) {++eit1;continue;} // if distance to initial edge < distance to current edge, skip it: wrong direction of search
						if(d8<d9 || angle<0){++eit1;continue;} // if distance to previous edge < distance to current edge, skip it: wrong direction of search
						imp=new_chain4->edgels[j];
						cost=costc;
						m1=1; // indicate found of an extendable edge
						eit2=eit1;
					}
				}
				else if(d1<cost2 && d1>1)
				{
					cost2=d1;
				}
				++eit1;
			}

			m3=0;m5=0;cost3=gap_;
			// Finding the closest edge which is part of a fragment, saved in 'im'
			// this is actually looking for termination of the searching path
			for(int t=0;t<new_chain3.size(); t++)
			{
				if(new_chain3[t]==ed || new_chain3[t]==ce)
					continue;
				d1= vgl_distance(ed->pt,new_chain3[t]->pt);
				if(d1<=cost3)
				{
						//Do not consider the previous 5 edges present in parent/child fragment of the starting edge
					    // in case they are too close, such that the distance threshold is satisfied
						for(int c=m8;c<m9;c++)
						{
							if(new_chain3[t]==c11->edgels[c])
								goto z;
							else
								continue;
						}
						//Dont use the edge which is part of the same tree again
						for(int c=0;c<new_chain5->edgels.size();c++)
						{
							if(new_chain3[t]==new_chain5->edgels[c])
								goto z;
							else
								continue;
						}
						im=new_chain3[t];
						cost3=d1;
						m5=1; // indicate found of an used edge, satisfy cost threshold
						dx=vgl_distance(im->pt,ce->pt);
						dy=vgl_distance(im->pt,ed->pt);
				}
				z: ;
			}
			// if the closest used edge < dist th, and has distance to previous edge > distance to current edge, terminate the search.
			// this constraint includes the junction case, when the searching path terminates in the middle of an existing curve fragment
			if(dx>dy && m5==1)
			{
				m3=5;// indicate for termination
				m1=1;
				imp=im; // use this used edge?  but m3 != 0, make this not usable at all
			}
			if(m1==1) // if found a qualified edge to extend current path
			{
				m2=1;m1=0;cost1=gap_;
				ce=ed; // update previous edge
				ed=imp;// update current edge
				xx->edgels.push_back(imp);
				new_chain5->edgels.push_back(imp);
				if(m3==0)
				{
					// update the used edge list and the unused edge list
					new_chain4->edgels.erase(eit2);
					new_chain3.push_back(imp);
					new_chain33.push_back(imp);
				}
				if(m3!=0)
				{
					m3=0;
					break;
				}
			}
			else if(cost2>1)
				break;
		}
		// the found path is saved in 'new_chain5'
		//No double contours within the same 2 end points.
		if(m2==1)
		{
			if(c11->edgels.front()==c11->edgels.back()) // is this right?????
				continue;
		}
		//Add the tree
		new_chain5->temp = true;
		curve_frag_graph_.CFTG.insert_fragment(new_chain5);
		end->edgels.push_back(new_chain5->edgels.back());
	}

	/*
	/////////// Following process seems to have No effect
	dbdet_edgel* edge1=0;dbdet_edgel* edge2=0;dbdet_edgel_chain *chain1=new dbdet_edgel_chain();vcl_list<dbdet_CFTG_link*> GD_list;
	double p1=1.0;int p2=0,p3=0,p16=0;
	dbdet_CFTG_link_list_iter l_it = curve_frag_graph_.CFTG.Links.begin();      
    for (; l_it != curve_frag_graph_.CFTG.Links.end(); l_it++)
    	GD_list.push_back(*l_it);
    //while size is greater than 0
  	while (GD_list.size()>0)
  	{
  		double distance1=0,distance2=0;
  		dbdet_CFTG_link* cur_Link = GD_list.front();GD_list.pop_front();
  		dbdet_edgel_chain_list_iter f_it = cur_Link->cCFs.begin();
  		dbdet_edgel_chain* new_chain5=(*f_it);
  		dbdet_edgel* edge3=new_chain5->edgels.front();
		gap_=1;
		// iterating through all the edgels of chain5
		for(int i=0;i<new_chain5->edgels.size();i++)
		{
			dbdet_edgel_chain *new_chain6a=new dbdet_edgel_chain();
			p1=gap_;
			dbdet_edgel_list_iter eit5=new_chain4->edgels.begin();
			dbdet_edgel_list_iter eit6=new_chain4->edgels.begin();
			for(int j=0;j<new_chain4->edgels.size();j++)
			{
				double p4=vgl_distance(new_chain5->edgels[i]->pt,new_chain4->edgels[j]->pt);				
				if(p4<p1)
				{
					edge1=new_chain4->edgels[j];
					p1=p4;
					p2=1;
					eit6=eit5;
					++eit5;
				} // if distance between edgels is less than gap
				else
				{
					++eit5;
					continue;
				}
			}
			if(p2==1) // if found the closest unused edge 'edge1', which is extremely close to current edge
			{
				new_chain4->edgels.erase(eit6); //remove edgel
				// find the closest edge to 'edge1' in new_chain5, saved as 'edge2'
				dbdet_edgel* edge4=0;p2=0;double p5=gap_;
				for(int b=0;b<new_chain5->edgels.size();b++)
				{
					double p6=vgl_distance(edge1->pt,new_chain5->edgels[b]->pt);
					if(p6<p5)
					{
						edge2=new_chain5->edgels[b];
						p5=p6;
					}
					else
						continue;
				}
				new_chain6a=new dbdet_edgel_chain();
				new_chain6a->edgels.push_back(edge2);
				new_chain6a->edgels.push_back(edge1);
				int p7=0,p8=0,p9=0;
				if(curve_frag_graph_.cFrags[edge3->id].size()>=1) // if size of child fragment is greater than 1
				{
					dbdet_edgel_chain_list_iter ccit = curve_frag_graph_.cFrags[edge3->id].begin();
					chain1=(*ccit);p7=1;
				}
				else if(curve_frag_graph_.pFrags[edge3->id].size()>=1) // if size of parent fragment is greater than 1
				{
					dbdet_edgel_chain_list_iter pcit = curve_frag_graph_.pFrags[edge3->id].begin();
					chain1=(*pcit);p7=2;
				}
				// chain1 is the connected cfrag/pfrag to the 'edge3', the front edge in new_chain5
				if(p7==2)
				{
					p8=chain1->edgels.size()-5;
					if(p8<0)
						p8=0;
					p9=chain1->edgels.size();
				}
				else if(p7==1)
				{
					p8=0;
					p9=5;
					if(p9>chain1->edgels.size())
						p9=chain1->edgels.size();
				}
				// iterating through all the unused edges
				while(1)
				{
					double p10=gap_,p11=0.0;
					dbdet_edgel_list_iter eit3=new_chain4->edgels.begin();
					dbdet_edgel_list_iter eit4=new_chain4->edgels.begin();

					// find the closest unused edge to 'edge1', saved in 'edge4'
					for(int a=0;a<new_chain4->edgels.size();a++)
					{
						p11= vgl_distance(edge1->pt,new_chain4->edgels[a]->pt);
						if(p11<p10)
						{
							double d8=vgl_distance(new_chain4->edgels[a]->pt,edge2->pt);
							double d9=vgl_distance(new_chain4->edgels[a]->pt,edge1->pt);
							double dx1 = edge1->pt.x() - edge2->pt.x(), dy1 = edge1->pt.y() - edge2->pt.y();
							double dx2=new_chain4->edgels[a]->pt.x()-edge1->pt.x();
							double dy2=new_chain4->edgels[a]->pt.y()-edge1->pt.y();
							if(d8<d9 || ((dx1*dx2 + dy1*dy2)/vcl_sqrt(dx1*dx1+dy1*dy1)/vcl_sqrt(dx2*dx2+dy2*dy2))<0.4)
							{
								++eit3;
								continue;
							}
							p10=p11;
							edge4=new_chain4->edgels[a];
							eit4=eit3;
							p2=1;
						}
						++eit3;
					}

					double p12=gap_,p13=0,p14=0.0,p15=0.0;p16=0;dbdet_edgel* edge5=0;
					for(int t=0;t<new_chain3.size(); t++)
					{
						if(new_chain3[t]==edge1) continue;                    			       		
						p13= vgl_distance(edge1->pt,new_chain3[t]->pt);
						if(p13<=p12)
						{
							for(int c=p8;c<p9;c++)
							{
								if(new_chain3[t]==chain1->edgels[c])
									goto jump;
								else
									continue;
							}
							for(int a=0;a<new_chain5->edgels.size();a++)
							{
								if(new_chain3[t]==new_chain5->edgels[a])
									goto jump;
								else
									continue;
							}
							for(int b=0;b<new_chain6a->edgels.size();b++)
							{
								if(new_chain3[t]==new_chain6a->edgels[b])
									goto jump;
								else
									continue;
							}
							edge5=new_chain3[t];
							p12=p13;
							p14=vgl_distance(edge5->pt,edge2->pt);
							p15=vgl_distance(edge5->pt,edge1->pt);
							p16=1;
						}
						jump: ;
					}
					if(p14>p15 && p16==1)
					{
						edge4=edge5;
						p2=1;
					}
					if(p2==1)
					{
						new_chain6a->edgels.push_back(edge4);
						p3=1;
						p2=0;
						if(p16==0)
						{
							new_chain4->edgels.erase(eit4);
							new_chain3.push_back(edge4);
							edge2=edge1;
							edge1=edge4;
						}
						else
							break;
					}
					else
						break;
				}
				double p17=0,p18=0,p21=0;
				if(p3==1 && new_chain6a->edgels.size()>5) // minimum size should be 5
				{
					dbdet_edgel* edge6=new_chain6a->edgels[new_chain6a->edgels.size()/2];				
					for(int i=0;i<new_chain33.size();i++)
					{
						p18=vgl_distance(edge6->pt,new_chain33[i]->pt);if(p18<1) p21=10; // if distance is less than 1
						if(p21==10)
						{
							p17=1;
							break;
						}
					}	
					if(p17==0)
					{
						new_chain6a->temp = true;
						curve_frag_graph_.CFTG.insert_fragment(new_chain6a);
					} // insert the fragment
					p3=0;p2=0;p16=0,p3=0;
				}
			}
		}
	}

/*
  	// Add by Yuliang, a indicator shows edges participating in unambiguous frags and hypothesis trees
	vcl_cout << "counting in participate edges" << vcl_endl;

	// count in hypothesis tree edges
	dbdet_CFTG_link_list_iter it_8 = curve_frag_graph_.CFTG.Links.begin();

	for (; it_8 != curve_frag_graph_.CFTG.Links.end(); it_8++)
	{
		dbdet_edgel_chain_list cur_cCFs = (*it_8)->cCFs;
		
		dbdet_edgel_chain_list_iter it_88 = cur_cCFs.begin();
		for(; it_88 != cur_cCFs.end(); it_88++)
		{
			dbdet_edgel_list_iter it_888 = (*it_88)->edgels.begin();
			for(; it_888 != (*it_88)->edgels.end(); it_888++)
				curve_frag_graph_.participate_edge_id.insert((*it_888)->id);
		}
	}
	vcl_cout <<"participate edge count: " <<curve_frag_graph_.participate_edge_id.size() << vcl_endl;

*/

/*	std::set<int>::iterator it = curve_frag_graph_.participate_edge_id.begin();
	for (; it!=curve_frag_graph_.participate_edge_id.end(); it++)
		vcl_cout << (*it) << " ";
	vcl_cout << vcl_endl;
*/

	vcl_cout << "Hypothesis Tree Constructed!!" << vcl_endl;	
}

// New Disambiguation Process by Naman Kumar
void dbdet_sel_base::Disambiguation()
{
	vcl_cout << "Disambiguating the Hypothesis Tree!!" << vcl_endl;
	dbdet_CFTG_link_list_iter l_it = curve_frag_graph_.CFTG.Links.begin();
	for (; l_it != curve_frag_graph_.CFTG.Links.end(); l_it++)
	{
        	double cost=0.0;
        	int deg_S = curve_frag_graph_.CFTG.cLinks[(*l_it)->eS->id].size() + curve_frag_graph_.CFTG.pLinks[(*l_it)->eS->id].size();
        	dbdet_CFTG_link* cur_Link = (*l_it);
		//Calculating the Cost        	
		vcl_vector<dbdet_edgel*> dummy_chain;
        	dbdet_edgel_chain* edgel_chain = cur_Link->cCFs.front();
        	vcl_vector<dbdet_edgel*> chain(edgel_chain->edgels.begin(),edgel_chain->edgels.end());
	 	cost = compute_path_metric2(dummy_chain, chain, dummy_chain);
        	//Degree = 1
        	if(deg_S==1) {curve_frag_graph_.insert_fragment((*l_it)->cCFs.front()); continue;}
        	//Degree > 1
        	// To fill in the small gaps in closed contours
		if((curve_frag_graph_.pFrags[edgel_chain->edgels.front()->id].size()+curve_frag_graph_.cFrags[edgel_chain->edgels.front()->id].size())==1    			&&(curve_frag_graph_.pFrags[edgel_chain->edgels.back()->id].size()+curve_frag_graph_.cFrags[edgel_chain->edgels.back()->id].size())==1 			&& edgel_chain->edgels.size()==2) curve_frag_graph_.insert_fragment(edgel_chain);
        	if(cost<1.0 && edgel_chain->edgels.size()>2) curve_frag_graph_.insert_fragment(edgel_chain); 
	}
	//clear the graph
	curve_frag_graph_.CFTG.clear();
	curve_frag_graph_.CFTG.resize(edgemap_->edgels.size());
}

// By Naman Kumar: a minor function just to prune some extra small part of contours
void dbdet_sel_base::Post_Process()
{

	dbdet_edgel_chain* new_chain= new dbdet_edgel_chain();
  // iterating through all the contours and deleting those that have length less than a certain threshold
	for (int i=0; i<edgemap_->edgels.size(); i++)
    	{
        	dbdet_edgel* eA1 = edgemap_->edgels[i];
            	if ((curve_frag_graph_.pFrags[eA1->id].size() + curve_frag_graph_.cFrags[eA1->id].size()) ==1) new_chain->edgels.push_back(eA1);
    	}   
		for(int j=0;j<new_chain->edgels.size();j++)
    	{
        	dbdet_edgel* edge=new_chain->edgels[j]; dbdet_edgel* edge2=0;dbdet_edgel_chain* chain= new dbdet_edgel_chain();dbdet_edgel* edge3=0;
        	int n=0,number=0,num=0,diff=0;dbdet_edgel* edge4=0;
        	if(curve_frag_graph_.cFrags[edge->id].size()==1) // if size of child fragment is 1
            	{
            		n=1;dbdet_edgel_chain_list_iter ccit = curve_frag_graph_.cFrags[edge->id].begin();chain=*ccit;
            		edge2 = chain->edgels[1];if(chain->edgels.size()>2){number=1;edge3=chain->edgels[2];}diff=1;
		          }
            	else if(curve_frag_graph_.pFrags[edge->id].size()==1) // if size of parent fragment is 1
            	{
            		n=1;dbdet_edgel_chain_list_iter pcit = curve_frag_graph_.pFrags[edge->id].begin();chain=*pcit;
            		edge2 = chain->edgels[chain->edgels.size()-2];
            		if(chain->edgels.size()>2){number=1;edge3=chain->edgels[chain->edgels.size()-3];}diff=2;
		          } 

        	dbdet_edgel_chain_list_iter fit = curve_frag_graph_.frags.begin();
          // going through all the curve fragments of the graph
        	for(;fit!=curve_frag_graph_.frags.end();fit++)
            	{dbdet_edgel_chain *test1=*fit;if(test1==chain)continue;for(int d=0;d<test1->edgels.size();d++){if(edge==test1->edgels[d]) goto end;}}
        	if(n==1 && chain->edgels.size()>1 && ((curve_frag_graph_.pFrags[edge2->id].size() + curve_frag_graph_.cFrags[edge2->id].size()) >=1))
        	{
            		curve_frag_graph_.extract_fragment(chain); // extract the fragment
            		if(diff==1) chain->edgels.pop_front();else if(diff==2) chain->edgels.pop_back();curve_frag_graph_.insert_fragment(chain);

        	}
        	else if(number==1 && chain->edgels.size()>1 && ((curve_frag_graph_.pFrags[edge3->id].size()+curve_frag_graph_.cFrags[edge3->id].size()) >=1))
        	{           
            		curve_frag_graph_.extract_fragment(chain); // extract the fragment
                	if(diff==1) {chain->edgels.pop_front();chain->edgels.pop_front();}
            		else if(diff==2){chain->edgels.pop_back();chain->edgels.pop_back();}curve_frag_graph_.insert_fragment(chain);
        	}
       		end: ;
    	}
} 


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//***********************************************************************//
// Functions resolving ambiguity based on edge topology graph (ETG):
//	junction detection and curve extraction
//***********************************************************************//
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

//: construct a dynamic edge hypothesis tree rooted at a given edgel,
//  attach hyp paths with each participating edgel. This is based on Dijstra's algorithm
dbdet_DEHT* dbdet_sel_base::construct_dyn_hyp_tree(dbdet_edgel* edge)
{
  if (edge_link_graph_.cLinks.size()==0){
    vcl_cout << "No Link Graph !" <<vcl_endl;
    return 0;
  }

  vcl_vector<dbdet_edgel*> dummy_chain;
  vcl_map<int, dbdet_DEHT_node*> node_list; // a list all nodes involving in the tree
  vcl_map<int, int> used_node; // frag of used nodes in constructing this tree
  vcl_map<int, dbdet_DEHT_node*> pri_queue; // This has to use a priority queue, whose contents are sorted by the cost

  //construct a Tree, assign the root
  dbdet_DEHT* Tree = new dbdet_DEHT();
  dbdet_DEHT_node* root1 = new dbdet_DEHT_node(edge);
  root1->path.push_back(edge);
  root1->path_cost = 0;
  Tree->root = root1;
  node_list[edge->id] = root1;
  //Tree->all_nodes.push_back(root1);

  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  //Expend the Tree until the connected component is fully explored, or the number of edgels achieve 10^4
  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  //pri_queue.push(root1);
  pri_queue[root1->e->id] = root1;
  int num_node = 0;
  while (!pri_queue.empty() && vcl_log10(double(num_node))<4)
  {

	// Always propagate from the lowest cost
	double min_cost = INF_COST;
	vcl_map<int, dbdet_DEHT_node*>::iterator tit, min_tit;
	for (tit = pri_queue.begin(); tit != pri_queue.end(); tit++ )
	{
		if(tit->second->path_cost < min_cost)
		{
			min_cost = tit->second->path_cost;
			min_tit = tit;
		}
	}
	dbdet_DEHT_node* cur_node = min_tit->second;
	pri_queue.erase(min_tit);

	/*
    //if we reach an end point, do not propagate
    if (cur_node!= root1 &&
        (curve_frag_graph_.pFrags[cur_node->e->id].size()>0 ||
         curve_frag_graph_.cFrags[cur_node->e->id].size()>0))
      continue;

    */
    //if we reach an edgel that is already linked in unambiguous curve fragments, do not propagate
    if (cur_node!= root1 && edge_link_graph_.linked[cur_node->e->id])
      continue;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // propagate this node's neighborhood: the union of cLinks and pLinks in edge_link_graph
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // explore in cLinks
    ////////////////////////////////////////////////////////////////////////////////////////////////

    vcl_vector<dbdet_edgel*> connected_frag;
    if(curve_frag_graph_.pFrags[edge->id].size()>=1)
    {
    	int sz = curve_frag_graph_.pFrags[edge->id].front()->edgels.size();
		connected_frag.push_back(curve_frag_graph_.pFrags[edge->id].front()->edgels[sz-2]);
		//connected_frag.push_back(curve_frag_graph_.pFrags[edge->id].front()->edgels[sz-1]);
    }
    else if(curve_frag_graph_.cFrags[edge->id].size()>=1)
    {
		connected_frag.push_back(curve_frag_graph_.cFrags[edge->id].front()->edgels[1]);
		//connected_frag.push_back(curve_frag_graph_.cFrags[edge->id].front()->edgels[0]);
    }

    dbdet_link_list_iter lit = edge_link_graph_.cLinks[cur_node->e->id].begin();
    for (; lit != edge_link_graph_.cLinks[cur_node->e->id].end(); lit++)
    {
      //if we reach an edgel that included in unambiguous curve fragments but not an end point, skip it
      int deg = curve_frag_graph_.pFrags[(*lit)->ce->id].size() + curve_frag_graph_.cFrags[(*lit)->ce->id].size();
      if (edge_link_graph_.linked[(*lit)->ce->id] && deg<1)
        continue;
      // if this node has been fully propagated in the search
      if(used_node[(*lit)->ce->id])
    	continue;
	  // if jump too far, skip it
	  if(vgl_distance((*lit)->ce->pt, cur_node->e->pt) > 2 && cur_node != root1)
		  continue;
      if (cur_node->parent) {
    	// don't trace the direction of cur_node->parent
        if((*lit)->ce->id == cur_node->parent->e->id)
          continue;
        //make a simple geometric consistency check
        double dx1 = cur_node->e->pt.x() - cur_node->parent->e->pt.x();
        double dy1 = cur_node->e->pt.y() - cur_node->parent->e->pt.y();
        double dx2 = (*lit)->ce->pt.x() - cur_node->e->pt.x();
        double dy2 = (*lit)->ce->pt.y() - cur_node->e->pt.y();

        // if not consistent, skip it
        if (((dx1*dx2 + dy1*dy2)/vcl_sqrt(dx1*dx1+dy1*dy1)/vcl_sqrt(dx2*dx2+dy2*dy2))<0) //not consistent, but with lower TH to keep more Hypothesis by Yuliang ////////// Cosine Formula
          continue;
      }

      // Check: is to update existing node or to add a new node
      // A node indicates its optimal path to the root via holding only one parent
      vcl_vector<dbdet_edgel*> path1 = cur_node->path;
      path1.push_back((*lit)->ce);
      // TODO: this need to be changed to a accumulate version to improve efficiency
      double cost1 = dbdet_sel_base::compute_path_metric3(connected_frag,path1,dummy_chain);
      // if this a node with the same edgel already exist in the tree
      if(node_list.find((*lit)->ce->id)!=node_list.end()) {
    	  //vcl_cout << "reach a previous added node" << vcl_endl;
    	  dbdet_DEHT_node* child_node = node_list[(*lit)->ce->id];
    	  vcl_vector<dbdet_edgel*> path2 = child_node->path;
          // TODO: this need to be changed to a accumulate version to improve efficiency
          double cost2 = dbdet_sel_base::compute_path_metric3(connected_frag,path2,dummy_chain);
          if(cost1>=cost2) // this path will not be considered
        	  continue;
          else { // update existing node
    		  cur_node->add_child(child_node); // modify links, child_node's parent will change
        	  child_node->path = path1; // update path
        	  child_node->path_cost = cost1; // update path
         }
      }
      //else add a new node with this edgel
      else {
		  dbdet_DEHT_node* child_node = new dbdet_DEHT_node((*lit)->ce);
		  cur_node->add_child(child_node); // modify links
    	  child_node->path = path1; // update path
    	  child_node->path_cost = cost1;
		  //pri_queue.push(child_node);
		  pri_queue[child_node->e->id] = child_node;
		  node_list[child_node->e->id] = child_node;
		  //Tree->all_nodes.push_back(child_node);
		  num_node ++;
      }
    }
    /////////////////////////////////
    // explore in pLinks
    /////////////////////////////////
    lit = edge_link_graph_.pLinks[cur_node->e->id].begin();
    for (; lit != edge_link_graph_.pLinks[cur_node->e->id].end(); lit++)
    {
      //if we reach an edgel that included in unambiguous curve fragments but not an end point, skip it
      int deg = curve_frag_graph_.pFrags[(*lit)->pe->id].size() + curve_frag_graph_.cFrags[(*lit)->pe->id].size();
      if (edge_link_graph_.linked[(*lit)->pe->id] && deg<1)
        continue;
      // if this node has been fully propagated in the search
      if(used_node[(*lit)->pe->id])
    	continue;
	  // if jump too far, skip it
	  if(vgl_distance((*lit)->pe->pt, cur_node->e->pt) > 2 && cur_node != root1)
		  continue;
      if (cur_node->parent) {
      	// don't trace the direction of cur_node->parent
        if((*lit)->pe->id == cur_node->parent->e->id)
		  continue;
        //make a simple geometric consistency check
		double dx1 = cur_node->e->pt.x() - cur_node->parent->e->pt.x();
		double dy1 = cur_node->e->pt.y() - cur_node->parent->e->pt.y();
		double dx2 = (*lit)->pe->pt.x() - cur_node->e->pt.x();
		double dy2 = (*lit)->pe->pt.y() - cur_node->e->pt.y();

        // if not consistent, skip it
		if (((dx1*dx2 + dy1*dy2)/vcl_sqrt(dx1*dx1+dy1*dy1)/vcl_sqrt(dx2*dx2+dy2*dy2))<0) //not consistent, but with lower TH to keep more Hypothesis by Yuliang
		  continue;
      }

      // Check: is to update existing node or to add a new node
      // A node indicates its optimal path to the root via holding only one parent
      vcl_vector<dbdet_edgel*> path1 = cur_node->path;
      path1.push_back((*lit)->pe);
      // TODO: this need to be changed to a accumulate version to improve efficiency
      double cost1 = dbdet_sel_base::compute_path_metric3(connected_frag,path1,dummy_chain);
      // if this a node with the same edgel already exist in the tree
      if(node_list.find((*lit)->pe->id)!=node_list.end()) {
    	  //vcl_cout << "reach a previous added node" << vcl_endl;
    	  dbdet_DEHT_node* child_node = node_list[(*lit)->pe->id];
    	  vcl_vector<dbdet_edgel*> path2 = child_node->path;
          // TODO: this need to be changed to a accumulate version to improve efficiency
          double cost2 = dbdet_sel_base::compute_path_metric3(connected_frag,path2,dummy_chain);
          if(cost1>=cost2) // this path will not be considered
        	  continue;
          else { // update existing node
			  cur_node->add_child(child_node); // modify links, child_node's parent will change
			  child_node->path = path1; // update path
			  child_node->path_cost = cost1; // update path
          }
      }
      //else add a new node with this edgel
      else {
    	  //vcl_cout << "add a new node" << vcl_endl;
		  dbdet_DEHT_node* child_node = new dbdet_DEHT_node((*lit)->pe);
		  cur_node->add_child(child_node); // modify links
    	  child_node->path = path1; // update path
    	  child_node->path_cost = cost1; // update path
		  //pri_queue.push(child_node);
		  pri_queue[child_node->e->id] = child_node;
		  node_list[child_node->e->id] = child_node;
		  //Tree->all_nodes.push_back(child_node);
		  num_node ++;
      }
    }

    // mark cur_node used, that is, all its neighborhood propagated
    used_node[cur_node->e->id] = 1;
  }
  //vcl_cout << "Constructed dynamic tree, node list size: " << node_list.size() << vcl_endl;
  //empty the bfs queue, in case the max number of nodes condition reached
  while (!pri_queue.empty())
	  pri_queue.clear();

  double best_cost = INF_COST;
  int best_free_end_len = 0;
  int count_path = 0;

  //////////////////////////////////////////////////////
  // Attach qualified paths to the participating edges
  //////////////////////////////////////////////////////
	  //vcl_cout << "number of nodes: " << node_list.size() << vcl_endl;
  vcl_map<int, dbdet_DEHT_node*>::iterator mit = node_list.begin();
  for (; mit!=node_list.end(); mit++) // it includes root node
  {
	  dbdet_DEHT_node* cur_node =  mit->second;
	  vcl_vector<dbdet_edgel*> path2add = cur_node->path;
	  // A legal attached path to junction
	  if(is_JCT_path_legal(path2add)) // short paths will not be considered
	  {
		  dbdet_edgel_chain* new_chain = new dbdet_edgel_chain();
		  new_chain->append(path2add);
		  curve_frag_graph_.HypFrags[cur_node->e->id].push_back(new_chain);
		  curve_frag_graph_.edgels_having_attached_paths[cur_node->e->id] = 1;
		  count_path ++;
	  }
	  // consider the best path to be the shortest path to a leaf node, that ends at a linked edge, it need to satisfy more restrict consistency check
	  //if(cur_node->children.size()==0 && cur_node->path_cost < best_cost)
	  int deg = curve_frag_graph_.cFrags[cur_node->e->id].size() + curve_frag_graph_.pFrags[cur_node->e->id].size();
	  vcl_deque<dbdet_edgel*> vvv(cur_node->path.begin(), cur_node->path.end());
	  //double cur_len = compute_path_len(vvv);
	  //if(cur_node->children.size()==0 && cur_len < best_cost && edge_link_graph_.linked[ cur_node->path.back()->id] && is_EHT_path_legal(path2add))
	  if(cur_node->children.size()==0 && cur_node->path_cost < best_cost && edge_link_graph_.linked[ cur_node->path.back()->id] && is_EHT_path_legal(path2add))
	  {
			  best_cost = cur_node->path_cost;
			  //best_cost = cur_len;
			  Tree->best_path = cur_node->path;
	  }

	  // if it is a leaf node, consider it to be the longest free end path in the tree
	  if(cur_node->children.size()==0 && cur_node->path.size() > best_free_end_len && deg==0 && is_EHT_path_legal(path2add))
	  {
			  best_free_end_len = cur_node->path.size();
			  Tree->best_free_end_path = cur_node->path;
	  }


  }

  //vcl_cout << "Attached paths from dynamic tree: " << count_path << vcl_endl;
  return Tree;
}

//: construct possible dynamic edge hypothesis Trees from the all the end points of curve frags,
void dbdet_sel_base::construct_all_DEHTs()
{
  //go over the contour fragment graph and form an EHT from every terminal node
  //validate each of the paths in the EHT

  vcl_vector<dbdet_edgel_chain*> new_frags;
  curve_frag_graph_.edgels_having_attached_paths.assign(edgemap_->edgels.size(), 0);

  //construct DEHT rooted at each edge point
  for (unsigned i=0; i<edgemap_->edgels.size(); i++)
  {
    dbdet_edgel* eA = edgemap_->edgels[i];

    if (curve_frag_graph_.pFrags[i].size() + curve_frag_graph_.cFrags[i].size() < 1) //no end point found
      continue;

	curve_frag_graph_.end_points.push_back(eA);

    // if an end point is found, construct an DEHT from it
    dbdet_DEHT* DEHT1 = construct_dyn_hyp_tree(eA);
    if (!DEHT1->best_path.empty()) // if there is a path ends at linked edge
    {
    	curve_frag_graph_.edgels_having_attached_paths[DEHT1->root->e->id] = 1;
    	curve_frag_graph_.best_paths[eA->id] = DEHT1->best_path;
    	//curve_frag_graph_.end_points_flag.push_back(0);
      //delete DEHT1; // free memory
    }

    if (!DEHT1->best_free_end_path.empty()) // if there is free_end path
    {
    	curve_frag_graph_.edgels_having_attached_paths[DEHT1->root->e->id] = 1;
    	curve_frag_graph_.best_free_end_paths[eA->id] = DEHT1->best_free_end_path;
    }
  }
  vcl_cout<<"Finish constructing all dynamic hypothesis trees"<<vcl_endl;
}

//: resolving junction based on the attached hyp paths:
//  find compatible junctions, and extract the participating curve fragments
void dbdet_sel_base::resolve_junction_conflict()
{
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// look for junction candidates
	// Refine attached paths to be non-overlapping with each other
	// the case that a junction locates at an end point is included
	/////////////////////////////////////////////////////////////////////////////////////////////////

	curve_frag_graph_.junction_edgels.assign(edgemap_->edgels.size(), 0);

	for (int i = 0; i< curve_frag_graph_.edgels_having_attached_paths.size(); i++)
	{
		if(!curve_frag_graph_.edgels_having_attached_paths[i])
			continue;

		int cur_eid = i;

		// extract the set of non-overlapping set of attached paths, count the number
		int num_non_overlap_paths = check_a_junction(cur_eid);
		// If there are >=3 non-overlapping attached paths, it is considered as a junction candidate
		if(num_non_overlap_paths >=3)
		{
			curve_frag_graph_.junction_edgels[cur_eid] = 1;
			//vcl_cout << "find a junction with non-overlapping paths: " << num_non_overlap_paths << vcl_endl;
		}

	}

	/////////////////////////////////////////////////////////////////////////////////////////////////
	// Special Case: the best_path growing from current end point ends at a free edgel
	// check if any junction appear on the path
	/////////////////////////////////////////////////////////////////////////////////////////////////
    int min_subpath_len = 3; // Parameter: the sub path to add should at least include 5 edges
	for (int k = 0; k< curve_frag_graph_.end_points.size(); k++)
	{
		int cur_eid = curve_frag_graph_.end_points[k]->id;
		if(curve_frag_graph_.best_free_end_paths.find(cur_eid)==curve_frag_graph_.best_free_end_paths.end()) // skip end points without any qualified free end path
			continue;
		vcl_vector<dbdet_edgel*> best_path = curve_frag_graph_.best_free_end_paths[cur_eid];

		if(best_path.size()<min_subpath_len+1)
			continue;

		int last_eid = best_path.back()->id;
		if(curve_frag_graph_.pFrags[last_eid].size() + curve_frag_graph_.cFrags[last_eid].size() == 0) // end at a free end
		{
			//dbdet_edgel_chain* new_chain = new dbdet_edgel_chain();
			//new_chain->append(best_path);
			//curve_frag_graph_.insert_fragment(new_chain);

			for(int i=1; i< best_path.size()-min_subpath_len+1; i++)
			{
				int HpyFrags_num = curve_frag_graph_.HypFrags[best_path[i]->id].size();
				if(HpyFrags_num>=2) // those junctions including >2 attach paths are already in the list, but still consider any additional attached path due to free end
				{
					// extract two sub-paths, from 0 to i, and from i to end
					vcl_vector<dbdet_edgel*> edges_01;
					for (int j = 0; j<=i; j++)
					{
						edges_01.push_back(best_path[i-j]);
					}

					vcl_vector<dbdet_edgel*> edges_02;
					for (int j = i; j<best_path.size(); j++)
					{
						edges_02.push_back(best_path[j]);
					}

					// attach the sub-path when it is not overlapping with any existing attached path
					bool add_subpath_1 = true;
					bool add_subpath_2 = true;

					dbdet_edgel_chain_list_iter clit1;
					for( clit1= curve_frag_graph_.HypFrags[best_path[i]->id].begin(); clit1!=curve_frag_graph_.HypFrags[best_path[i]->id].end(); clit1++)
					{
						dbdet_edgel_chain* path_1 = *clit1;
						vcl_vector<dbdet_edgel*> edges_1;
						for (int j = 0; j< path_1->edgels.size(); j++)
						{
							edges_1.push_back(path_1->edgels[path_1->edgels.size()-1-j]);
						}

						// found a non-overlapping path
						add_subpath_1 &= !is_overlapping(edges_01, edges_1);
						add_subpath_2 &= !is_overlapping(edges_02, edges_1);
					}


					if(add_subpath_1)
					{
						vcl_reverse(edges_01.begin(), edges_01.end());
						dbdet_edgel_chain* new_chain_01 = new dbdet_edgel_chain();
						new_chain_01->append(edges_01);
						curve_frag_graph_.HypFrags[best_path[i]->id].push_back(new_chain_01);
					}

					if(add_subpath_2)
					{
						vcl_reverse(edges_02.begin(), edges_02.end());
						dbdet_edgel_chain* new_chain_02 = new dbdet_edgel_chain();
						new_chain_02->append(edges_02);
						curve_frag_graph_.HypFrags[best_path[i]->id].push_back(new_chain_02);
					}

					// update marker of junction edge
					if(curve_frag_graph_.HypFrags[best_path[i]->id].size() >=3)
					{
						curve_frag_graph_.junction_edgels[best_path[i]->id] = 1;
					}

				}
			}
		}
	}


    ////////////////////////////////////////////////////////////////////////////////////////////////
	// Topological non maximum suppression of junction candidates
    ////////////////////////////////////////////////////////////////////////////////////////////////
	for (int i = 0 ; i< curve_frag_graph_.junction_edgels.size(); i++)
	{
		if(!curve_frag_graph_.junction_edgels[i])
			continue;

		int cur_eid = i;
		//topologial_NMS (cur_eid, best_saliency, cur_eid, cur_eid);
		topologial_NMS (cur_eid);
	}

    ////////////////////////////////////////////////////////////////////////////////////////////////
	// Resolving interaction between junctions, and extract related curve fragments
    ////////////////////////////////////////////////////////////////////////////////////////////////

	for (int i = 0 ; i< curve_frag_graph_.junction_edgels.size(); i++)
	{
		if(!curve_frag_graph_.junction_edgels[i])
			continue;

		// Constraint: there is only one junction connecting a unique set of end points
		int cur_eid = i;
		check_jct_compatibility (cur_eid);
	}

	vcl_cout << "Suppress conflict junctions" << vcl_endl;

	// extract curve fragments related to junctions
	for (int i = 0 ; i< curve_frag_graph_.junction_edgels.size(); i++)
	{
		if(!curve_frag_graph_.junction_edgels[i])
			continue;
		int cur_eid = i;
		cut_jct_curve_fragment (cur_eid);
		dbdet_edgel_chain_list_iter clit1;
		for( clit1= curve_frag_graph_.HypFrags[cur_eid].begin(); clit1!=curve_frag_graph_.HypFrags[cur_eid].end(); clit1++)
		{
			curve_frag_graph_.insert_fragment(*clit1); // simply insert curve fragment to check
			for(int j=0; j< (*clit1)->edgels.size(); j++)
				edge_link_graph_.linked[(*clit1)->edgels[j]->id]=1;
		}
		if(curve_frag_graph_.pFrags[cur_eid].size() + curve_frag_graph_.cFrags[cur_eid].size()<3)
			curve_frag_graph_.junction_edgels[i]=0; // update junction labels after cut
	}

	vcl_cout << "Extract junctions curve fragments" << vcl_endl;


	bool prune = true;
	if(prune)
	{
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Consider the T-junctions formed in extracting junction-associated curve fragments as false positive
	//locate them, only keep the most continuous two branches, based on local geometric continuity
	////////////////////////////////////////////////////////////////////////////////////////////////////////

	dbdet_edgel_chain_list_iter clit0 = curve_frag_graph_.frags.begin();
	while (clit0!=curve_frag_graph_.frags.end())
	{
		dbdet_edgel_chain* cur_chain = *clit0;
		vcl_vector <int> FP_jct_index;
		for (int j = 1; j<cur_chain->edgels.size()-1; j++)
		{
			// if it is a false positive (FP) junction, save it to check later
			// only the T-junction formed from cFrag is the False Positive junction formed in the previous stage
			if(curve_frag_graph_.pFrags[cur_chain->edgels[j]->id].size() == 0 && curve_frag_graph_.cFrags[cur_chain->edgels[j]->id].size() == 1)
			{
				FP_jct_index.push_back(j);
			}

			// a special case two FP junction joint together, just remove both path, this need to be solved after ==1 case
			if(curve_frag_graph_.cFrags[cur_chain->edgels[j]->id].size() >1)
			{
				FP_jct_index.push_back(j); // the later case will solve this one by one
				//dbdet_edgel_chain_list_iter clit1=curve_frag_graph_.cFrags[cur_chain->edgels[j]->id].begin();
				//for (;clit1!=curve_frag_graph_.cFrags[cur_chain->edgels[j]->id].end(); clit1 ++)
				//{
				//	curve_frag_graph_.extract_fragment(*clit1);
				//	clit1--;
				//}
				//continue;
			}

		}

		// For the case there are 2 junctions
		if(FP_jct_index.size()==2)
		{
			int j1 = FP_jct_index[0];
			int j2 = FP_jct_index[1];
			dbdet_edgel_chain* joint_chain1 = curve_frag_graph_.cFrags[cur_chain->edgels[j1]->id].front();
			dbdet_edgel_chain* joint_chain2 = curve_frag_graph_.cFrags[cur_chain->edgels[j2]->id].front();

			if(joint_chain1->edgels.back()==joint_chain2->edgels.back()) // both the joint paths are from the same junction
			{
				//vcl_cout << "Found double FP T-junction:"<< cur_chain->edgels[j1]->id << " " << cur_chain->edgels[j2]->id << vcl_endl;
				//				   /  \
				//				 /		\
				//			  joint1   joint2
				//				|		 |
				//				|		 |
				//        sub 1	v sub 2  v  sub 3
				//	    ------->o------->o-------->
				// remove the segment of curve between the two junctions: sub 2
				// update flag of linked edges
				//for(int k=j1+1;k<j2;k++)
					//edge_link_graph_.linked[cur_chain->edgels[k]->id]=0;
				clit0 --;
				curve_frag_graph_.extract_fragment(cur_chain);// remove the pointer from the graph
				vcl_vector<dbdet_edgel*> sub_path_1;
				for (int k = 0; k<=j1; k++)
				{
					sub_path_1.push_back(cur_chain->edgels[k]);
				}
				vcl_vector<dbdet_edgel*> sub_path_3;
				for (int k = j2; k<cur_chain->edgels.size(); k++)
				{
					sub_path_3.push_back(cur_chain->edgels[k]);
				}
				//TODO: check whether joint1 and joint2 form a narrow triangle, in which case, removing both joint paths might be preferred

				// insert sub 1 and sub 3
				dbdet_edgel_chain* new_chain1 = new dbdet_edgel_chain();
				new_chain1->append(sub_path_1);
				curve_frag_graph_.insert_fragment(new_chain1);

				dbdet_edgel_chain* new_chain3 = new dbdet_edgel_chain();
				new_chain3->append(sub_path_3);
				curve_frag_graph_.insert_fragment(new_chain3);

				clit0 ++;
				continue;
			}
		}
		// if there is only one FP jct, or more than 2 FP jct, just handle the first
		if(FP_jct_index.size()>0)
		{
			int j = FP_jct_index[0];
			//vcl_cout << "Found a possible FP T-junction:"<< cur_chain->edgels[j]->id << vcl_endl;
			//			  joint
			//				|
			//				|
			//      sub 1 	v   sub 2
			//	   -------> o ------->

			dbdet_edgel_chain* joint_chain;
			joint_chain = curve_frag_graph_.cFrags[cur_chain->edgels[j]->id].front();
			curve_frag_graph_.extract_fragment(joint_chain);
			vcl_reverse(joint_chain->edgels.begin(), joint_chain->edgels.end());// need to reverse because it's originally pointing outward

			vcl_vector<dbdet_edgel*> sub_path_1;
			for (int k = 0; k<=j; k++)
			{
				sub_path_1.push_back(cur_chain->edgels[k]);
			}
			vcl_vector<dbdet_edgel*> sub_path_2;
			for (int k = j; k<cur_chain->edgels.size(); k++)
			{
				sub_path_2.push_back(cur_chain->edgels[k]);
			}

			////////////////////////////////////////////////////////////////////////////////////////////////
			// A special case that two paths from junctions cross over each other, X links
			////////////////////////////////////////////////////////////////////////////////////////////////
			bool has_cross_case = false;
			int jct1 = joint_chain->edgels.front()->id;
			int jct2 = cur_chain->edgels.back()->id;
			if(curve_frag_graph_.pFrags[jct1].size() + curve_frag_graph_.cFrags[jct1].size() >=3 &&
					curve_frag_graph_.pFrags[jct2].size() + curve_frag_graph_.cFrags[jct2].size() >=3)
			{
				// NOTE: HypFrags may have conflict with curve fragment graph, use the actual fragments now
				dbdet_edgel_chain_list cur_jct2_paths;
				dbdet_edgel_chain_list_iter clit3;
				for(clit3=curve_frag_graph_.pFrags[jct2].begin(); clit3!=curve_frag_graph_.pFrags[jct2].end(); clit3++)
				{
					int other_end_id = (*clit3)->edgels.front()->id;
					if(other_end_id == jct2) // skip circle
						continue;

					cur_jct2_paths.push_back(*clit3);
				}
				for(clit3=curve_frag_graph_.cFrags[jct2].begin(); clit3!=curve_frag_graph_.cFrags[jct2].end(); clit3++)
				{
					int other_end_id = (*clit3)->edgels.back()->id;
					if(other_end_id == jct2) // skip circle
						continue;

					cur_jct2_paths.push_back(*clit3);
				}

				// check if another path from jct1 (path3) intersect at another path from jct 2
				dbdet_edgel_chain* joint_chain2=0;
				dbdet_edgel_chain* cur_chain2=0;
				int intersect_j = 0;
				clit3=  cur_jct2_paths.begin();
				while (clit3!=  cur_jct2_paths.end())
				{
					if(*clit3==cur_chain)
					{
						clit3++;
						continue;
					}
					//
					for (int k = 1; k<(*clit3)->edgels.size()-1; k++)
					{
						int eid_3 = (*clit3)->edgels[k]->id;
						if(curve_frag_graph_.pFrags[eid_3].size()==0 && curve_frag_graph_.cFrags[eid_3].size()==1
								&& curve_frag_graph_.cFrags[eid_3].front()->edgels.back()->id == jct1)
						{
							//vcl_cout << "found possible double cross between jct: "<<jct1 << " and jct: " << jct2 << vcl_endl;
							joint_chain2 = curve_frag_graph_.cFrags[eid_3].front();
							cur_chain2 = *clit3;
							intersect_j = k;
						}
					}
					clit3++;
				}

				// check if joint_chain2 cross over any paths from jct2
				for (clit3= cur_jct2_paths.begin(); clit3!= cur_jct2_paths.end(); clit3++)
				{
					// remove the joint_chain that cross over
					if(is_cross_over(*clit3, joint_chain) && (*clit3)!=cur_chain)
					{
						//vcl_cout << "remove joint chain from jct1: "<<jct1 <<vcl_endl;
						// do not need to extract joint_chain which is already extracted

						if(cur_chain2)
						{
							// make the remaining intersect point as true junction
							curve_frag_graph_.extract_fragment(cur_chain2);
							vcl_vector<dbdet_edgel*> vvv(cur_chain2->edgels.begin()+intersect_j, cur_chain2->edgels.end());
							dbdet_edgel_chain* new_chain = new dbdet_edgel_chain();
							new_chain->append(vvv);
							cur_chain2->edgels.erase(cur_chain2->edgels.begin()+intersect_j+1, cur_chain2->edgels.end());
							curve_frag_graph_.insert_fragment(cur_chain2);
							curve_frag_graph_.insert_fragment(new_chain);
						}
						has_cross_case = true;
						clit0 --; // this will ensure to check cur_chain again in case there are other FP T-junctions coming afterwards
						// A Special case:
						// for the joint to delete, if there is another T junction in the middle, cut the joint chain, put the remaining portion back
						vcl_reverse(joint_chain->edgels.begin(), joint_chain->edgels.end()); // reverse it back
						for(int kk = 1; kk<joint_chain->edgels.size()-1; kk++)
						{
							if(curve_frag_graph_.pFrags[joint_chain->edgels[kk]->id].size() + curve_frag_graph_.cFrags[joint_chain->edgels[kk]->id].size()>0)
							{
								joint_chain->edgels.erase(joint_chain->edgels.begin(), joint_chain->edgels.begin()+kk);
								curve_frag_graph_.insert_fragment(joint_chain);
								break;
							}
						}

						// update junction label
						if(curve_frag_graph_.pFrags[jct1].size() + curve_frag_graph_.cFrags[jct1].size()<3)
							curve_frag_graph_.junction_edgels[jct1]=0;
						break;
					}
					else if(joint_chain2>0 && intersect_j>0 && is_cross_over(*clit3, joint_chain2) && (*clit3)!=cur_chain2) // intersect_j>0 indicates that there is another intersect path
					{
						//vcl_cout << "remove joint chain 2 from jct1: "<<jct1 <<vcl_endl;
						curve_frag_graph_.extract_fragment(joint_chain2);
						// insert joint_chain back
						vcl_reverse(joint_chain->edgels.begin(), joint_chain->edgels.end()); // reverse it back
						curve_frag_graph_.insert_fragment(joint_chain);
						// make the remaining intersect point as true junction
						clit0--;
						curve_frag_graph_.extract_fragment(cur_chain);
						vcl_vector<dbdet_edgel*> vvv(cur_chain->edgels.begin()+j, cur_chain->edgels.end());
						dbdet_edgel_chain* new_chain = new dbdet_edgel_chain();
						new_chain->append(vvv);
						cur_chain->edgels.erase(cur_chain->edgels.begin()+j+1, cur_chain->edgels.end());
						curve_frag_graph_.insert_fragment(cur_chain);
						curve_frag_graph_.insert_fragment(new_chain);
						has_cross_case = true;
						// update junction label
						if(curve_frag_graph_.pFrags[jct1].size() + curve_frag_graph_.cFrags[jct1].size()<3)
							curve_frag_graph_.junction_edgels[jct1]=0;
						break;
					}
				}
			}


			if(!has_cross_case)
			{
				// The rest is the normal case
				int sz1 = sub_path_1.size();
				double dx_1o =  sub_path_1[sz1-1]->pt.x() - sub_path_1[sz1-2]->pt.x();
				double dy_1o =  sub_path_1[sz1-1]->pt.y() - sub_path_1[sz1-2]->pt.y();
				double dx_o2 = sub_path_2[1]->pt.x() - sub_path_2[0]->pt.x();
				double dy_o2 = sub_path_2[1]->pt.y() - sub_path_2[0]->pt.y();
				double cos_12 = (dx_1o*dx_o2+dy_1o*dy_o2)/vcl_sqrt(dx_1o*dx_1o+dy_1o*dy_1o)/vcl_sqrt(dx_o2*dx_o2+dy_o2*dy_o2);
				double cos_21 = cos_12;

				int szj = joint_chain->edgels.size();
				double dx_jo = joint_chain->edgels[szj-1]->pt.x() - joint_chain->edgels[szj-2]->pt.x();
				double dy_jo = joint_chain->edgels[szj-1]->pt.y() - joint_chain->edgels[szj-2]->pt.y();
				double cos_j2 = (dx_jo*dx_o2+dy_jo*dy_o2)/vcl_sqrt(dx_jo*dx_jo+dy_jo*dy_jo)/vcl_sqrt(dx_o2*dx_o2+dy_o2*dy_o2);
				double cos_j1 = (-dx_1o*dx_jo-dy_1o*dy_jo)/vcl_sqrt(dx_jo*dx_jo+dy_jo*dy_jo)/vcl_sqrt(dx_1o*dx_1o+dy_1o*dy_1o);

				//vcl_vector<dbdet_edgel*> dummy_path;
				//vcl_vector<dbdet_edgel*> joint_path(joint_chain->edgels.begin(), joint_chain->edgels.end());
				//double cost_1 = compute_path_metric3(dummy_path, sub_path_1, dummy_path);
				//double cost_2 = compute_path_metric3(dummy_path, sub_path_2, dummy_path);
				//double cost_j = compute_path_metric3(dummy_path, joint_path, dummy_path);

				if(joint_chain->edgels.size()==2 && compute_path_len(joint_chain->edgels)>2) // if the joint is single long link, remove directly
					clit0 --; // this will ensure to check cur_chain again in case there are other FP T-junctions coming afterwards
				// The case to choose joint over sub1
				else if((cos_j2 > cos_12 + 0.1))// && sub_path_1.size()<5) || (sub_path_1.size()==2 && compute_path_len(sub_path_1)>2))
				{
					clit0 --;
					curve_frag_graph_.extract_fragment(cur_chain);// remove the pointer from the graph, and delete
					joint_chain->append(sub_path_2);
					vcl_reverse(joint_chain->edgels.begin(), joint_chain->edgels.end()); // reverse it back
					curve_frag_graph_.insert_fragment(joint_chain);
					//break;
				}
				// The case to choose joint over sub2
				else if((cos_j1  > cos_21 + 0.1))// && sub_path_2.size()<5) || (sub_path_2.size()==2 && compute_path_len(sub_path_2)>2))
				{
					//vcl_cout << "solve FP T-junction" << vcl_endl;
					clit0 --;
					curve_frag_graph_.extract_fragment(cur_chain);// remove the pointer from the graph, and delete
					vcl_reverse(sub_path_1.begin(), sub_path_1.end());
					joint_chain->append(sub_path_1);
					vcl_reverse(joint_chain->edgels.begin(), joint_chain->edgels.end()); // reverse it back
					curve_frag_graph_.insert_fragment(joint_chain);
					//break;
				}
				else // the default is to remove the joint chain
				{
					clit0 --; // this will ensure to check cur_chain again in case there are other FP T-junctions coming afterwards
					// A Special case:
					// for the joint to delete, if there is another T junction in the middle, cut the joint chain, put the remaining portion back
					vcl_reverse(joint_chain->edgels.begin(), joint_chain->edgels.end()); // reverse it back
					for(int kk = 1; kk<joint_chain->edgels.size()-1; kk++)
					{
						if(curve_frag_graph_.pFrags[joint_chain->edgels[kk]->id].size() + curve_frag_graph_.cFrags[joint_chain->edgels[kk]->id].size()>0)
						{
							joint_chain->edgels.erase(joint_chain->edgels.begin(), joint_chain->edgels.begin()+kk);
							curve_frag_graph_.insert_fragment(joint_chain);
							break;
						}
					}
				}

				// NOTE: the associations with the junction edges are not updated, final junction and its associated curve fragments can be read from output CFG
			}
		}

		clit0 ++;
	}
	}
	refresh_linked_condition();
	vcl_cout << "resolve junction conflict" << vcl_endl;

}

//: extract curve fragments not participating in junctions
void dbdet_sel_base::extract_non_jct_curve_frages()
{
	// extract paths ending at linked edges
	for (int k = 0; k< curve_frag_graph_.end_points.size(); k++)
	{
		int cur_eid = curve_frag_graph_.end_points[k]->id;
		if(curve_frag_graph_.best_paths.find(cur_eid)==curve_frag_graph_.best_paths.end()) // skip end points without any qualified path
			continue;

		if(curve_frag_graph_.cFrags[cur_eid].size() + curve_frag_graph_.pFrags[cur_eid].size() != 1) // the case this end point has been linked in resolving junction
			continue;

		vcl_vector<dbdet_edgel*> best_path = curve_frag_graph_.best_paths[cur_eid];
		for (int i=1; i< best_path.size(); i++)
		{
			if(edge_link_graph_.linked[best_path[i]->id])
			{
				best_path.erase(best_path.begin()+i+1, best_path.end());
				break;
			}
		}

		// if the path to add is to form a T-junction, do not extract
		if(curve_frag_graph_.pFrags[best_path.back()->id].size() + curve_frag_graph_.cFrags[best_path.back()->id].size()<1)
			continue;

		bool found_overlapping = false;
		dbdet_edgel_chain_list_iter clit1;
		for(clit1=curve_frag_graph_.pFrags[cur_eid].begin(); clit1!=curve_frag_graph_.pFrags[cur_eid].end(); clit1++)
		{
			vcl_vector<dbdet_edgel*> curve_1((*clit1)->edgels.begin(), (*clit1)->edgels.end());
			vcl_reverse(curve_1.begin(), curve_1.end());
			// make sure curve_0 curve_1 both point out from cur_eid
			if(is_overlapping(best_path, curve_1))
			{
				found_overlapping = true;
				break;
			}
		}
		for(clit1=curve_frag_graph_.cFrags[cur_eid].begin(); clit1!=curve_frag_graph_.cFrags[cur_eid].end(); clit1++)
		{
			vcl_vector<dbdet_edgel*> curve_1((*clit1)->edgels.begin(), (*clit1)->edgels.end());
			// make sure curve_0 curve_1 both point out from cur_eid
			if(is_overlapping(best_path, curve_1))
			{
				found_overlapping = true;
				break;
			}
		}
		// if it is overlapping, do not extract
		if(found_overlapping)
		{
			continue;
		}

	    dbdet_edgel_chain* new_chain = new dbdet_edgel_chain();
		new_chain->append(best_path);
		curve_frag_graph_.insert_fragment(new_chain);
		// update flag of linked edges
		for(int k=0;k<best_path.size();k++)
			edge_link_graph_.linked[best_path[k]->id]=1;

	}

	// extract paths ending at a free edgel
	for (int k = 0; k< curve_frag_graph_.end_points.size(); k++)
	{
		int cur_eid = curve_frag_graph_.end_points[k]->id;
		if(curve_frag_graph_.best_free_end_paths.find(cur_eid)==curve_frag_graph_.best_free_end_paths.end()) // skip end points without any qualified path
			continue;

		if(curve_frag_graph_.cFrags[cur_eid].size() + curve_frag_graph_.pFrags[cur_eid].size() != 1) // the case this end point has been linked in resolving junction
			continue;

		vcl_vector<dbdet_edgel*> best_path = curve_frag_graph_.best_free_end_paths[cur_eid];
		for (int i=1; i< best_path.size(); i++)
		{
			if(edge_link_graph_.linked[best_path[i]->id])
			{
				best_path.erase(best_path.begin()+i+1, best_path.end());
				break;
			}
		}
		// if the path to add is to form a T-junction, do not extract
		if((curve_frag_graph_.pFrags[best_path.back()->id].size() + curve_frag_graph_.cFrags[best_path.back()->id].size()<1 && edge_link_graph_.linked[best_path.back()->id]) || best_path.size()==2)
			continue;

		bool found_overlapping = false;
		dbdet_edgel_chain_list_iter clit1;
		for(clit1=curve_frag_graph_.pFrags[cur_eid].begin(); clit1!=curve_frag_graph_.pFrags[cur_eid].end(); clit1++)
		{
			vcl_vector<dbdet_edgel*> curve_1((*clit1)->edgels.begin(), (*clit1)->edgels.end());
			vcl_reverse(curve_1.begin(), curve_1.end());
			// make sure curve_0 curve_1 both point out from cur_eid
			if(is_overlapping(best_path, curve_1))
			{
				found_overlapping = true;
				break;
			}
		}
		for(clit1=curve_frag_graph_.cFrags[cur_eid].begin(); clit1!=curve_frag_graph_.cFrags[cur_eid].end(); clit1++)
		{
			vcl_vector<dbdet_edgel*> curve_1((*clit1)->edgels.begin(), (*clit1)->edgels.end());
			// make sure curve_0 curve_1 both point out from cur_eid
			if(is_overlapping(best_path, curve_1))
			{
				found_overlapping = true;
				break;
			}
		}
		// if it is overlapping, do not extract
		if(found_overlapping)
		{
			continue;
		}

	    dbdet_edgel_chain* new_chain = new dbdet_edgel_chain();
		new_chain->append(best_path);
		curve_frag_graph_.insert_fragment(new_chain);
		// update flag of linked edges
		for(int k=0;k<best_path.size();k++)
			edge_link_graph_.linked[best_path[k]->id]=1;
	}


	merge_non_jct_curve_frags(); // merge all the curve fragments, except at junctions
	vcl_cout << "extract non-junction curve fragments" << vcl_endl;

}


//: resolve paths conflict
void dbdet_sel_base::resolve_paths_conflict()
{
	vcl_vector<dbdet_edgel*> dummy_path;


	///////////////////////////////////////////////////////////////////////////////
	// 2. prune out FP paths and double links from junctions
	///////////////////////////////////////////////////////////////////////////////

	for (unsigned i=0; i<edgemap_->edgels.size(); i++)
	{
		int cur_eid = edgemap_->edgels[i]->id;

		if(curve_frag_graph_.pFrags[cur_eid].size() + curve_frag_graph_.cFrags[cur_eid].size() <3)
			continue;

		dbdet_edgel_chain_list cur_jct_paths;
		dbdet_edgel_chain_list paths2delete;
		dbdet_edgel_chain_list_iter clit1;
		for(clit1=curve_frag_graph_.pFrags[cur_eid].begin(); clit1!=curve_frag_graph_.pFrags[cur_eid].end(); clit1++)
		{
			int other_end_id = (*clit1)->edgels.front()->id;
			if(other_end_id == cur_eid) // skip circle
				continue;
			//if(compute_path_len((*clit1)->edgels)<5 && (*clit1)->edgels.size()<5 && curve_frag_graph_.pFrags[other_end_id].size() + curve_frag_graph_.cFrags[other_end_id].size()==1) // short free end path
				//paths2delete.push_back(*clit1);
			if((*clit1)->edgels.size()<=3 && curve_frag_graph_.cFrags[other_end_id].size() + curve_frag_graph_.pFrags[other_end_id].size()==1) // single free end link
				paths2delete.push_back(*clit1);
			else
				cur_jct_paths.push_back(*clit1);
		}
		for(clit1=curve_frag_graph_.cFrags[cur_eid].begin(); clit1!=curve_frag_graph_.cFrags[cur_eid].end(); clit1++)
		{
			int other_end_id = (*clit1)->edgels.back()->id;
			if(other_end_id == cur_eid) // skip circle
				continue;
			//if(compute_path_len((*clit1)->edgels)<5 && (*clit1)->edgels.size()<5 && curve_frag_graph_.pFrags[other_end_id].size() + curve_frag_graph_.cFrags[other_end_id].size()==1) // short free end path
				//paths2delete.push_back(*clit1);
			if((*clit1)->edgels.size()<=3 && curve_frag_graph_.cFrags[other_end_id].size() + curve_frag_graph_.pFrags[other_end_id].size()==1) // free end link
				paths2delete.push_back(*clit1);
			else
				cur_jct_paths.push_back(*clit1);
		}

		// delete those single free end links: This do not need to form forming TRUE T-junctions
		for (clit1=paths2delete.begin(); clit1!=paths2delete.end(); clit1++)
			curve_frag_graph_.extract_fragment(*clit1);

		// look for a pair of paths form a circle, remove the weaker one;
		dbdet_edgel_chain_list_iter clit2;
		for (clit1=cur_jct_paths.begin(); clit1!=cur_jct_paths.end(); clit1++)
		{

			int other_end_id1;
			vcl_vector<dbdet_edgel*> path1((*clit1)->edgels.begin(), (*clit1)->edgels.end());
			if((*clit1)->edgels.front()->id == cur_eid)
				other_end_id1 = (*clit1)->edgels.back()->id;
			else
			{
				vcl_reverse(path1.begin(), path1.end());
				other_end_id1 = (*clit1)->edgels.front()->id;
			}

			clit2=clit1;
			clit2++;
			for (;clit2!=cur_jct_paths.end(); clit2++)
			{
				int other_end_id2;
				vcl_vector<dbdet_edgel*> path2((*clit2)->edgels.begin(), (*clit2)->edgels.end());
				if((*clit2)->edgels.front()->id == cur_eid)
					other_end_id2 = (*clit2)->edgels.back()->id;
				else
				{
					vcl_reverse(path2.begin(), path2.end());
					other_end_id2 = (*clit2)->edgels.front()->id;
				}

				//found the double links, remover the shorter path
				if(other_end_id1==other_end_id2)
				{
					//vcl_cout << "remove double link other end: " << other_end_id1 << vcl_endl;
					int len1 = compute_path_len(path1);
					int len2 = compute_path_len(path2);

					double cost1 =compute_path_metric3(dummy_path, path1, dummy_path);
					double cost2 =compute_path_metric3(dummy_path, path2, dummy_path);
					if((cost1 > cost2 || (*clit1)->edgels.size()==2) && len1<5 && len2<5) // both short, remove the higher cost
					{
						curve_frag_graph_.extract_fragment(*clit1);
						break;
					}
					else if ((cost2 >= cost1 || (*clit2)->edgels.size()==2) && len1<5 && len2<5) // both short, remove the higher cost
					{
						curve_frag_graph_.extract_fragment(*clit2);
						continue;
					}
					// or keep the cycle
				}

				// if two paths overlap with each other, delete the weaker one
				if(is_overlapping(path1, path2))
				{
					int len1 = compute_path_len(path1);
					int len2 = compute_path_len(path2);

					double cost1 =compute_path_metric3(dummy_path, path1, dummy_path);
					double cost2 =compute_path_metric3(dummy_path, path2, dummy_path);
					if(cost1 > cost2 && len1<5 && len2<5) // both short, remove the higher cost
					{
						curve_frag_graph_.extract_fragment(*clit1);
						break;
					}
					else if (cost2 >= cost1 && len1<5 && len2<5) // both short, remove the higher cost
					{
						curve_frag_graph_.extract_fragment(*clit2);
						break;
					}
				}
			}
		}


	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	// 2. prune out trivial cross-over paths from ends of each curve fragment, only consider short curves
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	dbdet_edgel_chain_list_iter clit0 = curve_frag_graph_.frags.begin();
	while (clit0!=curve_frag_graph_.frags.end())
	{
		int eS_id = (*clit0)->edgels.front()->id;
		int eE_id = (*clit0)->edgels.back()->id;
		if(eS_id == eE_id) // skip circle
		{
			clit0 ++;
			continue;
		}

		if(curve_frag_graph_.pFrags[eS_id].size() + curve_frag_graph_.cFrags[eS_id].size()<2 ||
				curve_frag_graph_.pFrags[eE_id].size() + curve_frag_graph_.cFrags[eE_id].size()<2	)
		{
			clit0 ++;
			continue;
		}
		dbdet_edgel_chain_list jct_paths_eS;
		dbdet_edgel_chain_list jct_paths_eE;

		// construct start point paths list
		dbdet_edgel_chain_list_iter clit1;
		dbdet_edgel_chain_list_iter clit2;
		for(clit1=curve_frag_graph_.pFrags[eS_id].begin(); clit1!=curve_frag_graph_.pFrags[eS_id].end(); clit1++)
		{
			if((*clit1)!=(*clit0) && (*clit1)->edgels.size()<5)
				jct_paths_eS.push_back((*clit1));
		}
		for(clit1=curve_frag_graph_.cFrags[eS_id].begin(); clit1!=curve_frag_graph_.cFrags[eS_id].end(); clit1++)
		{
			if((*clit1)!=(*clit0) && (*clit1)->edgels.size()<5)
				jct_paths_eS.push_back((*clit1));
		}

		// construct end point paths list
		for(clit1=curve_frag_graph_.pFrags[eE_id].begin(); clit1!=curve_frag_graph_.pFrags[eE_id].end(); clit1++)
		{
			if((*clit1)!=(*clit0) && (*clit1)->edgels.size()<5)
				jct_paths_eE.push_back((*clit1));
		}
		for(clit1=curve_frag_graph_.cFrags[eE_id].begin(); clit1!=curve_frag_graph_.cFrags[eE_id].end(); clit1++)
		{
			if((*clit1)!=(*clit0) && (*clit1)->edgels.size()<5)
				jct_paths_eE.push_back((*clit1));
		}

		// if found any cross over, remove the weaker path
		for (clit1=jct_paths_eS.begin(); clit1!=jct_paths_eS.end(); clit1++)
		{
			for(clit2=jct_paths_eE.begin(); clit2!=jct_paths_eE.end(); clit2++)
			{
				if((*clit1)==(*clit2)) // form a circle
					continue;

				if(is_cross_over((*clit1), (*clit2)))
				{
					double cost1=compute_path_metric3((*clit1)->edgels);
					double cost2=compute_path_metric3((*clit2)->edgels);
					if(cost1>cost2)
					{
						curve_frag_graph_.extract_fragment(*clit1);
						//vcl_cout << "remove curve with eS:" << vvv1.front()->id << " eE:" << vvv1.back()->id << vcl_endl;
						break;
					}
					else
					{
						curve_frag_graph_.extract_fragment(*clit2);
						//vcl_cout << "remove curve with eS:" << vvv2.front()->id << " eE:" << vvv2.back()->id << vcl_endl;
						break;
					}
				}
			}
		}
		clit0 ++;
	}
	merge_non_jct_curve_frags(); // merge all the curve fragments, except at junctions
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	// 3. Prune out extremely short curve fragments at very certain false positive T-junction
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	for (unsigned i=0; i<edgemap_->edgels.size(); i++)
	{

	    dbdet_edgel* eA = edgemap_->edgels[i];
	    bool found_T = false;

	    int deg = curve_frag_graph_.pFrags[i].size()+ curve_frag_graph_.cFrags[i].size();
	    //degree 3 is a junction (T-junction or Y-junction)
	    if (deg>2) //  Make length of contour as the first priority
	    {
	        //goal is to see if any two will produce smooth continuation
	    	dbdet_edgel_chain_list node_frags;
			dbdet_edgel_chain_list_iter p_fit = curve_frag_graph_.pFrags[i].begin();
			for(;p_fit!=curve_frag_graph_.pFrags[i].end();p_fit++)
			{
				if((*p_fit)->edgels.front() == (*p_fit)->edgels.back()) // skip circle
					continue;
				node_frags.push_back(*p_fit);
			}

	        dbdet_edgel_chain_list_iter c_fit = curve_frag_graph_.cFrags[i].begin();
			for(;c_fit!=curve_frag_graph_.cFrags[i].end();c_fit++)
	        {
				if((*c_fit)->edgels.front() == (*c_fit)->edgels.back()) // skip circle
					continue;
	            node_frags.push_back(*c_fit);
	        }

			if(node_frags.size()<3) // for the case initial deg>2 is due to circle, do not consider 4 way junction either
			{
				for (dbdet_edgel_chain_list_iter fit_1=node_frags.begin(); fit_1!=node_frags.end(); fit_1++)
				{
					if(is_short_high_cost_curve_frag(*fit_1))
					{
						curve_frag_graph_.extract_fragment(*fit_1);
					}
				}
				continue;
			}

			//sort all the pfrags and cfrags in length
			node_frags.sort(is_longer);

		    dbdet_edgel_chain *c1;
		    dbdet_edgel_chain *c2;
	        //compare each pair to decide if it is a T-junction
	        dbdet_edgel_chain_list_iter fit_1=node_frags.begin();
			for (;fit_1!=--node_frags.end();)
			{
				c1= *fit_1;

				if(c1->edgels.back()!= eA)
				{
					curve_frag_graph_.extract_fragment(c1);
					vcl_reverse(c1->edgels.begin(), c1->edgels.end());
					curve_frag_graph_.insert_fragment(c1);
				}
				dbdet_edgel_chain_list_iter fit_2 = fit_1;
				fit_2 ++;
				dbdet_edgel_chain_list_iter max_fit = fit_1;
				double max_SM = 0;
				for (;fit_2!=node_frags.end();fit_2++)
				{
					c2=*fit_2;
					if(c2->edgels.back()== eA){
						curve_frag_graph_.extract_fragment(c2);
						vcl_reverse(c2->edgels.begin(), c2->edgels.end());
						curve_frag_graph_.insert_fragment(c2);
					}

					double SM_0 = get_max_continuity(c1,c2);
					if(SM_0>max_SM){
						max_SM = SM_0;
						max_fit = fit_2;
					}
				}
				if(max_SM>=0.9)
				{
					c2=*max_fit;
					found_T = true;
					//curve_frag_graph_.extract_fragment(c1);
					//curve_frag_graph_.extract_fragment(c2);
					//c1->append(c2->edgels);
					//curve_frag_graph_.insert_fragment(c1);
					break;
				}
				//if(fit_2!=node_frags.end())
				//    break;
				fit_1++;
			}


			// if the remaining path is short and high cost, remove it
			if(found_T)
			{
				for (fit_1=node_frags.begin(); fit_1!=node_frags.end(); fit_1++)
				{
					if((*fit_1)!=c1 && (*fit_1)!=c2 && is_short_high_cost_curve_frag(*fit_1))
					{
						//vcl_cout << "remove a short curve at jct: " << i << vcl_endl;
						curve_frag_graph_.extract_fragment(*fit_1);
					}
				}
			}

		}
	}
	//refresh_linked_condition();
	merge_non_jct_curve_frags(); // merge all the curve fragments, except at junctions



/*
	//////////////////////////////////////////////////////////////////
	// Resolve conflicting paths : IS not necessuary when there is NO FP T-junction anymore
	//////////////////////////////////////////////////////////////////

	dbdet_edgel_chain_list_iter clit0 = curve_frag_graph_.frags.begin();
	while (clit0!=curve_frag_graph_.frags.end())
	{
		dbdet_edgel_chain* cur_chain = *clit0;
		for (int j = 1; j<cur_chain->edgels.size()-1; j++)
		{
			// if there is a joining path
			if(curve_frag_graph_.pFrags[cur_chain->edgels[j]->id].size() + curve_frag_graph_.cFrags[cur_chain->edgels[j]->id].size() == 1)
			{
				dbdet_edgel_chain* joint_chain;
				//vcl_cout << "deal joint_chain at: " <<  cur_chain->edgels[j]->id << vcl_endl;

				// extract the joint path
				if(curve_frag_graph_.pFrags[cur_chain->edgels[j]->id].size() == 1)
				{
					joint_chain = curve_frag_graph_.pFrags[cur_chain->edgels[j]->id].front();
					if(cur_chain == joint_chain)
						clit0 --;
					curve_frag_graph_.extract_fragment(joint_chain);
				}
				else
				{
					//vcl_cout<< "a FP T-jct at: " << cur_chain->edgels[j]->id << " start from: "<< cur_chain->edgels.front()->id << " end at: "<< cur_chain->edgels.back()->id << vcl_endl;
					//this need to be considered, because after merging, some frags' order got reversed
					joint_chain = curve_frag_graph_.cFrags[cur_chain->edgels[j]->id].front();
					if(cur_chain == joint_chain)
					{
						clit0 --;
						// in this case, reverse will change j as well
						j = cur_chain->edgels.size()-1-j;
					}
					curve_frag_graph_.extract_fragment(joint_chain);
					vcl_reverse(joint_chain->edgels.begin(), joint_chain->edgels.end());
				}



				vcl_vector<dbdet_edgel*> sub_path_1;
				for (int k = 0; k<=j; k++)
					sub_path_1.push_back(cur_chain->edgels[k]);
				vcl_vector<dbdet_edgel*> sub_path_2;
				for (int k = j; k<cur_chain->edgels.size(); k++)
					sub_path_2.push_back(cur_chain->edgels[k]);

				//			  joint
				//				|
				//				|
				//      sub 1 	v   sub 2
				//	   -------> o ------->

				// CASE 1: if it is a single curve intersect itself, break it, remove the tail, OR if introduce new jct
				if(cur_chain == joint_chain)
				{
					//vcl_cout << "case 1" << vcl_endl;
					// cur_chain has already been extracted via extract_fragment(joint_chain)
					if(sub_path_1.front()==sub_path_1.back())
					{
						cur_chain->edgels.erase(cur_chain->edgels.begin()+j+1, cur_chain->edgels.end());
						curve_frag_graph_.insert_fragment(cur_chain);

						if(sub_path_2.size()>5) // if the tail is long, do not remove it
						{
							dbdet_edgel_chain* new_chain = new dbdet_edgel_chain();
							new_chain->append(sub_path_2);
							curve_frag_graph_.insert_fragment(new_chain);
						}
					}
					else
					{
						cur_chain->edgels.erase(cur_chain->edgels.begin(), cur_chain->edgels.begin()+j);
						curve_frag_graph_.insert_fragment(cur_chain);
						if(sub_path_1.size()>5) // if the tail is long, do not remove it
						{
							dbdet_edgel_chain* new_chain = new dbdet_edgel_chain();
							new_chain->append(sub_path_1);
							curve_frag_graph_.insert_fragment(new_chain);
						}
					}
					break;
				}

				///////////////////////////////////////////////////
				// identify the intersect situation
				///////////////////////////////////////////////////
				// a: check if the other end of joint_path also intersect at cur_chain
				int joint_idx2 = j;
				for (int j2=1; j2<cur_chain->edgels.size()-1; j2 ++)
				{
					if(joint_chain->edgels.back()->id == cur_chain->edgels[j2]->id)
					{
						joint_idx2 = j2;
						break;
					}
				}
				// b: check if start point of cur_chain intersect on joint_chain
				int start_on_joint_idx = joint_chain->edgels.size()-1;
				for (int j2=0; j2<joint_chain->edgels.size()-1; j2 ++)
				{
					if(cur_chain->edgels[0]->id == joint_chain->edgels[j2]->id)
					{
						start_on_joint_idx = j2;
						break;
					}
				}
				// c: check if end point of cur_chain intersect on joint_chain
				int end_on_joint_idx = joint_chain->edgels.size()-1;
				for (int j2=0; j2<joint_chain->edgels.size()-1; j2 ++)
				{
					if(cur_chain->edgels.back()->id == joint_chain->edgels[j2]->id)
					{
						end_on_joint_idx = j2;
						break;
					}
				}


				vcl_vector<dbdet_edgel*> joint_path(joint_chain->edgels.begin(), joint_chain->edgels.end());
				double cost_1 = compute_path_metric3(dummy_path, sub_path_1, dummy_path);
				double cost_2 = compute_path_metric3(dummy_path, sub_path_2, dummy_path);
				double cost_j = compute_path_metric3(dummy_path, joint_path, dummy_path);
				vcl_vector<dbdet_edgel*> joint_path_rev = joint_path;
				vcl_reverse(joint_path_rev.begin(), joint_path_rev.end());
				vcl_vector<dbdet_edgel*> sub_path_1_rev = sub_path_1;
				vcl_reverse(sub_path_1_rev.begin(), sub_path_1_rev.end());
				int sz1 = sub_path_1.size();
				int sz2 =sub_path_2.size();
				int szj = joint_chain->edgels.size();

				double dx_1o =  sub_path_1[sz1-1]->pt.x() - sub_path_1[sz1-2]->pt.x();
				double dy_1o =  sub_path_1[sz1-1]->pt.y() - sub_path_1[sz1-2]->pt.y();
				double dx_o2 = sub_path_2[1]->pt.x() - sub_path_2[0]->pt.x();
				double dy_o2 = sub_path_2[1]->pt.y() - sub_path_2[0]->pt.y();
				double cos_12 = (dx_1o*dx_o2+dy_1o*dy_o2)/vcl_sqrt(dx_1o*dx_1o+dy_1o*dy_1o)/vcl_sqrt(dx_o2*dx_o2+dy_o2*dy_o2);
				double cos_21 = cos_12;

				double dx_jo = joint_chain->edgels[szj-1]->pt.x() - joint_chain->edgels[szj-2]->pt.x();
				double dy_jo = joint_chain->edgels[szj-1]->pt.y() - joint_chain->edgels[szj-2]->pt.y();
				double cos_j2 = (dx_jo*dx_o2+dy_jo*dy_o2)/vcl_sqrt(dx_jo*dx_jo+dy_jo*dy_jo)/vcl_sqrt(dx_o2*dx_o2+dy_o2*dy_o2);
				double cos_j1 = (-dx_1o*dx_jo-dy_1o*dy_jo)/vcl_sqrt(dx_jo*dx_jo+dy_jo*dy_jo)/vcl_sqrt(dx_1o*dx_1o+dy_1o*dy_1o);



				// CASE 2: start point of cur_chain also intersect joint_chain
				if(start_on_joint_idx ==0 && end_on_joint_idx ==joint_chain->edgels.size()-1)
				{
					//vcl_cout << "case 2.1" << vcl_endl;
					if(cos_j1 > -0.5 && joint_chain->edgels.size()>3) // case to keep both, form a close circle
					{
						clit0 --;
						curve_frag_graph_.extract_fragment(cur_chain);
						cur_chain->edgels.erase(cur_chain->edgels.begin(), cur_chain->edgels.begin()+j-1);
						if(cur_chain->edgels.size()>2)
							curve_frag_graph_.insert_fragment(cur_chain);
						vcl_reverse(joint_chain->edgels.begin(), joint_chain->edgels.end());
						joint_chain->append(sub_path_1);
						curve_frag_graph_.insert_fragment(joint_chain);
						break;
					}
					else if(cost_j < cost_1 && sub_path_1.size()<5) // choose j over sub 1
					{
						clit0 --;
						curve_frag_graph_.extract_fragment(cur_chain);
						joint_chain->append(sub_path_2);
						curve_frag_graph_.insert_fragment(joint_chain);
						break;
					}

					// default is to remove joint_chain
				}
				// start point of cur_chain intersect in the middle joint_chain
				else if(start_on_joint_idx > 0 && start_on_joint_idx < joint_chain->edgels.size()-1)
				{
					//vcl_cout << "case 2.2" << vcl_endl;
					clit0 --;
					curve_frag_graph_.extract_fragment(cur_chain);
					joint_chain->edgels.erase(joint_chain->edgels.begin()+start_on_joint_idx, joint_chain->edgels.end());
					joint_chain->append(cur_chain->edgels);
					curve_frag_graph_.insert_fragment(joint_chain);
					break;
				}
				// CASE 3: end point of cur_chain also intersect joint_chain
				else if(end_on_joint_idx ==0 && start_on_joint_idx ==joint_chain->edgels.size()-1)
				{
					//vcl_cout << "case 3.1" << vcl_endl;

					if(cos_j2 > -0.5 && joint_chain->edgels.size()>3) // case to keep both, form a close circle
					{
						clit0 --;
						curve_frag_graph_.extract_fragment(cur_chain);
						cur_chain->edgels.erase(cur_chain->edgels.begin()+j+1, cur_chain->edgels.end());
						if(cur_chain->edgels.size()>2)
							curve_frag_graph_.insert_fragment(cur_chain);
						dbdet_edgel_chain* new_chain = new dbdet_edgel_chain();
						new_chain->append(sub_path_2);
						new_chain->append(joint_chain->edgels);
						curve_frag_graph_.insert_fragment(new_chain);
						break;
					}
					else if(cost_j < cost_2 && sub_path_2.size()<5) // choose j over sub 2
					{
						clit0 --;
						curve_frag_graph_.extract_fragment(cur_chain);
						vcl_reverse(sub_path_1.begin(), sub_path_1.end());
						joint_chain->append(sub_path_1);
						curve_frag_graph_.insert_fragment(joint_chain);
						break;
					}
					// default is to remove joint_chain
				}
				// end point of cur_chain intersect in the middle joint_chain
				else if(end_on_joint_idx > 0 && end_on_joint_idx < joint_chain->edgels.size()-1)
				{
					//vcl_cout << "case 3.2" << vcl_endl;
					clit0 --;
					curve_frag_graph_.extract_fragment(cur_chain);
					vcl_reverse(cur_chain->edgels.begin(), cur_chain->edgels.end()); // reverse the whole cur_chain
					joint_chain->edgels.erase(joint_chain->edgels.begin()+end_on_joint_idx, joint_chain->edgels.end());
					joint_chain->append(cur_chain->edgels);
					curve_frag_graph_.insert_fragment(joint_chain);
					break;
				}
				// CASE 4: both ends of joint chain intersect cur_chain, joint_idx2 should > j
				else if(joint_idx2>j)
				{
					//vcl_cout << "case 4" << vcl_endl;
					//if((cos_j2 < -0.5 && joint_chain->edgels.size()<5))
					// default is to remove joint_chain
					break;
				}
				// CASE 5: if sub 1 and joint very close, but not intersect again
				else if(is_overlapping(joint_path_rev, sub_path_1_rev))
				{
					//vcl_cout << "case 5" << vcl_endl;
					if(sz1 < szj && sz1<10 && cost_j < cost_1 )
					{
						// choose j over sub 1
						clit0 --;
						curve_frag_graph_.extract_fragment(cur_chain);
						joint_chain->append(sub_path_2);
						curve_frag_graph_.insert_fragment(joint_chain);
						break;
					}
					else if(szj > 10 && sz1 > 10) // case to keep both,
					{
						curve_frag_graph_.insert_fragment(joint_chain);
						// break cur_chain to form a true junction
						clit0 --;
						curve_frag_graph_.extract_fragment(cur_chain);
						cur_chain->edgels.erase(cur_chain->edgels.begin()+j+1, cur_chain->edgels.end());
						curve_frag_graph_.insert_fragment(cur_chain);
						dbdet_edgel_chain* new_chain = new dbdet_edgel_chain();
						new_chain->append(sub_path_2);
						curve_frag_graph_.insert_fragment(new_chain);
						break;
					}
					// default is to remove joint_chain
				}
				// CASE 6: if sub 2 and joint very close, but not intersect again
				else if(is_overlapping(joint_path_rev, sub_path_2))
				{
					//vcl_cout << "case 6" << vcl_endl;
					if(sz2<szj && sz2<10 && cost_j < cost_2 )
					{
						clit0 --;
						curve_frag_graph_.extract_fragment(cur_chain);
						vcl_reverse(sub_path_1.begin(), sub_path_1.end());
						joint_chain->append(sub_path_1);
						curve_frag_graph_.insert_fragment(joint_chain);
						break;
					}
					else if(szj > 10 && sz2 >10) // case to keep both,
					{
						curve_frag_graph_.insert_fragment(joint_chain);
						// break cur_chain to form a true junction
						clit0 --;
						curve_frag_graph_.extract_fragment(cur_chain);
						cur_chain->edgels.erase(cur_chain->edgels.begin()+j+1, cur_chain->edgels.end());
						curve_frag_graph_.insert_fragment(cur_chain);
						dbdet_edgel_chain* new_chain = new dbdet_edgel_chain();
						new_chain->append(sub_path_2);
						curve_frag_graph_.insert_fragment(new_chain);
						break;
					}
					// default is to remove joint_chain
				}
				// in other cases, we consider a true T-junction formed:
				// theoretically, previous stages have not considered junctions in the middle of unambiguous fragments
				else if(szj > 5)
				{
					//vcl_cout << "case 7" << vcl_endl;
					curve_frag_graph_.insert_fragment(joint_chain);
					// break cur_chain to form a true junction
					clit0 --;
					curve_frag_graph_.extract_fragment(cur_chain);
					cur_chain->edgels.erase(cur_chain->edgels.begin()+j+1, cur_chain->edgels.end());
					curve_frag_graph_.insert_fragment(cur_chain);
					dbdet_edgel_chain* new_chain = new dbdet_edgel_chain();
					new_chain->append(sub_path_2);
					curve_frag_graph_.insert_fragment(new_chain);
					break;
					// default is to remove joint_chain
				}
			}
		}
		clit0 ++;
	}

	merge_non_jct_curve_frags(); // merge all the curve fragments, except at junctions
*/
	vcl_cout << "resolve paths conflict" << vcl_endl;

}


//: perform a geometric consistency check to determine whether a given temp path is valid
bool dbdet_sel_base::is_JCT_path_legal(vcl_vector<dbdet_edgel*>& edgel_chain)
{
  //what makes a path legal?
  if(edgel_chain.size()==1)
	  return false;

  else if(edgel_chain.size()==2)   //remove direct link of two edges over 5 pixels
  {
      dbdet_edgel* eS = edgel_chain.front();
      dbdet_edgel* eE = edgel_chain.back();
      double dx1 = eE->pt.x() - eS->pt.x();
      double dy1 = eE->pt.y() - eS->pt.y();
      double dist= vcl_sqrt(dx1*dx1+dy1*dy1);
      if(dist>3)
         return false;
   }
  /*else // the last step should not too far
  {
	  int sz = edgel_chain.size();
	  dbdet_edgel* eS = edgel_chain[sz-2];
	  dbdet_edgel* eE = edgel_chain.back();
	  double dx1 = eE->pt.x() - eS->pt.x();
	  double dy1 = eE->pt.y() - eS->pt.y();
	  double dist= vcl_sqrt(dx1*dx1+dy1*dy1);
	  if(dist>3)
	    return false;
  }*/

  // by Yuliang, construct two lists of end nodes from other Curve Fragments end points linking the end points of the path
  vcl_vector<dbdet_edgel*> S_link_end_nodes, E_link_end_nodes;

  // (a) if a c1 polyarc bundle can form within it
  // (b) if it is c1 compatible with the end points

  //For now, just check for...

  //1) continuity consistency at the end points
  //1a) at the start point
  dbdet_edgel* eS = edgel_chain.front();
  dbdet_edgel* e2 = edgel_chain[1];
  double dx1 = e2->pt.x() - eS->pt.x();
  double dy1 = e2->pt.y() - eS->pt.y();
  bool cons = true;


  //Is the starting point consistent with at least one of the connecting unambiguous chains? : angel < 90 degree
  //the starting point should not be too close to any of the connecting unambiguous chains, in direction. : angel should not be in (150, 210) degree
  dbdet_edgel_chain_list_iter pcit = curve_frag_graph_.pFrags[eS->id].begin();
  for ( ; pcit != curve_frag_graph_.pFrags[eS->id].end(); pcit++)
  {
    dbdet_edgel* pe = (*pcit)->edgels[(*pcit)->edgels.size()-2];
    S_link_end_nodes.push_back((*pcit)->edgels.front());

    //make a simple consistency check for child Frags
    double dx2 = eS->pt.x() - pe->pt.x();
    double dy2 = eS->pt.y() - pe->pt.y();

    cons = cons && ((dx1*dx2 + dy1*dy2)/vcl_sqrt(dx1*dx1+dy1*dy1)/vcl_sqrt(dx2*dx2+dy2*dy2))>-0.9;
  }
  dbdet_edgel_chain_list_iter ccit = curve_frag_graph_.cFrags[eS->id].begin();
  for ( ; ccit != curve_frag_graph_.cFrags[eS->id].end(); ccit++)
  {
    dbdet_edgel* ce = (*ccit)->edgels[1];
    S_link_end_nodes.push_back((*ccit)->edgels.back());

    //make a simple consistency check for parent Frags
    double dx2 = eS->pt.x() - ce->pt.x();
    double dy2 = eS->pt.y() - ce->pt.y();

    cons = cons && ((dx1*dx2 + dy1*dy2)/vcl_sqrt(dx1*dx1+dy1*dy1)/vcl_sqrt(dx2*dx2+dy2*dy2))>-0.9;
  }
  if (!cons) return false; //no good at the start point, and not long enough

  //1b) When the end of the path reach an unambiguous chain
  dbdet_edgel* eE = edgel_chain.back();
  if(curve_frag_graph_.pFrags[eE->id].size() + curve_frag_graph_.cFrags[eE->id].size()>=1)
  {
	  e2 = edgel_chain[edgel_chain.size()-2];
	  dx1 = eE->pt.x() - e2->pt.x();
	  dy1 = eE->pt.y() - e2->pt.y();
	  cons = true;
	  //the ending point should not be too close to any of the connecting unambiguous chains, in direction. : angel should not be in (150, 210) degree
	  pcit = curve_frag_graph_.pFrags[eE->id].begin();
	  for ( ; pcit != curve_frag_graph_.pFrags[eE->id].end(); pcit++)
	  {
		dbdet_edgel* pe = (*pcit)->edgels[(*pcit)->edgels.size()-2];
		E_link_end_nodes.push_back((*pcit)->edgels.front());

		//make a simple consistency check
		double dx2 = pe->pt.x() - eE->pt.x();
		double dy2 = pe->pt.y() - eE->pt.y();

		double cos_angle = (dx1*dx2 + dy1*dy2)/vcl_sqrt(dx1*dx1+dy1*dy1)/vcl_sqrt(dx2*dx2+dy2*dy2);
		cons = cons && (cos_angle > -0.9);
	  }
	  ccit = curve_frag_graph_.cFrags[eE->id].begin();
	  for ( ; ccit != curve_frag_graph_.cFrags[eE->id].end(); ccit++)
	  {
		dbdet_edgel* ce = (*ccit)->edgels[1];
		E_link_end_nodes.push_back((*ccit)->edgels.back());

		//make a simple consistency check
		double dx2 = ce->pt.x() - eE->pt.x();
		double dy2 = ce->pt.y() - eE->pt.y();

		double cos_angle = (dx1*dx2 + dy1*dy2)/vcl_sqrt(dx1*dx1+dy1*dy1)/vcl_sqrt(dx2*dx2+dy2*dy2);
		cons = cons && (cos_angle > -0.9);
	  }
	  if (!cons) return false; //no good at the end point


	  //2) use the two lists, check if it is a path between which unambiguous contours linked,
	  // this is only necessuary if both ends are at end points
	  for(int i =0; i<S_link_end_nodes.size(); i++)
	  {
		dbdet_edgel* S_l_e = S_link_end_nodes[i];
		if(S_l_e == eE)
			return false;
		for(int j=0; j<E_link_end_nodes.size(); j++)
		{
			dbdet_edgel* E_l_e = E_link_end_nodes[j];
			if(E_l_e == eS)
				return false;
			// the case two connected contours filling the path
			if(E_l_e== S_l_e)
				return false;
		}
	  }

  }

  // comment by Yuliang, in most cases, it is no need, because of short paths.
  //fit_polyarc_to_chain(&edgel_chain);
  return true;
}

////////////////////////////////////////////////////////////////////////////////
//extract the set of non-overlapping set of attached paths, count the number
//prune the set of attached paths to be non-overlapping with each other
//the case that a junction locates at an end point is included
////////////////////////////////////////////////////////////////////////////////
bool dbdet_sel_base::is_overlapping(vcl_vector<dbdet_edgel*> edges_1, vcl_vector<dbdet_edgel*> edges_2)
{
	int nbr_limit = 6;
	int sz1 = (edges_1.size()<nbr_limit)?edges_1.size():nbr_limit;
	int sz2 = (edges_2.size()<nbr_limit)?edges_2.size():nbr_limit;

/*
	double min_sum_dist = 0;
	int num_very_close = 0;
	int num_overlapping = 0;
	int k=0,l=0;
	// because all the attached paths has the same end at edgel_id, only need to check the overlapping in the last portion.
	while (k< sz1 && l < sz2) {
		if(edges_1[k]->id == edges_2[l]->id)
		{
			num_overlapping ++;
			num_very_close ++;
			k++;
			l++;
		}
		else if(k+1 < edges_1.size() && vgl_distance(edges_1[k]->pt, edges_2[l]->pt) > vgl_distance(edges_1[k+1]->pt, edges_2[l]->pt)  )
			k++;
		else if(l+1 < edges_2.size() && vgl_distance(edges_1[k]->pt, edges_2[l]->pt) > vgl_distance(edges_1[k]->pt, edges_2[l+1]->pt))
			l++;
		else
		{
			double dist = vgl_distance(edges_1[k]->pt, edges_2[l]->pt) + vcl_abs(edges_1[k]->tangent - edges_2[l]->tangent);
			min_sum_dist += dist;
			if(dist<1.5) // for Non-Subpixel edges, this is the same as the overlapping
				num_very_close ++;
			k++;
			l++;
		}
	}

	if(num_overlapping >=2)
		return true;

	if(num_very_close >=3) // include some cross over case
		return true;*/

	// check the initial directions difference: within a neighborhood, whenever it is smaller than 30 degree
	//int last_ind1 = (sz1<nbr_limit2)?(sz1-1):(nbr_limit2-1);
	//int last_ind2 = (sz2<nbr_limit2)?(sz2-1):(nbr_limit2-1);
	int last_ind1 = sz1-1;
	int last_ind2 = sz2-1;
	for (int i = 1; i<=last_ind1; i++)
	{
		for (int j = 1; j<=last_ind2; j++)
		{
		// check if the starting directions of two paths are super close
		double dx1 = edges_1[i]->pt.x() - edges_1[0]->pt.x();
		double dy1 = edges_1[i]->pt.y() - edges_1[0]->pt.y();
		double dx2 = edges_2[j]->pt.x() - edges_2[0]->pt.x();
		double dy2 = edges_2[j]->pt.y() - edges_2[0]->pt.y();

		if(((dx1*dx2 + dy1*dy2)/vcl_sqrt(dx1*dx1+dy1*dy1)/vcl_sqrt(dx2*dx2+dy2*dy2))>0.9)
			return true;
		//if(((dx1*dx2 + dy1*dy2)/vcl_sqrt(dx1*dx1+dy1*dy1)/vcl_sqrt(dx2*dx2+dy2*dy2))<0.7)
			//return false;
		}
	}


	return false;
}

int dbdet_sel_base::check_a_junction(int edgel_id)
{

	int num_non_overlap_paths = 0;
	int deg = curve_frag_graph_.pFrags[edgel_id].size() + curve_frag_graph_.cFrags[edgel_id].size();
	// if current edgel is at an end point, check how attached curves are overlapping with connected unambiguous curve fragment
	num_non_overlap_paths += deg;


	dbdet_edgel_chain_list_iter clit1, clit2;
	clit1= curve_frag_graph_.HypFrags[edgel_id].begin();
	////////////////////////////////////////////////////////////
	//  remove overlapping attached paths
	////////////////////////////////////////////////////////////
	while(  clit1!=curve_frag_graph_.HypFrags[edgel_id].end())
	{
		dbdet_edgel_chain* path_1 = *clit1;

		// only allows single link in the jct at the end points of unambiguous fragment
		// This is VERY IMPORTANT to prune many false positives
		if(deg==0 && path_1->edgels.size()==2 && compute_path_len(path_1->edgels)>2)
		{
			clit1 = curve_frag_graph_.HypFrags[edgel_id].erase(clit1);
			continue;
		}

		vcl_vector<dbdet_edgel*> edges_1(path_1->edgels.begin(), path_1->edgels.end());
		vcl_reverse(edges_1.begin(), edges_1.end());
		// if the path is extremely short, consider the its connected unambiguous cfrag
		int last_eid = edges_1.back()->id;
		if(edges_1.size()<=4 && curve_frag_graph_.cFrags[last_eid].size()+curve_frag_graph_.pFrags[last_eid].size()==1)
		{
			if(curve_frag_graph_.cFrags[last_eid].size()==1)
			{
				dbdet_edgel_chain* path_1_extend = curve_frag_graph_.cFrags[last_eid].front();
				for(int i = 1; i<path_1_extend->edgels.size(); i++)
					edges_1.push_back( path_1_extend->edgels[i]);
			}
			if(curve_frag_graph_.pFrags[last_eid].size()==1)
			{
				dbdet_edgel_chain* path_1_extend = curve_frag_graph_.pFrags[last_eid].front();
				for(int i = 1; i<path_1_extend->edgels.size(); i++)
					edges_1.push_back( path_1_extend->edgels[path_1_extend->edgels.size()-1-i]);
			}
		}

		////////////////////////////////////////////////////////////////////////////////
		// if there is connected unambiguous frag, remove the attached curve overlapping with it
		////////////////////////////////////////////////////////////////////////////////
		bool found_overlapping_cfrag = false;
	    dbdet_edgel_chain_list_iter pcit = curve_frag_graph_.pFrags[edgel_id].begin();
	    for ( ; pcit != curve_frag_graph_.pFrags[edgel_id].end(); pcit++)
	    {
	        vcl_vector<dbdet_edgel*> connected_frag;
	    	int sz = (*pcit)->edgels.size();
	    	for(int i=0; i< sz; i++) // just push two edges in
	    		connected_frag.push_back((*pcit)->edgels[sz-1-i]);

			// for the case connected_frag is too short, extend it
			int last_eid2 = connected_frag.back()->id;
			if(connected_frag.size()<=4 && curve_frag_graph_.cFrags[last_eid2].size()+curve_frag_graph_.pFrags[last_eid2].size()==2)
			{
				dbdet_edgel_chain_list_iter ccit2 = curve_frag_graph_.cFrags[last_eid2].begin();
				for (; ccit2 != curve_frag_graph_.cFrags[last_eid2].end(); ccit2++)
				{
					if((*ccit2) == (*pcit))
						continue;
					for(int i = 1; i<(*ccit2)->edgels.size(); i++)
						connected_frag.push_back( (*ccit2)->edgels[i]);
				}
				for (ccit2 = curve_frag_graph_.pFrags[last_eid2].begin(); ccit2 != curve_frag_graph_.pFrags[last_eid2].end(); ccit2++)
				{
					if((*ccit2) == (*pcit))
						continue;
					for(int i = 1; i<(*ccit2)->edgels.size(); i++)
						connected_frag.push_back( (*ccit2)->edgels[(*ccit2)->edgels.size()-1-i]);
				}
			}

			if(is_overlapping(edges_1, connected_frag))
			{
				found_overlapping_cfrag = true;
				break;
			}

	    }
	    dbdet_edgel_chain_list_iter ccit = curve_frag_graph_.cFrags[edgel_id].begin();
	    for ( ; ccit != curve_frag_graph_.cFrags[edgel_id].end(); ccit++)
	    {
	        vcl_vector<dbdet_edgel*> connected_frag;
	        int sz = (*ccit)->edgels.size();
	    	for(int i=0; i< sz; i++) // just push two edges in
	    		connected_frag.push_back((*ccit)->edgels[i]);

			// for the case connected_frag is too short, extend it
			int last_eid2 = connected_frag.back()->id;
			if(connected_frag.size()<=4 && curve_frag_graph_.cFrags[last_eid2].size()+curve_frag_graph_.pFrags[last_eid2].size()==2)
			{
				dbdet_edgel_chain_list_iter ccit2 = curve_frag_graph_.cFrags[last_eid2].begin();
				for (; ccit2 != curve_frag_graph_.cFrags[last_eid2].end(); ccit2++)
				{
					if((*ccit2) == (*pcit))
						continue;
					for(int i = 1; i<(*ccit2)->edgels.size(); i++)
						connected_frag.push_back( (*ccit2)->edgels[i]);
				}
				for (ccit2 = curve_frag_graph_.pFrags[last_eid2].begin(); ccit2 != curve_frag_graph_.pFrags[last_eid2].end(); ccit2++)
				{
					if((*ccit2) == (*pcit))
						continue;
					for(int i = 1; i<(*ccit2)->edgels.size(); i++)
						connected_frag.push_back( (*ccit2)->edgels[(*ccit2)->edgels.size()-1-i]);
				}
			}

			if(is_overlapping(edges_1, connected_frag))
			{
				found_overlapping_cfrag = true;
				break;
			}
	    }
	    if(found_overlapping_cfrag)
	    {
			clit1 = curve_frag_graph_.HypFrags[edgel_id].erase(clit1);
			continue;
	    }
		////////////////////////////////////////////////////////////////////////////////
		// locate overlapping attached paths
		////////////////////////////////////////////////////////////////////////////////
		clit2 = clit1;
		clit2 ++;
		bool remove_path_1 = false;
		while (clit2!=curve_frag_graph_.HypFrags[edgel_id].end())
		{
			dbdet_edgel_chain* path_2 = *clit2;
		    // Check if the two paths connect to the two ends of a single curve fragment
		    dbdet_edgel* eS1 = path_1->edgels.front();
		    dbdet_edgel* eS2 = path_2->edgels.front();
		    bool connect2onefrag = false;
		    if(curve_frag_graph_.cFrags[eS1->id].size()==1)
		    {
		    	connect2onefrag |= (curve_frag_graph_.cFrags[eS1->id].front()->edgels.back() == eS2);
		    }
		    if(curve_frag_graph_.pFrags[eS1->id].size()==1)
		    {
		    	connect2onefrag |= (curve_frag_graph_.pFrags[eS1->id].front()->edgels.front() == eS2);
		    }

		    // check if the two paths are overlapping
			vcl_vector<dbdet_edgel*> edges_2;
			for (int i = 0; i< path_2->edgels.size(); i++)
			{
				edges_2.push_back(path_2->edgels[path_2->edgels.size()-1-i]);
			}
			// if the path is extremely short, consider the its connected unambiguous cfrag
			int last_eid = edges_2.back()->id;
			if(edges_2.size()<=3 && curve_frag_graph_.cFrags[last_eid].size()+curve_frag_graph_.pFrags[last_eid].size()==1)
			{
				if(curve_frag_graph_.cFrags[last_eid].size()==1)
				{
					dbdet_edgel_chain* path_2_extend = curve_frag_graph_.cFrags[last_eid].front();
					for(int i = 1; i<path_2_extend->edgels.size(); i++)
						edges_2.push_back( path_2_extend->edgels[i]);
				}
				if(curve_frag_graph_.pFrags[last_eid].size()==1)
				{
					dbdet_edgel_chain* path_2_extend = curve_frag_graph_.pFrags[last_eid].front();
					for(int i = 1; i<path_2_extend->edgels.size(); i++)
						edges_2.push_back( path_2_extend->edgels[path_2_extend->edgels.size()-1-i]);
				}
			}
			// If the two paths are overlapping, remove the worse one
			if(is_overlapping(edges_1, edges_2) || connect2onefrag)
			{
				if(compute_path_len(path_1->edgels) < compute_path_len(path_2->edgels))
				{
					clit2 = curve_frag_graph_.HypFrags[edgel_id].erase(clit2); // clit will move to the next
				}
				else
				{
					remove_path_1 = true;
					clit2 ++;
				}
			}
			else
				clit2 ++;
		}
		if(remove_path_1) // this avoid removing path_1 more than once
		{
			clit1 = curve_frag_graph_.HypFrags[edgel_id].erase(clit1); // clit will move to the next
		}
		else
			clit1 ++;

	}

	num_non_overlap_paths += curve_frag_graph_.HypFrags[edgel_id].size();

	return num_non_overlap_paths;

}

//: Topological non maximum suppression of junction candidates
void dbdet_sel_base::topologial_NMS(int edgel_id)
{
	int jct_nbr_limit = 5;
	vcl_vector<dbdet_edgel*> dummy_chain;
	vcl_set<int> end_ids_0;
	if (curve_frag_graph_.pFrags[edgel_id].size() + curve_frag_graph_.cFrags[edgel_id].size() >= 1) // jct and end point
		 end_ids_0.insert(edgel_id);
	// compute the cost of center junction
	double best_cost = 0;
	int total_sz = 0;
	int best_eid = edgel_id;
	dbdet_edgel_chain_list_iter clit0;
	for( clit0= curve_frag_graph_.HypFrags[edgel_id].begin(); clit0!=curve_frag_graph_.HypFrags[edgel_id].end(); clit0++)
	{
		int path_sz = (*clit0)->edgels.size();
		if(path_sz>jct_nbr_limit)
			path_sz = jct_nbr_limit;
		vcl_vector<dbdet_edgel*> vvv((*clit0)->edgels.end()-path_sz, (*clit0)->edgels.end());
		best_cost += (compute_path_metric3(dummy_chain, vvv, dummy_chain)*(vvv.size()-1));
		total_sz += (vvv.size()-1);
		end_ids_0.insert((*clit0)->edgels.front()->id);
	}
	//best_cost /= curve_frag_graph_.HypFrags[edgel_id].size();
	best_cost /= total_sz;
	// if there is a directly connected junction that is better the the center junction, suppress the center junction
	for( clit0= curve_frag_graph_.HypFrags[edgel_id].begin(); clit0!=curve_frag_graph_.HypFrags[edgel_id].end(); clit0++)
    {
		dbdet_edgel_list_iter eit = (*clit0)->edgels.end();	eit --;	eit --;
		int connect_id = (*eit)->id;

    	if(curve_frag_graph_.junction_edgels[connect_id])
    	{
    		vcl_set<int> end_ids_1;
    		if (curve_frag_graph_.pFrags[connect_id].size() + curve_frag_graph_.cFrags[connect_id].size() >= 1) // jct and end point
    			 end_ids_1.insert(connect_id);
    		dbdet_edgel_chain_list_iter clit1;
    		for( clit1= curve_frag_graph_.HypFrags[connect_id].begin(); clit1!=curve_frag_graph_.HypFrags[connect_id].end(); clit1++)
    			end_ids_1.insert((*clit1)->edgels.front()->id);

    		//if(end_ids_0 == end_ids_1) // only neighbors sharing the same set of nodes are to suppress
    		//{
				double cur_cost = 0;
				int cur_total_sz = 0;
				//dbdet_edgel_chain_list_iter clit1;
				for( clit1= curve_frag_graph_.HypFrags[connect_id].begin(); clit1!=curve_frag_graph_.HypFrags[connect_id].end(); clit1++)
				{
					int path_sz = (*clit1)->edgels.size();
					if(path_sz>jct_nbr_limit)
						path_sz = jct_nbr_limit;
					vcl_vector<dbdet_edgel*> vvv((*clit1)->edgels.end()-path_sz, (*clit1)->edgels.end());
					cur_cost += (compute_path_metric3(dummy_chain, vvv, dummy_chain)*(vvv.size()-1));
					cur_total_sz += (vvv.size()-1);
				}
				//cur_cost /= curve_frag_graph_.HypFrags[connect_id].size();
				cur_cost /= cur_total_sz;
				if(cur_cost <= best_cost)
				{
					curve_frag_graph_.junction_edgels[best_eid] = 0;
					best_cost =  cur_cost;
					best_eid = connect_id;
				}
				else
					curve_frag_graph_.junction_edgels[connect_id] = 0;
    		//}

    	}
    }
}

//: Enforce the constraint: there is only one junction connecting a unique set of end points
void dbdet_sel_base::check_jct_compatibility (int edgel_id)
{
	int jct_nbr_limit = 5;
	vcl_vector<dbdet_edgel*> dummy_chain;
	vcl_set<int> end_ids_0;
	if (curve_frag_graph_.pFrags[edgel_id].size() + curve_frag_graph_.cFrags[edgel_id].size() >= 1) // jct and end point
		 end_ids_0.insert(edgel_id);
	// compute the cost of center junction
	double best_cost = 0;
	int total_sz = 0;
	int best_id = edgel_id;
	dbdet_edgel_chain_list_iter clit0;
	for( clit0= curve_frag_graph_.HypFrags[edgel_id].begin(); clit0!=curve_frag_graph_.HypFrags[edgel_id].end(); clit0++)
	{
		int path_sz = (*clit0)->edgels.size();
		if(path_sz>jct_nbr_limit)
			path_sz = jct_nbr_limit;
		vcl_vector<dbdet_edgel*> vvv((*clit0)->edgels.end()-path_sz, (*clit0)->edgels.end());
		best_cost += (compute_path_metric3(dummy_chain, vvv, dummy_chain)*(vvv.size()-1));
		total_sz += (vvv.size()-1);
		end_ids_0.insert((*clit0)->edgels.front()->id);
	}
	best_cost /= total_sz;
	// go through all the junctions, suppress those having the same connecting end points, but with higher cost
	// TODO: this is not the most efficient way in implementation. Assume there are not many junctions found
	for (int i=0; i<curve_frag_graph_.junction_edgels.size(); i++)
	{
		if(!curve_frag_graph_.junction_edgels[i] || i==edgel_id)
			continue;

		vcl_set<int> end_ids_1;
		if (curve_frag_graph_.pFrags[i].size() + curve_frag_graph_.cFrags[i].size() >= 1) // jct and end point
			 end_ids_1.insert(i);
		dbdet_edgel_chain_list_iter clit1;
		for( clit1= curve_frag_graph_.HypFrags[i].begin(); clit1!=curve_frag_graph_.HypFrags[i].end(); clit1++)
			end_ids_1.insert((*clit1)->edgels.front()->id);

		if(end_ids_0 == end_ids_1) // found a junction have the same set of connecting end points
		{
			double cur_cost = 0;
			int cur_total_sz=0;
			for( clit1= curve_frag_graph_.HypFrags[i].begin(); clit1!=curve_frag_graph_.HypFrags[i].end(); clit1++)
			{
				int path_sz = (*clit1)->edgels.size();
				if(path_sz>jct_nbr_limit)
					path_sz = jct_nbr_limit;
				vcl_vector<dbdet_edgel*> vvv((*clit1)->edgels.end()-path_sz, (*clit1)->edgels.end());
				cur_cost += (compute_path_metric3(dummy_chain, vvv, dummy_chain)*(vvv.size()-1));
				cur_total_sz += (vvv.size()-1);
			}
			cur_cost/=cur_total_sz;

			if(cur_cost < best_cost)
			{
				curve_frag_graph_.junction_edgels[best_id] = 0;
				best_id = i;
				best_cost = cur_cost;
			}
		}
	}

}

//:   //: For each curve fragment reaching out from a junction,  cut it when it reach a existing curve fragment

void dbdet_sel_base::cut_jct_curve_fragment(int edgel_id)
{
	vcl_vector<dbdet_edgel*> dummy_chain;
	edge_link_graph_.linked[edgel_id] = 1;
	dbdet_edgel_chain_list_iter clit0= curve_frag_graph_.HypFrags[edgel_id].begin();
	for( ; clit0!=curve_frag_graph_.HypFrags[edgel_id].end(); clit0++)
    {
		// from the closest edge to jct with edgel_id to the farthest edge
		for (int i = (*clit0)->edgels.size()-2; i>=0; i--)
		{
			int connect_id = (*clit0)->edgels[i]->id;
			if(curve_frag_graph_.junction_edgels[connect_id] || edge_link_graph_.linked[connect_id]) // if reach a junction edge, or linked edge, cut the path to here
			{
				(*clit0)->edgels.erase((*clit0)->edgels.begin(), (*clit0)->edgels.begin()+i);
				vcl_vector<dbdet_edgel*> curve_0((*clit0)->edgels.begin(), (*clit0)->edgels.end());
				vcl_reverse(curve_0.begin(), curve_0.end()); // make curve_0 point out from edgel_id
				///////////////////////////////////////////////////////////////////////////////////
				// check if *clit0 overlaps with any existing curve fragment to edgel_id
				bool found_overlapping = false;
				dbdet_edgel_chain_list_iter clit1;
				for(clit1=curve_frag_graph_.pFrags[edgel_id].begin(); clit1!=curve_frag_graph_.pFrags[edgel_id].end(); clit1++)
				{
					vcl_vector<dbdet_edgel*> curve_1((*clit1)->edgels.begin(), (*clit1)->edgels.end());
					vcl_reverse(curve_1.begin(), curve_1.end());
					// make sure curve_0 curve_1 both point out from edgel_id
					if(is_overlapping(curve_0, curve_1))
					{
						found_overlapping = true;
						break;
					}
				}
				for(clit1=curve_frag_graph_.cFrags[edgel_id].begin(); clit1!=curve_frag_graph_.cFrags[edgel_id].end(); clit1++)
				{
					vcl_vector<dbdet_edgel*> curve_1((*clit1)->edgels.begin(), (*clit1)->edgels.end());
					// make sure curve_0 curve_1 both point out from edgel_id
					if(is_overlapping(curve_0, curve_1))
					{
						found_overlapping = true;
						break;
					}
				}
				// if it is overlapping, remove the path
				if(found_overlapping)
				{
					curve_frag_graph_.HypFrags[edgel_id].remove(*clit0);
					clit0--;
				}
				/*
				// THIS STEP would remove a lot possible choices
				///////////////////////////////////////////////////////////////////////////////////
				// check if *clit0 overlaps any existing curve fragment to connect_id
				found_overlapping = false;
				vcl_reverse(curve_0.begin(), curve_0.end()); // make curve_0 point out from connect_id
				for(clit1=curve_frag_graph_.pFrags[connect_id].begin(); clit1!=curve_frag_graph_.pFrags[connect_id].end(); clit1++)
				{
					vcl_vector<dbdet_edgel*> curve_1((*clit1)->edgels.begin(), (*clit1)->edgels.end());
					vcl_reverse(curve_1.begin(), curve_1.end());
					// make sure curve_0 curve_1 both point out from connect_id
					if(is_overlapping(curve_0, curve_1))
					{
						found_overlapping = true;
						break;
					}
				}
				for(clit1=curve_frag_graph_.cFrags[connect_id].begin(); clit1!=curve_frag_graph_.cFrags[connect_id].end(); clit1++)
				{
					vcl_vector<dbdet_edgel*> curve_1((*clit1)->edgels.begin(), (*clit1)->edgels.end());
					// make sure curve_0 curve_1 both point out from connect_id
					if(is_overlapping(curve_0, curve_1))
					{
						found_overlapping = true;
						break;
					}
				}
				// if it is overlapping, remove the path
				if(found_overlapping)
				{
					curve_frag_graph_.HypFrags[edgel_id].remove(*clit0);
					clit0--;
				}*/

				break;
			}
		}


    }
}

//: merge all the curve fragment exact at junction point
void dbdet_sel_base::merge_non_jct_curve_frags()
{
	for (unsigned i=0; i<edgemap_->edgels.size(); i++)
	{
		dbdet_edgel* eA = edgemap_->edgels[i];

		if (curve_frag_graph_.pFrags[eA->id].size() + curve_frag_graph_.cFrags[eA->id].size() != 2) // not a merging point
			continue;

		//standard operation: extract them from the graph, reorder them, either merge or put them back
		dbdet_edgel_chain *c1=0, *c2=0;
		//segments need to meet continuity criteria (simple one for now)
		if (curve_frag_graph_.pFrags[i].size()>1){
			dbdet_edgel_chain_list_iter fit = curve_frag_graph_.pFrags[i].begin();
			c1 =  (*fit); fit++;
			c2 =  (*fit);

			curve_frag_graph_.extract_fragment(c1);
			curve_frag_graph_.extract_fragment(c2);

			//reverse the sequence of edgels
			vcl_reverse(c2->edgels.begin(), c2->edgels.end());
		}
		else if (curve_frag_graph_.pFrags[i].size()==1){
			c1 =  curve_frag_graph_.pFrags[i].front();
			c2 =  curve_frag_graph_.cFrags[i].front();
			//skip the closed contour case
			if(c1==c2)
				continue;
			curve_frag_graph_.extract_fragment(c1);
			curve_frag_graph_.extract_fragment(c2);
		}
		else {
			dbdet_edgel_chain_list_iter fit = curve_frag_graph_.cFrags[i].begin();
			c1 =  (*fit); fit++;
			c2 =  (*fit);

			//add the second one to the first one and delete it from the graph
			curve_frag_graph_.extract_fragment(c1);
			curve_frag_graph_.extract_fragment(c2);

			//reverse the sequence of edgels
			vcl_reverse(c1->edgels.begin(), c1->edgels.end());
		}

		if ( !is_not_continue(c1, c2)){ //if two contours are all very short < 5 edges and are continuous, merge them anyway
			//merge the two contours
			c1->append(c2->edgels);
			curve_frag_graph_.insert_fragment(c1);
			//when it makes a closed contour, just count as the child frag rather than parent frag
			//if(c1->edgels.front()==c1->edgels.back())
				//curve_frag_graph_.pFrags[c1->edgels.front()->id].remove(c1);
			//delete c2;
		}
		/*else if (c1->edgels.front()==c2->edgels.back())// form a cycle
		{
			c1->append(c2->edgels);
			curve_frag_graph_.insert_fragment(c1);
			delete c2;
		}*/
		else{
			curve_frag_graph_.insert_fragment(c1);
			curve_frag_graph_.insert_fragment(c2);
		}

	}
}

double cross_2d (double dx_1, double dy_1, double dx_2, double dy_2)
{
	return dx_1*dy_2 - dy_1*dx_2;
}
//: check if two chains have cross over links
bool dbdet_sel_base::is_cross_over(dbdet_edgel_chain* chain_1, dbdet_edgel_chain* chain_2)
{
	// only use the end points not for simplicity, but the right way should be the
	double p1_x = chain_1->edgels.front()->pt.x();
    double p1_y = chain_1->edgels.front()->pt.y();
    double q1_x = chain_1->edgels.back()->pt.x();
    double q1_y = chain_1->edgels.back()->pt.y();
    double p2_x = chain_2->edgels.front()->pt.x();
    double p2_y = chain_2->edgels.front()->pt.y();
    double q2_x = chain_2->edgels.back()->pt.x();
    double q2_y = chain_2->edgels.back()->pt.y();

    double dx_p1q1 = q1_x-p1_x;
    double dy_p1q1 = q1_y-p1_y;
    double dx_q1p2 = p2_x-q1_x;
    double dy_q1p2 = p2_y-q1_y;
    double dx_q1q2 = q2_x-q1_x;
    double dy_q1q2 = q2_y-q1_y;

    double dx_p2q2 = q2_x-p2_x;
    double dy_p2q2 = q2_y-p2_y;
    double dx_q2p1 = p1_x-q2_x;
    double dy_q2p1 = p1_y-q2_y;
    double dx_q2q1 = q1_x-q2_x;
    double dy_q2q1 = q1_y-q2_y;

    if(cross_2d(dx_p1q1, dy_p1q1, dx_q1p2, dy_q1p2)*cross_2d(dx_p1q1, dy_p1q1, dx_q1q2, dy_q1q2)<0 &&
    	cross_2d(dx_p2q2, dy_p2q2, dx_q2p1, dy_q2p1)*cross_2d(dx_p2q2, dy_p2q2, dx_q2q1, dy_q2q1)<0	)
    	return true;

	return false;
}


void dbdet_sel_base::prune_extreme_short_curve_frags() //  those isolated frags with only 2 edges, or single long link
{
	dbdet_edgel_chain_list_iter clit0;
	for( clit0= curve_frag_graph_.frags.begin(); clit0!=curve_frag_graph_.frags.end(); clit0++)
	{
		int eS_id = (*clit0)->edgels.front()->id;
		int eE_id = (*clit0)->edgels.back()->id;
		if((*clit0)->edgels.size()==2 && curve_frag_graph_.cFrags[eS_id].size() + curve_frag_graph_.pFrags[eS_id].size()==1
				&& curve_frag_graph_.cFrags[eE_id].size() + curve_frag_graph_.pFrags[eE_id].size()==1 )
		{
			curve_frag_graph_.extract_fragment(*clit0);
			clit0--;
			continue;
		}
		if((*clit0)->edgels.size()==2 && compute_path_len((*clit0)->edgels)>2)
		{
			curve_frag_graph_.extract_fragment(*clit0);
			clit0--;
			continue;
		}
	}
}

//:  those frags with only <=3 edges, with avg step length > 1
bool dbdet_sel_base::is_short_high_cost_curve_frag(dbdet_edgel_chain* chain)
{

		if(chain->edgels.size()>=4)
			return false;

		// remove single link
		if(chain->edgels.size()==2)
			return true;


		// remove free end short curve fragments
		int eS_id = (chain)->edgels.front()->id;
		int eE_id = (chain)->edgels.back()->id;
		if(curve_frag_graph_.cFrags[eS_id].size() + curve_frag_graph_.pFrags[eS_id].size()==1
				|| curve_frag_graph_.cFrags[eE_id].size() + curve_frag_graph_.pFrags[eE_id].size()==1 )
			return true;

		// remove high cost curve fragments
		if(compute_path_len((chain)->edgels)/((chain)->edgels.size()-1)>1.5)
			return true;

		return false;
}

//: A pre-processing function to fill small gaps so as to simplify the graph
//New version: we replaced end-to-end gap filling by end-to-non-end gap filling, but only link once. This is very risky.
void dbdet_sel_base::pre_processing()
{
  //vcl_cout << "Pre-processing..." << vcl_endl;

  //prune_redundant_frags();
  //regular_contour_filter();
  correct_CFG_topology();

  vcl_map<int, bool> explored;

  for(unsigned i = 0; i < edgemap_->edgels.size(); i++)
  {
    dbdet_edgel* eA = edgemap_->edgels[i];


    if((curve_frag_graph_.cFrags[eA->id].size() + curve_frag_graph_.pFrags[eA->id].size()) == 1)
    {
      //We only search in end points
      dbdet_link_list_iter lit = edge_link_graph_.cLinks[eA->id].begin();
      for(; lit != edge_link_graph_.cLinks[eA->id].end(); lit++)
      {
        dbdet_edgel* eB = (eA->id == (*lit)->pe->id)? (*lit)->ce : (*lit)->pe;
        bool triangle_formed = false;
        if(curve_frag_graph_.cFrags[eA->id].size() >= 1)
        {
          dbdet_edgel_chain_list_iter eclit = curve_frag_graph_.cFrags[eA->id].begin();
          for(; eclit != curve_frag_graph_.cFrags[eA->id].end(); eclit++)
          {
            if((*eclit)->edgels.back()->id == eB->id) triangle_formed = true;
          }
        }
        if(curve_frag_graph_.pFrags[eA->id].size() >= 1)
        {
          dbdet_edgel_chain_list_iter eclit = curve_frag_graph_.pFrags[eA->id].begin();
          for(; eclit != curve_frag_graph_.pFrags[eA->id].end(); eclit++)
          {
            if((*eclit)->edgels.front()->id == eB->id) triangle_formed = true;
          }
        }
        if(triangle_formed) continue;
        if((curve_frag_graph_.cFrags[eB->id].size() + curve_frag_graph_.pFrags[eB->id].size()) >= 1)
        {
          if((curve_frag_graph_.cFrags[eB->id].size() + curve_frag_graph_.pFrags[eB->id].size()) > 1 && (curve_frag_graph_.cFrags[eA->id].size() + curve_frag_graph_.pFrags[eA->id].size()) > 1)
          {
            continue;
          }
          if(explored[eA->id] || explored[eB->id]) continue;
          //If the other end is also an end point
          double dx = eB->pt.x() - eA->pt.x();
          double dy = eB->pt.y() - eA->pt.y();
          if(vcl_sqrt(dx * dx + dy * dy) < 1)
          {
            //For very short links joining two end points, fill the gap
            dbdet_edgel_chain* tmp_chain = new dbdet_edgel_chain();
            tmp_chain->push_back(eA);
            tmp_chain->push_back(eB);

            edge_link_graph_.linked[eA->id] = 1;
            edge_link_graph_.linked[eB->id] = 1;
            explored[eA->id] = true;
            explored[eB->id] = true;
            curve_frag_graph_.insert_fragment(tmp_chain);
            goto endthis;
          }
        }
      }

      lit = edge_link_graph_.pLinks[eA->id].begin();
      for(; lit != edge_link_graph_.pLinks[eA->id].end(); lit++)
      {
        dbdet_edgel* eB = (eA->id == (*lit)->pe->id)? (*lit)->ce : (*lit)->pe;
        bool triangle_formed = false;
        if(curve_frag_graph_.cFrags[eA->id].size() >= 1)
        {
          dbdet_edgel_chain_list_iter eclit = curve_frag_graph_.cFrags[eA->id].begin();
          for(; eclit != curve_frag_graph_.cFrags[eA->id].end(); eclit++)
          {
            if((*eclit)->edgels.back()->id == eB->id) triangle_formed = true;
          }
        }
        if(curve_frag_graph_.pFrags[eA->id].size() >= 1)
        {
          dbdet_edgel_chain_list_iter eclit = curve_frag_graph_.pFrags[eA->id].begin();
          for(; eclit != curve_frag_graph_.pFrags[eA->id].end(); eclit++)
          {
            if((*eclit)->edgels.front()->id == eB->id) triangle_formed = true;
          }
        }
        if(triangle_formed) continue;
        if((curve_frag_graph_.cFrags[eB->id].size() + curve_frag_graph_.pFrags[eB->id].size()) >= 1)
        {
          if((curve_frag_graph_.cFrags[eB->id].size() + curve_frag_graph_.pFrags[eB->id].size()) > 1 && (curve_frag_graph_.cFrags[eA->id].size() + curve_frag_graph_.pFrags[eA->id].size()) > 1)
          {
            continue;
          }
          if(explored[eA->id] || explored[eB->id]) continue;
          //If the other end is also an end point
          double dx = eB->pt.x() - eA->pt.x();
          double dy = eB->pt.y() - eA->pt.y();
          if(vcl_sqrt(dx * dx + dy * dy) < 1)
          {
            //For very short links joining two end points, fill the gap
            dbdet_edgel_chain* tmp_chain = new dbdet_edgel_chain();
            tmp_chain->push_back(eA);
            tmp_chain->push_back(eB);

            edge_link_graph_.linked[eA->id] = 1;
            edge_link_graph_.linked[eB->id] = 1;
            explored[eA->id] = true;
            explored[eB->id] = true;
            curve_frag_graph_.insert_fragment(tmp_chain);
            goto endthis;
          }
        }
      }
      endthis:;
    }
  }

  //Merge and apply filter again for a cleaner output
  //prune_redundant_frags();
  //minor_adjustment();
  correct_CFG_topology();
  //regular_contour_filter();
}

//: Refresh the linked array to avoid unforeseen mistakes
void dbdet_sel_base::refresh_linked_condition()
{

	//vcl_cout << edge_link_graph_.linked[16104] << vcl_endl;
	for(unsigned i = 0; i < edgemap_->edgels.size(); i++)
	{
	dbdet_edgel* eA = edgemap_->edgels[i];
	edge_link_graph_.linked[eA->id] = 0;
	}

	dbdet_edgel_chain_list_iter clit = curve_frag_graph_.frags.begin();
	for(; clit != curve_frag_graph_.frags.end(); clit++)
	{
	  dbdet_edgel_chain* tmp = (*clit);
	  dbdet_edgel_list_iter elit = tmp->edgels.begin();
	  for(; elit != tmp->edgels.end(); elit++)
	  {
		edge_link_graph_.linked[(*elit)->id] = 1;
	  }
	}

	int count = 0;
	for(unsigned i = 0; i < edgemap_->edgels.size(); i++)
	{
		dbdet_edgel* eA = edgemap_->edgels[i];
		if(edge_link_graph_.linked[eA->id] == 1)
			count ++;
	}
	//vcl_cout << count << " out of " << edgemap_->edgels.size() << " edges are linked" << vcl_endl;
}
