// This is /lemsvxl/contrib/firat/dbcvr_2d_shape_descriptors/dbcvr_2d_shape_descriptors.cxx.

// \file
// \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
// \date May 20, 2011

#include "dbcvr_2d_shape_descriptors.h"
#include <vsol/vsol_digital_curve_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vcl_cmath.h>
#include <vnl/vnl_math.h>

bool dbcvr_diff_chord_angles(vsol_digital_curve_2d_sptr curve, bool is_closed, vcl_vector<unsigned>& indices,
		vnl_matrix<double>& alpha_prime)
{
	int M = curve->size();
	int num_rows;
	if(is_closed)
	{
		num_rows = M;
		alpha_prime.set_size(M,M);
		for(int i = 0; i < M; i++)
		{
			indices.push_back(i);
		}
	}
	else
	{
		if(indices.empty())
		{
			indices.push_back(0); indices.push_back(1);
		}
		num_rows = indices.size();
		alpha_prime.set_size(num_rows, M);
	}
	double* alpha = new double[M];
	for(int j = 0; j < num_rows; j++)
	{
		unsigned i = indices[j];
		vsol_point_2d_sptr p_i = curve->point(i);
		double x_i = p_i->x();
		double y_i = p_i->y();

		for(int k = 0; k < M; k++)
		{
			vsol_point_2d_sptr p_k = curve->point(k);
			double dx = p_k->x() - x_i;
			double dy = p_k->y() - y_i;
			double angle = vcl_atan2(dy, dx);
			if(angle < 0)
			{
				angle += vnl_math::pi*2;
			}
			alpha[k] = angle;
		}
		for(int k = 1; k < M-1; k++)
		{
			alpha_prime(i,k) = alpha[k+1] - alpha[k-1];
		}
		if(is_closed)
		{
			alpha_prime(i,0) = alpha[1] - alpha[M-1];
			alpha_prime(i,M-1) = alpha[0] - alpha[M-2];
		}
		else
		{
			alpha_prime(i,0) = 0;
			alpha_prime(i,M-1) = 0;
		}
	}
	delete[] alpha;
	return true;
}
