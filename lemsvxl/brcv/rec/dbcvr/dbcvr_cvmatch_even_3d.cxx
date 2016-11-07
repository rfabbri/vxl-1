#include "dbcvr_cvmatch_even_3d.h"

#include <dbsol/dbsol_interp_curve_3d.h>
#include <dbnl/algo/dbnl_eno_third_order.h>
#include <vnl/vnl_math.h>

#include <vcl_cmath.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_utility.h>

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cstdio.h>

dbcvr_cvmatch_even_3d::dbcvr_cvmatch_even_3d()
{
  R1_ = 10;
  R2_ = 10;
  normalized_stretch_cost_ = false;
  setTemplateSize(3);
}

dbcvr_cvmatch_even_3d::dbcvr_cvmatch_even_3d(dbsol_interp_curve_3d *c1, 
                                             dbsol_interp_curve_3d *c2, 
                                             int n1, int n2, 
                                             double R1, double R2, 
                                             int template_size, int cost_formula_type,
                                             int ang_der_comp_type)
{
  ang_der_comp_type_ = ang_der_comp_type;
  cost_formula_type_ = cost_formula_type;
  curve1_ = c1;
  curve2_ = c2;
  R1_ = R1;
  R2_ = R2;

  n1_ = n1;
  n2_ = n2;

  //: we want n1 points on the first curve including first and last points --> n1 - 1 intervals
  // i.e. curves will be sampled with this arclength, (n1_-1)*delta_s1_ = L1
  delta_s1_ = curve1_->length()/(n1_-1);
  delta_s2_ = curve2_->length()/(n2_-1); 

  normalized_stretch_cost_ = false;
  setTemplateSize(template_size);
}

dbcvr_cvmatch_even_3d::~dbcvr_cvmatch_even_3d()
{
}

