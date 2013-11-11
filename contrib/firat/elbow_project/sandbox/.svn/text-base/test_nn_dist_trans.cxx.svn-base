/*
 * test_nn_dist_trans.cxx
 *
 *  Created on: Nov 14, 2011
 *      Author: firat
 */

#include "../algo/nn_subpixel_distance_transform.h"

int main()
{
	vcl_vector<dbdet_3d_edge_sptr> edges;
	dbdet_3d_edge_sptr edg = new dbdet_3d_edge;
	edg->x = 5.1;
	edg->y = 4.2;
	edg->z = 3.3;
	edges.push_back(edg);
	edg = new dbdet_3d_edge;
	edg->x = 1.01;
	edg->y = 7.76;
	edg->z = 3.05;
	edges.push_back(edg);
	vil_image_view<double> DT;
	nn_subpixel_distance_transform(edges, DT, 10, 10, 3, 1, 1, 1);

	for(int j = 0; j < DT.nj(); j++)
	{
		for(int i = 0; i < DT.ni(); i++)
		{

			vcl_cout << DT(i,j) << " ";
		}
		vcl_cout << vcl_endl;
	}

	vcl_cout << vcl_endl << vcl_endl;
	nn_subpixel_distance_transform(edges, DT, 10, 10, 3, 1, 1, 1);

		for(int j = 0; j < DT.nj(); j++)
		{
			for(int i = 0; i < DT.ni(); i++)
			{

				vcl_cout << DT(i,j) << " ";
			}
			vcl_cout << vcl_endl;
		}

	return 0;
}




