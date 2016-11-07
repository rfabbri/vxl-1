// This is dborl/algo/vox_detect_xgraph/vox_detect_xgraph_postprocess_utils.cxx

//:
// \file

#include "vox_detect_xgraph_postprocess_utils.h"
#include "vox_detect_xgraph_params_sptr.h"
#include "vox_detect_xgraph_params.h"

#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsksp/xio/dbsksp_xio_xshock_graph.h>

#include <dbsks/dbsks_wcm.h>
#include <dbsks/dbsks_biarc_sampler.h>
#include <dbsks/dbsks_xgraph_geom_model.h>
#include <dbsks/dbsks_xgraph_ccm_model.h>
#include <dbsks/dbsks_xshock_wcm_likelihood.h>
#include <dbsks/algo/dbsks_detect_xgraph.h>
#include <dbsks/algo/dbsks_load.h>
#include <dbsks/xio/dbsks_xio_xshock_det.h>
#include <dbsks/dbsks_xshock_utils.h>
#include <dbsks/dbsks_det_nms.h>
#include <dbsks/dbsks_det_desc_xgraph.h>
#include <dbsks/dbsks_det_desc_xgraph_sptr.h>
#include <dbsks/dbsks_utils.h>
#include <dbsks/dbsks_xfrag_geom_model.h>

#include <dbsksp/algo/dbsksp_screenshot.h>



#include <dbul/dbul_parse_simple_file.h>
#include <bpro1/bpro1_parameters.h>
#include <vsol/vsol_box_2d.h>


#include <vul/vul_file.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vnl/vnl_math.h>
#include <vul/vul_file_iterator.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_map.h>
#include <vcl_sstream.h>


// =============================================================================
// Helper functions
// =============================================================================


// =============================================================================
// Utility functions
// =============================================================================

