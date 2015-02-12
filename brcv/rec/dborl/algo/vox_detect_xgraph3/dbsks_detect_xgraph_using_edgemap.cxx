// This is dbsks/algo/dbsks_detect_xgraph_pyramid.cxx

//:
// \file
// author Yuliang, 02/20/2014
// load in edg from .cem
// use single scale edgemap, single scale xgraph
// aim to detection in sequential frames


#include "dbsks_detect_xgraph_using_edgemap.h"

#include <dbsks/dbsks_det_desc_xgraph.h>
#include <dbsks/dbsks_det_desc_xgraph_sptr.h>
#include <dbsks/dbsks_det_nms.h>
#include <dbsks/dbsks_biarc_sampler.h>
#include <dbsks/dbsks_xshock_detector.h>
#include <dbsks/dbsks_xgraph_geom_model.h>
#include <dbsks/dbsks_xgraph_ccm_model.h>
#include <dbsks/dbsks_xshock_ccm_likelihood.h>
#include <dbsks/xio/dbsks_xio_xshock_det.h>

#include <dbsks/algo/dbsks_load.h>
#include <dbsks/algo/dbsks_vox_utils.h>
#include <dbsks/algo/dbsks_algos.h>

#include <dbsksp/dbsksp_xshock_graph.h>

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

static bool is_edges_covered_in_window (vgl_box_2d<int > window, vcl_vector<dbdet_edgel*> edgels);

//------------------------------------------------------------------------------
//:
bool dbsks_detect_xgraph_using_edgemap::
execute()
{
  vcl_cout << "\n|========================================================|"
           << "\n|   Xgraph detection using Contour-Chamfer-Matching      |"
           << "\n|========================================================|\n\n";

  //> Preliminary checks
  output_det_list.clear(); // clean up output results

  //> Load params and models
  this->load_params_and_models();

  //> Load edgemap pyramid
  this->load_edgemap_pyramid();

  //> Figure out scales of xgraph to run on
  if (!this->compute_list_model_graph_size())
  {
    return false;
  }

  //////////////////////////////////////////////////////////////////////////////////////
  // Overview of procedure: 
  // 1. We will detect objects of the specified category using all xgraph sizes.
  // 2. Then we run non-max suppression to reduce number of candidates
  // 3. At the end, we threshold to finalize the detection list
  //////////////////////////////////////////////////////////////////////////////////////

  //> List of detection records from all scales
  vcl_vector<dbsks_det_desc_xgraph_sptr > raw_dets_all_scales;

  // Iterate thru all possible sizes of prototype xgraph
//  for (unsigned i_scale =0; i_scale < xgraph_scales.size(); ++i_scale)
//  {
	unsigned i_scale = 0;
    vcl_cout
      << "\n\n-------------------------------------------------------------------"
      << "\nProcessing xgraph size = " << xgraph_scales[i_scale] << "\n\n";

    //> Name of folder to store detect records from this scale
//    vcl_string storage_foldername = "scale_" + vul_sprintf("%d", vnl_math::rnd(xgraph_scales[i_scale]));
	vcl_string storage_foldername = "base_scale";
    vcl_string storage_folder = work_folder + "/" + storage_foldername;
	vcl_string prev_storage_folder = prev_folder + "/" + storage_foldername;

    //> create the directory if not yet done
    if (!vul_file::is_directory(storage_folder))
    {
      if (!vul_file::make_directory(storage_folder))
      {
        vcl_cout << "\nERROR: cannot create work_folder to save detections.\n";
        //continue;
		return true;
      }
    }

    //> Desired graph size, compared to original image
//    double target_xgraph_size = xgraph_scales[i_scale];
    
    // Instead of using the original image and change the size of the prototype xgraph,
    // we will change the size of the image and keep the size of the prototype xgraph fixed
    double actual_pyramid_scale            = -1;  //> scale of image to keep size of xgraph fixed
    dbdet_edgemap_sptr actual_edgemap      = 0;   //> loaded edgemap corresponding to actual_pyramid_scale
    dbsksp_xshock_graph_sptr actual_xgraph = 0;   //> xgraph to use as prototype
	dbdet_sel_storage_sptr actual_sel = 0; //> cfrags to use in case

//    this->load_edgemap_in_pyramid_keeping_graph_size_fixed(target_xgraph_size,
//      actual_pyramid_scale, actual_edgemap, actual_xgraph);

	//this->load_edgemap_from_curve_fragments(actual_sel, actual_edgemap, actual_xgraph);
	if(!this ->load_initial_edgemap(actual_edgemap, actual_xgraph))
		this ->load_edgemap_singe_scale(actual_edgemap, actual_xgraph);
    //> If a scale has been processed, simply load the results back
    vcl_vector<dbsks_det_desc_xgraph_sptr > dets; //> List of detections for this scale
    vcl_vector<dbsks_det_desc_xgraph_sptr > prev_dets; //> List of detections for this scale in prev frame

    if (!dbsks_load_detections_from_folder(prev_storage_folder, prev_dets))
		vcl_cout << "\n [Fail] loading previous dets \n";

    if (dbsks_load_detections_from_folder(storage_folder, dets))
    {
      vcl_cout << "\nThis image has been processed. All records are loaded back.\n";
	  return true;
    }
    else
    {
      //> Detect xgraph with the specified scale
      dets.clear();
      this->run_detection_on(actual_edgemap, actual_xgraph, min_accepted_confidence, 
        storage_folder, dets, prev_dets);

      //> Dump the detections to a folder for backing up
      vcl_cout << "\n> Saving detections of selected scale to dump folder = " 
        << storage_folder << "\n";
      {
        // form a unique id for this group of detections
        vcl_string xgraph_name = vul_file::strip_extension(vul_file::strip_directory(xgraph_file));
        vcl_string det_group_id = xgraph_name + "+" + object_id + "+" + storage_foldername;
        vcl_string model_category = "";

        // create a binary image from the edgemap
        vil_image_view<vxl_byte > bg_view;
        dbsks_detect_xgraph_using_edgemap::convert_edgemap_to_bw(actual_edgemap, bg_view);
        
		//vcl_sort(dets.begin(), dets.end(), dbsks_decreasing_confidence);
        // save detection to disk
        dbsks_save_detections_to_folder(dets, object_id, model_category, 
          det_group_id, bg_view, storage_folder, "");
      }
    } // if exist
    

    // scale the detection back to original size of the image
    for (unsigned m =0; m < dets.size(); ++m)
    {
      //dets[m]->xgraph()->scale_up(0, 0, 1.0 / actual_pyramid_scale);
      dets[m]->compute_bbox();
    }


    // Some report on results
    vcl_cout 
      << "> Detection results for selected scale:\n"
      //<< "\n  xgraph size:   " << target_xgraph_size
      << "\n  # detections:  " << dets.size() << "\n";

    // Save detections to the global list
    raw_dets_all_scales.insert(raw_dets_all_scales.end(), dets.begin(), dets.end());
    vcl_cout
      << "\n-------------------------------------------------------------------\n";
//  }

  //> Summarize detections for all scales
  vcl_cout << "\n> Total # raw detections across all scales: " << raw_dets_all_scales.size() << "\n";

  //> Non-max suppression across detection results form different scales
  if (run_nms_based_on_overlap)
  {
    vcl_cout 
      << "\n> Running non-max-suppression based on overlap ..."
      << "\n    min_overlap_ratio_for_rejection: " << min_overlap_ratio_for_rejection << "\n";
    dbsks_det_nms_using_polygon(raw_dets_all_scales, this->output_det_list, min_overlap_ratio_for_rejection);
  }
  else
  {
    vcl_cout << "\n> NO non-max-suppression. All detections are accepted.\n";
    this->output_det_list = raw_dets_all_scales;
  }

  vcl_cout << "\n> Final # detections = " << this->output_det_list.size() 
    << "\n------------------------------------------------------------------\n";

  return true;
}


