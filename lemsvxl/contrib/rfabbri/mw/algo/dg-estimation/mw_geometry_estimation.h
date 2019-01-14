// This is mw_geometry_estimation.h
#ifndef mw_geometry_estimation_h
#define mw_geometry_estimation_h
//:
//\file
//\brief Estimation of differential geometry by many methods
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@gmail.com)
//\date Wed May 09 21:00:07 EDT 2006
//

#include <bmcsd/bmcsd_util.h>

#include <vsol/vsol_line_2d_sptr.h>
#include <dbdet/sel/dbdet_sel_sptr.h>
class dbdet_edgel;

class mw_geometry_estimation {
  public:

  static bool 
  differential_geometry_at(dbdet_edgel *e1, std::vector<double> &k, std::vector<double> &kdot);

  static void
  sel_quad(
      std::vector<vsol_line_2d_sptr> &lines,
      unsigned  nrows,
      unsigned  ncols,
      unsigned  nrad,
      double    dtheta,
      double    dpos,
      std::vector<std::vector<double> > &k, 
      std::vector<std::vector<double> > &kdot,
      std::vector<std::vector<double> > &k_stdv, 
      std::vector<std::vector<double> > &kdot_stdv,
      dbdet_sel_sptr &sel
      );

  static void 
  sel_quad(
      const dbdet_sel_sptr &sel,
      std::vector<std::vector<double> > &k, 
      std::vector<std::vector<double> > &kdot,
      std::vector<std::vector<double> > &k_stdv, 
      std::vector<std::vector<double> > &kdot_stdv
      );
};

#endif // mw_geometry_estimation_h

