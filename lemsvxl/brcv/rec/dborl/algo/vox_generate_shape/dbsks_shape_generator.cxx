// This is dbsks/algo/dbsks_shape_generator.cxx

//:
// \file
// author Yuliang, 02/20/2014
// load in edg from .cem
// use single scale edgemap, single scale xgraph
// aim to detection in sequential frames


#include "dbsks_shape_generator.h"

#include <dbsks/dbsks_det_desc_xgraph.h>
#include <dbsks/dbsks_det_desc_xgraph_sptr.h>
#include <dbsks/dbsks_det_nms.h>
#include <dbsks/dbsks_biarc_sampler.h>
#include <dbsks/dbsks_xshock_detector.h>
#include <dbsks/dbsks_xgraph_geom_model.h>
#include <dbsks/dbsks_xgraph_ccm_model.h>
#include <dbsks/dbsks_xshock_ccm_likelihood.h>
#include <dbsks/xio/dbsks_xio_xshock_det.h>

#include <dbsks/dbsks_xfrag_geom_model.h>
#include <dbsks/dbsks_xnode_geom_model.h>

#include <dbsks/algo/dbsks_load.h>
#include <dbsks/algo/dbsks_vox_utils.h>
#include <dbsks/algo/dbsks_algos.h>

#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsksp/algo/dbsksp_xgraph_algos.h>

#include <dbdet/algo/dbdet_resize_edgemap.h>
#include <dbdet/pro/dbdet_save_edg_process.h>
#include <dbdet/pro/dbdet_load_cem_process.h>
#include <dbdet/pro/dbdet_load_edg_process.h>
#include <dbdet/pro/dbdet_edgemap_storage.h>


#include <vsol/vsol_box_2d.h>
#include <bpro1/bpro1_parameters.h>
#include <vil/vil_image_resource.h>
#include <vil/file_formats/vil_pyramid_image_list.h>
#include <vil/vil_load.h>
#include <vil/vil_convert.h>
#include <bil/algo/bil_color_conversions.h>

#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_timer.h>
#include <vul/vul_sprintf.h>

//#include <vcl_iostream.h>
//#include <vcl_fstream.h>
#include <vcl_sstream.h>

