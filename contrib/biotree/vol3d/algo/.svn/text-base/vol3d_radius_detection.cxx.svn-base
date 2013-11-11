#include "vol3d_radius_detection.h"
#include <vcl_cmath.h>
#include <vcl_fstream.h>
#include <vcl_iostream.h>
#include <assert.h>
#include <vcl_string.h>
#include <vcl_fstream.h>
#include <vil3d/vil3d_new.h>
#include <vil3d/vil3d_save.h>
#include <vul/vul_timer.h>

double 
vol3d_radius_detection::mylog2(double x)
{
  return vcl_log(x)/vcl_log(2.0);
}

vol3d_radius_detection::vol3d_radius_detection(int min_r, int max_r)
  : rp_(static_cast<int>(vcl_floor(2*mylog2((double)max_r/min_r)+0.5))+1)
{

  assert(min_r <= max_r);

  int num_probes = rp_.size(); 
 
  assert(num_probes > 0);

  double sqrt2 = vcl_sqrt(2.0); 

  for(int i = 0; i <num_probes; i++)
      {
   // rp_[i] = static_cast<int>(vcl_floor((vcl_pow(sqrt2, i)*min_r)+0.5)); 
      rp_[i] = ((vcl_pow(sqrt2, i)*min_r)+0.5); 
  vcl_cout << "rp " << rp_[i] << vcl_endl;
      }


  // setup a radius filter with maxium radius
  rf_ = new vol3d_radius_filter(static_cast<int>(vcl_ceil(rp_[num_probes-1]))); 
    
}

vol3d_radius_detection::~vol3d_radius_detection()
{
  if(rf_)
    delete rf_;
  rf_ = 0;
}

vbl_array_3d<double> vol3d_radius_detection::
radius(vil3d_image_resource_sptr  vol_sptr, double density_mean, double density_sigma)
{

  int dim1 = vol_sptr->ni();

  int dim2 = vol_sptr->nj();

  int dim3 = vol_sptr->nk();
  
  vbl_array_3d<double> res(dim1, dim2, dim3);

  vil3d_image_view<vxl_uint_16> vol_view = vol_sptr->get_copy_view(0,vol_sptr->ni(),0,vol_sptr->nj(),0,vol_sptr->nk());

  for(int i = 0; i<dim1; i++)
    for(int j = 0; j < dim2; j++)
      for(int k = 0; k < dim3; k++)
      {
        vcl_valarray<double> dens = rf_->densities(vol_view, rp_, i, j, k);

        assert(dens.size() > 1 );

        // predetermine the size as maxium size unless changed afterwards
        res[i][j][k] = rp_[rp_.size()-1];
        
        // change the size
        if(dens[0] < density_mean - 3* density_sigma) // air
          res[i][j][k] = 0;
        else
        {
          for(unsigned int r = 1; r < dens.size(); r++)
            if(dens[r] < density_mean - 3* density_sigma)
            {
              res[i][j][k] = rp_[r];
              break;
            }
            
        } 
      }
        
#if 0
  vcl_string txt_file = "C:\\scale_selection\\radius_labeling.txt"; 
          vcl_ofstream fstream(txt_file.c_str());

          for(int k = 0; k < dim3; k++)
      {
    for(int j = 0; j < dim2; j++)
        {
      for(int i = 0; i<dim1; i++)
          {
          
fstream << res[i][j][k] << "  " ;
          }
      fstream << vcl_endl;
        }
fstream << "end of row " << k << vcl_endl;
    }
fstream.close();
#endif

  return res;
}

