#include "vol3d_radius_filter.h"
#include <vcl_cmath.h>
#include <vcl_valarray.h>
#include <vcl_algorithm.h>
#include <vcl_cassert.h>
#include <vcl_fstream.h>


vol3d_radius_filter::vol3d_radius_filter(int r)
  : shells_(2*r + 1, 2*r + 1, 2*r + 1), 
  lookup_((int)vcl_ceil(vcl_sqrt(3.0)*r)+1)
{
  assert( r > 0);

  dim_ = 2*r + 1;

  for(int i = 0; i < lookup_.size(); i++)
    lookup_[i] = new voxels_t;
  
  for(int i = 0 ; i < dim_; i++)
    for(int j = 0; j < dim_; j++)
      for(int k = 0; k < dim_; k++)
      {
        double d = vcl_sqrt(static_cast<double>((i-r)*(i-r) + (j-r)*(j-r) + (k-r)*(k-r)));
        
        shells_[i][j][k] = d;
        
        int shell_index = static_cast<int>( vcl_floor(d));

        // change to a coordinate origin at center of the filter.
        quadret coord(i - r, j - r, k - r,d);
        
        lookup_[shell_index]->push_back(coord );
      }
 
}

vol3d_radius_filter::~vol3d_radius_filter()
{
  for(int i = 0; i < lookup_.size(); i++)
  {
    delete lookup_[i];
    lookup_[i] = 0;
  }
}

double vol3d_radius_filter::density(vbl_array_3d<double> const & vol,
    int i, int j, int k, double radius)
{
unsigned int dimx = vol.get_row1_count();
unsigned int dimy = vol.get_row2_count();
unsigned int dimz = vol.get_row3_count();



 // assert(radius < (dim_ -1) / 2 && radius > 0);
assert(radius< (dim_ -1)/2);

  double density = 0;
  int num_pts = 0;

  int upper_bound = static_cast<int>(vcl_ceil(radius));

  for(int r = 0; r <= upper_bound; r++)
  {

    for(int l = 0; l < lookup_[r]->size(); l++)  
    {
    if ((*lookup_[r])[l].d_ < radius)
        {
      int x = (*lookup_[r])[l].x_;
      int y = (*lookup_[r])[l].y_;
      int z = (*lookup_[r])[l].z_;

int coord_x = i+x;
int coord_y = j+y;
int coord_z = k+z;

      if (coord_x >= 0 && coord_x < dimx && coord_y >=0 && coord_y < dimy && coord_z >=0 && coord_z < dimz)
          {
      density += vol[coord_x][coord_y][coord_z];
      num_pts++;

    }
        }
    }
  }

  density /= num_pts;
  return density;

}

vcl_valarray<double> vol3d_radius_filter::densities(
    vbl_array_3d<double> const & vol,
    int i, int j, int k)
{
 unsigned int dimx = vol.get_row1_count();
unsigned int dimy = vol.get_row2_count();
unsigned int dimz = vol.get_row3_count();

  vcl_valarray<int> tmp(7);
  
  tmp[0] = i;
  
  tmp[1] = j;

  tmp[2] = k;

  tmp[3] = vol.get_row1_count() - i;

  tmp[4] = vol.get_row2_count() - j;

  tmp[5] = vol.get_row3_count() - k;

  tmp[6] = (dim_-1)/2;
  
  
  int min_r = tmp.min();
  
  vcl_valarray<double> dens(min_r);
  for(int r = 0; r < min_r; r++)
  {
    int num_pts = 0;
    
    double density = 0;
    for(int l = 0; l < lookup_[r]->size(); l++)  
    {
      int x = (*lookup_[r])[l].x_;
      int y = (*lookup_[r])[l].y_;
      int z = (*lookup_[r])[l].z_;

      int coord_x = i+x;
      int coord_y = j+y;
      int coord_z = k+z;

      if (coord_x >=0 && coord_x < dimx && coord_y >=0 && coord_y < dimy && coord_z >=0 && coord_z < dimz)
          {
      density += vol[coord_x][coord_y][coord_z];
      num_pts++;
    }
      
    }

    density /= num_pts;

    dens[r] = density;
  }

  return dens;
}