//------------------------------------------------------------------------------
//: Load all data from input files
bool dbsks_detect_xgraph_using_edgemap::
load_params_and_models()
{
  // Load data from the files

  // image
  vil_image_resource_sptr image_resource;
  if (!dbsks_load_image_resource(image_file, image_resource))
  {
    return false;
  }
  source_image = *vil_convert_cast(vxl_byte(), image_resource->get_view());

  // prev image
	vil_image_resource_sptr prev_image_resource;
	if(std::ifstream(prev_image_file.c_str()))
	{
		dbsks_load_image_resource(prev_image_file, prev_image_resource);
  		prev_source_image = *vil_convert_cast(vxl_byte(), prev_image_resource->get_view()); 
	}
	else
	{
		vcl_cout << "No Prev Image Found...\n";
	}	



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

  // geometric model L
  vcl_string xgraph_geom_file_L = xgraph_geom_file.substr(0, xgraph_geom_file.size()-4) + "-left.xml";
  if (!dbsks_load_xgraph_geom_model(xgraph_geom_file_L, xgraph_geom_param_file, xgraph_geom_L))
  {
    return false;
  }
  xgraph_geom_L->compute_attribute_constraints();

  // geometric model R
  vcl_string xgraph_geom_file_R = xgraph_geom_file.substr(0, xgraph_geom_file.size()-4) + "-right.xml";
  if (!dbsks_load_xgraph_geom_model(xgraph_geom_file_R, xgraph_geom_param_file, xgraph_geom_R))
  {
    return false;
  }
  xgraph_geom_R->compute_attribute_constraints();

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

  // ccm model
  dbsks_load_xgraph_ccm_model(xgraph_ccm_file, xgraph_ccm_param_file, xgraph_ccm);

  // Check compatibility between Contour Chamfer Matching model and xgraph (are all edges covered)
  vcl_cout << ">> Checking compatibility between CCM model and xgraph...";
  if (!xgraph_ccm->is_compatible(xgraph_prototype_))
  {
    vcl_cout << "Failed\n." << vcl_endl;
    return false;
  }
  else
  {
    vcl_cout << "Passed\n." << vcl_endl;
  }

  // Set distributions of user-selected boundary fragments to constant
  vcl_cout << "\n>> Overriding 'ignored' edges with constant distribution...";
  if (!xgraph_ccm->override_cfrag_with_constant_distribution(cfrag_list_to_ignore))
  {
    vcl_cout << "[ Failed ]\n";
    return false;
  }
  else
  {
    vcl_cout << "[ OK ]\n";
  }

  //: Compute cache values for ccm models
  xgraph_ccm->compute_cache_loglike_for_all_edges();

  //: Construct biarc sampler from xgraph_ccm model
  xgraph_ccm->build_biarc_sampler(this->biarc_sampler);
  this->biarc_sampler.compute_cache_sample_points();
  this->biarc_sampler.compute_cache_nkdiff();
  

  vcl_cout << "\n>> loading prototype appearance model...";
  if(!load_appearance_model())
  {
    vcl_cout << "[ Failed ]\n";
    return false;	
  }	
  else
  {
    vcl_cout << "[ OK ]\n";
  }

  return true;
}

bool dbsks_detect_xgraph_using_edgemap::
load_bb_file()
{
	vcl_ifstream myfile (this->bb_file.c_str());
	vcl_cout << this->xgraph_appearance_file << vcl_endl;
	vcl_string line;
	if (myfile.is_open())
	{
		getline (myfile,line);
		vcl_istringstream is( line );
   		double n;
   		while( is >> n ) 
		{
			bb_coordinates.push_back(int(n));
		}
		myfile.close();
	}
	else
	{ 
		vcl_cout << "Unable to open BB file" <<vcl_endl;
		return false;
	}
	return true;
}

bool dbsks_detect_xgraph_using_edgemap::
load_appearance_model()
{
	vcl_vector<int> ids;
    vcl_vector<int> values;
	vcl_ifstream myfile (this->xgraph_appearance_file.c_str());
	vcl_cout << this->xgraph_appearance_file << vcl_endl;
	vcl_string line;
	if (myfile.is_open())
	{
		getline (myfile,line);
		vcl_istringstream is( line );
   		double n;
   		while( is >> n ) 
		{
			ids.push_back(int(n));
		}

		getline (myfile,line);
		vcl_istringstream is_2( line );
   		while( is_2 >> n ) 
		{
			values.push_back(int(n));
		}
		
		myfile.close();
		for(int i =1; i< ids.size(); i++)
		{
			if(values[i]!=0)
			{
				this->appearance_model_node_id.push_back(ids[i]);
				this->appearance_model_node_value.push_back(values[i]);
				vcl_cout << " id: " << ids[i] << " value: " << values[i];
			}
		}
		vcl_cout << vcl_endl;
	}
  	else
	{ 
		vcl_cout << "Unable to open appearance model file" <<vcl_endl;
		return false;
	}
	return true;
}



// local data structure to facilitate sorting the edgemap by their width
struct edgemap_level_info
{
  vcl_string base_name;
  int width;
  double scale;
};


//------------------------------------------------------------------------------
//: Load edge map pyramid
bool dbsks_detect_xgraph_using_edgemap::
load_edgemap_pyramid()
{
  //>> Load all edgemaps in the pyramid ........................................
  vcl_cout << "\n>> Loading all edgemap images in the pyramid ... ";

  // regular expression to iterate thru edgemap files
  vcl_string edgemap_regexp = edgemap_folder + "/" + object_id +  "/" + object_id + "*" + edgemap_ext;
  
  // clean up any existing data
  this->list_edgemap_base_name.clear();
  this->list_edgemap_width.clear();
  this->list_edgemap_scale.clear();
  double image_width = this->source_image.ni();

  

  // sort the edgemaps by their width, decreasing order
  vcl_map<int, edgemap_level_info> map_width2info;
  for (vul_file_iterator fn = edgemap_regexp; fn; ++fn)
  {
    vil_image_resource_sptr img = vil_load_image_resource(fn());
    if (img)
    {
      edgemap_level_info info;
      info.width = int(img->ni());
      info.scale = img->ni() / image_width;
      // base name: computed by taking away the "edgemap_ext" part of file name
      // note that "vul_file::strip_extension(...) will not work because 
      // "edgemap_ext" may containt a dot ".", which will confuse the 
      // vul_file::strip_extension(...) function
      vcl_string fname = vul_file::strip_directory(fn());
      info.base_name = fname.substr(0, fname.size()-edgemap_ext.size());
      map_width2info.insert(vcl_make_pair(-info.width, info));
    }
  }

  // put the info back in the form we're familiar with
  for (vcl_map<int, edgemap_level_info>::iterator iter = map_width2info.begin();
    iter != map_width2info.end(); ++iter)
  {
    edgemap_level_info info = iter->second;
    this->list_edgemap_width.push_back(info.width);
    this->list_edgemap_scale.push_back(info.scale);
    this->list_edgemap_base_name.push_back(info.base_name); 
  }
  return true;
}


