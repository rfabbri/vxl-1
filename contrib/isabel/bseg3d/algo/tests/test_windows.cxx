#include <testlib/testlib_test.h>
//#include "../bseg3d_gauss_multiplier.h"
#include <bvxm/grid/bvxm_voxel_grid.h>
#include <bvxm/grid/bvxm_voxel_slab.h>
#include "../bseg3d_explore_mixtures.h"
#include "../bseg3d_window_detector.h"
#include "../bseg3d_gmm_l2norm.h"
#include <vcl_limits.h>
#include <bsta/bsta_distribution.h>
#include <bsta/bsta_gauss_f1.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gaussian_indep.h>
#include <vul/vul_timer.h>
//#include <vnl/vnl_random.h>

typedef bsta_num_obs<bsta_gauss_f1> gauss_type;
typedef bsta_mixture_fixed<gauss_type, 3> mix_gauss_fixed;
typedef bsta_mixture<gauss_type> mix_gauss;
typedef bsta_num_obs<mix_gauss_fixed> mix_gauss_type;


void test1()
{
  //1. Define a grid with the size of the capitol grid
  vgl_vector_3d<unsigned> grid_size(475,475,185);
  bvxm_voxel_grid<float> *capitol_grid = new bvxm_voxel_grid<float>("C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/test_windows/test.vox", grid_size);
  //capitol_grid->initialize_data(0.0f);

  //2. Define the window grid
  vgl_vector_3d<unsigned> window_size(11,11,16);
  bvxm_voxel_grid<float> *window = new bvxm_voxel_grid<float>("C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/test_windows/window.vox", window_size);
  window->initialize_data(0.0f);

  
  bvxm_voxel_grid<float>::iterator slab_it = window->begin();
  bvxm_voxel_grid<float>::iterator cap_slab_it = capitol_grid->begin();
  for(unsigned z = 0; z<window_size.z(); z++, ++slab_it, ++cap_slab_it)
  {
    
    for(unsigned x = 0 ; x<window_size.x(); x++)
      for(unsigned y= 0; y<window_size.y(); y++)
      {
        if(x==y){
          if((z>2 && z <13) && (x>2 &&x <8)  && (y>3 && y<7)){
            (*slab_it)(x,y) = 0;
            (*cap_slab_it)(x,y) = 0;
          }
          else{
            (*slab_it)(x,y) = 1;
            (*cap_slab_it)(x,y) =20;
          }
        }
      }
  }

  bseg3d_explore_mixtures explorer;
  vcl_string filename("C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/test_windows/window.raw");
 // bvxm_voxel_grid_base_sptr win_sptr = new bvxm_voxel_grid<float>(window);
  explorer.save_float_grid_raw(window,filename);
  
  vcl_string filename2("C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/test_windows/cap.raw");
  //bvxm_voxel_grid_base_sptr capitol_sptr = new bvxm_voxel_grid<float>(capitol_grid);
  explorer.save_float_grid_raw(capitol_grid,filename2);

}

