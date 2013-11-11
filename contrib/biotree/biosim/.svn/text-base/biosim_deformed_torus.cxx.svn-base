#include "biosim_deformed_torus.h"
#include <vcl_limits.h>
#include <vnl/vnl_math.h>
#include <vcl_string.h>
#include <vnl/vnl_random.h>

biosim_deformed_torus::biosim_deformed_torus(int xdim, int ydim, int zdim,
    double a, double b, double c,
    double mean,double gaussian_sigma)
:volume_(xdim, ydim, zdim), img_view_(xdim, ydim, zdim)
{
  volume_.fill(0.0);
  img_view_.fill(0);

  double x,y,z;
  double quant1,quant2;
  vnl_random v_rand;

  for (int i = 0;i<xdim;i++)
      {
      for (int j = 0;j<ydim;j++)
          {
          for (int k = 0;k<zdim;k++)
              {
              x = i - xdim/2.0;
              y = j - ydim/2.0;
              z = k - zdim/2.0;
             
              quant1 = vcl_sqrt(z*z + vcl_pow((vcl_sqrt(x*x + y*y) - c),2.0));
              quant2 = a + (b * x)/vcl_sqrt(x*x + y*y);

              if (quant1 <= quant2 + 0.5)
                  {
                    double &vx = volume_(i,j,k);
                    vx = mean +  gaussian_sigma*v_rand.normal();
                    unsigned char &c = img_view_(i,j,k);
                    c = static_cast<unsigned char> (vx);
                  } 
              }
          }
      }

}


         
 

   
