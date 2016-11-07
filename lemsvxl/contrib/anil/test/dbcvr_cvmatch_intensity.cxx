#include "dbcvr_cvmatch_intensity.h"
#include <vcl_cmath.h>

dbcvr_cvmatch_intensity::dbcvr_cvmatch_intensity(vcl_vector<double> vector1, vcl_vector<double> vector2)
{
    pixel_vector1_ = vector1;
    pixel_vector2_ = vector2;
}

double dbcvr_cvmatch_intensity::computeIntervalCost(int i, int ip, int j, int jp)
{
    //double first_half = vcl_abs((pixel_vector1_[ip]*(i-ip)/2)-(pixel_vector2_[jp]*(j-jp)/2));
    //double second_half = vcl_abs((pixel_vector1_[i]*(i-ip)/2)-(pixel_vector2_[j]*(j-jp)/2));
    //return first_half + second_half;
    
    //return vcl_abs((pixel_vector1_[ip]*(i-ip))-(pixel_vector2_[jp]*(j-jp)));

    return vcl_abs(pixel_vector1_[ip]-pixel_vector2_[jp])+vcl_abs(pixel_vector1_[i]-pixel_vector2_[j]);
}

void dbcvr_cvmatch_intensity::Match ()
{
  initializeDPCosts();
  computeDPCosts ();
  findDPCorrespondence ();
}

void dbcvr_cvmatch_intensity::initializeDPCosts()
{
  //Ming: init
  _DPCost.clear();
  _DPMap.clear();
  int _n = pixel_vector1_.size();
  int _m = pixel_vector2_.size();
  assert (_n>0);
  assert (_m>0);

  for (int i=0;i<_n;i++) {
    vcl_vector<double> tmp1(_m,DP_VERY_LARGE_COST);
    _DPCost.push_back(tmp1);
    vcl_pair <int,int> tmp3(0,0);
    vcl_vector< vcl_pair <int,int> > tmp2(_m,tmp3);
    _DPMap.push_back(tmp2);
  }

  //Cost Matrix Initialization
  _finalCost = DP_VERY_LARGE_COST;
  for (int n=0;n<_n;n++) {
    for (int m=0;m<_m;m++) {
      _DPCost[n][m]=DP_VERY_LARGE_COST;
    }
  }
  _DPCost[0][0]=0.0;

}

void dbcvr_cvmatch_intensity::computeDPCosts ()
{ 
  int sum,start,i,ip,j,jp,k;
  double cost;

  int _n = pixel_vector1_.size();
  int _m = pixel_vector2_.size();

  for (sum = 1; sum<_n+_m-1; sum++) {
    start=(int)curve_maxof(0,sum-_m+1,-10000);
    for (i=start;(i<=_n-1 && i<=sum);i++) {
      j=sum-i;
      for (k=0;k<_template_size;k++) { //TEMPLATE_SIZE=9 originally
        ip=i+XOFFSET[k];
        jp=j+YOFFSET[k];
        if (ip >= 0 &&  jp >=0) {
            double incCost=computeIntervalCost(i,ip,j,jp);
            cost =_DPCost[ip][jp]+incCost;
            if (cost < _DPCost[i][j]){
              _DPCost[i][j]=cost;
              _DPMap[i][j].first=ip;
              _DPMap[i][j].second=jp;
            }
        }
      }
    }
  }
  //Kai
  ///vcl_cout<<"computeDPCosts() Number of computation: "<<count<<" "<<"\n";
}

void dbcvr_cvmatch_intensity::findDPCorrespondence (void)
{
  int i, j, ip, jp;

  _finalMap.clear();          //Clean the table
  _finalMapCost.clear();

  int _n = pixel_vector1_.size();
  int _m = pixel_vector2_.size();

  _finalCost = _DPCost[_n-1][_m-1];  //The final value of DPMap

  ip = _n-1;
  jp = _m-1;
  i = _n-1;
  j = _m-1;

  vcl_pair <int,int> p(ip,jp);
  _finalMap.push_back(p);
  _finalMapCost.push_back(_DPCost[p.first][p.second]);

  while (ip > 0 || jp > 0) { //Ming: should be &&
    ip=_DPMap[i][j].first;
    jp=_DPMap[i][j].second;
    vcl_pair <int,int> p(ip,jp);
    _finalMap.push_back(p);
    _finalMapCost.push_back(_DPCost[p.first][p.second]);
  
    i=ip; //Go to the previous point
    j=jp;
  }
}