//------------------------------------------------------------------------------
//: Compute sizes of model xgraph to run
bool dbsks_detect_xgraph_using_edgemap::
compute_list_model_graph_size()
{
  // Preliminary checks
  if (
    //!(this->prototype_xgraph_base_size > 0) ||
    !(this->prototype_xgraph_min_size > 0) ||
    !(this->prototype_xgraph_min_size <= this->prototype_xgraph_max_size) ||
    !(this->prototype_xgraph_log2_increment_step > 0) ||
    !(this->prototype_xgraph_ratio_max_size_to_min_size >= 1))
  {
    vcl_cout << "\nERROR: Invalid graph size parameters.\n";
    return false;
  }

  //>> Compute list of xgraph scales to detect
  double image_width = this->source_image.ni();
  double image_height = this->source_image.nj();
  double image_size = vcl_sqrt(image_width * image_height);

  // Maximum xgraph scale is bounded above by image size
  this->prototype_xgraph_max_size = vnl_math::min(image_size, this->prototype_xgraph_max_size);

  xgraph_scales.clear();
  for (double s = this->prototype_xgraph_min_size; s <= this->prototype_xgraph_max_size; 
    s *= vcl_pow(2, this->prototype_xgraph_log2_increment_step))
  {
    xgraph_scales.push_back(vnl_math::rnd(s)); // rounding simply for good-looking numbers
  }

  // reverse the order - process large size first
  vcl_reverse(xgraph_scales.begin(), xgraph_scales.end());

  // Remove graph size that are too small (compared to the maximum size)
  double min_allowed_xgraph_size = xgraph_scales.front() / this->prototype_xgraph_ratio_max_size_to_min_size;
  while (!xgraph_scales.empty() && xgraph_scales.back() < min_allowed_xgraph_size)
  {
    xgraph_scales.pop_back();
  }

  

  
  //////////////////////////////////////////////////////////////////////////////
  //>> Detect xgraphs ..........................................................
  vcl_cout << "\n>> Computing the xgraph scales to run.\n";
  vcl_cout << "\n   - Image size (W x H) = " << image_width << " x " << image_height 
    << "\n   - Image scale (sqrt of area) = " << image_size
    << "\n   - Min xgraph scale = " << this->prototype_xgraph_min_size
    << "\n   - Log2 of scale step = " << this->prototype_xgraph_log2_increment_step
    << "\n   - Max xgraph scale = " << this->prototype_xgraph_max_size
    << "\n   - List of scales = [ ";
  for (unsigned i =0; i < xgraph_scales.size(); ++i)
  {
    vcl_cout << xgraph_scales[i] << ", ";
  }
  vcl_cout << "]\n";

  if (xgraph_scales.empty())
  {
    vcl_cout << "\nERROR: There is no xgraph scale. Please check params again.\n";
    return false;
  }
  return true;
}


bool dbsks_detect_xgraph_using_edgemap::
load_edgemap_from_curve_fragments(  dbdet_sel_storage_sptr& actual_sel,
									dbdet_edgemap_sptr& actual_edgemap,
									dbsksp_xshock_graph_sptr& actual_xgraph)
{
    vcl_cout<<"************ Load CEM file ************"<<vcl_endl;
    vcl_string cem_file_1;
    
    //params->output_image_extension_() == ".bmp";
    cem_file_1 = edgemap_folder + "/" + object_id + "/" + object_id + input_cemv_extension;

    bpro1_filepath cem_path_1(cem_file_1, ".cem");
    vcl_vector<bpro1_storage_sptr> load_cem_results_1;
    dbdet_load_cem_process load_cem_pro_1;
    load_cem_pro_1.parameters()->set_value("-cem_filename",cem_path_1);
    // Before we start the process lets clean input output
    load_cem_pro_1.clear_input();
    load_cem_pro_1.clear_output();

    // Kick of process
    bool load_cem_status_1 = load_cem_pro_1.execute();
    load_cem_pro_1.finish();

	if ( load_cem_status_1 )
	{
	    load_cem_results_1 = load_cem_pro_1.get_output();
	}

    //Clean up after ourselves
    load_cem_pro_1.clear_input();

    if ( !load_cem_status_1 )
    {
        vcl_cerr << "Problems in loading GT cem file" 
                 << cem_file_1 << vcl_endl;
        return 1;

    }

    // first input the GT contours
    actual_sel.vertical_cast(load_cem_results_1[1]);
    actual_edgemap = actual_sel->EM();
    //dbdet_curve_fragment_graph& CFG_0 = actual_sel->CFG();

    actual_xgraph = new dbsksp_xshock_graph(*(this->xgraph_prototype_));
	//double cur_xgraph_size = vcl_sqrt(actual_xgraph->area());
	//actual_xgraph->scale_up(0, 0, target_xgraph_size / cur_xgraph_size);
    return true;
}

bool dbsks_detect_xgraph_using_edgemap::
load_edgemap_singe_scale(dbdet_edgemap_sptr& actual_edgemap,dbsksp_xshock_graph_sptr& actual_xgraph)
{
	vcl_cout<<"************ Load Edge Map Single Scale ************"<<vcl_endl;
	vcl_string edgemap_fname = object_id + edgemap_ext;
	vcl_string edgemap_file = edgemap_folder + "/" + object_id + "/" + edgemap_fname;

	// Name of edge orientation file
	vcl_string edgeorient_fname = object_id + edgeorient_ext;
	vcl_string edgeorient_file = edgemap_folder + "/" + object_id + "/" + edgeorient_fname;

	// Load the edgel map/////////////////////////////////////////////////////////
	actual_edgemap = dbsks_load_subpix_edgemap(edgemap_file, edgeorient_file, 15.0f, 255.0f);

    actual_xgraph = new dbsksp_xshock_graph(*(this->xgraph_prototype_));
	//double cur_xgraph_size = vcl_sqrt(actual_xgraph->area());
	//actual_xgraph->scale_up(0, 0, target_xgraph_size / cur_xgraph_size);
    return true;
}

