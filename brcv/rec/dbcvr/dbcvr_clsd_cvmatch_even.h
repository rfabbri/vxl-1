//---------------------------------------------------------------------
// This is brcv/rec/dbcvr/dbcvr_clsd_cvmatch_even.h
//:
// \file
// \brief closed curve matching using even sampling on both of the curves
//
//
// \author
//  O.C. Ozcanli - April 27, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#ifndef _dbcvr_clsd_cvmatch_even_h
#define _dbcvr_clsd_cvmatch_even_h

#include <vcl_ctime.h>
#include <dbsol/dbsol_interp_curve_2d_sptr.h>

#include "dbcvr_cvmatch.h"

class dbcvr_clsd_cvmatch_even : public vbl_ref_count
{

protected:
  //Data
  dbsol_interp_curve_2d_sptr _curve1;
  dbsol_interp_curve_2d_sptr _curve2;

  DPCostType _cost; //Temporary array for each match (2_n x _m)
  vcl_vector<int> _leftMask;
  vcl_vector<int> _rightMask;

  DPMapType _map; //Temporary array for each match (2_n x _m)
  DPMapType _finalMap; //Shortest Path for each start point (_n+1 x 1)
  vcl_vector<double> _finalCost; //Final cost for each start point (_n+1 x 1)

  //: compute tangents and arclengths at each sample on the curves
  vcl_vector<double>  _tangents_curve1;
  vcl_vector<double>  _tangents_curve2;

  vcl_vector<double>  _lengths_curve1;
  vcl_vector<double>  _lengths_curve2;

  int _n1;
  int _n2;
  double _R;
  bool _normalized_stretch_cost;
  int _template_size;
  vcl_vector<int> XOFFSET;
  vcl_vector<int> YOFFSET;

  double L1, L2;
  double _delta_s1, _delta_s2;

  //Functions
  
  //: compute arclengths and tangents at even samples of both curves to avoid their recomputation 
  //  at each interval cost computation
  void computeCurveProperties();

  void initializeDPMask1();
  void initializeDPMask2(int s1, int s2);
  void findOptimalPath(int startPoint);
  void computeMiddlePaths(int i, int j);
  void computeDPCosts(int startPoint);
  void findDPCorrespondence(int startPoint);
  virtual double computeIntervalCost(int i, int ip, int j, int jp);

public:
  dbcvr_clsd_cvmatch_even();
  dbcvr_clsd_cvmatch_even(const dbsol_interp_curve_2d_sptr c1, 
                          const dbsol_interp_curve_2d_sptr c2, 
                          int n1,
                          int n2,
                          double R, 
                          int template_size);   
  virtual ~dbcvr_clsd_cvmatch_even(){};
  
  //access functions
  double finalCost(int index){return _finalCost[index];};
  vcl_vector<double> finalCost(){return _finalCost;};
  FinalMapType finalMap(int index){return _finalMap[index];};
  int n1(){return _n1;};
  int n2(){return _n2;};

  void setStretchCostFlag (bool flag) {
        _normalized_stretch_cost = flag;
  }
  void setTemplateSize (int size);

  //display functions (debug)
  void printCost();
  void writeCost(vcl_string f);
  void printMap();
  
  // Match() corresponds to closedCurveDPMatch(ClosedDPMatch *d) in original source code
  
  void Match();  

  dbsol_interp_curve_2d_sptr curve1() { return _curve1; }
  dbsol_interp_curve_2d_sptr curve2() { return _curve2; }

};

#endif
