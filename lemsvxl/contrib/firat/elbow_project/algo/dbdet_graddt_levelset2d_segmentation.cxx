/*
 * dbdet_graddt_levelset2d_segmentation.cxx
 *
 *  Created on: Feb 22, 2012
 *      Author: firat
 */

#include "dbdet_graddt_levelset2d_segmentation.h"
#include "../algo/nn_subpixel_distance_transform.h"
#include <vil/vil_math.h>

void dbdet_graddt_levelset2d_segmentation::reinitialize()
{
	narrowband_reinitialization_();
	//sussman_reinitialization_();
}

void dbdet_graddt_levelset2d_segmentation::set_narrowband(double band)
{
	band_ = band;
}

void dbdet_graddt_levelset2d_segmentation::set_sussman_reinitilization_threshold(double phi_thresh)
{
	phi_threshold_ = phi_thresh;
}

void dbdet_graddt_levelset2d_segmentation::narrowband_reinitialization_()
{
	vil_math_truncate_range(phi_, -band_, band_);
}

void dbdet_graddt_levelset2d_segmentation::sussman_reinitialization_()
{

}

void dbdet_compute_graddt_force(vcl_vector<dbdet_3d_edge_sptr>& edgemap, vil_image_view<double>& DT,
		vil_image_view<double>& Fx,  vil_image_view<double>& Fy, int slice_index, int width, int height,
		double hx, double hy, double dist_thresh)
{
	nn_subpixel_distance_transform(edgemap, DT, width, height, slice_index, hx, hy, dist_thresh);
	dbdet_levelset2d_utils_compute_difference(DT, Fx, 0, 2, -hx);
	dbdet_levelset2d_utils_compute_difference(DT, Fy, 1, 2, -hy);
}

void dbdet_classify_edges(vil3d_image_view<double>& V,  vcl_vector<dbdet_3d_edge_sptr>& orig_edgemap,
		vcl_vector<dbdet_3d_edge_sptr>& FG_edgemap, vcl_vector<dbdet_3d_edge_sptr>& BG_edgemap, double BG_thresh)
{
	int num_edges = orig_edgemap.size();
	int maxx = V.ni()-1;
	int maxy = V.nj()-1;
	int maxz = V.nk()-1;
	for(int i = 0; i < num_edges; i++)
	{
		int x = orig_edgemap[i]->x;
		int y = orig_edgemap[i]->y;
		int z = orig_edgemap[i]->z;
		if(x >= 0 && y >= 0 && z >= 0 && x <= maxx && y <= maxy && z <= maxz)
		{
			if(V(x,y,z) < BG_thresh)
			{
				BG_edgemap.push_back(orig_edgemap[i]);
			}
			else
			{
				FG_edgemap.push_back(orig_edgemap[i]);
			}
		}
	}
}

void dbdet_get_active_regions(vil_image_view<double>& DT, vil_image_view<bool>& external_active, vil_image_view<bool>& external_inactive, double dist_thresh)
{
	int height = DT.nj();
	int width = DT.ni();
	external_active.set_size(width, height);
	external_active.fill(false);
	external_inactive.set_size(width, height);
	external_inactive.fill(false);
	for(int j = 0; j < height; j++)
	{
		for(int i = 0; i < width; i++)
		{
			if(DT(i,j) < dist_thresh)
			{
				external_active(i,j) = true;
			}
			else
			{
				external_inactive(i,j) = true;
			}
		}
	}
}