// -----------------------------------------------------------------------------
//: Re-evaluate the cost of each detection using whole-contour-matching cost
// This was not during DP so we do this to fine-tune the cost after-the-fact
bool dbsks_recompute_cost_using_wcm()
{
  //############################################################################
  //>> input data

  // header and footer to complete the xml record
  vcl_string xml_header_file = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-experiments/xml_det_header.txt";
  vcl_string xml_footer_file = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-experiments/xml_det_footer.txt";

  //// experiment setting - 1
  //vcl_string input_xml_file = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-experiments/exp_47-all_images-combine_exp_45_46/input.xml";
  //vcl_string orig_exp_folder = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-experiments";
  //vcl_string dest_exp_folder = "D:/vision/projects/symseg/xshock/xshock-experiments";
  //vcl_string exp_name = "exp_48-all_images-v0_74-window_512x512";
  //vcl_string list_xml_det_record_file = "list_xml_det_record_exp_48.txt";
  ////vcl_string list_xml_det_record_file = "list_xml_det_record_exp_48_1record.txt";


  // ------------------------
  // experiment setting - 2
  vcl_string input_xml_file = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-experiments/exp_49-swans-all_images-v0_75-window_512x512/input_template.xml";
  vcl_string orig_exp_folder = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-experiments";
  vcl_string dest_exp_folder = "D:/vision/projects/symseg/xshock/xshock-experiments";
  vcl_string exp_name = "exp_49-swans-all_images-v0_75-window_512x512";
  vcl_string list_xml_det_record_file = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-experiments/exp_49-swans-all_images-v0_75-window_512x512/list_det_record.txt";
  //############################################################################

  //>> some announcement
  vcl_cout << "\n>> Experiment input.xml = " << input_xml_file << vcl_endl;
  vcl_cout << "\n>> Original experiment folder = " << orig_exp_folder << vcl_endl;
  vcl_cout << "\n>> Destination experiment folder = " << dest_exp_folder << vcl_endl;
  vcl_cout << "\n>> Experiment name = " << exp_name << vcl_endl;
  vcl_cout << "\n-------------------------------------------------------------\n\n";

  //>> Paths to original and destination folders
  vcl_string orig_exp_path = orig_exp_folder + "/" + exp_name;
  vcl_string dest_exp_path = dest_exp_folder + "/" + exp_name;

  // Check original experiment folder
  if (!vul_file::is_directory(orig_exp_path))
  {
    vcl_cout << "\nERROR: Original experiment folder does not exist.\n";
    return false;
  }

  // Create destination folder, if necessary
  if (!vul_file::is_directory(dest_exp_path))
  {
    vul_file::make_directory(dest_exp_path);
  }

  // Parse the list_xml_det_record file to get a list of filenames
  vcl_vector<vcl_string > list_xml_det_record;
  dbul_parse_string_list(list_xml_det_record_file, list_xml_det_record);

  //>> Load all detection records in the original experiments
  vcl_vector<dbsks_xshock_det_record_sptr > all_xshock_dets;
  for (unsigned m =0; m < list_xml_det_record.size(); ++m)
  {
    vcl_string xml_record_filename = list_xml_det_record[m];
    vcl_string xml_record_file = orig_exp_path + "/" + xml_record_filename;

    vcl_cout << "\n>> Processing xml_record_file = " << xml_record_file << vcl_endl;

    // Add a header and footer to this xml file to make it complete. Output to a temp file
    vcl_string temp_xml_file = dest_exp_folder + "/" + "temp_det.xml";
    vcl_ofstream os(temp_xml_file.c_str());
    dbsks_append_text_file(os, xml_header_file);
    dbsks_append_text_file(os, xml_record_file);
    dbsks_append_text_file(os, xml_footer_file);
    os.close();

    // Load the temp file to get the list of detection
    vcl_vector<dbsks_xshock_det_record_sptr > xshock_det_list;
    x_read(temp_xml_file, xshock_det_list);
    vul_file::delete_file_glob(temp_xml_file.c_str());

    all_xshock_dets.insert(all_xshock_dets.end(), xshock_det_list.begin(), xshock_det_list.end());
  }

  //>> Group the detection record by image
  vcl_map<vcl_string, vcl_vector<dbsks_xshock_det_record_sptr > > map_obj_to_detlist;
  for (unsigned m =0; m < all_xshock_dets.size(); ++m)
  {
    dbsks_xshock_det_record_sptr det = all_xshock_dets[m];
    vcl_string object_name = "";
    det->get_value("object_name", object_name);
    map_obj_to_detlist[object_name].push_back(det);
  }


  //>> Now compute the wcm cost for detections in each image
  vcl_cout << "\n>> Compute cost using WCM for detection in each image\n";
  for (vcl_map<vcl_string, vcl_vector<dbsks_xshock_det_record_sptr > >::iterator obj_iter =
    map_obj_to_detlist.begin(); obj_iter != map_obj_to_detlist.end(); ++obj_iter)
  {
    vcl_string object_name = obj_iter->first;
    vcl_vector<dbsks_xshock_det_record_sptr > det_list = obj_iter->second;

    vcl_cout << "\nObject name = " << object_name << ":" << vcl_endl;

    // Compute wcm for each detection
    for (unsigned m =0; m < det_list.size(); ++m)
    {
      vcl_cout << " " << m;

      dbsks_xshock_det_record_sptr det_record = det_list[m];

      //i) Retrieve the detection xgraph filename
      vcl_string xgraph_filename = "";
      det_record->get_value("xgraph_xml", xgraph_filename);

      // full path to the xgraph file
      vcl_string xgraph_file = orig_exp_path + "/" + xgraph_filename;

      //ii) Compute wcm cost of the xgraph
      vcl_map<float, float > map_uw2wcm;
      dbsks_compute_wcm_cost(input_xml_file, object_name, xgraph_file, map_uw2wcm);

      // form two strings consisting of all unmatched weight and their costs
      vcl_stringstream uw_sstream;
      vcl_stringstream wcm_sstream;
      
      // first pair
      vcl_map<float, float >::iterator iter = map_uw2wcm.begin();
      uw_sstream << "[" << iter->first;
      wcm_sstream << "[" << iter->second;

      
      // remaining pairs
      ++iter;
      for (; iter != map_uw2wcm.end(); ++iter)
      {
        uw_sstream << "," << iter->first;
        wcm_sstream << "," << iter->second;
      }
      uw_sstream << "]";
      wcm_sstream << "]";

      det_record->add("unmatched_weight", "unmatched_weight", uw_sstream.str());
      det_record->add("wcm_confidence", "wcm_confidence", wcm_sstream.str());

      // write the record out
      vcl_string xml_filename = "xml_det_record+" + 
        vul_file::strip_extension(xgraph_filename) + // remove ".xml"
        ".xml";
      vcl_string xml_file = dest_exp_path + "/" + xml_filename;
      x_write(xml_file, vcl_vector<dbsks_xshock_det_record_sptr >(1, det_record));
    } // iter detection
    vcl_cout << "\n";
  } // iter object name
  vcl_cout << ".Done.\n";
  return true;
}













