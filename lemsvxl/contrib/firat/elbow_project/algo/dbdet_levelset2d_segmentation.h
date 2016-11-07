/*
 * dbdet_levelset2d_segmentation.h
 *
 *  Created on: Feb 8, 2012
 *      Author: firat
 */

#ifndef DBDET_LEVELSET2D_SEGMENTATION_H_
#define DBDET_LEVELSET2D_SEGMENTATION_H_

#include <vil/vil_image_view.h>

// a generic 2d levelset segmentation class
class dbdet_levelset2d_segmentation
{
public:
	dbdet_levelset2d_segmentation();
	void initialize(const vil_image_view<double>& init, double hx, double hy);
	virtual void reinitialize() = 0;
	void set_reinitialization_frequency(int k);
	void use_reaction_flux(bool flag);
	void use_diffusion_flux(bool flag);
	void use_external_force(bool flag);
	void set_external_force_field(const vil_image_view<double>& Fx, const vil_image_view<double>& Fy);
	// beta0: if positive -> grow
	//		  if negative -> shrink
	// beta1: must be <= 0
	// alpha: must be >= 0
	void set_params(double beta0, double beta1, double alpha);
	void set_active_regions(const vil_image_view<bool>& active_reaction, const vil_image_view<bool>& active_diffusion, const vil_image_view<bool>& active_external);
	void set_speed(const vil_image_view<double>& speed);
	void set_time_step(double t);
	void set_stop_threshold(double t);
	bool update(int max_iter);
	bool update();
	vil_image_view<double>& get_phi();
	int iteration();

	virtual ~dbdet_levelset2d_segmentation(){}

protected:
	bool use_reaction_flux_;
	bool use_diffusion_flux_;
	bool use_external_force_;

	double hx_;
	double hy_;
	double time_step_;
	vil_image_view<double> phi_;
	vil_image_view<double> phi_x_;
	vil_image_view<double> phi_y_;
	vil_image_view<bool> active_reaction_;
	vil_image_view<bool> active_diffusion_;
	vil_image_view<bool> active_external_;
	vil_image_view<double> speed_;
	vil_image_view<double> diffusion_flux_;
	vil_image_view<double> reaction_flux_;
	vil_image_view<double> external_flux_;
	vil_image_view<double> Fx_;
	vil_image_view<double> Fy_;

	int physical_width_;
	int physical_height_;

	int iteration_;
	int reinit_freq_;

	double stop_thresh_;

	double beta0_;
	double beta1_;
	double alpha_;

	void compute_diffusion_flux_();
	void compute_reaction_flux_();
	void compute_external_flux_();
	void compute_grad_phi_central_();
	bool stop_(vil_image_view<double>& phi_old);
	void add_to_phi_(double coeff, bool use_speed, vil_image_view<bool>& active_region, vil_image_view<double>& values);
	bool dbdet_levelset2d_utils_compute_rflux_pos_(vil_image_view<double>& phi_x_minus, vil_image_view<double>& phi_x_plus, vil_image_view<double>& rflux);
	bool dbdet_levelset2d_utils_compute_rflux_neg_(vil_image_view<double>& phi_x_minus, vil_image_view<double>& phi_x_plus, vil_image_view<double>& rflux);
	void make_border_zero_(vil_image_view<double>& data);
};

// utils
// dir = 0 (x), dir = 1 (y), type = 0 (forward), type = 1 (backward), type = 2 (center)
bool dbdet_levelset2d_utils_compute_difference(vil_image_view<double>& input, vil_image_view<double>& output, unsigned dir, unsigned type, double h);

#endif /* DBDET_LEVELSET2D_SEGMENTATION_H_ */
