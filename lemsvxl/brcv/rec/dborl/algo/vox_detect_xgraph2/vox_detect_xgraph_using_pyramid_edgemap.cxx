//:
// \file

#include "vox_detect_xgraph_using_pyramid_edgemap.h"

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

#include <vsol/vsol_box_2d.h>
#include <bpro1/bpro1_parameters.h>
#include <vil/vil_image_resource.h>
#include <vil/file_formats/vil_pyramid_image_list.h>
#include <vil/vil_load.h>
#include <vil/vil_convert.h>

#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_timer.h>
#include <vul/vul_sprintf.h>


//------------------------------------------------------------------------------
//:
bool vox_detect_xgraph_using_pyramid_edgemap::
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

  // We will detect objects of the specified category using all xgraph sizes.
  // Then we run non-max suppression to reduce number of candidates
  // At the end, we threshold to finalize the detection list

  //> List of detection records from all scales
  vcl_vector<dbsks_det_desc_xgraph_sptr > raw_dets_all_scales;

  // Iterate thru all possible sizes of prototype xgraph
  for (unsigned i_scale =0; i_scale < xgraph_scales.size(); ++i_scale)
  {
    vcl_cout
      << "\n\n-------------------------------------------------------------------"
      << "\nProcessing xgraph scale = " << xgraph_scales[i_scale] << "\n\n";

    //> Name of folder to store detect records from this scale
    vcl_string storage_foldername = "scale_" + vul_sprintf("%d", vnl_math_rnd(xgraph_scales[i_scale]));
    vcl_string storage_folder = work_folder + "/" + storage_foldername;

    //> create the directory if not yet done
    if (!vul_file::is_directory(storage_folder))
    {
      if (!vul_file::make_directory(storage_folder))
      {
        vcl_cout << "\nERROR: cannot create storage_folder to save detections.\n";
        continue;
      }
    }

    //> Desired graph size, compared to original image
    double target_xgraph_size = xgraph_scales[i_scale];
    
    // Instead of using the original image and change the size of the prototype xgraph,
    // we will change the size of the image and keep the size of the prototype xgraph fixed
    double actual_pyramid_scale = -1; //> scale of image to keep size of xgraph fixed
    dbdet_edgemap_sptr actual_edgemap = 0; //> loaded edgemap corresponding to actual_pyramid_scale
    dbsksp_xshock_graph_sptr actual_xgraph = 0; //> xgraph to use as prototype

    this->load_edgemap_in_pyramid_keeping_graph_size_fixed(target_xgraph_size,
      actual_pyramid_scale, actual_edgemap, actual_xgraph);

    //> If a scale has been processed, simply load the results back
    vcl_vector<dbsks_det_desc_xgraph_sptr > dets; //> List of detections for this scale
    if (dbsks_load_detections_from_folder(storage_folder, dets))
    {
      vcl_cout << "\nThis scale has been processed. All records are loaded back.\n";
    }
    else
    {
      //> Detect xgraph with the specified scale
      dets.clear();
      this->run_detection_on(actual_edgemap, actual_xgraph, 
        min_accepted_confidence, storage_folder,
        dets);

      //> Dump the detections to a folder for backing up
      vcl_cout << "\n> Saving detections of selected scale to dump folder = " 
        << storage_folder << "\n";
      {
        // form a unique id for this group of detections
        vcl_string xgraph_name = vul_file::strip_extension(vul_file::strip_directory(xgraph_file));
        vcl_string det_group_id = xgraph_name + "+" + object_id + "+" + storage_foldername;
        vcl_string model_category = "";

        // create a binary image from the edgemap
        unsigned ni = actual_edgemap->ncols();
        unsigned nj = actual_edgemap->nrows();
        vil_image_view<vxl_byte > bg_view(ni, nj);
        bg_view.fill(0);
        for (unsigned i =0; i < ni; ++i)
        {
          for (unsigned j =0; j < nj; ++j)
          {
            if (!actual_edgemap->cell(i, j).empty())
            {
              bg_view(i, j) = 255;
            }
          }
        }
        dbsks_save_detections_to_folder(dets, object_id, model_category, det_group_id, bg_view, storage_folder, "");
      }
    } // if exist
    

    // scale the detection back to original size of the image
    for (unsigned m =0; m < dets.size(); ++m)
    {
      dets[m]->xgraph()->scale_up(0, 0, 1.0 / actual_pyramid_scale);
      dets[m]->compute_bbox();
    }


    // Some report on results
    vcl_cout 
      << "> Detection results for selected scale:\n"
      << "\n  xgraph size:   " << target_xgraph_size
      << "\n  # detections:  " << dets.size() << "\n";

    // Save detections to the global list
    raw_dets_all_scales.insert(raw_dets_all_scales.end(), dets.begin(), dets.end());
    vcl_cout
      << "\n-------------------------------------------------------------------\n";
  }

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
bool vox_detect_xgraph_using_pyramid_edgemap::
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
  

  // xgraph
  if (!dbsks_load_xgraph(xgraph_file, xgraph_prototype_))
  {
    return false;
  }

  // geometric model
  if (!dbsks_load_xgraph_geom_model(xgraph_geom_file, xgraph_geom))
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

  // ccm model
  dbsks_load_xgraph_ccm_model(xgraph_ccm_file, xgraph_ccm);

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
bool vox_detect_xgraph_using_pyramid_edgemap::
load_edgemap_pyramid()
{
  //>> Load all edgemaps in the pyramid ........................................
  vcl_cout << "\n>> Loading all edgemap images in the pyramid ... ";

  // regular expression to iterate thru edgemap files
  vcl_string edgemap_regexp = edgemap_folder + "/" + object_id + "*" + edgemap_ext;
  
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



////------------------------------------------------------------------------------
////: Compute sizes of model xgraph to run
//bool dbsks_detect_xgraph_using_pyramid_edgemap::
//compute_list_model_graph_size()
//{
//    //>> Compute list of xgraph scales to detect
//  double image_width = this->source_image.ni();
//  double image_height = this->source_image.nj();
//  double image_scale = vcl_sqrt(image_width * image_height);
//
//  // Maximum xgraph scale is bounded above by image size
//  max_xgraph_scale = vnl_math_min(image_scale, max_xgraph_scale);
//
//  xgraph_scales.clear();
//  for (double s = min_xgraph_scale; s <= max_xgraph_scale; s *= vcl_pow(2, log2_scale_step))
//  {
//    xgraph_scales.push_back(vnl_math_rnd(s)); // rounding simply for good-looking numbers
//  }
//
//  // reverse the order - process large size first
//  vcl_reverse(xgraph_scales.begin(), xgraph_scales.end());
//  
//
//  
//  //////////////////////////////////////////////////////////////////////////////
//  //>> Detect xgraphs ..........................................................
//  vcl_cout << "\n>> Computing the xgraph scales to run.\n";
//  vcl_cout << "\n   - Image size (W x H) = " << image_width << " x " << image_height 
//    << "\n   - Image scale (sqrt of area) = " << vcl_sqrt(image_width * image_height)
//    << "\n   - Min xgraph scale = " << min_xgraph_scale 
//    << "\n   - Log2 of scale step = " << log2_scale_step
//    << "\n   - Max xgraph scale = " << max_xgraph_scale
//    << "\n   - List of scales = [ ";
//  for (unsigned i =0; i < xgraph_scales.size(); ++i)
//  {
//    vcl_cout << xgraph_scales[i] << ", ";
//  }
//  vcl_cout << "]\n";
//
//  if (xgraph_scales.empty())
//  {
//    vcl_cout << "\nERROR: There is no xgraph scale. Please check params again.\n";
//    return false;
//  }
//  return true;
//}







//------------------------------------------------------------------------------
//: Compute sizes of model xgraph to run
bool vox_detect_xgraph_using_pyramid_edgemap::
compute_list_model_graph_size()
{
  // Preliminary checks
  if (!(this->prototype_xgraph_base_size > 0) ||
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
  this->prototype_xgraph_max_size = vnl_math_min(image_size, this->prototype_xgraph_max_size);

  xgraph_scales.clear();
  for (double s = this->prototype_xgraph_min_size; s <= this->prototype_xgraph_max_size; 
    s *= vcl_pow(2, this->prototype_xgraph_log2_increment_step))
  {
    xgraph_scales.push_back(vnl_math_rnd(s)); // rounding simply for good-looking numbers
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



//------------------------------------------------------------------------------
//: Load edgemap corresponding to a target xgraph size to that the actual
// xgraph size is about the same as the base xgraph size
bool vox_detect_xgraph_using_pyramid_edgemap::
load_edgemap_in_pyramid_keeping_graph_size_fixed(double target_xgraph_size, 
                        double& actual_pyramid_scale,
                        dbdet_edgemap_sptr& actual_edgemap,
                        dbsksp_xshock_graph_sptr& actual_xgraph)
{
  // Compute image scale (compared to base image) such that we can maintain the same graph/image ratio
  // while keeping the graph scale at the base level
  double target_pyramid_scale = this->prototype_xgraph_base_size / target_xgraph_size; // 

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
      scale_diff[k] = vnl_math_abs(vcl_log(this->list_edgemap_scale[k] / target_edgemap_scale));
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
  vcl_string edgemap_file = edgemap_folder + "/" + edgemap_fname;

  // Name of edge orientation file
  vcl_string edgeorient_fname = base_name + edgeorient_ext;
  vcl_string edgeorient_file = edgemap_folder + "/" + edgeorient_fname;

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
bool vox_detect_xgraph_using_pyramid_edgemap::
run_detection_on(const dbdet_edgemap_sptr& edgemap, 
    const dbsksp_xshock_graph_sptr& xgraph,
    double confidence_lower_threshold,
    const vcl_string& work_dir,
    vcl_vector<dbsks_det_desc_xgraph_sptr >& dets)
{
  vul_timer timer;
  timer.mark();

  //> Contour-Chamfer-Matching cost function...................................
  vcl_cout << "\n> Constructing a likelihood function based on CCM cost ...";

  dbsks_xshock_ccm_likelihood ccm_like;
  ccm_like.set_edgemap(edgemap);
  ccm_like.set_biarc_sampler(&dbsks_biarc_sampler::default_instance());
  ccm_like.set_ccm_model(xgraph_ccm);
  vcl_cout << " [ OK ]\n";

  //> Compute all windows (rectangular boxes) necessary to cover the whole image
  vcl_cout << "\n> Computing sliding (rectangular) windows to cover the whole image ...";
  
  vcl_vector<vgl_box_2d<int > > windows; // list of detection windows
  dbsks_algos::compute_detection_windows(det_window_width, det_window_height, 
    edgemap->ncols(), edgemap->nrows(), windows);

  // Print out list of windows
  vcl_cout << "\n> Total #windows = " << windows.size() << "\n";
  vcl_cout << "\n> List of windows: \n";
  for (unsigned iw =0; iw < windows.size(); ++iw)
  {
    vgl_box_2d<int > window = windows[iw];
    vcl_cout 
      << "  - index= " << iw 
      << " : xmin=" << window.min_x() 
      << " ymin=" << window.min_y() 
      << " xmax=" << window.max_x()
      << " ymax=" << window.max_y() << "\n";
  }
  vcl_cout << " [ OK ]\n";
  vcl_cout.flush();

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
      << "\n   [xmin ymin xmax ymax] = "
      << "[" << window.min_x() 
      << " " << window.min_y() 
      << " " << window.max_x()
      << " " << window.max_y() << "]\n";

    //> Name of folder to store detect records from this scale
    vcl_string storage_dirname = "ROI" + 
      vul_sprintf("_%d_%d_%d_%d", window.min_x(), window.min_y(), window.max_x(), window.max_y());
    vcl_string storage_dir = work_dir + "/" + storage_dirname;

    //> create the directory if not yet done
    if (!vul_file::is_directory(storage_dir))
    {
      if (!vul_file::make_directory(storage_dir))
      {
        vcl_cout << "\nERROR: cannot create dump_folder to save detections.\n";
        continue;
      }
    }

    // If a scale has been processed, simply load the results back
    vcl_vector<dbsks_det_desc_xgraph_sptr > dets_window;
    if (dbsks_load_detections_from_folder(storage_dir, dets_window))
    {
      vcl_cout << "  This window has been processed. Loading back detection records...";
      raw_dets_all_windows.insert(raw_dets_all_windows.end(), dets_window.begin(), dets_window.end());
      vcl_cout << " Done. Move on to next window.\n";
      continue;
    }

    //> Compute ccm for a region of interest only
    ccm_like.compute_internal_data(window);

    // xshock detection engine
    dbsks_xshock_detector engine;
    engine.xshock_likelihood_ = &ccm_like;
    engine.xgraph_geom_ = xgraph_geom;
    engine.set_xgraph(xgraph);

    //--------------------------------------------------------------------------
    engine.detect(window, float(confidence_lower_threshold));
    //--------------------------------------------------------------------------

    //> Construct a vector of detection descriptor
    dets_window.clear();
    dets_window.reserve(engine.list_solutions_.size());
    for (unsigned i =0; i < engine.list_solutions_.size(); ++i)
    {
      dbsksp_xshock_graph_sptr sol_xgraph = engine.list_solutions_[i];
      
      double confidence = -engine.list_solution_costs_[i];
      double real_confidence = -engine.list_solution_real_costs_[i];

      // only consider detetion with at least minimal confidence level
      if (real_confidence > confidence_lower_threshold)
      {
        dbsks_det_desc_xgraph_sptr det = new dbsks_det_desc_xgraph(sol_xgraph, real_confidence);
        det->compute_bbox();
        dets_window.push_back(det);
      }
    } // solution

    //> Save solutions of the selected window to a folder
    vcl_cout << "\n> Saving detections of selected window to dump folder = " 
      << storage_dir << "\n";
    {
      // form a unique id for this group of detections
      vcl_string xgraph_name = vul_file::strip_extension(vul_file::strip_directory(xgraph_file));
      vcl_string det_group_id = xgraph_name + "+" + object_id + "+" + storage_dirname;
      vcl_string model_category = "";
      
      // create a binary image from the edgemap
      unsigned ni = edgemap->ncols();
      unsigned nj = edgemap->nrows();
      vil_image_view<vxl_byte > bg_view(ni, nj);
      bg_view.fill(0);
      for (unsigned i =0; i < ni; ++i)
      {
        for (unsigned j =0; j < nj; ++j)
        {
          if (!edgemap->cell(i, j).empty())
          {
            bg_view(i, j) = 255;
          }
        }
      }
      dbsks_save_detections_to_folder(dets_window, object_id, model_category, det_group_id, bg_view, storage_dir, "");
    }

    // add detections from this window to the overall list
    raw_dets_all_windows.insert(raw_dets_all_windows.end(), dets_window.begin(), dets_window.end());
    vcl_cout << "\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
  } // iw

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
    dets = raw_dets_all_windows;
  }
  return true;
}