//------------------------------------------------------------------------------
//: Compute wcm cost of a list of xgraphs in a given image (object)
bool dbsks_compute_wcm_cost(vcl_string exp_input_xml_file, vcl_string object_name, 
                            vcl_string xgraph_file,
                            vcl_map<float, float >& map_uw2wcm)
{
  // Load parameters from input.xml
  vox_detect_xgraph_params_sptr params = new vox_detect_xgraph_params("vox_detect_xgraph");
  params->input_param_filename_ = exp_input_xml_file;

  
  
  // parse the input.xml file
  if (!params->parse_input_xml())
  {
    vcl_cout << "\nERROR: could not parse input xml file: " << params->input_param_filename_ << vcl_endl;
    return false;
  }

  // override the object name retrieved from the input xml file
  params->input_object_name_ = object_name;

  // retrieve experiment parameters from the xml file
  vcl_string image_file = params->get_image_file();
  vcl_string edgemap_file = params->get_edgemap_file();
  vcl_string edgeorient_file = params->get_edgeorient_file();
  vcl_string xgraph_geom_file = params->get_xgraph_geom_file();
  vcl_string xgraph_ccm_file = params->get_xgraph_ccm_file();
  vcl_string cemv_file = params->get_cemv_file();
  vcl_vector<vcl_string > cfrag_list_to_ignore = params->get_cfrag_list_to_ignore();

  
  //>> Load the contents////////////////////////////////////////////////////////

  // Load data from the files
  vil_image_view<float > edgemap;
  vil_image_view<float > edge_angle;
  vcl_vector<vsol_polyline_2d_sptr > polyline_list;
  dbsksp_xshock_graph_sptr xgraph;
  dbsks_xgraph_geom_model_sptr xgraph_geom;
  dbsks_xgraph_ccm_model_sptr xgraph_ccm;

  // Load data from a list of file names
  vcl_cout << "\nLoading data from the files..........................." << "\n"; 

  if (!dbsks_load_data(image_file, edgemap_file, edgeorient_file, cemv_file, 
    xgraph_file, xgraph_geom_file, xgraph_ccm_file, cfrag_list_to_ignore,
    edgemap, edge_angle, polyline_list, xgraph, xgraph_geom, xgraph_ccm))
  {
    vcl_cout << "\nERROR: Could not load all data.\n";
    return false;
  }


  double standard_scale = 130;
  vil_image_view<float > std_edgemap;
  vcl_vector<vsol_polyline_2d_sptr > std_polyline_list;
  vil_image_view<float > std_edge_angle;
  
  // scale the edgemap up so that the xgraph size is "standard", i.e. 130
  double xgraph_scale = vcl_sqrt(xgraph->area());
  double scaled_up_factor = 1.0;
  dbsks_adjust_to_standard_scale(standard_scale, xgraph_scale,
    edgemap, polyline_list, edge_angle,
    scaled_up_factor, std_edgemap, std_polyline_list, std_edge_angle);
  // scale up the xgraph to match with the newly created edgemap
  xgraph->scale_up(0, 0, scaled_up_factor);

  vcl_cout << "\nAdjusting scale of edgemap and linked conotur to match the standard scale\n";
  vcl_cout << "Standard scale =" << standard_scale << "\n"
    << "Original scale = " << xgraph_scale << "\n";

  xgraph_scale *= scaled_up_factor;
  vcl_cout << "Scaled-up factor = " << scaled_up_factor << "\n"
    << "Final xgraph scale = " << xgraph_scale << "\n";

  // Set things up for Whole-Contour-Matching //////////////////////////////////

  //>> WCM ......................................................................
  vcl_cout << "\n>>Constructing Whole-Contour-Matching cost function ...";

  // get parameters
  float ccm_distance_threshold = 8;
  float ccm_tol_near_zero = 4;
  float ccm_lambda = 0.4f;
  float ccm_gamma = 0.3f;
  float ccm_edge_threshold = 15;
  int nbins_0topi = 18;
  xgraph_ccm->get_ccm_params(ccm_edge_threshold, ccm_tol_near_zero, ccm_distance_threshold, 
    ccm_gamma, ccm_lambda, nbins_0topi);
  xgraph->compute_vertex_depths(xgraph_geom->root_vid());
  
  // instatiate wcm
  dbsks_wcm wcm;
  wcm.set_ocm_params(ccm_distance_threshold, ccm_tol_near_zero, ccm_lambda, ccm_gamma, nbins_0topi);
  wcm.set_edge_strength(std_edgemap, ccm_edge_threshold, 255);
  wcm.set_edge_orient(std_edge_angle);
  wcm.set_edge_labels(std_polyline_list);   // set the edge label image 
  wcm.set_weight_unmatched(0.0f); 

  // Compute wcm for xgraph region on image
  vsol_box_2d_sptr bbox = xgraph->bounding_box();
  int bbox_xmin = vnl_math_max(vnl_math_rnd(bbox->get_min_x())-20, int(8));
  int bbox_xmax = vnl_math_min(vnl_math_rnd(bbox->get_max_x())+20, int(std_edgemap.ni()-8)); 
  int bbox_ymin = vnl_math_max(vnl_math_rnd(bbox->get_min_y())-20, int(8));
  int bbox_ymax = vnl_math_min(vnl_math_rnd(bbox->get_max_y())+20, int(std_edgemap.nj()-8));

  vgl_box_2d<int > wcm_window;
  wcm_window.add(vgl_point_2d<int >(bbox_xmin, bbox_ymin));
  wcm_window.add(vgl_point_2d<int >(bbox_xmax, bbox_ymax));
  vcl_cout << "\n  wcm_window" << ": xmin=" << wcm_window.min_x() 
    << " ymin=" << wcm_window.min_y() 
    << " xmax=" << wcm_window.max_x()
    << " ymax=" << wcm_window.max_y() << "\n";

  // compute ccm cost for every pixel in the window
  wcm.compute(wcm_window, dbsks_ccm::USE_CLOSEST_ORIENTED_EDGE);
  vcl_cout << "done" << vcl_endl;;
  
  //>> Biarc sampler............................................................

  // Define a biarc sampler
  static dbsks_biarc_sampler biarc_sampler; 
  
  // we don't want to recompute this all the time
  if (!biarc_sampler.has_cache_samples())
  {
    vcl_cout << "\n>>Constructing a biarc sampler ...";

    // Set parameters of biarc sampler
    dbsks_biarc_sampler_params bsp;
    bsp.set_to_default_values();

    // compute coordinates of the grid points
    biarc_sampler.set_grid(bsp);

    // compute cache sample points for all biarcs in the grid
    double ds = 2;
    int nbins_0to2pi = 36;
    biarc_sampler.set_sampling_params(nbins_0to2pi, ds);
    biarc_sampler.compute_cache_sample_points(); 

    vcl_cout << "done\n";
  }
  else
  {
    vcl_cout << "\n>>Biarc sampler is reused from previous iteration.\n";
  }


  //>> Build a likelihood calculator ...........................................
  dbsks_xshock_wcm_likelihood wcm_like(xgraph_ccm, &wcm, &biarc_sampler);

  //>> Compute likelihood of the xgraph
  for (float kk = 0.0f; kk <= 0.5f; kk = kk+0.02f)
  {
    wcm.set_weight_unmatched(kk);
    float wcm_confidence = float(wcm_like.f_whole_contour(xgraph, vcl_vector<unsigned >(), false));
    map_uw2wcm.insert(map_uw2wcm.end(), vcl_make_pair(kk, wcm_confidence));
  }



  //>> Compute likelihood of the xgraph
  wcm.set_weight_unmatched(0.0f);
  double wcm_confidence = wcm_like.f_whole_contour(xgraph, vcl_vector<unsigned >(1, 10), false);
  vcl_cout << "\n    the (wrongly) computed confidence = " << wcm_confidence << "\n";
  return true;
}










