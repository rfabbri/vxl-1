// This is /lemsvxl/contrib/firat/polygon_distance_transform/compute_polygon_distance_transform.cxx.

// \file
// \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
// \date Aug 15, 2011

#include "polygon_distance_transform.h"
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>

int main(int argn, char* args[])
{
	std::string polygon_file = args[1];
	int height = std::atoi(args[2]);
	int width = std::atoi(args[3]);
	double hx = std::atof(args[4]);
	double hy = std::atof(args[5]);
	std::string phi_file = args[6];
	std::vector<double* > xv;
	std::vector<double* > yv;
	std::vector<int> num_points;
	read_polygon_file(polygon_file, xv, yv, num_points);
	vnl_matrix<double> phi;
	compute_polygon_signed_distance_transform(xv, yv, num_points, phi, height, width, hx, hy);
	std::ofstream ofs(phi_file.c_str());
	int real_height = (height-1)/hy + 1;
	int real_width = (width-1)/hx + 1;
	for(int yi = 0; yi < real_height; yi++)
	{
		for(int xi = 0; xi < real_width; xi++)
		{
			ofs << phi(yi,xi);
			if(xi != real_width-1)
			{
				ofs << " ";
			}
		}
		if(yi != real_height-1)
		{
			ofs << std::endl;
		}
	}
	ofs.close();
	return 0;
}
