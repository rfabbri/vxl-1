#include "dbdet_contour_breaker.h"
#include <sel/dbdet_edgel.h>
#include <vcl_algorithm.h>

dbdet_curve_fragment_graph dbdet_contour_breaker::
dbdet_contour_breaker_geom(
      dbdet_curve_fragment_graph & CFG,
      y_feature_vector beta1,
      y_feature_vector fmean
      )
{
  unsigned const ref_tabel_nbr_range = 2;
  unsigned const not_assigned = -1;

  dbdet_curve_fragment_graph newCFG(CFG);
  
  vcl_vector <*dbdet_edgel_chain> frags(newCFG.frags.size());
  for (dbdet_edgel_chain_list_const_iter it=newCFG.frags.begin(); it != newCFG.frags.end(); it++)
    frags[i] = (*it);

  vcl_vector<double> clen(frags.size(), 0.0);

  double min_contour_len = nbr_len_th * diag_ratio;

  for (unsigned i = 0; i < ni(); ++i)
  {
    for(unsigned j = 0; j < nj(); ++j)
    {
      ref_start_pts(i, j) = not_assigned;
      ref_end_pts(i, j) = not_assigned;
    }
  }

  for (unsigned i = 0; i < frags.size(); ++i)
  {
    clen[i] = euclidean_length(*frags[i]);

    dbdet_edgel & start = *(frags[i]->edgels.front);
    dbdet_edgel & end = *(frags[i]->edgels.back);

    if (start.pt == end.pt && start.deriv == end.deriv)
      continue;

    if (clen[i] > min_contour_len && frags[i]->size() > nbr_len_th)
    {
      unsigned x, y, xi, xf, yi, yf;
      x = static_cast<unsigned>(start->pt.x() + 0.5);
      y = static_cast<unsigned>(start->pt.y() + 0.5);
      xi = vcl_max(x - ref_tabel_nbr_range, 0);
      xf = vcl_min(x + ref_tabel_nbr_range, ni());
      yi = vcl_max(y - ref_tabel_nbr_range, 0);
      yf = vcl_min(y + ref_tabel_nbr_range, nj());
      
      for (unsigned l = xi; l < xf; ++l)
        for (unsigned m = yi; m < yf; ++m)
          ref_start_pts(l, m) = i;

      x = static_cast<unsigned>(end->pt.x() + 0.5);
      y = static_cast<unsigned>(end->pt.y() + 0.5);
      xi = vcl_max(x - ref_tabel_nbr_range, 0);
      xf = vcl_min(x + ref_tabel_nbr_range, ni());
      yi = vcl_max(y - ref_tabel_nbr_range, 0);
      yf = vcl_min(y + ref_tabel_nbr_range, nj());
      
      for (unsigned l = xi; l < xf; ++l)
        for (unsigned m = yi; m < yf; ++m)
          ref_end_pts(l, m) = i;
    }  
  }


  for (unsigned i = 0; i < frags.size(); ++i)
    dbdet_edgel & start = *(frags[i]->edgels.front);
    dbdet_edgel & end = *(frags[i]->edgels.back);

    if (start.pt == end.pt && start.deriv == end.deriv)
      continue;

    if(clen[i] > (10 * diag_ratio) && frags[i]->edgels.size() > nbr_num_edges + 1)
    {
      vcl_vector<unsigned> cur_break_e_id;
      vcl_vector <unsigned> start_ids(frags[i]->edgels.size());
      vcl_set <unsigned> unique_start_ids;
      vcl_vector <unsigned> end_ids(frags[i]->edgels.size());
      vcl_set <unsigned> unique_end_ids;

      for (unsigned k = 0; k < frags[i]->size(); ++k)
      {
        unsigned x = static_cast<unsigned>(end->pt.x() + 0.5);
        unsigned y = static_cast<unsigned>(end->pt.y() + 0.5);
        x = vcl_max(vcl_min(x, ni() - 1), 0);
        y = vcl_max(vcl_min(y, nj() - 1), 0);
          
        start_ids[k] = ref_start_pts[x][y];
        end_ids[k] = ref_end_pts[x][y];

        if (ref_start_pts[x][y] != not_assigned && ref_start_pts[x][y] != i)
          unique_start_id.insert(ref_start_pts[x][y]);

        if (ref_end_pts[x][y] != not_assigned && ref_end_pts[x][y] != i)
          unique_end_id.insert(ref_end_pts[x][y]);
      }

      compute_break_point(frags, i, start_ids, unique_start_ids, true, cur_break_e_id);
      compute_break_point(frags, i, end_ids, unique_end_ids, false, cur_break_e_id);

      if (cur_break_e_id.size() > 0)
      {
        vcl_sort(cur_break_e_id.begin(), cur_break_e_id.end());
        dbdet_edgel_chain copy = *frags[i];
        dbdet_edgel_chain & ref = *frags[i];

        dbdet_edgel_chain * newChain;

        ref.edgels = dbdet_edgel_list(copy.edgels.begin() + cur_break_e_id[cur_break_e_id.size() - 1], copy.edgels.end());
        clen[i] = euclidean_length(ref);

        newChain = new dbdet_edgel_chain();
        newChain.edgels = dbdet_edgel_list(copy.edgels.begin(), copy.edgels.begin() + cur_break_e_id[0] + 1);
        newCFG.frags.push_back(newChain);
        clen.push_back(euclidean_length(*newChain));

        unsigned x, y, xi, xf, yi, yf;
        x = static_cast<unsigned>(start->pt.x() + 0.5);
        y = static_cast<unsigned>(start->pt.y() + 0.5);
        xi = vcl_max(x - ref_tabel_nbr_range, 0);
        xf = vcl_min(x + ref_tabel_nbr_range, ni());
        yi = vcl_max(y - ref_tabel_nbr_range, 0);
        yf = vcl_min(y + ref_tabel_nbr_range, nj());
        
        for (unsigned l = xi; l < xf; ++l)
          for (unsigned m = yi; m < yf; ++m)
            ref_start_pts(l, m) = newCFG.frags.size();

        for (unsigned k = 1; k < cur_break_e_id.size(); ++k)
        {
          newChain = new dbdet_edgel_chain();
          newChain.edgels = dbdet_edgel_list(copy.edgels.begin() + cur_break_e_id[k-1], copy.edgels.begin() + cur_break_e_id[k]);
          newCFG.frags.push_back(newChain);
          clen.push_back(euclidean_length(*newChain));
        }   
      }
    }  
  }

  for(unsigned i = 0; i < max_it; ++i)
  {
    for(dbdet_edgel_chain_list_iter it = newCFG.frags.begin(); it != newCFG.frags.end(); it++)
    {
      dbdet_edgel & start = *(*it->edgels.front);
      dbdet_edgel & end = *(*it->edgels.back);

      if (start.pt == end.pt && start.deriv == end.deriv)
        continue;

      if (clen[i] > (10 * diag_ratio) && *it->edgels.size() > (nbr_num_edges + 1) / i)
      {
        vcl_vector<double> prob;
        compute_merge_probability(*it, nbr_num_edges /(2 * i) , beta1, fmean, prob);
        double min = vcl_numeric_limits<double>::max();
        double lastVal = min;
        unsigned firstId = -1, lastId = -1;
        for (int k = 0; k < prob.size() ++k)
        {
          if (prob[i] < min)
          {
            min = prob[i];
            firstId = lastId = k; 
          }
          else if(prob[i] == min)
          {
            lastId = k;
          }
        }
        if (min < merge_th_geom)
        {
          unsigned id = (firstId + lastId + 1) / 2;
          dbdet_edgel_chain copy = *it;
          dbdet_edgel_chain & ref = *it;

          dbdet_edgel_chain * newChain;

          ref.edgels = dbdet_edgel_list(copy.edgels.begin() + id, copy.edgels.end());
          clen[i] = euclidean_length(ref);

          newChain = new dbdet_edgel_chain();
          newChain.edgels = dbdet_edgel_list(copy.edgels.begin(), copy.edgels.begin() + id + 1);
          newCFG.frags.push_back(newChain);
          clen.push_back(euclidean_length(*newChain));
        }
      }
    }
  }

  return new_CFG;
}

