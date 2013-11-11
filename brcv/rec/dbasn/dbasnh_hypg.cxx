//---------------------------------------------------------------------
// This is brcv/rec/dbasnh/dbasnh_hypg.cxx
//:
// \file
// \brief Definition of a hypergraph structure
//
// \author
//  Ming-Ching Chang - January 6, 2008
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vcl_algorithm.h>
#include <vgl/vgl_box_3d.h>
#include <vul/vul_printf.h>

#include <dbasn/dbasnh_hypg.h>

//Allocate adjacency matrix for links
void dbasnh_hypg::alloc_corners_angle ()
{
  const unsigned int nN = nodes_.size();
  corners_ = new float**[nN];
  for (unsigned int i = 0; i<nN; i++) {
    corners_[i] = new float*[nN];
    for (unsigned int j = 0; j<nN; j++)
      corners_[i][j] = new float[nN];
  }

  //initialize corner[][][] to 0
  for (unsigned int i=0; i<nN; i++) {
    for (unsigned int j=0; j<nN; j++) {
      for (unsigned int k=0; k<nN; k++) {
        corners_[i][j][k] = 0;
      }
    }
  }
}

int _factorial (int x) {
 int fac = 1;
 for (int i=2; i<=x; i++) fac *= i;
 return fac;
}

#define nCr(n, r) (_factorial(n) / _factorial(n-r) / _factorial(r))
#define nPr(n, r) (_factorial(n) / _factorial(n-r))

int dbasnh_hypg::count_max_corners ()
{
  //Loop through each node and check all incident links for total number of corners.
  int n_total = 0;
  for (unsigned int i=0; i<nodes_.size(); i++) {
    dbasn_node* N = nodes_[i];
    int n_conn = N->conn_nids().size();
    //n_corners = C_n_2
    int n_corners = nCr (n_conn, 2);
    n_total += n_corners;
  }
  return n_total;
}

//: We have to re-define the link normalization here! because the degeneracy is not handled!
//  We might have very long A3 rib that is not suitable for curve length matching.
//   - possible sol: Normalize against the longest A13 Axial curve?

void dbasnh_hypg::normalize_corner_cost (const double max, const int verbose)
{
  if (max == 0)
    return; //avoid divide by zero (can't happen).

  if (verbose)
    vul_printf (vcl_cout, "  %s: normalize_corner_cost() max = %lf.\n", 
                b_1st_graph_ ? "G" : "g", max);

  int count = 0;
  const unsigned int nN = nodes_.size();
  for (unsigned int i=0; i<nN; i++) {
    for (unsigned int j=0; j<nN; j++) {
      for (unsigned int k=0; k<nN; k++) {
        if (corners_[i][j][k] !=0) {
          assert (corners_[k][j][i] != 0);
          corners_[i][j][k] /= max;
          count++;
        }
      }
    }
  }

  if (verbose>1)
    print_corner_cost();

  //In some rare but possible case this not true.
  //For example, a corner of [i][j][k=i] exists.
  ///assert (n_corners_ * 2 == count);
}

void dbasnh_hypg::print_corner_cost ()
{
  vul_printf (vcl_cout, "\n%s->print_corner_cost():\n", b_1st_graph_ ? "G" : "g");
  int count = 0;
  const unsigned int nN = nodes_.size();
  for (unsigned int i=0; i<nN; i++)
    for (unsigned int j=0; j<nN; j++) {
      for (unsigned int k=0; k<nN; k++) {
        if (corners_[i][j][k] != 0) {
          vul_printf (vcl_cout, "corners[%d][%d][%d]=%2.2f \n", i, j, k, corners_[i][j][k]);
          count++;
        }
      }
    }
  vul_printf (vcl_cout, "# of corners: %d.\n", count);
}

//##################################################################

//Allocate adjacency matrix for links
void dbasnh_hypg_aug::alloc_corner_nf_grad_r ()
{
  const unsigned int nN = nodes_.size();
  corner_nf_ = new float**[nN];
  corner_grad_r_ = new float**[nN];
  for (unsigned int i = 0; i<nN; i++) {
    corner_nf_[i] = new float*[nN];
    corner_grad_r_[i] = new float*[nN];
    for (unsigned int j = 0; j<nN; j++) {
      corner_nf_[i][j] = new float[nN];
      corner_grad_r_[i][j] = new float[nN];
    }
  }

  //initialize corner[][][] to 0
  for (unsigned int i=0; i<nN; i++) {
    for (unsigned int j=0; j<nN; j++) {
      for (unsigned int k=0; k<nN; k++) {
        corner_nf_[i][j][k] = 0;
        corner_grad_r_[i][j][k] = 0;
      }
    }
  }
}


float dbasnh_hypg_aug::max_of_grad_r_max ()
{
  float max = 0;
  for (unsigned int i=0; i<nodes_.size(); i++) {
    if (N_aug(i)->grad_r_max() > max)
      max = N_aug(i)->grad_r_max();
  }
  return max;
}

float dbasnh_hypg_aug::max_of_grad_r_min ()
{
  float max = 0;
  for (unsigned int i=0; i<nodes_.size(); i++) {
    if (N_aug(i)->grad_r_min() > max)
      max = N_aug(i)->grad_r_min();
  }
  return max;
}

float dbasnh_hypg_aug::max_of_corner_a_max ()
{
  float max = 0;
  for (unsigned int i=0; i<nodes_.size(); i++) {
    if (N_aug(i)->corner_a_max() > max)
      max = N_aug(i)->corner_a_max();
  }
  return max;
}

