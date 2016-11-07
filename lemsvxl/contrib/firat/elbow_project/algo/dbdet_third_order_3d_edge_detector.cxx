/*
 * dbdet_third_order_3d_edge_detector.cxx
 *
 *  Created on: Oct 11, 2011
 *      Author: firat
 */

#include "dbdet_third_order_3d_edge_detector.h"
#include <vcl_cmath.h>
#include <vil3d/vil3d_trilin_interp.h>
#include <vil3d/algo/vil3d_convolve_1d.h>
#include <vil3d/vil3d_resample_trilinear.h>
#include <vil3d/vil3d_switch_axes.h>
#include <vcl_cstdlib.h>
#include <vcl_algorithm.h>
#include <vil3d/vil3d_crop.h>

dbdet_1d_gaussian_derivative_filters::dbdet_1d_gaussian_derivative_filters(double sigma, double h)
{
	int M = get_filter_size(sigma);
	int actual_filter_size = (M-1)/h+1;
	int L = (actual_filter_size-1)/2;
	double* X = new double[actual_filter_size];
	for(int i = -L; i<=L; i++)
	{
		X[i+L] = i*h;
	}
	double sigma2 = sigma*sigma;
	double sigma4 = sigma2*sigma2;
	double sigma6 = sigma2*sigma4;
	G0 = new double[actual_filter_size];
	G1 = new double[actual_filter_size];
	G2 = new double[actual_filter_size];
	G3 = new double[actual_filter_size];
	double sum_G0 = 0;
	for(int i = 0; i<actual_filter_size; i++)
	{
		G0[i] = vcl_exp(-X[i]*X[i]/2/sigma2);
		sum_G0 += G0[i];
	}
	for(int i = 0; i<actual_filter_size; i++)
	{
		G0[i] = G0[i] / sum_G0 / h;
	}
	for(int i = 0; i<actual_filter_size; i++)
	{
		G1[i] = (-X[i]/sigma2)*G0[i];
		G2[i] = ((X[i]*X[i]/sigma4)-(1/sigma2)) * G0[i];
		G3[i] = ((3*X[i]/sigma4) - (X[i]*X[i]*X[i]/sigma6)) * G0[i];
	}
	delete[] X;
	this->filter_size = actual_filter_size;
}

int dbdet_1d_gaussian_derivative_filters::get_filter_size(double sigma)
{
	int M = sigma*10;
	if(M%2 == 0)
	{
		M++;
	}
	return M;
}

dbdet_1d_gaussian_derivative_filters::~dbdet_1d_gaussian_derivative_filters()
{
	delete[] G0;
	delete[] G1;
	delete[] G2;
	delete[] G3;
}

bool dbdet_separable_convolve3d(const vil3d_image_view<double>& V, vil3d_image_view<double>& VF, int L, double* F1, double* F2, double* F3)
{
	vil3d_image_view<double> temp1, temp2;
	vil3d_convolve_1d(V, temp1, F1+L,-L,L,double(),
			vil_convolve_ignore_edge, vil_convolve_ignore_edge);
	vil3d_convolve_1d(vil3d_switch_axes_jki(temp1), temp2, F2+L,-L,L,double(),
			vil_convolve_ignore_edge, vil_convolve_ignore_edge);
	vil3d_convolve_1d(vil3d_switch_axes_jki(temp2), temp1, F3+L,-L,L,double(),
			vil_convolve_ignore_edge, vil_convolve_ignore_edge);
	VF = vil3d_crop(vil3d_switch_axes_jki(temp1), L, V.ni()-2*L, L, V.nj()-2*L, L, V.nk()-2*L);

	return true;
}

