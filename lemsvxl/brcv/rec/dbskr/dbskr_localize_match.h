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
#include <vector>
#include <utility>
#include <map>

class dbskr_localize_match {
 public:
  dbskr_localize_match();
  
  //dbskr_localize_match(dbskr_scurve_sptr c1,dbskr_scurve_sptr c2, std::vector< std::pair<int,int> > discMap,
  //                     double sampleSize);
  dbskr_localize_match(dbskr_scurve_sptr c1,
                       dbskr_scurve_sptr c2, 
                       dbskr_scurve_sptr dc1,
                       dbskr_scurve_sptr dc2,
                       std::vector<int>& map1,
                       std::vector<int>& map2,
                       std::vector< std::pair<int,int> > discMap,
                       float R);

  virtual ~dbskr_localize_match(){};
  
  //access functins
  float finalCost(){return _finalCost;};
  float finalCost(float cost){_finalCost=cost; return _finalCost;};

  std::vector < std::pair <int,int> > finalMap() { return _finalMap; } ;
  int n(){return _n;};
  int m(){return _m;};

  //display functions (debug)
  /*void printCost();
  void writeCost(std::string f);
  void printMap();
  void printFinalMap();
  dbskr_scurve_sptr curve1(){return _curve1;};
  dbskr_scurve_sptr curve2(){return _curve2;};
  */
  void match();

  //Data
  std::vector <double> _ds1;
  std::vector <double> _ds2;
  std::vector <double> _dt1;
  std::vector <double> _dt2;

 protected:
  //Data
  dbskr_scurve_sptr _curve1; //Interpolated curve 1
  dbskr_scurve_sptr _curve2; //Interpolated curve 2
  std::vector<int> _c1Map; //Index of original curve points
  std::vector<int> _c2Map; 

  std::vector< std::pair <int,int> > _initMap; //Map computed by the curve matching (indexes new curves)
  
  std::vector <float> _arcLength1;
  std::vector <float> _arcLength2;
  std::vector <float> _angle1;
  std::vector <float> _angle2;

  std::map <int, float> _prevCostC1;
  std::map <int, float> _prevCostC2;

  std::map <int, float> _currCostC1;
  std::map <int, float> _currCostC2;

  std::vector <float> _cost;
  float _finalCost;

  int _n;
  int _m;

  int _interpn;
  int _interpm;

  float _R;
  std::vector<float> _lambda;
  std::vector< std::pair<int,int> > _finalMap;
  int _numCostElems;

  //vector< vector< pair <int,int> > > _map;
  std::map< std::pair <int,int>, std::pair <int,int> > _map;

  //Functions
  virtual float computeIntervalCost(int i, int ip, int j, int jp);
};


#endif  // _dbskr_localize_match_h
