#include "mw_test_sel_geometry.h"
#include <mw/algo/mw_geometry_estimation.h>
#include <dbdet/sel/dbdet_sel_sptr.h>
#include <mw/mw_analytic.h>
#include <vsol/vsol_line_2d.h>
#include <vcl_algorithm.h>
#include <mw/algo/mw_data.h> // for get_lines


void mw_test_sel_geometry::
test_circle(
      double rad_ini, 
      double rad_end, 
      unsigned  nrad, 
      double dpos,
      double dtheta,
      bool do_perturb,
      double pert_pos,
      double pert_tan
)
{
  for (double radius=rad_ini; radius <= rad_end; radius += 1.0) {

    // - create circle edgels (function)
    vcl_vector<vsol_line_2d_sptr> lines;
    vcl_vector<dbdif_3rd_order_point_2d> crv;
    get_circle_edgels(radius, lines, crv, do_perturb, pert_pos, pert_tan);
    
    // k[i][j] == curvature at edgel i, valid quad number j
    vcl_vector<vcl_vector<double> > k, kdot;
    vcl_vector<vcl_vector<double> > k_stdv, kdot_stdv;

    dbdet_sel_sptr sel;
    unsigned  nrows, ncols;
    nrows = ncols  = (unsigned) (2*vcl_ceil(radius)+1);
    mw_geometry_estimation::sel_quad(
        lines, 
        nrows,
        ncols,
        nrad,
        dtheta,
        dpos,
        k, kdot, k_stdv, kdot_stdv, sel);

    double k_err, kdot_err;
    double k_mean;
    double k_stdev_nopoly; //:< disconsidering k_stdv, kdot_stddv
    double kdot_mean;
    double kdot_stdev_nopoly; //:< disconsidering k_stdv, kdot_stddv
    unsigned  nquads_pass;

    compute_circle_stats( crv, k, kdot, k_stdv, kdot_stdv,
        k_err, k_mean, k_stdev_nopoly, kdot_err, kdot_mean, kdot_stdev_nopoly, nquads_pass);

    vcl_cout << "------------------\n";
    vcl_cout << "Circle radius: " << radius << " (k = " << 1.0/radius << ") " 
      << ",      # quads used: " << nquads_pass << vcl_endl;
    vcl_cout << "k - Global error: " << k_err << " avg: " << k_mean << " stddev: " << k_stdev_nopoly << vcl_endl;
    vcl_cout << "kdot - Global error: " << kdot_err << " avg: " << kdot_mean << " stddev: " << kdot_stdev_nopoly << vcl_endl;
  }

  // - final output:
  //    - vector of error btw kmean and ground-truth
  //    - vector of error btw kdot mean and ground-truth
  //    - what radius gave the largest error
  //    - what radius gave the least error
}

void mw_test_sel_geometry::
compute_circle_stats(
  const vcl_vector<dbdif_3rd_order_point_2d> &crv,
  const vcl_vector<vcl_vector<double> > &k, 
  const vcl_vector<vcl_vector<double> > &kdot,
  const vcl_vector<vcl_vector<double> > &/*k_stdv*/, 
  const vcl_vector<vcl_vector<double> > &/*kdot_stdv*/,
  double &k_err,
  double &k_mean,
  double &k_stddev_nopoly,//:< disconsidering k_stdv, kdot_stddv
  double &kdot_err,
  double &kdot_mean,
  double &kdot_stddev_nopoly, //:< disconsidering k_stdv, kdot_stddv
  unsigned  &nquads_pass
  )
{
  //: global measure: mean, stddev of k
  //: global measure: mean, stddev of kdot

  {
    k_mean = 0; unsigned  n=0; k_err=0; unsigned  n_print=0;
    for (unsigned  i=0; i<k.size(); ++i) {
      for (unsigned  j=0; j<k[i].size(); ++j) {
        k_mean += k[i][j];
        ++n;

        k_err += vcl_fabs(k[i][j] - crv[i].k);
        if (j ==1 && vcl_fabs(k[i][0] - k[i][1]) > 1e-3 && n_print < 10) {
          ++n_print;
          vcl_cout << "Different k's --" << " id: " << i+1 << " k_left: " << k[i][0] << " k_right: " << k[i][1] << vcl_endl;
        }       
      }
    }
    if (n_print) {
      if (n_print == 10)
        vcl_cout << "...\n";
      vcl_cout << vcl_endl;
    }

    assert(n != 0);
    k_mean /= n;
    k_err /= n;

    k_stddev_nopoly=0;
    for (unsigned  i=0; i<k.size(); ++i) {
      for (unsigned  j=0; j<k[i].size(); ++j) {
        double dk = k[i][j] - k_mean;
        k_stddev_nopoly += dk*dk;
      }
    }

    k_stddev_nopoly = sqrt(k_stddev_nopoly/(n-1));
    nquads_pass = n;
  }

  {
    // --------- KDOT -------
    kdot_mean = 0; unsigned  n=0; kdot_err=0;
    for (unsigned  i=0; i<kdot.size(); ++i) {
      for (unsigned  j=0; j<kdot[i].size(); ++j) {
        kdot_mean += kdot[i][j];
        ++n;

        kdot_err += vcl_fabs(kdot[i][j] - crv[i].kdot);
      }
    }
    assert(n != 0);
    kdot_mean /= n;
    kdot_err /= n;

    kdot_stddev_nopoly=0;
    for (unsigned  i=0; i<kdot.size(); ++i) {
      for (unsigned  j=0; j<kdot[i].size(); ++j) {
        double dkdot = kdot[i][j] - kdot_mean;
        kdot_stddev_nopoly += dkdot*dkdot;
      }
    }

    kdot_stddev_nopoly = sqrt(kdot_stddev_nopoly/(n-1));
    assert(nquads_pass == n);
  }
}
