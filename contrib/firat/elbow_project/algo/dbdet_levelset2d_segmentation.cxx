/*
 * dbdet_levelset2d_segmentation.cxx
 *
 *  Created on: Feb 8, 2012
 *      Author: firat
 */

#include "dbdet_levelset2d_segmentation.h"
#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vil/vil_math.h>
#include <vil/algo/vil_convolve_1d.h>
#include <vil/vil_transpose.h>
#include <vil/vil_fill.h>

dbdet_levelset2d_segmentation::dbdet_levelset2d_segmentation()
{
	use_reaction_flux_ = false;
	use_diffusion_flux_ = false;
	use_external_force_ = false;
	hx_ = 1;
	hy_ = 1;
	time_step_ = 1;
	physical_height_ = 0;
	physical_width_ = 0;
	reinit_freq_ = 1;
	iteration_ = 0;
}

void dbdet_levelset2d_segmentation::initialize(const vil_image_view<double>& init, double hx, double hy)
{
	phi_ = init;
	hx_ = hx;
	hy_ = hy;
	physical_height_ = init.nj();
	physical_width_ = init.ni();
}

void dbdet_levelset2d_segmentation::use_reaction_flux(bool flag)
{
	use_reaction_flux_ = flag;
}

void dbdet_levelset2d_segmentation::use_diffusion_flux(bool flag)
{
	use_diffusion_flux_ = flag;
}

void dbdet_levelset2d_segmentation::use_external_force(bool flag)
{
	use_external_force_ = flag;
}



void dbdet_levelset2d_segmentation::set_external_force_field(const vil_image_view<double>& Fx, const vil_image_view<double>& Fy)
{
	assert(Fx.nj() == physical_height_ && Fx.ni() == physical_width_ &&
			Fy.nj() == physical_height_ && Fy.ni() == physical_width_);
	Fx_ = Fx;
	Fy_ = Fy;
}

void dbdet_levelset2d_segmentation::set_params(double beta0, double beta1, double alpha)
{
	beta0_ = beta0;
	beta1_ = beta1;
	alpha_ = alpha;
}

void dbdet_levelset2d_segmentation::set_active_regions(const vil_image_view<bool>& active_reaction, const vil_image_view<bool>& active_diffusion, const vil_image_view<bool>& active_external)
{
	assert(active_reaction.nj() == physical_height_ && active_reaction.ni() == physical_width_ &&
			active_diffusion.nj() == physical_height_ && active_diffusion.ni() == physical_width_ &&
			active_external.nj() == physical_height_ && active_external.ni() == physical_width_);
	active_diffusion_ = active_diffusion;
	active_reaction_ = active_reaction;
	active_external_ = active_external;
}

void dbdet_levelset2d_segmentation::set_speed(const vil_image_view<double>& speed)
{
	assert(speed.nj() == physical_height_ && speed.ni() == physical_width_);
	speed_ = speed;
}

void dbdet_levelset2d_segmentation::set_time_step(double t)
{
	time_step_ = t;
}

void dbdet_levelset2d_segmentation::set_stop_threshold(double t)
{
	stop_thresh_ = t;
}

void dbdet_levelset2d_segmentation::set_reinitialization_frequency(int k)
{
	reinit_freq_ = k;
}

int dbdet_levelset2d_segmentation::iteration()
{
	return iteration_;
}

bool dbdet_levelset2d_segmentation::update(int max_iter)
{
	for(int i = 0; i < max_iter; i++)
	{
		bool flag = this->update();
		if(flag)
		{
			return true;
		}
	}
	return false;
}

bool dbdet_levelset2d_segmentation::update()
{
	vil_image_view<double> phi_old;
	phi_old.deep_copy(phi_);
	if(use_reaction_flux_)
	{
		compute_reaction_flux_();
		add_to_phi_(time_step_ * beta0_, true, active_reaction_, reaction_flux_);
	}
	if(use_diffusion_flux_)
	{
		compute_grad_phi_central_();
		compute_diffusion_flux_();
		add_to_phi_(time_step_ * -beta1_, true, active_diffusion_, diffusion_flux_);
	}
	if(use_external_force_)
	{
		if(!use_diffusion_flux_)
		{
			compute_grad_phi_central_();
		}
		compute_external_flux_();
		add_to_phi_(time_step_ * -alpha_, false, active_external_, external_flux_);
	}
	iteration_++;
	if(iteration_ % reinit_freq_ == 0)
	{
		reinitialize();
	}
	return stop_(phi_old);
}

void dbdet_levelset2d_segmentation::compute_grad_phi_central_()
{
	dbdet_levelset2d_utils_compute_difference(phi_, phi_x_, 0, 2, hx_);
	dbdet_levelset2d_utils_compute_difference(phi_, phi_y_, 1, 2, hy_);
}

void dbdet_levelset2d_segmentation::compute_diffusion_flux_()
{
	//implement if required
	return;
}

void dbdet_levelset2d_segmentation::compute_reaction_flux_()
{
	vil_image_view<double> phi_x_minus, phi_x_plus, phi_y_minus, phi_y_plus;
	dbdet_levelset2d_utils_compute_difference(phi_, phi_x_minus, 0, 1, hx_);
	dbdet_levelset2d_utils_compute_difference(phi_, phi_x_plus, 0, 0, hx_);
	dbdet_levelset2d_utils_compute_difference(phi_, phi_y_minus, 1, 1, hy_);
	dbdet_levelset2d_utils_compute_difference(phi_, phi_y_plus, 1, 0, hy_);
	vil_image_view<double> xflux, yflux;
	if(beta0_ > 0)
	{
		dbdet_levelset2d_utils_compute_rflux_pos_(phi_x_minus, phi_x_plus, xflux);
		dbdet_levelset2d_utils_compute_rflux_pos_(phi_y_minus, phi_y_plus, yflux);
	}
	else
	{
		dbdet_levelset2d_utils_compute_rflux_neg_(phi_x_minus, phi_x_plus, xflux);
		dbdet_levelset2d_utils_compute_rflux_neg_(phi_y_minus, phi_y_plus, yflux);
	}
	vil_math_image_vector_mag(xflux, yflux, reaction_flux_);
	make_border_zero_(reaction_flux_);
}

