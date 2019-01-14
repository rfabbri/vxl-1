//: 
// \file  imgr_babe_rsq_generator.cxx
// \brief  an example to  generator a babe rsq file format
// \author    Kongbin Kang
// \date        2005-04-18
// 
#include <vil/vil_stream_fstream.h>
#if VXL_WIN32 
#include <vil/vil_stream_fstream_64.h>
#endif
#include <vil/vil_stream_write.h>
#include <imgr/file_formats/imgr_rsq_header.h>
#include <imgr/file_formats/imgr_rsq.h>

int main(int argc, char* argv[])
{

  
  vil_stream* is = new vil_stream_fstream("/mnt/backup/data/scanco0405/c0000892.rsq", "r");

  if(!is->ok()){
    std::cout << "cannot open file to read \n";
    exit(1);
  }

  //: construct a header to gain information
  imgr_rsq_header rsq_header(is);

  std::cout << "number of bytes in the original files is " << rsq_header.nr_of_bytes_ << "\n";

#if 0
//this variable is not used in the code.  PLEASE FIX!  -MM
  int num_bytes = rsq_header.nr_of_bytes_;
#endif

  vil_stream* os = new vil_stream_fstream("babe.rsq", "w");

  if(!os->ok()){
    std::cout << "cannot open file to write \n";
    exit(1);
  }

  unsigned header_block = 1 + rsq_header.data_offset_;

  vxl_byte bytes[512];

  

  is->seek(0);
  os->seek(0);
  for(unsigned i = 0; i < header_block; i++)
  {
    is->read(bytes, 512);

    os->write(bytes, 512);
  }

  // write images include first two calibrate imagesa
  unsigned bytes_per_image = 2*rsq_header.dimx_p_ * rsq_header.dimz_p_;

  // copy an images
 vxl_byte * image = new vxl_byte[bytes_per_image];

 for(unsigned i = 0; i< 3 ; i++){
   is->read(image, bytes_per_image);
   os->write(image, bytes_per_image);
 }


 int total_num_bytes = header_block*512 + 3*bytes_per_image;
 
 // change the header file
 os->seek(20);
 vil_stream_write_little_endian_uint_32(os, total_num_bytes); 
 vil_stream_write_little_endian_uint_32(os, 0);

 os->seek(48);
 vil_stream_write_little_endian_uint_16(os, 3);

 // destroy the memeroy allocated
 delete [] image;

  vil_stream* babe_is = new vil_stream_fstream("babe.rsq", "r");
  imgr_rsq_header babe_rsq_header(babe_is);

  std::cout << babe_rsq_header.check_ << "\n";

  std::cout << babe_rsq_header.data_type_ << "\n";

  std::cout << "number of bytes " << babe_rsq_header.nr_of_bytes_ << "\n";

  std::cout << "number of block " << babe_rsq_header.nr_of_blocks_ << "\n";

  std::cout << babe_rsq_header.patient_index_ << "\n";

  std::cout << babe_rsq_header.scanner_id_ << "\n";

  std::cout << babe_rsq_header.creation_date_[0] <<'\n' << babe_rsq_header.creation_date_[1] <<'\n';

  std::cout <<  babe_rsq_header.dimx_p_ <<'\n'<<babe_rsq_header.dimy_p_ << '\n' << babe_rsq_header.dimz_p_ <<  "\n";

  std::cout << babe_rsq_header.dimx_um_ <<'\n' << babe_rsq_header.dimy_mdeg_ << '\n' << babe_rsq_header.dimz_um_ <<'\n';

  std::cout << babe_rsq_header.slice_thickness_um_ << '\n';

  std::cout << babe_rsq_header.slice_increment_um_ << '\n';

  std::cout << babe_rsq_header.slice_1_pos_um_ << '\n';

  std::cout << babe_rsq_header.scanner_type_ << '\n';

  std::cout<< babe_rsq_header.dummy1_ << '\n';
  
  std::cout<< babe_rsq_header.dummy2_ << '\n';
  
  std::cout<< babe_rsq_header.dummy3_ << '\n';

  std::cout<< babe_rsq_header.dummy4_ << '\n';

  std::cout<< babe_rsq_header.hext_offset_ << '\n';

  std::cout << babe_rsq_header.data_offset_ << "\n";

  std::cout << "detector length " <<babe_rsq_header.u_.area_.nr_of_det_x_ << "\n";
  
  std::cout << "detector heigh " << babe_rsq_header.u_.area_.nr_of_det_y_ << "\n";
  
  std::cout << "detector length " <<babe_rsq_header.u_.area_.detector_length_x_um_ << "\n";
  
  std::cout << "detector heigh " << babe_rsq_header.u_.area_.detector_length_y_um_ << "\n";

  std::cout << "principle_x = "<<babe_rsq_header.u_.area_.detector_center_x_p_ <<'\n';

  std::cout << "principle_y = "<<babe_rsq_header.u_.area_.detector_center_y_p_ << '\n';

  std::cout << "detector_angle_x_mdeg_ = "<<babe_rsq_header.u_.area_.detector_angle_x_mdeg_ << "\n";

  std::cout << "detector angle y mdeg_ = " << babe_rsq_header.u_.area_.detector_angle_y_mdeg_ << "\n";

  std::cout << "detector angle z mdeg = " << babe_rsq_header.u_.area_.detector_angle_z_mdeg_ << "\n";

  std::cout <<"detector distance = "<< babe_rsq_header.u_.area_.detector_distance_um_ << "\n";
  
  std::cout << "center distance " << babe_rsq_header.u_.area_.rotation_center_distance_um_<< "\n";

  std::cout <<"i0 pixel left side (" << babe_rsq_header.u_.area_.i0_pixel_l_x_ << ", " << babe_rsq_header.u_.area_.i0_pixel_u_y_ << ")\n";
  
  std::cout <<"i0 pixel right bottom side (" << babe_rsq_header.u_.area_.i0_pixel_r_x_ << ", " << babe_rsq_header.u_.area_.i0_pixel_d_y_ << ")\n";

  std::cout <<"data pixel left side (" << babe_rsq_header.u_.area_.data_pixel_l_x_ << ", " << babe_rsq_header.u_.area_.data_pixel_u_y_ << ")\n";
  
  std::cout <<"data pixel right bottom side (" << babe_rsq_header.u_.area_.data_pixel_r_x_ << ", " << babe_rsq_header.u_.area_.data_pixel_d_y_ << ")\n";

  std::cout <<"i0 index " << babe_rsq_header.u_.area_.i0_index_ << "\n";

  std::cout <<"dark index " << babe_rsq_header.u_.area_.dark_index_ << "\n";

  std::cout <<"data index " << babe_rsq_header.u_.area_.data_index_ << "\n";

  std::cout << "index_measurement " << babe_rsq_header.u_.area_.index_measurement_ << "\n";
    
  std::cout << "z pos 1st frame center " << babe_rsq_header.u_.area_.z_pos_1stframe_center_um_ << "\n";
 // release the stream

  return 0;
}
