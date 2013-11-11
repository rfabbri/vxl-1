#include <slice/algo/sliceRidgeDetectProcessor.h>
#include <slice/sliceEngine.h>
#include <slice/sliceFileManager.h>
#include <vcl_fstream.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/io/vil3d_io_image_view.h>
#include <vil3d/algo/vil3d_grad_1x3.h>
#include <vil3d/vil3d_math.h>
#include <vil3d/vil3d_transform.h>
#include <basic/dbil3d/algo/dbil3d_gauss_filter.h> 
#include <vcl_cmath.h>

void saveout(vcl_string fname, const vil3d_image_view<float> & img){
        vsl_b_ofstream bfs_out(fname.c_str());
        vsl_b_write(bfs_out, img);
        bfs_out.close();
}

class vil_math_abs_functor
{
 public:
  vxl_byte operator()(vxl_byte x) const { return vcl_abs(x); }
  unsigned operator()(unsigned x) const { return x; }
  int operator()(int x)           const { return vcl_abs(x); }
  short operator()(short x)       const { return vcl_abs(x); }
  float operator()(float x)       const { return vcl_fabs(x); }
  double operator()(double x)     const { return vcl_fabs(x); }
};

int main(int argc, char* argv[])
{        
        if(argc < 2){
                vcl_cerr << "usage : " << argv[0]  << " <vil3d binary image file> | <gx slice file> <gy slice file> <gz slice file>\n";
                return 1;
        }
        else if(argc < 4){
                vcl_cerr << "usage : " << argv[0]  << " <vil3d binary image file> | <gx slice file> <gy slice file> <gz slice file>\n";
                return 1;
        }


        vcl_string gx_file,gy_file,gz_file;
        if(argc == 2)
        {
          vil3d_image_view<float> volume;
          vsl_b_ifstream bfs_in(argv[1]);
          vsl_b_read(bfs_in, volume);
          bfs_in.close();

          vil3d_image_view<float> smoothed;
          vil3d_image_view<float> gx;
          vil3d_image_view<float> gy;
          vil3d_image_view<float> gz;
          dbil3d_gauss_filter(volume,1.5,smoothed);
          vil3d_grad_1x3(smoothed,gx,gy,gz);

          sliceFileManager<float>::write(gx,"gx.out");
          sliceFileManager<float>::write(gy,"gy.out");
          sliceFileManager<float>::write(gz,"gz.out");

          gx_file = "gx.out";
          gy_file = "gy.out";
          gz_file = "gz.out";
        }
        else if (argc == 4){
          gx_file = argv[1];
          gy_file = argv[2];
          gz_file = argv[3];
        }


        sliceEngine<float> engine;
        engine.addStream(gx_file);
        engine.addStream(gy_file);
        engine.addStream(gz_file);


        /*
         * using the engine to save the output to a memory location
        vil3d_image_view<float> lambda1(w,h,d);
        vil3d_image_view<float> lambda2(w,h,d);
        vil3d_image_view<float> rho(w,h,d);

        lambda1.fill(0);
        lambda2.fill(0);
        rho.fill(0);

        vcl_vector<float*> outputs;
        outputs.push_back(rho.origin_ptr());
        outputs.push_back(lambda1.origin_ptr());
        outputs.push_back(lambda2.origin_ptr());
        engine.setSaveToMemory(outputs);
        */
        vcl_vector<vcl_string> outputs;
        outputs.push_back("rho.out");
        outputs.push_back("lambda1.out");
        outputs.push_back("lambda2.out");
        engine.setSaveToDisk(outputs);

        sliceRidgeDetectProcessor proc(0.7);
        engine.processWith(&proc);

        vil3d_image_view<float> rho,lambda1,lambda2;

        rho = sliceFileManager<float>::read("rho.out");
        lambda1 = sliceFileManager<float>::read("lambda1.out");
        lambda2 = sliceFileManager<float>::read("lambda2.out");

        saveout("rho.float" , rho);
        saveout("lambda1.float" , lambda1);
        saveout("lambda2.float" , lambda2); 

        vil3d_image_view<float> arithmetic_mean(rho.ni(),rho.nj(),rho.nk());
        vil3d_math_image_sum(lambda1,lambda2,arithmetic_mean);
        vil3d_transform(arithmetic_mean,arithmetic_mean,vil_math_abs_functor());
        vil3d_math_scale_and_offset_values(arithmetic_mean, 0.5, 0);

        float avg_lambda_min,avg_lambda_max;
        vil3d_math_value_range(arithmetic_mean,avg_lambda_min,avg_lambda_max);

        vcl_cerr << "avg_lambda_min " << avg_lambda_min << " avg_lambda_max " << avg_lambda_max << "\n";
        vil3d_image_view<float> test(rho.ni(),rho.nj(),rho.nk());
        test.fill(0);

        vcl_cerr << "setting cutoff to avg_lambda_max * 0.8\n" ;
        double cutoff = 0.08*avg_lambda_max;

        for(int k = 0; k < rho.nk(); k++){
        for(int j = 0; j < rho.nj(); j++){
        for(int i = 0; i < rho.ni(); i++){
                double diff = vcl_fabs(lambda1(i,j,k) - lambda2(i,j,k));
                float mean = arithmetic_mean(i,j,k); 
                double ratio = diff/mean;
    
                if(rho(i,j,k) == 1  && mean > cutoff){
                }
                if(rho(i,j,k) == 1 )
                  test(i,j,k) = ratio;
        }
        }
        }
      
        vcl_cerr << " Writing out to test.float ... \n";
        saveout("test.float" , test);
        return 0;
}
