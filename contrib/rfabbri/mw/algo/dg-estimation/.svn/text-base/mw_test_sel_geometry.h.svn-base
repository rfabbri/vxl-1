// This is mw_test_sel_geometry.h
#ifndef mw_test_sel_geometry_h
#define mw_test_sel_geometry_h
//:
//\file
//\brief 
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date 05/09/2006 10:11:31 PM EDT
//

#include <mw/mw_util.h>
#include <dbdif/dbdif_frenet.h>
#include <vsol/vsol_line_2d_sptr.h>

//: Class to test the precision of differential-geometric estimations from Tamrakkar's symbolic edge linker.
class mw_test_sel_geometry {
public:
  static void test_circle(
      double rad_ini, 
      double rad_end, 
      unsigned  nrad, 
      double dpos,
      double dtheta,
      bool do_perturb=false,
      double pert_pos=0.1,
      double pert_tan=10
   );

  static void compute_circle_stats(
  const vcl_vector<dbdif_3rd_order_point_2d> &crv,
  const vcl_vector<vcl_vector<double> > &k, 
  const vcl_vector<vcl_vector<double> > &kdot,
  const vcl_vector<vcl_vector<double> > &k_stdv, 
  const vcl_vector<vcl_vector<double> > &kdot_stdv,
  double &k_err,
  double &k_mean,
  double &k_stddev_nopoly , //:< disconsidering k_stdv, kdot_stddv
  double &kdot_err,
  double &kdot_mean,
  double &kdot_stddev_nopoly, //:< disconsidering k_stdv, kdot_stddv
  unsigned  &nquads_pass
  );
};

#endif // mw_test_sel_geometry_h

