#include "dbdet_graphical_model_contour_merge.h"

struct var_node {
  enum label { UNDETERMINED, BREAK, MERGE };
  unsigned id;
  unsigned dim;
  unsigned edgel_id;
  vcl_vector<unsigned> n_facs;
  bool merged;
  double p;
  label gt_label;

  var_node(unsigned uid, unsigned e_id) : id(uid), dim(0), edgel_id(e_id), merged(false), p(0.0), gt_label(UNDETERMINED) {} 

  void push_fac(unsigned fac_id)
  {
    n_facs.push_back(fac_id);
    dim = n_facs.size();
  }
}

struct fac_node {
  dbdet_edgel_chain * chain;
  unsigned id;
  vcl_vector<unsigned> n_vars;
  bool removed;

  fac_node(dbdet_edgel_chain * c, unsigned uid, vcl_vector<unsigned> vars) : chain(c), n_vars(vars), id(uid), removed(false) {}
}

class factor_graph {
  
public:
  vcl_vector<var_node> var;
  vcl_vector<fac_node> fac;
 
  factor_graph(dbdet_curve_fragment_graph & CFG)
  {
    var.reserve(2 * CFG.frags.size());
    fac.reserve(CFG.frags.size());
    typedef vcl_pair<vcl_unordered_map<unsigned, unsigned>::iterator, bool> insert_ret;
    vcl_unordered_map<unsigned, unsigned> used_edgels;// edgel_id, var_node_id
    used_edgels.reserve(2 * CFG.frags.size());
    for (dbdet_edgel_chain_list_const_iter it=CFG.frags.begin(); it != CFG.frags.end(); it++)
    {
      dbdet_edgel * e1 = (*it)->edgels.front();
      dbdet_edgel * e2 = (*it)->edgels.back();

      vcl_vector<unsigned> var_ids;
      unsigned id = var.size();
      insert_ret ret = used_edgels.insert(vcl_pair<unsigned, unsigned>(e1->id, id));
      if(ret.second)
      {
        add_var_node(e1->id);
        var_ids.push_back(id);
      }
      else
      {
        var_ids.push_back((ret.first)->second);
      }

      id = var.size();
      ret = used_edgels.insert(vcl_pair<unsigned, unsigned>(e2->id, id));
      if(ret.second)
      {
        add_var_node(e2->id);
        var_ids.push_back(id);
      }
      else
      {
        var_ids.push_back((ret.first)->second);
      }
    }
  }

private:
  void add_var_node(unsigned edgel_id)
  {
    var.push_back(var_node(var.size() + 1, edgel_id));
  }

  void add_fac_node(dbdet_edgel_chain * c, vcl_vector<unsigned> vars)
  {
    unsigned id = var.size();
    fac.push_back(fac_node(c, id, vars));
    for (unsigned i = 0; i < vars.size(); ++i)
    {
       var[vars[i]].push_fac(id);
    }
  }
};

void dbdet_graphical_model_contour_merge::
dbdet_merge_contour(
      dbdet_curve_fragment_graph & CFG,
      y_params_1_vector & beta1,
      y_params_1_vector & fmean1,
      y_params_0_vector & beta0,
      y_params_0_vector & fmean0
      )
{
  factor_graph g(CFG);

  for (unsigned k = 0; k < g.var.size(); ++k)
  {
    var_node & cur_node = g.var[k];
    if (cur_node.dim == 2)
    {
      unsigned c1_id = cur_node.n_facs.front();
      unsigned c2_id = cur_node.n_facs.back();
      if (c1_id == c2_id)
        continue;

      dbdet_edgel_chain * c1 = g.fac[c1_id].chain;
      dbdet_edgel_chain * c2 = g.fac[c2_id].chain;

      dbdet_edgel_chain c1_cut, c2_cut;
      

      //always make directions c1 -> node -> c2

      unsigned cut_size = vcl_min(nbr_num_edges, c1->edgels.size());
      c1_cut.edgels.resize(cut_size);
      if (c1->edgels.front()->id == cur_node.edgel_id)
      {
        for (unsigned i = 0; i < cut_size; ++i)
        {
          c1_cut.edgels[i] = c1->edgels[cut_size - i - 1];
        }
      }
      else if(c1->edgels.back()->id == cur_node.edgel_id)
      {
        unsigned start_i = c1->edgels.size() - cut_size;
        for (unsigned i = 0; i < cut_size; ++i)
        {
          c1_cut.edgels[i] = c1->edgels[start_i + i];
        }
      }

      cut_size = vcl_min(nbr_num_edges, c2->edgels.size());
      c2_cut.edgels.resize(cut_size);
      if (c2->edgels.front()->id == cur_node.edgel_id)
      {
        for (unsigned i = 0; i < cut_size; ++i)
        {
          c2_cut.edgels[i] = c2->edgels[i];
        }
      }
      else if(c2->edgels.back()->id == cur_node.edgel_id)
      {
        unsigned start_i = c2->edgels.size() - 1;
        for (unsigned i = 0; i < cut_size; ++i)
        {
          c1_cut.edgels[i] = c1->edgels[start_i - i];
        }
      }

      y_feature_vector c1_features, c2_features;
      cues_computer.compute_all_cues(c1, &c1_features);
      cues_computer.compute_all_cues(c2, &c2_features);

      y_params_0_vector cues;
      cues[y_params_0::Y_ONE] = 1.0;
      for (unsigned i = 1; i < abs_diff.size(); ++i)
      {
        cues[i] = vcl_abs(c1_features[i] - c2_features[i]);
      }

      double geom_diff, texture_diff;
      dbdet_degree_2_node_cues(c1, c2, geom_diff, texture_diff);

      cues[y_params_0::Y_GEOM] = geom_diff;
      cues[y_params_0::Y_TEXTURE] = texture_diff;

      bool merge = false;

      //for very short curve, just decide merging based on geometry;
      if (c1->edgels.size() < dbdet_yuliang_const::nbr_len_th || c2->edgels.size() < dbdet_yuliang_const::nbr_len_th)
      {
        double p = 1.0 / (1.0 + exp(-((1.0 - fmean1[0]) * beta1[0] + (geom_diff - fmean1[1]) * beta1[1])));
        merge = p > dbdet_yuliang_const::merge_th_geom ? true : false;
      }
      else
      {
        double sum = 0.0; 
        for (unsigned i = 0; i < cues.size(); ++i)
        {
          sum += (cues[i] - fmean0[i]) * beta0[i];
        }
        double p = 1.0 / (1.0 + exp(-sum));
        merge = p > dbdet_yuliang_const::merge_th_sem ? true : false;
      }

      if (merge)
      {
        merge_at_degree_2_node(/*G, merged_cem, merged_cf_idx, v, actual_edge, nbrs_fac, c1_ids, c2_ids*/);
      }
    }
  }

  //dim 3 node should be dealt with after all dim 2 node are solved 
  for (unsigned k = 0; k < g.var.size(); ++k)
  {
    var_node & cur_node = g.var[k];
    if (cur_node.dim == 3)
    {
      //TODO
    }
  }
}
