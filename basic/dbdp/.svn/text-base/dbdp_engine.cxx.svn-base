#include "dbdp_engine.h"
#include <vcl_cmath.h>

int max_of(int a, int b, int c)
{
  if (a>b && a>c)
    return a;  
  else if (b>c && b>a)
    return b;
  else if (c>b && c>a)
    return c;
  else
    return a;
}

dbdp_engine::dbdp_engine(int grid_w, int grid_h, int template_size, dbdp_cost *cost_fnc)
{
  assert(grid_w > 0 && grid_h > 0);
  assert(template_size == 3 ||  template_size == 5 || template_size == 11);
  assert(cost_fnc != NULL);
  grid_w_ = grid_w;
  grid_h_ = grid_h;
  setTemplateSize(template_size);
  cost_fnc_ = cost_fnc;
}

dbdp_engine::~dbdp_engine()
{
}

void dbdp_engine::setTemplateSize(int temp_size) 
{
  XOFFSET.clear();
  YOFFSET.clear();
  
  if (temp_size == 3) 
  { // 3x3 template
    template_size_ = 9;
    
    int dummyX[9] = {-1, 0,-1,-1,-2,-2,-3,-1,-3};
    int dummyY[9] = {-1,-1, 0,-2,-1,-3,-2,-3,-1};
    
    for (int i = 0; i<9; i++) 
    {
      XOFFSET.push_back(dummyX[i]); 
      YOFFSET.push_back(dummyY[i]); 
    }
  } 
  else if (temp_size == 5) 
  { // 5x5 template
    template_size_ = 21;
    
    int dummyX[21] = {-1, 0,-1,-1,-2,-2,-3,-1,-3, -1,-3,-4,-4, -1,-2,-3,-4,-5,-5,-5,-5};
    int dummyY[21] = {-1,-1, 0,-2,-1,-3,-2,-3,-1, -4,-4,-3,-1, -5,-5,-5,-5,-1,-2,-3,-4};
    
    for (int i = 0; i<21; i++) 
    {
      XOFFSET.push_back(dummyX[i]);
      YOFFSET.push_back(dummyY[i]);
    }
  } 
  else if (temp_size == 11) 
  { // 11x11 template
    template_size_ = 93;
    
    int dummyX[93] = {-1, 0,-1,-1,-2,-2,-3,-1,-3, -1,-3,-4,-4, -1,-2,-3,-4,-5,-5,-5,-5, //5x5
    -1,-5,-6,-6, -1,-2,-3,-4,-5,-6,-7,-7,-7,-7,-7,-7, -1,-3,-5,-7,-8,-8,-8,-8, -1,-2,-4,-5,-7,-8,-9,-9,-9,-9,-9,-9, //9x9
     -1, -3, -7, -9,-10,-10,-10,-10, -1, -2, -3, -4, -5, -6, -7, -8, -9,-10,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11, //11x11
     -1, -5, -7, -11,-12,-12,-12,-12};
    int dummyY[93] = {-1,-1, 0,-2,-1,-3,-2,-3,-1, -4,-4,-3,-1, -5,-5,-5,-5,-1,-2,-3,-4, //5x5
    -6,-6,-5,-1, -7,-7,-7,-7,-7,-7,-1,-2,-3,-4,-5,-6, -8,-8,-8,-8,-1,-3,-5,-7, -9,-9,-9,-9,-9,-9,-1,-2,-4,-5,-7,-8, //9x9
    -10,-10,-10,-10, -1, -3, -7, -9,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11, -1, -2, -3, -4, -5, -6, -7, -8, -9,-10, //11x11
    -12,-12,-12,-12, -1, -5, -7, -11};

    for (int i = 0; i<93; i++) 
    {
      XOFFSET.push_back(dummyX[i]);
      YOFFSET.push_back(dummyY[i]);
    }
  }
}

void dbdp_engine::RunDP()
{
  initializeDPCosts();
  computeDPCosts();
  findDPCorrespondence();
}

void dbdp_engine::initializeDPCosts()
{
  DPCost_.clear();
  DPMap_.clear();

  for (int i=0; i < grid_w_; i++) 
  {
    vcl_vector<double> tmp1(grid_h_,DP_VERY_LARGE_COST);
    DPCost_.push_back(tmp1);
    vcl_pair <int,int> tmp3(0,0);
    vcl_vector< vcl_pair <int,int> > tmp2(grid_h_,tmp3);
    DPMap_.push_back(tmp2);
  }

  //Cost Matrix Initialization
  finalCost_ = DP_VERY_LARGE_COST;
  for (int n = 0; n < grid_w_; n++) 
    for (int m = 0; m < grid_h_; m++) 
      DPCost_[n][m]=DP_VERY_LARGE_COST;

  DPCost_[0][0]=0.0;
}

// Cost between the interval [x(ip),x(i)] and [y(jp),y(j)].
double dbdp_engine::computeIntervalCost(int i, int ip, int j, int jp)
{
  return cost_fnc_->compute_interval_cost(i, ip, j, jp);
}

  //#define TEMPLATE_SIZE 9 //3x3
void dbdp_engine::computeDPCosts()
{ 
  int sum,start,i,ip,j,jp,k;
  double cost;

  for (sum = 1; sum < grid_w_ + grid_h_ - 1; sum++) 
  {
    start = max_of(0, sum - grid_h_ + 1, -10000);
    for (i = start; (i <= grid_w_-1 && i <= sum);i++) 
    {
      j=sum-i;
      for (k = 0;k < template_size_;k++) 
      { //TEMPLATE_SIZE=9 originally
        ip=i+XOFFSET[k];
        jp=j+YOFFSET[k];
        if (ip >= 0 &&  jp >=0) 
        {
            double incCost=computeIntervalCost(i,ip,j,jp);
            cost = DPCost_[ip][jp]+incCost;
            if (cost < DPCost_[i][j])
            {
              DPCost_[i][j]=cost;
              DPMap_[i][j].first=ip;
              DPMap_[i][j].second=jp;
            }
        }
      }
    }
  }
}

void dbdp_engine::findDPCorrespondence()
{
  int i, j, ip, jp;

  finalMap_.clear();          //Clean the table
  finalMapCost_.clear();

  finalCost_ = DPCost_[grid_w_-1][grid_h_-1];  //The final value of DPMap

  ip = grid_w_-1;
  jp = grid_h_-1;
  i = grid_w_-1;
  j = grid_h_-1;

  vcl_pair <int,int> p(ip,jp);
  finalMap_.push_back(p);
  finalMapCost_.push_back(DPCost_[p.first][p.second]);

  while (ip > 0 && jp > 0) 
  { //Ming: should be &&
    ip=DPMap_[i][j].first;
    jp=DPMap_[i][j].second;
    vcl_pair <int,int> p(ip,jp);
    finalMap_.push_back(p);
    finalMapCost_.push_back(DPCost_[p.first][p.second]);
  
    i=ip; //Go to the previous point
    j=jp;
  }
}
