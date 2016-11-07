/*
 * nn_subpixel_distance_transform.cxx
 *
 *  Created on: Nov 14, 2011
 *      Author: firat
 */

#include"nn_subpixel_distance_transform.h"
#include<ANN/ANN.h>
#include<vcl_cmath.h>

bool nn_subpixel_distance_transform(vcl_vector<dbdet_3d_edge_sptr>& edges, vil_image_view<double>& DT,
		int ni, int nj, int slice_index, double hx, double hy, double dist_thresh)
{
	int NI = (ni-1)/hx+1;
	int NJ = (nj-1)/hy+1;
	int dim = 3;
	DT.set_size(NI,NJ);
	vcl_vector<dbdet_3d_edge_sptr> close_edges;
	for(int i = 0; i < edges.size(); i++)
	{
		if(vcl_fabs(edges[i]->z - slice_index) < dist_thresh)
		{
			close_edges.push_back(edges[i]);
		}
	}
	ANNpointArray dataPts = annAllocPts(close_edges.size(), dim);
	for(int i = 0; i < close_edges.size(); i++)
	{
		dataPts[i][0] = close_edges[i]->x;
		dataPts[i][1] = close_edges[i]->y;
		dataPts[i][2] = close_edges[i]->z;
	}
	ANNkd_tree* kdTree = new ANNkd_tree(// build search structure
			dataPts, close_edges.size(), dim);
	ANNpoint queryPt = annAllocPt(dim);
	queryPt[2] = slice_index;
	for(int i = 0; i < NI; i++)
	{
		queryPt[0] = i*hx;
		for(int j = 0; j < NJ; j++)
		{
			queryPt[1] = j*hy;
			ANNidx nnIdx;
			ANNdist dist;
			kdTree->annkSearch(						// search
					queryPt,						// query point
					1,								// number of near neighbors
					&nnIdx,							// nearest neighbors (returned)
					&dist,							// distance (returned)
					0);
			DT(i,j) = vcl_sqrt(dist);
		}
	}
	delete kdTree;
	annDeallocPt(queryPt);
	annDeallocPts(dataPts);
	annClose();
	return true;
}