// -----------------------------------------------------------------------------
//: Combine detection results from multiple folders. Examples: detection with
// different scales or different prototypes. Do non-max suppression if 
// the detections overlap.

bool dbsks_combine_det_results(const vcl_string& orig_exp_folder, 
                               const vcl_string& exp_name1,
                               const vcl_string& exp_name2,
                               double min_overlap_ratio_for_rejection,
                               const vcl_string& dest_exp_path,
                               const vcl_string& prototype_name)
{
  //############################################################################
  //>> input data

  //// ------------------------
  //// input
  //vcl_string orig_exp_folder = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-experiments";
  //vcl_string exp_name1 = "exp_64-applelogos_prototype1-all_images-v0_81-no_wcm";
  //vcl_string exp_name2 = "exp_65-applelogos_prototype2-all_images-v0_81-no_wcm";

  //double min_overlap_ratio_for_rejection = 0.3;

  //// output
  //vcl_string dest_exp_folder = "D:/vision/projects/symseg/xshock/xshock-experiments";
  //vcl_string output_exp_name = "exp_66-applelogos_prototype1_and_2-all_images-v0_81-no_wcm";  
  //vcl_string prototype_name = "applelogos_prototype_1_and_2";
  ////############################################################################

  vcl_vector<vcl_string > input_exp_names;
  input_exp_names.push_back(exp_name1);
  input_exp_names.push_back(exp_name2);

  //>> some announcement
  vcl_cout << "\n>> Combine detection data from multiple folders\n"
    << "--------------------------------------------------------------------"
    << "\n>> Original experiment folder = " << orig_exp_folder << "\n"
    << ">>   input_exp_name-1 = " << input_exp_names[0] << "\n"
    << ">>   input_exp_name-2 = " << input_exp_names[1] << "\n"
    << "\n"
    << "\n>> Destination experiment path = " << dest_exp_path << "\n";
  vcl_cout << "\n-------------------------------------------------------------\n\n";

  //>> Load detection results in all folders
  vcl_vector<dbsks_xshock_det_record_sptr > all_xshock_dets;
  for (unsigned i_exp =0; i_exp < input_exp_names.size(); ++i_exp)
  {
    // Path to original experiment folder
    vcl_string orig_exp_path = orig_exp_folder + "/" + input_exp_names[i_exp];

    // Check original experiment folder
    if (!vul_file::is_directory(orig_exp_path))
    {
      vcl_cout << "\nWARNING: Original experiment folder does not exist.\n";
      continue;
    }


    // iterate thru all xml_det_record files
    vcl_string xml_det_record_regexp = "xml_det_record+*.xml";
    for (vul_file_iterator fn= (orig_exp_path + "/" + xml_det_record_regexp); fn; ++fn) 
    {
      vcl_string xml_det_record_file = fn();
      vcl_cout << "\n>> Processing xml_record_file = " << xml_det_record_file << vcl_endl;

      vcl_string xml_det_record_filename = vul_file::strip_directory(xml_det_record_file);

      // Load the list of detections from this xml_record_file
      vcl_vector<dbsks_xshock_det_record_sptr > xshock_det_records;
      x_read(xml_det_record_file, xshock_det_records);

      // add a new attribute to each of the record: the experiment's filepath
      for (unsigned m =0; m < xshock_det_records.size(); ++m)
      {
        xshock_det_records[m]->add("exp_path", "exp_path", orig_exp_path);
        xshock_det_records[m]->add("xml_det_record_filename", "xml_det_record_filename", xml_det_record_filename);
      }

      // update to the global list
      all_xshock_dets.insert(all_xshock_dets.end(), xshock_det_records.begin(), xshock_det_records.end());
    }
  }

  //>> Group the detection record by image
  vcl_map<vcl_string, vcl_vector<dbsks_xshock_det_record_sptr > > map_obj_to_detlist;
  for (unsigned m =0; m < all_xshock_dets.size(); ++m)
  {
    dbsks_xshock_det_record_sptr det = all_xshock_dets[m];
    vcl_string object_name = "";
    det->get_value("object_name", object_name);
    map_obj_to_detlist[object_name].push_back(det);
  }

  // Create destination folder, if necessary
  if (!vul_file::is_directory(dest_exp_path))
  {
    vul_file::make_directory(dest_exp_path);
  }


  //>> Now do non-max supression for detection within each image, assuming their
  // confidence score has been normalized
  for (vcl_map<vcl_string, vcl_vector<dbsks_xshock_det_record_sptr > >::iterator i_obj =
    map_obj_to_detlist.begin(); i_obj != map_obj_to_detlist.end(); ++i_obj)
  {
    vcl_string obj_name = i_obj->first;
    vcl_vector<dbsks_xshock_det_record_sptr > xshock_det_records = i_obj->second;

    // form a list of xshock detections equivalent of the records
    vcl_vector<dbsks_det_desc_xgraph_sptr > xshock_dets; // for non-max suppression
    vcl_map<dbsks_det_desc_xgraph_sptr, dbsks_xshock_det_record_sptr> map_det2record; // for backward retrieval after NMS
    for (unsigned m =0; m < xshock_det_records.size(); ++m)
    {
      dbsks_xshock_det_record_sptr record = xshock_det_records[m];

      // retrieve confidence score of this record
      double confidence = 0;
      record->get_value("confidence", confidence);

      // retrieve path to xgraph
      vcl_string xgraph_filename = ""; // filename
      record->get_value("xgraph_xml", xgraph_filename);

      vcl_string exp_path = ""; // folder name
      record->get_value("exp_path", exp_path);

      // complete path
      vcl_string xgraph_file = exp_path + "/" + xgraph_filename;

      // load xgraph
      dbsksp_xshock_graph_sptr xgraph = 0;
      x_read(xgraph_file, xgraph);
      dbsks_det_desc_xgraph_sptr det = new dbsks_det_desc_xgraph(xgraph, confidence);

      // save to lists
      xshock_dets.push_back(det);
      map_det2record.insert(vcl_make_pair(det, record));
    }

    //>> Non-max suppression across detection results form different scales
  
    vcl_vector<dbsks_det_desc_xgraph_sptr > nms_xshock_dets;
    dbsks_det_nms_using_polygon(xshock_dets, nms_xshock_dets, min_overlap_ratio_for_rejection);

    // Extract the surving records
    vcl_vector<dbsks_xshock_det_record_sptr > nms_xshock_det_records;
    nms_xshock_det_records.reserve(nms_xshock_dets.size());
    for (unsigned m =0; m < nms_xshock_dets.size(); ++m)
    {
      dbsks_det_desc_xgraph_sptr det = nms_xshock_dets[m];
      dbsks_xshock_det_record_sptr record = map_det2record[det];
      nms_xshock_det_records.push_back(record);
    }

    //>> save the surving records

    // construct a meaningful name
    vcl_string xml_det_record_filename = "xml_det_record+" + prototype_name + 
      "+" + obj_name + ".xml";

    vcl_string xml_det_record_file = dest_exp_path + "/" + xml_det_record_filename;

    // write the records to file
    x_write(xml_det_record_file, nms_xshock_det_records);
  } // iter object name

  return true;
}





