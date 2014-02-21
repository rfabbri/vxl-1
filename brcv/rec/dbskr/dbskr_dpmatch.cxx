#include <dbskr/dbskr_dpmatch.h>
#include <dbskr/dbskr_scurve.h>

#include <vcl_iostream.h>

#define VERY_LARGE_BOUNDARY_COST  (300)

dbskr_dpmatch::dbskr_dpmatch() : dbcvr_cvmatch_even() 
{
  R1_ = 6.0f;

  num_cost_elems_=3;
  vcl_vector <double> v(num_cost_elems_,0);
  ds1_=v;
  ds2_=v;
  dt1_=v;
  dt2_=v;
  for(int i = 0; i<3; i++)
    lambda_.push_back(1.0f);
}

dbskr_dpmatch::dbskr_dpmatch(dbskr_scurve_sptr c1, dbskr_scurve_sptr c2) : dbcvr_cvmatch_even() 
{
  scurve1_ = c1;
  scurve2_ = c2;
  
  _n1 = scurve1_->num_points();
  _n2 = scurve2_->num_points();

  R1_ = 6.0f; 
  
  num_cost_elems_=3;
  vcl_vector <double> v(num_cost_elems_,0);
  ds1_=v;
  ds2_=v;
  dt1_=v;
  dt2_=v;
  for(int i = 0; i<3; i++)
    lambda_.push_back(1.0f);
}

void dbskr_dpmatch::set_scurves(dbskr_scurve_sptr sc1, dbskr_scurve_sptr sc2)
{
  scurve1_ = sc1;
  scurve2_ = sc2;
  _n1=scurve1_->num_points();
  _n2=scurve2_->num_points();
}

dbskr_dpmatch::dbskr_dpmatch(dbskr_scurve_sptr c1,
                             dbskr_scurve_sptr c2, 
                             double R1, 
                             vcl_vector<double>& lambda, 
                             int num_cost_elems) : dbcvr_cvmatch_even() 
{
  scurve1_ = c1;
  scurve2_ = c2;
  _n1=scurve1_->num_points();
  _n2=scurve2_->num_points();

  R1_ = R1;

  lambda_ =lambda;
  num_cost_elems_=num_cost_elems;
  vcl_vector <double> v(num_cost_elems_,0);
  ds1_=v;
  ds2_=v;
  dt1_=v;
  dt2_=v;
  
  //setTemplateSize(3);
}

dbskr_dpmatch::~dbskr_dpmatch() 
{ 
  if (scurve1_) 
    scurve1_ = 0; 
  if (scurve2_) 
    scurve2_ = 0;  
  clear();
}

void dbskr_dpmatch::initializeDPCosts()
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

}

// Cost of matching the  interval [x(i),x(ip)] to the interval [y(j),y(jp)].
double dbskr_dpmatch::computeIntervalCost(int i, int ip, int j, int jp) 
{
  double cost,dF=0,dK=0;
  double dA1(0.0),dA2(0.0);
  double area_cost(0.0);

  int k;
  scurve1_->stretch_cost(i,ip,ds1_);
  scurve2_->stretch_cost(j,jp,ds2_);
  scurve1_->bend_cost(i,ip,dt1_);
  scurve2_->bend_cost(j,jp,dt2_);

  for (k=0;k<num_cost_elems_;k++)
    dF = dF+vcl_fabs(ds1_[k]-lambda_[k]*ds2_[k]);
  
  for (k=0;k<num_cost_elems_;k++)
    dK = dK+vcl_fabs(dt1_[k]-dt2_[k]);
 
  if ( scurve1_->get_area_factor() > 0.0)
  {
      scurve1_->area_cost(i,ip,dA1);
      scurve2_->area_cost(j,jp,dA2);  
      area_cost=scurve1_->get_area_factor()*vcl_fabs(dA1-dA2);
  }
  cost = dF+R1_*dK+area_cost;
  return cost;
} 