void dbdet_contour_breaker::
compute_break_point(
      vcl_vector<*dbdet_edgel_chain> & frags,
      unsigned frag_id, vcl_vector<unsigned> & ids,
      vcl_set<unsigned> & unique_ids,
      bool front,
      vcl_vector<unsigned> break_e_ids)
{
      unsigned prev_id = 0;
      for (vcl_set<unsigned>::iterator set_it = unique_ids.begin(); set_it != unique_ids.end(); ++set_it)
      {
        dbdet_edgel_chain & chain = (*frags[frag_id]);
        double min = numeric_limits<double>::max();
        unsigned e_id = -1;
        for (int k = 0; k < ids.size(); ++k)
        {
          if((*set_it) == ids[k])
          {
            double dist = vgl_distance(chain.edgels[k].pt, (*frags[(*set_it)]).edgels[0].pt);
            if (dist < min)
            {
              min = dist;
              e_id = k;
            }
          }
        }

        if (e_id < nbr_len_th - 1 || e_id > chain.size() - nbr_len_th || (e_id - prev_id) < nbr_len_th)
          continue;

        if(min == 0)
        {
          //junction_pts = [junction_pts; cur_c(e_id, :)];
          //introduced_num_junction_points = introduced_num_junction_points + 1; 
          break_e_ids.push_back(e_id);
          continue;
        }

        vgl_point_2d<double> c_ori = chain[e_id + 1].pt - chain[e_id - 1].pt;

        unsigned a_id1, a_id2;
        if (front)
        {
          a_id1 = vcl_min(nbr_len_th, frags[(*set_it)]->size() - 1);
          a_id2 = 0;
        }
        else
        {
          a_id1 = frags[(*set_it)]->edgels.size() - 1;
          a_id2 = vcl_max(frags[(*set_it)]->edgels.size() - 1 - nbr_len_th, 0);
        }
        vgl_point_2d<double> a_ori = (*frags[(*set_it)]).edgels[a_id1].pt - (*frags[(*set_it)]).edgels[a_id2].pt;

        double cos_diff = (c_ori.x() * a_ori.x() + c_ori.y() + a_ori.y()) / 
        (vcl_sqrt(c_ori.x() * c_ori.x() + c_ori.y() + c_ori.y()) * vcl_sqrt(a_ori.x() * a_ori.x() + a_ori.y() + a_ori.y()));

        if(cos_diff > vcl_cos(vnl_math::pi / 6.0))
          continue;

        break_e_ids.push_back(e_id);
        prev_id = e_id;
      }
}

