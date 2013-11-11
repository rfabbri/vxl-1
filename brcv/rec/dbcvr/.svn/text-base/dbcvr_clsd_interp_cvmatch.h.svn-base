//---------------------------------------------------------------------
// This is brcv/rec/dbcvr/dbcvr_clsd_interp_cvmatch.h
//:
// \file
// \brief closed curve matching independent from the sampling along input curves.
//
// \author
//  O.C. Ozcanli - December 03, 2004
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

//        cost of alignment curve is found using a new coordinate system found
//        via advancing equal step sizes along the minimum alignment curve 
//        cost of matching is found for each point on this new grid
//        samples along curves are interpolated based on arc length
// 
//        Assumptions:
//        Input curves are closed, i.e. curve->point_at(0) = curve->point_at(L)
//                                      for both curves
//                                 but curve->tangent_angle_at(0) != curve->tangent_angle_at(L)
//                                     is acceptable 
//                                 will use curve->tangent_angle_at(L)
//                                 if tangent at 0 is asked as a convention
//                                 (because according to the assumptions made  in
//                                  dbsol_interp_curve_2d class data points belong to preceding intervals)
//       c2
//
//       ^
//       |
// L2    -----|
//       |  / |
//       |/   |
//       -----'-->  c1           keep c2 as it is and try different starting points on c1
//    (0,0)   L1
//
//  IMPORTANT ASSUMPTION: Shorter curve is c1

#ifndef _dbcvr_clsd_interp_cvmatch_h
#define _dbcvr_clsd_interp_cvmatch_h

#include <vbl/vbl_ref_count.h>
#include <vcl_vector.h>
#include <vcl_utility.h>

#include <vsol/vsol_point_2d_sptr.h>

#include <dbsol/dbsol_interp_curve_2d_sptr.h>

#include <dbcvr/dbcvr_interp_cvmatch.h>

// dynamic programming table cell
struct dptcell
{
  double cost;
  vcl_pair<int, int> prev_cell_coord;  // to trace back alignment curve when finished
  double s1;
  double s2;
  double theta1;   // store tangent angle of first curve at this cell, required for fast computation of bending cost
  double theta2;   // store tangent angle of second curve at this cell
  bool valid; // nonvalid if out of search space for a given starting point
};
typedef struct dptcell dptcell;


class dbcvr_clsd_interp_cvmatch : public vbl_ref_count 
{

protected:
  //Data
  dbsol_interp_curve_2d_sptr _curve1;
  dbsol_interp_curve_2d_sptr _curve2;

  //: cells up for positive eta
  //  cells down for negative eta,  we need two cost arrays since we cannot use negative indices.
  //  CU[i][j]  i runs along diogonal
  //            j runs along axis perpendicular to the diagonal
  vcl_vector< vcl_vector<dptcell> > CU; //cells up, cost array, positive indices
  vcl_vector< vcl_vector<dptcell> > CD; //cells down, cost array, negative indices
  
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

  int k_;   // number of starting points
  vcl_vector< vcl_pair<int, int> > start_cells_; // vector with size k, holding starting points on the grid
  vcl_vector< vcl_pair<int, int> > end_cells_; // vector with size k, holding end points
  vcl_vector< vcl_vector <int> > paths_;   // for each starting point k, there is a path such that
                                           // paths_[k][i] shows on which j is the alignment curve passing
                                           // j negative for CD, if that i is irrelevant for a given
                                           // starting point, then the value should never be checked,
                                           // so it can be anything, will just put 0 in that case.

  vcl_vector<double> final_cost_;
  int k_min_;

  // to construct bsol_intrinsic_curve_2d(const vcl_vector<vsol_point_2d_sptr> &new_vertices);
  vcl_vector<vsol_point_2d_sptr> out_curve1_;   // best alignment's points along 1st curve
  vcl_vector<vsol_point_2d_sptr> out_curve2_;   // best alignment's points along 2nd curve
  
  //Functions
  //: construct the grid 
  void construct();   
  //: initialize matching 
  void initialize(int k, int k_left, int k_right);  

  //: find optimum path starting from given start point and in the space
  //  restricted to the region in between paths_[k_left] and paths_[k_right]
  //  this restriction is achieved via initialization of the costs in the grid
  //  via initialize function, so this function should be called
  //  appropriately a priori to the find_path function
  void find_path(int k);

  //: update the cost of the cell, as if coming from cell: c_prev 
  void update_cost(dptcell *c, dptcell *c_prev, int i_prev, int j_prev);

  //: alignment curves starting at s1 and s2 on curve 2, find middle alignment curve
  void computeMiddlePaths(int k_left, int k_right);  
  
  void come_previous(dptcell *c, int i, int j);

  double get_s1(double s);
  double get_s2(double s);

public:
  dbcvr_clsd_interp_cvmatch() {}
  dbcvr_clsd_interp_cvmatch(dbsol_interp_curve_2d_sptr c1, 
                            dbsol_interp_curve_2d_sptr c2, 
                            double R, 
                            double delta_ksi, double delta_eta);   // only the curves and the original number of samples are given
  virtual ~dbcvr_clsd_interp_cvmatch(){};
  
  void Match();

  //: return the cost of optimum alignment corresponding to kth starting point
  double finalCost(int k);
  double finalCostMin() { assert(k_min_ >= 0 && k_min_ < k_);
                          return final_cost_[k_min_]; }

  //void printCosts(int m);
  //void setCostFlag(bool flag) { _cost_flag = flag; }

  //: return number of starting points
  int k() { return k_; }

  dbsol_interp_curve_2d_sptr curve1() { return _curve1; }
  dbsol_interp_curve_2d_sptr curve2() { return _curve2; }

  vcl_vector<vsol_point_2d_sptr> output_curve1() { return out_curve1_; }
  vcl_vector<vsol_point_2d_sptr> output_curve2() { return out_curve2_; }
};

#endif // _dbcvr_clsd_interp_cvmatch_h
