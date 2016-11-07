// This is /lemsvxl/contrib/firat/xgraph_to_binary/dbsksp_convert_xgraph_to_binary_mask_utils.h.

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date Jul 1, 2010
//

#ifndef DBSKSP_CONVERT_XGRAPH_TO_BINARY_MASK_UTILS_H_
#define DBSKSP_CONVERT_XGRAPH_TO_BINARY_MASK_UTILS_H_

#include <vil/vil_image_view.h>
#include <dbsksp/dbsksp_xshock_graph_sptr.h>


bool dbsksp_draw_binary_map(const dbsksp_xshock_graph_sptr& xgraph, vil_image_view<vxl_byte >& screenshot);

#endif /* DBSKSP_CONVERT_XGRAPH_TO_BINARY_MASK_UTILS_H_ */
