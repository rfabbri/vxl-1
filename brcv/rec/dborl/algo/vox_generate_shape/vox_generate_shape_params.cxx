
//:
// \file


#include "vox_generate_shape_params.h"
#include <dborl/algo/dborl_algo_params.h>
#include <vul/vul_file.h>
#include <vcl_cstdlib.h>
//#include <dborl/algo/dborl_utilities.h>

// -----------------------------------------------------------------------------
//: Constructor
vox_generate_shape_params::
vox_generate_shape_params(vcl_string algo_name) : 
  dborl_algo_params(algo_name) 
{

  //: xgraph Contour-Chamfer-Matching model param filename (.xml)
  this->input_xgraph_geom_param_filename_.set_values(this->param_list_, "io", "input_xgraph_geom_param_filename",
          "[MODEL-PARAM] Input xgraph geom model parameter filename (full path)",
          "/media/New_Volume/Research/Project_skeleton/Detect_xshock_BMS/training_set/geom_params.xml",
          "/media/New_Volume/Research/Project_skeleton/Detect_xshock_BMS/training_set/geom_params.xml");



  // Category-spectific info ////////////////////////////////////////////////////////

  // a hack to change default info for debugging purpose
  vcl_string category = "mouse";

  if(category == "mouse")
  {
    //: Object categery of input shock graph
    this->input_model_category_.set_values(this->param_list_, "io", "input_model_category", 
      "input model category", 
      "mouse", 
      "mouse");

    //: Folder containing the object shock graph
    this->input_xgraph_folder_.set_values(this->param_list_, "io", "input_xgraph_folder",
      "[XGRAPH] input folder to read xgraph file", 
      "/media/New_Volume/Research/Project_skeleton/Detect_xshock_BMS/training_set2",
      "/media/New_Volume/Research/Project_skeleton/Detect_xshock_BMS/training_set2");

    // XML filename of object
    this->input_xgraph_prototype_filename_.set_values(this->param_list_, "io", "input_xgraph_name", 
      "[XGRAPH] input xgraph name", 
      "0021_b.xgraph.0.xml", 
      "0021_b.xgraph.0.xml");

    //: xgraph geometric model filename (.xml)
    this->input_xgraph_geom_filename_.set_values(this->param_list_, "io", "input_xgraph_geom_filename",
      "[GEOM] Input xgraph geometric model filename",
      "test-geom-model-08272014.xml",
      "test-geom-model-08272014.xml");

  }
  else
  {
    vcl_cout << "\nERROR!!!! Unknown category. Many parameters were not set properly.\n";
  }

  
  //////////////////////////////////////////////////////////////////////////////
  // Output 

  // Save result to the object folder?
  this->save_to_object_folder_.set_values(this->param_list_, "io", "save_to_object_folder", 
    "[OUTPUT] save result to object folder?", false, false);
  
  // if written to this folder as opposed to object folder 
  this->output_object_folder_.set_values(this->param_list_, "io", "output_object_folder", 
    "[OUTPUT] output folder to write results", 
    "/media/New_Volume/Research/Project_skeleton/Detect_xshock_BMS/shape_factory",
    "/media/New_Volume/Research/Project_skeleton/Detect_xshock_BMS/shape_factory");

    this->num_samples_.set_values(this->param_list_, "io", "num_samples",
      "Number of samples intend to generate",
      200,
      200);

  // tag for this algorithm
  this->algo_abbreviation_ = "generate_shape";

}


//------------------------------------------------------------------------------  
//:
vcl_string vox_generate_shape_params::
get_xgraph_file() const
{
  vcl_string xgraph_file = this->input_xgraph_folder_()+ "/" +this->input_xgraph_prototype_filename_();
  return xgraph_file;
}


//------------------------------------------------------------------------------
//: xgraph geometric model
vcl_string vox_generate_shape_params::
get_xgraph_geom_file() const
{
  vcl_string xgraph_geom_file = this->input_xgraph_folder_()+ "/" +this->input_xgraph_geom_filename_();
  return xgraph_geom_file;
}

//: xgraph geometric model
vcl_string vox_generate_shape_params::
get_xgraph_geom_param_file() const
{
  vcl_string xgraph_geom_param_file = this->input_xgraph_geom_param_filename_();
  return xgraph_geom_param_file;
}



// location of all output files
vcl_string vox_generate_shape_params::
get_output_folder() const
{
  if (this->save_to_object_folder_())
  {
    return this->input_object_dir_();
  }
  else
  {
    return this->output_object_folder_();
  }
}



//: Name of the prototype xgraph
vcl_string vox_generate_shape_params::
get_xgraph_prototype_name() const
{
  return vul_file::strip_extension(this->input_xgraph_prototype_filename_());
}