void dbdet_levelset2d_segmentation::compute_external_flux_()
{
	vil_image_view<double> flux_x, flux_y;
	vil_math_image_product(phi_x_, Fx_, flux_x);
	vil_math_image_product(phi_y_, Fy_, flux_y);
	vil_math_image_sum(flux_x, flux_y, external_flux_);
}

bool dbdet_levelset2d_segmentation::stop_(vil_image_view<double>& phi_old)
{
	double max = -1;
	for(int j = 0; j < physical_height_; j++)
	{
		for(int i = 0; i < physical_width_; i++)
		{
			double v = vcl_pow(phi_(i,j) - phi_old(i,j), 2);
			if(max < v)
			{
				max = v;
			}
		}
	}
	return max < stop_thresh_;
}

void dbdet_levelset2d_segmentation::add_to_phi_(double coeff, bool use_speed, vil_image_view<bool>& active_region, vil_image_view<double>& values)
{
	vil_image_view<double> values2;
	vil_math_image_product(values, active_region, values2);
	vil_math_scale_values(values2, coeff);
	if(use_speed)
	{
		vil_math_image_product(values2, speed_, values2);
	}
	vil_math_image_sum(phi_, values2, phi_);
}

vil_image_view<double>& dbdet_levelset2d_segmentation::get_phi()
{
	return phi_;
}

bool dbdet_levelset2d_segmentation::dbdet_levelset2d_utils_compute_rflux_pos_(vil_image_view<double>& phi_x_minus, vil_image_view<double>& phi_x_plus, vil_image_view<double>& rflux)
{
	for(int j = 0; j < physical_height_; j++)
	{
		for(int i = 0; i < physical_width_; i++)
		{
			if(phi_x_minus(i,j) > 0)
			{
				phi_x_minus(i,j) = 0;
			}

			if(phi_x_plus(i,j) < 0)
			{
				phi_x_plus(i,j) = 0;
			}

			if(phi_x_minus(i,j) != 0 && phi_x_plus(i,j) != 0) //degenerate case
			{
				double val = vcl_fabs(phi_x_minus(i,j)) + vcl_fabs(phi_x_plus(i,j));
				phi_x_minus(i,j) = val;
				phi_x_plus(i,j) = 0.5;
			}
		}
	}
	vil_math_image_vector_mag(phi_x_minus, phi_x_plus, rflux);
	return true;
}

bool dbdet_levelset2d_segmentation::dbdet_levelset2d_utils_compute_rflux_neg_(vil_image_view<double>& phi_x_minus, vil_image_view<double>& phi_x_plus, vil_image_view<double>& rflux)
{
	for(int j = 0; j < physical_height_; j++)
	{
		for(int i = 0; i < physical_width_; i++)
		{
			if(phi_x_minus(i,j) < 0)
			{
				phi_x_minus(i,j) = 0;
			}

			if(phi_x_plus(i,j) > 0)
			{
				phi_x_plus(i,j) = 0;
			}

			if(phi_x_minus(i,j) == 0 && phi_x_plus(i,j) == 0) //degenerate case
			{
				double val = vcl_fabs(phi_x_minus(i,j)) + vcl_fabs(phi_x_plus(i,j));
				phi_x_minus(i,j) = val;
				phi_x_plus(i,j) = 0.5;
			}
		}
	}
	vil_math_image_vector_mag(phi_x_minus, phi_x_plus, rflux);
	return true;
}

void dbdet_levelset2d_segmentation::make_border_zero_(vil_image_view<double>& data)
{
	vil_fill_row(data, 0, 0.0);
	vil_fill_row(data, physical_height_-1, 0.0);
	vil_fill_col(data, 0, 0.0);
	vil_fill_col(data, physical_width_-1, 0.0);
}

// utils
// dir = 0 (x), dir = 1 (y), type = 0 (forward), type = 1 (backward), type = 2 (center)
bool dbdet_levelset2d_utils_compute_difference(vil_image_view<double>& input, vil_image_view<double>& output, unsigned dir, unsigned type, double h)
{
	vil_image_view<double> input2;
	if(dir == 0)
	{
		input2 = input;
	}
	else if(dir == 1)
	{
		input2 = vil_transpose(input);
	}
	else
	{
		return false;
	}
	double filter_fwd[3] = {1, -1, 0};
	double filter_bck[3] = {0, 1, -1};
	double filter_center[3] = {1, 0 ,-1};
	double* filter;
	double scale = 1/h;
	switch(type)
	{
	case 0: // forward
		filter = filter_fwd;
		break;
	case 1: // backward
		filter = filter_bck;
		break;
	case 2: // center
		filter = filter_center;
		scale /= 2;
		break;
	default:
		return false;
	}
	vil_image_view<double> temp_output;
	vil_convolve_1d(input2, temp_output, filter+1, -1, 1, double(),
			vil_convolve_zero_extend, vil_convolve_zero_extend);
	if(dir == 0)
	{
		output = temp_output;
	}
	else if(dir == 1)
	{
		output = vil_transpose(temp_output);
	}
	vil_math_scale_values(output, scale);
	return true;
}