double dbskr_dpmatch::computeIntervalCostPrint(int i, int ip, int j, int jp)
{
  double cost,dF=0,dK=0;
  int k;
  scurve1_->stretch_cost(i,ip,ds1_);
  scurve2_->stretch_cost(j,jp,ds2_);
  scurve1_->bend_cost(i,ip,dt1_);
  scurve2_->bend_cost(j,jp,dt2_);

  //vcl_cout.precision(2);
  //vcl_cout<<i<<"-"<<ip<<":"<<j<<"-"<<jp<<"\n";
  vcl_cout.precision(2);
  vcl_cout << "\tds1+: " << ds1_[0] << "\tds1-: " << ds1_[1] << "\t2*dr1: " << ds1_[2] << "\n";
  vcl_cout.precision(2);
  vcl_cout << "\tds2+: " << ds2_[0] << "\tds2-: " << ds2_[1] << "\t2*dr2: " << ds2_[2] << "\n";

  for (k=0;k<num_cost_elems_;k++) 
    dF = dF+vcl_fabs(ds1_[k]-lambda_[k]*ds2_[k]);
  
  vcl_cout.precision(2);
  vcl_cout << "\tdS+: " << vcl_fabs(ds1_[0]-lambda_[0]*ds2_[0]) << " dS-: " << vcl_fabs(ds1_[1]-lambda_[1]*ds2_[1]) << " ";
  vcl_cout.precision(2);
  vcl_cout << "dR: " << vcl_fabs(ds1_[2]-lambda_[2]*ds2_[2]) << " dS = dS+ + dS- + dR = " << dF << "\n";

  vcl_cout.precision(2);
  vcl_cout << "\tdt1+: " << dt1_[0] << " dt1-: " << dt1_[1] << " 2*dphi1: " << dt1_[2] << "\n";
  vcl_cout.precision(2);
  vcl_cout << "\tdt2+: " << dt2_[0] << " dt2-: " << dt2_[1] << " 2*dphi2: " << dt2_[2] << "\n";

  for (k=0;k<num_cost_elems_;k++) {
    dK = dK+vcl_fabs(dt1_[k]-dt2_[k]);
  }

  vcl_cout.precision(2);
  vcl_cout << "\tdT+: " << vcl_fabs(dt1_[0]-dt2_[0]) << " dT-: " << vcl_fabs(dt1_[1]-dt2_[1]) << " ";
  vcl_cout.precision(2);
  vcl_cout << "dPhi: " << vcl_fabs(dt1_[2]-dt2_[2]) << " dT = dT+ + dT- + dPhi = " << dK << "\n";
  vcl_cout.precision(2);
  vcl_cout << "\t\tR1: " << R1_ << " R1*dT: " << R1_*dK << " dS+R1*dT: " << dF+R1_*dK << vcl_endl;

  //double phi11=scurve1_->phi(i);
  //double phi12=scurve1_->phi(ip);
  //double phi21=scurve2_->phi(j);
  //double phi22=scurve2_->phi(jp);
  //vcl_cout.precision(5);
  //vcl_cout<<"phi11:"<<phi11<<" "<<"phi12:"<<phi12<<" "<<"phi21:"<<phi21<<" "<<"phi22:"<<phi22<<" "<<vcl_endl;


  cost = dF+R1_*dK;
  return cost;
}

//: ozge added this function to find an approximate cost before giving DP solution
//  needed by tree-edit shock curve deformation cost computation to prune some high cost matchings
float dbskr_dpmatch::approx_cost() 
{  
#if 0  // removing the rotation variant cost
  //: rotation variant cost, we're only allowing the branches to rotate for 90 degrees
  vgl_point_2d<double> ps1 = scurve1_->sh_pt(0);
  vgl_point_2d<double> pe1 = scurve1_->sh_pt(scurve1_->num_points()-1);

  vgl_point_2d<double> ps2 = scurve2_->sh_pt(0);
  vgl_point_2d<double> pe2 = scurve2_->sh_pt(scurve2_->num_points()-1);

  vgl_vector_2d<double> pv1(pe1-ps1); normalize(pv1);
  vgl_vector_2d<double> pv2(pe2-ps2); normalize(pv2);

  double product = dot_product(pv1, pv2);
  
  // rotation variant cost, we're only allowing the branches to rotate for 90 degrees
  if (product < 0) return 2*VERY_LARGE_BOUNDARY_COST; 
#endif

  //Amir: we need to update this function to reflect the new cost function
  float approx_cost =  float (vcl_fabs(scurve1_->boundary_plus_length() - lambda_[0]*scurve2_->boundary_plus_length()) +
                              vcl_fabs(scurve1_->boundary_minus_length() - lambda_[1]*scurve2_->boundary_minus_length()));
  return approx_cost;
}

