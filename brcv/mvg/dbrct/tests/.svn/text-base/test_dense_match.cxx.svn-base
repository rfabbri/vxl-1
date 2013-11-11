#include <dbrct/dbrct_wide_dense_match.h>
#include <vcl_fstream.h>
#include <vcl_string.h>
#include <vil1/vil1_load.h>
#include <vil1/vil1_save.h>
#include <vnl/io/vnl_io_matrix.h>
#include <vsl/vsl_binary_io.h>
#include <vnl/io/vnl_io_matrix.h>
#include <vnl/vnl_vector.h>
#include <vcl_sstream.h>
#include <testlib/testlib_test.h>

int test_dense_match_main(int argc, char *argv[])
{
   testlib_test_begin("dense match");
   
   vcl_string image_base;
    if(argc>=2)
    {
    image_base=argv[1];
    }
   #ifdef VCL_WIN32
    image_base += "\\";
   #else
    image_base += "/";
   #endif
   
   vcl_string  Limg=image_base+"img1.png";
   vcl_string  Rimg=image_base+"img2.png";

   vil1_image img1=vil1_load(Limg.c_str());
   vil1_image img2=vil1_load(Rimg.c_str());

   //: initialize the dense matcher
   dbrct_wide_dense_match wdm(img1,img2,2,4);
  wdm.downsample_images();

   
   //: testing for the downsampling

   for(int i=wdm.params.levels-1;i>=0;i--)
   {
     

       wdm.params.paramslist[i].bias_cost=0.0;
       wdm.params.paramslist[i].common_intensity_diff=1.0;
       wdm.params.paramslist[i].continuity_cost=0.1;
       wdm.params.paramslist[i].correlation_range=20;
       wdm.params.paramslist[i].correlation_window_height=3;
       wdm.params.paramslist[i].correlation_window_width=3;
       wdm.params.paramslist[i].inner_cost=1.0;
       wdm.params.paramslist[i].outer_cost=0.25;
   }
   wdm.do_dense_matching();
 
   //wdm.display_matrices(wdm.get_correspondence());

   /*for(unsigned int i=0;i<wdm.f_assignments.rows();i++)
   {
       for(unsigned int j=0;j<wdm.f_assignments.cols();j++)
       {
        vcl_cout<<wdm.f_assignments(i,j)-j<<" ";

       }
        vcl_cout<<"\n";
   }*/

   return testlib_test_summary();   
}


