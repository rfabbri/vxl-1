// This is shp/dbsksp/algo/dbsksp_screenshot.h
#ifndef dbsksp_screenshot_h_
#define dbsksp_screenshot_h_

//:
// \file
// \brief Collection of functions to capture the look of the shock graph
//        
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Nov 6, 2008
//
// \verbatim
//  Modifications
// \endverbatim

//

#include <vil/vil_image_view.h>
#include <dbsksp/dbsksp_xshock_graph_sptr.h>
#include <vil/vil_rgb.h>



//: Create a screenshot of an xgraph
bool dbsksp_screenshot(const vil_image_view<vxl_byte >& image, 
                       const dbsksp_xshock_graph_sptr& xgraph,
                       vil_image_view<vxl_byte >& screenshot,
                       int contour_radius = 1, // total width = 3
                       int padding_width = 1, // padding 1 pixel on each side
                       vil_rgb<vxl_byte > contour_color = vil_rgb<vxl_byte >(0, 0, 255), // blue
                       vil_rgb<vxl_byte > padding_color = vil_rgb<vxl_byte >(255, 255, 255) // white
                       );

//: Draw the shock graph on top of an existing image
bool dbsksp_screenshot_in_place(const dbsksp_xshock_graph_sptr& xgraph,
                       vil_image_view<vxl_byte >& screenshot,
                       int contour_radius,// = 1, // total width = 3
                       int padding_width,// = 1, // padding 1 pixel on each side
                       vil_rgb<vxl_byte > contour_color,// = vil_rgb<vxl_byte >(0, 0, 255), // blue
                       vil_rgb<vxl_byte > padding_color// = vil_rgb<vxl_byte >(255, 255, 255) // white
                       );




#endif // shp/dbsksp/dbsksp_screenshot.h









