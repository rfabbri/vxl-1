#include "biosim_cylinder_pyramid.h"
#include <limits>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_random.h>

biosim_cylinder_pyramid::biosim_cylinder_pyramid(double density,double gaussian_sigma):volume_(80,80,80)
{
// right now, the pyramid volume size is being hardcoded as 80 x 80 x 80

  vbl_array_3d<double> vol(80, 80, 80,double(0));



 

  // asign a few axis aliged cylinders with different radius
  for(int k = 0; k < 30; k++)
    for(int i = 0; i < 80; i++)
      for(int j = 0; j < 80; j++)
        if((i-40)*(i-40)+(j-40)*(j-40) <= 4)
          vol[i][j][k] = density;

  for(int k = 30; k < 50; k++)
    for(int i = 0; i < 80; i++)
      for(int j = 0; j < 80; j++)
        if((i-40)*(i-40)+(j-40)*(j-40) <= 16)
          vol[i][j][k] = density;
   
  for(int k = 50; k < 80; k++)
    for(int i = 0; i < 80; i++)
      for(int j = 0; j < 80; j++)
        if((i-40)*(i-40)+(j-40)*(j-40) <= 36)
          vol[i][j][k] = density;

 
  std::cout << "volume size " << volume_.size();

  vnl_random v_rand;

 
for (int k = 0;k < 80;k++)
    {
 for (int i = 0;i<80;i++)
     {
   for (int j = 0;j<80;j++)
       {
       volume_[i][j][k] = vol[i][j][k] + gaussian_sigma*v_rand.normal();
    //   std::cout << "points " << volume_[i][j][k] <<std::endl;
       }
     }
    }


}

void biosim_cylinder_pyramid::radii_range(double &min_radius,double &max_radius)
{
min_radius = std::numeric_limits<float >::max();
max_radius = std::numeric_limits<float >::min();

for (std::vector<double>::iterator it = radii_.begin();it != radii_.end();it++)
    {
    if (*it < min_radius)
        min_radius = *it;
    if (*it>max_radius)
        max_radius = *it;

    }

}

         
 

   