void test2()
{
   //1. Define a grid with the size of the capitol grid
  vgl_vector_3d<unsigned> grid_size(475,475,185);
  bvxm_voxel_grid<float> *convolved_grid = new bvxm_voxel_grid<float>("C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/test_windows/convolved_fake1.vox", grid_size);
  convolved_grid->initialize_data(0.0f);
  bvxm_voxel_grid<float> *convolved_grid2 = new bvxm_voxel_grid<float>("C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/test_windows/convolved_fake2.vox", grid_size);
  convolved_grid->initialize_data(0.0f);
  //2. Read volume of interest
   bvxm_voxel_grid<float> *capitol_grid = new bvxm_voxel_grid<float>("C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/test_windows/test_world.vox", grid_size);


  //3. Define the window grid
  vgl_vector_3d<unsigned> window_size(11,11,16);
  bvxm_voxel_slab<float> window(11,11,16);
  window.fill(0.0f);

  //fill in window grid
  for(unsigned z = 0; z<window_size.z(); z++)
  {
    for(unsigned x = 0 ; x<window_size.x(); x++)
      for(unsigned y= 0; y<window_size.y(); y++)
      {
        if(x==y){
          if((z>2 && z <13) && (x>2 &&x <8)  && (y>3 && y<7)){
            window(x,y,z) = 0;
          }
          else{
            window(x,y,z) = 1;
          }
        }
      }
  }


  //2. Define the window grid
  //bvxm_voxel_grid<float> *window_grid = new bvxm_voxel_grid<float>("C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/test_windows/window.vox", window_size);

 
  // bvxm_voxel_grid<float>::iterator slab_it = window_grid->slab_iterator(0,window_size.z());
  // for(; slab_it != window_grid->end(); ++slab_it){
  //   for(unsigned z = 0 ; z<window_size.z(); z++)
  //     for(unsigned x = 0 ; x<window_size.x(); x++)
  //       for(unsigned y= 0; y<window_size.y(); y++)
  //       {
  //         (*slab_it)(x,y,z) = window(x,y,z);
  //       }
  // }

  //vcl_string filename("C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/test_windows/win.raw");
  ////bvxm_voxel_grid_base_sptr capitol_sptr = new bvxm_voxel_grid<float>(capitol_grid);
  //explorer.save_float_grid_raw(window_grid,filename);

  //3. Convolve
   unsigned slab_idx = 0; 
   bvxm_voxel_grid<float>::iterator cap_slab_it = capitol_grid->slab_iterator(slab_idx,window_size.z());
   bvxm_voxel_grid<float>::iterator conv_slab_it = convolved_grid->slab_iterator(slab_idx,window_size.z());
   bvxm_voxel_grid<float>::iterator conv_slab_it2 = convolved_grid2->slab_iterator(slab_idx,window_size.z());

   unsigned count = 0;
   //for (unsigned z = 0; z<(grid_size.z() - window_size.z()); z++)
   for(; slab_idx + window_size.z() < grid_size.z(); ++cap_slab_it, ++conv_slab_it,++conv_slab_it2, ++slab_idx)
   {
     count++;
     for (unsigned x = 0; x<(grid_size.x() - window_size.x()); x++)
       for (unsigned y = 0; y<(grid_size.y() - window_size.y()); y++)
       {
         float mask_val = 0;
         float mask_val2 = 1;
         for(unsigned x_w = 0; x_w< window_size.x(); x_w++)
         {
           //y coordinate should be the same as x coordinate for now, since my 
           //window plane lies in the main diagonal
              for(unsigned z_w = 0; z_w< window_size.z(); z_w++)
              {
                //vcl_cout << (*cap_slab_it)(x+x_w, y+x_w,z_w) << vcl_endl;
                if(window(x_w, x_w, z_w)== 0){
                  mask_val = mask_val +vcl_log(( 1-(*cap_slab_it)(x+x_w, y+x_w,z_w)));
                  mask_val2 = mask_val2 *( 1-(*cap_slab_it)(x+x_w, y+x_w,z_w));
                }
                if(window(x_w, x_w, z_w)== 1){
                  mask_val = mask_val + vcl_log(((*cap_slab_it)(x+x_w, y+x_w,z_w)));
                  mask_val2 = mask_val2 * ((*cap_slab_it)(x+x_w, y+x_w,z_w));
                }
              }
         }
          if((float(mask_val) ==vcl_numeric_limits<float>::infinity()) ||
           (float(mask_val) == -1 *vcl_numeric_limits<float>::infinity()))
           mask_val = -1000;
           (*conv_slab_it)(x+5,y+5,8) =float(mask_val);  //(*conv_slab_it)(x+5,y+5,8) + window(x_w, y_w, z_w)*(*conv_slab_it)(x+x_w, y+y_w,z_w);
           (*conv_slab_it2)(x+5,y+5,8) =float(mask_val2); 
       }
   }

    
   vcl_cout <<count<<"\n";

  bseg3d_explore_mixtures explorer;
 // vcl_string filename("C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/test_windows/window.raw");
 //// bvxm_voxel_grid_base_sptr win_sptr = new bvxm_voxel_grid<float>(window);
 // explorer.save_float_grid_raw(window,filename);
 
  vcl_string filename2("C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/test_windows/fake_conv.raw");
  //bvxm_voxel_grid_base_sptr capitol_sptr = new bvxm_voxel_grid<float>(capitol_grid);
  explorer.save_float_grid_raw(convolved_grid,filename2);

}