// -----------------------------------------------------------------------------
//: For each xshock detection record in txt format, add an equivalent .xml file
bool dbsks_convert_det_record_txt_to_xml(const vcl_string& exp_folder)
{
  // Annoucement
  vcl_cout << "\nConverting xshock detection record from .txt to .xml format\n";
  vcl_cout << "\nExperiment folder = " << exp_folder << "\n";

  // For each detection record file (format: "xml_det_record+*.txt"), add a header
  // and footer and save back with a new name, replacing .txt by .xml
  vcl_string txt_det_record_regexp = "xml_det_record+*.txt";
  for (vul_file_iterator fn= (exp_folder + "/" + txt_det_record_regexp); fn; ++fn) 
  {
    vcl_string txt_det_record_file = fn();
    vcl_string txt_det_record_filename = vul_file::strip_directory(txt_det_record_file);
    
    // Some annoucement
    vcl_cout << "\n>> Processing txt_record_file = " << txt_det_record_filename << vcl_endl;
    
    // new name
    vcl_string xml_det_record_file = vul_file::strip_extension(txt_det_record_file) + ".xml";

    // Add a header and footer to the .txt record file to make an .xml file
    vcl_ofstream os(xml_det_record_file.c_str());

    // XML header
    os << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << "\n"
      << "<xgraph_det_list>" << "\n";
      
    dbsks_append_text_file(os, txt_det_record_file);
    os << "</xgraph_det_list>" << "\n";
    os.close();
  }
  return true;
}









