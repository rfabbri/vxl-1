#ifndef _dbcvr_cvmatch_even_3d_h
#define _dbcvr_cvmatch_even_3d_h
//---------------------------------------------------------------------
// This is brcv/rec/dbcvr/dbcvr_cvmatch_even_3d.h
//:
// \file
// \brief Open curve matching with even spacing on both curves 
//        (input curve class is interp_curve_3d)
//
// \author
//  H. Can Aras (can@lems.brown.edu)
//
// \verbatim
//
// \endverbatim
//
//-------------------------------------------------------------------------

#include <dbsol/dbsol_interp_curve_3d_sptr.h>
#include "dbcvr_cvmatch.h"
#include "dbcvr_cvmatch_even_3d.h"

#include <vbl/vbl_ref_count.h>

// EXTRINSIC/INTRINSIC keyword tells the program to use 
// extrinsic/intrinsic cost formulation, respectively
enum {EXTRINSIC, INTRINSIC};
// USE_ENO/USE_FORMULAS keyword tells the program to use 
// ENO/differential geometry formulas to compute the first derivatives of the angles
enum {USE_ENO, USE_FORMULAS};

class dbcvr_cvmatch_even_3d : public vbl_ref_count
{
protected:
  // curves
  dbsol_interp_curve_3d *curve1_;
  dbsol_interp_curve_3d *curve2_;
  // number of sample points
  int n1_;
  int n2_;
  // sampling step size
  double delta_s1_;
  double delta_s2_;

  DPCostType DPCost_; //DPMap of cost: n*m array of double
  DPMapType DPMap_; //DPMap of prev point vcl_map: n*m array of vcl_pair of index
  FinalMapType finalMap_; //alignment curve
  vcl_vector<double> finalMap_Cost_; //cost on alignment curve
  
  // curvature at each sampling point
  vcl_vector<double>  curve1_curvatures_;
  vcl_vector<double>  curve2_curvatures_;
  // torsion at each sampling point
  vcl_vector<double>  curve1_torsions_;
  vcl_vector<double>  curve2_torsions_;
  // arclength at each sampling point
  vcl_vector<double>  curve1_lengths_;
  vcl_vector<double>  curve2_lengths_;
  // phi (tilt angle of the tangent vector) at each sampling point
  vcl_vector<double>  curve1_phis_;
  vcl_vector<double>  curve2_phis_;
  // theta (azimuth angle of the tangent vector) at each sampling point
  vcl_vector<double>  curve1_thetas_;
  vcl_vector<double>  curve2_thetas_;
  // dphi at each sample point (derivative w.r.t. to arclength)
  vcl_vector<double>  curve1_dphis_;
  vcl_vector<double>  curve2_dphis_;
  // dtheta at each sample point (derivative w.r.t. to arclength)
  vcl_vector<double>  curve1_dthetas_;
  vcl_vector<double>  curve2_dthetas_;

  double finalCost_; // final cost

  // R1_/R2_ are the bending/twisting cost weights, respectively
  double R1_, R2_;

  bool normalized_stretch_cost_;
  int template_size_;
  vcl_vector<int> XOFFSET;
  vcl_vector<int> YOFFSET;

  // cost formula type to use: extrinsic or intrinsic
  int cost_formula_type_;
  // angle derivatives computation technique: using eno or formulas
  int ang_der_comp_type_;

public:

  // Result of matching
  DPCostType* DPCost(){ return &DPCost_; }
  DPMapType* DPMap(){ return &DPMap_; }
  FinalMapType* finalMap(){ return &finalMap_; }
  vcl_vector<double>* finalMapCost(){ return &finalMap_Cost_; }
  double finalCost(){ return finalCost_; }

  int getFMapFirst (int i)
  {
    assert (i>-1);
    return (*finalMap())[i].first;
  }
  int getFMapSecond (int i) 
  {
    assert (i>-1);
    return (*finalMap())[i].second;
  }

  vcl_string    _fileName1, _fileName2;
  dbsol_interp_curve_3d_sptr curve1() { return curve1_; }
  dbsol_interp_curve_3d_sptr curve2() { return curve2_; }
  void setFinalMap (FinalMapType map) { finalMap_ = map; }
  void setFinalMap (int i, int first, int second) 
  {
    finalMap_[i].first = first;
    finalMap_[i].second = second;
  }
  void setStretchCostFlag(bool flag) { normalized_stretch_cost_ = flag; }
  void setTemplateSize(int size);

  dbcvr_cvmatch_even_3d();
  dbcvr_cvmatch_even_3d(dbsol_interp_curve_3d *c1, 
                        dbsol_interp_curve_3d *c2, 
                        int n1, int n2, 
                        double R1, double R2, 
                        int template_size, int formula_type,
                        int ang_der_comp_type);

  virtual ~dbcvr_cvmatch_even_3d();

  virtual void initializeDPCosts();
  virtual double computeIntervalCost(int i, int ip, int j, int jp);

  void computeDPCosts();
  void findDPCorrespondence();

  void Match();

  // helper function for applying ENO on angles
  // "picked_points/picked_arclengths" are picked from "points/arclengths" arrays, respectively,
  // based on the index value
  int pick_points_for_eno(vcl_vector<double> &points, vcl_vector<double> &picked_points, 
                          vcl_vector<double> &arclengths, vcl_vector<double> &picked_arclengths,
                          int index);
  // removes discontinuties of the specified array holding angle values
  void continuous_angles(vcl_vector<double> &angles);
  // writes "data" to the file named "fname" in plain text format
  void write_data(vcl_vector<double> &data, vcl_string fname);
};

#endif

