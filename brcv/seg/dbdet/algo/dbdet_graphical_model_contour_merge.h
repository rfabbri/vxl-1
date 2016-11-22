// This is dbdet_graphical_model_contour_merge.h
#ifndef dbdet_graphical_model_contour_merge_h
#define dbdet_graphical_model_contour_merge_h

class dbdet_graphical_model_contour_merge {

public:

  dbdet_graphical_model_contour_merge(
    const vil_image_view<vil_rgb<vxl_byte> > &img,
    const dbdet_edgemap &em,
    const vnl_matrix<unsigned> & tmap
    )
    :
    img_(img),
    em_(em),
    tmap_(tmap)
  {
    assert(em.ncols() == img.ni() && em.nrows() == img.nj() && tmap.rows() == img.ni() && tmap.cols() == img.nj());
  }

  void dbdet_merge_contour(dbdet_curve_fragment_graph & CFG, y_params_1_vector & beta, y_params_1_vector & fmean, y_params_0_vector & beta, y_params_0_vector & fmean);
private:

  void dbdet_degree_2_node_cues(dbdet_edgel_chain & c1, dbdet_edgel_chain & c2/*, tmap, nbr_num_edges*/);

  void merge_at_degree_2_node(dbdet_factor_graph G, dbdet_curve_fragment_graph & newCFG, v, actual_edge, nbrs_fac, c1_ids, c2_ids);

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

  unsigned ni() const { return em_.ncols(); }
  unsigned nj() const { return em_.nrows(); }
  const vil_image_view<vil_rgb<vxl_byte> > &img_; // color RGB image
  const dbdet_edgemap &em_;
  const vnl_matrix<unsigned> tmap_;
};
#endif //dbdet_graphical_model_contour_merge_h