vbl_array_3d<double> vol3d_radius_detection::
radius(vil3d_image_resource_sptr vol_sptr, double density_sigma)
{

  double threshold = 3. * vcl_sqrt(2.0) * density_sigma;
  
  int dim1 = vol_sptr->ni();

  int dim2 = vol_sptr->nj();

  int dim3 = vol_sptr->nk();

  vil3d_image_view<vxl_uint_16> vol_view = vol_sptr->get_copy_view(0,vol_sptr->ni(),0,vol_sptr->nj(),0,vol_sptr->nk());

                                                   
  vbl_array_3d<double> res(dim1, dim2, dim3,-1.0);
  
 

  for(int i = 0; i<dim1; i++)
    for(int j = 0; j < dim2; j++)
      for(int k = 0; k < dim3; k++)
      {
        vcl_valarray<double> dens = rf_->densities(vol_view, rp_, i, j, k);

        assert(dens.size() > 1 );
       
        vcl_valarray<double> diff(dens.size()-1);
 
        for (unsigned int r = 0;r < diff.size();r++)
            {
diff[r] = dens[r+1] - dens[r];
if (diff[r] > 3*density_sigma)
    {
res[i][j][k] = 0;
break;
    }
   }

        for (unsigned int r = 0;r < diff.size();r++)
            {

            diff[r] = dens[r+1] - dens[r];
if ((diff[r] < -3*density_sigma) & (res[i][j][k] != 0 ) )
    {
    res[i][j][k] = rp_[r+1];
    break;

    }
            }
      }

#if 0
   vcl_string txt_file = "C:\\scale_selection\\radius_labeling_with_known_sigma.txt"; 
          vcl_ofstream fstream(txt_file.c_str());

          for(int k = 0; k < dim3; k++)
      {
    for(int j = 0; j < dim2; j++)
        {
      for(int i = 0; i<dim1; i++)
          {
          
fstream << res[i][j][k] << "  " ;
          }
      fstream << vcl_endl;
        }
fstream << "end of row " << k << vcl_endl;
    }
fstream.close();
#endif

  return res;
}
  



vbl_array_3d<double> vol3d_radius_detection::
radius(vil3d_image_resource_sptr vol_sptr, double density_sigma ,float threshold)
{
  
  int dim1 = vol_sptr->ni();

  int dim2 = vol_sptr->nj();

  int dim3 = vol_sptr->nk();
  
  // initializing the array with -1 instead of 0 so that radius 
  // labelling errors can be detected

  vbl_array_3d<double> res(dim1, dim2, dim3, -1);


  vil3d_image_view<vxl_uint_16> vol_view = vol_sptr->get_copy_view(0,vol_sptr->ni(),0,vol_sptr->nj(),0,vol_sptr->nk());



  for(int i = 0; i<dim1; i++)
    for(int j = 0; j < dim2; j++)
      for(int k = 0; k < dim3; k++)
      {
        vcl_valarray<double> dens = rf_->densities(vol_view, rp_, i, j, k);

        assert(dens.size() > 1 );

        vcl_valarray<double> diff(dens.size()-1);
 
        for (unsigned int r = 0;r < diff.size();r++)
            {
diff[r] = dens[r+1] - dens[r];
if (diff[r] > 3*density_sigma)
    {
res[i][j][k] = 0;
break;
    }
   }

        for (unsigned int r = 0;r < diff.size();r++)
            {

            diff[r] = dens[r+1] - dens[r];
if ((diff[r] < -3*density_sigma) & (res[i][j][k] != 0 ) )
    {
    res[i][j][k] = rp_[r+1];
    break;

    }
            }

        double sum = 0;

for (unsigned int r = 0;r < dens.size();r++)
    {
sum += dens[r] ;

    }
sum = sum/dens.size();

if (res[i][j][k] == -1)
    {
    if (sum > threshold )
        res[i][j][k] = 1e5; // basically the size can't be determined with the current set of shells
    else
        res[i][j][k] = 0;
    }
      }

#if 0
  vcl_string txt_file = "C:\\scale_selection\\radius_labeling.txt"; 
          vcl_ofstream fstream(txt_file.c_str());

  for(int i = 0; i<dim1; i++)
      {
    for(int j = 0; j < dim2; j++)
        {
      for(int k = 0; k < dim3; k++)
          {
          
fstream << res[i][j][k] << "  " ;
          }
      fstream << vcl_endl;
        }
fstream << "end of row " << i << vcl_endl;
    }
fstream.close();
#endif

  return res;
}



vbl_array_3d<double> vol3d_radius_detection::
radius(vbl_array_3d<double> vol, double density_mean, double density_sigma)
{

  int dim1 = vol.get_row1_count();

  int dim2 = vol.get_row2_count();

  int dim3 = vol.get_row3_count();
  
  // initializing the array with -1 instead of 0 so that radius 
  // labelling errors can be detected

  vbl_array_3d<double> res(dim1, dim2, dim3,-1);

   vul_timer t;

  for(int i = 0; i<dim1; i++)
    for(int j = 0; j < dim2; j++)
      for(int k = 0; k < dim3; k++)
      {
   //   vul_timer t1;

        vcl_valarray<double> dens = rf_->densities(vol, rp_, i, j, k);

    //    vcl_cout <<" time for finding densities at a voxel " << "(" << i <<","<<j<<","<<k<<")" <<t1.real()<< vcl_endl;

        assert(dens.size() > 1 );

        // predetermine the size as maxium size unless changed afterwards
        res[i][j][k] = rp_[rp_.size()-1];
        
        // change the size
        if(dens[0] < density_mean - 3* density_sigma) // air
          res[i][j][k] = 0;
        else
        {
          for(unsigned int r = 1; r < dens.size(); r++)
            if(dens[r] < density_mean - 3* density_sigma)
            {
              res[i][j][k] = rp_[r];
              break;
            }
            
        } 
      }
   double time = t.real();
   vcl_cout <<" Time for the radius detection " << time / 1000.0 << "s.\n";
        
  return res;
}

