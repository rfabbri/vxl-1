// This is dbsks/algo/dbsks_detect_xgraph_local_min.cxx

//:
// \file

#include "dbsks_detect_xgraph_local_min.h"

//#include <dbsks/dbsks_det_desc_xgraph.h>
//#include <dbsks/dbsks_det_desc_xgraph_sptr.h>
//#include <dbsks/dbsks_det_nms.h>
//#include <dbsks/dbsks_biarc_sampler.h>
//#include <dbsks/dbsks_xshock_detector.h>
//#include <dbsks/dbsks_xgraph_geom_model.h>
//#include <dbsks/dbsks_xgraph_ccm_model.h>
//#include <dbsks/dbsks_xshock_ccm_likelihood.h>
//#include <dbsks/xio/dbsks_xio_xshock_det.h>
//
//#include <dbsks/algo/dbsks_load.h>
//#include <dbsks/algo/dbsks_vox_utils.h>
//#include <dbsks/algo/dbsks_algos.h>
//
#include <dbsksp/dbsksp_xshock_graph.h>
//
//#include <dbdet/algo/dbdet_resize_edgemap.h>
//
//#include <vsol/vsol_box_2d.h>
//#include <bpro1/bpro1_parameters.h>
//#include <vil/vil_image_resource.h>
//#include <vil/file_formats/vil_pyramid_image_list.h>
//#include <vil/vil_load.h>
//#include <vil/vil_convert.h>
//
//#include <vul/vul_file.h>
//#include <vul/vul_file_iterator.h>
//#include <vul/vul_timer.h>
//#include <vul/vul_sprintf.h>


//------------------------------------------------------------------------------
//:
bool dbsks_detect_xgraph_local_min::
execute()
{
  //1) Load params and models
  this->load_params_and_models();

  //2) Load metainfo of edge map pyramid
  this->load_edgemap_pyramid();

  //3) Vary xgraph to maximize likelihood
  this->optimize_xgraph();

  return true;
}


//------------------------------------------------------------------------------
//: Load all data from input files
bool dbsks_detect_xgraph_local_min::
load_params_and_models()
{
  //// Load data from the files

  //// image
  //vil_image_resource_sptr image_resource;
  //if (!dbsks_load_image_resource(image_file, image_resource))
  //{
  //  return false;
  //}
  //source_image = *vil_convert_cast(vxl_byte(), image_resource->get_view());
  //

  //// xgraph
  //if (!dbsks_load_xgraph(xgraph_file, xgraph_prototype_))
  //{
  //  return false;
  //}

  //// geometric model
  //if (!dbsks_load_xgraph_geom_model(xgraph_geom_file, xgraph_geom))
  //{
  //  return false;
  //}
  //xgraph_geom->compute_attribute_constraints();

  //// Check compatibility between the geometric model and the shock graph (are all edges covered?)
  //vcl_cout << "\n>> Checking compatibility between geometric model and xgraph...";
  //if (!xgraph_geom->is_compatible(xgraph_prototype_))
  //{
  //  vcl_cout << "Failed\n." << vcl_endl;
  //  return false;
  //}
  //else
  //{
  //  vcl_cout << "Passed\n." << vcl_endl;
  //}

  //// ccm model
  //dbsks_load_xgraph_ccm_model(xgraph_ccm_file, xgraph_ccm);

  //// Check compatibility between Contour Chamfer Matching model and xgraph (are all edges covered)
  //vcl_cout << ">> Checking compatibility between CCM model and xgraph...";
  //if (!xgraph_ccm->is_compatible(xgraph_prototype_))
  //{
  //  vcl_cout << "Failed\n." << vcl_endl;
  //  return false;
  //}
  //else
  //{
  //  vcl_cout << "Passed\n." << vcl_endl;
  //}

  //// Set distributions of user-selected boundary fragments to constant
  //vcl_cout << "\n>> Overriding 'ignored' edges with constant distribution...";
  //if (!xgraph_ccm->override_cfrag_with_constant_distribution(cfrag_list_to_ignore))
  //{
  //  vcl_cout << "[ Failed ]\n";
  //  return false;
  //}
  //else
  //{
  //  vcl_cout << "[ OK ]\n";
  //}

  ////: Compute cache values for ccm models
  //xgraph_ccm->compute_cache_loglike_for_all_edges();

  ////: Construct biarc sampler from xgraph_ccm model
  //xgraph_ccm->build_biarc_sampler(this->biarc_sampler);
  //this->biarc_sampler.compute_cache_sample_points();
  //this->biarc_sampler.compute_cache_nkdiff();
  //


  return true;
}






//// local data structure to facilitate sorting the edgemap by their width
//struct edgemap_level_info
//{
//  vcl_string base_name;
//  int width;
//  double scale;
//};


////------------------------------------------------------------------------------
////: Load edge map pyramid
//bool dbsks_detect_xgraph_local_min::
//load_edgemap_pyramid()
//{
//  //>> Load all edgemaps in the pyramid ........................................
//  vcl_cout << "\n>> Loading all edgemap images in the pyramid ... ";
//
//  // regular expression to iterate thru edgemap files
//  vcl_string edgemap_regexp = edgemap_folder + "/" + object_id +  "/" + object_id + "*" + edgemap_ext;
//  
//  // clean up any existing data
//  this->list_edgemap_base_name.clear();
//  this->list_edgemap_width.clear();
//  this->list_edgemap_scale.clear();
//  double image_width = this->source_image.ni();
//
//  
//
//  // sort the edgemaps by their width, decreasing order
//  vcl_map<int, edgemap_level_info> map_width2info;
//  for (vul_file_iterator fn = edgemap_regexp; fn; ++fn)
//  {
//    vil_image_resource_sptr img = vil_load_image_resource(fn());
//    if (img)
//    {
//      edgemap_level_info info;
//      info.width = int(img->ni());
//      info.scale = img->ni() / image_width;
//      // base name: computed by taking away the "edgemap_ext" part of file name
//      // note that "vul_file::strip_extension(...) will not work because 
//      // "edgemap_ext" may containt a dot ".", which will confuse the 
//      // vul_file::strip_extension(...) function
//      vcl_string fname = vul_file::strip_directory(fn());
//      info.base_name = fname.substr(0, fname.size()-edgemap_ext.size());
//      map_width2info.insert(vcl_make_pair(-info.width, info));
//    }
//  }
//
//  // put the info back in the form we're familiar with
//  for (vcl_map<int, edgemap_level_info>::iterator iter = map_width2info.begin();
//    iter != map_width2info.end(); ++iter)
//  {
//    edgemap_level_info info = iter->second;
//    this->list_edgemap_width.push_back(info.width);
//    this->list_edgemap_scale.push_back(info.scale);
//    this->list_edgemap_base_name.push_back(info.base_name); 
//  }
//  return true;
//}



//------------------------------------------------------------------------------
//: Optimize xgraph
bool dbsks_detect_xgraph_local_min::
optimize_xgraph()
{
  // Initial a model xgraph (for optimization) with the loaded xgraph
  dbsksp_xshock_graph_sptr model_xgraph = new dbsksp_xshock_graph(*this->init_xgraph_);




  return false;
}