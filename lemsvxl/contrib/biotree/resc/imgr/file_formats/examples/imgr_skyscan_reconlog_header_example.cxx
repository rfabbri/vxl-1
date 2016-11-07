//: 
// \file    imgr_skyscan_reconlog_header.cxx
// \brief   an example to test SkyScan reconlog header reader
// \author  K. Kang
// \date    2006-2-14
// 
#include <vcl_iostream.h>
#include <imgr/file_formats/imgr_skyscan_reconlog.h>

int main(int argc, char* argv[])
{
  if(argc < 2)
  {
    vcl_cerr << "usage:\n"<<argv[0]<<" reconstructed_image_header_name\n";
    exit(1);
  }
  
  imgr_skyscan_reconlog header(argv[1]);

  if(header.is_valid())
  {
    vcl_cout << "starting slice is: " << header.start_slice_<<'\n';
    vcl_cout << "ending slice is: " << header.end_slice_<<'\n';
    vcl_cout << "slice step is: " << header.slice_step_ <<'\n';
    vcl_cout << "voxel size is: " << header.voxel_size_ <<'\n';
#if 0
    vcl_cout << "sensor source distance is: " << header.src_snsr_dist_ <<'\n';
    vcl_cout << "source rotation distance is: " << header.src_rot_dist_ <<'\n';
#endif
  
    vcl_cout << header.fbpc_to_bsc(vgl_point_3d<double>(855, 1119, 347));
   }


  return 0;
}
