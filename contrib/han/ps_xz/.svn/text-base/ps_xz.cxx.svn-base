//---------------------------------------------------------------------
// This is dbdet/examples/contourtracing_command.cxx
//:
// \file
// \brief command line program that takes image name and contour traces the image
// 
// 
//
// \author
// 
//
// 
//
// 
//
// \verbatim
//  Modifications
// i
// \endverbatim
//
//-------------------------------------------------------------------------

#include <dbdet/tracer/dbdet_contour_tracer.h>
#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_print.h>
#include <vil/algo/vil_threshold.h>
#include <vul/vul_psfile.h>
#include <bdgl/bdgl_curve_algs.h>
#include "ps_xz.h"

int ni,nj;
int main( int argc, char* argv[] )
{
  char strtemp1[100];
  char strtemp2[100];

  if (argc != 3 &&argc != 4) {
    vcl_cout << "USAGE: ps_xz.exe <res*.txt> <xz =>0 yz=>1> <bottom height>\n";
    return 1;
  }

  vcl_string image_filename(argv[1]);
  //float s = atof(argv[2]);
  //unsigned cnt = atoi(argv[3]);



 // vul_file_iterator fn="./*.png";
  //vul_file_iterator fn=input_file_path+"/*.con";

 // vcl_string input_file = fn();
 // vil_image_view<vxl_byte> loaded_image  = vil_load(input_file.c_str());

 // int ni=loaded_image.ni();
 // int nj=loaded_image.nj();

  //// read_result_txt_file(argv[1],vul_string_to_bool(argv[2]),argv[3]);
  read_result_txt_file_both(argv[1],vul_string_to_bool(argv[2]));
  bottom_=vul_string_atof(argv[3]);
  read_result_txt_file_vgl_point_3d(argv[1],vul_string_to_bool(argv[2]),bottom_);






  return 0;


}