void create_sample_window(bvxm_voxel_slab<float> &window)
{
    //fill in window grid
  //fill in window grid
   vgl_vector_3d<unsigned> window_size(11,11,16);
  for(unsigned z = 0; z<window_size.z(); z++)
  {
    for(unsigned x = 0 ; x<window_size.x(); x++)
      for(unsigned y= 0; y<window_size.y(); y++)
      {
        if(x==y){
          if((z>2 && z <13) && (x>2 &&x <8)  && (y>3 && y<7)){
            window(x,y,z) = 0;
          }
          else{
            window(x,y,z) = 1;
          }
        }
      }
  }
}
void log_ocp()
{
   //1. Define a grid with the size of the capitol grid
  vgl_vector_3d<unsigned> grid_size(475,475,185);
  bvxm_voxel_grid<float> *convolved_grid = new bvxm_voxel_grid<float>("C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/test_windows/conv_ocp.vox", grid_size);
  convolved_grid->initialize_data(0.0f);

  //2. Read volume of interest
   bvxm_voxel_grid<float> *capitol_grid = new bvxm_voxel_grid<float>("C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/test_windows/capitol.vox", grid_size);


  //3. Define the window grid
  vgl_vector_3d<unsigned> window_size(11,11,16);
  bvxm_voxel_slab<float> window(11,11,16);
  window.fill(0.0f);
  create_sample_window(window);



  //4. Convolve
   unsigned slab_idx = 110; 
   bvxm_voxel_grid<float>::iterator cap_slab_it = capitol_grid->slab_iterator(slab_idx,window_size.z());
   bvxm_voxel_grid<float>::iterator conv_slab_it = convolved_grid->slab_iterator(slab_idx,window_size.z());
   unsigned count = 0;
   //for (unsigned z = 0; z<(grid_size.z() - window_size.z()); z++)
   //for(; slab_idx + window_size.z() < grid_size.z(); ++cap_slab_it, ++conv_slab_it, ++slab_idx)
   for(; slab_idx + window_size.z() < 161; ++cap_slab_it, ++conv_slab_it, ++slab_idx)
   {
     count++;
     for (unsigned x = 0; x<(grid_size.x() - window_size.x()); x++)
       for (unsigned y = 0; y<(grid_size.y() - window_size.y()); y++)
       {
         double mask_val = 0;

         for(unsigned x_w = 0; x_w< window_size.x(); x_w++)
         {
           //y coordinate should be the same as x coordinate for now, since my 
           //window plane lies in the main diagonal
              for(unsigned z_w = 0; z_w< window_size.z(); z_w++)
              {

                if(window(x_w, x_w, z_w)== 0)
                  mask_val = mask_val + vcl_log(( 1-(*cap_slab_it)(x+x_w, y+x_w,z_w)));
                if(window(x_w, x_w, z_w)== 1)
                  mask_val = mask_val + vcl_log(((*cap_slab_it)(x+x_w, y+x_w,z_w)));
              }
         }
         if((float(mask_val) ==vcl_numeric_limits<float>::infinity()) ||
           (float(mask_val) == -1 *vcl_numeric_limits<float>::infinity()))
           mask_val = -1000;
        /* if (float(mask_val) - vcl_log(1.0 - float(mask_val)) > vcl_log(0.5))
           (*conv_slab_it)(x+5,y+5,8) =1; */ 
         (*conv_slab_it)(x+5,y+5,8)= float(mask_val);
      
       }

   }
   vcl_cout <<count<<"\n";

  bseg3d_explore_mixtures explorer;
 // vcl_string filename("C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/test_windows/window.raw");
 //// bvxm_voxel_grid_base_sptr win_sptr = new bvxm_voxel_grid<float>(window);
 // explorer.save_float_grid_raw(window,filename);
 
  vcl_string filename2("C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/test_windows/cap_conv_short.raw");
  //bvxm_voxel_grid_base_sptr capitol_sptr = new bvxm_voxel_grid<float>(capitol_grid);
  explorer.save_float_grid_raw(convolved_grid,filename2);

}
void ocp()
{
  vcl_cout<<"Finding windows...ocp \n";
   //1. Define a grid with the size of the capitol grid
  vgl_vector_3d<unsigned> grid_size(475,475,185);
  bvxm_voxel_grid_base_sptr convolved_base = new bvxm_voxel_grid<float>("C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/test_windows/new/ocp_avg.vox", grid_size);
 
  bvxm_voxel_grid<float> *convolved_grid =
    dynamic_cast<bvxm_voxel_grid<float>*>(convolved_base.ptr());
  convolved_grid->initialize_data(0.0f);

  //2. Read volume of interest
   bvxm_voxel_grid<float> *capitol_grid = new bvxm_voxel_grid<float>("C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/test_windows/capitol.vox", grid_size);


  //3. Define the window grid
  vgl_vector_3d<unsigned> window_size(11,11,16);
  bvxm_voxel_slab<float> window(11,11,16);
  window.fill(0.0f);
  create_sample_window(window);



  //4. Convolve
   unsigned slab_idx = 0; 
   vnl_random rand;
   bvxm_voxel_grid<float>::iterator cap_slab_it = capitol_grid->slab_iterator(slab_idx,window_size.z());
   bvxm_voxel_grid<float>::iterator conv_slab_it = convolved_grid->slab_iterator(slab_idx,window_size.z());

   vul_timer timer;
   timer.mark();
  for(; slab_idx  < grid_size.z() - window_size.z(); ++cap_slab_it, ++conv_slab_it, ++slab_idx)
   {
     vcl_cout<<".";
    for (unsigned x = 0; x<(grid_size.x() - window_size.x()); x++)
   //  for (unsigned x = 177; x<(327 - window_size.x()); x++)
     {
       for (unsigned y = 0; y<(grid_size.y() - window_size.y()); y++)
       //for (unsigned y = 135; y<(278 - window_size.y()); y++)

       {
     /*    double p_full = 0.0;
         double p_empty = 0.0;
         double p_rand = 0.0;
         double p_window = 0.0;*/
         double avg = 0.0;

         for(unsigned x_w = 0; x_w< window_size.x(); x_w++)
         {
          
           //y coordinate should be the same as x coordinate for now, since my 
           //window plane lies in the main diagonal
              for(unsigned z_w = 0; z_w< window_size.z(); z_w++)
              {
                 double this_ocp = (*cap_slab_it)(x+x_w, y+x_w,z_w);

                //if(window(x_w, x_w, z_w)== 0)
                //  p_window = p_window + vcl_log(( 1.0-(*cap_slab_it)(x+x_w, y+x_w,z_w)));
                 if(window(x_w, x_w, z_w)== 1)
                   avg = avg + (*cap_slab_it)(x+x_w, y+x_w,z_w);
                  //p_window = p_window + vcl_log(((*cap_slab_it)(x+x_w, y+x_w,z_w)));
                 
               // p_empty =  p_empty + vcl_log(1.0-(*cap_slab_it)(x+x_w, y+x_w,z_w));
                //p_full = p_full + vcl_log((*cap_slab_it)(x+x_w, y+x_w,z_w));
                //p_rand = p_rand + vcl_log(vcl_abs(double(rand.lrand32(0, 1))-(*cap_slab_it)(x+x_w, y+x_w,z_w)));
              }
         }
       
     
         //double r1 = vcl_exp(p_full-p_window);
         //double r2 = vcl_exp(p_empty-p_window);
         //double r3 = vcl_exp(p_rand-p_window);
         //double den = r1+r3 +1.0;
         //double den = vcl_abs(p_window + 600.0);
         (*conv_slab_it)(x+5,y+5,8)= avg/146.0;
       }
     }
  
  }

  vcl_cout<<vcl_endl;
  vcl_cout<< "Running time in ms" <<timer.real()/6000.0 <<vcl_endl;
 

  bseg3d_explore_mixtures explorer; 
  //vcl_string filename_byte("C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/test_windows/new/conv_ocp2_byte.raw");
  vcl_string filename_float("C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/test_windows/new/conv_ocp_avg_float.raw");

 // explorer.save_byte_grid_raw(convolved_grid,filename_byte);
  explorer.save_float_grid_raw(convolved_grid,filename_float);


}



