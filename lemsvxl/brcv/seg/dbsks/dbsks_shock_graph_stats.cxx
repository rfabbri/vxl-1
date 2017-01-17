// This is file seg/dbsks/dbsks_shock_graph_stats.cxx

//:
// \file

#include "dbsks_shock_graph_stats.h"
#include <vcl_utility.h>
#include <vnl/vnl_math.h>
#include <dbsksp/dbsksp_shock_graph.h>
#include <dbsksp/dbsksp_shapelet.h>
#include <dbsksp/dbsksp_shock_fragment.h>
#include <dbsks/dbsks_utils.h>


// =============================================================================
// dbsks_shock_graph_stats
// =============================================================================


//: Clear all existing statistics
void dbsks_shock_graph_stats::
clear()
{
  this->shock_graph_ = 0;
  this->shapelet_stats_map_.clear();
}


// -----------------------------------------------------------------------------
//: Compute the shock graph parameter statistics from a set of shock graphs
// REQUIREMENT: all shock graphs have exactly the same topology
bool dbsks_shock_graph_stats::
compute_stats(const vcl_vector<dbsksp_shock_graph_sptr >& shock_graph_list)
{
  if (shock_graph_list.empty())
  {
    vcl_cout << "ERROR: shock_graph_list is empty! \n";
    return true;
  }

  // make sure all parameters are computed explicitly
  for (unsigned i =0; i < shock_graph_list.size(); ++i)
  {
    shock_graph_list[i]->compute_all_dependent_params();
  }

  // Scale every graph to the scale of the reference graph
  double ref_size = dbsks_compute_graph_size(this->shock_graph());
  
  // Set the graph size corresponding to the statistics
  this->graph_size_ = ref_size;
  
  
  // make sure all parameters are computed explicitly
  for (unsigned i =0; i < shock_graph_list.size(); ++i)
  {
    dbsksp_shock_graph_sptr graph = shock_graph_list[i];
    graph->compute_all_dependent_params();
    double size = dbsks_compute_graph_size(graph);
    graph->scale_up(ref_size / size);
  }

  // >> Collect corresponding shapelets from the set of shock graphs
  vcl_map<unsigned int, vcl_vector<dbsksp_shapelet_sptr > > shapelet_bins;
  
  // create place holder for all the shapelets
  dbsksp_shock_graph_sptr shock_graph = shock_graph_list[0];
  for (dbsksp_shock_graph::edge_iterator eit = shock_graph->edges_begin();
    eit != shock_graph->edges_end(); ++eit)
  {
    dbsksp_shock_edge_sptr e = *eit;
    
    // only deal with regular A1^2 fragments
    if (e->is_terminal_edge())
      continue;

    vcl_vector<dbsksp_shapelet_sptr > empty_list;
    vcl_map<unsigned int, vcl_vector<dbsksp_shapelet_sptr > >::iterator it = 
      shapelet_bins.insert(shapelet_bins.end(), vcl_make_pair(e->id(), empty_list));
    it->second.reserve(shock_graph_list.size());
  }

  // iterate thru the shock graphs and and put their shapelets in appropriate bins
  for (unsigned i =0; i < shock_graph_list.size(); ++i)
  {
    dbsksp_shock_graph_sptr shock_graph = shock_graph_list[i];

    // terminal edges are grouped with the edge adjacent to them
    for (dbsksp_shock_graph::edge_iterator eit = shock_graph->edges_begin();
      eit != shock_graph->edges_end(); ++eit)
    {
      dbsksp_shock_edge_sptr e = *eit;
      if (e->is_terminal_edge())
        continue;

      // retrieve the current (reference) shapelet for this edge
      dbsksp_shapelet_sptr s = e->fragment()->get_shapelet();

      // make sure the shapelet is in the right direction, i.e. facing downward to its children
      if (e->target()->parent_edge() != e )
      {
        s = s->reversed_dir();
      }

      // put shape fragment into appropriate bin
      vcl_map<unsigned int, vcl_vector<dbsksp_shapelet_sptr > >::iterator it =
        shapelet_bins.find(e->id());
      if (it == shapelet_bins.end())
      {
        vcl_cout << "ERROR: the shock graphs are not similar topologically.\n";
        return false;
      }
      it->second.push_back(s);
    }
  }

  // Compute statistics within each bin separately
  for (vcl_map<unsigned int, vcl_vector<dbsksp_shapelet_sptr > >::iterator it =
    shapelet_bins.begin(); it != shapelet_bins.end(); ++it)
  {
    dbsks_shapelet_stats stat;
    stat.compute_stats(it->second);
    this->shapelet_stats_map_.insert(vcl_make_pair(it->first, stat));
  }

  return true;
}