// -----------------------------------------------------------------------------
//: Create B/W of shape boundary for each detected shape and update detection record
bool dbsks_create_bnd_screenshot(const vcl_string& exp_folder)
{
  // Annoucement
  vcl_cout << "\nCreate B/W screenshot for boundary of xshock detection\n";
  vcl_cout << "\nExperiment folder = " << exp_folder << "\n";

  // For each detection record file (format: "xml_det_record+*.xml"), 
  // - retrieve the xgraph and the screenshot 
  // - create a new bw image containting the footprint of the xgraph's boundary contour
  vcl_string det_record_regexp = "xml_det_record+*.xml";
  for (vul_file_iterator fn= (exp_folder + "/" + det_record_regexp); fn; ++fn) 
  {
    vcl_string det_record_file = fn();
    vcl_string det_record_filename = vul_file::strip_directory(det_record_file);
    
    // Some annoucement
    vcl_cout << "\n>> Processing det_record_file = " << det_record_filename << vcl_endl;

    // Load the detection record file
    vcl_vector<dbsks_xshock_det_record_sptr > xshock_det_list;
    x_read(det_record_file, xshock_det_list);
    
    for (unsigned m =0; m < xshock_det_list.size(); ++m)
    {
      dbsks_xshock_det_record_sptr det_record = xshock_det_list[m];
      
      // Retrieve the shock graph
      vcl_string xgraph_file = "";
      det_record->get_value("xgraph_xml", xgraph_file);
      xgraph_file = exp_folder + "/" + vul_file::strip_directory(xgraph_file);

      // Load the xgraph
      dbsksp_xshock_graph_sptr xgraph = 0;
      x_read(xgraph_file, xgraph);


      // Re-evaluate the bounding box, if necessary
      bool recompute_bbox = true;
      if (recompute_bbox)
      {
        double xgraph_scale = vcl_sqrt(xgraph->area());
        vsol_box_2d_sptr bbox = xgraph->bounding_box();

        det_record->set_value("xgraph_scale", xgraph_scale);
        det_record->set_value("bbox_xmin", bbox->get_min_x());
        det_record->set_value("bbox_ymin", bbox->get_min_y());
        det_record->set_value("bbox_xmax", bbox->get_max_x());
        det_record->set_value("bbox_ymax", bbox->get_max_y());  
      }



      // Create an emptry black image with size equal size of the original image
      vcl_string screenshot_file = "";
      det_record->get_value("screenshot", screenshot_file);
      screenshot_file = vul_file::strip_extension(vul_file::strip_directory(screenshot_file));
      screenshot_file = exp_folder + "/" + screenshot_file + ".png";
      vil_image_resource_sptr screenshot_rs = vil_load_image_resource(screenshot_file.c_str());
      if (!screenshot_rs)
      {
        vcl_cout << "\nERROR: couldn't load screenshot file " << screenshot_file << "\n";
        return false;
      }

      vil_image_view<vxl_byte > bnd_screenshot(screenshot_rs->ni(), screenshot_rs->nj());
      bnd_screenshot.fill(0);

      // draw the xgraph boundary on the empty image
      dbsksp_screenshot_in_place(xgraph, bnd_screenshot, 
        1, 0, // no padding
        vil_rgb<vxl_byte >(255, 255, 255), // white for center line
        vil_rgb<vxl_byte >(0, 0, 0)); // black for padding

      // filename to save the new image
      vcl_string bnd_screenshot_filename =  
        vul_file::strip_extension(vul_file::strip_directory(screenshot_file)) + ".bnd.png";

      vcl_string bnd_screenshot_file =  exp_folder+"/"+bnd_screenshot_filename;


      // save image
      if (!vil_save(bnd_screenshot, bnd_screenshot_file.c_str()))
      {
        vcl_cout << "\nERROR: couldn't save image to file: " << bnd_screenshot_file << vcl_endl;
        return false;
      }


      vcl_string bnd_screenshot_tag = "bnd_screenshot";
      if (!det_record->valid_parameter(bnd_screenshot_tag))
      {
        det_record->add("bnd_screenshot", bnd_screenshot_tag, vcl_string(""));
      }
      det_record->set_value(bnd_screenshot_tag, bnd_screenshot_filename);
    } // det_record

    // save the file back
    x_write(det_record_file, xshock_det_list);
  } // det_record_file
  return true;
}









