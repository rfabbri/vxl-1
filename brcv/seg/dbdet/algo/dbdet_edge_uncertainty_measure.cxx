// This is brcv/dbdet/edge/dbdet_edge_uncertainty_measure.cxx

//:
// \file

#include "dbdet_edge_uncertainty_measure.h"
#include <vcl_cstdio.h>
#include <vcl_cstdlib.h>   // for vcl_abs(int) and vcl_sqrt()
#include <vnl/vnl_math.h>
#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_matrix_inverse.h>
#include <vnl/algo/vnl_svd.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/algo/vgl_homg_operators_2d.h>

#include <dbdet/edge/dbdet_gaussian_kernel.h>
#include <dbdet/algo/dbdet_subpix_convolution.h>

//---------------------------------------------------------------
// Constructors
//----------------------------------------------------------------

//: Constructor from a parameter block, gradient magnitudes given as an image and directions given as component image
dbdet_edge_uncertainty_measure::
dbdet_edge_uncertainty_measure(const vil_image_view<double>& grad_x, const vil_image_view<double>& grad_y, double sigma) : 
  thresh_(0.0),
  Ix_(grad_x), Iy_(grad_y), 
  H_(vil_image_view<double>(grad_x.ni(), grad_x.nj(), 1))
{
  //fill it with zeros first
  H_.fill(0.0);

  //// compute the H measure at every point inside the margins
  //double mean_fac = ((2*n+1)*(2*n+1)*(2*n+1)*(2*n+1)); //for computing the mean of the measures
  //
  //for (unsigned x = n; x < H_.ni()-n; x++){
  //  for (unsigned y = n; y < H_.nj()-n; y++)
  //  {
  //    double Ix2 = 0.0;
  //    double Iy2 = 0.0;
  //    double IxIy = 0.0;

  //    //compute the matrix W from the average around the window size
  //    for (int wx = -n; wx <= n; wx++){
  //      for (int wy = -n; wy <= n; wy++)
  //      {
  //        double Ix = Ix_(x+wx,y+wy); 
  //        double Iy = Iy_(x+wx,y+wy);

  //        Ix2 += (Ix*Ix);
  //        Iy2 += (Iy*Iy);
  //        IxIy += (Ix*Iy);
  //      }
  //    }

  //    //the H measure = det(W)
  //    double h = vnl_math_max(0.0, (Ix2*Iy2 - IxIy*IxIy)/((Ix2+Iy2)*(Ix2+Iy2))); //remove negative values
  //    H_(x,y) = norm_fac*h;

  //    //H_(x,y) = norm_fac*vcl_pow(h, 0.25); //normalize
  //  }
  //}

  //second method for computing the H measure
  vil_image_view<double> Ix2, Iy2, IxIy, Ix22, Iy22, IxIy2;

  Ix2.set_size(grad_x.ni(), grad_x.nj());
  Iy2.set_size(grad_x.ni(), grad_x.nj());
  IxIy.set_size(grad_x.ni(), grad_x.nj());
  Ix22.set_size(grad_x.ni(), grad_x.nj());
  Iy22.set_size(grad_x.ni(), grad_x.nj());
  IxIy2.set_size(grad_x.ni(), grad_x.nj());

  const double *gx  =  grad_x.top_left_ptr();
  const double *gy  =  grad_y.top_left_ptr();
  double *ix2  =  Ix2.top_left_ptr();
  double *iy2  =  Iy2.top_left_ptr();
  double *ixiy  =  IxIy.top_left_ptr();
  double *ix22  =  Ix22.top_left_ptr();
  double *iy22  =  Iy22.top_left_ptr();
  double *ixiy2  =  IxIy2.top_left_ptr();
  double *h  =  H_.top_left_ptr();

  for(unsigned long i=0; i<grad_x.size(); i++){
    ix2[i] = gx[i]*gx[i];
    iy2[i] = gy[i]*gy[i];
    ixiy[i] = gx[i]*gy[i];
  }

  //now smooth them with a gaussian
  dbdet_subpix_convolve_2d(Ix2, Ix22,   dbdet_G_kernel(sigma),   double(), 0);
  dbdet_subpix_convolve_2d(Iy2, Iy22,   dbdet_G_kernel(sigma),   double(), 0);
  dbdet_subpix_convolve_2d(IxIy, IxIy2,   dbdet_G_kernel(sigma),   double(), 0);

  //then compute the h measure from it
  double norm_fac = sqrt(sigma)/25.0;
  for(unsigned long i=0; i<grad_x.size(); i++)
    h[i] = norm_fac*vcl_pow(vnl_math_max(0.0, (ix22[i]*iy22[i] - ixiy2[i]*ixiy2[i])), 0.25);// /((ix22[i]+iy22[i])*(ix22[i]+iy22[i])));

}

  
//: get the uncertainty measures at the edgel locations
void dbdet_edge_uncertainty_measure::
get_edgel_uncertainties(vcl_vector<vgl_point_2d<double> >& edge_locs, vcl_vector<double>& edge_uncertainties)
{
  //interpolate the uncertainty measure at the edgel locations and return them in the vector of uncertainties
  for (unsigned i=0; i<edge_locs.size(); i++)
  {
    ////Note: for now just round off and read the value from the grid points without interpolating
    //unsigned xx = (unsigned) vcl_floor(edge_locs[i].x()+0.5);
    //unsigned yy = (unsigned) vcl_floor(edge_locs[i].y()+0.5);
    //edge_uncertainties[i] = H_(xx,yy);
    
    //Inteprolate the values
    unsigned x0 = (unsigned) vcl_floor(edge_locs[i].x());
    unsigned y0 = (unsigned) vcl_floor(edge_locs[i].y());

    unsigned x1 = (unsigned) vcl_ceil(edge_locs[i].x());
    unsigned y1 = (unsigned) vcl_ceil(edge_locs[i].y());

    double x = edge_locs[i].x()-x0;
    double y = edge_locs[i].y()-y0;

    edge_uncertainties[i] = H_(x0, y0)*(1-x)*(1-y) + H_(x1, y0)*x*(1-y) + H_(x0, y1)*(1-x)*y + H_(x1, y1)*x*y; 
  }

}