//: ozge added this function
//  it adds initial time/radius difference and alpha difference costs to the final cost of matching
float dbskr_dpmatch::init_dr() 
{ 
  int n1 = scurve1_->num_points()-1;
  int n2 = scurve2_->num_points()-1;

#if 0  //Same as Matching-Tek 
  float initDr = float((vcl_fabs(scurve1_->time(0)-lambda_[2]*scurve2_->time(0)) +
                        vcl_fabs(scurve1_->time(n1)-lambda_[2]*scurve2_->time(n2)))/2.0);
#else //: Ozge added multiplication by 2.0 (which cancels averaging..) on May 18, 07. In the original implementation there is no multiplication even though in the PAMI06 paper there is..
  float initDr = float(2.0*(vcl_fabs(scurve1_->time(0)-lambda_[2]*scurve2_->time(0)) +
                        vcl_fabs(scurve1_->time(n1)-lambda_[2]*scurve2_->time(n2)))/2.0);
#endif  
  return initDr; 
}

float dbskr_dpmatch::init_phi() 
{ 
  int n1 = scurve1_->num_points()-1;
  int n2 = scurve2_->num_points()-1;
  //: Ozge added multiplication by 2.0 on May 18, 07. In the original implementation there is no multiplication even though in the PAMI06 paper there is..
  //float initAlp = float(2.0*R1_*(vcl_fabs(angleDiff_new(scurve1_->phi(0),scurve2_->phi(0)))+
  //                               vcl_fabs(angleDiff_new(scurve1_->phi(n1),scurve2_->phi(n2))))/2.0);
  
  //same as Matching-Tek
  float initAlp = float(R1_*(vcl_fabs(angleDiff_new(scurve1_->phi(0),scurve2_->phi(0)))+
                             vcl_fabs(angleDiff_new(scurve1_->phi(n1),scurve2_->phi(n2))))/2.0);
  
  return initAlp; 
}

double dbskr_dpmatch::recompute_final_cost()
{
  double total_cost = 0;

  for (unsigned i=0; i<_finalMap.size()-1 ; i++)
    total_cost += computeIntervalCost(_finalMap[i].first, _finalMap[i+1].first, _finalMap[i].second, _finalMap[i+1].second);

  return (total_cost + init_dr() + init_phi());
}

//: ozge added this function to be used for coarse tree matching (includes init_dr and init_phi)
float dbskr_dpmatch::coarse_match()
{
  // create the final map as the diagonal in the DPMatch table
  _finalMap.clear();
  vcl_pair <int,int> p(_n1-1,_n2-1);
  _finalMap.push_back(p);
  if (_n1 < _n2) {
    int rat = _n2/_n1;
    for (int j = _n1-2, k = _n2-1-rat; (j > 0 && k > 0); j--, k -= rat) {
      vcl_pair <int,int> p(j,k);
      _finalMap.push_back(p);
    }
  } else {
    int rat = _n1/_n2;
    for (int k = _n2-2, j = _n1-1-rat; (j > 0 && k > 0); k--, j -= rat) {
      vcl_pair <int,int> p(j,k);
      _finalMap.push_back(p);
    }
  }

  p.first = 0;
  p.second = 0;
  _finalMap.push_back(p);

  //: find the cost
  return (float)recompute_final_cost(); 
}