//------------------------------------------------------------------------------
//:
bool dbsks_shape_generator::
execute()
{
	vcl_cout << "\n|=================================================================|"
		   << "\n|   Generate Shapes (xgraphs) based on Learned Geometriy Model    |"
		   << "\n|=================================================================|\n\n";

	//> Preliminary checks
	output_shape_list.clear(); // clean up output results

	//> Load params and models
	this->load_params_and_models();


	//    dbsksp_xshock_graph_sptr actual_xgraph = 0;   //> xgraph to use as prototype
	//	dbdet_sel_storage_sptr actual_sel = 0; //> cfrags to use in case

	vcl_vector<dbsks_det_desc_xgraph_sptr > shapes;
	//> Detect xgraph with the specified scale
	shapes.clear();

	int num_shapes = num_samples;

	root_vid_ = this->xgraph_geom->root_vid();
	major_child_eid_ = this->xgraph_geom->major_child_eid();
	dbsksp_xshock_node_sptr xv_root = xgraph_prototype_->node_from_id(root_vid_);
	// Geometric model for the root 
	dbsks_xnode_geom_model_sptr geom_model_root = this-> xgraph_geom->map_node2geom()[xv_root->id()];
	// keep x, y, r of root node. fix psi = 0, resample phi

	vnl_vector<double> phi_list(num_shapes), phi_diff_list(num_shapes);
	double min_psi, max_psi,min_radius, max_radius,min_phi, max_phi,min_phi_diff, max_phi_diff,graph_size;
	geom_model_root->get_param_range(min_psi, max_psi,min_radius, max_radius,min_phi, max_phi,min_phi_diff, max_phi_diff,graph_size);

    // resample phi of root node
	pdf1d_flat phi_model;
	phi_model.set(min_phi, max_phi);
	pdf1d_sampler* phi_sampler = phi_model.new_sampler();
    phi_sampler->get_samples(phi_list);

    if(xv_root->degree()>2)
	{
		vcl_cout<< "Sampling can not proceed from root with degree > 2 \n";
		return false;
    }
	/*
	// if root is in degree 3, resample phi_diff
    if(xv_root->degree()==3)
	{
		pdf1d_flat phi_diff_model;
		phi_diff_model.set(min_phi, max_phi);
		pdf1d_sampler* phi_diff_sampler = phi_diff_model.new_sampler();
		phi_diff_sampler->get_samples(phi_diff_list);	
    }
	*/

	// Sample num of shapes
	for(int n =0; n < num_shapes; n++)
	{
		dbsksp_xshock_graph_sptr xgraph = new dbsksp_xshock_graph(*this->xgraph_prototype_);
		xgraph->compute_vertex_depths(xgraph->root_vertex_id());
		vcl_cout << "\nConstructed new output xgraph. Num: " <<(n+1) << vcl_endl;

		// assume the root node is degree-2 or degree-1 right now
		dbsksp_xshock_edge_sptr xe1 = xgraph->edge_from_id(major_child_eid_);
		double x, y, r, psi, phi;
		xv_root = xgraph->node_from_id(root_vid_);
		xv_root->descriptor(xe1)->get(x,y, psi, phi, r);
		//psi = 0;
		dbsksp_xshock_node_descriptor xd_root_new(x, y, psi, phi_list(n), r);
		// update root node
        vcl_cout << "resample root node..." << vcl_endl;
		dbsksp_xgraph_algos::update_degree2_node(xv_root, xe1, xd_root_new);	
		vcl_cout << "recursively sample nodes from geometric model and update in xgraph" << vcl_endl;
		if(!this->sample_shapes_from_geom_model(xgraph))
			continue;

	    dbsks_det_desc_xgraph_sptr det = new dbsks_det_desc_xgraph(xgraph, 100 );
	    det->compute_bbox();
	    shapes.push_back(det);
	}
	this->output_shape_list = shapes;

	if(shapes.size() ==0)
	{
		vcl_cout << "---------------------------- Fail in Sampling Any Shape ----------------------------\n";
		return false;
	}
    vcl_cout << "---------------------------- Sampled "<< shapes.size() <<" Shapes ----------------------------\n";
  return true;
}


//------------------------------------------------------------------------------
//: Load all data from input files
bool dbsks_shape_generator::
load_params_and_models()
{
  // Load data from the files

  // xgraph
  if (!dbsks_load_xgraph(xgraph_file, xgraph_prototype_))
  {
    return false;
  }

  // geometric model
  if (!dbsks_load_xgraph_geom_model(xgraph_geom_file, xgraph_geom_param_file, xgraph_geom))
  {
    return false;
  }
  xgraph_geom->compute_attribute_constraints();

  // Check compatibility between the geometric model and the shock graph (are all edges covered?)
  vcl_cout << "\n>> Checking compatibility between geometric model and xgraph...";
  if (!xgraph_geom->is_compatible(xgraph_prototype_))
  {
    vcl_cout << "Failed\n." << vcl_endl;
    return false;
  }
  else
  {
    vcl_cout << "Passed\n." << vcl_endl;
  }

/*
  //: Compute cache values for ccm models
  xgraph_ccm->compute_cache_loglike_for_all_edges();

  //: Construct biarc sampler from xgraph_ccm model
  xgraph_ccm->build_biarc_sampler(this->biarc_sampler);
  this->biarc_sampler.compute_cache_sample_points();
  this->biarc_sampler.compute_cache_nkdiff();
*/
  return true;
}


