// This is rec/dborl/algo/vox_xgraph_edit_distance/vox_xgraph_edit_distance_one_to_many.cxx


//:
// \file

#include "vox_xgraph_edit_distance_one_to_many.h"
#include "vox_xgraph_edit_distance_params.h"


#include <dbsk2d/pro/dbsk2d_shock_storage_sptr.h>
#include <dbsk2d/pro/dbsk2d_load_esf_process.h>

#include <dbsksp/pro/dbsksp_xgraph_storage_sptr.h>
#include <dbsksp/pro/dbsksp_xgraph_storage.h>
#include <dbsksp/pro/dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process.h>
#include <dbsksp/pro/dbsksp_normalize_xgraph_process.h>
#include <dbsksp/pro/dbsksp_mirror_xgraph_process.h>
#include <dbsksp/xio/dbsksp_xio_xshock_graph.h>

#include <dborl/algo/dborl_index_parser.h>
#include <dborl/dborl_index.h>
#include <dborl/dborl_index_node.h>

#include <dbxml/dbxml_xio.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_numeric_traits.h>
#include <vul/vul_file.h>

//==============================================================================
// vox_xgraph_edit_distance_one_to_many
//==============================================================================

//: Constructor
vox_xgraph_edit_distance_one_to_many::
vox_xgraph_edit_distance_one_to_many(vox_xgraph_edit_distance_params_sptr params):
  params_(params)
{

}

//------------------------------------------------------------------------------
  // Called by main statement to intiate algorithm on vox
bool vox_xgraph_edit_distance_one_to_many::
process()
{
  bool status = false;

  // Read index file is also performed in initialize process
  status = this->initialize();
  if ( status )
  {
    // Perform averaging
    status = this->perform_distance_computation();
    if ( status )
    {
      // Finally write out results
      status = this->write_out();
    }
  }

  return status;
}


//------------------------------------------------------------------------------
//: Initialize parameter settings
bool vox_xgraph_edit_distance_one_to_many::
initialize()
{
  //1) Parse list of esf files of model objects
  dborl_index_parser parser;
  parser.clear();
  dborl_index_sptr dataset_index = dborl_index_parser::parse(params_->model_dataset_index_file_(),parser);

  if ( !dataset_index )
  {
    vcl_cerr<<"\nParing index file failed!\n";
    return false;
  }

    
  // Loop over index file and compare query shock against all 
  // shapes within index file
  dborl_index_node_sptr index_node = dataset_index->root_->cast_to_index_node();
  unsigned num_shapes = index_node->paths().size();

  this->model_esf_files_.clear();
  this->model_object_names_.clear();

  for (unsigned i =0; i < num_shapes; ++i)
  {
    vcl_string object_name = index_node->names()[i];
    // form the full file path for the esf file
    vcl_string esf_file = index_node->paths()[i] + "/" + object_name + ".esf";

    if (!vul_file::exists(esf_file)) 
    {
      vcl_cerr << "\nCannot find shock (.esf) files: " << esf_file << "\n";
      return false;
    }
    this->model_esf_files_.push_back(esf_file);
    this->model_object_names_.push_back(object_name);
  }


  // Print out list of object names
  vcl_cout << "\nModel dataset has " << this->model_object_names_.size() << " objects:\n";
  for (unsigned i =0; i < this->model_object_names_.size(); ++i)
  {
    vcl_cout << "\n  " << this->model_object_names_[i];
  }
  vcl_cout << "\n";


  //2) Parse query object

  // object name
  this->query_object_name_ = this->params_->query_object_name_();

  // esf file
  this->query_esf_file_ = this->params_->query_object_dir_() + "/" + this->query_object_name_ + ".esf";

  vcl_cout << "Query object name: " << this->query_object_name_ << "\n";


  //3) Set up folder for output

  // location of output dir
  this->output_folder_ = this->params_->output_folder_();

  // create if not yet exist
  if (!vul_file::is_directory(this->output_folder_))
  {
    vul_file::make_directory(this->output_folder_);
  }

  // Prefix to save output resutls
  this->output_prefix_ = this->output_folder_ + "/" + this->query_object_name_ + "+" + this->params_->model_dataset_name_();

  //4) save input parameter file
  this->params_->print_input_xml(this->output_prefix_ + "-input.xml");

  return true;
}



