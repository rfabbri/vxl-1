// This is /lemsvxl/contrib/firat/third_order_edge_detector/third_order_edge_detector.cxx.

// \file
// \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
// \date Jul 20, 2011

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_cstdlib.h>
#include <dbdet/pro/dbdet_third_order_edge_detector_process.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vil/vil_load.h>
#include <dbdet/pro/dbdet_save_edg_process.h>
#include <dbdet/pro/dbdet_edgemap_storage.h>

int main(int argn, char* args[])
{
	vcl_string input_file = args[1];
	vcl_string output_file = args[2];
	double sigma = vcl_atof(args[3]);
	double grad_thresh = vcl_atof(args[4]);
	int N = vcl_atoi(args[5]);

	vil_image_resource_sptr img_sptr = vil_load_image_resource(input_file.c_str());
	vidpro1_image_storage_sptr inp = new vidpro1_image_storage();
	inp->set_image(img_sptr);

	dbdet_third_order_edge_detector_process pro;
	pro.parameters()->set_value("-sigma", sigma);
	pro.parameters()->set_value("-thresh", grad_thresh);
	pro.parameters()->set_value("-int_factor", N);
	pro.clear_input();
	pro.clear_output();
	pro.add_input(inp);
	pro.execute();

	vcl_vector<bpro1_storage_sptr> edge_det_results = pro.get_output();

	dbdet_save_edg_process save_edg_pro;
	bpro1_filepath output(output_file);
	save_edg_pro.parameters()->set_value("-edgoutput", output);
	save_edg_pro.clear_input();
	save_edg_pro.clear_output();
	save_edg_pro.add_input(edge_det_results[0]);
	save_edg_pro.execute();
	return 0;
}
