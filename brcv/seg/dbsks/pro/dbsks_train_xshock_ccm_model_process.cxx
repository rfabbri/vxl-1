// This is dbsks/pro/dbsks_train_xshock_ccm_model_process.cxx

//:
// \file

#include "dbsks_train_xshock_ccm_model_process.h"

#include <bpro1/bpro1_parameters.h>
#include <dbul/dbul_parse_simple_file.h>
//#include <vidpro1/storage/vidpro1_image_storage.h>


//#include <vil/vil_convert.h>
//#include <vil/vil_new.h>
//#include <vnl/vnl_math.h>

#include <dbsks/algo/dbsks_train_ccm_model.h>


//: Constructor
dbsks_train_xshock_ccm_model_process::
dbsks_train_xshock_ccm_model_process()
{
  if ( 
    //// number of data point for groundtruth xgraph
    //!parameters()->add("Number of samples per xgraph:", "-num_samples_per_xgraph", int(100))||
    //!parameters()->add("Random perturbation in x-direction (pixels):", "-perturb_delta_x", float(2)) ||
    //!parameters()->add("Random perturbation in y-direction (pixels):", "-perturb_delta_y", float(2)) ||
    //!parameters()->add("Base size for xgraph (square root of area, in pixels):", "-base_xgraph_size", float(64)) ||

    !parameters()->add("Train-xshock-CCM parameter file (.txt): ", "-train-xshock-ccm-file", bpro1_filepath("", "")) ||

    // image folder
    !parameters()->add("Image folder:", "-image_folder", bpro1_filepath("V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_originals")) ||

    // edge folder
    !parameters()->add("Edge map folder", "-edgemap_folder", bpro1_filepath("V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_pb_edges_pyramid-clean_using_kovesi-I_15-len_4")) ||
    !parameters()->add("edgemap extension (to add to image name):", "-edgemap_ext", vcl_string("_pb_edges.png")) ||
    !parameters()->add("edgeorient extension (to add to image name):", "-edgeorient_ext", vcl_string("_pb_orient.txt")) ||

    // Category-specific info
    //!parameters()->add("Object category:", "-category", vcl_string("applelogos")) ||
    !parameters()->add("File containing list of training positive images", "-list_pos_images", bpro1_filepath("V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph/list_applelogos_prototype1_pos_images.txt")) ||
    !parameters()->add("File containing list of training negative images", "-list_neg_images", bpro1_filepath("V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph/list_applelogos_prototype1_neg_images.txt")) ||
    !parameters()->add("Folder containing groundtruth xgraphs:", "-xgraph_folder", bpro1_filepath("V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph/applelogos-xgraph")) ||
    !parameters()->add("File containing list of groundtruth xgraphs:", "-list_gt_xgraph", bpro1_filepath("V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph/list_applelogos_prototype1_xml.txt")) ||
    !parameters()->add("Prototype xgraph file", "-prototype_xgraph_file", bpro1_filepath("V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph/applelogos-xgraph/applelogos_another.xgraph.0.prototype1.xml")) ||
    !parameters()->add("Root vertex id:", "-root_vid", unsigned(4)) ||
    !parameters()->add("Positive output data file:", "-pos_output_file", bpro1_filepath("D:/vision/projects/symseg/xshock/applelogos_prototype1-pos_ccm_cost-2009jul15-long-range.xml")) ||
    !parameters()->add("Negative output data file:", "-neg_output_file", bpro1_filepath("D:/vision/projects/symseg/xshock/applelogos_prototype1-neg_ccm_cost-2009jul15-long-range.xml")) ||
	!parameters()->add("Using Edgemap Pyramid:", "-use_edgemap_pyramid", bool(true))
  //
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbsks_train_xshock_ccm_model_process::
~dbsks_train_xshock_ccm_model_process()
{
}


//: Clone the process
bpro1_process* dbsks_train_xshock_ccm_model_process::
clone() const
{
  return new dbsks_train_xshock_ccm_model_process(*this);
}

//: Returns the name of this process
vcl_string dbsks_train_xshock_ccm_model_process::
name()
{ 
  return "Train xshock ccm model"; 
}

//: Provide a vector of required input types
vcl_vector< vcl_string > dbsks_train_xshock_ccm_model_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.clear();
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbsks_train_xshock_ccm_model_process::
get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.clear();
  return to_return;
}

//: Return the number of input frames for this process
int dbsks_train_xshock_ccm_model_process::
input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int dbsks_train_xshock_ccm_model_process::
output_frames()
{
  return 1;
}



//------------------------------------------------------------------------------
//: Execute this process
bool dbsks_train_xshock_ccm_model_process::
execute()
{
  // USER PARAMS --------------------------------------------------------------    
  dbsks_train_ccm_model_params params = dbsks_train_ccm_model_default_params("swans");

  bpro1_filepath temp_path;

  // parameter file
  this->parameters()->get_value("-train-xshock-ccm-file", temp_path);
  vcl_string params_file = temp_path.path;

  vcl_map<vcl_string, vcl_map<vcl_string, vcl_string > > section_params;
  dbul_parse_ini_file(params_file, section_params);

  vcl_map<vcl_string, vcl_string > ccm_params = section_params["ccm"];
  vcl_map<vcl_string, vcl_string > data_params = section_params["data"];

  // print out ccm parameters
  for (vcl_map<vcl_string, vcl_string >::iterator iter = ccm_params.begin(); 
    iter != ccm_params.end(); ++iter)
  {
    vcl_cout << iter->first << " " << iter->second << "\n";
  }

  // print out data parameters
  for (vcl_map<vcl_string, vcl_string >::iterator iter = data_params.begin(); 
    iter != data_params.end(); ++iter)
  {
    vcl_cout << iter->first << " " << iter->second << "\n";
  }

  // Parse these parameters and save to to the trainer
  params.parse_ccm_params(ccm_params);
  params.parse_data_params(data_params);



  // image folder
  this->parameters()->get_value("-image_folder", temp_path);
  params.image_folder = temp_path.path;

  // edge folder
  this->parameters()->get_value("-edgemap_folder", temp_path);
  params.edgemap_folder = temp_path.path;

  // edgemap extension
  this->parameters()->get_value("-edgemap_ext", params.edgemap_ext);
  this->parameters()->get_value("-edgeorient_ext", params.edgeorient_ext);

  // Category-specific info
  this->parameters()->get_value("-list_pos_images", temp_path);
  params.list_pos_images = temp_path.path;
  
  this->parameters()->get_value("-list_neg_images", temp_path);
  params.list_neg_images = temp_path.path;
  
  this->parameters()->get_value("-xgraph_folder", temp_path);
  params.xgraph_folder = temp_path.path;
  
  this->parameters()->get_value("-list_gt_xgraph", temp_path);
  params.list_gt_xgraph = temp_path.path;
  
  this->parameters()->get_value("-prototype_xgraph_file", temp_path);
  params.prototype_xgraph_file = temp_path.path;
  
  this->parameters()->get_value("-root_vid", params.root_vid);
  
  // Output files
  this->parameters()->get_value("-pos_output_file", temp_path);
  params.pos_output_file = temp_path.path;
  
  this->parameters()->get_value("-neg_output_file", temp_path);
  params.neg_output_file = temp_path.path;
  
  bool use_edgemap_pyramid;
  this->parameters()->get_value("-use_edgemap_pyramid", use_edgemap_pyramid);
  params.use_edgemap_pyramid = use_edgemap_pyramid;
  // STORAGE CLASSES ----------------------------------------------------------
  
  
  // PROCESS DATA -------------------------------------------------------------
  dbsks_train_ccm_model trainer;
  trainer.set_training_data_info(params);
  trainer.collect_positive_data();
  trainer.collect_negative_data();

  // OUTPUT DATA ---------------------------------------------------------------

  return true;
}


// ----------------------------------------------------------------------------
bool dbsks_train_xshock_ccm_model_process::
finish()
{
  return true;
}







