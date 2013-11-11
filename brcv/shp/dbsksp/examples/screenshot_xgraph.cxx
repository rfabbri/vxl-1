// This is shp/dbsksp/examples/screenshot_xgraph.cxx

// \file


//#include <vcl_iostream.h>
//#include <vcl_cmath.h> // for vcl_sqrt()
//#include <vcl_vector.h>
//#include <vul/vul_timer.h>

#include <dbsksp/algo/dbsksp_screenshot.h>
#include <dbsksp/xio/dbsksp_xio_xshock_graph.h>
#include <vil/vil_load.h>
#include <vil/vil_convert.h>
#include <vil/vil_save.h>


//: Read in a shock graph and an image and produce an image overlays the shock 
// graph on top of the original image
int main(int argc, char *argv[])
{
  if (argc < 4)
  {
    vcl_cout << "This program draws a xshock graph on top of an existing (source) image\n" 
      << "Usage: " << argv[0] << " <xgraph xml> <source image> <new image>\n";
    return 0;
  }

  vcl_string xgraph_file = argv[1];
  vcl_string source_image_file = argv[2];
  vcl_string target_image_file = argv[3];

  // Load shock graph
  dbsksp_xshock_graph_sptr xgraph;
  if (!x_read(xgraph_file, xgraph))
  {
    vcl_cout << "ERROR: couldn't read xgraph XML file - " << xgraph_file << vcl_endl;
    return 1;
  }

  // Load source image
  vil_image_resource_sptr image_resource = vil_load_image_resource(source_image_file.c_str());
  if (!image_resource)
  {
    vcl_cout << "ERROR: couldn't load source image - " << source_image_file << vcl_endl;
    return 1;
  }

  // convert to image_view
  vil_image_view<vxl_byte > source_image = *vil_convert_cast(vxl_byte(), image_resource->get_view());

  // draw the shock graph on top of the source image
  vil_image_view<vxl_byte > target_image;
  dbsksp_screenshot(source_image, xgraph, target_image);

  // save image
  if (!vil_save(target_image, target_image_file.c_str()))
  {
    vcl_cout << "ERROR: couldn't save image to target image file - " << target_image_file << vcl_endl;
    return 1;
  }  
  return 0;
}

