/*
 * elbow_edge_clustering.cxx
 *
 *  Created on: Oct 18, 2011
 *      Author: firat
 */

#include "elbow_edge_clustering.h"
#include <vil3d/algo/vil3d_find_blobs.h>
#include <vil3d/vil3d_image_view.h>
#include <vcl_map.h>

#define round_flt(a) (((a) - int(a) >= 0.5) ? int((a)+1) : int(a))

bool elbow_get_largest_edge_cluster(const vcl_vector<dbdet_3d_edge_sptr>& edgemap, vcl_vector<dbdet_3d_edge_sptr>& cluster,
		int ni, int nj, int nk, int h)
{
	int NI = (ni-1)/h+1;
	int NJ = (nj-1)/h+1;
	int NK = (nk-1)/h+1;
	vcl_cout << "Start: Create edge bins..." << vcl_endl;
	vcl_vector<vcl_vector<dbdet_3d_edge_sptr> > bins(NI*NJ*NK);
	vil3d_image_view<bool> bw(NI, NJ, NK);
	bw.fill(false);
	for(int i = 0; i < edgemap.size(); i++)
	{
		int i0 = round_flt(edgemap[i]->x/h);
		int j0 = round_flt(edgemap[i]->y/h);
		int k0 = round_flt(edgemap[i]->z/h);
		if(i0 >= 0 && i0 < NI && j0 >= 0 && j0 < NJ && k0 >= 0 && k0 < NK)
		{
			bins[i0 + j0*NI + k0*NI*NJ].push_back(edgemap[i]);
			bw(i0,j0,k0) = true;
		}
	}
	vcl_cout << "Finish: Create edge bins..." << vcl_endl;
	vcl_cout << "Start: Connected component analysis..." << vcl_endl;
	vil3d_image_view<unsigned> labels;
	vil3d_find_blobs(bw, vil3d_find_blob_connectivity_26_conn, labels);
	vcl_cout << "Finish: Connected component analysis..." << vcl_endl;
	vcl_cout << "Start: Find max label..." << vcl_endl;
	vcl_map<unsigned,int> hist;
	unsigned num_labels = 0;
	for(int i = 0; i < NI; i++)
	{
		for(int j = 0; j < NJ; j++)
		{
			for(int k = 0; k < NK; k++)
			{
				unsigned L = labels(i,j,k);
				if(num_labels < L)
				{
					num_labels = L;
				}
				vcl_map<unsigned,int>::iterator it = hist.find(L);
				if(it == hist.end())
				{
					hist[L] = 1;
				}
				else
				{
					(it->second)++;
				}
			}
		}
	}
	int max_count = -1;
	unsigned max_label;
	for(unsigned i = 1; i < num_labels+1; i++)
	{
		int c = hist[i];
		if(max_count < c)
		{
			max_count = c;
			max_label = i;
		}
	}
	vcl_cout << "Max count: " << max_count << " max_label: " << max_label << vcl_endl;
	vcl_cout << "Finish: Find max label..." << vcl_endl;
	vcl_cout << "Start: Extract largest cluster..." << vcl_endl;
	for(int i = 0; i < NI; i++)
	{
		for(int j = 0; j < NJ; j++)
		{
			for(int k = 0; k < NK; k++)
			{
				unsigned L = labels(i,j,k);
				if(L == max_label)
				{
					vcl_vector<dbdet_3d_edge_sptr>& edg = bins[i + j*NI + k*NI*NJ];
					cluster.insert(cluster.end(), edg.begin(), edg.end());
				}
			}
		}
	}
	vcl_cout << "Finish: Extract largest cluster..." << vcl_endl;
}