float dbasnh_hypg_aug::max_of_corner_a_min ()
{
  float max = 0;
  for (unsigned int i=0; i<nodes_.size(); i++) {
    if (N_aug(i)->corner_a_min() > max)
      max = N_aug(i)->corner_a_min();
  }
  return max;
}

void dbasnh_hypg_aug::normalize_node_grad_r_max (const float max, const int verbose)
{
  if (max == 0)
    return; //avoid divide by zero (can't happen).

  if (verbose)
    vul_printf (vcl_cout, "  %s: normalize_node_grad_r_max() max = %lf.\n", 
                b_1st_graph_ ? "G" : "g", max);

  for (unsigned int i=0; i<nodes_.size(); i++) {
    N_aug(i)->set_grad_r_max (N_aug(i)->grad_r_max() / max);
  }
}

void dbasnh_hypg_aug::normalize_node_grad_r_min (const float max, const int verbose)
{
  if (max == 0)
    return; //avoid divide by zero (can't happen).

  if (verbose)
    vul_printf (vcl_cout, "  %s: normalize_node_grad_r_min() max = %lf.\n", 
                b_1st_graph_ ? "G" : "g", max);

  for (unsigned int i=0; i<nodes_.size(); i++) {
    N_aug(i)->set_grad_r_min (N_aug(i)->grad_r_min() / max);
  }
}

void dbasnh_hypg_aug::normalize_node_corner_a_max (const float max, const int verbose)
{
  if (max == 0)
    return; //avoid divide by zero (can't happen).

  if (verbose)
    vul_printf (vcl_cout, "  %s: normalize_node_corner_a_max() max = %lf.\n", 
                b_1st_graph_ ? "G" : "g", max);

  for (unsigned int i=0; i<nodes_.size(); i++) {
    N_aug(i)->set_corner_a_max (N_aug(i)->corner_a_max() / max);
  }
}

void dbasnh_hypg_aug::normalize_node_corner_a_min (const float max, const int verbose)
{
  if (max == 0)
    return; //avoid divide by zero (can't happen).

  if (verbose)
    vul_printf (vcl_cout, "  %s: normalize_node_corner_a_min() max = %lf.\n", 
                b_1st_graph_ ? "G" : "g", max);

  for (unsigned int i=0; i<nodes_.size(); i++) {
    N_aug(i)->set_corner_a_min (N_aug(i)->corner_a_min() / max);
  }
}

void dbasnh_hypg_aug::normalize_corner_nf_cost (const float max, const int verbose)
{
  if (max == 0) {
    //There is no valid corner found (due to using a subset of ms_curves.
    return; //avoid divide by zero.
  }

  //Here we normalize the cost by first taking a log() and then normalized by max of log value!!
  const double log_max = vcl_log (max);

  if (verbose)
    vul_printf (vcl_cout, "  %s: normalize_corner_nf_cost() max = %f, log_max = %lf.\n", 
                b_1st_graph_ ? "G" : "g", max, log_max);

  const unsigned int nN = nodes_.size();
  for (unsigned int i=0; i<nN; i++) {
    for (unsigned int j=0; j<nN; j++) {
      for (unsigned int k=0; k<nN; k++) {
        int nf = corner_nf_[i][j][k];        
        if (nf >= 1) { //Only proceed where there exists a ms_sheet.
          const double log_nf = vcl_log (double(nf));
          corner_nf_[i][j][k] = log_nf / log_max;
          assert (vnl_math_isnan(corner_nf_[i][j][k]) == false);
        }
      }
    }
  }

  //debug print!
}

void dbasnh_hypg_aug::normalize_corner_grad_r_cost (const float max, const int verbose)
{
  if (max == 0)
    return; //avoid divide by zero (can't happen).

  if (verbose)
    vul_printf (vcl_cout, "  %s: normalize_corner_grad_r_cost() max = %lf.\n", 
                b_1st_graph_ ? "G" : "g", max);

  const unsigned int nN = nodes_.size();
  for (unsigned int i=0; i<nN; i++)
    for (unsigned int j=0; j<nN; j++) {
      for (unsigned int k=0; k<nN; k++) {
        if (corner_grad_r_[i][j][k] !=0)
          corner_grad_r_[i][j][k] /= max;
      }
    }
}

double dbasnh_hypg_aug::bound_box_radius_of_nodes ()
{
  //loop through each node and compute the min_max box.
  vgl_box_3d<double> bbox;

  for (unsigned int i=0; i<nodes_.size(); i++) {
    bbox.add (N_aug(i)->pt());
  }
  
  //return the max bounding box width, height, or depth.
  double w = bbox.max_x() - bbox.min_x();
  double h = bbox.max_y() - bbox.min_y();
  double d = bbox.max_z() - bbox.min_z();

  return vcl_max (vcl_max (w, h), d);
}

void dbasnh_hypg_aug::get_cen_N_aug (vgl_point_3d<double>& C)
{
  double x=0, y=0, z=0;
  for (unsigned int i=0; i<nodes_.size(); i++) {
    x += N_aug(i)->pt().x();
    y += N_aug(i)->pt().y();
    z += N_aug(i)->pt().z();
  }

  x /= nodes_.size();
  y /= nodes_.size();
  z /= nodes_.size();
  C.set (x, y, z);
}

