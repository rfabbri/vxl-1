/*
 * dbdet_graddt_levelset2d_volume_segmentation_process.cxx
 *
 *  Created on: Feb 24, 2012
 *      Author: firat
 */


#include "dbdet_graddt_levelset2d_volume_segmentation_process.h"
#include "../storage/dbdet_third_order_3d_edge_storage.h"
#include "../storage/dbdet_third_order_3d_edge_storage_sptr.h"
#include "../storage/elbow_vil3d_storage_sptr.h"
#include "../storage/elbow_vil3d_storage.h"
#include "../algo/dbdet_graddt_levelset2d_segmentation.h"
#include "../algo/nn_subpixel_distance_transform.h"
#include <bpro1/bpro1_parameters.h>
#include <bvis1/bvis1_manager.h>
#include <vil/vil_math.h>
#include <vul/vul_timer.h>

//: Constructor
dbdet_graddt_levelset2d_volume_segmentation_process::
dbdet_graddt_levelset2d_volume_segmentation_process()
{
	if( 	   !parameters()->add( "[LEVELSET] beta0" , "-beta0", double(-1.0))
			|| !parameters()->add( "[LEVELSET] beta1" , "-beta1", double(0))
			|| !parameters()->add( "[LEVELSET] alpha" , "-alpha", double(0.15))
			|| !parameters()->add( "[LEVELSET] Maximum iteration" , "-maxiter", int(2000))
			|| !parameters()->add( "[LEVELSET] XGrid spacing (hx)" , "-hx", double(0.5))
			|| !parameters()->add( "[LEVELSET] YGrid spacing (hy)" , "-hy", double(0.5))
			|| !parameters()->add( "[LEVELSET] Edge-Slice Distance Threshold" , "-dthresh", double(.5))
			|| !parameters()->add( "[REINIT] Narrow band" , "-band", double(2))
			|| !parameters()->add( "[REINIT] Frequency" , "-reinitfreq", int(1))
			|| !parameters()->add( "[INIT] Background intensity threshold" , "-bgthresh", double(10))
			|| !parameters()->add( "[STOP] Stopping Threshold" , "-stopthresh", double(.05))
	)
	{
		vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
	}
}


//: Destructor
dbdet_graddt_levelset2d_volume_segmentation_process::
~dbdet_graddt_levelset2d_volume_segmentation_process()
{

}


//: Clone the process
bpro1_process* dbdet_graddt_levelset2d_volume_segmentation_process::
clone() const
{
	return new dbdet_graddt_levelset2d_volume_segmentation_process(*this);
}

//: Returns the name of this process
vcl_string dbdet_graddt_levelset2d_volume_segmentation_process::
name()
{
	return "Run slice-by-slice levelset segmentation";
}

