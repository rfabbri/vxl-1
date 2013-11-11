/*
 * test_levelset2d.cxx
 *
 *  Created on: Feb 22, 2012
 *      Author: firat
 */

#include "../algo/dbdet_graddt_levelset2d_segmentation.h"
#include "../algo/nn_subpixel_distance_transform.h"
#include <vil/vil_math.h>
#include <vil/algo/vil_convolve_1d.h>

int main()
{
	dbdet_graddt_levelset2d_segmentation seg;
	vcl_vector<dbdet_3d_edge_sptr> edges;
	dbdet_3d_edge_sptr edg = new dbdet_3d_edge;
	int H = 11;
	int W = 11;
	edg->x = 5;
	edg->y = 5;
	edg->z = 0;
	edges.push_back(edg);
	vil_image_view<double> DT;
	nn_subpixel_distance_transform(edges, DT, W, H, 0, 1, 1, 1);
	vil_math_scale_and_offset_values(DT, 1, -3);
	for(int j = 0; j < H; j++)
	{
		for(int i = 0; i < W; i++)
		{
			vcl_cout << DT(i,j) << " ";
		}
		vcl_cout << vcl_endl;
	}
	seg.initialize(DT, 1, 1);
	seg.set_reinitialization_frequency(1);
	seg.set_narrowband(2);
	seg.use_reaction_flux(true);
	vil_image_view<bool> active(W,H);
	vil_image_view<bool> passive(W,H);
	passive.fill(false);
	active.fill(true);
	seg.set_active_regions(active, passive, passive);
	seg.set_params(-1, 0, 0);
	vil_image_view<double> speed(W,H);
	speed.fill(1);
	seg.set_speed(speed);
	seg.set_stop_threshold(0.005);
	seg.update(10);
	vcl_cout << vcl_endl;
	for(int j = 0; j < H; j++)
	{
		for(int i = 0; i < W; i++)
		{
			vcl_cout << seg.get_phi()(i,j) << " ";
		}
		vcl_cout << vcl_endl;
	}

	/*vcl_cout << vcl_endl;
	vcl_cout << vcl_endl;
	vil_image_view<double> A(3,3);
	vil_image_view<double> B,C,D,E;
	double F1[2] = {1, -1};
	double F2[3] = {1, 0, -1};
	A(0,0) = 3; A(0,1) = 5; A(0,2) = 1;
	A(1,0) = 8; A(1,1) = 6; A(1,2) = 7;
	A(2,0) = -1; A(2,1) = 10; A(2,2) = 9;

	vil_convolve_1d(A, B, F1+1,-1,0,double(),
			vil_convolve_zero_extend, vil_convolve_zero_extend); // forward, discard last column

	vil_convolve_1d(A, C, F1,0,1,double(),
				vil_convolve_zero_extend, vil_convolve_zero_extend); // backward, discard first column

	vil_convolve_1d(A, D, F2+1,-1,1,double(),
					vil_convolve_zero_extend, vil_convolve_zero_extend); // center, discard first and last columns


	for(int j = 0; j < B.nj(); j++)
	{
		for(int i = 0; i < B.ni(); i++)
		{
			vcl_cout << B(i,j) << " ";
		}
		vcl_cout << vcl_endl;
	}
	vcl_cout << vcl_endl;
	vcl_cout << vcl_endl;
	for(int j = 0; j < C.nj(); j++)
		{
			for(int i = 0; i < C.ni(); i++)
			{
				vcl_cout << C(i,j) << " ";
			}
			vcl_cout << vcl_endl;
		}
	vcl_cout << vcl_endl;
		vcl_cout << vcl_endl;
		for(int j = 0; j < D.nj(); j++)
			{
				for(int i = 0; i < D.ni(); i++)
				{
					vcl_cout << D(i,j) << " ";
				}
				vcl_cout << vcl_endl;
			}*/
	return 0;
}