bool dbsks_detect_xgraph_using_edgemap::
load_initial_edgemap(dbdet_edgemap_sptr& actual_edgemap,dbsksp_xshock_graph_sptr& actual_xgraph)
{
	vcl_cout<<"************ Load Initial Frame Edge Map ************"<<vcl_endl;
	vcl_string edgemap_fname = object_id + edgemap_ext;
	vcl_string edgemap_file = initial_edge_folder + "/"  + edgemap_fname;
	if(!std::ifstream(edgemap_file.c_str()))
		return false;

	// Name of edge orientation file
	vcl_string edgeorient_fname = object_id + edgeorient_ext;
	vcl_string edgeorient_file = initial_edge_folder + "/" + edgeorient_fname;

	if(!std::ifstream(edgeorient_file.c_str()))
		return false;
	// Load the edgel map/////////////////////////////////////////////////////////
	actual_edgemap = dbsks_load_subpix_edgemap(edgemap_file, edgeorient_file, 15.0f, 255.0f);

    actual_xgraph = new dbsksp_xshock_graph(*(this->xgraph_prototype_));
	//double cur_xgraph_size = vcl_sqrt(actual_xgraph->area());
	//actual_xgraph->scale_up(0, 0, target_xgraph_size / cur_xgraph_size);

	return true;
}
//------------------------------------------------------------------------------
//: Load edgemap corresponding to a target xgraph size to that the actual
// xgraph size is about the same as the base xgraph size
bool dbsks_detect_xgraph_using_edgemap::
load_edgemap_in_pyramid_keeping_graph_size_fixed(double target_xgraph_size, 
                        double& actual_pyramid_scale,
                        dbdet_edgemap_sptr& actual_edgemap,
                        dbsksp_xshock_graph_sptr& actual_xgraph)
{
  // Compute image scale (compared to base image) such that we can maintain the same graph/image ratio
  // while keeping the graph scale at the base level
  double target_pyramid_scale = this->xgraph_ccm->base_xgraph_size() / target_xgraph_size; // 

  // Set the actual scale to be the target scale
  // note that this may result in enlarging the edgemap when the target scale > 1
  actual_pyramid_scale = target_pyramid_scale;

  unsigned actual_edgemap_level;
  double actual_edgemap_scale;
  {
    // scale of the "edgemap" which is different from the "image" scale by "edgemap_log2_scale_ratio"
    double scale_ratio = vcl_pow(2.0, this->edgemap_log2_scale_ratio);
    double target_edgemap_scale = target_pyramid_scale * scale_ratio;

    // find the edgemap closest to this scale
    vnl_vector<double > scale_diff(this->list_edgemap_scale.size(), vnl_numeric_traits<double >::maxval);
    for (unsigned k =0; k < scale_diff.size(); ++k)
    {
      scale_diff[k] = vnl_math::abs(vcl_log(this->list_edgemap_scale[k] / target_edgemap_scale));
    }
    actual_edgemap_level = scale_diff.arg_min();
    actual_edgemap_scale = this->list_edgemap_scale[actual_edgemap_level];
    if (actual_edgemap_scale == vnl_numeric_traits<double >::maxval)
    {
      return false;
    }
  }

  // base-name for edge-related files
  vcl_string base_name = this->list_edgemap_base_name[actual_edgemap_level];

  // edgemap file
  vcl_string edgemap_fname = base_name + edgemap_ext;
  vcl_string edgemap_file = edgemap_folder + "/" + object_id + "/" + edgemap_fname;

  // Name of edge orientation file
  vcl_string edgeorient_fname = base_name + edgeorient_ext;
  vcl_string edgeorient_file = edgemap_folder + "/" + object_id + "/" + edgeorient_fname;

  // Load the edgel map/////////////////////////////////////////////////////////
  actual_edgemap = dbsks_load_subpix_edgemap(edgemap_file, edgeorient_file, 15.0f, 255.0f);

  // re-scale the edgemap to the level of image in the pyramid
  dbdet_resize_edgemap(actual_edgemap, actual_pyramid_scale / actual_edgemap_scale);
  //////////////////////////////////////////////////////////////////////////////
    
  //> Adjust the size of the model xgraph

  // Because the edgemap pyramid is discrete, the actual loaded edgemap does
  // not neccessarily have the same scale as the target scale. Need to make
  // adjustment on the size of the xgraph:

  // Scale the xgraph up (or down) so that: 
  //   size of adjusted xgraph                target_xgraph_size 
  // ------------------------------- = ---------------------------
  //  size of actual pyramid image            base image (=1)

  //////////////////////////////////////////////////////////////////////////////
  actual_xgraph = new dbsksp_xshock_graph(*(this->xgraph_prototype_));
  double adjusted_xgraph_size = target_xgraph_size * actual_pyramid_scale;
  double cur_xgraph_size = vcl_sqrt(actual_xgraph->area());
  actual_xgraph->scale_up(0, 0, adjusted_xgraph_size / cur_xgraph_size);
  //////////////////////////////////////////////////////////////////////////////

  return true;
}





