/*
 * dbdet_third_order_3d_edge_detector_process.cxx
 *
 *  Created on: Oct 16, 2011
 *      Author: firat
 */


#include "dbdet_third_order_3d_edge_detector_process.h"
#include <bpro1/bpro1_parameters.h>
#include <vil3d/vil3d_image_view.h>
#include "../storage/elbow_vil3d_storage_sptr.h"
#include "../storage/elbow_vil3d_storage.h"
#include "../storage/dbdet_third_order_3d_edge_storage.h"
#include "../storage/dbdet_third_order_3d_edge_storage_sptr.h"
#include "../algo/dbdet_third_order_3d_edge_detector.h"
#include "dbdet_third_order_3d_edge_detector_process_utils.h"
#include <bvis1/bvis1_manager.h>
#include <vul/vul_timer.h>


//: Constructor
dbdet_third_order_3d_edge_detector_process::
dbdet_third_order_3d_edge_detector_process()
{
	if( 	   !parameters()->add( "Sigma" , "-sigma", double(1.0))
			|| !parameters()->add( "Strength threshold" , "-str", double(1))
			|| !parameters()->add( "Grid spacing (h)" , "-h", double(0.5))
			|| !parameters()->add( "Maximum block size (in pixels)", "-bs", int(100))
	)
	{
		vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
	}
}


//: Destructor
dbdet_third_order_3d_edge_detector_process::
~dbdet_third_order_3d_edge_detector_process()
{

}


//: Clone the process
bpro1_process* dbdet_third_order_3d_edge_detector_process::
clone() const
{
	return new dbdet_third_order_3d_edge_detector_process(*this);
}

//: Returns the name of this process
vcl_string dbdet_third_order_3d_edge_detector_process::
name()
{
	return "Run 3D edge detector";
}

//: Provide a vector of required input types
vcl_vector< vcl_string > dbdet_third_order_3d_edge_detector_process::
get_input_type()
{
	vcl_vector< vcl_string > to_return;
	to_return.push_back("3d_dataset");
	return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbdet_third_order_3d_edge_detector_process::
get_output_type()
{
	vcl_vector<vcl_string > to_return;
	to_return.push_back("3d_edges");
	return to_return;
}

//: Return the number of input frames for this process
int dbdet_third_order_3d_edge_detector_process::
input_frames()
{
	bvis1_manager::instance()->first_frame();
	return 1;
}


//: Return the number of output frames for this process
int dbdet_third_order_3d_edge_detector_process::
output_frames()
{
	return 1;
}

//: Execute this process
bool dbdet_third_order_3d_edge_detector_process::
execute()
{
	vul_timer t;
	// 1. parse process parameters

	// filename
	double sigma;
	double h;
	double str;
	int block_size;
	parameters()->get_value( "-sigma" , sigma );
	parameters()->get_value( "-h" , h );
	parameters()->get_value( "-str" , str );
	parameters()->get_value( "-bs" , block_size );

	elbow_vil3d_storage_sptr data_storage;
	data_storage.vertical_cast(input_data_[0][0]);
	vil3d_image_view<double>& image = data_storage->image();

	vcl_vector<vcl_vector<int> > rois;
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
	this->output_data_[0].push_back(edge_storage);

	double time_taken = t.real()/1000.0;
	t.mark();
	vcl_cout << "************ Time taken: "<< time_taken <<" sec" << vcl_endl;

	return true;
}

bool dbdet_third_order_3d_edge_detector_process::
finish()
{
	return true;
}

