// CAN
#ifndef __TBS_FINE_DPMATCH_DEF__
#define __TBS_FINE_DPMATCH_DEF__

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_vector.h>

template <class curveType,class floatType>
class FineDPMatch
{
public:
  FineDPMatch();
  FineDPMatch(curveType &c1, curveType &c2);
  ~FineDPMatch(){};

  //From NewFineDPMatch
  vcl_vector <floatType> _ds1;
  vcl_vector <floatType> _ds2;
  vcl_vector <floatType> _dt1;
  vcl_vector <floatType> _dt2;

  //access functions
  floatType finalCost(){return _finalCost;};
  floatType finalCost(floatType cost){_finalCost=cost; return _finalCost;};
  vcl_vector < vcl_pair <int,int> > finalMap(){return _finalMap;};
  vcl_vector< floatType > finalMapCost(){return _finalMapCost;};
  int n(){return _n;};
  int m(){return _m;};

  //display functions (debug)
  curveType curve1(){return _curve1;};
  curveType curve2(){return _curve2;};
  void match();

 protected:
  //Data
  curveType _curve1;
  curveType _curve2;
  vcl_vector< vcl_vector<floatType> > _cost;
  vcl_vector< vcl_vector< vcl_pair <int,int> > > _map;
  vcl_vector< vcl_pair <int,int> > _finalMap;
  floatType _finalCost;
  vcl_vector< floatType > _finalMapCost;
  int _n;
  int _m;
  bool _flip;
  
  //From NewFineDPMatch header file:
  int _numCostElems;
  floatType _R1;
  vcl_vector<floatType> _lambda;

  //Functions
  void initializeDPCosts();
  void computeDPCosts();
  void findDPCorrespondence();
  floatType computeIntervalCost(int i, int ip, int j, int jp);
};

//##############################################

template <class curveType,class floatType>
FineDPMatch<curveType,floatType>::FineDPMatch()
{
  vcl_vector< vcl_vector<floatType> > a;
  vcl_vector< vcl_vector< vcl_pair <int,int> > > b;
  vcl_vector< vcl_pair <int,int> > c;
  vcl_vector< floatType > d;

  curveType c1,c2;
  _curve1 = c1;
  _curve2 = c2;
  _cost = a;
  _map = b;
  _finalMap = c;
  _finalMapCost = d;
  _finalCost = 0;
  _m = 0;
  _n = 0;
}

//Constructor copied from NewFineDPMatch.cpp
template <class curveType,class floatType>
FineDPMatch<curveType,floatType>::FineDPMatch(curveType &c1, curveType &c2)
{
  vcl_vector< vcl_pair <int,int> > b;

  int n; 
  _curve1 = c1;
  _curve2 = c2;
  _flip = false;
//  _curve1.compute_properties();
//  _curve2.compute_properties();
  _n=_curve1.num_fine_points();
  _m=_curve2.num_fine_points();
  for (n=0;n<_n;n++)
  {
    vcl_vector<floatType> tmp1(_m,DP_VERY_LARGE_COST);
    _cost.push_back(tmp1);
    vcl_pair <int,int> tmp3(0,0);
    vcl_vector< vcl_pair <int,int> > tmp2(_m,tmp3);
    _map.push_back(tmp2);
  }
  _finalMap = b;
  _finalCost = DP_VERY_LARGE_COST;
  _R1 = 15.0;

  _numCostElems=3;
  vcl_vector <floatType> v(_numCostElems,0);
  _ds1=v;
  _ds2=v;
  _dt1=v;
  _dt2=v;
  _lambda=v;
}

template <class curveType,class floatType>
void FineDPMatch<curveType,floatType>::initializeDPCosts()
{
  _cost[0][0]=0.0;
}


template <class curveType,class floatType>
void FineDPMatch<curveType,floatType>::computeDPCosts()
{
  int XOFFSETF[9] = {-1, 0,-1,-1,-2,-2,-3,-1,-3};
  int YOFFSETF[9] = {-1,-1, 0,-2,-1,-3,-2,-3,-1};

  int i,ip,j,jp,k;
  floatType cost;

  for (i=0;i<_n;i++)
  {
    for (j=0;j<_m;j++)
    {
      for (k=0;k<9;k++)
      {
        ip=i+XOFFSETF[k];
        jp=j+YOFFSETF[k];
        if (ip >= 0 &&  jp >=0)
        {
          cost =_cost[ip][jp]+computeIntervalCost(i,ip,j,jp);
          if (cost < _cost[i][j])
          {     
            _cost[i][j]=cost;
            _map[i][j].first=ip;
            _map[i][j].second=jp;
          }
        }
      }  
    }
  }
}

//Copied from NewFineDPMatch
// Cost of matching the interval [x(ip),x(i)]  to [y(jp),y(j)].
template <class curveType,class floatType>
floatType FineDPMatch<curveType,floatType>::computeIntervalCost(int i, int ip, int j, int jp)
{
  floatType cost,dF=0,dK=0;
  double l1,l2,a1,a2;

  l1=_curve1.total_length(ip,i);
  l2=_curve2.total_length(jp,j);

  dF = fabs(l1-l2);
  
  if(ip==0 && jp==0)
    dK=0;
  else
  {
    a1 = angleDiff(_curve1.angle(i),_curve1.angle(ip));
    a2 = angleDiff(_curve2.angle(j),_curve2.angle(jp));
    dK = fabs(a1-a2);
  }
  cost = dF+_R1*dK;
  return cost;
}

template <class curveType,class floatType>
void FineDPMatch<curveType,floatType>::findDPCorrespondence()
{
  int i,j,ip,jp;
  _finalMap.clear();
  _finalMapCost.clear();
  _finalCost=_cost[_n-1][_m-1];
  
  ip=_n-1;
  jp=_m-1;
  i=_n-1;
  j=_m-1;
 
  vcl_pair <int,int> p(ip,jp);
  _finalMap.push_back(p);
  _finalMapCost.push_back(_cost[p.first][p.second]);
  while (ip > 0 || jp > 0)
  {
    ip=_map[i][j].first;
    jp=_map[i][j].second;

    vcl_pair <int,int> p(ip,jp);
    _finalMap.push_back(p);
    _finalMapCost.push_back(_cost[p.first][p.second]);
    i=ip;
    j=jp;
  } 
}

template <class curveType,class floatType>
void FineDPMatch<curveType,floatType>::match()
{
  initializeDPCosts();
  computeDPCosts();
  findDPCorrespondence();
}
#endif