//------------------------------------------------------------------------------
//: Perform detection on a specific edgemap
bool dbsks_detect_xgraph_using_edgemap::
run_detection_on(const dbdet_edgemap_sptr& edgemap, 
    const dbsksp_xshock_graph_sptr& xgraph,
    double confidence_lower_threshold,
    const vcl_string& work_dir,
    vcl_vector<dbsks_det_desc_xgraph_sptr >& dets,
    vcl_vector<dbsks_det_desc_xgraph_sptr > prev_dets)
{
  vul_timer timer;
  timer.mark();

  //> Contour-Chamfer-Matching cost function...................................
  vcl_cout << "\n> Constructing a likelihood function based on CCM cost ...";

  dbsks_xshock_ccm_likelihood ccm_like;
  ccm_like.set_edgemap(edgemap);
  ccm_like.set_biarc_sampler(&this->biarc_sampler);
  ccm_like.set_ccm_model(xgraph_ccm);
  vcl_cout << " [ OK ]\n";


  
  vcl_vector<vgl_box_2d<int > > windows; // list of detection windows

  if(prev_dets.empty())
  {

	  load_bb_file();

	  //> Compute window (rectangular boxes) from input bounding box
	  vcl_cout << "\n> Computing (rectangular) window from input bounding box...";
	  //vcl_vector<vgl_box_2d<int > > all_windows;
	  //dbsks_algos::compute_detection_windows(det_window_width, det_window_height, edgemap->ncols(), edgemap->nrows(), all_windows);
	  // Print out list of windows
	  vcl_cout << "\n> detect window: \n";

		vgl_box_2d<int > window(bb_coordinates[0], bb_coordinates[2], bb_coordinates[1], bb_coordinates[3]);
		vcl_cout
		  << "  [xmin ymin xmax ymax] = "
		  << "[" << window.min_x() 
		  << " " << window.min_y() 
		  << " " << window.max_x()
		  << " " << window.max_y() << "]\n";
		windows.push_back(window);
	  
	  vcl_cout << " [ OK ]\n";
	  vcl_cout.flush();

  }
  else
  {

	// allocate window conners based on best prev dets
	  int w_min_x = edgemap->ncols()-1;
	  int w_max_x = 0;
	  int w_min_y = edgemap->nrows()-1;
	  int w_max_y = 0;

	  for (int id = 0; id< vcl_min(int(prev_dets.size()), 5); id++)
	  {
		if(prev_dets[id]->bbox()->get_min_x() < w_min_x)
			w_min_x = prev_dets[id]->bbox()->get_min_x();
		if(prev_dets[id]->bbox()->get_max_x() > w_max_x)
			w_max_x = prev_dets[id]->bbox()->get_max_x();
		if(prev_dets[id]->bbox()->get_min_y() < w_min_y)
			w_min_y = prev_dets[id]->bbox()->get_min_y();
		if(prev_dets[id]->bbox()->get_max_y() > w_max_y)
			w_max_y = prev_dets[id]->bbox()->get_max_y();
	  }
  	  vcl_cout << w_min_x << " " << w_max_x << " " << w_min_y << " " << w_max_y << vcl_endl;
	  vgl_box_2d<int> wd_0(vcl_max(w_min_x-40, 0), vcl_min(w_max_x+40, int(edgemap->ncols()-1)), vcl_max(0, w_min_y-40), vcl_min(int(edgemap->nrows()-1),w_max_y+40));

	  // just save this window.
	  windows.push_back(wd_0);
  }


  //>> Detect objects within each window
  vcl_cout << "\n> Detecting objects in all computed windows ...";
  vcl_vector<dbsks_det_desc_xgraph_sptr > raw_dets_all_windows;
  raw_dets_all_windows.clear();

  for (unsigned iw =0; iw < windows.size(); ++iw)
  {
    vgl_box_2d<int > window = windows[iw];
    vcl_cout 
      << "\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
      << "\n> Window index = " << iw 
      << "\n    [xmin ymin xmax ymax] = "
      << "[" << window.min_x() 
      << " " << window.min_y() 
      << " " << window.max_x()
      << " " << window.max_y() << "]\n";

    vcl_vector<dbsks_det_desc_xgraph_sptr > dets_window;

    //> Compute ccm for a region of interest only
    bool cid_status = ccm_like.compute_internal_data(window);
    if(!cid_status)
    {
        vcl_cout << "CCM Likelihood Error in this window. Skip..." << vcl_endl;
        continue;
    }

	bool is_initial = false;

	vil_image_view<vxl_byte> L_, prev_L_;
	vil_convert_planes_to_grey(this->source_image, L_);

	// Pipeline Update: if no previous dets, do window detection in a coarse grid search first. Refine the coarse results in the next steps
	if(prev_dets.empty())
	{
		vil_convert_planes_to_grey(this->source_image, prev_L_);

		is_initial = true;
		// xshock detection engine
		dbsks_xshock_detector engine;
		engine.generate_xnode_grid_option = 3; // use bounding box window
		engine.xshock_likelihood_ = &ccm_like;
		engine.xgraph_geom_ = xgraph_geom;
		engine.set_xgraph(xgraph);
		//engine.compute_vertices_para_range();

		//--------------------------------------------------------------------------
		engine.detect(window, float(confidence_lower_threshold));
		//--------------------------------------------------------------------------

		if(engine.list_solutions_.size()!=0)
		{
			//> Construct a vector of detection descriptor
			dets_window.clear();
			dets_window.reserve(engine.list_solutions_.size());

			//if(!update_appearance_model(prev_dets, prev_L_))
			//	vcl_cout<< "Fail in updaing appearance model" << vcl_endl;

			for (unsigned i =0; i < engine.list_solutions_.size(); ++i)
			{
				dbsksp_xshock_graph_sptr sol_xgraph = engine.list_solutions_[i];

				double confidence = -engine.list_solution_costs_[i];
				double real_confidence = -engine.list_solution_real_costs_[i];

				// for detection with enough edge support, add appearance confidence into it.
				// edge matching cost term
				vcl_cout <<"\nedge_confidence:" << real_confidence << vcl_endl;		

				// appearance term
				//double appearance_cost = compute_appearance_cost(sol_xgraph, L_);
				//vcl_cout <<"\nappearance_cost:" << appearance_cost << vcl_endl;

				//real_confidence += (20 - appearance_cost);
				vcl_cout << " real confidence: " << real_confidence << vcl_endl;
				dbsks_det_desc_xgraph_sptr det = new dbsks_det_desc_xgraph(sol_xgraph, real_confidence );
				det->compute_bbox();
				dets_window.push_back(det);
			}
			// add detections from this window to the overall list
			//raw_dets_all_windows.insert(raw_dets_all_windows.end(), dets_window.begin(), dets_window.end());
			prev_dets.insert(prev_dets.end(), dets_window.begin(), dets_window.end());
	  		vcl_sort(prev_dets.begin(), prev_dets.end(), dbsks_decreasing_confidence);		
		}
		
		if(prev_dets.size()==0)
		{
		//////////////////////////////// detect based on geom model left  ///////////////////////////////////////////////////////////////
			vcl_cout << "\n Detecting based on Left-Turnning Geom Model" << vcl_endl;
			engine.xgraph_geom_ = xgraph_geom_L;

			//--------------------------------------------------------------------------
			engine.detect(window, float(confidence_lower_threshold));

			if(engine.list_solutions_.size()!=0)
			{
				//> Construct a vector of detection descriptor
				dets_window.clear();
				dets_window.reserve(engine.list_solutions_.size());


				//if(!update_appearance_model(prev_dets, prev_L_))
				//	vcl_cout<< "Fail in updaing appearance model" << vcl_endl;

				for (unsigned i =0; i < engine.list_solutions_.size(); ++i)
				{
					dbsksp_xshock_graph_sptr sol_xgraph = engine.list_solutions_[i];

					double confidence = -engine.list_solution_costs_[i];
					double real_confidence = -engine.list_solution_real_costs_[i];

					// for detection with enough edge support, add appearance confidence into it.
					// edge matching cost term
					vcl_cout <<"\nedge_confidence:" << real_confidence << vcl_endl;		

					// appearance term
					//double appearance_cost = compute_appearance_cost(sol_xgraph, L_);
					//vcl_cout <<"\nappearance_cost:" << appearance_cost << vcl_endl;

					//real_confidence += (20 - appearance_cost);
					vcl_cout << " real confidence: " << real_confidence << vcl_endl;
					dbsks_det_desc_xgraph_sptr det = new dbsks_det_desc_xgraph(sol_xgraph, real_confidence );
					det->compute_bbox();
					dets_window.push_back(det);
				}
				// add detections from this window to the overall list
				//raw_dets_all_windows.insert(raw_dets_all_windows.end(), dets_window.begin(), dets_window.end());
				prev_dets.insert(prev_dets.end(), dets_window.begin(), dets_window.end());
		  		vcl_sort(prev_dets.begin(), prev_dets.end(), dbsks_decreasing_confidence);		
			}
		}

		if(prev_dets.size()==0)
		{
		//////////////////////////////// detect based on geom model right  ///////////////////////////////////////////////////////////////
			vcl_cout << "\n Detecting based on Right-Turnning Geom Model" << vcl_endl;
			engine.xgraph_geom_ = xgraph_geom_R;

			//--------------------------------------------------------------------------
			engine.detect(window, float(confidence_lower_threshold));

			if(engine.list_solutions_.size()!=0)
			{
				//> Construct a vector of detection descriptor
				dets_window.clear();
				dets_window.reserve(engine.list_solutions_.size());

				//if(!update_appearance_model(prev_dets, prev_L_))
				//	vcl_cout<< "Fail in updaing appearance model" << vcl_endl;

				for (unsigned i =0; i < engine.list_solutions_.size(); ++i)
				{
					dbsksp_xshock_graph_sptr sol_xgraph = engine.list_solutions_[i];

					double confidence = -engine.list_solution_costs_[i];
					double real_confidence = -engine.list_solution_real_costs_[i];

					// for detection with enough edge support, add appearance confidence into it.
					// edge matching cost term
					vcl_cout <<"\nedge_confidence:" << real_confidence << vcl_endl;		

					// appearance term
					//double appearance_cost = compute_appearance_cost(sol_xgraph, L_);
					//vcl_cout <<"\nappearance_cost:" << appearance_cost << vcl_endl;

					//real_confidence += (20 - appearance_cost);
					vcl_cout << " real confidence: " << real_confidence << vcl_endl;
					dbsks_det_desc_xgraph_sptr det = new dbsks_det_desc_xgraph(sol_xgraph, real_confidence );
					det->compute_bbox();
					dets_window.push_back(det);
				}
				// add detections from this window to the overall list
				//raw_dets_all_windows.insert(raw_dets_all_windows.end(), dets_window.begin(), dets_window.end());
				prev_dets.insert(prev_dets.end(), dets_window.begin(), dets_window.end());
		  		vcl_sort(prev_dets.begin(), prev_dets.end(), dbsks_decreasing_confidence);		
			}
		}
	}
	else
	{
		vil_convert_planes_to_grey(this->prev_source_image, prev_L_);
	}

		for(int prev_i = 0; prev_i < vcl_min(int(prev_dets.size()), 2); prev_i ++)
		{


		// xshock detection engine
			dbsks_xshock_detector engine;
			engine.generate_xnode_grid_option = 4; // use prev dets window
			engine.xshock_likelihood_ = &ccm_like;
			engine.xgraph_geom_ = xgraph_geom;
			engine.set_xgraph(xgraph);
			//engine.prev_dets_ = prev_dets;
			engine.prev_dets_.push_back(prev_dets[prev_i]);
			engine.compute_vertices_para_range();

			vcl_cout << "\n Detecting based on Geom Model" << vcl_endl;
			//--------------------------------------------------------------------------
			engine.detect(window, float(confidence_lower_threshold));
			//--------------------------------------------------------------------------

			//> Construct a vector of detection descriptor
			dets_window.clear();
			dets_window.reserve(engine.list_solutions_.size());


			//if(!update_appearance_model(prev_dets, prev_L_))
			//	vcl_cout<< "Fail in updaing appearance model" << vcl_endl;
			if(engine.list_solutions_.size()==0)
				dets_window.push_back(prev_dets[prev_i]);
			else
			{
				for (unsigned i =0; i < engine.list_solutions_.size(); ++i)
				{
					dbsksp_xshock_graph_sptr sol_xgraph = engine.list_solutions_[i];

					double confidence = -engine.list_solution_costs_[i];
					double real_confidence = -engine.list_solution_real_costs_[i];

					// for detection with enough edge support, add appearance confidence into it.
					// edge matching cost term
					vcl_cout <<"\nedge_confidence:" << real_confidence << vcl_endl;		

					// appearance term
					double appearance_cost = compute_appearance_cost_v2(sol_xgraph, L_, prev_dets[prev_i]->xgraph(), prev_L_);
					vcl_cout <<"appearance_cost:" << appearance_cost << vcl_endl;

					// shape change term, now just the differece between radius of shock nodes, but shape is more complex representation
					double shape_trans_cost = compute_shape_trans_cost(sol_xgraph, prev_dets[prev_i]->xgraph());
					vcl_cout <<"shape_cost:" << shape_trans_cost << vcl_endl;

					//(TODO: change to differece between velocity, delta_x, delta_y, delta_psi)

					// only consider the dets which is not too diff in bg as prototype
					real_confidence += (20 - appearance_cost);
					if(!is_initial)
						real_confidence += (10 - shape_trans_cost); 
					vcl_cout << " real confidence: " << real_confidence << vcl_endl;
					dbsks_det_desc_xgraph_sptr det = new dbsks_det_desc_xgraph(sol_xgraph, real_confidence );
					det->compute_bbox();
					dets_window.push_back(det);
				}
			}
			// add detections from this window to the overall list
			raw_dets_all_windows.insert(raw_dets_all_windows.end(), dets_window.begin(), dets_window.end());
  			vcl_sort(raw_dets_all_windows.begin(), raw_dets_all_windows.end(), dbsks_decreasing_confidence);

			if(!is_initial && raw_dets_all_windows.front()->confidence()<40)
			{
				//////////////////////////////// detect based on geom model left  ///////////////////////////////////////////////////////////////
				vcl_cout << "\n Detecting based on Left-Turnning Geom Model" << vcl_endl;
				engine.xgraph_geom_ = xgraph_geom_L;

				//--------------------------------------------------------------------------
				engine.detect(window, float(confidence_lower_threshold));
				//--------------------------------------------------------------------------

				//> Construct a vector of detection descriptor
				dets_window.clear();
				dets_window.reserve(engine.list_solutions_.size());

				//if(!update_appearance_model(prev_dets, prev_L_))
				//	vcl_cout<< "Fail in updaing appearance model" << vcl_endl;
				if(engine.list_solutions_.size()!=0)
				{
					for (unsigned i =0; i < engine.list_solutions_.size(); ++i)
					{
						dbsksp_xshock_graph_sptr sol_xgraph = engine.list_solutions_[i];

						double confidence = -engine.list_solution_costs_[i];
						double real_confidence = -engine.list_solution_real_costs_[i];

						// for detection with enough edge support, add appearance confidence into it.
						// edge matching cost term
						vcl_cout <<"\nedge_confidence:" << real_confidence << vcl_endl;		

						// appearance term
						double appearance_cost = compute_appearance_cost_v2(sol_xgraph, L_, prev_dets[prev_i]->xgraph(), prev_L_);
						vcl_cout <<"appearance_cost:" << appearance_cost << vcl_endl;

						// shape change term, now just the differece between radius of shock nodes, but shape is more complex representation
						double shape_trans_cost = compute_shape_trans_cost(sol_xgraph, prev_dets[prev_i]->xgraph());
						vcl_cout <<"shape_cost:" << shape_trans_cost << vcl_endl;

						//(TODO: change to differece between velocity, delta_x, delta_y, delta_psi)

						// only consider the dets which is not too diff in bg as prototype
						real_confidence += (20 - appearance_cost);
						if(!is_initial)
							real_confidence += (10 - shape_trans_cost);
						real_confidence *= 0.9;
						vcl_cout << " real confidence: " << real_confidence << vcl_endl;
						dbsks_det_desc_xgraph_sptr det = new dbsks_det_desc_xgraph(sol_xgraph, real_confidence );
						det->compute_bbox();
						dets_window.push_back(det);
					}
					raw_dets_all_windows.insert(raw_dets_all_windows.end(), dets_window.begin(), dets_window.end());
		  			vcl_sort(raw_dets_all_windows.begin(), raw_dets_all_windows.end(), dbsks_decreasing_confidence);
				}
			}


			if(!is_initial && raw_dets_all_windows.front()->confidence()<36)
			{
				//////////////////// detect based on geom model right  //////////////////////////////////////////

				vcl_cout << "\n Detecting based on Right-Turnning Geom Model" << vcl_endl;
				engine.xgraph_geom_ = xgraph_geom_R;

				//--------------------------------------------------------------------------
				engine.detect(window, float(confidence_lower_threshold));
				//--------------------------------------------------------------------------

				//> Construct a vector of detection descriptor
				dets_window.clear();
				dets_window.reserve(engine.list_solutions_.size());


				//if(!update_appearance_model(prev_dets, prev_L_))
				//	vcl_cout<< "Fail in updaing appearance model" << vcl_endl;
				if(engine.list_solutions_.size()!=0)
				{
					for (unsigned i =0; i < engine.list_solutions_.size(); ++i)
					{
						dbsksp_xshock_graph_sptr sol_xgraph = engine.list_solutions_[i];

						double confidence = -engine.list_solution_costs_[i];
						double real_confidence = -engine.list_solution_real_costs_[i];

						// for detection with enough edge support, add appearance confidence into it.
						// edge matching cost term
						vcl_cout <<"\nedge_confidence:" << real_confidence << vcl_endl;		

						// appearance term
						double appearance_cost = compute_appearance_cost_v2(sol_xgraph, L_, prev_dets[prev_i]->xgraph(), prev_L_);
						vcl_cout <<"appearance_cost:" << appearance_cost << vcl_endl;

						// shape change term, now just the differece between radius of shock nodes, but shape is more complex representation
						double shape_trans_cost = compute_shape_trans_cost(sol_xgraph, prev_dets[prev_i]->xgraph());
						vcl_cout <<"shape_cost:" << shape_trans_cost << vcl_endl;

						//(TODO: change to differece between velocity, delta_x, delta_y, delta_psi)

						// only consider the dets which is not too diff in bg as prototype
						real_confidence += (20 - appearance_cost);
						if(!is_initial)
							real_confidence += (10 - shape_trans_cost);
						real_confidence *= 0.9;
						vcl_cout << " real confidence: " << real_confidence << vcl_endl;
						dbsks_det_desc_xgraph_sptr det = new dbsks_det_desc_xgraph(sol_xgraph, real_confidence );
						det->compute_bbox();
						dets_window.push_back(det);
					}
					raw_dets_all_windows.insert(raw_dets_all_windows.end(), dets_window.begin(), dets_window.end());
				}
			}
		}


    vcl_cout << "\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
  } // iw

  vcl_sort(raw_dets_all_windows.begin(), raw_dets_all_windows.end(), dbsks_decreasing_confidence);

	// only keep the first 10

  //if(raw_dets_all_windows.size()>10)
		//raw_dets_all_windows.erase(raw_dets_all_windows.begin()+10, raw_dets_all_windows.end());

  vcl_cout << "\n> Number of raw detections: " << raw_dets_all_windows.size() << vcl_endl;

  //> Non-max supression on the boundary polygon
  if (run_nms_based_on_overlap)
  {
    vcl_cout << "\n> Non-max suppression (NMS) based on boundary polygon box: ...";
    dbsks_det_nms_using_polygon(raw_dets_all_windows, dets, min_overlap_ratio_for_rejection);
    vcl_cout << "\n  # detections after NMS: " << dets.size() << "\n"; 
  }
  else
  {
    vcl_cout << "\n> No non-max suppression (NMS). All detections accepted.\n";
	// delete the duplicated detections, and only save the top 10
	dets.push_back(raw_dets_all_windows[0]);
	int max_size = vnl_math::min(int(raw_dets_all_windows.size()), 10);
	if(prev_dets.empty())
		max_size = vnl_math::min(int(raw_dets_all_windows.size()), 20);
	for(int d = 1; d < max_size; d++)
	{
		if(raw_dets_all_windows[d]->confidence() == raw_dets_all_windows[d-1]->confidence())
			continue;
		else
			dets.push_back(raw_dets_all_windows[d]);
	}

    //dets = raw_dets_all_windows;
  }
  return true;
}


