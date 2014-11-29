// This is rec/dborl/algo/vox_average_n_xgraphs/vox_average_n_xgraphs_one_to_many.cxx


//:
// \file

#include "vox_average_n_xgraphs_greedy.h"
#include "vox_average_n_xgraphs_params.h"


#include <dbsksp/pro/dbsksp_xgraph_storage.h>
#include <dbsksp/pro/dbsksp_average_xgraph_storage.h>
#include <dbsksp/pro/dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process.h>
#include <dbsksp/algo/dbsksp_average_n_xgraphs.h>
#include <dbsk2d/pro/dbsk2d_load_esf_process.h>
#include <dborl/algo/dborl_index_parser.h>
#include <dborl/dborl_index.h>
#include <dborl/dborl_index_node.h>
#include <dbsksp/algo/dbsksp_screenshot.h>
#include <dbsksp/xio/dbsksp_xio_xshock_graph.h>
#include <dbsksp/algo/dbsksp_avg_n_xgraphs_grad_descent.h>

#include <dbxml/dbxml_xio.h>
#include <dbxml/dbxml_vector_xio.h>
#include <dbxml/dbxml_algos.h>

#include <vsol/vsol_box_2d.h>
#include <vnl/vnl_math.h>
#include <vil/vil_save.h>


#include <vul/vul_file.h>

//==============================================================================
// vox_average_n_xgraphs_greedy
//==============================================================================

//: Constructor
vox_average_n_xgraphs_greedy::
vox_average_n_xgraphs_greedy(vox_average_n_xgraphs_params_sptr params):
  params_(params)
{
  this->method_name_ = "";
}

//------------------------------------------------------------------------------
  // Called by main statement to intiate algorithm on vox
