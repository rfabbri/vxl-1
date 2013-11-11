// This is /lemsvxl/contrib/firat/dbcvr_2d_shape_descriptors/driver.cxx.

// \file
// \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
// \date May 3, 2011

#include "dbcvr_2d_shape_descriptor_utils.h"
#include "dbcvr_2d_shape_descriptors.h"
#include "dbcvr_2d_curve_matching.h"
#include <dbsksp/xio/dbsksp_xio_xshock_graph.h>
#include <dbsksp/algo/dbsksp_xgraph_boundary_algos.h>
#include <vcl_vector.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_digital_curve_2d.h>

int main()
{
	dbsksp_xshock_graph_sptr xg = 0;
	x_read("/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph-firat/swans/swans_black.xgraph.0.xml", xg);
	vcl_vector<vsol_point_2d_sptr> point_list =  dbsksp_get_boundary_point_list(xg, 1000);
	vsol_digital_curve_2d_sptr curve = new vsol_digital_curve_2d(point_list);
	curve = dbcvr_uniform_sampling_closed(curve, 81);

	/*vcl_ofstream ofs("/home/firat/Desktop/1.txt");
	ofs << curve->size() << " 0" << vcl_endl;
	for(int i = 0; i < curve->size(); i++)
	{
		vsol_point_2d_sptr point = curve->point(i);
		ofs << point->x() << " " << point->y() << vcl_endl;
	}
	ofs.close();*/
	vnl_matrix<double> alpha_prime;
	vcl_vector<unsigned> indices;
	dbcvr_diff_chord_angles(curve, true, indices, alpha_prime);
	vcl_cout << alpha_prime(0,0) << " " << alpha_prime(1,42) << " " << alpha_prime(1,80) << vcl_endl;

	vcl_vector<vsol_point_2d_sptr> point_list2;
	vcl_cout << "curve size: " << curve->size() << vcl_endl;
	for(int i = 10; i < 35; i++)
	{
		point_list2.push_back(new vsol_point_2d(curve->point(i)->x(),curve->point(i)->y()));
	}
	vsol_digital_curve_2d_sptr curve2 = new vsol_digital_curve_2d(point_list2);
	vcl_cout << "curve2 size: " << curve2->size() << vcl_endl;
	vcl_vector<dbcvr_curve_match> matches;
	vcl_vector<unsigned> indices2;

	//dbcvr_get_best_matches_single_scale(curve, curve2, indices2, matches, false, 30, 1);
	vcl_vector<int> sample_points;
	for(int i = 5; i < 81; i+=4)
	sample_points.push_back(i);


	dbcvr_get_best_matches_multi_scale(curve, curve2, indices2, matches, sample_points, 30, 2);

	for(int i = 0; i < matches.size(); i++)
	{
		matches[i].print();
	}
	return 0;
}
