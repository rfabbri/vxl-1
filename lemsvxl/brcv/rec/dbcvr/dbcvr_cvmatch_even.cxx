#include "dbcvr_cvmatch_even.h"

#include <dbsol/dbsol_interp_curve_2d.h>

#include <vcl_cmath.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_utility.h>

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vnl/vnl_math.h>

dbcvr_cvmatch_even::dbcvr_cvmatch_even()
{
  _R = 10;
  _normalized_stretch_cost = false;
  setTemplateSize(3);
}

dbcvr_cvmatch_even::dbcvr_cvmatch_even (dbsol_interp_curve_2d_sptr c1, 
                                        dbsol_interp_curve_2d_sptr c2, 
                                        int n1, 
                                        int n2, 
                                        double R,
                                        int template_size)
{
  _curve1 = c1;
  _curve2 = c2;
  _R = R;

  _n1 = n1;
  _n2 = n2;

  //: we want n1 points on the first curve including first and last points --> n1 - 1 intervals
  _delta_s1 = _curve1->length()/(_n1-1);
  _delta_s2 = _curve2->length()/(_n2-1); // i.e. curves will be sampled with this arclength 
                                         // (_n1-1)*_delta_s1 = L1

  _normalized_stretch_cost = false;
  setTemplateSize(template_size);
}

dbcvr_cvmatch_even::~dbcvr_cvmatch_even ()
{
  if (_curve1)
    _curve1 = 0;
  if (_curve2)
    _curve2 = 0;
  clear();
}

void dbcvr_cvmatch_even::clear() {
  _DPCost.clear();          //DPMap of cost: n*m array of double
  _DPMap.clear();          //DPMap of prev point vcl_map: n*m array of vcl_pair of index
  _finalMap.clear();        //alignment curve
  _finalMapCost.clear();      //cost on alignment curve
  
  //: compute tangents and arclengths at each sample on the curves
  _tangents_curve1.clear();
  _tangents_curve2.clear();

  _lengths_curve1.clear();
  _lengths_curve2.clear();

  _curve1 = 0;
  _curve2 = 0;
}

void dbcvr_cvmatch_even::setTemplateSize (int temp_size) {
  
  XOFFSET.clear();
  YOFFSET.clear();
  
  if (temp_size == 1) {  // 1x1 template
    _template_size = 3;

    int dummyX[3] = {-1, 0,-1};
    int dummyY[3] = {-1,-1, 0};
    
    for (int i = 0; i<3; i++) {
      XOFFSET.push_back(dummyX[i]); 
      YOFFSET.push_back(dummyY[i]); 
    }

  } else if (temp_size == 3) { // 3x3 template
    _template_size = 11;
    
    int dummyX[11] = {-3,-2,-1, 0,-1,-1,-2,-2,-3,-1,-3};
    int dummyY[11] = {-3,-2,-1,-1, 0,-2,-1,-3,-2,-3,-1};
    
    for (int i = 0; i<11; i++) {
      XOFFSET.push_back(dummyX[i]); 
      YOFFSET.push_back(dummyY[i]); 
    }
  
  } else if (temp_size == 5) { // 5x5 template
    _template_size = 21;
    
    int dummyX[21] = {-1, 0,-1,-1,-2,-2,-3,-1,-3, -1,-3,-4,-4, -1,-2,-3,-4,-5,-5,-5,-5};
    int dummyY[21] = {-1,-1, 0,-2,-1,-3,-2,-3,-1, -4,-4,-3,-1, -5,-5,-5,-5,-1,-2,-3,-4};
    
    for (int i = 0; i<21; i++) {
      XOFFSET.push_back(dummyX[i]);
      YOFFSET.push_back(dummyY[i]);
    }
  
  } else if (temp_size == 11) { // 11x11 template
    _template_size = 93;
    
    int dummyX[93] = {-1, 0,-1,-1,-2,-2,-3,-1,-3, -1,-3,-4,-4, -1,-2,-3,-4,-5,-5,-5,-5, //5x5
    -1,-5,-6,-6, -1,-2,-3,-4,-5,-6,-7,-7,-7,-7,-7,-7, -1,-3,-5,-7,-8,-8,-8,-8, -1,-2,-4,-5,-7,-8,-9,-9,-9,-9,-9,-9, //9x9
     -1, -3, -7, -9,-10,-10,-10,-10, -1, -2, -3, -4, -5, -6, -7, -8, -9,-10,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11, //11x11
     -1, -5, -7, -11,-12,-12,-12,-12};
    int dummyY[93] = {-1,-1, 0,-2,-1,-3,-2,-3,-1, -4,-4,-3,-1, -5,-5,-5,-5,-1,-2,-3,-4, //5x5
    -6,-6,-5,-1, -7,-7,-7,-7,-7,-7,-1,-2,-3,-4,-5,-6, -8,-8,-8,-8,-1,-3,-5,-7, -9,-9,-9,-9,-9,-9,-1,-2,-4,-5,-7,-8, //9x9
    -10,-10,-10,-10, -1, -3, -7, -9,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11, -1, -2, -3, -4, -5, -6, -7, -8, -9,-10, //11x11
    -12,-12,-12,-12, -1, -5, -7, -11};

    for (int i = 0; i<93; i++) {
      XOFFSET.push_back(dummyX[i]);
      YOFFSET.push_back(dummyY[i]);
    }
  }
}