// -----------------------------------------------------------------------------
//: Combine multiple detection records per image
bool dbsks_combine_det_records_per_image(const vcl_string& exp_folder, 
                                         const vcl_string& output_folder,
                                         const vcl_string& xgraph_prototype_name)
{
  // Annoucement
  vcl_cout << "\nCombine det records with same object_name into one file\n";
  vcl_cout << "\n  Experiment folder = " << exp_folder << "\n"
    << "\n  Output folder = " << output_folder << "\n"
    << "\n  xgraph_prototype_name = " << xgraph_prototype_name << "\n";

  // Collect names of all detection records in folder
  vcl_vector<vcl_string > list_det_record_filename;
  vcl_string det_record_regexp = "xml_det_record+*.xml";
  for (vul_file_iterator fn= (exp_folder + "/" + det_record_regexp); fn; ++fn) 
  {
    vcl_string det_record_file = fn();
    vcl_string det_record_filename = vul_file::strip_directory(det_record_file);
    list_det_record_filename.push_back(det_record_filename);
  }

  // Now read the files and group the records by object names
  vcl_map<vcl_string, vcl_vector<dbsks_xshock_det_record_sptr > > map_obj_to_records;
  for (unsigned i_file =0; i_file < list_det_record_filename.size(); ++i_file)
  {
    vcl_string det_record_filename = list_det_record_filename[i_file];
    
    // Some annoucement
    vcl_cout << "\n>> Processing det_record_file = " << det_record_filename << vcl_endl;

    // Load the detection record file
    vcl_string det_record_file = exp_folder + "/" + det_record_filename;
    vcl_vector<dbsks_xshock_det_record_sptr > list_det_record;
    x_read(det_record_file, list_det_record);

    // Group the record by their object name
    for (unsigned m =0; m < list_det_record.size(); ++m)
    {
      dbsks_xshock_det_record_sptr det_record = list_det_record[m];

      // Retrieve the image (object) name
      vcl_string object_name = "";
      det_record->get_value("object_name", object_name);

      map_obj_to_records[object_name].push_back(det_record);
    } // det_record
  } // det_record_file

  // Save the new record files
  for (vcl_map<vcl_string, vcl_vector<dbsks_xshock_det_record_sptr > >::iterator iter =
    map_obj_to_records.begin(); iter != map_obj_to_records.end(); ++iter)
  {
    vcl_string object_name = iter->first;

    // name pattern: xml_det_record+mugs_blue.xgraph.0+applelogos_another.xml
    vcl_string det_record_filename = "xml_det_record+" + 
      xgraph_prototype_name + "+" +
      object_name + ".xml";

    vcl_string det_record_file = output_folder + "/" + det_record_filename;

    // save the file
    x_write(det_record_file, iter->second);
  }
  return true;
}