//finds windows based on appereance model and KL-divergence(average)
void apm_kl()
{
   //1. Define a grid with the size of the capitol grid
  vgl_vector_3d<unsigned> grid_size(475,475,185);
  bvxm_voxel_grid_base_sptr convolved_base = new bvxm_voxel_grid<float>("C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/test_windows/new/conv_amp_kl.vox", grid_size);
 
  bvxm_voxel_grid<float> *convolved_grid =
    dynamic_cast<bvxm_voxel_grid<float>*>(convolved_base.ptr());
  convolved_grid->initialize_data(0.0f);  convolved_grid->initialize_data(0.0f);

  //2. Read volume of interest
   bvxm_voxel_grid<gauss_type> *capitol_grid = new bvxm_voxel_grid<gauss_type>("C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/test_windows/apm_gauss.vox", grid_size);


  //3. Define the window grid
  vgl_vector_3d<unsigned> window_size(11,11,16);
  bvxm_voxel_slab<float> window(11,11,16);
  window.fill(0.0f);
  create_sample_window(window);

  //4. Convolve
   unsigned slab_idx = 0; 
   unsigned empty_dim_idx = 0;
   unsigned frame_dim_idx = 0;
   unsigned empty_dim= 30;
   unsigned frame_dim= 146;
   bseg3d_gmm_l2norm distance;
  

   bvxm_voxel_grid<gauss_type>::iterator cap_slab_it = capitol_grid->slab_iterator(slab_idx,window_size.z());
   bvxm_voxel_grid<float>::iterator conv_slab_it = convolved_grid->slab_iterator(slab_idx,window_size.z());
   unsigned count = 0;
   for(; slab_idx + window_size.z() < grid_size.z(); ++cap_slab_it, ++conv_slab_it, ++slab_idx)
  // for(; slab_idx + window_size.z() < 161; ++cap_slab_it, ++conv_slab_it, ++slab_idx)
   {
     count++;
     for (unsigned x = 0; x<(grid_size.x() - window_size.x()); x++)
       for (unsigned y = 0; y<(grid_size.y() - window_size.y()); y++)
       {
         mix_gauss frame_mixture;
         mix_gauss empty_mixture;
         float mean_empty = 0.0;
         float mean_frame = 0.0;
         float var_empty = 0.0;
         float var_frame = 0.0;
         frame_dim_idx = 0;
         empty_dim_idx = 0;

         for(unsigned x_w = 0; x_w< window_size.x(); x_w++)
         {
           //y coordinate should be the same as x coordinate for now, since my 
           //window plane lies in the main diagonal
           for(unsigned z_w = 0; z_w< window_size.z(); z_w++)
           {

             gauss_type this_gauss = (*cap_slab_it)(x + x_w, y + x_w, z_w);
             if(window(x_w, x_w, z_w)== 0)
             {
               empty_mixture.insert(this_gauss, 1.0f/float(empty_dim));
               mean_empty = mean_empty +this_gauss.mean();
               var_empty = var_empty + this_gauss.var() + vcl_pow(this_gauss.mean(),2);  
               empty_dim_idx++;
             }
             else if(window(x_w, x_w, z_w)== 1)
             {
               frame_mixture.insert(this_gauss, 1.0f/float(frame_dim));
               mean_frame = mean_frame +this_gauss.mean();
               var_frame = var_frame + this_gauss.var() + vcl_pow(this_gauss.mean(),2);  
               
               frame_dim_idx++;
             }
           }
         }

         if((frame_dim_idx!= frame_dim)||(empty_dim_idx!= empty_dim))
           vcl_cerr<<"dimension error \n";

         mean_empty = mean_empty/float(empty_dim_idx);
         var_empty = var_empty/float(empty_dim_idx) - vcl_pow(mean_empty,2);
         bsta_gauss_f1 mean_gauss_empty(mean_empty, var_empty);

         mean_frame = mean_frame/float(frame_dim_idx);
         var_frame = var_frame/float(frame_dim_idx) - vcl_pow(mean_frame,2);
         bsta_gauss_f1 mean_gauss_frame(mean_frame,var_frame);

         //distance between regions
          float dist_btw = distance.kl_symmetric_distance(mean_gauss_empty,mean_gauss_frame);
         
     
         //average distances whithin regions
          float avg_frame_dist = 0.0;
          for(unsigned i =0; i<frame_mixture.num_components(); ++i)
          {
            avg_frame_dist = avg_frame_dist + distance.kl_symmetric_distance(mean_gauss_frame, frame_mixture.distribution(i));

          }

         avg_frame_dist = avg_frame_dist/float(frame_dim);

          float avg_empty_dist = 0.0;
          for(unsigned i =0; i<empty_mixture.num_components(); ++i)
          {
            avg_empty_dist = avg_empty_dist + distance.kl_symmetric_distance(mean_gauss_empty, empty_mixture.distribution(i));

          }

         avg_empty_dist = avg_empty_dist/float(empty_dim);

         if((avg_empty_dist<0)||(avg_empty_dist<0)||(dist_btw<0))
           vcl_cout<< "dist smaller than 0 at " << slab_idx <<vcl_endl;

         (*conv_slab_it)(x+5,y+5,8)=(1.0f/(1.0f + avg_empty_dist))*(1.0f/(1.0f + avg_frame_dist)) * (1.0f - 1.0f/(1.0f + dist_btw));


       }
   }


  
   bseg3d_explore_mixtures explorer;
   vcl_string filename("C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/test_windows/apm_conv2.raw");
   explorer.save_float_grid_raw(convolved_grid,filename);


}