void dbdet_contour_breaker::
compute_merge_probability(
      dbdet_edgel_chain & chain,
      unsigned nbr_range_th,
      double[2] beta1,
      double[2] fmean,
      vcl_vector<double> & prob
      )
{
  nbr_range_th = vcl_min(nbr_range_th, nbr_len_th);
  prob.resize(chain.edgels.size());
  
  for (unsigned i = 0; i < prob.size(); ++i)
    prob[i] = 1.0;

  for (unsigned i = nbr_range_th; i < prob.size() - 1 - nbr_range_th; ++i)
  {
    vgl_point_2d<double> a = chain.edgels[i]->pt - chain.edgels[i - nbr_range_th]->pt;
    vgl_point_2d<double> b = chain.edgels[i + nbr_range_th]->pt - chain.edgels[i]->pt;
    
    double cos_diff = (a.x() * b.x() + a.y() + b.y()) / 
                      (vcl_sqrt(a.x() * a.x() + a.y() + a.y()) * vcl_sqrt(b.x() * b.x() + b.y() + b.y()));
    //prob(i) = 1 / (1 + exp(-([1 geom_diff] - fmean_1)*beta_1'));
    prob[i] =  1.0 / (1.0 + exp(-((1.0 - fmean[0]) * beta[0] + (cos_diff - fmean[1]) * beta[1]))));
  }
}

dbdet_curve_fragment_graph dbdet_contour_breaker::
dbdet_contour_breaker_semantic(
      dbdet_curve_fragment_graph & CFG,
      y_feature_vector beta1,
      y_feature_vector fmean
      )
{
  dbdet_curve_fragment_graph new_CFG(CFG);

  //TODO
  return new_CFG;
}
