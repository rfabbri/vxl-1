#ifndef _dbcvr_open_space_cvmatch_even_grid_h
#define _dbcvr_open_space_cvmatch_even_grid_h
//---------------------------------------------------------------------
// This is brcv/rec/dbcvr/dbcvr_open_space_cvmatch_even_grid.h
//:
// \file
// \brief This class matches two open space curves using an even DP grid
//        The curves (c1 and c2) have to be constructed as 
//        dbsol_interp_curve_3d objects.
//        See dbcvr_open_space_cvmatch_even_grid_example.cxx for an 
//        example of using this class.
//
// \author H. Can Aras (can@lems.brown.edu)
// \date 2007-01-16
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//----------------------------------------------------------------------

#include <dbsol/dbsol_interp_curve_3d.h>
#include <dbcvr/dbcvr_open_space_cvmatch_even_grid_cost.h>
#include <dbdp/dbdp_engine.h>

class dbcvr_open_space_cvmatch_even_grid
{
  //functions
public:
  //: c1, c2 : curves
  //  grid_w, grid_h : dynamic programming grid sizes (also the number of samples taken
  //  on c1 and c2, respectively)
  //  R1, R2 : cost weight coefficients (R1 for bending cost, R2 for twisting cost)
  //  template_size : for dynamic programming algorithm
  dbcvr_open_space_cvmatch_even_grid(dbsol_interp_curve_3d *c1, dbsol_interp_curve_3d *c2, 
                                     int grid_w, int grid_h, 
                                     double R1 = 0.7, double R2 = 0.07,
                                     char *cost_formula_comput_type = "explicit",
                                     char *angle_der_comput_type = NULL,
                                     int template_size = 3);
  virtual ~dbcvr_open_space_cvmatch_even_grid() {};

  // typedef vcl_vector< vcl_pair<int,int> > FinalMapType; (from dbdp_engine.h)
  vcl_pair<double, FinalMapType*> match();
protected:

  //variables
public:

protected:
  // pointers to curves
  dbsol_interp_curve_3d *c1_;
  dbsol_interp_curve_3d *c2_;
  // number of sampling points on each curve
  // also determines the DP grid size
  int grid_w_, grid_h_;
  // cost weight parameters for bending and twisting
  double R1_, R2_;
  // cost formula computation type, case-sensitive
  char *cfct_;
  // angle derivative computation type (only when cfct_ = "implicit"), case-sensitive
  char *adct_;
  // template size for the dynamic programming
  int template_size_;
  // pointer to the dynamic programming object
  dbdp_engine *dp_engine_;
};

#endif