// -----------------------------------------------------------------------------
//: Return statistics of one fragment
dbsks_shapelet_stats dbsks_shock_graph_stats::
shapelet_stats(unsigned int edge_id) const
{
  vcl_map<unsigned int, dbsks_shapelet_stats >::const_iterator it = 
    this->shapelet_stats_map_.find(edge_id);
  assert(it != this->shapelet_stats_map_.end());

  return it->second;
}



// -----------------------------------------------------------------------------
//: Set statistics of one fragment
void dbsks_shock_graph_stats::
set_shapelet_stats(unsigned int edge_id, const dbsks_shapelet_stats& frag_stats)
{
  this->shapelet_stats_map_[edge_id] = frag_stats;
  return;
}



// -----------------------------------------------------------------------------
//: Generate a shapelet grid based on the computed stats and the desired sampling parameters
dbsks_shapelet_grid dbsks_shock_graph_stats::
build_shapelet_grid(unsigned int edge_id, 
                    const dbsks_shapelet_sampling_params& p,
                    double target_graph_size) const
{
  if (target_graph_size == 0)
  {
    target_graph_size = this->graph_size();
  }

  // \TODO check if edge_id is valid
  dbsks_shapelet_grid grid;

  // Determine whether this shapelet includes the front or rear arcs (end-fragments)
  dbsksp_shock_edge_sptr e = this->shock_graph()->edge_from_id(edge_id);

  dbsksp_shock_node_sptr v_child = e->child_node();
  if (v_child->degree() == 2 && 
    this->shock_graph()->cyclic_adj_succ(e, v_child)->is_terminal_edge())
  {
    grid.has_rear_arc_ = true;
  }
  else
  {
    grid.has_rear_arc_ = false;
  }

  dbsksp_shock_node_sptr v_parent = e->parent_node();
  if (v_parent->degree() == 2 && 
    this->shock_graph()->cyclic_adj_succ(e, v_parent)->is_terminal_edge())
  {
    grid.has_front_arc_ = true;
  }
  else
  {
    grid.has_front_arc_ = false;
  }


  // retrieve the current (reference) shapelet for this edge
  dbsksp_shapelet_sptr sref = e->fragment()->get_shapelet();

  // make sure the shapelet is in the right direction, i.e. facing downward to its children
  if (e->target()->parent_edge() != e )
  {
    sref = sref->reversed_dir();
  }



  // -----------------------------------------------------------------
  // sample other parameters from the statistics of the traning data
  // retrieve the statistics for this fragment
  dbsks_shapelet_stats frag_stats = 
    this->shapelet_stats_map_.find(edge_id)->second;

  // x
  grid.max_xA_ = p.center_x + p.step_x * (p.num_x-1) / 2;
  grid.min_xA_ = p.center_x - p.step_x * (p.num_x-1) / 2;
  grid.num_xA_ = p.num_x;

  // y
  grid.max_yA_ = p.center_y + p.step_y * (p.num_y-1) / 2;
  grid.min_yA_ = p.center_y - p.step_y * (p.num_y-1) / 2;
  grid.num_yA_ = p.num_y;

  // theta
  grid.min_psiA_ = 0;
  grid.num_psiA_ = p.num_psi;
  grid.step_psiA_ = vnl_math::pi*2 / p.num_psi;
  grid.max_psiA_ = grid.min_psiA_+ (grid.num_psiA_-1)* grid.step_psiA_;

  //// phiA
  //double mean_phiA, var_phiA;
  //frag_stats.stats_phiA(mean_phiA, var_phiA);
  //grid.min_phiA_ = mean_phiA - 2 * vcl_sqrt(var_phiA);
  //grid.max_phiA_ = mean_phiA + 2 * vcl_sqrt(var_phiA);
  //grid.num_phiA_ = p.num_phiA;


  //// phiB
  //double mean_phiB, var_phiB;
  //frag_stats.stats_phiB(mean_phiB, var_phiB);
  //grid.min_phiB_ = mean_phiB - 2 * vcl_sqrt(var_phiB);
  //grid.max_phiB_ = mean_phiB + 2 * vcl_sqrt(var_phiB);
  //grid.num_phiB_ = p.num_phiB;

  //// m
  //double mean_m, var_m;
  //frag_stats.stats_m(mean_m, var_m);
  //grid.min_m_ = mean_m - 2 * vcl_sqrt(var_m);
  //grid.max_m_ = mean_m + 2 * vcl_sqrt(var_m);
  //grid.num_m_ = p.num_m;

  //// rA
  //double mean_log2_rA, var_log2_rA;
  //frag_stats.stats_log2_rA(mean_log2_rA, var_log2_rA);
  //
  //// adjust according to scaling parameter
  //mean_log2_rA += vcl_log(target_graph_size / this->graph_size()) / vnl_math::ln2;

  //grid.ref_rA_ = vcl_exp(vnl_math::ln2 * mean_log2_rA);
  //grid.min_log2_rA_ = - 2 * vcl_sqrt(var_log2_rA);
  //grid.max_log2_rA_ = 2 * vcl_sqrt(var_log2_rA);
  //grid.num_rA_ = p.num_rA;

  //// length
  //double mean_log2_len, var_log2_len;
  //frag_stats.stats_log2_len(mean_log2_len, var_log2_len);

  //// adjust according to scaling parameter
  //mean_log2_len += vcl_log(target_graph_size / this->graph_size()) / vnl_math::ln2;

  //grid.ref_len_ = vcl_exp(vnl_math::ln2 * mean_log2_len);
  //grid.min_log2_len_ = - 2 * vcl_sqrt(var_log2_len);
  //grid.max_log2_len_ = 2 * vcl_sqrt(var_log2_len);
  //grid.num_len_ = p.num_len;


  //// //////////////////////////////////////////////
  //// use the shock graph as reference
  //// //////////////////////////////////////////////


  //// phiA
  //double mean_phiA, var_phiA;
  //frag_stats.stats_phiA(mean_phiA, var_phiA);
  //mean_phiA = sref->phi_start();

  //grid.min_phiA_ = mean_phiA - 2 * vcl_sqrt(var_phiA);
  //grid.max_phiA_ = mean_phiA + 2 * vcl_sqrt(var_phiA);
  //grid.num_phiA_ = p.num_phiA;


  //// phiB
  //double mean_phiB, var_phiB;
  //frag_stats.stats_phiB(mean_phiB, var_phiB);
  //mean_phiB = sref->phi_end();

  //grid.min_phiB_ = mean_phiB - 2 * vcl_sqrt(var_phiB);
  //grid.max_phiB_ = mean_phiB + 2 * vcl_sqrt(var_phiB);
  //grid.num_phiB_ = p.num_phiB;


  //// m
  //double mean_m, var_m;
  //frag_stats.stats_m(mean_m, var_m);
  //mean_m = sref->m_start();

  //grid.min_m_ = mean_m - 4 * vcl_sqrt(var_m);
  //grid.max_m_ = mean_m + 4 * vcl_sqrt(var_m);
  //grid.num_m_ = p.num_m;

  //// rA
  //double mean_log2_rA, var_log2_rA;
  //frag_stats.stats_log2_rA(mean_log2_rA, var_log2_rA);
  //
  //mean_log2_rA = vcl_log(sref->radius_start()) / vnl_math::ln2;

  //// adjust according to scaling parameter
  ////mean_log2_rA += vcl_log(target_graph_size / this->graph_size()) / vnl_math::ln2;

  //grid.ref_rA_ = vcl_exp(vnl_math::ln2 * mean_log2_rA);
  //grid.min_log2_rA_ = - 4 * vcl_sqrt(var_log2_rA);
  //grid.max_log2_rA_ = 4 * vcl_sqrt(var_log2_rA);
  //grid.num_rA_ = p.num_rA;

  //// length
  //double mean_log2_len, var_log2_len;
  //frag_stats.stats_log2_len(mean_log2_len, var_log2_len);

  //mean_log2_len = vcl_log(sref->len()) / vnl_math::ln2;
  //// adjust according to scaling parameter
  ////mean_log2_len += vcl_log(target_graph_size / this->graph_size()) / vnl_math::ln2;

  //grid.ref_len_ = vcl_exp(vnl_math::ln2 * mean_log2_len);
  //grid.min_log2_len_ = - 4 * vcl_sqrt(var_log2_len);
  //grid.max_log2_len_ = 4 * vcl_sqrt(var_log2_len);
  //grid.num_len_ = p.num_len;



  // //////////////////////////////////////////
  // USE MIN MAX to construct grid

  // phiA
  double mean_phiA, var_phiA, min_phiA, max_phiA;
  frag_stats.stats_phiA(mean_phiA, var_phiA, min_phiA, max_phiA);
  grid.min_phiA_ = min_phiA;
  grid.max_phiA_ = max_phiA;
  grid.num_phiA_ = p.num_phiA;


  // phiB
  double mean_phiB, var_phiB, min_phiB, max_phiB;
  frag_stats.stats_phiB(mean_phiB, var_phiB, min_phiB, max_phiB);
  grid.min_phiB_ = min_phiB;
  grid.max_phiB_ = max_phiB;
  grid.num_phiB_ = p.num_phiB;


  // m
  double mean_m, var_m, min_m, max_m;
  frag_stats.stats_m(mean_m, var_m, min_m, max_m);
  grid.min_m_ = min_m;
  grid.max_m_ = max_m;
  grid.num_m_ = p.num_m;

  // rA
  double mean_log2_rA, var_log2_rA, min_log2_rA, max_log2_rA;
  frag_stats.stats_log2_rA(mean_log2_rA, var_log2_rA, min_log2_rA, max_log2_rA);
  
  // ignore the statistical mean, only use min and max
  mean_log2_rA = (min_log2_rA + max_log2_rA) / 2;
  double span_log2_rA = (max_log2_rA - min_log2_rA);

  // adjust according to scaling parameter
  mean_log2_rA += vcl_log(target_graph_size / this->graph_size()) / vnl_math::ln2;

  grid.ref_rA_ = vcl_exp(vnl_math::ln2 * mean_log2_rA);
  grid.min_log2_rA_ = - span_log2_rA / 2;
  grid.max_log2_rA_ = span_log2_rA / 2;
  grid.num_rA_ = p.num_rA;

  // length
  double mean_log2_len, var_log2_len, min_log2_len, max_log2_len;
  frag_stats.stats_log2_len(mean_log2_len, var_log2_len, min_log2_len, max_log2_len);

  // ignore the statistical mean, only use min and max
  mean_log2_len = (min_log2_len + max_log2_len) / 2;
  double span_log2_len = (max_log2_len - min_log2_len);

  // adjust according to scaling parameter
  mean_log2_len += vcl_log(target_graph_size / this->graph_size()) / vnl_math::ln2;

  grid.ref_len_ = vcl_exp(vnl_math::ln2 * mean_log2_len);
  grid.min_log2_len_ = - span_log2_len;
  grid.max_log2_len_ = span_log2_len;
  grid.num_len_ = p.num_len;

  grid.compute_grid();

  return grid;
}



// -----------------------------------------------------------------------------
//: Print summary
vcl_ostream& dbsks_shock_graph_stats::
print(vcl_ostream& str) const
{
  return str;
}


