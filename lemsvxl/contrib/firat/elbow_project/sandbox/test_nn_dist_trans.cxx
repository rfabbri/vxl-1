/*
 * test_nn_dist_trans.cxx
 *
 *  Created on: Nov 14, 2011
 *      Author: firat
 */

#include "../algo/nn_subpixel_distance_transform.h"

int main()
{
	std::vector<dbdet_3d_edge_sptr> edges;
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

			std::cout << DT(i,j) << " ";
		}
		std::cout << std::endl;
	}

	std::cout << std::endl << std::endl;
	nn_subpixel_distance_transform(edges, DT, 10, 10, 3, 1, 1, 1);

		for(int j = 0; j < DT.nj(); j++)
		{
			for(int i = 0; i < DT.ni(); i++)
			{

				std::cout << DT(i,j) << " ";
			}
			std::cout << std::endl;
		}

	return 0;
}




