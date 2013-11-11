#include <vcl_iostream.h>
#include <vcl_fstream.h>
//#include <stl_config.h>
//#include <string>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_cmath.h>


#ifndef __TBS_DPMATCH_DEF__
#define __TBS_DPMATCH_DEF__

#define DP_VERY_LARGE_COST 1E10
#include "Curve.h"

template <class curveType,class floatType>
class DPMatch{
 public:
  DPMatch();
  DPMatch(curveType &c1, curveType &c2);
  ~DPMatch(){};
  
  //access functions
  floatType finalCost(){return _finalCost;};
  floatType finalCost(floatType cost){_finalCost=cost; return _finalCost;};
  vector < pair <int,int> > finalMap(){return _finalMap;};
  vector< floatType > finalMapCost(){return _finalMapCost;};
   vector< floatType > finalCost1(){return _cost1;};
  int n(){return _n;};
  int m(){return _m;};

  //display functions (debug)
  void printCost();
  void writeCost(string f);
  void printMap();
  void printFinalMap();
  curveType curve1(){return _curve1;};
  curveType curve2(){return _curve2;};
  void match();

 protected:
  //Data
  curveType _curve1;
  curveType _curve2;
  vector< vector<floatType> > _cost;
  vector< vector< pair <int,int> > > _map;
  vector< pair <int,int> > _finalMap;
  floatType _finalCost;
  vector< floatType > _finalMapCost;
  vector< floatType > _cost1;

  int _n;
  int _m;
  bool _flip;
  
  //Functions
  void initializeDPCosts();
  void computeDPCosts();
  void findDPCorrespondence();
  virtual floatType computeIntervalCost(int i, int ip, int j, int jp);
  virtual floatType computeIntervalCost1(int i, int ip, int j, int jp);
};


#endif