void apm_l2_kl()
{
  vcl_cout<<"Detecting windows... amp l2-kl \n";
   //1. Define a grid with the size of the capitol grid
  vgl_vector_3d<unsigned> grid_size(475,475,185);
  bvxm_voxel_grid_base_sptr convolved_base = new bvxm_voxel_grid<float>("C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/test_windows/new/conv_apm_kl_l2.vox", grid_size);
 
  bvxm_voxel_grid<float> *convolved_grid =
    dynamic_cast<bvxm_voxel_grid<float>*>(convolved_base.ptr());
  convolved_grid->initialize_data(0.0f);  
  //2. Read volume of interest
   bvxm_voxel_grid<gauss_type> *capitol_grid = new bvxm_voxel_grid<gauss_type>("C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/test_windows/apm_gauss.vox", grid_size);


  //3. Define the window grid
  vgl_vector_3d<unsigned> window_size(11,11,16);
  bvxm_voxel_slab<float> window(11,11,16);
  window.fill(0.0f);
  create_sample_window(window);

  //4. Convolve
   unsigned slab_idx = 0; 
   unsigned empty_dim_idx = 0;
   unsigned frame_dim_idx = 0;
   unsigned empty_dim= 30;
   unsigned frame_dim= 146;
   bseg3d_gmm_l2norm distance;
  
   vul_timer timer;
   timer.mark();

   bvxm_voxel_grid<gauss_type>::iterator cap_slab_it = capitol_grid->slab_iterator(slab_idx,window_size.z());
   bvxm_voxel_grid<float>::iterator conv_slab_it = convolved_grid->slab_iterator(slab_idx,window_size.z());
   unsigned count = 0;
   for(; slab_idx + window_size.z() < grid_size.z(); ++cap_slab_it, ++conv_slab_it, ++slab_idx)
  // for(; slab_idx + window_size.z() < 161; ++cap_slab_it, ++conv_slab_it, ++slab_idx)
   {
     vcl_cout<<".";
     count++;
     for (unsigned x = 0; x<(grid_size.x() - window_size.x()); x++)
       for (unsigned y = 0; y<(grid_size.y() - window_size.y()); y++)
       {
         mix_gauss frame_mixture;
         mix_gauss empty_mixture;
         float mean_empty = 0.0;
         float mean_frame = 0.0;
         float var_empty = 0.0;
         float var_frame = 0.0;
         frame_dim_idx = 0;
         empty_dim_idx = 0;

         for(unsigned x_w = 0; x_w< window_size.x(); x_w++)
         {
           //y coordinate should be the same as x coordinate for now, since my 
           //window plane lies in the main diagonal
           for(unsigned z_w = 0; z_w< window_size.z(); z_w++)
           {

             gauss_type this_gauss = (*cap_slab_it)(x + x_w, y + x_w, z_w);
             if(window(x_w, x_w, z_w)== 0)
             {
               empty_mixture.insert(this_gauss, 1.0f/float(empty_dim));
               mean_empty = mean_empty +this_gauss.mean();
               var_empty = var_empty + this_gauss.var() + vcl_pow(this_gauss.mean(),2);  
               empty_dim_idx++;
             }
             else if(window(x_w, x_w, z_w)== 1)
             {
               frame_mixture.insert(this_gauss, 1.0f/float(frame_dim));
               mean_frame = mean_frame +this_gauss.mean();
               var_frame = var_frame + this_gauss.var() + vcl_pow(this_gauss.mean(),2);  
               
               frame_dim_idx++;
             }
           }
         }

         if((frame_dim_idx!= frame_dim)||(empty_dim_idx!= empty_dim))
           vcl_cerr<<"dimension error \n";

         mean_empty = mean_empty/float(empty_dim_idx);
         var_empty = var_empty/float(empty_dim_idx) - vcl_pow(mean_empty,2);
         bsta_gauss_f1 mean_gauss_empty(mean_empty, var_empty);

         mean_frame = mean_frame/float(frame_dim_idx);
         var_frame = var_frame/float(frame_dim_idx) - vcl_pow(mean_frame,2);
         bsta_gauss_f1 mean_gauss_frame(mean_frame,var_frame);

         //distance between regions
          float dist_btw = distance.kl_symmetric_distance(mean_gauss_empty,mean_gauss_frame);
         
     
         //average distances whithin regions
          float within_frame_dist = distance.l2_gauss2mix(mean_gauss_frame,frame_mixture,1);

          float within_empty_dist = distance.l2_gauss2mix(mean_gauss_empty,empty_mixture,1);


         if((within_frame_dist<0)||(within_empty_dist<0)||(dist_btw<0))
           vcl_cout<< "dist smaller than 0 at " << slab_idx <<vcl_endl;

         (*conv_slab_it)(x+5,y+5,8)=(1.0f-within_frame_dist)*(1.0f- within_empty_dist) * (1.0f - 1.0f/(1.0f + dist_btw));


       }
   }


   vcl_cout<<vcl_endl;
   vcl_cout<< "Running time in ms" <<timer.real() <<vcl_endl;
   bseg3d_explore_mixtures explorer;
   vcl_string filename("C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/test_windows/new/apm_conv_l2_kl_byte.raw");
   vcl_string filename_float("C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/test_windows/new/apm_conv_l2_kl_float.raw");
  
   explorer.save_byte_grid_raw(convolved_grid,filename);
   explorer.save_float_grid_raw(convolved_grid,filename_float);

}

