// This is brcv/seg/dbdet/algo/dbdet_pb_edge_detector.h

//:
// \file
// \brief
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date Feb 16, 2010

#ifndef DBDET_PB_EDGE_DETECTOR_H_
#define DBDET_PB_EDGE_DETECTOR_H_

#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <vil/vil_image_view.h>
#include <vcl_string.h>

class dbdet_pb_edge_detector
{
private:
    vcl_string temp_dir_;
public:
    dbdet_pb_edge_detector(const vcl_string& temp_dir);
    ~dbdet_pb_edge_detector();
    dbdet_edgemap_sptr detect_bg(vil_image_view<vxl_byte>& image, double radius, int norient);
    dbdet_edgemap_sptr detect_cg(vil_image_view<vxl_byte>& image, double radius, int norient);
    dbdet_edgemap_sptr detect_gm(vil_image_view<vxl_byte>& image, double sigma);
    dbdet_edgemap_sptr detect_gm2(vil_image_view<vxl_byte>& image, double sigma);
    dbdet_edgemap_sptr detect_tg(vil_image_view<vxl_byte>& image, double radius, int norient);
    dbdet_edgemap_sptr detect_bgtg(vil_image_view<vxl_byte>& image, const vcl_string& pres, double radius[], int norient);
    dbdet_edgemap_sptr detect_cgtg(vil_image_view<vxl_byte>& image, double radius[], int norient);

    dbdet_edgemap_sptr detect_bg(vil_image_view<vxl_byte>& image);
    dbdet_edgemap_sptr detect_cg(vil_image_view<vxl_byte>& image);
    dbdet_edgemap_sptr detect_gm(vil_image_view<vxl_byte>& image);
    dbdet_edgemap_sptr detect_gm2(vil_image_view<vxl_byte>& image);
    dbdet_edgemap_sptr detect_tg(vil_image_view<vxl_byte>& image);
    dbdet_edgemap_sptr detect_bgtg(vil_image_view<vxl_byte>& image);
    dbdet_edgemap_sptr detect_cgtg(vil_image_view<vxl_byte>& image);
};
#endif /* DBDET_PB_EDGE_DETECTOR_H_ */
