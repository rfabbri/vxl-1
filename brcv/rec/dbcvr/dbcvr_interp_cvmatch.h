//---------------------------------------------------------------------
// This is brcv/rec/dbcvr/dbcvr_interp_cvmatch.h
//:
// \file
// \brief open curve matching independent from the sampling along input curves.
//        cost of alignment curve is found using a new coordinate system found
//        via advancing equal step sizes along the minimum alignment curve 
//        cost of matching is found for each point on this new grid
//        samples along curves are interpolated based on arc length
//
// \author
//  O.C. Ozcanli - December 03, 2004
//
// \verbatim
//  Modifications
//    O.C. Ozcanli - May 16, 2005
//   
// \endverbatim
//
//-------------------------------------------------------------------------

#ifndef _dbcvr_interp_cvmatch_h
#define _dbcvr_interp_cvmatch_h

#include <vbl/vbl_ref_count.h>
#include <vcl_vector.h>
#include <vcl_utility.h>
#include <assert.h>

#include <vsol/vsol_point_2d_sptr.h>
#include <dbsol/dbsol_interp_curve_2d_sptr.h>

// dynamic programming table cell
struct dpt_cell
{
  double cost;
  vcl_pair<int, int> prev_cell_coord;  // to trace back alignment curve when finished
  double s1;
  double s2;
  double theta1;   // store tangent angle of first curve at this cell, required for fast computation of bending cost
  double theta2;   // store tangent angle of second curve at this cell
  bool valid;    // nonvalid if out of search space
};
typedef struct dpt_cell dpt_cell;


class dbcvr_interp_cvmatch : public vbl_ref_count 
{

protected:
  //Data
  dbsol_interp_curve_2d_sptr _curve1;
  dbsol_interp_curve_2d_sptr _curve2;

  //: cells up for positive eta
  //  cells down for negative eta,  we need two cost arrays since we cannot use negative indices.
  //  CU[i][j]  i runs along diogonal
  //            j runs along axis perpendicular to the diagonal
  vcl_vector< vcl_vector<dpt_cell> > CU; //cells up, cost array, positive indices
  vcl_vector< vcl_vector<dpt_cell> > CD; //cells down, cost array, negative indices
  
  double _delta;              // step size along the min length alignment curve
  double _delta_eta;          // step size along the axis orthogonal to min length alignment curve axis
  double _R;
  bool _cost_flag;
  double _cost_threshold;   // cost of min length alignment curve
  double _alpha;            // angle of min length alignment curve (will be pi/4 for same length curves)
  
  int max_i_;                   // number of steps along max diagonal, depends on _delta
  int max_j_up_;
  int max_j_down_;

  double ca;
  double sa;
  double ta;
  double L1; 
  double L2;

  // to construct bsol_intrinsic_curve_2d(const vcl_vector<vsol_point_2d_sptr> &new_vertices);
  vcl_vector<vsol_point_2d_sptr> out_curve1_;   // best alignment's points along 1st curve
  vcl_vector<vsol_point_2d_sptr> out_curve2_;   // best alignment's points along 2nd curve
  
  //Functions
  //: construct the grid 
  void construct();   

  //: find optimum path 
  void find_path();

  //: update the cost of the cell, as if coming from cell: c_prev 
  void update_cost(dpt_cell *c, dpt_cell *c_prev, int i_prev, int j_prev);

  void come_previous(dpt_cell *c, int i, int j);

public:
  dbcvr_interp_cvmatch() {}
  dbcvr_interp_cvmatch(dbsol_interp_curve_2d_sptr c1, 
                       dbsol_interp_curve_2d_sptr c2, 
                       double R, 
                       double delta_ksi, double delta_eta);   // only the curves and the original number of samples are given
  virtual ~dbcvr_interp_cvmatch() {};
  
  void Match();

  //: return the cost of optimum alignment corresponding to kth starting point
  double finalCost();
  
  dbsol_interp_curve_2d_sptr curve1() { return _curve1; }
  dbsol_interp_curve_2d_sptr curve2() { return _curve2; }

  vcl_vector<vsol_point_2d_sptr> output_curve1() { return out_curve1_; }
  vcl_vector<vsol_point_2d_sptr> output_curve2() { return out_curve2_; }
};

#endif // _dbcvr_interp_cvmatch_h