//------------------------------------------------------------------------------
//: Filter detection results using geometric constraints;
bool dbsks_filter_dets_using_geometric_constraints()
{
  vcl_string xgraph_geom_file = //"D:/vision/projects/symseg/xshock/xshock-graph/xgraph_geom_model-mugs.xml";
  "v:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph/mugs-xgraph/xgraph_geom_model-mugs-v2.xml";
  vcl_string prototype_xgraph_file = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph/mugs-xgraph/mugs_apple.xgraph.0.xml";

  vcl_string test_xgraph_file = "D:/vision/projects/symseg/xshock/xshock-experiments/exp_86t_test_exemplars/mugs_blue.xgraph.0+mugs_tdnkitchen/mugs_blue.xgraph.0+mugs_tdnkitchen.0.xml";

  //test_xgraph_file = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph/mugs-xgraph/mugs_mat.xgraph.0.xml";

  // now load them all up

  // prototype xgraph
  dbsksp_xshock_graph_sptr prototype_xgraph;
  dbsks_load_xgraph(prototype_xgraph_file, prototype_xgraph);

  // geometric model
  dbsks_xgraph_geom_model_sptr xgraph_geom;
  dbsks_load_xgraph_geom_model(xgraph_geom_file, xgraph_geom);
  xgraph_geom->compute_attribute_constraints();

  // load test xgraph
  dbsksp_xshock_graph_sptr test_xgraph;
  dbsks_load_xgraph(test_xgraph_file, test_xgraph);
  test_xgraph->compute_vertex_depths(xgraph_geom->root_vid());

  double cur_graph_size = vcl_sqrt(test_xgraph->area());
  double model_graph_size = 100; // hack for now.

  //test_xgraph->scale_up(0, 0, model_graph_size / cur_graph_size);
  test_xgraph->scale_up(0, 0, 64.0 / 200);


  vcl_cout << "\nCheck geometric constraint for each edge of test xgraph:\n";
  const vcl_map<unsigned, dbsks_xfrag_geom_model_sptr >& map_edge2geom = xgraph_geom->map_edge2geom();
  for (vcl_map<unsigned, dbsks_xfrag_geom_model_sptr >::const_iterator iter = map_edge2geom.begin();
    iter != map_edge2geom.end(); ++iter)
  {
    unsigned eid = iter->first;
    dbsks_xfrag_geom_model_sptr xfrag_geom = iter->second;

    dbsksp_xshock_edge_sptr xe = test_xgraph->edge_from_id(eid);
    if (xe->is_terminal_edge())
      continue;

    dbsksp_xshock_node_sptr xv_p = xe->parent_node();
    dbsksp_xshock_node_sptr xv_c = xe->child_node();
    dbsksp_xshock_node_descriptor xdesc_p = *(xv_p->descriptor(xe));
    dbsksp_xshock_node_descriptor xdesc_c = xv_c->descriptor(xe)->opposite_xnode();
    dbsksp_xshock_fragment xfrag(xdesc_p, xdesc_c);

    bool pass = xfrag_geom->check_constraints(xfrag);
    vcl_cout << "eid = " << eid << (pass ? " yes " : " no ") << "\n";

  }
  
  

  return true;
}