bool dbsks_shape_generator::
sample_shapes_from_geom_model( dbsksp_xshock_graph_sptr& xgraph)
{
  vul_timer timer;
  timer.mark();

  if (!this->xgraph_geom)
    return false;

  vcl_cout << "++++++++++++++++++++++++++ Start Sampling ++++++++++++++++++++++++++++++++\n" ;


//  xgraph = this->xgraph_prototype_;

//  unsigned root_vid = this->xgraph_geom->root_vid();
//  unsigned major_child_eid = this->xgraph_geom->major_child_eid();
  dbsksp_xshock_node_sptr xv_root = xgraph->node_from_id(root_vid_);

//  // root node needs to be a degree-2 node
//  if (!xv_root || xv_root->degree() != 2) 
//    return false;
  
  // the "major" edge
  dbsksp_xshock_edge_sptr xe1 = xgraph->edge_from_id(major_child_eid_);
  
  // check validity of the major edge
  if (!xe1 || !xe1->is_vertex(xv_root))
  {
    return false;
  }

  // the "minor" edge
  dbsksp_xshock_edge_sptr xe2 = xgraph->cyclic_adj_succ(xe1, xv_root);

  // Child node of major edge
  dbsksp_xshock_node_sptr xv_c1 = xe1->opposite(xv_root);

  //: Child node of minor edge
  dbsksp_xshock_node_sptr xv_c2 = xe2->opposite(xv_root);


// base descriptors for the child and parent nodes
  dbsksp_xshock_node_descriptor xd_p1 = *xv_root->descriptor(xe1);
  dbsksp_xshock_node_descriptor xd_p2 = *xv_root->descriptor(xe2);


// discritors of child nodes seem not necessuary
//    dbsksp_xshock_node_descriptor xdesc_c1 = xv_c1->descriptor(xe1)->opposite_xnode();
  
//    dbsksp_xshock_node_descriptor xdesc_c2 = xv_c2->descriptor(xe2)->opposite_xnode();
  

  // Geometric model for the two fragments
  dbsks_xfrag_geom_model_sptr geom_model1 = this->xgraph_geom->map_edge2geom()[xe1->id()];
  //geom_model1->set(xdesc_p1, xdesc_c1);

  dbsks_xfrag_geom_model_sptr geom_model2 = this->xgraph_geom->map_edge2geom()[xe2->id()];
  //geom_model2->set(xdesc_p1.opposite_xnode(), xdesc_c2);


  bool succeed = sample_child_frags_and_nodes (xd_p1, xe1, xv_c1, xgraph, geom_model1);
  // recurve function to sample child nodes and frags until the end of each branch
  if(succeed )
  {
	  if(xv_c2->degree()==2)
		  succeed &= sample_child_frags_and_nodes (xd_p2, xe2, xv_c2, xgraph, geom_model2);
  	  xgraph->update_all_degree_1_nodes();
	  vcl_cout << "++++++++++++++++++++++++++ Done Sampling Shape ++++++++++++++++++++++++++++++++\n";
  }	
  return succeed;
}

