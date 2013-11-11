#ifndef _dbcvr_open_2d_cvmatch_even_grid_h
#define _dbcvr_open_2d_cvmatch_even_grid_h
//---------------------------------------------------------------------
// This is brcv/rec/dbcvr/dbcvr_open_2d_cvmatch_even_grid.h
//:
// \file
// \brief This class matches two open 2d curves using an even DP grid
//        The curves (c1 and c2) have to be constructed as 
//        dbsol_interp_curve_2d objects.
//        See dbcvr_open_2d_cvmatch_even_grid_example.cxx for an 
//        example of using this class.
//
// \author H. Can Aras (can@lems.brown.edu)
// \date 2007-01-31
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//----------------------------------------------------------------------

#include <dbsol/dbsol_interp_curve_2d.h>
#include <dbcvr/dbcvr_open_2d_cvmatch_even_grid_cost.h>
#include <dbdp/dbdp_engine.h>

class dbcvr_open_2d_cvmatch_even_grid
{
  //functions
public:
  //: c1, c2 : curves
  //  grid_w, grid_h : dynamic programming grid sizes (also the number of samples taken
  //  on c1 and c2, respectively)
  //  R1: bending cost weight coefficient
  //  template_size : for dynamic programming algorithm
  dbcvr_open_2d_cvmatch_even_grid(dbsol_interp_curve_2d *c1, dbsol_interp_curve_2d *c2, 
                                  int grid_w, int grid_h, 
                                  double R1 = 0.7,
                                  int template_size = 3);
  virtual ~dbcvr_open_2d_cvmatch_even_grid() {};

  // typedef vcl_vector< vcl_pair<int,int> > FinalMapType; (from dbdp_engine.h)
  vcl_pair<double, FinalMapType*> match();
protected:

  //variables
public:

protected:
  // pointers to curves
  dbsol_interp_curve_2d *c1_;
  dbsol_interp_curve_2d *c2_;
  // number of sampling points on each curve
  // also determines the DP grid size
  int grid_w_, grid_h_;
  // cost weight parameter for bending
  double R1_;
  // template size for the dynamic programming
  int template_size_;
  // pointer to the dynamic programming object
  dbdp_engine *dp_engine_;
};

#endif