// ###########################################################
//          MATCH
// ###########################################################

void dbcvr_cvmatch_even::Match ()
{
  initializeDPCosts();
  computeDPCosts ();
  findDPCorrespondence ();

#if 0

  vcl_ofstream fpoo; 
  fpoo.open("D:\\contours\\Mpeg-7\\temp_even.out", vcl_ios::app);

  fpoo << _n1 << " " << _n2 << "\n";
  for (int i = 0; i<_n1; i++) {
    for (int j = 0; j<_n2; j++) {   // -1 since (0,0) will go to CD
      fpoo << i*_delta_s1 << " ";
      fpoo << j*_delta_s2 << vcl_endl;
    }
  }

  //vcl_cout << "k_min was: " << k_min << " changhed to 0\n";
  //k_min = 0;

  int N0=_finalMap.size();
  fpoo << N0 << "\n";
  for (int i=0; i<N0 ; i++) {
    int ii =_finalMap[i].first;
    int jj =_finalMap[i].second;
    fpoo << ii*_delta_s1 << " ";
    fpoo << jj*_delta_s2 << vcl_endl;    
  }

  fpoo.close();
#endif
}

// ###########################################################
//          DP Cost
// ###########################################################

void dbcvr_cvmatch_even::initializeDPCosts()
{
  _DPCost.clear();
  _DPMap.clear();
  
  assert (_n1>0);
  assert (_n2>0);

  for (int i=0;i<_n1;i++) {
    vcl_vector<double> tmp1(_n2,DP_VERY_LARGE_COST);
    _DPCost.push_back(tmp1);
    vcl_pair <int,int> tmp3(0,0);
    vcl_vector< vcl_pair <int,int> > tmp2(_n2,tmp3);
    _DPMap.push_back(tmp2);
  }

  //Cost Matrix Initialization
  _finalCost = DP_VERY_LARGE_COST;
  for (int n=0;n<_n1;n++) {
    for (int m=0;m<_n2;m++) {
      _DPCost[n][m]=DP_VERY_LARGE_COST;
    }
  }
  _DPCost[0][0]=0.0;

  for (int i = 0; i<_n1; i++) 
    _lengths_curve1.push_back(i*_delta_s1);
    
  for (int i = 0; i<_n1; i++) 
    _tangents_curve1.push_back(_curve1->tangent_angle_at(_lengths_curve1[i]));

  for (int i = 0; i<_n2; i++) 
    _lengths_curve2.push_back(i*_delta_s2);

  for (int i = 0; i<_n2; i++)
    _tangents_curve2.push_back(_curve2->tangent_angle_at(_lengths_curve2[i]));

}

// Cost of matching the interval [x(ip),x(i)]  to [y(jp),y(j)].
double dbcvr_cvmatch_even::computeIntervalCost(int i, int ip, int j, int jp){
  
  double s1 = _lengths_curve1[i];
  double s1_p = _lengths_curve1[ip];

  double s2 =  _lengths_curve2[j];
  double s2_p =  _lengths_curve2[jp];
  
  //Here the cost is based on lengths of the segments.
  double ds1 = s1-s1_p;
  double ds2 = s2-s2_p;
  
  double dF;
  if (_normalized_stretch_cost) {
    if (ds1+ds2 > 1E-5)
      dF = vcl_pow(ds1-ds2,2)/(ds1+ds2);
    else dF = 0;
  } else dF = vcl_fabs(ds1-ds2);

  double dt1 = curve_angleDiff (_tangents_curve1[i], _tangents_curve1[ip]);
  double dt2 = curve_angleDiff (_tangents_curve2[j], _tangents_curve2[jp]);

  double dK = vcl_fabs(dt1-dt2);
  
  return dF + _R*dK;
}

void dbcvr_cvmatch_even::computeDPCosts ()
{  
  int sum,start,i,ip,j,jp,k;
  double cost;

  for (sum = 1; sum<_n1+_n2-1; sum++) {
    start=(int)curve_maxof(0,sum-_n2+1,-10000);
    for (i=start;(i<=_n1-1 && i<=sum);i++) {
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
}

// ###########################################################
//          AFTER DP, FIND MATCHING
// ###########################################################

void dbcvr_cvmatch_even::findDPCorrespondence (void)
{
  int i, j, ip, jp;

  _finalMap.clear();          //Clean the table
  _finalMapCost.clear();

  _finalCost = _DPCost[_n1-1][_n2-1];  //The final value of DPMap

  ip = _n1-1;
  jp = _n2-1;
  i = _n1-1;
  j = _n2-1;

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