void apm_l2()
{
  vcl_cout<<"Detecting windows... amp l2\n";
   //1. Define a grid with the size of the capitol grid
  vgl_vector_3d<unsigned> grid_size(475,475,185);
  bvxm_voxel_grid_base_sptr convolved_base = new bvxm_voxel_grid<float>("C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/test_windows/new/conv_apml2.vox", grid_size);
 
  bvxm_voxel_grid<float> *convolved_grid =
    dynamic_cast<bvxm_voxel_grid<float>*>(convolved_base.ptr());
  convolved_grid->initialize_data(0.0f);

  //2. Read volume of interest
   bvxm_voxel_grid<gauss_type> *capitol_grid = new bvxm_voxel_grid<gauss_type>("C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/test_windows/apm_gauss.vox", grid_size);


  //3. Define the window grid
  vgl_vector_3d<unsigned> window_size(11,11,16);
  bvxm_voxel_slab<float> window(11,11,16);
  window.fill(0.0f);
  create_sample_window(window);

  //4. Convolve
   unsigned slab_idx = 0; 
   unsigned empty_dim_idx = 0;
   unsigned frame_dim_idx = 0;
   unsigned empty_dim= 30;
   unsigned frame_dim= 146;
   bseg3d_gmm_l2norm distance;
  

   bvxm_voxel_grid<gauss_type>::iterator cap_slab_it = capitol_grid->slab_iterator(slab_idx,window_size.z());
   bvxm_voxel_grid<float>::iterator conv_slab_it = convolved_grid->slab_iterator(slab_idx,window_size.z());
   unsigned count = 0;
   vul_timer timer;
   vcl_cout<<"Start timing \n";
   timer.mark();
   for(; slab_idx + window_size.z() < grid_size.z(); ++cap_slab_it, ++conv_slab_it, ++slab_idx)
  // for(; slab_idx + window_size.z() < 161; ++cap_slab_it, ++conv_slab_it, ++slab_idx)
   {
     vcl_cout<<".";
     count++;
     for (unsigned x = 0; x<(grid_size.x() - window_size.x()); x++)
       for (unsigned y = 0; y<(grid_size.y() - window_size.y()); y++)
       {
         mix_gauss frame_mixture;
         mix_gauss empty_mixture;
         float mean_empty = 0.0;
         float mean_frame = 0.0;
         float var_empty = 0.0;
         float var_frame = 0.0;
         frame_dim_idx = 0;
         empty_dim_idx = 0;

         for(unsigned x_w = 0; x_w< window_size.x(); x_w++)
         {
           //y coordinate should be the same as x coordinate for now, since my 
           //window plane lies in the main diagonal
           for(unsigned z_w = 0; z_w< window_size.z(); z_w++)
           {

             gauss_type this_gauss = (*cap_slab_it)(x + x_w, y + x_w, z_w);
             if(window(x_w, x_w, z_w)== 0)
             {
               empty_mixture.insert(this_gauss, 1.0f/float(empty_dim));
               mean_empty = mean_empty +this_gauss.mean();
               var_empty = var_empty + this_gauss.var() + vcl_pow(this_gauss.mean(),2);  
               empty_dim_idx++;
             }
             else if(window(x_w, x_w, z_w)== 1)
             {
               frame_mixture.insert(this_gauss, 1.0f/float(frame_dim));
               mean_frame = mean_frame +this_gauss.mean();
               var_frame = var_frame + this_gauss.var() + vcl_pow(this_gauss.mean(),2);  
               
               frame_dim_idx++;
             }
           }
         }

         if((frame_dim_idx!= frame_dim)||(empty_dim_idx!= empty_dim))
           vcl_cerr<<"dimension error \n";

         mean_empty = mean_empty/float(empty_dim_idx);
         var_empty = var_empty/float(empty_dim_idx) - vcl_pow(mean_empty,2);
         bsta_gauss_f1 mean_gauss_empty(mean_empty, var_empty);

         mean_frame = mean_frame/float(frame_dim_idx);
         var_frame = var_frame/float(frame_dim_idx) - vcl_pow(mean_frame,2);
         bsta_gauss_f1 mean_gauss_frame(mean_frame,var_frame);

          //distance between regions
          float dist_btw = distance.l2_gauss(mean_gauss_empty,mean_gauss_frame,1);
            
         //average distances whithin regions
          float within_frame_dist = distance.l2_gauss2mix(mean_gauss_frame,frame_mixture,1);

          float within_empty_dist = distance.l2_gauss2mix(mean_gauss_empty,empty_mixture,1);


         if((within_frame_dist<0)||(within_empty_dist<0)||(dist_btw<0))
           vcl_cout<< "dist smaller than 0 at " << slab_idx <<vcl_endl;

         (*conv_slab_it)(x+5,y+5,8)=(1.0f-within_frame_dist)*(1.0f- within_empty_dist) * dist_btw;


       }
   }

   vcl_cout<< vcl_endl;
   vcl_cout<< "Running time in ms" <<timer.real() <<vcl_endl;
   bseg3d_explore_mixtures explorer;
   vcl_string filename("C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/test_windows/new/apm_conv_l2_byte.raw");
   vcl_string filename_float("C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/test_windows/new/apm_conv_l2_float.raw");
  
   explorer.save_byte_grid_raw(convolved_grid,filename);
   explorer.save_float_grid_raw(convolved_grid,filename_float);

}