vbl_array_3d<double> vol3d_radius_detection::
radius(vbl_array_3d<double> vol, double density_sigma)
{

  
  
  int dim1 = vol.get_row1_count();

  int dim2 = vol.get_row2_count();

  int dim3 = vol.get_row3_count();

  


//vcl_string density_txt_file = "C:\\scale_selection\\densities_prior_to_labeling.txt";
//  vcl_ofstream fstr(density_txt_file.c_str());

                                                  
  
  // initializing the array with 0s

  vbl_array_3d<double> res(dim1, dim2, dim3, 0.0);

  for(int i = 0; i<dim1; i++)
    for(int j = 0; j < dim2; j++)
      for(int k = 0; k < dim3; k++)
      {
        vcl_valarray<double> dens = rf_->densities(vol, rp_, i, j, k);

      //   fstr << "(" << i+510 << "," << j+309<<")" << vcl_endl; 
  for (int d = 0;d<dens.size();d++)
            {
     //   fstr << (unsigned short)dens[d] <<" ";
            }
     //   fstr << vcl_endl;

        assert(dens.size() > 1 );

        vcl_valarray<double> diff(dens.size()-1);
 
     
           for (unsigned int r = 0;r < diff.size();r++)
            {
diff[r] = (dens[r] - dens[0])/(1+1/rp_[r]/rp_[r]);


if (diff[r] > 3*density_sigma)
    {
res[i][j][k] = 0;
break;
    }

    else

        {

        if (diff[r] < -3*density_sigma)
            {
            res[i][j][k] = rp_[r];
        break;
            }

        }
            }
      
      }


   vcl_string txt_file = "C:\\scale_selection\\radius_labeling_with_known_sigma.txt"; 
          vcl_ofstream fstream(txt_file.c_str());

          for(int k = 0; k < dim3; k++)
      {
    for(int j = 0; j < dim2; j++)
        {
      for(int i = 0; i<dim1; i++)
          {
          
fstream << res[i][j][k] << "  " ;
          }
      fstream << vcl_endl;
        }
fstream << "end of row " << k << vcl_endl;
    }
fstream.close();

//fstr.close();

  return res;
}
  



vbl_array_3d<double> vol3d_radius_detection::
radius(vbl_array_3d<double> vol, double density_sigma ,float background_threshold)
{
  
int dim1 = vol.get_row1_count();

int dim2 = vol.get_row2_count();

int dim3 = vol.get_row3_count();
  
  // initializing the array with -1 instead of 0 so that radius 
  // labelling errors can be detected

 vbl_array_3d<double> res(dim1, dim2, dim3, 0.0);

 
 


  for(int i = 0; i<dim1; i++)
    for(int j = 0; j < dim2; j++)
      for(int k = 0; k < dim3; k++)
      {
        vcl_valarray<double> dens = rf_->densities(vol, rp_, i, j, k);

        assert(dens.size() > 1 );

        vcl_valarray<double> diff(dens.size()-1);
 
     
if (((dens[0] - dens[dens.size()-1])/dens[0]) < background_threshold)
    {
    res[i][j][k] = 0;
    }
else
    {
           for (unsigned int r = 0;r < diff.size();r++)
            {
diff[r] = (dens[r] - dens[0])/(1+1/rp_[r]/rp_[r]);


 if (diff[r] > 3*density_sigma)
    {
res[i][j][k] = 0;
break;
    }

    else
        {
        if (diff[r] < -3*density_sigma)
            {
            res[i][j][k] = rp_[r];
        break;
            }
        }
   }
    }
      }

  vcl_string txt_file = "C:\\scale_selection\\radius_labeling_with_0.7_thresh.txt"; 
          vcl_ofstream fstream(txt_file.c_str());

          for(int k = 0; k < dim3; k++)
      {
    for(int j = 0; j < dim2; j++)
        {
      for(int i = 0; i<dim1; i++)
          {
          
fstream << res[i][j][k] << "  " ;
          }
      fstream << vcl_endl;
        }
fstream << "end of row " << k << vcl_endl;
    }
fstream.close();


  return res;
}
