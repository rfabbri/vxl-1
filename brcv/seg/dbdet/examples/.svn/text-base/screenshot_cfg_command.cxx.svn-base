//---------------------------------------------------------------------
// This is dbdet/examples/screenshot_cfg_command.cxx
//:
// \file
// \brief command line program that takes an image and a curve fragment map file (.CEM)
//        and draw the CFM on the image
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Nov 17, 2008
// \verbatim
//  Modifications
// \endverbatim
//
//-------------------------------------------------------------------------

#include <dbdet/algo/dbdet_screenshot.h>
#include <dbdet/algo/dbdet_cem_file_io.h>
#include <dbdet/algo/dbdet_screenshot.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>


//: Main program
int main( int argc, char* argv[] )
{
  if (argc != 4) {
    vcl_cout << "USAGE: " << argv[0] << " <image file name> <curve fragment map file (.cem)> <screenshot image file>\n";
    return 1;
  }

  vcl_string image_file = argv[1];
  vcl_string cfg_file = argv[2];
  vcl_string screenshot_file = argv[3];
  
  // load image
  vil_image_view<vxl_byte> image_view  = vil_load(image_file.c_str());

  // load contour map

  //get pointers to the data structures in it
  dbdet_curve_fragment_graph CFG;
  dbdet_edgemap_sptr EM = dbdet_load_cem(cfg_file, CFG);

  if (!EM) 
  {
    vcl_cout << "Unable to load the boundary .cem file " 
      << cfg_file << ". Quit now.\n";
    return 1;
  }

  // draw curve fragment map on the image
  dbdet_screenshot(CFG, image_view);

  // save the screenshot to file
  if (!vil_save(image_view, screenshot_file.c_str()))
  {
    vcl_cout << "\nERROR: couldn't save screenshot to file: " << screenshot_file << vcl_endl;
    return 1;
  }

  return 0;
}

