//: 
// \file    imgr_skyscan_reconlog_header.cxx
// \brief   an example to test SkyScan reconlog header reader
// \author  K. Kang
// \date    2006-2-14
// 
#include <iostream>
#include <imgr/file_formats/imgr_skyscan_reconlog.h>

int main(int argc, char* argv[])
{
  if(argc < 2)
  {
    std::cerr << "usage:\n"<<argv[0]<<" reconstructed_image_header_name\n";
    exit(1);
  }
  
  imgr_skyscan_reconlog header(argv[1]);

  if(header.is_valid())
  {
    std::cout << "starting slice is: " << header.start_slice_<<'\n';
    std::cout << "ending slice is: " << header.end_slice_<<'\n';
    std::cout << "slice step is: " << header.slice_step_ <<'\n';
    std::cout << "voxel size is: " << header.voxel_size_ <<'\n';
#if 0
    std::cout << "sensor source distance is: " << header.src_snsr_dist_ <<'\n';
    std::cout << "source rotation distance is: " << header.src_rot_dist_ <<'\n';
#endif
  
    std::cout << header.fbpc_to_bsc(vgl_point_3d<double>(855, 1119, 347));
   }


  return 0;
}
