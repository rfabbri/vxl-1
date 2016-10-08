#include "dbdet_contour_breaker.h"
#include <sel/dbdet_edgel.h>

dbdet_curve_fragment_graph dbdet_contour_breaker::
dbdet_contour_breaker_geom(
      dbdet_curve_fragment_graph & CFG,
      y_feature_vector beta1,
      y_feature_vector fmean
      )
{
  unsigned const ref_tabel_nbr_range = 2;
  dbdet_curve_fragment_graph newCFG(CFG);
  dbdet_edgel_chain_list & frags = newCFG.frags;
  vcl_vector<double> clen(frags.size(), 0.0);

  double min_contour_len = nbr_len_th * diag_ratio;

  for (int i = 0;  i < frags.size(); ++i)
  {
    clen[i] = euclidean_length(*frags[i]);

    dbdet_edgel & start = frags[i]->front;
    dbdet_edgel & end = frags[i]->back;

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
      
      for (unsigned l = xi; l < xj; ++l)
        for(unsigned m = yi; m < yj; ++m)
          ref_start_pts(l, m) = i;

      x = static_cast<unsigned>(end->pt.x() + 0.5);
      y = static_cast<unsigned>(end->pt.y() + 0.5);
      xi = vcl_max(x - ref_tabel_nbr_range, 0);
      xf = vcl_min(x + ref_tabel_nbr_range, ni());
      yi = vcl_max(y - ref_tabel_nbr_range, 0);
      yf = vcl_min(y + ref_tabel_nbr_range, nj());
      
      for (unsigned l = xi; l < xj; ++l)
        for(unsigned m = yi; m < yj; ++m)
          ref_end_pts(l, m) = i;
    }  
  }

  //TODO: work in progress
  return new_CFG;
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
