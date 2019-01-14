#include <dbskr/dbskr_dpmatch_combined.h>
#include <dbskr/dbskr_scurve.h>

#include <iostream>

// Cost of matching the  interval [x(i),x(ip)] to the interval [y(j),y(jp)].
double dbskr_dpmatch_combined::computeIntervalCost(int i, int ip, int j, int jp) 
{
  double cost,dF=0,dK=0;

  int k;
  scurve1_->stretch_cost_combined(i,ip,ds1_);
  scurve2_->stretch_cost_combined(j,jp,ds2_);
  scurve1_->bend_cost_combined(i,ip,dt1_);
  scurve2_->bend_cost_combined(j,jp,dt2_);

  for (k=0;k<num_cost_elems_;k++)
    dF = dF+std::fabs(ds1_[k]-lambda_[k]*ds2_[k]);
  
  for (k=0;k<num_cost_elems_;k++)
    dK = dK+std::fabs(dt1_[k]-dt2_[k]);

  cost = dF+R1_*dK;
  return cost;
} 

double dbskr_dpmatch_combined::computeIntervalCostPrint(int i, int ip, int j, int jp)
{
  double cost,dF=0,dK=0;
  int k;
  scurve1_->stretch_cost_combined(i,ip,ds1_);
  scurve2_->stretch_cost_combined(j,jp,ds2_);
  scurve1_->bend_cost_combined(i,ip,dt1_);
  scurve2_->bend_cost_combined(j,jp,dt2_);

  std::cout.precision(2);
  std::cout << "\tds1+: " << ds1_[0] << "\tds1-: " << ds1_[1] << "\tdr1*sin(phi1): " << ds1_[2] << "\n";
  std::cout.precision(2);
  std::cout << "\tds2+: " << ds2_[0] << "\tds2-: " << ds2_[1] << "\tdr2*sin(phi2): " << ds2_[2] << "\n";

  for (k=0;k<num_cost_elems_;k++) 
    dF = dF+std::fabs(ds1_[k]-lambda_[k]*ds2_[k]);
  
  std::cout.precision(2);
  std::cout << "\tdS+: " << std::fabs(ds1_[0]-lambda_[0]*ds2_[0]) << " dS-: " << std::fabs(ds1_[1]-lambda_[1]*ds2_[1]) << " ";
  std::cout.precision(2);
  std::cout << "dR: " << std::fabs(ds1_[2]-lambda_[2]*ds2_[2]) << " dS = dS+ + dS- + dR = " << dF << "\n";

  std::cout.precision(2);
  std::cout << "\tdt1+: " << dt1_[0] << " dt1-: " << dt1_[1] << " dphi1: " << dt1_[2] << "\n";
  std::cout.precision(2);
  std::cout << "\tdt2+: " << dt2_[0] << " dt2-: " << dt2_[1] << " dphi2: " << dt2_[2] << "\n";

  for (k=0;k<num_cost_elems_;k++) {
    dK = dK+std::fabs(dt1_[k]-dt2_[k]);
  }

  std::cout.precision(2);
  std::cout << "\tdT+: " << std::fabs(dt1_[0]-dt2_[0]) << " dT-: " << std::fabs(dt1_[1]-dt2_[1]) << " ";
  std::cout.precision(2);
  std::cout << "dPhi: " << std::fabs(dt1_[2]-dt2_[2]) << " dT = dT+ + dT- + dPhi = " << dK << "\n";
  std::cout.precision(2);
  std::cout << "\t\tR1*dT: " << R1_*dK << " dS+R1*dT: " << dF+R1_*dK << std::endl;

  cost = dF+R1_*dK;
  return cost;
}

//: new initial dr after combination of width and orientation
float dbskr_dpmatch_combined::init_dr() 
{ 
  //Amir: changed this function to reflect the changes in the cost function
  float initDr = float(2*std::fabs(scurve1_->time(0)*std::sin(scurve1_->phi(0)) - 
                       lambda_[2]*scurve2_->time(0)*std::sin(scurve2_->phi(0))));

  return initDr; 
}

//: new initial phi is just zero, this function is kept to preserve interface
float dbskr_dpmatch_combined::init_phi() 
{ 
  return 0.0f; 
}