bool dbdet_compute_gradF_gradI(const vil3d_image_view<double>&W, vil3d_image_view<double>& Fx, vil3d_image_view<double>& Fy,
		vil3d_image_view<double>& Fz, vil3d_image_view<double>& Ix, vil3d_image_view<double>& Iy,
		vil3d_image_view<double>& Iz, double sigma, double h)
{
	dbdet_1d_gaussian_derivative_filters filters(sigma, h);
	vil3d_image_view<double> V;
	int n1 = (W.ni()-1)/h+1;
	int n2 = (W.nj()-1)/h+1;
	int n3 = (W.nk()-1)/h+1;
	// vcl_cout << "Start: resample image..." << vcl_endl;
	//	vil3d_resample_trilinear_edge_extend(W, V, 0, 0, 0,
	//			h, 0, 0,
	//			0, h, 0,
	//			0, 0, h,
	//			n1,n2,n3);
	vil3d_resample_trilinear(W, V, n1, n2, n3);
	double h3 = h*h*h;
	for(int i = 0; i < n1; i++)
	{
		for(int j = 0; j < n2; j++)
		{
			for(int k = 0; k < n3; k++)
			{
				V(i,j,k) = V(i,j,k)*h3;
			}
		}
	}
	// vcl_cout << "Finish: resample image..." << vcl_endl;
	int L = (filters.filter_size-1)/2;
	vil3d_image_view<double> Ixx, Iyy, Izz, Ixy, Ixz, Iyz, Ixxx,
	Iyyy, Izzz, Ixxy, Ixxz, Ixyy, Iyyz, Ixzz, Iyzz, Ixyz;
	// vcl_cout << "Start: convolutions" << vcl_endl;
	// vcl_cout << "Ix" << vcl_endl;
	dbdet_separable_convolve3d(V, Ix, L, filters.G1, filters.G0, filters.G0);
	// vcl_cout << "Iy" << vcl_endl;
	dbdet_separable_convolve3d(V, Iy, L, filters.G0, filters.G1, filters.G0);
	// vcl_cout << "Iz" << vcl_endl;
	dbdet_separable_convolve3d(V, Iz, L, filters.G0, filters.G0, filters.G1);
	// vcl_cout << "Ixx" << vcl_endl;
	dbdet_separable_convolve3d(V, Ixx, L, filters.G2, filters.G0, filters.G0);
	// vcl_cout << "Iyy" << vcl_endl;
	dbdet_separable_convolve3d(V, Iyy, L, filters.G0, filters.G2, filters.G0);
	// vcl_cout << "Izz" << vcl_endl;
	dbdet_separable_convolve3d(V, Izz, L, filters.G0, filters.G0, filters.G2);
	// vcl_cout << "Ixy" << vcl_endl;
	dbdet_separable_convolve3d(V, Ixy, L, filters.G1, filters.G1, filters.G0);
	// vcl_cout << "Ixz" << vcl_endl;
	dbdet_separable_convolve3d(V, Ixz, L, filters.G1, filters.G0, filters.G1);
	// vcl_cout << "Iyz" << vcl_endl;
	dbdet_separable_convolve3d(V, Iyz, L, filters.G0, filters.G1, filters.G1);
	// vcl_cout << "Ixxx" << vcl_endl;
	dbdet_separable_convolve3d(V, Ixxx, L, filters.G3, filters.G0, filters.G0);
	// vcl_cout << "Iyyy" << vcl_endl;
	dbdet_separable_convolve3d(V, Iyyy, L, filters.G0, filters.G3, filters.G0);
	// vcl_cout << "Izzz" << vcl_endl;
	dbdet_separable_convolve3d(V, Izzz, L, filters.G0, filters.G0, filters.G3);
	// vcl_cout << "Ixxy" << vcl_endl;
	dbdet_separable_convolve3d(V, Ixxy, L, filters.G2, filters.G1, filters.G0);
	// vcl_cout << "Ixxz" << vcl_endl;
	dbdet_separable_convolve3d(V, Ixxz, L, filters.G2, filters.G0, filters.G1);
	// vcl_cout << "Ixyy" << vcl_endl;
	dbdet_separable_convolve3d(V, Ixyy, L, filters.G1, filters.G2, filters.G0);
	// vcl_cout << "Iyyz" << vcl_endl;
	dbdet_separable_convolve3d(V, Iyyz, L, filters.G0, filters.G2, filters.G1);
	// vcl_cout << "Ixzz" << vcl_endl;
	dbdet_separable_convolve3d(V, Ixzz, L, filters.G1, filters.G0, filters.G2);
	// vcl_cout << "Iyzz" << vcl_endl;
	dbdet_separable_convolve3d(V, Iyzz, L, filters.G0, filters.G1, filters.G2);
	// vcl_cout << "Ixyz" << vcl_endl;
	dbdet_separable_convolve3d(V, Ixyz, L, filters.G1, filters.G1, filters.G1);
	// vcl_cout << "Finish: convolutions" << vcl_endl;
	n1-= 2*L;
	n2-= 2*L;
	n3-= 2*L;
	Fx.set_size(n1,n2,n3);
	Fy.set_size(n1,n2,n3);
	Fz.set_size(n1,n2,n3);
	// vcl_cout << "Start: compute gradF..." << vcl_endl;
	for(int i = 0; i < n1; i++)
	{
		for(int j = 0; j < n2; j++)
		{
			for(int k = 0; k < n3; k++)
			{
				Fx(i,j,k) = 2*Ix(i,j,k)*Ixx(i,j,k)*Ixx(i,j,k) + Ix(i,j,k)*Ix(i,j,k)*Ixxx(i,j,k)
									+ 2*Iy(i,j,k)*Ixy(i,j,k)*Iyy(i,j,k) + Iy(i,j,k)*Iy(i,j,k)*Ixyy(i,j,k)
									+ 2*Iz(i,j,k)*Ixz(i,j,k)*Izz(i,j,k) + Iz(i,j,k)*Iz(i,j,k)*Ixzz(i,j,k)
									+ 2*Ixx(i,j,k)*Iy(i,j,k)*Ixy(i,j,k) + 2*Ix(i,j,k)*Ixy(i,j,k)*Ixy(i,j,k)
									+ 2*Ix(i,j,k)*Iy(i,j,k)*Ixxy(i,j,k) + 2*Ixx(i,j,k)*Iz(i,j,k)*Ixz(i,j,k)
									+ 2*Ix(i,j,k)*Ixz(i,j,k)*Ixz(i,j,k) + 2*Ix(i,j,k)*Iz(i,j,k)*Ixxz(i,j,k)
									+ 2*Ixy(i,j,k)*Iz(i,j,k)*Iyz(i,j,k) + 2*Iy(i,j,k)*Ixz(i,j,k)*Iyz(i,j,k)
									+ 2*Iy(i,j,k)*Iz(i,j,k)*Ixyz(i,j,k);

				Fy(i,j,k) = 2*Iy(i,j,k)*Iyy(i,j,k)*Iyy(i,j,k) + Iy(i,j,k)*Iy(i,j,k)*Iyyy(i,j,k)
									+ 2*Ix(i,j,k)*Ixy(i,j,k)*Ixx(i,j,k) + Ix(i,j,k)*Ix(i,j,k)*Ixxy(i,j,k)
									+ 2*Iz(i,j,k)*Iyz(i,j,k)*Izz(i,j,k) + Iz(i,j,k)*Iz(i,j,k)*Iyzz(i,j,k)
									+ 2*Iyy(i,j,k)*Ix(i,j,k)*Ixy(i,j,k) + 2*Iy(i,j,k)*Ixy(i,j,k)*Ixy(i,j,k)
									+ 2*Ix(i,j,k)*Iy(i,j,k)*Ixyy(i,j,k) + 2*Iyy(i,j,k)*Iz(i,j,k)*Iyz(i,j,k)
									+ 2*Iy(i,j,k)*Iyz(i,j,k)*Iyz(i,j,k) + 2*Iy(i,j,k)*Iz(i,j,k)*Iyyz(i,j,k)
									+ 2*Ixy(i,j,k)*Iz(i,j,k)*Ixz(i,j,k) + 2*Ix(i,j,k)*Ixz(i,j,k)*Iyz(i,j,k)
									+ 2*Ix(i,j,k)*Iz(i,j,k)*Ixyz(i,j,k);

				Fz(i,j,k) = 2*Iz(i,j,k)*Izz(i,j,k)*Izz(i,j,k) + Iz(i,j,k)*Iz(i,j,k)*Izzz(i,j,k)
									+ 2*Iy(i,j,k)*Iyz(i,j,k)*Iyy(i,j,k) + Iy(i,j,k)*Iy(i,j,k)*Iyyz(i,j,k)
									+ 2*Ix(i,j,k)*Ixz(i,j,k)*Ixx(i,j,k) + Ix(i,j,k)*Ix(i,j,k)*Ixxz(i,j,k)
									+ 2*Izz(i,j,k)*Iy(i,j,k)*Iyz(i,j,k) + 2*Iz(i,j,k)*Iyz(i,j,k)*Iyz(i,j,k)
									+ 2*Iz(i,j,k)*Iy(i,j,k)*Iyzz(i,j,k) + 2*Izz(i,j,k)*Ix(i,j,k)*Ixz(i,j,k)
									+ 2*Iz(i,j,k)*Ixz(i,j,k)*Ixz(i,j,k) + 2*Ix(i,j,k)*Iz(i,j,k)*Ixzz(i,j,k)
									+ 2*Iyz(i,j,k)*Ix(i,j,k)*Ixy(i,j,k) + 2*Iy(i,j,k)*Ixz(i,j,k)*Ixy(i,j,k)
									+ 2*Iy(i,j,k)*Ix(i,j,k)*Ixyz(i,j,k);
			}
		}
	}
	// vcl_cout << "Finish: compute gradF..." << vcl_endl;

	return true;
}

