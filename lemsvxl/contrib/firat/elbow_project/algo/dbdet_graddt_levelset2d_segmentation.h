/*
 * dbdet_graddt_levelset2d_segmentation.h
 *
 *  Created on: Feb 22, 2012
 *      Author: firat
 */

#ifndef DBDET_GRADDT_LEVELSET2D_SEGMENTATION_H_
#define DBDET_GRADDT_LEVELSET2D_SEGMENTATION_H_

#include "dbdet_levelset2d_segmentation.h"
#include "dbdet_third_order_3d_edge_detector.h"
#include <vil3d/vil3d_image_view.h>

class dbdet_graddt_levelset2d_segmentation : public dbdet_levelset2d_segmentation
{
public:
	virtual void reinitialize();
	void set_narrowband(double band);
	void set_sussman_reinitilization_threshold(double phi_thresh);
protected:
	void narrowband_reinitialization_();
	void sussman_reinitialization_();

	double band_;
	double phi_threshold_;
};

void dbdet_compute_graddt_force(vcl_vector<dbdet_3d_edge_sptr>& edgemap, vil_image_view<double>& DT,
		vil_image_view<double>& Fx,  vil_image_view<double>& Fy, int slice_index, int width,
		int height, double hx, double hy, double dist_thresh);

void dbdet_classify_edges(vil3d_image_view<double>& V,  vcl_vector<dbdet_3d_edge_sptr>& orig_edgemap,
		vcl_vector<dbdet_3d_edge_sptr>& FG_edgemap, vcl_vector<dbdet_3d_edge_sptr>& BG_edgemap, double BG_thresh);

void dbdet_get_active_regions(vil_image_view<double>& DT, vil_image_view<bool>& external_active,  vil_image_view<bool>& external_inactive, double dist_thresh);

#endif /* DBDET_GRADDT_LEVELSET2D_SEGMENTATION_H_ */
