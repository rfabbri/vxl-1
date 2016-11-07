// This is /lemsvxl/contrib/firat/polygon_distance_transform/polygon_distance_transform.cxx.

// \file
// \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
// \date Aug 14, 2011

#include "polygon_distance_transform.h"
#include <vgl/vgl_distance.h>
#include <vgl/vgl_polygon_test.h>
#include <vcl_fstream.h>

bool compute_polygon_signed_distance_transform(vcl_vector<double* >& xv, vcl_vector<double* >& yv, vcl_vector<int> num_points, vnl_matrix<double>& phi, int height, int width, double hx, double hy)
{
	int real_height = (height-1)/hy + 1;
	int real_width = (width-1)/hx + 1;
	phi.set_size(real_height, real_width);
	for(int yi = 0; yi < real_height; yi++)
	{
		for(int xi = 0; xi < real_width; xi++)
		{
			double x = xi*hx;
			double y = yi*hy;
			double min_d = height+width;
			int polygon_id = -1;
			for(int i = 0; i < xv.size(); i++)
			{
				double d = vgl_distance_to_closed_polygon<double>(xv[i], yv[i], num_points[i], x, y);
				if(d < min_d)
				{
					min_d = d;
					polygon_id = i;
				}
			}
			phi(yi,xi) = min_d;
			for(int i = 0; i < xv.size(); i++)
			{
				if(vgl_polygon_test_inside<double>(xv[i], yv[i], num_points[i], x, y))
				{
					phi(yi,xi) = -phi(yi,xi);
				}
			}
		}
	}
	return true;
}

bool read_polygon_file(const vcl_string& polygon_file, vcl_vector<double* >& xv, vcl_vector<double* >& yv, vcl_vector<int>& num_points)
{
	vnl_matrix<double> polygons;
	vcl_ifstream ifs(polygon_file.c_str());
	polygons.read_ascii(ifs);
	ifs.close();
	int i = 0;
	int length_polygons = polygons.rows();
	while(i < length_polygons)
	{
		int l = polygons(i,0);
		double* px = new double[l];
		double* py = new double[l];
		for(int k = 0; k < l; k++)
		{
			px[k] = polygons(i+k+1, 0);
			py[k] = polygons(i+k+1, 1);
		}
		xv.push_back(px);
		yv.push_back(py);
		num_points.push_back(l);
		i = i + l + 1;
	}
	vcl_cout << "Number of polygons: " << num_points.size() << vcl_endl;
	return true;
}
