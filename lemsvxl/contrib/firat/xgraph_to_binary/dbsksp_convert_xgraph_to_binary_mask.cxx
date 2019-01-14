// This is /lemsvxl/contrib/firat/xgraph_to_binary/dbsksp_convert_xgraph_to_binary_mask.cxx.

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date Jul 1, 2010
//

#include <iostream>
#include <vil/vil_convert.h>
#include <vil/vil_new.h>
#include <vil/vil_save.h>
#include <dbsksp/xio/dbsksp_xio_xshock_graph.h>
#include "dbsksp_convert_xgraph_to_binary_mask_utils.h"


int main(int argn, char* args[])
{
    std::cout << "Number of input arguments: " << argn <<std::endl;
    if(argn != 5)
    {
        std::cout << "Not enough inputs!" <<std::endl;
        return 0;
    }

    std::string xgraph_file = args[1];
    std::string out_image_file = args[2];
    int width = atoi(args[3]);
    int height = atoi(args[4]);

    std::cout << "Xgraph file path: " << xgraph_file << std::endl;
    std::cout << "[W, H] = [" << width << " ," << height << "]" << std::endl;

    vil_image_resource_sptr image_resource = vil_new_image_resource(width, height, 1,  VIL_PIXEL_FORMAT_BYTE);
    vil_image_view<vxl_byte > bg_image = *vil_convert_cast(vxl_byte(), image_resource->get_view());
    bg_image.fill(0);

    dbsksp_xshock_graph_sptr xg = 0;
    x_read(xgraph_file, xg);

    dbsksp_draw_binary_map(xg, bg_image);
    vil_save(bg_image, out_image_file.c_str());

    return 0;
}
