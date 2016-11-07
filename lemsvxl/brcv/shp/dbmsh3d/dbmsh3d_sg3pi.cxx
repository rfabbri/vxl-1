//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/algo/dbmsh3d_sg_scan.cxx
//-------------------------------------------------------------------------

#include <vcl_cfloat.h>
#include <vcl_iostream.h>
#include <vnl/vnl_math.h>
#include <vul/vul_printf.h>
#include <vnl/vnl_vector_fixed.h>

#include <dbgl/dbgl_dist.h>
#include <dbgl/dbgl_triangle.h>
#include <dbgl/algo/dbgl_distance.h>
#include <dbgl/algo/dbgl_curve_smoothing.h>
#include <dbmsh3d/dbmsh3d_sg3pi.h>

int dbmsh3d_sg3pi::get_num_points () const
{
  int total = 0;
  for (unsigned int i=0; i<data_.size(); i++)
    total += data_[i].size();
  return total;
}

int dbmsh3d_sg3pi::get_max_pn () const
{
  int max_pn = 0;
  for (unsigned int i=0; i<data_.size(); i++) {
    for (unsigned int j=0; j<data_[i].size(); j++) {
      dbmsh3d_sg3pi_pt* SP = data_[i][j];
      if (SP->pn() > max_pn)
        max_pn = SP->pn();
    }
  }
  return max_pn;
}

//: Estimate intra- and inter-scanline sample distance.
void dbmsh3d_sg3pi::get_sl_sample_dist ()
{
  if (intra_sl_dist_ > 0 && inter_sl_dist_ > 0)
    return;
  vul_printf (vcl_cout, "get_sl_sample_dist():\n");
  assert (data_.size() > 0);

  //Store all intra-scanline dists to a vector.
  vcl_vector<double> sqdists;
  for (unsigned int i=0; i<data_.size(); i++) {
    for (int j=0; j<int(data_[i].size())-1; j++) {
      double sqd = dbgl_sqdist_3d (data_[i][j]->pt(), data_[i][j+1]->pt());
      sqdists.push_back (sqd);
    }
  }

  //Compute the median of sqdists.
  vcl_nth_element (sqdists.begin(),
                   sqdists.begin() + int(sqdists.size()/2), 
                   sqdists.end());
  double median = *(sqdists.begin() + int(sqdists.size()/2));
  intra_sl_dist_ = (float) vcl_sqrt (median);
  vul_printf (vcl_cout, "  avg. intra-scanline sample dist = %f\n", intra_sl_dist_);

  if (data_.size() < 2)
    return;

  //Store all min_sq_dists between samples of two scanlines int a vector.
  sqdists.clear();
  for (int i=0; i<int(data_.size())-1; i++) {
    vcl_vector<dbmsh3d_sg3pi_pt*> scanline0 = data_[i];
    vcl_vector<dbmsh3d_sg3pi_pt*> scanline1 = data_[i+1];

    //Only add this min_sq_dist if both scanlines are non-empty.
    if (scanline0.size() != 0 && scanline1.size() != 0) {
      double min_sq_dist = _min_sqd_between_scanlines (scanline0, scanline1);
      sqdists.push_back (min_sq_dist);
    }
  }

  //Compute the median of sqdists.
  vcl_nth_element (sqdists.begin(),
                   sqdists.begin() + int(sqdists.size()/2), 
                   sqdists.end());
  median = *(sqdists.begin() + int(sqdists.size()/2));
  sqdists.clear();
  inter_sl_dist_ = (float) vcl_sqrt (median);
  vul_printf (vcl_cout, "  avg. inter-scanline sample dist = %f\n", inter_sl_dist_);
}

//: Estimate the range coordinate system Vx, Vy, Vz for this scan.
//  compute Vx by averaging (last_pt - first_pt) vectors. 
//          Vy = averaging normals of scan-planes.
//          Vz = Vx cross Vy.
void dbmsh3d_sg3pi::estimate_range_coord ()
{
  vul_printf (vcl_cout, "  estimate_range_coord():\n");

  vgl_vector_3d<double> sumx, sumy, normal;
  unsigned count = 0;

  for (unsigned int i=0; i<data_.size(); i++) {
    int n = data_[i].size()-1;
    if (n < 2)
      continue;

    sumx += data_[i][n]->pt() - data_[i][0]->pt();
    count++;

    //find the farthest point from the (first_pt, last_pt) line.
    normal = _compute_scanline_normal (data_[i]);
   
    if (i > 0) //need to flip the normal to make it consistent.
      if (dot_product (normal, sumy) < 0)
        normal = -normal;
    sumy += normal;
  }  
  Vx_ = sumx / sumx.length();
  Vy_ = sumy / sumy.length();
  Vz_ = cross_product (Vx_, Vy_);

  //Use the intrinsic coord.
  Vx_.set (1, 0, 0);
  Vy_.set (0, 1, 0);
  Vz_.set (0, 0, 1);

  vul_printf (vcl_cout, "\tVx: (%lf, %lf, %lf)\n", Vx_.x(), Vx_.y(), Vx_.z());  
  vul_printf (vcl_cout, "\tVy: (%lf, %lf, %lf)\n", Vy_.x(), Vy_.y(), Vy_.z());  
  vul_printf (vcl_cout, "\tVz: (%lf, %lf, %lf)\n", Vz_.x(), Vz_.y(), Vz_.z());  
}