bool vox_average_n_xgraphs_greedy::
process()
{
  bool status = false;

  // Read index file is also performed in initialize process
  status = this->initialize();
  if ( status )
  {
    // Perform averaging
    status = this->perform_averaging();
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
bool vox_average_n_xgraphs_greedy::
initialize()
{
  //1) Parse index file to form a list of esf files

  dborl_index_parser parser;
  parser.clear();
  dborl_index_sptr dataset_index = dborl_index_parser::parse(params_->index_file_(),parser);

  if ( !dataset_index )
  {
    vcl_cerr<<"\nParing index file failed!\n";
    return false;
  }

    
  // Loop over index file and compare query shock against all 
  // shapes within index file
  dborl_index_node_sptr index_node = dataset_index->root_->cast_to_index_node();

  unsigned start_index = this->params_->dataset_start_index_();
  unsigned end_index = vnl_math::min(this->params_->dataset_end_index_()+1, unsigned(index_node->paths().size()));
    
    //unsigned num_shapes = vnl_math::min(unsigned(index_node->paths().size()), unsigned(this->params_->num_xgraphs_to_average_()));

  this->list_esf_file_.clear();
  this->list_object_name_.clear();

  for (unsigned i =start_index; i < end_index; ++i)
  {
    vcl_string object_name = index_node->names()[i];
    // form the full file path for the esf file
    vcl_string esf_file = index_node->paths()[i] + "/" + object_name + ".esf";

    if (!vul_file::exists(esf_file)) 
    {
      vcl_cerr << "\nCannot find shock (.esf) files: " << esf_file << "\n";
      return false;
    }
    this->list_esf_file_.push_back(esf_file);
    this->list_object_name_.push_back(object_name);
  }


  // Print out list of object names
  vcl_cout << "\nDataset has " << this->list_object_name_.size() << " objects:\n";
  for (unsigned i =0; i < this->list_esf_file_.size(); ++i)
  {
    vcl_cout << "\n  " << this->list_object_name_[i];
  }
  vcl_cout << "\n";

  //2) set-up folder for intermediate fields

  // location of output dir
  this->output_folder_ = this->params_->output_average_xgraph_folder_();

  // create if not yet exist
  if (!vul_file::is_directory(this->output_folder_))
  {
    vul_file::make_directory(this->output_folder_);
  }

  // base name is the prefix for all other intermediate output
  this->base_name_ = this->output_folder_ + "/" + this->params_->dataset_object_name_();

  //3) save input parameter file
  this->params_->print_input_xml(this->base_name_ + "-input.xml");

  return true;
}



//------------------------------------------------------------------------------
//: Actual matching performed
bool vox_average_n_xgraphs_greedy::
perform_averaging()
{
  // 1) Load the two esf files
  vcl_cout << "\n>>Load all esf files ...\n";
  
  // Output container:
  vcl_vector<dbsk2d_shock_storage_sptr > list_sk2d_storage;

  // Status
  bool load_status = true;
  for (unsigned i =0; i < this->list_esf_file_.size(); ++i)
  {
    vcl_cout << "\nLoading " << this->list_esf_file_[i] << "...";
    dbsk2d_shock_storage_sptr shock_storage;
    load_status &= this->load_esf(this->list_esf_file_[i], shock_storage);
    list_sk2d_storage.push_back(shock_storage);
    if (!load_status)
    {
      vcl_cout << "[ Failed ]\n";
      return false;
    }
    else
    {
      vcl_cout << "[ OK ]";
    }
  }

  //2) Convert dbsk2d_shock_graph to dbsksp_xshock_graph
  vcl_cout << "\n>>Convert dbsk2d_shock_graph to dbsksp_xshock_graph ...\n";

  // Output container
  vcl_vector<dbsksp_xgraph_storage_sptr > list_xgraph_storage;
  for (unsigned i =0; i < list_sk2d_storage.size(); ++i)
  {
    vcl_cout << "\n  Converting sk2d_storage i =" << i << "...";
    dbsksp_xgraph_storage_sptr xgraph_storage;
    bool convert_status = this->convert_sk2d_to_sksp(list_sk2d_storage[i], xgraph_storage);
    if (!convert_status)
    {
      vcl_cout << "[ Failed ]\n";
      return false;
    }
    else
    {
      list_xgraph_storage.push_back(xgraph_storage);
      vcl_cout << "[ OK ]";
    }
  }


  // Collect all the xgraph files
  this->list_xgraph_.clear();
  for (unsigned i =0; i < list_xgraph_storage.size(); ++i)
  {
    this->list_xgraph_.push_back(list_xgraph_storage[i]->xgraph());  
  }
  

  //3) Average list of load xgraphs
  vcl_cout << 
    "\n[Begin]----------------------------------------------------------------\n";
  
  vcl_cout << "Compute average of N xgraphs";
  
  bool average_status = this->compute_average();
  if (!average_status)
  {
    vcl_cout << "[ Failed ]<<\n";
    return false;
  }
  else
  {
    vcl_cout << "[ OK ]<<\n";
  }

  vcl_cout << 
    "\n[End]----------------------------------------------------------------\n";

  return true;
}





//------------------------------------------------------------------------------
//: Load esf file
bool vox_average_n_xgraphs_greedy::
load_esf(const vcl_string& esf_file, dbsk2d_shock_storage_sptr& shock_storage)
{
  // sanitize output container
  shock_storage = 0;

  dbsk2d_load_esf_process load_process;
  load_process.parameters()->set_value("-esfinput" , bpro1_filepath(esf_file));
  load_process.clear_input();
  load_process.clear_output();

  bool success = load_process.execute();
  if (success)
  {
    shock_storage.vertical_cast(load_process.get_output()[0]);
    return true;
  }
  else
  {
    return false;
  }
}





//------------------------------------------------------------------------------
//: Convert sk2d graph to sksp graph
bool vox_average_n_xgraphs_greedy::
convert_sk2d_to_sksp(const dbsk2d_shock_storage_sptr& sk2d_storage,
                     dbsksp_xgraph_storage_sptr& sksp_storage)
{
  // sanitize output container
  sksp_storage = 0;

  // set parameters for storage
  dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process fit_process;
  float tol = this->params_->convert_sk2d_to_sksp_tol_();

  fit_process.parameters()->set_value("-distance-rms-error-threshold", tol);
  fit_process.clear_input();
  fit_process.clear_output();
  fit_process.add_input(sk2d_storage.ptr());
  bool success = fit_process.execute();

  if (!success)
  {
    return false;
  }
  else
  {
    sksp_storage.vertical_cast(fit_process.get_output()[0]);  
    return true;
  }
}



//------------------------------------------------------------------------------
//: Compute average of N xgraphs
bool vox_average_n_xgraphs_greedy::
compute_average()
{
  // average xgraph
  dbsksp_xshock_graph_sptr average_xgraph = 0;

  if (this->params_->use_curve_shortening_())
  {
    this->method_name_ = "-use_curve_shortening";
    dbsksp_average_n_xgraphs_by_curve_shortening engine;

    engine.set_parent_xgraphs(this->list_xgraph_, this->list_object_name_);
    engine.set_scurve_matching_R(this->params_->average_xgraph_scurve_matching_R_());
    engine.set_scurve_sample_ds(this->params_->average_xgraph_scurve_sample_ds_());
    engine.set_relative_error_tol(0.01);
    engine.set_base_name(this->base_name_);

    bool success = engine.compute();
    if (success)
    {
      average_xgraph = engine.average_xgraph();
    }
  }
  else if (this->params_->use_iterative_merging_())
  {
    this->method_name_ = "-use_iterative_merging";

    dbsksp_average_n_xgraphs_iterative_merge engine;
    
    engine.set_parent_xgraphs(this->list_xgraph_, this->list_object_name_);
    engine.set_scurve_matching_R(this->params_->average_xgraph_scurve_matching_R_());
    engine.set_scurve_sample_ds(this->params_->average_xgraph_scurve_sample_ds_());
    engine.set_relative_error_tol(0.01);
    engine.set_base_name(this->base_name_);

    bool success = engine.compute();
    if (success)
    {
      average_xgraph = engine.average_xgraph();
    }
  }
  else if (this->params_->use_best_exemplar_())
  {
    this->method_name_ = "-use_best_exemplar";

    // do nothing for now
    // \todo Implement this
    vcl_cout << "\nThis function has not been implemented.\n";
    return false;
  }
  else if (this->params_->use_existing_xgraph_in_object_folder_())
  {
    this->method_name_ = "-use_existing_xgraph_in_object_folder";

    vcl_string ext = this->params_->extension_of_existing_xgraph_();
    vcl_string xgraph_file = this->params_->dataset_object_dir_() + "/" + 
      this->params_->dataset_object_name_() + ext;

    dbsksp_xshock_graph_sptr xg = 0;
    bool success = x_read(xgraph_file, xg);
    if (success)
    {
      average_xgraph = xg;
    }
  }
  else if (this->params_->use_specific_exemplar_())
  {
    this->method_name_ = "-use_specific_exemplar";

    unsigned index = this->params_->index_of_specific_exemplar_();
    if (index >= this->list_xgraph_.size())
    {
      vcl_cout << "\nERROR: exemplar index exceeds xgraph vector range."
        << "\n  exemplar index = " << index
        << "\n  vector size    = " << this->list_xgraph_.size() << "\n";
    }
    average_xgraph = new dbsksp_xshock_graph(*(this->list_xgraph_[index]));
  }
  else
  {
    vcl_cout << "\nERROR: no averaging method was chosen.\n";
    return false;
  }
  
  if (!average_xgraph)
  {
    vcl_cout << "\nERROR: average xgraph not computed.\n";
    return false;
  }


  // Do we need to do gradient descent at the end?

  if (this->params_->run_gradient_descent_at_the_end_())
  {
    dbsksp_avg_n_xgraphs_grad_descent engine;

    vcl_vector<double > weights(this->list_xgraph_.size(), 1);
    engine.set_parent_xgraphs(this->list_xgraph_, weights);
    engine.set_scurve_matching_R(this->params_->average_xgraph_scurve_matching_R_());
    engine.set_scurve_sample_ds(this->params_->average_xgraph_scurve_sample_ds_());
    engine.set_init_model_xgraph(average_xgraph);
    engine.set_base_name(this->base_name_);

    bool success = engine.compute();

    if (success)
    {
      average_xgraph = engine.average_xgraph();
    }
  }

  if (average_xgraph)
  {
    this->compute_edit_distance(average_xgraph, 
    this->list_distance_parent_to_average_, 
    this->list_deform_cost_parent_to_average_);

    this->average_xgraph_storage_ = dbsksp_average_xgraph_storage_new();
    this->average_xgraph_storage_->set_xgraph(average_xgraph);
    return true;
  }
  else
  {
    return false;
  }
}


//------------------------------------------------------------------------------
//: Compute edit cost and deformation cost from all parents to an xgraph
void vox_average_n_xgraphs_greedy::
compute_edit_distance(const dbsksp_xshock_graph_sptr& query,
                      vcl_vector<double >& distance_parent_to_query,
                      vcl_vector<double >& deform_cost_parent_to_query)
{
  unsigned num_xgraphs = this->list_xgraph_.size();
  distance_parent_to_query.resize(num_xgraphs);
  deform_cost_parent_to_query.resize(num_xgraphs);

  for (unsigned i =0; i < num_xgraphs; ++i)
  {
    dbsksp_edit_distance work;
    work.set(this->list_xgraph_[i], query, 
      this->params_->average_xgraph_scurve_matching_R_(),
      this->params_->average_xgraph_scurve_sample_ds_());
    work.save_path(true);
    work.edit();

    // edit cost
    distance_parent_to_query[i] = work.final_cost();

    // deform cost
    vcl_vector<pathtable_key > correspondence;
    double deform_cost = work.get_deform_cost(correspondence);
    deform_cost_parent_to_query[i] = deform_cost;
  }
}






//------------------------------------------------------------------------------
//: Write out results
bool vox_average_n_xgraphs_greedy::
write_out()
{
  if (!this->average_xgraph_storage_)
    return false;

  if (this->base_name_.empty())
    return false;

  // save average file
  vcl_string xgraph_file = this->base_name_ +  "-average.xml";
  dbsksp_xshock_graph_sptr xgraph = this->average_xgraph_storage_->xgraph();
  bool success = x_write(xgraph_file, xgraph);

  if (!success)
  {
    vcl_cout << "\nERROR: Failed to write out average xgraph xml file.\n";
    return false;
  }

  // save computation data
  vcl_string data_file = this->base_name_ + "-data.xml";

  // write an xml document for output
  bxml_document doc;
  bxml_element *root = new bxml_element("average_n_xgraphs");
  doc.set_root_element(root);

  root->set_attribute("version", "1");
  root->set_attribute("dataset_name", this->params_->dataset_object_name_());

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
  bxml_element* parents_elm = new bxml_element("list_parent_xgraphs");
  root->append_data(parents_elm);

  // make sure there is a distance for every parent
  if (this->list_distance_parent_to_average_.size() != this->list_object_name_.size())
  {
    this->list_distance_parent_to_average_.resize(this->list_object_name_.size(), 1e6);
  }

  if (this->list_deform_cost_parent_to_average_.size() != this->list_object_name_.size())
  {
    this->list_deform_cost_parent_to_average_.resize(this->list_object_name_.size(), 1e6);
  }

  
  parents_elm->set_attribute("size", this->list_object_name_.size());
  for (unsigned i =0; i < this->list_object_name_.size(); ++i)
  {
    bxml_element* parent_elm = new bxml_element("parent_xgraph");
    parents_elm->append_data(parent_elm);
    parent_elm->set_attribute("index", i);
    parent_elm->append_data(xml_new("object_name", this->list_object_name_[i]));
    parent_elm->append_data(xml_new("distance_to_average", this->list_distance_parent_to_average_[i]));
    parent_elm->append_data(xml_new("deform_cost_to_average", this->list_deform_cost_parent_to_average_[i]));
  }

  
  // actually save the -data xml file
  bxml_write(data_file, doc);

  //c) save screenshot of the shock graphs
  vcl_string screenshot_file = this->base_name_ + "-average.png";
  this->save_screenshot(xgraph, screenshot_file);

  //d) Copy three new files to object folder
  xgraph_file = this->params_->dataset_object_dir_() + "/" + vul_file::strip_directory(xgraph_file);
  x_write(xgraph_file, xgraph);

  data_file = this->params_->dataset_object_dir_() + "/" + vul_file::strip_directory(data_file);
  bxml_write(data_file, doc);

  screenshot_file = this->params_->dataset_object_dir_() + "/" + vul_file::strip_directory(screenshot_file);
  this->save_screenshot(xgraph, screenshot_file);
     
  return true;
}










//------------------------------------------------------------------------------
//: save a screenshot of an to a file
bool vox_average_n_xgraphs_greedy::
save_screenshot(const dbsksp_xshock_graph_sptr& xgraph, const vcl_string& out_png_filename)
{
  // compute bounding box for the xgraph
  xgraph->update_bounding_box();
  vsol_box_2d_sptr bbox = xgraph->bounding_box();

  // construct a canvas to draw the xgraph
  int border = 20;
  int frame_width = vnl_math::rnd(bbox->width() + 2*border);
  int frame_height = vnl_math::rnd(bbox->height() + 2*border);
  vil_image_view<vxl_byte > frame(frame_width, frame_height);
  frame.fill(0);

  // translate the shock graph so that its top-left corner is about [20, 20]
  vgl_vector_2d<double > t0(-(bbox->get_min_x()-border), -(bbox->get_min_y()-border));
  xgraph->translate(t0.x(), t0.y());

  vil_image_view<vxl_byte > screenshot;

  // create a screenshot
  dbsksp_screenshot(frame, xgraph, screenshot);

  // save the image
  vil_save(screenshot, out_png_filename.c_str());

  // translate the shock graph back
  xgraph->translate(-t0.x(), -t0.y());

  return true;
}







