/*
 * dbdet_third_order_3d_edge_detector.h
 *
 *  Created on: Oct 11, 2011
 *      Author: firat
 */

#ifndef DBDET_THIRD_ORDER_3D_EDGE_DETECTOR_H_
#define DBDET_THIRD_ORDER_3D_EDGE_DETECTOR_H_

#include <vil3d/vil3d_image_view.h>
#include <vcl_vector.h>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vbl/vbl_smart_ptr.txx>

class dbdet_1d_gaussian_derivative_filters
{
public:
	double* G0, *G1, *G2, *G3;
	int filter_size;
	dbdet_1d_gaussian_derivative_filters(double sigma, double h);
	static int get_filter_size(double sigma);
	~dbdet_1d_gaussian_derivative_filters();
};

class dbdet_3d_edge : public vbl_ref_count
{
public:
	double x,y,z,nx,ny,nz,strength;
};

VBL_SMART_PTR_INSTANTIATE(dbdet_3d_edge);

typedef vbl_smart_ptr<dbdet_3d_edge> dbdet_3d_edge_sptr;

bool dbdet_third_order_3d_edge_detector(const vil3d_image_view<double>& W, vcl_vector<dbdet_3d_edge_sptr>& edgemap,
		double strength_threshold, double sigma, double h);

bool dbdet_third_order_3d_edge_detector_roi(const vil3d_image_view<double>& W, vcl_vector<dbdet_3d_edge_sptr>& edgemap,
		double strength_threshold, double sigma, double h, int i0, int j0, int k0, int ni_roi, int nj_roi, int nk_roi);

bool dbdet_compute_gradF_gradI(const vil3d_image_view<double>&W, vil3d_image_view<double>& Fx, vil3d_image_view<double>& Fy,
		vil3d_image_view<double>& Fz, vil3d_image_view<double>& Ix, vil3d_image_view<double>& Iy,
		vil3d_image_view<double>& Iz, double sigma, double h);

bool dbdet_nms_3d(const vil3d_image_view<double>&Fx, const vil3d_image_view<double>&Fy, const vil3d_image_view<double>&Fz,
		const vil3d_image_view<double>& mag_gradI, vcl_vector<dbdet_3d_edge_sptr>& partial_edgemap, double strength_threshold);

bool dbdet_separable_convolve3d(const vil3d_image_view<double>& V, vil3d_image_view<double>& VF, int L, double* F1, double* F2, double* F3);

bool dbdet_keep_strong_edges(vcl_vector<dbdet_3d_edge_sptr>& edgemap_in, vcl_vector<dbdet_3d_edge_sptr>& edgemap_out, double strength_threshold, bool modify_original);

#endif /* DBDET_THIRD_ORDER_3D_EDGE_DETECTOR_H_ */
