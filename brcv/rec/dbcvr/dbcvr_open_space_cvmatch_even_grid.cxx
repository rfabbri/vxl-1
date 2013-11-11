#include "dbcvr_open_space_cvmatch_even_grid.h"

dbcvr_open_space_cvmatch_even_grid::
dbcvr_open_space_cvmatch_even_grid(dbsol_interp_curve_3d *c1, dbsol_interp_curve_3d *c2, 
                                   int grid_w, int grid_h, 
                                   double R1, double R2,
                                   char *cost_formula_comput_type,
                                   char *angle_der_comput_type,
                                   int template_size
                                   )
{
  c1_ = c1;
  c2_ = c2;
  grid_w_ = grid_w;
  grid_h_ = grid_h;
  R1_ = R1;
  R2_ = R2;
  cfct_ = cost_formula_comput_type;
  adct_ = angle_der_comput_type;
  template_size_ = template_size;
}

vcl_pair<double, FinalMapType*> dbcvr_open_space_cvmatch_even_grid::match()
{
  // create cost object
    dbcvr_open_space_cvmatch_even_grid_cost cost_fnc(c1_, c2_, 
                                                     grid_w_, grid_h_, 
                                                     R1_, R2_, 
                                                     cfct_, adct_
                                                     );
    cost_fnc.compute_properties();
    // create dynamic programming object
    dp_engine_ = new dbdp_engine(grid_w_, grid_h_, template_size_, &cost_fnc);
    // run DP
    dp_engine_->RunDP();

    FinalMapType* fmap = dp_engine_->finalMap();
    double cost = dp_engine_->finalCost();

    vcl_pair<double, FinalMapType*> match_result;
    match_result.first = cost;
    match_result.second = fmap;

    return match_result;
}