void dbmsh3d_sg3pi::compute_range ()
{
  bool bfirst = true;
  vgl_point_3d<double> O;

  //For each scan point, compute and store the depth value.
  for (unsigned int i=0; i<data_.size(); i++) {
    for (unsigned int j=0; j<data_[i].size(); j++) {
      dbmsh3d_sg3pi_pt* SP = data_[i][j];

      if (bfirst) { //Use the first available point as origin, i.e. it's range = 0.
        O = SP->pt();
        SP->set_depth (0.0f);
        bfirst = false;
        continue;
      }

      double t = dot_product (SP->pt() - O, Vz_);
      SP->set_depth ((float) t);
    }
  }
}

// #################################################################

double _min_sqd_between_scanlines (vcl_vector<dbmsh3d_sg3pi_pt*>& scanline0, 
                                   vcl_vector<dbmsh3d_sg3pi_pt*>& scanline1)
{
  double min_sq_dist = 1000000;

  for (unsigned int i=0; i<scanline0.size(); i++) {
    for (unsigned int j=0; j<scanline1.size(); j++) {
      double sqd = dbgl_sqdist_3d (scanline0[i]->pt(), scanline1[j]->pt());
      if (sqd < min_sq_dist)
        min_sq_dist = sqd;
    }
  }
  
  return min_sq_dist;
}

vgl_vector_3d<double> _compute_scanline_normal (const vcl_vector<dbmsh3d_sg3pi_pt*>& scanline)
{
  assert (scanline.size() > 2);
  vgl_point_3d<double> A = scanline[0]->pt();
  vgl_point_3d<double> B = scanline[scanline.size()-1]->pt();
  vgl_vector_3d<double> AB = B - A; //vector vx

  //Find the farthest point P
  vgl_point_3d<double> Pmax;
  double dmax = 0;
  for (unsigned int i=1; i<scanline.size()-1; i++) {
    vgl_point_3d<double> P = scanline[i]->pt();
    double t = dot_product (P-A, AB) / vgl_distance (A, B);
    double d = vcl_sqrt (dbgl_sqdist_3d (A, P) - t*t);
    if (d > dmax) {
      dmax = d;
      Pmax = P;
    }
  }

  //compute normal from the farthest point from the (first_pt, last_pt) line.
  vgl_vector_3d<double> AP = Pmax - scanline[0]->pt();
  return cross_product (AP, AB);
}

//: for input scan point SP, find the two closest points on the given scanline
//  m, n are the indices of two points containing the footpt of SP.
bool _get_closest_pts (const vcl_vector<dbmsh3d_sg3pi_pt*>& scanline, 
                       const dbmsh3d_sg3pi_pt* SP, const double& intra_scanline_th,
                       int& m, int& n)
{
  double t, l;

  for (int i=0; i<int(scanline.size())-1; i++) {
    dbmsh3d_sg3pi_pt* SA = scanline[i];
    dbmsh3d_sg3pi_pt* SB = scanline[i+1];
    bool test_foot = dbgl_footpt_on_line (SP->pt(), SA->pt(), SB->pt(), t, l);
    bool test_size = l < intra_scanline_th && 
                     vgl_distance (SP->pt(), SA->pt()) < intra_scanline_th && 
                     vgl_distance (SP->pt(), SB->pt()) < intra_scanline_th;
    if (test_foot && test_size) {
      m = i;
      n = i+1;
      return true;
    }
  }
  return false;
}

//: Add the closest point on the scanline within dist_th to the kernel.
void _add_to_kernel_1 (const dbmsh3d_sg3pi_pt* SP, const double& dist_th,
                       const vcl_vector<dbmsh3d_sg3pi_pt*>& scanline,
                       vcl_vector<float>& kernel)
{
  if (scanline.size() == 0)
    return;

  double min_sqd = FLT_MAX;
  unsigned int min_i;
  for (unsigned int i=0; i<scanline.size(); i++) {
    double sqd = dbgl_sqdist_3d (SP->pt(), scanline[i]->pt());
    if (sqd < min_sqd) {
      min_sqd = sqd;
      min_i = i;
    }
  }

  if (vcl_sqrt (min_sqd) < dist_th)
    kernel.push_back (scanline[min_i]->depth());
}

//: Brute-force add neighboring scan points within threshold.
void _add_to_kernel_2 (const dbmsh3d_sg3pi_pt* SP, const double& dist_th,
                       const vcl_vector<dbmsh3d_sg3pi_pt*>& scanline,
                       vcl_vector<float>& kernel)
{
  if (scanline.size() == 0)
    return;

  for (unsigned int i=0; i<scanline.size(); i++) {
    double d = vgl_distance (SP->pt(), scanline[i]->pt());
    if (d < dist_th)
      kernel.push_back (scanline[i]->depth());
  }
}
