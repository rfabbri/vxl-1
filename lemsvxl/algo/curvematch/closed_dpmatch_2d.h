//---------------------------------------------------------------------
// This is algo/curvematching/closed_dpmatch_2d.h
//:
// \file
// \brief closed curve matching adapted from original source code for Windows in
//        /vision/projects/kimia/curve-matching/CurveMatch/CODE/CODE-IRIX6.2/closed-match/CodeWin
//
//        bsol_intrinsic_curve_2d class is used to represent curves, thus the cost computations
//        for stretch cost is different from the original code in the above 
//        directory. (The difference is noted in the related section below.)
//
// \author
//  O.C. Ozcanli - October 11, 2004
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#ifndef _closed_dpmatch_2d_h
#define _closed_dpmatch_2d_h

#include <vcl_ctime.h>

#include "curve_dpmatch_2d.h"

class closed_dpmatch_2d : public vbl_ref_count 
{
protected:
  //Data
  bsol_intrinsic_curve_2d_sptr _curve1;
  bsol_intrinsic_curve_2d_sptr _curve2;

  DPCostType _cost; //Temporary array for each match (2_n x _m)
  vcl_vector<int> _leftMask;
  vcl_vector<int> _rightMask;

  DPMapType _map; //Temporary array for each match (2_n x _m)
  DPMapType _finalMap; //Shortest Path for each start point (_n+1 x 1)
  vcl_vector<double> _finalCost; //Final cost for each start point (_n+1 x 1)
  DPCostType _finalMapCost;
  int _n;
  int _m;
  
  //Functions
  void initializeDPMask(int i, int j);
  void initializeDPMask(int startPoint);
  void findOptimalPath(int startPoint);
  void computeMiddlePaths(int i, int j);

  void computeDPCosts(int startPoint);
  void findDPCorrespondence(int startPoint);

  virtual double computeIntervalCost(int i, int ip, int j, int jp);

  // original: pow(_arcLength[i]-_arcLength[ip],2)/(_arcLength[i]+_arcLength[ip])
  double stretchCost (bsol_intrinsic_curve_2d_sptr curve, int i, int ip) {
                return curve->arcLength(i) - curve->arcLength(ip);
        }
        double bendCost (bsol_intrinsic_curve_2d_sptr curve, int i, int ip) {
                return curve_angleDiff (curve->angle(i), curve->angle(ip));
        }

public:
  closed_dpmatch_2d();
  closed_dpmatch_2d(bsol_intrinsic_curve_2d_sptr c1, bsol_intrinsic_curve_2d_sptr c2);
  virtual ~closed_dpmatch_2d(){};
  
  //access functions
  double finalCost(int index){return _finalCost[index];};
  vcl_vector<double> finalCost(){return _finalCost;};
  FinalMapType finalMap(int index){return _finalMap[index];};
  vcl_vector<double> finalMapCost(int index){return _finalMapCost[index];};
  int n(){return _n;};
  int m(){return _m;};

  //display functions (debug)
  void printCost();
  void writeCost(vcl_string f);
  void printMap();
  //void printFinalMap();
  
  void Match();
  void Match(int startPoint);

  bsol_intrinsic_curve_2d_sptr curve1() { return _curve1; }
        bsol_intrinsic_curve_2d_sptr curve2() { return _curve2; }
};

class closednew_dpmatch_2d: public closed_dpmatch_2d 
{
 public:
  closednew_dpmatch_2d();
  closednew_dpmatch_2d(bsol_intrinsic_curve_2d_sptr c1, bsol_intrinsic_curve_2d_sptr c2);
  closednew_dpmatch_2d(bsol_intrinsic_curve_2d_sptr c1, bsol_intrinsic_curve_2d_sptr c2,
                       double R1, double R2, double lambda, int numLenElems);
  ~closednew_dpmatch_2d(){};

 protected:
  //Data
  int _numLenElems;
  double _R1;
  double _R2;
  //vcl_vector<double> _lambda;
  double _lambda;

  //Functions
  double computeIntervalCost(int i, int ip, int j, int jp);
};

#endif
