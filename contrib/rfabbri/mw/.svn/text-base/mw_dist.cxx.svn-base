#include "mw_dist.h"

double mw_dist::
hdist(vcl_vector<mw_vector_3d> &c1, vcl_vector<mw_vector_3d> &c2)
{
  double hd12 = dir_hausdorff_distance(c1,c2);
  double hd21 = dir_hausdorff_distance(c2,c1);

  if (hd12 > hd21)
    return hd12;
  
  return hd21;
}


double mw_dist::
dir_hausdorff_distance(vcl_vector<mw_vector_3d> &c1, vcl_vector<mw_vector_3d> &c2)
{
  double d=0;

  for (unsigned i=0; i<c1.size(); ++i) {
    double dmin = vcl_numeric_limits<double>::infinity();
    double px = c1[i][0];
    double py = c1[i][1];
    double pz = c1[i][2];

    for (unsigned k=0; k < c2.size(); ++k) {

      double d_tmp;
      double qx = c2[k][0];
      double qy = c2[k][1];
      double qz = c2[k][2];
      d_tmp = (px - qx)*(px - qx) + 
              (py - qy)*(py - qy) + 
              (pz - qz)*(pz - qz);

      if (dmin > d_tmp)
        dmin = d_tmp;
    }

    if (dmin > d)
      d = dmin;
  }

  return d;
}
