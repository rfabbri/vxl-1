//:
// \file
// \brief

#include "mpi_compute_ishock_params.h"

#include <bxml/bxml_read.h>
#include <bxml/bxml_write.h>
#include <bxml/bxml_find.h>
#include <dborl/algo/dborl_utilities.h>

#include <dbdet/pro/dbdet_third_order_edge_detector_process.h>
#include <dbdet/pro/dbdet_third_order_color_edge_detector_process.h>
#include <dbdet/pro/dbdet_generic_linker_process.h>
#include <dbsk2d/pro/dbsk2d_compute_ishock_process.h>


//: Constructor
mpi_compute_ishock_params::
mpi_compute_ishock_params(vcl_string algo_name):
  dborl_algo_params(algo_name)
{
  // file list
  this->file_list_.set_values(this->param_list_, "io", "file_list", 
    "List of images", 
    "/vision/projects/kimia/shock-project/shock-detection-revisted/results/ETHZShapeClasses/list_bottles.txt", 
    "/vision/projects/kimia/shock-project/shock-detection-revisted/results/ETHZShapeClasses/list_bottles.txt");
  
  // file directory
  this->file_dir_.set_values(this->param_list_, "io", "file_dir", 
    "Image directory", 
    "/vision/images/misc/ethz-shape-database/ETHZShapeClasses/all_images/all_originals", 
    "/vision/images/misc/ethz-shape-database/ETHZShapeClasses/all_images/all_originals");

  // output directory
  this->out_dir_.set_values(this->param_list_, "io", "out_dir", 
    "Output directory", 
    "/vision/projects/kimia/shock-project/shock-detection-revisited/results/ETHZShapeClasses", 
    "/vision/projects/kimia/shock-project/shock-detection-revisited/results/ETHZShapeClasses");

  // extension
  this->extension_.set_values(this->param_list_, "io", "extension", 
    "Image extension", ".jpg", ".jpg");

  // Parameters of the processes
  // Edge detection
  dbdet_third_order_color_edge_detector_process pro1;
  vcl_vector<bpro1_param*> params1 = pro1.parameters()->get_param_list();
  for (unsigned i = 0; i < params1.size(); i++) 
  {
    dborl_parameter_base* p = 
      convert_parameter_from_bpro1("third_order_color_edge_detector", params1[i]);
    param_list_.push_back(p);
    process_params_.push_back(p);
  }

  // Boundary linking
  dbdet_generic_linker_process pro2;
  vcl_vector<bpro1_param*> params2 = pro2.parameters()->get_param_list();
  for (unsigned i = 0; i < params2.size(); i++) 
  {
    dborl_parameter_base* p = 
      convert_parameter_from_bpro1("generic_linker", params2[i]);
    param_list_.push_back(p);
    process_params_.push_back(p);
  }

  // Intrinsic shock detection
  dbsk2d_compute_ishock_process pro3;
  vcl_vector<bpro1_param*> params3 = pro3.parameters()->get_param_list();
  for (unsigned i = 0; i < params3.size(); i++) 
  {
    dborl_parameter_base* p = 
      convert_parameter_from_bpro1("compute_ishock", params3[i]);
    param_list_.push_back(p);
    process_params_.push_back(p);
  }
}


mpi_compute_ishock_params::
~mpi_compute_ishock_params()
{
  for (unsigned i =0; i < this->process_params_.size(); ++i)
  {
    delete this->process_params_[i];
  }
}



//: lsjflsd
bool mpi_compute_ishock_params::
parse_from_data(bxml_data_sptr root)
{
  return dborl_algo_params::parse_from_data(root);
}



// -----------------------------------------------------------------------------
//:
bxml_element* mpi_compute_ishock_params::
create_default_document_data() 
{
  // create a default sks_match_shape_params object
  mpi_compute_ishock_params params("mpi_compute_ishock");
  return params.create_document_data();
}


// -----------------------------------------------------------------------------
//: Create a XML document for the current parameter set
bxml_element* mpi_compute_ishock_params::
create_document_data()
{
  return dborl_algo_params::create_document_data();
}



// -----------------------------------------------------------------------------
//: Return pointer to a parameter with a given name and group
dborl_parameter_base* mpi_compute_ishock_params::
get_param(const vcl_string& group, const vcl_string& name)
{
  for (unsigned i =0; i < this->param_list_.size(); ++i)
  {
    dborl_parameter_base* p = this->param_list_[i];
    if (p->param_group() == group && p->param_name() == name)
      return p;
  }
  return 0;
}