vcl_vector<double> dbsks_detect_xgraph_using_edgemap::
compute_bg_vec (dbsksp_xshock_graph_sptr& sol_xgraph, vil_image_view<float>& L)
{
	int ni = L.ni();
	int nj = L.nj();
	vcl_vector<double> bg_vec;

	for (dbsksp_xshock_graph::vertex_iterator vit = sol_xgraph->vertices_begin();
		vit != sol_xgraph->vertices_end(); ++vit)
	{
		dbsksp_xshock_node_sptr xv = *vit;
		double x, y, psi, phi, radius;
		//vcl_cout<< "read para" <<vcl_endl;
		xv->descriptor(xv->edge_list().front())->get(x, y, psi, phi, radius);

		int id = xv->id();

		int pos = find(this->appearance_model_node_id.begin(), this->appearance_model_node_id.end(), id) - appearance_model_node_id.begin();
		

		if (pos < this->appearance_model_node_id.size())
		{
			//vcl_cout << " pos: " << pos;
			// compute the average brightness in a 9 X 9 window centered at the nodes which are included in appearance model
			int cnt = 0;
			int sum_bg = 0;
			for (int i = vnl_math::max(int(x) - 4, 0) ; i< vnl_math::min(int(x)+5, ni); i++)
			{
				for(int j = vnl_math::max(int(y) - 4, 0) ; j< vnl_math::min(int(y)+5, nj); j++)
				{
					cnt++;
					sum_bg += L(i,j);
				}
			}
			double avg_bg = double(sum_bg)/double(cnt);
			bg_vec.push_back(avg_bg);
		}
	}
	return bg_vec;
}

