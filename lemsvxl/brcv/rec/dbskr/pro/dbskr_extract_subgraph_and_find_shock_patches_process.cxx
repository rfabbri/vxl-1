// This is dbskr/pro/dbskr_extract_subgraph_and_find_shock_patches_process.cxx

//:
// \file

#include "dbskr_extract_subgraph_and_find_shock_patches_process.h"
#include <bpro1/bpro1_parameters.h>
#include <dbskr/algo/dbskr_shock_patch_selector.h>
#include <vul/vul_file.h>




//:
dbskr_extract_subgraph_and_find_shock_patches_process::
dbskr_extract_subgraph_and_find_shock_patches_process() : bpro1_process()
{
  if ( !parameters()->add( "Image file:" , "-image_filepath", bpro1_filepath("", "*.bin")) ||
    !parameters()->add( "ESF file:" , "-esf_filepath", bpro1_filepath("", "*.esf")) ||
    !parameters()->add( "Output folder:" , "-output_folder", bpro1_filepath("", "")) ||
    !parameters()->add( "Output file prefix:" , "-output_prefix", vcl_string("")) ||
    !parameters()->add( "Contour ratio?" , "-contour_ratio", false ) ||
    !parameters()->add( "Circular ends?" , "-circular_ends", false ) ||
    !parameters()->add( "Area threshold ratio:" , "-area_thresh_ratio", (float) 0.01f ) ||
    !parameters()->add( "Overlap ratio:" , "-overlap_threshold", (float) 0.8f ) ||
    !parameters()->add( "Min depth:" , "-min_depth", (int) 1 ) ||
    !parameters()->add( "Max depth:" , "-max_depth", (int) 3 ) ||
    !parameters()->add( "Depth interval:" , "-depth_int", (int) 1 ) ||
    !parameters()->add( "Pruning depth:" , "-pruning_depth", (int) 2 ) ||
    !parameters()->add( "Sort threshold:" , "-sort_threshold", (float) 0.5 ) ||
    !parameters()->add( "Keep pruned patches?" , "-keep_pruned", true ) ||
    !parameters()->add( "Save patch images?" , "-save_images", true ) ||
    !parameters()->add( "Save discarded images?" , "-save_disc_images", false )
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


// -----------------------------------------------------------------------------
//: Clone the process
bpro1_process* dbskr_extract_subgraph_and_find_shock_patches_process::
clone() const
{
  return new dbskr_extract_subgraph_and_find_shock_patches_process(*this);
}



// -----------------------------------------------------------------------------
//:
vcl_vector< vcl_string > dbskr_extract_subgraph_and_find_shock_patches_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.clear();
  return to_return;
}


// -----------------------------------------------------------------------------
//:
vcl_vector< vcl_string > dbskr_extract_subgraph_and_find_shock_patches_process::
get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.clear();
  return to_return;
}

// -----------------------------------------------------------------------------
//:
bool dbskr_extract_subgraph_and_find_shock_patches_process::
execute()
{
  // Process parameters
  vcl_string image_file; 
  vcl_string esf_file;
  vcl_string boundary_file;
  vcl_string kept_dir_name; 
  vcl_string discarded_dir_name;
  vcl_string output_name;
  bool contour_ratio;
  bool circular_ends; 
  float area_threshold_ratio;
  float overlap_threshold;
  int min_depth;
  int max_depth; 
  int depth_int; 
  int pruning_depth; 
  float sort_threshold;
  bool keep_pruned;
  bool save_images; 
  bool save_discarded_images;


  bpro1_filepath image_filepath;
  this->parameters()->get_value("-image_filepath", image_filepath);
  image_file = image_filepath.path;

  bpro1_filepath esf_filepath;
  this->parameters()->get_value("-esf_filepath", esf_filepath);
  esf_file = esf_filepath.path;

  
  bpro1_filepath output_folder_filepath;
  this->parameters()->get_value("-output_folder", output_folder_filepath);
  vcl_string output_folder = output_folder_filepath.path;

  vcl_string output_prefix;
  this->parameters()->get_value("-output_prefix", output_prefix);
  this->parameters()->get_value("-contour_ratio", contour_ratio );
  this->parameters()->get_value("-circular_ends", circular_ends );
  this->parameters()->get_value("-area_thresh_ratio", area_threshold_ratio);
  this->parameters()->get_value("-overlap_threshold", overlap_threshold );
  this->parameters()->get_value("-min_depth", min_depth );
  this->parameters()->get_value("-max_depth", max_depth ); 
  this->parameters()->get_value("-depth_int", depth_int ); 
  this->parameters()->get_value("-pruning_depth", pruning_depth);
  this->parameters()->get_value("-sort_threshold", sort_threshold);
  this->parameters()->get_value("-keep_pruned", keep_pruned );
  this->parameters()->get_value("-save_images", save_images );
  this->parameters()->get_value("-save_disc_images", save_discarded_images );

  kept_dir_name = output_folder + "/kept/";
  discarded_dir_name = output_folder + "/discarded/";
  output_name = output_folder + "/" + output_prefix + "-patch_strg.bin";



  // Run the process
  vul_file::make_directory(kept_dir_name);
  vul_file::make_directory(discarded_dir_name);



  //////////////////////////////////////////////////////////////////////////////////////////
  extract_subgraph_and_find_shock_patches(image_file, esf_file, boundary_file, 
    kept_dir_name, discarded_dir_name, output_name, contour_ratio, circular_ends, 
    area_threshold_ratio, overlap_threshold, min_depth, max_depth, depth_int, pruning_depth, 
    sort_threshold, keep_pruned, save_images, save_discarded_images);
  //////////////////////////////////////////////////////////////////////////////////////////

  return true;
}