bool dbdet_nms_3d(const vil3d_image_view<double>&Fx, const vil3d_image_view<double>&Fy, const vil3d_image_view<double>&Fz,
		const vil3d_image_view<double>& mag_gradI, vcl_vector<dbdet_3d_edge_sptr>& partial_edgemap, double strength_threshold)
{
	int ni = Fx.ni(), nj = Fx.nj(), nk = Fx.nk();
	vcl_ptrdiff_t istep = mag_gradI.istep();
	vcl_ptrdiff_t jstep = mag_gradI.jstep();
	vcl_ptrdiff_t kstep = mag_gradI.kstep();
	for(int i = 2; i <= ni-3; i++)
	{
		for(int j = 2; j <= nj-3; j++)
		{
			for(int k = 2; k <= nk-3; k++)
			{
				double f = mag_gradI(i,j,k);
				if(f >= strength_threshold)
				{
					double gx = Fx(i,j,k);
					double gy = Fy(i,j,k);
					double gz = Fz(i,j,k);
					if(vcl_abs(gx) > 10e-6 || vcl_abs(gy) > 10e-6 || vcl_abs(gz) > 10e-6)
					{
						double mag_N = vcl_sqrt(gx*gx + gy*gy + gz*gz);
						double Nx = gx/mag_N;
						double Ny = gy/mag_N;
						double Nz = gz/mag_N;
						double s = 1/vcl_max(vcl_abs(Nx), vcl_max(vcl_abs(Ny), vcl_abs(Nz)));
						double pp_x = i + s*Nx;
						double pp_y = j + s*Ny;
						double pp_z = k + s*Nz;
						double pm_x = i - s*Nx;
						double pm_y = j - s*Ny;
						double pm_z = k - s*Nz;
						double fp = vil3d_trilin_interp_raw(pp_x, pp_y, pp_z, mag_gradI.origin_ptr(), istep, jstep, kstep);
						double fm = vil3d_trilin_interp_raw(pm_x, pm_y, pm_z, mag_gradI.origin_ptr(), istep, jstep, kstep);
						if((f >  fm && f >  fp) || (f >  fm && f >= fp) || (f >= fm && f >  fp))
						{
							double A = (fm+fp-2*f)/(2*s*s);
							double B = (fp-fm)/(2*s);
							double C = f;
							double s_star = -B/(2*A);
							double max_f = A*s_star*s_star + B*s_star + C;
							if(vcl_abs(s_star) <= s)
							{
								dbdet_3d_edge_sptr edg = new dbdet_3d_edge;
								edg->x = i + s_star*Nx;
								edg->y = j + s_star*Ny;
								edg->z = k + s_star*Nz;
								edg->strength = max_f;
								partial_edgemap.push_back(edg);
							}
						}
					}
				}
			}
		}
	}
	return true;
}