bool dbsks_detect_xgraph_using_edgemap::
update_appearance_model(vcl_vector<dbsks_det_desc_xgraph_sptr > prev_dets, vil_image_view<float>& L)
{
	int num_dets = 2; 
	if(prev_dets.size()==0)
		return false;
	vcl_vector<vcl_vector<double> > bg_vec;
	for (int i = 0; i< vcl_min(int(prev_dets.size()),num_dets); i++)
	{
		dbsksp_xshock_graph_sptr prev_xgraph = prev_dets[i]->xgraph();
		bg_vec.push_back(compute_bg_vec (prev_xgraph,  L));
		//for (int j = 0; j< bg_vec.size(); i++)
			
	}

	vcl_cout << " updated appearnce: ";
	for (int j =0 ;j< bg_vec[0].size();j++)
	{
		double bg_1 = 0;
		for (int i = 0; i< vcl_min(int(prev_dets.size()),num_dets); i++)
		{
			bg_1 += bg_vec[i][j];
		}
		bg_1 = bg_1/vcl_min(int(prev_dets.size()),num_dets);
		this->appearance_model_node_value[j] = bg_1;
		vcl_cout << bg_1 << " ";
	}	
	vcl_cout << vcl_endl;
	
	return true;
}

double dbsks_detect_xgraph_using_edgemap::
compute_appearance_cost(dbsksp_xshock_graph_sptr& sol_xgraph, vil_image_view<float>& L)
{
	
	int ni = L.ni();
	int nj = L.nj();
	double bg_sum = 0;
	double cost = 0;
	vcl_vector<double> bg_vec;

	for (dbsksp_xshock_graph::vertex_iterator vit = sol_xgraph->vertices_begin();
		vit != sol_xgraph->vertices_end(); ++vit)
	{
		dbsksp_xshock_node_sptr xv = *vit;
		double x, y, psi, phi, radius;
		//vcl_cout<< "read para" <<vcl_endl;
		xv->descriptor(xv->edge_list().front())->get(x, y, psi, phi, radius);

		int id = xv->id();

		int pos = find(this->appearance_model_node_id.begin(), this->appearance_model_node_id.end(), id) - appearance_model_node_id.begin();
		

		if (pos < this->appearance_model_node_id.size())
		{
			vcl_cout << " pos: " << pos;
			// compute the average brightness in a 9 X 9 window centered at the nodes which are included in appearance model
			int cnt = 0;
			int sum_bg = 0;
			for (int i = vnl_math::max(int(x) - 4, 0) ; i< vnl_math::min(int(x)+5, ni); i++)
			{
				for(int j = vnl_math::max(int(y) - 4, 0) ; j< vnl_math::min(int(y)+5, nj); j++)
				{
					cnt++;
					sum_bg += L(i,j);
				}
			}
			double avg_bg = double(sum_bg)/double(cnt);

			vcl_cout << " bg: " << avg_bg;
			bg_vec.push_back(avg_bg);
			bg_sum += avg_bg;
			cost+= vcl_sqrt((avg_bg - this->appearance_model_node_value[pos])*(avg_bg - this->appearance_model_node_value[pos]));
		}
	}

	double bg_mean = bg_sum/bg_vec.size();
	double var;

	for(int i = 0; i< bg_vec.size(); i++)
	{
		var += (bg_vec[i]-bg_mean)*(bg_vec[i]-bg_mean);
	}

//	double appearance_cost = (var + cost)/bg_vec.size();
	double appearance_cost = cost/bg_vec.size();
	//vcl_cout << " appearance cost: "<< appearance_cost << vcl_endl;
	if(appearance_model_node_value.back()>100)
		appearance_cost /= 2.5;
	return 0.3*appearance_cost;
}