//------------------------------------------------------------------------------
//:
bool vox_xgraph_edit_distance_one_to_many::
perform_distance_computation()
{
  //1) Load esf file of query object
  dbsksp_xshock_graph_sptr query_xgraph;
  this->load_esf(this->query_esf_file_, query_xgraph, 
    this->params_->use_existing_xgraph_file_(),
    this->params_->query_xgraph_extension_());

  if (!query_xgraph)
    return false;

  //: normalize query xgraph if requested
  if (this->params_->normalize_before_computing_distance_())
  {
    query_xgraph = this->normalize_xgraph(query_xgraph);
  }

  dbsksp_xshock_graph_sptr query_mirrored = 0;
  if (this->params_->use_mirrored_shape_())
  {
    query_mirrored = this->mirror_xgraph(query_xgraph);  
  }

  //2) Compute distance for each model shock graph
  unsigned num_models = this->model_object_names_.size();
  this->distance_query_to_models_.resize(num_models, vnl_numeric_traits<double >::maxval);
  for (unsigned i =0; i < this->model_object_names_.size(); ++i)
  {
    vcl_string model_esf_file = this->model_esf_files_[i];

    // Load model shock graph
    dbsksp_xshock_graph_sptr model_xgraph = 0;
    this->load_esf(model_esf_file, model_xgraph,
      this->params_->use_existing_xgraph_file_(),
      this->params_->model_xgraph_extension_());

    if (!model_xgraph)
    {
      vcl_cout << "\nERROR: failed to load model xgraph: " << this->model_object_names_[i] << "\n";
      continue;
    }

    // Do we need to normalize before computing distance?
    if (this->params_->normalize_before_computing_distance_())
    {
      model_xgraph = this->normalize_xgraph(model_xgraph);
    }

    // Compute edit distance
    dbsksp_edit_distance work;
    double distance = this->compute_edit_distance(query_xgraph, model_xgraph, work);

    // Check distance of mirrored shape too?
    if (query_mirrored)
    {
      double distance2 = this->compute_edit_distance(query_mirrored, model_xgraph, work);
      distance = vnl_math::min(distance, distance2);
    }
    

    this->distance_query_to_models_[i] = distance;

    // write out
    vcl_cout 
      << "query = " << this->query_object_name_ << ", "
      << "model = " << this->model_object_names_[i] << ", "
      << "distance = " << distance << "\n";

    // Update status results
    this->params_->percent_completed = ((float)(i+1)/num_models)*100.0f;
    this->params_->print_status_xml();
  }
  return true;
}




//------------------------------------------------------------------------------
//: Compute distance between two xgraphs - keeping the intermediate work
double vox_xgraph_edit_distance_one_to_many::
compute_edit_distance(const dbsksp_xshock_graph_sptr& xgraph1, 
                      const dbsksp_xshock_graph_sptr& xgraph2,
                      dbsksp_edit_distance& work)
{
  dbsksp_edit_distance edit_distance;
  edit_distance.set(xgraph1, xgraph2, this->params_->average_xgraph_scurve_matching_R_(), 
    this->params_->average_xgraph_scurve_sample_ds_());
  edit_distance.save_path(true);
  edit_distance.edit();
  work = edit_distance;
  return edit_distance.final_cost();
}




//------------------------------------------------------------------------------
//: Load esf file
bool vox_xgraph_edit_distance_one_to_many::
load_esf(const vcl_string& esf_file, dbsksp_xshock_graph_sptr& xgraph, 
    bool use_existing_xgraph, const vcl_string& xgraph_extension)
{
  // sanitize output container
  xgraph = 0;

  //0) If xgraph file exists, load it (instead of converting from esf).
  if (use_existing_xgraph)
  {
    vcl_string xgraph_file = vul_file::strip_extension(esf_file) + xgraph_extension;
    if (vul_file::exists(xgraph_file) && x_read(xgraph_file, xgraph))
    {
      vcl_cout << "\nLoaded existing xgraph file in object folder: " 
        << vul_file::strip_directory(xgraph_file) << ".\n";
      return true;
    }
    else // reset xgraph to 0
    {
      xgraph = 0;
    }
  }


  //1) Load esf file

  dbsk2d_load_esf_process load_process;
  load_process.parameters()->set_value("-esfinput" , bpro1_filepath(esf_file));
  load_process.clear_input();
  load_process.clear_output();

  bool success = load_process.execute();
  if (!success)
    return false;

  // retrieve output
  dbsk2d_shock_storage_sptr sk2d_storage = 0;
  sk2d_storage.vertical_cast(load_process.get_output()[0]);


  //2) Convert dbsk2d_shock_graph to dbsksp_xshock_graph

  // set parameters for storage
  dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process fit_process;
  float tol = this->params_->convert_sk2d_to_sksp_tol_();

  fit_process.parameters()->set_value("-distance-rms-error-threshold", tol);
  fit_process.clear_input();
  fit_process.clear_output();
  fit_process.add_input(sk2d_storage.ptr());
  success = fit_process.execute();

  if (!success)
    return false;

  // retrieve output
  dbsksp_xgraph_storage_sptr sksp_storage;
  sksp_storage.vertical_cast(fit_process.get_output()[0]);  

  //3) Save output
  xgraph = sksp_storage->xgraph();
  vcl_cout << "\nConverted esf file:[ " << vul_file::strip_directory(esf_file) 
    << " ] to xgraph successfully.\n";
  return true;  
}








