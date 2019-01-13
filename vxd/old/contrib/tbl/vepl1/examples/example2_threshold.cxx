// This is tbl/vepl1/examples/example2_threshold.cxx

//:
// \file
//  This example program shows a typical use of the vepl1_threshold function on
//  a ubyte image.  The input image (argv[1]) must be ubyte, and in that
//  case is clipped (threshold value argv[3], default 10) to argv[2]
//  which is always a PGM file image.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   15 May 2001, from vipl/examples
//
#include <vil1/vil1_pixel.h>

#include <vepl1/vepl1_threshold.h>

#include <vxl_config.h> // for vxl_byte

// for I/O:
#include <vil1/vil1_load.h>
#include <vil1/vil1_save.h>
#include <iostream>
#include <cstdlib> // for atoi()

int
main(int argc, char** argv)
{
  if (argc < 3)
  {
    std::cerr << "Syntax: example2_threshold file_in file_out [threshold]\n";
    return 1;
  }

  // The input image:
  vil1_image in = vil1_load(argv[1]);
  if (vil1_pixel_format(in) != VIL1_BYTE)
  {
    std::cerr << "Please use a ubyte image as input\n";
    return 2;
  }

  // The threshold value:
  vxl_byte threshold = (argc < 4) ? vxl_byte(128) : vxl_byte(std::atoi(argv[3]));

  // perform thresholding:
  vil1_image out = vepl1_threshold(in,threshold,0,255);

  // Write output:
  vil1_save(out, argv[2], "pnm");
  std::cout << "Written image of type PGM to " << argv[2] << std::endl;

  return 0;
}