void dbcvr_cvmatch_even_3d::setTemplateSize(int temp_size)
{
  XOFFSET.clear();
  YOFFSET.clear();
  
  if (temp_size == 1) 
  {  // 1x1 template
    template_size_ = 3;

    int dummyX[3] = {-1, 0,-1};
    int dummyY[3] = {-1,-1, 0};
    
    for (int i = 0; i<3; i++) 
    {
      XOFFSET.push_back(dummyX[i]); 
      YOFFSET.push_back(dummyY[i]); 
    }

  } 
  else if (temp_size == 3) 
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

// ###########################################################
//          MATCH
// ###########################################################

void dbcvr_cvmatch_even_3d::Match()
{
  initializeDPCosts();
  computeDPCosts ();
  findDPCorrespondence ();
}

// ###########################################################
//          DP Cost
// ###########################################################

void dbcvr_cvmatch_even_3d::initializeDPCosts()
{
  DPCost_.clear();
  DPMap_.clear();

  assert (n1_>0);
  assert (n2_>0);

  for (int i=0;i<n1_;i++) 
  {
    vcl_vector<double> tmp1(n2_,DP_VERY_LARGE_COST);
    DPCost_.push_back(tmp1);
    vcl_pair <int,int> tmp3(0,0);
    vcl_vector< vcl_pair <int,int> > tmp2(n2_,tmp3);
    DPMap_.push_back(tmp2);
  }

  //Cost Matrix Initialization
  finalCost_ = DP_VERY_LARGE_COST;
  for (int n=0;n<n1_;n++) 
  {
    for (int m=0;m<n2_;m++)
    {
      DPCost_[n][m]=DP_VERY_LARGE_COST;
    }
  }

  DPCost_[0][0]=0.0;

  // lengths
  for (int i = 0; i<n1_; i++) 
    curve1_lengths_.push_back(i*delta_s1_);
  for (int i = 0; i<n2_; i++) 
    curve2_lengths_.push_back(i*delta_s2_);
  
  // curvatures (value independent of any parametrization)
  for (int i = 0; i<n1_; i++) 
    curve1_curvatures_.push_back(curve1_->curvature_at(curve1_lengths_[i]));
  for (int i = 0; i<n2_; i++)
    curve2_curvatures_.push_back(curve2_->curvature_at(curve2_lengths_[i]));
  
  // torsions (value independent of any parametrization)
  for (int i = 0; i<n1_; i++) 
    curve1_torsions_.push_back(curve1_->torsion_at(curve1_lengths_[i]));
  for (int i = 0; i<n2_; i++)
    curve2_torsions_.push_back(curve2_->torsion_at(curve2_lengths_[i]));
  
  if(cost_formula_type_ == INTRINSIC)
  {
    // curve 1 angles (value independent of any parametrization)
    for (int i = 0; i<n1_; i++)
    {
      struct tangent_angles tan_ang = curve1_->tangent_angles_at(curve1_lengths_[i]);
      curve1_phis_.push_back(tan_ang.phi_);
      curve1_thetas_.push_back(tan_ang.theta_);
    }
    // curve 2 angles (value independent of any parametrization)
    for (int i = 0; i<n2_; i++)
    {
      struct tangent_angles tan_ang = curve2_->tangent_angles_at(curve2_lengths_[i]);
      curve2_phis_.push_back(tan_ang.phi_);
      curve2_thetas_.push_back(tan_ang.theta_);
    }
  }

  if(cost_formula_type_ == INTRINSIC && ang_der_comp_type_ == USE_FORMULAS)
  {
    // curve1 phi_s and theta_s formulated computations
    for(int i=0; i<n1_; i++)
    {
      vgl_vector_3d<double> N = curve1_->normal_at(curve1_lengths_[i]);
      double k = curve1_curvatures_[i];
      double phi = curve1_phis_[i];
      double theta = curve1_thetas_[i];
      vgl_vector_3d<double> Ts(k * N);
      double phi_s = -Ts.z() / (vcl_sin(phi));
      double theta_s = (Ts.y()*vcl_cos(theta) - Ts.x()*vcl_sin(theta)) / vcl_sin(phi);
      curve1_dphis_.push_back(phi_s);
      curve1_dthetas_.push_back(theta_s);
    }
    // curve2 phi_s and theta_s formulated computations
    for(int i=0; i<n2_; i++)
    {
      vgl_vector_3d<double> N = curve2_->normal_at(curve2_lengths_[i]);
      double k = curve2_curvatures_[i];
      double phi = curve2_phis_[i];
      double theta = curve2_thetas_[i];
      vgl_vector_3d<double> Ts(k * N);
      double phi_s = -Ts.z() / (vcl_sin(phi));
      double theta_s = (Ts.y()*vcl_cos(theta) - Ts.x()*vcl_sin(theta)) / vcl_sin(phi);
      curve2_dphis_.push_back(phi_s);
      curve2_dthetas_.push_back(theta_s);
    }
  }
  else if(cost_formula_type_ == INTRINSIC && ang_der_comp_type_ == USE_ENO)
  {
    // adjust the angles to be continuous so that ENO works fine
    continuous_angles(curve1_phis_);
    continuous_angles(curve1_thetas_);
    continuous_angles(curve2_phis_);
    continuous_angles(curve2_thetas_);
    // ENO on phi of curve1
    int interval_index;
    double t, dphi, dtheta;
    vcl_vector<double> points, arclengths;
    double a0, a1, a2, a3;
    for(int i = 0; i < n1_-1; i++)
    {
      interval_index = pick_points_for_eno(curve1_phis_, points, curve1_lengths_, arclengths, i);
      dbnl_eno_third_order eno;
      eno.interpolate(arclengths, points);
      a0 = eno.coefficient(interval_index, 0);
      a1 = eno.coefficient(interval_index, 1);
      a2 = eno.coefficient(interval_index, 2);
      a3 = eno.coefficient(interval_index, 3);
      t = arclengths[interval_index];
      dphi = a1 + 2 * a2 * t + 3 * a3 * vcl_pow(t, 2.0);
      curve1_dphis_.push_back(dphi);
    }
    // deal with last point
    t = arclengths[interval_index+1];
    dphi = a1 + 2 * a2 * t + 3 * a3 * vcl_pow(t, 2.0);
    curve1_dphis_.push_back(dphi);

    // ENO on theta of curve1
    for(int i = 0; i < n1_-1; i++)
    {
      interval_index = pick_points_for_eno(curve1_thetas_, points, curve1_lengths_, arclengths, i);
      dbnl_eno_third_order eno;
      eno.interpolate(arclengths, points);
      a0 = eno.coefficient(interval_index, 0);
      a1 = eno.coefficient(interval_index, 1);
      a2 = eno.coefficient(interval_index, 2);
      a3 = eno.coefficient(interval_index, 3);
      t = arclengths[interval_index];
      dtheta = a1 + 2 * a2 * t + 3 * a3 * vcl_pow(t, 2.0);
      curve1_dthetas_.push_back(dtheta);
    }
    // deal with last point
    t = arclengths[interval_index+1];
    dtheta = a1 + 2 * a2 * t + 3 * a3 * vcl_pow(t, 2.0);
    curve1_dthetas_.push_back(dtheta);

    // ENO on phi of curve2
    for(int i = 0; i < n2_-1; i++)
    {
      interval_index = pick_points_for_eno(curve2_phis_, points, curve2_lengths_, arclengths, i);
      dbnl_eno_third_order eno;
      eno.interpolate(arclengths, points);
      a0 = eno.coefficient(interval_index, 0);
      a1 = eno.coefficient(interval_index, 1);
      a2 = eno.coefficient(interval_index, 2);
      a3 = eno.coefficient(interval_index, 3);
      t = arclengths[interval_index];
      dphi = a1 + 2 * a2 * t + 3 * a3 * vcl_pow(t, 2.0);
      curve2_dphis_.push_back(dphi);
    }
    // deal with last point
    t = arclengths[interval_index+1];
    dphi = a1 + 2 * a2 * t + 3 * a3 * vcl_pow(t, 2.0);
    curve2_dphis_.push_back(dphi);

    // ENO on theta of curve2
    for(int i = 0; i < n2_-1; i++)
    {
      interval_index = pick_points_for_eno(curve2_thetas_, points, curve2_lengths_, arclengths, i);
      dbnl_eno_third_order eno;
      eno.interpolate(arclengths, points);
      a0 = eno.coefficient(interval_index, 0);
      a1 = eno.coefficient(interval_index, 1);
      a2 = eno.coefficient(interval_index, 2);
      a3 = eno.coefficient(interval_index, 3);
      t = arclengths[interval_index];
      dtheta = a1 + 2 * a2 * t + 3 * a3 * vcl_pow(t, 2.0);
      curve2_dthetas_.push_back(dtheta);
    }
    // deal with last point
    t = arclengths[interval_index+1];
    dtheta = a1 + 2 * a2 * t + 3 * a3 * vcl_pow(t, 2.0);
    curve2_dthetas_.push_back(dtheta);
  }
  else if(cost_formula_type_ == INTRINSIC)
  {
    vcl_cout << "Error: Angle derivatives computation type not well specified, quitting!" << vcl_endl;
    exit(-1);
  }
}

// Cost of matching the interval [x(ip),x(i)]  to [y(jp),y(j)].
double dbcvr_cvmatch_even_3d::computeIntervalCost(int i, int ip, int j, int jp)
{
  // compute stretch cost
  double sc1 = curve1_lengths_[i] - curve1_lengths_[ip];
  double sc2 = curve2_lengths_[j] - curve2_lengths_[jp];
  double stretch_cost = vcl_fabs(sc1 - sc2);

  double bend_cost, twist_cost;

  if(cost_formula_type_ == EXTRINSIC)
  {
    // compute extrinsic bend cost
    double bc1 = (curve1_curvatures_[i] + curve1_curvatures_[ip])/ 2;
    double bc2 = (curve2_curvatures_[j] + curve2_curvatures_[jp])/ 2;
    bend_cost = vcl_fabs(bc1*sc1 - bc2*sc2);
    // compute extrinsic twist cost
    double tc1 = (curve1_torsions_[i] + curve1_torsions_[ip])/ 2;
    double tc2 = (curve2_torsions_[j] + curve2_torsions_[jp])/ 2;
    twist_cost = vcl_fabs(tc1*sc1 - tc2*sc2);
  }
  else if(cost_formula_type_ == INTRINSIC)
  {
    // compute necessary values for intrinsic bend and twist costs
    double phi1 = (curve1_phis_[i] + curve1_phis_[ip])/2;
    double d_phi1 = curve1_phis_[i] - curve1_phis_[ip];
    double d_theta1 = curve1_thetas_[i] - curve1_thetas_[ip];
    double d_phi_s1 = curve1_dphis_[i] - curve1_dphis_[ip];
    double d_theta_s1 = curve1_dthetas_[i] - curve1_dthetas_[ip];

    double phi2 = (curve2_phis_[j] + curve2_phis_[jp])/2;
    double d_phi2 = curve2_phis_[j] - curve2_phis_[jp];
    double d_theta2 = curve2_thetas_[j] - curve2_thetas_[jp];
    double d_phi_s2 = curve2_dphis_[j] - curve2_dphis_[jp];
    double d_theta_s2 = curve2_dthetas_[j] - curve2_dthetas_[jp];

    // bending cost
    double bc1 = vcl_pow(d_phi1, 2.0) + vcl_pow(vcl_sin(phi1) * d_theta1, 2.0);
    double bc2 = vcl_pow(d_phi2, 2.0) + vcl_pow(vcl_sin(phi2) * d_theta2, 2.0);
    bend_cost = vcl_fabs(bc1-bc2);

    // twist cost
    double temp1 = 2*vcl_cos(phi1) * d_theta1 * vcl_pow(d_phi1, 2.0) + vcl_sin(phi1) * d_phi1 * d_theta_s1 * sc1;
    double temp2 = vcl_sin(phi1) * d_theta1 * (-d_phi_s1 * sc1 + vcl_sin(phi1) * vcl_cos(phi1) * vcl_pow(d_theta1, 2.0));
    double temp3 = vcl_pow(d_phi1, 2.0) + vcl_pow(vcl_sin(phi1) * d_theta1, 2.0);
    double tc1 = (temp1 + temp2) / temp3;

    double temp4 = 2*vcl_cos(phi2) * d_theta2 * vcl_pow(d_phi2, 2.0) + vcl_sin(phi2) * d_phi2 * d_theta_s2 * sc2;
    double temp5 = vcl_sin(phi2) * d_theta2 * (-d_phi_s2 * sc2 + vcl_sin(phi2) * vcl_cos(phi2) * vcl_pow(d_theta2, 2.0));
    double temp6 = vcl_pow(d_phi2, 2.0) + vcl_pow(vcl_sin(phi2) * d_theta2, 2.0);
    double tc2 = (temp4 + temp5) / temp6;

    twist_cost = vcl_fabs(tc1-tc2);
  }
  else
  {
    vcl_cout << "Error: Cost computation formula type not well specified, quitting!" << vcl_endl;
  }
  double cost = stretch_cost + R1_ * bend_cost + R2_ * twist_cost;

  return cost;
}

void dbcvr_cvmatch_even_3d::computeDPCosts()
{  
  int sum,start,i,ip,j,jp,k;
  double cost;

  // adjust the angles to be continuous so that ENO works fine
  continuous_angles(curve1_phis_);
  continuous_angles(curve1_thetas_);
  continuous_angles(curve2_phis_);
  continuous_angles(curve2_thetas_);

  for (sum = 1; sum<n1_+n2_-1; sum++)
  {
    start=(int)curve_maxof(0,sum-n2_+1,-10000);
    for (i=start;(i<=n1_-1 && i<=sum);i++)
    {
      j=sum-i;
      for (k=0;k<template_size_;k++)
      { //TEMPLATE_SIZE=9 originally
        ip=i+XOFFSET[k];
        jp=j+YOFFSET[k];
        if (ip >= 0 &&  jp >=0) 
        {
          double incCost=computeIntervalCost(i,ip,j,jp);
          cost =DPCost_[ip][jp]+incCost;
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

// ###########################################################
//          AFTER DP, FIND MATCHING
// ###########################################################

void dbcvr_cvmatch_even_3d::findDPCorrespondence(void)
{
  int i, j, ip, jp;

  finalMap_.clear();          //Clean the table
  finalMap_Cost_.clear();

  finalCost_ = DPCost_[n1_-1][n2_-1];  //The final value of DPMap

  ip = n1_-1;
  jp = n2_-1;
  i = n1_-1;
  j = n2_-1;

  vcl_pair <int,int> p(ip,jp);
  finalMap_.push_back(p);
  finalMap_Cost_.push_back(DPCost_[p.first][p.second]);

  while (ip > 0 || jp > 0) 
  { //Ming: should be &&
    ip=DPMap_[i][j].first;
    jp=DPMap_[i][j].second;
    vcl_pair <int,int> p(ip,jp);
    finalMap_.push_back(p);
    finalMap_Cost_.push_back(DPCost_[p.first][p.second]);
  
    i=ip; //Go to the previous point
    j=jp;
  }

//  double sum = 0;
//  for(unsigned i=0; i<finalMap_.size()-1; i++)
//  {
//    int fir1 = finalMap_[i].first;
//    int sec1 = finalMap_[i].second;
//    int fir2 = finalMap_[i+1].first;
//    int sec2 = finalMap_[i+1].second;
//    vcl_printf("%d-%d and %d-%d: %f\n", fir1, fir2, sec1, sec2, computeIntervalCost(fir1, fir2, sec1, sec2));
//    sum += computeIntervalCost(fir1, fir2, sec1, sec2);
//  }
//  vcl_cout << "Sum: " << sum << vcl_endl;

//  write_data(curve1_curvatures_, "D:/MyDocs/courses/en256/project/code-and-data/Temp/match/k1.txt");
//  write_data(curve1_torsions_, "D:/MyDocs/courses/en256/project/code-and-data/Temp/match/t1.txt");
//  write_data(curve1_xs_, "D:/MyDocs/courses/en256/project/code-and-data/Temp/match/x1.txt");
//  write_data(curve1_ys_, "D:/MyDocs/courses/en256/project/code-and-data/Temp/match/y1.txt");
//  write_data(curve1_zs_, "D:/MyDocs/courses/en256/project/code-and-data/Temp/match/z1.txt");
//  write_data(curve1_phis_, "D:/MyDocs/courses/en256/project/code-and-data/Temp/match/phi1.txt");
//  write_data(curve1_dphis_, "D:/MyDocs/courses/en256/project/code-and-data/Temp/match/dphi1.txt");
//  write_data(curve1_thetas_, "D:/MyDocs/courses/en256/project/code-and-data/Temp/match/theta1.txt");
//  write_data(curve1_dthetas_, "D:/MyDocs/courses/en256/project/code-and-data/Temp/match/dtheta1.txt");
}

int dbcvr_cvmatch_even_3d::pick_points_for_eno(vcl_vector<double> &points, vcl_vector<double> &picked_points, 
                                               vcl_vector<double> &arclengths, vcl_vector<double> &picked_arclengths,
                                               int index)
{
  picked_points.clear();
  picked_arclengths.clear();

  int num_points = points.size();
  int interval_index;
  if(index - 2 >= 0)
  {
    picked_points.push_back(points[index-2]);
    picked_arclengths.push_back(arclengths[index-2]);
  }
  if(index - 1 >= 0)
  {
    picked_points.push_back(points[index-1]);
    picked_arclengths.push_back(arclengths[index-1]);
  }

  picked_points.push_back(points[index]);
  picked_arclengths.push_back(arclengths[index]);

  if(index + 1 < num_points)
  {
    picked_points.push_back(points[index+1]);
    picked_arclengths.push_back(arclengths[index+1]);
  }
  if(index + 2 < num_points)
  {
    picked_points.push_back(points[index+2]);
    picked_arclengths.push_back(arclengths[index+2]);
  }
  if(index + 3 < num_points)
  {
    picked_points.push_back(points[index+3]);
    picked_arclengths.push_back(arclengths[index+3]);
  }

  if(index - 1 < 0)
    interval_index = 0;
  else if(index - 2 < 0)
    interval_index = 1;
  else
    interval_index = 2;

  return interval_index;
}

void dbcvr_cvmatch_even_3d::continuous_angles(vcl_vector<double> &angles)
{
  int size = angles.size();
  for(int i=1; i<size; i++)
  {
    double diff = angles[i] - angles[i-1];
    if(diff < -vnl_math::pi)
      angles[i] += 2*vnl_math::pi;
    else if(diff > vnl_math::pi)
      angles[i] -= 2*vnl_math::pi;
    else
      1; // do nothing
  }
}

void dbcvr_cvmatch_even_3d::write_data(vcl_vector<double> &data, vcl_string fname)
{
  vcl_ofstream fp(fname.c_str());
  int size = data.size();
  for(int i=0; i<size; i++)
    fp << data[i] << vcl_endl;
}