void apm_weighted_l2()
{
  vcl_cout<<"Detecting windows... amp l2 with simple weights\n";
   //1. Define a grid with the size of the capitol grid
  vgl_vector_3d<unsigned> grid_size(475,475,185);
  bvxm_voxel_grid_base_sptr convolved_base = new bvxm_voxel_grid<float>("C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/test_windows/new/conv_apm_wl2.vox", grid_size);
 
  bvxm_voxel_grid<float> *convolved_grid =
    dynamic_cast<bvxm_voxel_grid<float>*>(convolved_base.ptr());
  convolved_grid->initialize_data(0.0f);

  //2. Read volume of interest
   bvxm_voxel_grid<gauss_type> *capitol_grid = new bvxm_voxel_grid<gauss_type>("C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/test_windows/apm_gauss.vox", grid_size);


  //3. Define the window grid
  vgl_vector_3d<unsigned> window_size(11,11,16);
  bvxm_voxel_slab<float> window(11,11,16);
  window.fill(0.0f);
  create_sample_window(window);

  //4. Convolve
   unsigned slab_idx = 0; 
   unsigned empty_dim_idx = 0;
   unsigned frame_dim_idx = 0;
   unsigned empty_dim= 30;
   unsigned frame_dim= 146;
   bseg3d_gmm_l2norm distance;
  

   bvxm_voxel_grid<gauss_type>::iterator cap_slab_it = capitol_grid->slab_iterator(slab_idx,window_size.z());
   bvxm_voxel_grid<float>::iterator conv_slab_it = convolved_grid->slab_iterator(slab_idx,window_size.z());
   unsigned count = 0;
   vul_timer timer;
   vcl_cout<<"Start timing \n";
   timer.mark();
   for(; slab_idx + window_size.z() < grid_size.z(); ++cap_slab_it, ++conv_slab_it, ++slab_idx)
  // for(; slab_idx + window_size.z() < 161; ++cap_slab_it, ++conv_slab_it, ++slab_idx)
   {
     vcl_cout<<".";
     count++;
     for (unsigned x = 0; x<(grid_size.x() - window_size.x()); x++)
       for (unsigned y = 0; y<(grid_size.y() - window_size.y()); y++)
       {
   
         float mean_empty = 0.0;
         float mean_frame = 0.0;
         float var_empty = 0.0;
         float var_frame = 0.0;
         frame_dim_idx = 0;
         empty_dim_idx = 0;

         for(unsigned x_w = 0; x_w< window_size.x(); x_w++)
         {
           //y coordinate should be the same as x coordinate for now, since my 
           //window plane lies in the main diagonal
           for(unsigned z_w = 0; z_w< window_size.z(); z_w++)
           {

             gauss_type this_gauss = (*cap_slab_it)(x + x_w, y + x_w, z_w);
             if(window(x_w, x_w, z_w)== 0)
             {
               mean_empty = mean_empty +this_gauss.mean();
               var_empty = var_empty + this_gauss.var() + vcl_pow(this_gauss.mean(),2);  
               empty_dim_idx++;
             }
             else if(window(x_w, x_w, z_w)== 1)
             {
               mean_frame = mean_frame +this_gauss.mean();
               var_frame = var_frame + this_gauss.var() + vcl_pow(this_gauss.mean(),2);  
               frame_dim_idx++;
             }
           }
         }

         if((frame_dim_idx!= frame_dim)||(empty_dim_idx!= empty_dim))
           vcl_cerr<<"dimension error \n";

         mean_empty = mean_empty/float(empty_dim_idx);
         var_empty = var_empty/float(empty_dim_idx) - vcl_pow(mean_empty,2);
         bsta_gauss_f1 mean_gauss_empty(mean_empty, var_empty);

         mean_frame = mean_frame/float(frame_dim_idx);
         var_frame = var_frame/float(frame_dim_idx) - vcl_pow(mean_frame,2);
         bsta_gauss_f1 mean_gauss_frame(mean_frame,var_frame);

         //distance between regions
         float dist_btw = distance.kl_symmetric_distance(mean_gauss_empty,mean_gauss_frame);
            
         if((dist_btw<0))
           vcl_cout<< "dist smaller than 0 at " << slab_idx <<vcl_endl;

         (*conv_slab_it)(x+5,y+5,8)=(1.0f/(1.0f +var_frame))*(1.0f/(1.0f + var_empty)) * (1.0f - 1.0f/(1.0f + dist_btw));


       }
   }

   vcl_cout<<vcl_endl;
   vcl_cout<< "Running time in ms" <<timer.real() <<vcl_endl;
   bseg3d_explore_mixtures explorer;
   vcl_string filename("C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/test_windows/new/apm_conv_simple_byte.raw");
   vcl_string filename_float("C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/test_windows/new/apm_conv_simple_float.raw");
  
   explorer.save_byte_grid_raw(convolved_grid,filename);
   explorer.save_float_grid_raw(convolved_grid,filename_float);

}
//void create_fake_world()
//{
//  vcl_cout<<"Creating fake world \n";
//   //1. Define a grid with the size of the capitol grid
//  vgl_vector_3d<unsigned> grid_size(475,475,185);
//
//  //2. Read volume of interest
//   bvxm_voxel_grid<float> *capitol_grid = new bvxm_voxel_grid<float>("C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/test_windows/test_world.vox", grid_size);
//  capitol_grid->initialize_data(0.0f);
//
//  //3. Define the window grid
//  vgl_vector_3d<unsigned> window_size(11,11,16);
//  bvxm_voxel_slab<float> window(11,11,16);
//  window.fill(0.1f);
//
//  //fill in window grid
//  for(unsigned z = 0; z<window_size.z(); z++)
//  {
//    for(unsigned x = 0 ; x<window_size.x(); x++)
//      for(unsigned y= 0; y<window_size.y(); y++)
//      {
//        if(x==y){
//          if((z>2 && z <13) && (y>2 &&y <8)  && (x>3 && x<7)){
//            window(x,y,z) = 0.0f;
//          }
//          else{
//            window(x,y,z) = 1.0f;
//          }
//        }
//      }
//  }
//
//  //fill the test world with some ideal windows
//   unsigned slab_idx = 124; 
//   bvxm_voxel_grid<float>::iterator cap_slab_it = capitol_grid->slab_iterator(slab_idx,window_size.z());
//   unsigned count = 0;
//   //for (unsigned z = 0; z<(grid_size.z() - window_size.z()); z++)
//   bool next = true;
//   //for(; slab_idx + window_size.z() < capitol_grid->grid_size().z(); cap_slab_it+window_size.z(), slab_idx=slab_idx +window_size.z())
//   {
//     count++;
//     next= true;
//     
//     for (unsigned x = 172, y = 127; (x<(grid_size.x() - window_size.x())); x+=window_size.x(),y+=window_size.y())
//     {     
//      //for (unsigned y = 127; (y<(grid_size.y() - window_size.y())); y+=window_size.y())
//       {
//         //if((x == y ) ) //&&(x== slab_idx))
//         {
//           for(unsigned x_w = 0; x_w< window_size.x(); x_w++)
//           {
//             for(unsigned y_w = 0; y_w< window_size.y(); y_w++)
//             {
//               for(unsigned z_w = 0; z_w< window_size.z(); z_w++)
//               {
//
//                 (*cap_slab_it)(x + x_w, y + y_w, z_w) = window(x_w,y_w,z_w);
//               }
// 
//             }
//           }
//           //next = false;
//         }    
//       }
//     }
//
//   }
//   //increase slab count so that it gets written to disk
//   ++cap_slab_it;
//
//  bseg3d_explore_mixtures explorer;
//  vcl_string filename2("C:/Experiments/object_recognition/bseg3d/CapitolSiteHigh/test_windows/fake_world2.raw");
//  explorer.save_float_grid_raw(capitol_grid,filename2);
//}