//------------------------------------------------------------------------------
//: Normalize xgraph
dbsksp_xshock_graph_sptr vox_xgraph_edit_distance_one_to_many::
normalize_xgraph(const dbsksp_xshock_graph_sptr& xgraph)
{
  //1) Construct input storage
  dbsksp_xgraph_storage_sptr input_storage = dbsksp_xgraph_storage_new();
  input_storage->set_xgraph(xgraph);

  //2) Configure normalization process
  dbsksp_normalize_xgraph_process normalize_process;
  normalize_process.parameters()->set_value("-norm_shape_size", this->params_->norm_shape_size_());
  normalize_process.clear_input();
  normalize_process.clear_output();
  normalize_process.add_input(input_storage);
  bool success = normalize_process.execute();
  if (!success)
    return false;

  //3) retrieve output
  dbsksp_xgraph_storage_sptr output_storage = 0;
  output_storage.vertical_cast(normalize_process.get_output()[0]);

  return output_storage->xgraph();
}






//------------------------------------------------------------------------------
//: Mirror an xgraph around y-axis
dbsksp_xshock_graph_sptr vox_xgraph_edit_distance_one_to_many::
mirror_xgraph(const dbsksp_xshock_graph_sptr& xgraph)
{
  //1) Construct input storage
  dbsksp_xgraph_storage_sptr input_storage = dbsksp_xgraph_storage_new();
  input_storage->set_xgraph(xgraph);

  //2) Configure normalization process
  dbsksp_mirror_xgraph_process mirror_process;
  mirror_process.parameters()->set_value("-mirror_y_axis_bbox_center", true);
  mirror_process.clear_input();
  mirror_process.clear_output();
  mirror_process.add_input(input_storage);
  bool success = mirror_process.execute();
  if (!success)
    return false;

  //3) retrieve output
  dbsksp_xgraph_storage_sptr output_storage = 0;
  output_storage.vertical_cast(mirror_process.get_output()[0]);
  
  return output_storage->xgraph();
}





//------------------------------------------------------------------------------
//: Write out results
bool vox_xgraph_edit_distance_one_to_many::
write_out()
{
  //1) Checks
  if (this->output_prefix_.empty())
    return false;

  if (this->distance_query_to_models_.empty())
    return false;

  //2) Save edit distance
  vcl_string data_file = this->output_prefix_ + "-xgraph_edit_distance.xml";

  // write an xml document for output
  bxml_document doc;
  bxml_element *root = new bxml_element("xgraph_edit_distance");
  doc.set_root_element(root);

  root->set_attribute("version", "1");
  root->set_attribute("model_dataset_name", this->params_->model_dataset_name_());
  root->set_attribute("query_object_name", this->params_->query_object_name_());

  //a) set of parameters used for averaging
  bxml_element* params_elm = new bxml_element("params");
  root->append_data(params_elm);

  // fitting tolerance
  params_elm->append_data(xml_new(this->params_->convert_sk2d_to_sksp_tol_.param_name(), 
    this->params_->convert_sk2d_to_sksp_tol_()));

  // param R in shock matching
  params_elm->append_data(xml_new(this->params_->average_xgraph_scurve_matching_R_.param_name(),
    this->params_->average_xgraph_scurve_matching_R_()));

  // sample_ds in in shock matching
  params_elm->append_data(xml_new(this->params_->average_xgraph_scurve_sample_ds_.param_name(),
    this->params_->average_xgraph_scurve_sample_ds_()));

  //b) set of parents and distance from the average to the parents
  bxml_element* distances_elm = new bxml_element("xgraph_distance_list");
  root->append_data(distances_elm);
  
  distances_elm->set_attribute("size", this->model_object_names_.size());

  for (unsigned i =0; i < this->model_object_names_.size(); ++i)
  {
    bxml_element* distance_elm = new bxml_element("xgraph_distance");
    distances_elm->append_data(distance_elm);
    distance_elm->set_attribute("index", i);
    distance_elm->append_data(xml_new("model_name", this->model_object_names_[i]));
    distance_elm->append_data(xml_new("query_name", this->query_object_name_));
    distance_elm->append_data(xml_new("distance", this->distance_query_to_models_[i]));
  }

  
  // actually save the -data xml file
  bxml_write(data_file, doc);

  return true;
}


