// This is /lemsvxl/contrib/firat/dbcvr_2d_shape_descriptors/dbcvr_2d_shape_descriptor_utils.cxx.

// \file
// \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
// \date May 3, 2011

#include "dbcvr_2d_shape_descriptor_utils.h"
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_digital_curve_2d.h>
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>

bool linear_interpolation(double x[], double y[], int orig_size, double xi[], double yi[], int new_size)
{
	int j = 0;
	int count = 0;
	for(int i = 0; i < new_size; i++)	{

		while(j < orig_size-2 && (xi[i] < x[j] || xi[i] >= x[j+1]))
		{
			j++;
		}
		double v = (xi[i] - x[j])/(x[j+1] - x[j])*(y[j+1] - y[j]) + y[j];
		yi[count++] = v;
	}
	return true;
}


vsol_digital_curve_2d_sptr dbcvr_uniform_sampling_open(vsol_digital_curve_2d_sptr curve, int num_points, int iteration)
{
	int curve_size = curve->size();
	double* cum_seg_lengths = new double[curve_size];
	double* x = new double[curve_size];
	double* y = new double[curve_size];
	cum_seg_lengths[0] = 0.0;
	vsol_point_2d_sptr p1;
	for(int i = 1; i < curve_size; i++)
	{
		vsol_point_2d_sptr p0 = curve->point(i-1);
		p1 = curve->point(i);
		x[i-1] = p0->x();
		y[i-1] = p0->y();
		double dx = x[i-1] - p1->x();
		double dy = y[i-1] - p1->y();
		double d = vcl_sqrt(dx*dx + dy*dy);
		cum_seg_lengths[i] = cum_seg_lengths[i-1] + d;
	}
	x[curve_size-1] = p1->x();
    y[curve_size-1] = p1->y();
    double interp_step	= cum_seg_lengths[curve_size-1] / (num_points-1);
	double* s = new double[num_points];
	double* x_interpolated = new double[num_points];
	double* y_interpolated = new double[num_points];
	for(int i=0; i < num_points; i++)
	{
		s[i] = interp_step*i;
	}
	linear_interpolation(cum_seg_lengths, x, curve_size, s, x_interpolated, num_points);
	linear_interpolation(cum_seg_lengths, y, curve_size, s, y_interpolated, num_points);
	vcl_vector<vsol_point_2d_sptr> samples;
	for(int i = 0; i < num_points; i++)
	{
		samples.push_back(new vsol_point_2d(x_interpolated[i], y_interpolated[i]));
	}
	vsol_digital_curve_2d_sptr new_curve = new vsol_digital_curve_2d(samples);
	delete[] cum_seg_lengths;
	delete[] x;
	delete[] y;
	delete[] s;
	delete[] x_interpolated;
	delete[] y_interpolated;
	if(iteration < 50)
	{
		return dbcvr_uniform_sampling_open(new_curve, num_points, ++iteration);
	}
	else
	{
		return new_curve;
	}
}

vsol_digital_curve_2d_sptr dbcvr_uniform_sampling_closed(vsol_digital_curve_2d_sptr curve, int num_points, int iteration)
{
	int curve_size = curve->size();
	double* cum_seg_lengths = new double[curve_size+1];
	double* x = new double[curve_size+1];
	double* y = new double[curve_size+1];
	cum_seg_lengths[0] = 0.0;
	vsol_point_2d_sptr p1;
	for(int i = 1; i < curve_size+1; i++)
	{
		vsol_point_2d_sptr p0 = curve->point(i-1);
		x[i-1] = p0->x();
		y[i-1] = p0->y();
		if(i == curve_size)
		{
			p1 = curve->point(0);
		}
		else
		{
			p1 = curve->point(i);
		}
		double dx = x[i-1] - p1->x();
		double dy = y[i-1] - p1->y();
		double d = vcl_sqrt(dx*dx + dy*dy);
		cum_seg_lengths[i] = cum_seg_lengths[i-1] + d;
	}
	x[curve_size] = p1->x();
    y[curve_size] = p1->y();
    double interp_step	= cum_seg_lengths[curve_size] / num_points;
	double* s = new double[num_points];
	double* x_interpolated = new double[num_points];
	double* y_interpolated = new double[num_points];
	for(int i=0; i < num_points; i++)
	{
		s[i] = interp_step*i;
	}
	linear_interpolation(cum_seg_lengths, x, curve_size+1, s, x_interpolated, num_points);
	linear_interpolation(cum_seg_lengths, y, curve_size+1, s, y_interpolated, num_points);
	vcl_vector<vsol_point_2d_sptr> samples;
	for(int i = 0; i < num_points; i++)
	{
		samples.push_back(new vsol_point_2d(x_interpolated[i], y_interpolated[i]));
	}
	vsol_digital_curve_2d_sptr new_curve = new vsol_digital_curve_2d(samples);
	delete[] cum_seg_lengths;
	delete[] x;
	delete[] y;
	delete[] s;
	delete[] x_interpolated;
	delete[] y_interpolated;
	if(iteration < 50)
	{
		return dbcvr_uniform_sampling_closed(new_curve, num_points, ++iteration);
	}
	else
	{
		return new_curve;
	}
}

vsol_digital_curve_2d_sptr dbcvr_get_reversed_curve(vsol_digital_curve_2d_sptr curve)
{
	vcl_vector<vsol_point_2d_sptr> samples;
	for(int i = curve->size()-1; i >= 0; i--)
	{
		samples.push_back(new vsol_point_2d(*curve->point(i)));
	}
	return new vsol_digital_curve_2d(samples);
}

