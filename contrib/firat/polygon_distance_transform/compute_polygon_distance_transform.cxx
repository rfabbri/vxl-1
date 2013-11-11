// This is /lemsvxl/contrib/firat/polygon_distance_transform/compute_polygon_distance_transform.cxx.

// \file
// \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
// \date Aug 15, 2011

#include "polygon_distance_transform.h"
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_fstream.h>
#include <vcl_cstdlib.h>

int main(int argn, char* args[])
{
	vcl_string polygon_file = args[1];
	int height = vcl_atoi(args[2]);
	int width = vcl_atoi(args[3]);
	double hx = vcl_atof(args[4]);
	double hy = vcl_atof(args[5]);
	vcl_string phi_file = args[6];
	vcl_vector<double* > xv;
	vcl_vector<double* > yv;
	vcl_vector<int> num_points;
	read_polygon_file(polygon_file, xv, yv, num_points);
	vnl_matrix<double> phi;
	compute_polygon_signed_distance_transform(xv, yv, num_points, phi, height, width, hx, hy);
	vcl_ofstream ofs(phi_file.c_str());
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
			ofs << vcl_endl;
		}
	}
	ofs.close();
	return 0;
}
