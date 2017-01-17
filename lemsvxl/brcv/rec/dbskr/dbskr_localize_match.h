//---------------------------------------------------------------------
// This is brcv/rec/dbskr/dbskr_localize_match.h
//:
// \file
// \brief This class recomputes matching cost of densely sampled versions of curve1 and curve2
//
// \author
//  O.C. Ozcanli - October 04, 2005
//
// \verbatim
//   
// \endverbatim
//
//-------------------------------------------------------------------------
#ifndef _dbskr_localize_match_h
#define _dbskr_localize_match_h

#include <dbskr/dbskr_scurve_sptr.h>
#include <vcl_vector.h>
#include <vcl_utility.h>
#include <vcl_map.h>

class dbskr_localize_match {
 public:
  dbskr_localize_match();
  
  //dbskr_localize_match(dbskr_scurve_sptr c1,dbskr_scurve_sptr c2, vcl_vector< vcl_pair<int,int> > discMap,
  //                     double sampleSize);
  dbskr_localize_match(dbskr_scurve_sptr c1,
                       dbskr_scurve_sptr c2, 
                       dbskr_scurve_sptr dc1,
                       dbskr_scurve_sptr dc2,
                       vcl_vector<int>& map1,
                       vcl_vector<int>& map2,
                       vcl_vector< vcl_pair<int,int> > discMap,
                       float R);

  virtual ~dbskr_localize_match(){};
  
  //access functins
  float finalCost(){return _finalCost;};
  float finalCost(float cost){_finalCost=cost; return _finalCost;};

  vcl_vector < vcl_pair <int,int> > finalMap() { return _finalMap; } ;
  int n(){return _n;};
  int m(){return _m;};

  //display functions (debug)
  /*void printCost();
  void writeCost(vcl_string f);
  void printMap();
  void printFinalMap();
  dbskr_scurve_sptr curve1(){return _curve1;};
  dbskr_scurve_sptr curve2(){return _curve2;};
  */
  void match();

  //Data
  vcl_vector <double> _ds1;
  vcl_vector <double> _ds2;
  vcl_vector <double> _dt1;
  vcl_vector <double> _dt2;

 protected:
  //Data
  dbskr_scurve_sptr _curve1; //Interpolated curve 1
  dbskr_scurve_sptr _curve2; //Interpolated curve 2
  vcl_vector<int> _c1Map; //Index of original curve points
  vcl_vector<int> _c2Map; 

  vcl_vector< vcl_pair <int,int> > _initMap; //Map computed by the curve matching (indexes new curves)
  
  vcl_vector <float> _arcLength1;
  vcl_vector <float> _arcLength2;
  vcl_vector <float> _angle1;
  vcl_vector <float> _angle2;

  vcl_map <int, float> _prevCostC1;
  vcl_map <int, float> _prevCostC2;

  vcl_map <int, float> _currCostC1;
  vcl_map <int, float> _currCostC2;

  vcl_vector <float> _cost;
  float _finalCost;

  int _n;
  int _m;

  int _interpn;
  int _interpm;

  float _R;
  vcl_vector<float> _lambda;
  vcl_vector< vcl_pair<int,int> > _finalMap;
  int _numCostElems;

  //vector< vector< pair <int,int> > > _map;
  vcl_map< vcl_pair <int,int>, vcl_pair <int,int> > _map;

  //Functions
  virtual float computeIntervalCost(int i, int ip, int j, int jp);
};


#endif  // _dbskr_localize_match_h