// recursive function to sample child nodes and fragments
//  xv_p----xe_c----> xv_c ------xe_c_next------> xv_c_next
bool dbsks_shape_generator::
sample_child_frags_and_nodes (dbsksp_xshock_node_descriptor xd_p, dbsksp_xshock_edge_sptr& xe_c, dbsksp_xshock_node_sptr& xv_c, dbsksp_xshock_graph_sptr& xgraph, dbsks_xfrag_geom_model_sptr geom_model)
{

    if(xv_c->degree()==1)
		return true;
   // given some p_node, no legal child node can be sampled from geom model. Need to skip this shape
	vcl_cout << "Sample legal end at eid: "<< xe_c->id() << vcl_endl;
	vcl_vector<dbsksp_xshock_node_descriptor > xdesc_list;

	if (!geom_model->sample_new_legal_end_given_start_using_model_minmax_range(xd_p, vcl_sqrt(xgraph->area()), 1, xdesc_list))
	{
		vcl_cout << "fail in sampling legal end given start using model minmax range, at eid: "<< xe_c->id() << vcl_endl;
		return false;
	}
    // newly sampled descriptor of the child node
	dbsksp_xshock_node_descriptor xd_c = xdesc_list[0];
	// need to update values in xgraph
	// .......

	if(xv_c->degree()==2)
	{

		//Prepare inputs for the function taking xv_c as the parent node
		// locate the outgoing edge at child node
		dbsksp_xshock_edge_sptr xe_c_next = xgraph->cyclic_adj_succ(xe_c, xv_c); 
		// locat the next child node of the child node
		dbsksp_xshock_node_sptr xv_c_next = xe_c_next->opposite(xv_c);

		// update degree-2 node
		dbsksp_xgraph_algos::update_degree2_node(xv_c, xe_c_next, xd_c);		

		// discripter of xv_c -> xe_c_next, assuming updated
		dbsksp_xshock_node_descriptor xd_p_next = *xv_c->descriptor(xe_c_next);

		dbsks_xfrag_geom_model_sptr geom_model_next = this->xgraph_geom->map_edge2geom()[xe_c_next->id()];
		//recursive sample from the child, terminate at the end of the tree branch

		return sample_child_frags_and_nodes (xd_p_next, xe_c_next, xv_c_next, xgraph, geom_model_next);
	}
	else if(xv_c->degree()==3)
	{

		//Prepare inputs for the function taking xv_c as the parent node
		// locate the outgoing edge at child node
		dbsksp_xshock_edge_sptr xe_c_next1 = xgraph->cyclic_adj_succ(xe_c, xv_c); 
		dbsksp_xshock_edge_sptr xe_c_next2 = xgraph->cyclic_adj_succ(xe_c_next1, xv_c); 
		// locat the next child node of the child node
		dbsksp_xshock_node_sptr xv_c_next1 = xe_c_next1->opposite(xv_c);
		dbsksp_xshock_node_sptr xv_c_next2 = xe_c_next2->opposite(xv_c);

		// need to resample this
		dbsks_xnode_geom_model_sptr geom_model_c = this-> xgraph_geom->map_node2geom()[xv_c->id()];
		// keep x, y, r of root node. fix psi = 0, resample phi

		double min_psi, max_psi,min_radius, max_radius,min_phi, max_phi,min_phi_diff, max_phi_diff,graph_size;
		geom_model_c->get_param_range(min_psi, max_psi,min_radius, max_radius,min_phi, max_phi,min_phi_diff, max_phi_diff,graph_size);

		// resample phi of root node
		pdf1d_flat phi_diff_model;
		phi_diff_model.set(min_phi_diff, max_phi_diff);
		pdf1d_sampler* phi_diff_sampler = phi_diff_model.new_sampler();
		double phi_diff = phi_diff_sampler->sample();
		double phi_c_next1 = (xd_c.phi() + phi_diff)/2; // because here the xd_c is --->
		// update degree-3 node
		dbsksp_xgraph_algos::update_degree3_node(xv_c, xe_c, xe_c_next1, xe_c_next2, xd_c.opposite_xnode(), phi_c_next1);

		// discripter of xv_c -> xe_c_next, assuming updated
		dbsksp_xshock_node_descriptor xd_p_next1 = *xv_c->descriptor(xe_c_next1);
		dbsksp_xshock_node_descriptor xd_p_next2 = *xv_c->descriptor(xe_c_next2);
		dbsks_xfrag_geom_model_sptr geom_model_next1 = this->xgraph_geom->map_edge2geom()[xe_c_next1->id()];
		dbsks_xfrag_geom_model_sptr geom_model_next2 = this->xgraph_geom->map_edge2geom()[xe_c_next2->id()];
		//recursive sample from the child, terminate at the end of the tree branch

		return (sample_child_frags_and_nodes (xd_p_next1, xe_c_next1, xv_c_next1, xgraph, geom_model_next1) && sample_child_frags_and_nodes (xd_p_next2, xe_c_next2, xv_c_next2, xgraph, geom_model_next2));

	}
}


