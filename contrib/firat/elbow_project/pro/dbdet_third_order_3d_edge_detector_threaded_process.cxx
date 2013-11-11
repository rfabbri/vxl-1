/*
 * dbdet_third_order_3d_edge_detector_threaded_process.cxx
 *
 *  Created on: Oct 16, 2011
 *      Author: firat
 */

#include "dbdet_third_order_3d_edge_detector_threaded_process.h"
#include <bpro1/bpro1_parameters.h>
#include <vil3d/vil3d_image_view.h>
#include "../storage/elbow_vil3d_storage_sptr.h"
#include "../storage/elbow_vil3d_storage.h"
#include "../storage/dbdet_third_order_3d_edge_storage.h"
#include "../storage/dbdet_third_order_3d_edge_storage_sptr.h"
#include "../algo/dbdet_third_order_3d_edge_detector.h"
#include "dbdet_third_order_3d_edge_detector_process_utils.h"
#include <vil3d/vil3d_slice.h>
#include <bvis1/bvis1_manager.h>
#include <vul/vul_timer.h>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <boost/threadpool.hpp>

using namespace boost::threadpool;


//: Constructor
dbdet_third_order_3d_edge_detector_threaded_process::
dbdet_third_order_3d_edge_detector_threaded_process()
{
	if( 	!parameters()->add( "Sigma" , "-sigma", double(1.0))
			|| !parameters()->add( "Strength threshold" , "-str", double(1))
			|| !parameters()->add( "Grid spacing (h)" , "-h", double(0.5))
			|| !parameters()->add( "Maximum block size (in pixels)", "-bs", int(80))
			|| !parameters()->add( "Maximum number of threads" , "-maxt", int(2))
	)
	{
		vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
	}
}


//: Destructor
dbdet_third_order_3d_edge_detector_threaded_process::
~dbdet_third_order_3d_edge_detector_threaded_process()
{

}


//: Clone the process
bpro1_process* dbdet_third_order_3d_edge_detector_threaded_process::
clone() const
{
	return new dbdet_third_order_3d_edge_detector_threaded_process(*this);
}

//: Returns the name of this process
vcl_string dbdet_third_order_3d_edge_detector_threaded_process::
name()
{
	return "Run 3D edge detector (threaded)";
}

//: Provide a vector of required input types
vcl_vector< vcl_string > dbdet_third_order_3d_edge_detector_threaded_process::
get_input_type()
{
	vcl_vector< vcl_string > to_return;
	to_return.push_back("3d_dataset");
	return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbdet_third_order_3d_edge_detector_threaded_process::
get_output_type()
{
	vcl_vector<vcl_string > to_return;
	to_return.push_back("3d_edges");
	return to_return;
}

//: Return the number of input frames for this process
int dbdet_third_order_3d_edge_detector_threaded_process::
input_frames()
{
	bvis1_manager::instance()->first_frame();
	return 1;
}


//: Return the number of output frames for this process
int dbdet_third_order_3d_edge_detector_threaded_process::
output_frames()
{
	return 1;
}

//workaround for the compilation error
double sigma;
double h;
double str;
int block_size;
int num_threads;
vil3d_image_view<double>* image;
vcl_vector<vcl_vector<int> > rois;
vcl_vector<dbdet_3d_edge_sptr>* edgemaps;

void thread_func(int i)
{vcl_cout << "Block " << i+1 << " : " << rois[i][0] << " " << rois[i][1] << " " <<
	rois[i][2] << " " << rois[i][3] << " " << rois[i][4] << " " << rois[i][5] << vcl_endl;
dbdet_third_order_3d_edge_detector_roi(*image, edgemaps[i], str, sigma, h,
			rois[i][0], rois[i][1], rois[i][2], rois[i][3], rois[i][4], rois[i][5]);
}

//: Execute this process
bool dbdet_third_order_3d_edge_detector_threaded_process::
execute()
{
	vul_timer t;
	// 1. parse process parameters

	// filename

	parameters()->get_value( "-sigma" , sigma );
	parameters()->get_value( "-h" , h );
	parameters()->get_value( "-str" , str );
	parameters()->get_value( "-bs" , block_size );
	parameters()->get_value( "-maxt" , num_threads );

	elbow_vil3d_storage_sptr data_storage;
	data_storage.vertical_cast(input_data_[0][0]);
	image = &data_storage->image();

	dbdet_compute_rois(*image, rois, block_size);

	vcl_cout << "Number of blocks = " << rois.size() << vcl_endl;

	edgemaps = new vcl_vector<dbdet_3d_edge_sptr>[rois.size()];
	{
		pool tp(num_threads);
		for(int i = 0; i < rois.size(); i++)
		{
			tp.schedule(boost::bind(thread_func, i));
		}
	}

	vcl_vector<dbdet_3d_edge_sptr> edgemap;
	for(int i = 0; i < rois.size(); i++)
	{
		for(int j = 0; j <  edgemaps[i].size(); j++)
		{
			edgemaps[i][j]->x += rois[i][0];
			edgemaps[i][j]->y += rois[i][1];
			edgemaps[i][j]->z += rois[i][2];
		}
		edgemap.insert(edgemap.end(), edgemaps[i].begin(), edgemaps[i].end());
	}
	vcl_cout << "Number of edges = " << edgemap.size() << vcl_endl;
	delete[] edgemaps;

	dbdet_third_order_3d_edge_storage_sptr edge_storage = dbdet_third_order_3d_edge_storage_new();
	edge_storage->set_edgemap(edgemap);
	this->output_data_[0].push_back(edge_storage);


	double time_taken = t.real()/1000.0;
	t.mark();
	vcl_cout << "************ Time taken: "<< time_taken <<" sec" << vcl_endl;

	return true;

}

bool dbdet_third_order_3d_edge_detector_threaded_process::
finish()
{
	return true;
}