bool vil3d_my_crop(const vil3d_image_view<double>& W, vil3d_image_view<double>& V, int i0, int j0, int k0, int ni, int nj, int nk)
{
	int wni = W.ni(), wnj = W.nj(), wnk = W.nk();
	V.set_size(ni, nj, nk);
	for(int i = 0; i < ni; i++)
	{
		for(int j = 0; j < nj; j++)
		{
			for(int k = 0; k < nk; k++)
			{
				int ii = i + i0;
				int jj = j + j0;
				int kk = k + k0;
				if(ii < 0)
				{
					ii = 0;
				}
				else if(ii > wni - 1)
				{
					ii = wni - 1;
				}

				if(jj < 0)
				{
					jj = 0;
				}
				else if(jj > wnj - 1)
				{
					jj = wnj - 1;
				}

				if(kk < 0)
				{
					kk = 0;
				}
				else if(kk > wnk - 1)
				{
					kk = wnk - 1;
				}

				V(i,j,k) = W(ii,jj,kk);
			}
		}
	}
	return true;
}

bool dbdet_third_order_3d_edge_detector_roi(const vil3d_image_view<double>& W, vcl_vector<dbdet_3d_edge_sptr>& edgemap,
		double strength_threshold, double sigma, double h, int i0, int j0, int k0, int ni_roi,
		int nj_roi, int nk_roi)
{
	int M = dbdet_1d_gaussian_derivative_filters::get_filter_size(sigma); // filter size when h = 1
	int L = (M-1)/2; // padding due to filtering when h = 1
	int P = 2; // additional padding
	int new_i0 = i0 - L - P; // effective i0
	int new_j0 = j0 - L - P; // effective j0
	int new_k0 = k0 - L - P; // effective k0
	int new_ni = ni_roi + M + 2*P; // effective roi size ni
	int new_nj = nj_roi + M + 2*P; // effective roi size nj
	int new_nk = nk_roi + M + 2*P; // effective roi size nk
	vil3d_image_view<double> V; // volume of interest
	vil3d_my_crop(W, V, new_i0, new_j0, new_k0, new_ni, new_nj, new_nk); // crop original volume
	vil3d_image_view<double> Fx, Fy, Fz, Ix, Iy, Iz, mag_gradI;
	// vcl_cout << "Start: Compute gradF and gradI..." << vcl_endl;
	dbdet_compute_gradF_gradI(V, Fx, Fy, Fz, Ix, Iy, Iz, sigma, h);
	// vcl_cout << "Finish: Compute gradF and gradI..." << vcl_endl;
	int ni = Ix.ni(), nj = Ix.nj(), nk = Ix.nk();
	// vcl_cout << "Start: Compute mag_gradI..." << vcl_endl;
	mag_gradI.set_size(ni, nj, nk);
	for(int i = 0; i < ni; i++)
	{
		for(int j = 0; j < nj; j++)
		{
			for(int k = 0; k < nk; k++)
			{
				mag_gradI(i,j,k) = vcl_sqrt(Ix(i,j,k)*Ix(i,j,k) + Iy(i,j,k)*Iy(i,j,k) + Iz(i,j,k)*Iz(i,j,k));

			}
		}
	}
	// vcl_cout << "Finish: Compute mag_gradI..." << vcl_endl;
	// vcl_cout << "Start: NMS..." << vcl_endl;
	dbdet_nms_3d(Fx, Fy, Fz, mag_gradI, edgemap, strength_threshold);
	// vcl_cout << "Finish: NMS..." << vcl_endl;
	vcl_ptrdiff_t istep = Fx.istep();
	vcl_ptrdiff_t jstep = Fx.jstep();
	vcl_ptrdiff_t kstep = Fx.kstep();
	int num_edges = edgemap.size();
	// vcl_cout << "Start: Compute edge normals..." << vcl_endl;
	for(int i = 0; i < num_edges; i++)
	{
		dbdet_3d_edge_sptr edg = edgemap[i];
		edg->nx = vil3d_trilin_interp_raw(edg->x, edg->y, edg->z, Fx.origin_ptr(), istep, jstep, kstep);
		edg->ny = vil3d_trilin_interp_raw(edg->x, edg->y, edg->z, Fy.origin_ptr(), istep, jstep, kstep);
		edg->nz = vil3d_trilin_interp_raw(edg->x, edg->y, edg->z, Fz.origin_ptr(), istep, jstep, kstep);
		double n_mag = vcl_sqrt(edg->nx*edg->nx + edg->ny*edg->ny + edg->nz*edg->nz);
		edg->nx = edg->nx/n_mag;
		edg->ny = edg->ny/n_mag;
		edg->nz = edg->nz/n_mag;
		edg->x = edg->x*h - P;
		edg->y = edg->y*h - P;
		edg->z = edg->z*h - P;
	}
	// vcl_cout << "Finish: Compute edge normals..." << vcl_endl;
	return true;
}

bool dbdet_third_order_3d_edge_detector(const vil3d_image_view<double>& W, vcl_vector<dbdet_3d_edge_sptr>& edgemap,
		double strength_threshold, double sigma, double h)
{
	return dbdet_third_order_3d_edge_detector_roi(W, edgemap, strength_threshold, sigma, h, 0, 0, 0, W.ni(),
			W.nj(), W.nk());
}

bool dbdet_keep_strong_edges(vcl_vector<dbdet_3d_edge_sptr>& edgemap_in,
		vcl_vector<dbdet_3d_edge_sptr>& edgemap_out, double strength_threshold, bool modify_original)
{
	if(modify_original)
	{
		vcl_vector<dbdet_3d_edge_sptr>::iterator it = edgemap_in.begin();
		for(int i = 0; i < edgemap_in.size(); i++)
		{
			if(edgemap_in[i]->strength < strength_threshold)
			{
				edgemap_in.erase(it + i);
				i--;
			}
		}
	}
	else
	{
		for(int i = 0; i < edgemap_in.size(); i++)
		{
			if(edgemap_in[i]->strength >= strength_threshold)
			{
				edgemap_out.push_back(edgemap_in[i]);
			}
		}
	}
}