//: Provide a vector of required input types
vcl_vector< vcl_string > dbdet_graddt_levelset2d_volume_segmentation_process::
get_input_type()
{
	vcl_vector< vcl_string > to_return;
	to_return.push_back("3d_dataset");
	to_return.push_back("3d_edges");
	return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbdet_graddt_levelset2d_volume_segmentation_process::
get_output_type()
{
	vcl_vector<vcl_string > to_return;
	to_return.push_back("levelset_function");
	return to_return;
}

//: Return the number of input frames for this process
int dbdet_graddt_levelset2d_volume_segmentation_process::
input_frames()
{
	bvis1_manager::instance()->first_frame();
	return 1;
}


//: Return the number of output frames for this process
int dbdet_graddt_levelset2d_volume_segmentation_process::
output_frames()
{
	return 1;
}

//: Execute this process
bool dbdet_graddt_levelset2d_volume_segmentation_process::
execute()
{
	vul_timer t;
	// 1. parse process parameters

	// filename
	double beta0, beta1, alpha;
	double hx, hy;
	int maxiter;
	int reinit_freq;
	double band;
	double BG_thresh;
	double dist_thresh;
	double stop_thresh;
	parameters()->get_value( "-beta0" , beta0 ); vcl_cout << "beta0: " << beta0 << vcl_endl;
	parameters()->get_value( "-beta1" , beta1 ); vcl_cout << "beta1: " << beta1 << vcl_endl;
	parameters()->get_value( "-alpha" , alpha ); vcl_cout << "alpha: " << alpha << vcl_endl;
	parameters()->get_value( "-hx" , hx ); vcl_cout << "hx: " << hx << vcl_endl;
	parameters()->get_value( "-hy" , hy ); vcl_cout << "hy: " << hy << vcl_endl;
	parameters()->get_value( "-maxiter" , maxiter ); vcl_cout << "maxiter: " << maxiter << vcl_endl;
	parameters()->get_value( "-bgthresh" , BG_thresh ); vcl_cout << "BG_thresh: " << BG_thresh << vcl_endl;
	parameters()->get_value( "-dthresh" , dist_thresh ); vcl_cout << "dist_thresh: " << dist_thresh << vcl_endl;
	parameters()->get_value( "-reinitfreq" , reinit_freq ); vcl_cout << "reinit_freq: " << reinit_freq << vcl_endl;
	parameters()->get_value( "-stopthresh" , stop_thresh ); vcl_cout << "stop_thresh: " << stop_thresh << vcl_endl;

	elbow_vil3d_storage_sptr data_storage;
	data_storage.vertical_cast(input_data_[0][0]);
	vil3d_image_view<double>& image = data_storage->image();

	dbdet_third_order_3d_edge_storage_sptr edg_storage;
	edg_storage.vertical_cast(input_data_[0][1]);
	vcl_vector<dbdet_3d_edge_sptr>& edgemap = edg_storage->edgemap();

	vcl_vector<dbdet_3d_edge_sptr> FG_edgemap;
	vcl_vector<dbdet_3d_edge_sptr> BG_edgemap;

	dbdet_classify_edges(image, edgemap, FG_edgemap, BG_edgemap, BG_thresh);
	int width = image.ni();
	int height = image.nj();
	int nbands = image.nk();
	vil_image_view<double> speed((width-1)/hx+1, (height-1)/hy+1);
	speed.fill(1);
	for(int i = 0; i < nbands; i++)
	{
		dbdet_graddt_levelset2d_segmentation seg;
		vil_image_view<double> DT;
		nn_subpixel_distance_transform(BG_edgemap, DT, width, height, i, hx, hy, dist_thresh);
		vil_math_scale_and_offset_values(DT, 1, -0.1);
		seg.initialize(DT, hx, hy);
		seg.set_reinitialization_frequency(reinit_freq);
		seg.set_narrowband(band);
		if(beta0 != 0.0)
		{
			seg.use_reaction_flux(true);
		}
		if(beta1 < 0.0)
		{
			seg.use_diffusion_flux(true);
		}
		if(alpha > 0.0)
		{
			seg.use_external_force(true);
		}
		seg.set_params(beta0, beta1, alpha);
		seg.set_speed(speed);
		seg.set_stop_threshold(stop_thresh);
		vil_image_view<double> edge_dt, Fx, Fy;
		dbdet_compute_graddt_force(FG_edgemap, edge_dt, Fx, Fy, i, width,
				height, hx, hy, dist_thresh);
		seg.set_external_force_field(Fx, Fy);
		vil_image_view<bool> external_active, external_inactive;
		dbdet_get_active_regions(edge_dt, external_active, external_inactive, hx*1.8);
		seg.set_active_regions(external_inactive, external_inactive, external_active);

		bool converged = seg.update(maxiter);
		vcl_cout << "Segmentation took " << seg.iteration() << " iterations. ";
		if(converged)
		{
			vcl_cout << "[Converged]" << vcl_endl;
		}
		else
		{
			vcl_cout << "[Not Converged]" << vcl_endl;
		}
	}



	/*vcl_vector<vcl_vector<int> > rois;
	dbdet_compute_rois(image, rois, block_size);
	vcl_vector<dbdet_3d_edge_sptr> edgemap;
	vcl_cout << "Number of blocks = " << rois.size() << vcl_endl;
	for(int i = 0; i < rois.size(); i++)
	{
		vcl_cout << "Block " << i+1 << " : " << rois[i][0] << " " << rois[i][1] << " " <<
				rois[i][2] << " " << rois[i][3] << " " << rois[i][4] << " " << rois[i][5] << vcl_endl;
		vcl_vector<dbdet_3d_edge_sptr> edg;
		dbdet_third_order_3d_edge_detector_roi(image, edg, str, sigma, h,
				rois[i][0], rois[i][1], rois[i][2], rois[i][3], rois[i][4], rois[i][5]);
		for(int j = 0; j <  edg.size(); j++)
		{
			edg[j]->x += rois[i][0];
			edg[j]->y += rois[i][1];
			edg[j]->z += rois[i][2];
		}
		edgemap.insert(edgemap.end(), edg.begin(), edg.end());
	}
	vcl_cout << "Number of edges = " << edgemap.size() << vcl_endl;
	dbdet_third_order_3d_edge_storage_sptr edge_storage = dbdet_third_order_3d_edge_storage_new();
	edge_storage->set_edgemap(edgemap);
	this->output_data_[0].push_back(edge_storage);*/

	double time_taken = t.real()/1000.0;
	t.mark();
	vcl_cout << "************ Time taken: "<< time_taken <<" sec" << vcl_endl;

	return true;
}

bool dbdet_graddt_levelset2d_volume_segmentation_process::
finish()
{
	return true;
}