double dbsks_detect_xgraph_using_edgemap::
compute_appearance_cost_v2(dbsksp_xshock_graph_sptr& cur_xgraph, vil_image_view<vxl_byte>& cur_image, dbsksp_xshock_graph_sptr prev_xgraph, vil_image_view<vxl_byte>& prev_image)
{
	vil_image_view<vxl_byte > cur_screenshot_binary;
	vcl_vector<vgl_point_2d<int > > cur_region_pts;
    if(!dbsks_fill_in_silhouette(cur_xgraph, cur_image, cur_region_pts, cur_screenshot_binary))
		vcl_cout << "failed in fill the silhouette of current xgraph.\n";
	//else
		//vcl_cout << "Done: fill the silhouette of current xgraph.\n";
	// compute the appearance id matrix
	vcl_vector<vnl_matrix<double> > cur_appearance_id_matrix = dbsks_compute_appearance_id_matrix( cur_region_pts, cur_image);

	vil_image_view<vxl_byte > prev_screenshot_binary;
	vcl_vector<vgl_point_2d<int > > prev_region_pts;
    if(!dbsks_fill_in_silhouette(prev_xgraph, prev_image, prev_region_pts, prev_screenshot_binary))
		vcl_cout << "failed in fill the silhouette of previous xgraph.\n";
	//else
		//vcl_cout << "Done: fill the silhouette of previous xgraph.\n";
	// compute the appearance id matrix
	vcl_vector<vnl_matrix<double> > prev_appearance_id_matrix = dbsks_compute_appearance_id_matrix( prev_region_pts, prev_image);

	vnl_matrix<double> diff_matrix_0 =  cur_appearance_id_matrix[0] - prev_appearance_id_matrix[0];
	vnl_matrix<double> diff_matrix_1 =  cur_appearance_id_matrix[1] - prev_appearance_id_matrix[1];
	double cost = diff_matrix_0.absolute_value_sum() + diff_matrix_1.absolute_value_sum();

	return 10*cost;
}


//------------------------------------------------------------------------------
//: Compute black/white mask for an edge map: 255 at edge pixels and 0 otherwise
bool dbsks_detect_xgraph_using_edgemap::
convert_edgemap_to_bw(const dbdet_edgemap_sptr& edgemap,
                      vil_image_view<vxl_byte >& bw_mask)
{
  if (!edgemap)
    return false;

  unsigned ni = edgemap->ncols();
  unsigned nj = edgemap->nrows();
  bw_mask.set_size(ni, nj, 1);

  // mask's pixel values: 255 at each positions and 0 otherwise
  for (unsigned i =0; i < ni; ++i)
  {
    for (unsigned j =0; j < nj; ++j)
    {   
      bw_mask(i, j) = (edgemap->cell(i, j).empty() ? 0 : 255);
    }
  }

  return true;
}

static bool is_edges_covered_in_window (vgl_box_2d<int > window, vcl_vector<dbdet_edgel*> edgels)
{
	int cover_cnt = 0;
	for (int i = 0; i< edgels.size(); i++)
	{
		if (edgels[i]->pt.x()>=window.min_x() && edgels[i]->pt.x()<=window.max_x() && edgels[i]->pt.y()>=window.min_y() && edgels[i]->pt.y()<=window.max_y())
			cover_cnt ++;
	}

	if(double(cover_cnt)/double(edgels.size())>0.4)
		return true;
	else
		return false;
}

// currently, it's difference between radius and chord
double dbsks_detect_xgraph_using_edgemap::
compute_shape_trans_cost(dbsksp_xshock_graph_sptr& cur_xgraph, dbsksp_xshock_graph_sptr prev_xgraph)
{

	vcl_vector<double> prev_x_vec, prev_y_vec, prev_r_vec;
	for (dbsksp_xshock_graph::vertex_iterator vit = prev_xgraph->vertices_begin();
		vit != prev_xgraph->vertices_end(); ++vit)
	{
		dbsksp_xshock_node_sptr xv = *vit;
		double x, y, psi, phi, radius;
		//vcl_cout<< "read para" <<vcl_endl;
		xv->descriptor(xv->edge_list().front())->get(x, y, psi, phi, radius);

		prev_x_vec.push_back(x);
		prev_y_vec.push_back(y);
		prev_r_vec.push_back(radius);
	}
	vcl_vector<double> prev_chord_vec;
	for (dbsksp_xshock_graph::edge_iterator eit = prev_xgraph->edges_begin(); eit !=prev_xgraph->edges_end(); ++eit)
	{
		dbsksp_xshock_edge_sptr xe = *eit;
		dbsksp_xshock_node_descriptor start = *xe->source()->descriptor(xe);
		dbsksp_xshock_node_descriptor end = xe->target()->descriptor(xe)->opposite_xnode();

		if(xe->source()->degree()==1 || xe->target()->degree()==1)
			continue;
		//dbsksp_xshock_fragment xfrag(start, end);

		double chord_len = vgl_distance(start.pt(), end.pt());
		prev_chord_vec.push_back(chord_len);
	}

	vcl_vector<double> cur_x_vec, cur_y_vec, cur_r_vec;
	double cur_root_r;
	for (dbsksp_xshock_graph::vertex_iterator vit = cur_xgraph->vertices_begin();
		vit != cur_xgraph->vertices_end(); ++vit)
	{
		dbsksp_xshock_node_sptr xv = *vit;
		double x, y, psi, phi, radius;
		//vcl_cout<< "read para" <<vcl_endl;
		xv->descriptor(xv->edge_list().front())->get(x, y, psi, phi, radius);
		if(unsigned(xv->id()) == cur_xgraph->root_vertex_id())
			cur_root_r = radius;
		cur_x_vec.push_back(x);
		cur_y_vec.push_back(y);
		cur_r_vec.push_back(radius);
	}

	vcl_vector<double> cur_chord_vec;
	for (dbsksp_xshock_graph::edge_iterator eit = cur_xgraph->edges_begin(); eit !=cur_xgraph->edges_end(); ++eit)
	{
		dbsksp_xshock_edge_sptr xe = *eit;
		dbsksp_xshock_node_descriptor start = *xe->source()->descriptor(xe);
		dbsksp_xshock_node_descriptor end = xe->target()->descriptor(xe)->opposite_xnode();

		if(xe->source()->degree()==1 || xe->target()->degree()==1)
			continue;
		//dbsksp_xshock_fragment xfrag(start, end);

		double chord_len = vgl_distance(start.pt(), end.pt());
		cur_chord_vec.push_back(chord_len);
	}

// Compute the cost of shape consistency: radius change + chord length change
	double diff_dist=0, diff_r_2=0, diff_chord=0;
	for (int i = 0; i< cur_x_vec.size(); i++)
	{
//		diff_dist += (cur_x_vec[i]-prev_x_vec[i])*(cur_x_vec[i]-prev_x_vec[i]) + (cur_y_vec[i]-prev_y_vec[i])*(cur_y_vec[i]-prev_y_vec[i]);
		//diff_r_2 += (cur_r_vec[i]-proto_r_vec[i])*(cur_r_vec[i]-proto_r_vec[i]);
		diff_r_2 += (cur_r_vec[i]-prev_r_vec[i])*(cur_r_vec[i]-prev_r_vec[i]);
	}

	for (int i = 0; i< cur_chord_vec.size(); i++)
	{
		diff_chord += (cur_chord_vec[i]-prev_chord_vec[i])*(cur_chord_vec[i]-prev_chord_vec[i]);
	}
	double cost = vcl_sqrt(diff_r_2) + vcl_sqrt(diff_chord);

	return cost;

}