vcl_valarray<double> 
vol3d_radius_filter::densities(vbl_array_3d<double> const & vol,
    vcl_valarray<double> const &rprobes, 
    int i, int j, int k)
{
unsigned int dimx = vol.get_row1_count();
unsigned int dimy = vol.get_row2_count();
unsigned int dimz = vol.get_row3_count();

  // for each probe shell
  vcl_valarray<double> dens(0.0, rprobes.size());
  
  assert(rprobes.size() > 0);
  for(int l = 0; l < rprobes.size(); l++)
    assert(rprobes[l] <= (dim_-1)/2);
  
  
  // for the first shell
  dens[0] = this->density(vol, i, j, k, rprobes[0]);

  // all the other shell
  
  for(int rp = 1; rp < rprobes.size(); rp ++)
  {
    double density = 0;
    int num_pts = 0;

   double lower_val = rprobes[rp-1];
   double upper_val = rprobes[rp];
   
   int lower_bound = static_cast<int> (vcl_floor(lower_val));
   int upper_bound = static_cast<int> (vcl_ceil(upper_val));

    for(int r = lower_bound; r <= upper_bound; r++)
    {
     for(int l = 0; l < lookup_[r]->size(); l++)  
      {
      double dist = (*lookup_[r])[l].d_ ;
      if (dist > lower_bound && dist <= upper_bound)
          {
        int x = (*lookup_[r])[l].x_;
        int y = (*lookup_[r])[l].y_;
        int z = (*lookup_[r])[l].z_;

        int coord_x = i+x;
        int coord_y = j+y;
        int coord_z = k+z;


         if (coord_x >=0 && coord_x < dimx && coord_y >=0 && coord_y < dimy && coord_z >=0 && coord_z < dimz)
          {
        density += vol[coord_x][coord_y][coord_z];
        num_pts++;
          }
          }
      }

    }
      density /= num_pts;

      dens[rp] = density;
 
  }
    return dens;
}


double vol3d_radius_filter::density(vil3d_image_view<vxl_uint_16>const & vol_view,
    int i, int j, int k, int radius)
{
unsigned int dimx = vol_view.ni();
unsigned int dimy = vol_view.nj();
unsigned int dimz = vol_view.nk();

 // assert(radius < (dim_ -1) / 2 && radius > 0);
assert(radius < (dim_ -1) / 2);

  double density = 0;
  int num_pts = 0;

  for(int r = 0; r <= radius; r++)
  {

    for(int l = 0; l < lookup_[r]->size(); l++)  
    {
      int x = (*lookup_[r])[l].x_;
      int y = (*lookup_[r])[l].y_;
      int z = (*lookup_[r])[l].z_;

// single plane vil3d_image_view
       if (i+x >=0 && i+x < dimx && j+y >=0 && j+y < dimy && k+z >=0 && k+z < dimz)
          {
        density += vol_view(i+x,j+y,k+z,0);
        num_pts++;
          }
    }


  }

  density /= num_pts;
  return density;

}

vcl_valarray<double> 
vol3d_radius_filter::densities(vil3d_image_view<vxl_uint_16>const & vol_view,
    vcl_valarray<double> const &rprobes, 
    int i, int j, int k)
{
unsigned int dimx = vol_view.ni();
unsigned int dimy = vol_view.nj();
unsigned int dimz = vol_view.nk();

  // for each probe shell
  vcl_valarray<double> dens(0.0, rprobes.size());
  
  assert(rprobes.size() > 0);
  for(int l = 0; l < rprobes.size(); l++)
    assert(rprobes[l] <= (dim_-1)/2);


  // for the first shell
  dens[0] = this->density(vol_view, i, j, k, rprobes[0]);

  // all the other shell

  vcl_string point_intensity = "C:\\scale_selection\\intensities.txt";
  vcl_ofstream fstream(point_intensity.c_str());

  for(int rp = 1; rp < rprobes.size(); rp ++)
  {
    double density = 0;
    int num_pts = 0;
    for(int r = rprobes[rp-1]+1; r <= rprobes[rp]; r++)
    {
      for(int l = 0; l < lookup_[r]->size(); l++)  
      {
        int x = (*lookup_[r])[l].x_;
        int y = (*lookup_[r])[l].y_;
        int z = (*lookup_[r])[l].z_;

// a single plane vil_3d_image_view
if (i+x >=0 && i+x < dimx && j+y >=0 && j+y < dimy && k+z >=0 && k+z < dimz)
          {
        density += vol_view(i+x,j+y,k+z,0);
        num_pts++;
         if (i == 8 && j == 19 && k == 0 && rp == 2)
             fstream <<"(" << i+x << "," <<j+y<<"," << k+z<<")"<< vol_view(i+x,j+y,k+z)<< " ";
          }

      }

    }
fstream.close();

      density /= num_pts;

      dens[rp] = density;
 
  }
    return dens;
}

