/*
 * dbdet_third_order_3d_edge_detector_process_utils.cxx
 *
 *  Created on: Oct 24, 2011
 *      Author: firat
 */

#include "dbdet_third_order_3d_edge_detector_process_utils.h"


void dbdet_compute_effective_block_size_and_number(int n, int bs, int& num, int*& cumsum_ebs_vec)
{
	num = n / bs; // number of blocks
	int rem1 = n % bs;
	if(rem1 > 0)
	{
		num++;
	}
	int ebs = n / num; // effective block size
	int rem = n % num;
	int* ebs_vec = new int[num];
	for(int i = 0; i < num; i++)
	{
		if(i < rem)
		{
			ebs_vec[i] = ebs + 1;
		}
		else
		{
			ebs_vec[i] = ebs;
		}
	}
	cumsum_ebs_vec = new int[num+1];
	cumsum_ebs_vec[0] = 0;
	for(int i = 0; i < num; i++)
	{
		cumsum_ebs_vec[i+1] = cumsum_ebs_vec[i] + ebs_vec[i];
	}
	delete[] ebs_vec;
}

bool dbdet_compute_rois(const vil3d_image_view<double>& image, vcl_vector<vcl_vector<int> >& rois, int block_size)
{
	int ni = image.ni(), nj = image.nj(), nk = image.nk();
	int num_i, *vec_i;
	dbdet_compute_effective_block_size_and_number(ni, block_size, num_i, vec_i);
	int num_j, *vec_j;
	dbdet_compute_effective_block_size_and_number(nj, block_size, num_j, vec_j);
	int num_k, *vec_k;
	dbdet_compute_effective_block_size_and_number(nk, block_size, num_k, vec_k);

	for(int i = 0; i < num_i; i++)
	{
		int i0 = vec_i[i];
		int ni_roi = vec_i[i+1]-vec_i[i];
		for(int j = 0; j < num_j; j++)
		{
			int j0 = vec_j[j];
			int nj_roi = vec_j[j+1]-vec_j[j];
			for(int k = 0; k < num_k; k++)
			{
				int k0 = vec_k[k];
				int nk_roi = vec_k[k+1]-vec_k[k];
				vcl_vector<int> temp(6);
				temp[0] = i0; temp[1] = j0; temp[2] = k0;
				temp[3] = ni_roi; temp[4] = nj_roi; temp[5] = nk_roi;
				rois.push_back(temp);
			}
		}
	}
	delete[] vec_i;
	delete[] vec_j;
	delete[] vec_k;
	return true;
}