void test()
{
 ocp();
// apm_weighted_l2();
// apm_l2();
// apm_l2_kl();
}

MAIN( test_windows)
{
  START ("testing windows");
  test();
  SUMMARY();
}

 // //////////////////////////////temp/////////////////////////////////////

 ////fill the test world with some ideal windows
 //  unsigned slab_idx1 = 90; 
 //  bvxm_voxel_grid<float>::iterator cap_slab_it1 = convolved_grid->slab_iterator(slab_idx1,window_size.z());
 //  //for (unsigned z = 0; z<(grid_size.z() - window_size.z()); z++)
 //  bool next = true;
 //  for(; slab_idx1 + window_size.z() < 107; cap_slab_it1+window_size.z(), slab_idx1=slab_idx1 +window_size.z())
 //  {
 //    next= true;
 //    for (unsigned x = 0; (x<(grid_size.x() - window_size.x())) &&(next == true); x++)
 //    {     
 //      for (unsigned y = 0; (y<(grid_size.y() - window_size.y()))&&(next == true); y++)
 //      {
 //        if((x == y ) &&(x== slab_idx1))
 //        {
 //          for(unsigned x_w = 0; x_w< window_size.x(); x_w++)
 //          {
 //            for(unsigned y_w = 0; y_w< window_size.y(); y_w++)
 //            {
 //              for(unsigned z_w = 0; z_w< window_size.z(); z_w++)
 //              {

 //                (*cap_slab_it1)(x + x_w, y + y_w, z_w) = window(x_w,y_w,z_w);
 //              }
 //
 //            }
 //          }
 //          next = false;
 //        }    
 //      }
 //    }

 //  }

 /////////////////////////////////////////////////////////
