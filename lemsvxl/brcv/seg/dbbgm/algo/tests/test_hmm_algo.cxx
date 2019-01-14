#include <testlib/testlib_test.h>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <dbbgm/algo/dbbgm_hmm_algo.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vil/vil_plane.h>
#include <vil/vil_crop.h>
#include <vil/vil_convert.h>
#include <vul/vul_timer.h>
#include <vnl/vnl_random.h>

namespace{

void normalize_probs(vil_image_view<float>& img)
{
  vnl_random rand;
  for(unsigned int j=0; j<img.nj(); ++j){
    for(unsigned int i=0; i<img.ni(); ++i){
      float sum = 0.0f;
      for(unsigned int p=0; p<img.nplanes(); ++p){
        sum += img(i,j,p);
      }
      if(sum > std::numeric_limits<float>::epsilon()){
        for(unsigned int p=0; p<img.nplanes(); ++p)
          img(i,j,p) /= sum;
      }
      else{
        for(unsigned int p=0; p<img.nplanes(); ++p)
          img(i,j,p) = 1.0f/img.nplanes();
      }
    }
  }
}

void add_random_noise(vil_image_view<float>& img, float std)
{
  vnl_random rand;
  for(unsigned int p=0; p<img.nplanes(); ++p)
    for(unsigned int j=0; j<img.nj(); ++j)
      for(unsigned int i=0; i<img.ni(); ++i){
        img(i,j) = img(i,j) + rand.normal()*std;
        if(img(i,j)<0) img(i,j) = 0;
      }
}


};

MAIN( test_hmm_algo )
{
  const unsigned int ni = 320, nj = 240;

  vil_image_view<float> img(ni,nj,3);
  img.fill(1.0);
  vil_plane(img,0).fill(3.0);
  vil_plane(vil_crop(img,10,50,30,50),1).fill(10.0);
  vil_plane(vil_crop(img,30,20,20,60),2).fill(9.0);
  add_random_noise(img,0.5);
  normalize_probs(img);

  vil_image_view<double> dimg(ni,nj,3);
  vil_image_view<vxl_byte> bimg(ni,nj,3);


  vil_convert_stretch_range_limited(img,dimg,0.0f,1.0f,0.0,255.0);
  vil_convert_cast(dimg,bimg);
  vil_save(bimg,"orig.png");

  for(unsigned int i=0; i<10; ++i){
    vil_image_view<float> probs; probs.deep_copy(img);
    dbbgm_mean_field(probs,4,10*i);

    vil_convert_stretch_range_limited(probs,dimg,0.0f,1.0f,0.0,255.0);
    vil_convert_cast(dimg,bimg);
    std::stringstream name;
    name << "mean"<<std::setfill('0')<<std::setw(2)<<i<<".png";
    //vil_save(bimg,name.str().c_str());
  }

  SUMMARY();
}


