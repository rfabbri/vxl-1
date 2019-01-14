//: 
// \file    rsq_to_tiff_converter.cxx
// \brief   This program reads an rsq file, and saves the first two 
//          calibration images as two tiff image
// 

#include <vil/vil_stream_fstream.h>
#if VXL_WIN32 
#include <vil/vil_stream_fstream_64.h>
#endif
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>

#include <cstddef>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#include <imgr/file_formats/imgr_rsq.h>

int main(int argc, char* argv[])
{

  if(argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " rsqfile";
    exit(1);
  }

  vil_stream* is = new vil_stream_fstream(argv[1], "r");
  imgr_rsq rsq_reader(is);

  // unsigned int interval = 10;
  std::vector<vil_image_resource_sptr> img_res_sptrs = rsq_reader.get_cali_images();
  double min = rsq_reader.min_intensity();
  double max = rsq_reader.max_intensity();
  unsigned nk = img_res_sptrs.size();
  std::cout << "\nminimum: " << min << std::endl;
  std::cout << "maximum: " << max << std::endl;
  std::cout << "number of images: " << nk << std::endl;

  char out_fname[255];

  for(unsigned k = 0; k<nk; ++k)
    //   for(unsigned k = 0; k<nk; k = k+100)

  {
    std::sprintf(out_fname, "%d", k);
    std::strcat(out_fname, ".tiff");

    std::cout <<"enter the " << k <<"th resource "<< std::flush;

    unsigned ni = img_res_sptrs[k]->ni();
    unsigned nj = img_res_sptrs[k]->nj();

    std::cout<<" "<<ni<<std::endl;
    std::cout<<" "<<nj<<std::endl;

    //   vil_image_view<unsigned short> v(img_res_sptrs[k]->get_view(0,ni,0,nj));
    vil_image_view<unsigned short> v(img_res_sptrs[k]->get_view());
    std::cout << "open view [ " << k <<"] " << v.ni() << ' ' << v.nj() << '\n';
    vil_save(v, out_fname, "tiff");
  }

  return 0;
}
