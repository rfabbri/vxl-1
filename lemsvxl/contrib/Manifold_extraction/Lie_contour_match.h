//---------------------------------------------------------------------
// \file
// \brief closed curve matching using Lie distance notion from even sampling on both of the curves
//
//
// \author
//  Pradeep- June 2007
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#ifndef _Lie_contour_match_h
#define _Lie_contour_match_h

#include <vcl_ctime.h>
#include <dbsol/dbsol_interp_curve_2d_sptr.h>

#include <vbl/vbl_ref_count.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_utility.h>
#define DP_VERY_LARGE_COST 1E10

//value in (-Pi,Pi]
double Lie_curve_fixAngleMPiPi (double a);

//Does a1-a2, value in (-Pi,Pi]
double Lie_curve_angleDiff (double a1, double a2);

double  Lie_curve_maxof (double a, double b, double c);

typedef vcl_vector< vcl_vector<double> >            DPCostType;
typedef vcl_vector< vcl_vector< vcl_pair <int,int> > >  DPMapType;
typedef vcl_vector< vcl_pair<int,int> >            FinalMapType;

class Lie_contour_match : public vbl_ref_count
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
  
  //: compute arclengths and tangents at even samples of both curves 
  void computeCurveProperties();

  void initializeDPMask1();
  void initializeDPMask2(int s1, int s2);
  void findOptimalPath(int startPoint);
  void computeMiddlePaths(int i, int j);
  void computeDPCosts(int startPoint);
  void findDPCorrespondence(int startPoint);
 

public:
  Lie_contour_match();
  Lie_contour_match(const dbsol_interp_curve_2d_sptr c1, 
                          const dbsol_interp_curve_2d_sptr c2, 
                          int n1,
                          int n2,
                          double R, 
                          int template_size);   
  virtual ~Lie_contour_match(){};
  
   double computeLieIntervalCost(int i, int ip, int j, int jp);

  //access functions
  double finalCost(int index){return _finalCost[index];};

  DPCostType DPcost(){return _cost;};
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

    void setCurve1 (const dbsol_interp_curve_2d_sptr& c1) {
    _curve1 = c1;
  }

 void setCurve2 (const dbsol_interp_curve_2d_sptr& c2) {
    _curve2 = c2;
  }

};

#endif

