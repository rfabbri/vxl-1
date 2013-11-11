#include <dbskr/dbskr_dpmatch_combined.h>
#include <dbskr/dbskr_scurve.h>

#include <vcl_iostream.h>

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
    dF = dF+vcl_fabs(ds1_[k]-lambda_[k]*ds2_[k]);
  
  for (k=0;k<num_cost_elems_;k++)
    dK = dK+vcl_fabs(dt1_[k]-dt2_[k]);

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

  vcl_cout.precision(2);
  vcl_cout << "\tds1+: " << ds1_[0] << "\tds1-: " << ds1_[1] << "\tdr1*sin(phi1): " << ds1_[2] << "\n";
  vcl_cout.precision(2);
  vcl_cout << "\tds2+: " << ds2_[0] << "\tds2-: " << ds2_[1] << "\tdr2*sin(phi2): " << ds2_[2] << "\n";

  for (k=0;k<num_cost_elems_;k++) 
    dF = dF+vcl_fabs(ds1_[k]-lambda_[k]*ds2_[k]);
  
  vcl_cout.precision(2);
  vcl_cout << "\tdS+: " << vcl_fabs(ds1_[0]-lambda_[0]*ds2_[0]) << " dS-: " << vcl_fabs(ds1_[1]-lambda_[1]*ds2_[1]) << " ";
  vcl_cout.precision(2);
  vcl_cout << "dR: " << vcl_fabs(ds1_[2]-lambda_[2]*ds2_[2]) << " dS = dS+ + dS- + dR = " << dF << "\n";

  vcl_cout.precision(2);
  vcl_cout << "\tdt1+: " << dt1_[0] << " dt1-: " << dt1_[1] << " dphi1: " << dt1_[2] << "\n";
  vcl_cout.precision(2);
  vcl_cout << "\tdt2+: " << dt2_[0] << " dt2-: " << dt2_[1] << " dphi2: " << dt2_[2] << "\n";

  for (k=0;k<num_cost_elems_;k++) {
    dK = dK+vcl_fabs(dt1_[k]-dt2_[k]);
  }

  vcl_cout.precision(2);
  vcl_cout << "\tdT+: " << vcl_fabs(dt1_[0]-dt2_[0]) << " dT-: " << vcl_fabs(dt1_[1]-dt2_[1]) << " ";
  vcl_cout.precision(2);
  vcl_cout << "dPhi: " << vcl_fabs(dt1_[2]-dt2_[2]) << " dT = dT+ + dT- + dPhi = " << dK << "\n";
  vcl_cout.precision(2);
  vcl_cout << "\t\tR1*dT: " << R1_*dK << " dS+R1*dT: " << dF+R1_*dK << vcl_endl;

  cost = dF+R1_*dK;
  return cost;
}

//: new initial dr after combination of width and orientation
float dbskr_dpmatch_combined::init_dr() 
{ 
  //Amir: changed this function to reflect the changes in the cost function
  float initDr = float(2*vcl_fabs(scurve1_->time(0)*vcl_sin(scurve1_->phi(0)) - 
                       lambda_[2]*scurve2_->time(0)*vcl_sin(scurve2_->phi(0))));

  return initDr; 
}

//: new initial phi is just zero, this function is kept to preserve interface
float dbskr_dpmatch_combined::init_phi() 
{ 
  return 0.0f; 
}

