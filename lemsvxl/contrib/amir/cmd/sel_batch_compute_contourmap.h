// This is sel_batch_compute_contourmap.h
#ifndef sel_batch_compute_contourmap_h
#define sel_batch_compute_contourmap_h
//:
//\file
//\brief 
//\author Amir Tamrakar
//\date 11/25/06
//
//\verbatim
//  Modifications
//\endverbatim

#include <vcl_vector.h>
#include <vcl_string.h>

#include <vil/vil_image_view.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>

#include <dbdet/sel/dbdet_edgel.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/sel/dbdet_sel_sptr.h>

dbdet_edgemap_sptr compute_generic_edges(vil_image_view<vxl_byte>& image, int N, vcl_string filename);

dbdet_edgemap_sptr compute_third_order_edges(vil_image_view<vxl_byte>& image, vcl_string filename);

void link_edges(dbdet_edgemap_sptr edgemap, double nrad, unsigned maxN, double dx_sel, double dt_deg, vcl_string filename);

bool saveEDG(vcl_string filename, dbdet_edgemap_sptr edgemap);

bool saveCEM (vcl_string filename, dbdet_sel_sptr edge_linker);

#endif // sel_batch_compute_contourmap_h
