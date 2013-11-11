//:
// \file
// \brief
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/30/07

//
//

#include "dborl_sks_match_shape_params.h"

#include <bxml/bxml_read.h>
#include <bxml/bxml_write.h>
#include <bxml/bxml_find.h>

#include "bpro1_xio_parameters.h"
#include <dbsks/pro/dbsks_compute_arc_cost_process.h>
#include <dbsks/pro/dbsks_detect_shape_process.h>
#include <dbsks/pro/dbsks_local_match_process.h>


//: 
bool dborl_sks_match_shape_params::
parse_from_data(bxml_data_sptr root)
{
  vcl_cout << "parsing dborl_sks_match_shape_params\n";
  bxml_element query("dborl_sks_match_shape_params");
  bxml_data_sptr result = bxml_find_by_name(root, query);
  
  
  if (!result) 
  {
    vcl_cout << "dborl_sks_match_shape_params::parse_from_data() - "
      << " could not find the node dborl_sks_match_shape_params\n";
    return false;
  }

  // data
  bxml_element query2("data");
  bxml_data_sptr result2 = bxml_find_by_name(result, query2);

  if (!result2) 
  {
    vcl_cout << "dborl_sks_match_shape_params::parse_from_data() - "
      << "could not find the node data\n";
    return false;
  }
   
  bxml_element* data0 = static_cast<bxml_element*>(result2.ptr());
  if (!data0)
    return false;

  data0->get_attribute("file_list", file_list_);
  data0->get_attribute("file_dir", file_dir_);
  data0->get_attribute("out_dir", out_dir_);
  data0->get_attribute("model_file", model_file_);
  data0->get_attribute("extension", extension_);
  

  // compute_arc_cost_params
  bxml_element query3("dbsks_compute_arc_cost_params");
  bxml_data_sptr result3 = bxml_find_by_name(result, query3);

  if (!result3) 
  {
    vcl_cout << "dborl_sks_match_shape_params::parse_from_data() - "
      << "could not find the node dbsks_compute_image_cost_params\n";
    return false;
  }
   
  bxml_element* data1 = static_cast<bxml_element*>(result3.ptr());
  if (!data1)
    return false;

  this->dbsks_compute_arc_cost_params_ = 
    dbsks_compute_arc_cost_process().parameters();
  x_read(data1, this->dbsks_compute_arc_cost_params_);



  // detect_shape_params
  bxml_element query4("dbsks_detect_shape_params");
  bxml_data_sptr result4 = bxml_find_by_name(result, query4);

  if (!result4) 
  {
    vcl_cout << "dborl_sks_match_shape_params::parse_from_data() - "
      << "could not find the node dbsks_detect_shape_params\n";
    return false;
  }
   
  bxml_element* data2 = static_cast<bxml_element*>(result4.ptr());
  if (!data2)
    return false;

  this->dbsks_detect_shape_params_ = 
    dbsks_detect_shape_process().parameters();
  x_read(data2, this->dbsks_detect_shape_params_);


  // local match params
  bxml_element query5("dbsks_local_match_params");
  bxml_data_sptr result5 = bxml_find_by_name(result, query5);

  if (!result5) 
  {
    vcl_cout << "dborl_sks_match_shape_params::parse_from_data() - "
      << "could not find the node dbsks_local_match_params\n";
    return false;
  }
   
  bxml_element* data3 = static_cast<bxml_element*>(result5.ptr());
  if (!data3)
    return false;

  this->dbsks_local_match_params_ = 
    dbsks_local_match_process().parameters();
  x_read(data3, this->dbsks_local_match_params_);

  return true;
}



// -----------------------------------------------------------------------------
//:
bxml_element* dborl_sks_match_shape_params::
create_default_document_data() 
{
  // create a default sks_match_shape_params object
  dborl_sks_match_shape_params params;
  params.file_list_ = "/vision/projects/kimia/shockshape/symseg/results/ETHZ-database/experiments/bottle_model_scale_0_7/list_bottle_images.txt";
  params.file_dir_ = "/vision/images/misc/ethz-shape-database/ETHZShapeClasses/Bottles/";
  params.out_dir_ = "/vision/projects/kimia/shockshape/symseg/results/ETHZ-database/experiments/bottle_model_scale_0_7/";
  params.model_file_ = "/vision/projects/kimia/shockshape/symseg/results/ETHZ-database/experiments/bottle_model_scale_0_7/bottle-5-fragments-mw2mh_0_3.xml";
  params.extension_ = "_edges.tif";

  // get parameter list from a generic process
  dbsks_compute_arc_cost_process compute_arc_cost_process;
  params.dbsks_compute_arc_cost_params_ = compute_arc_cost_process.parameters();

  // Detect shape params
  dbsks_detect_shape_process detect_shape_process;
  params.dbsks_detect_shape_params_ = detect_shape_process.parameters();

  // Local match params
  dbsks_local_match_process local_match_process;
  params.dbsks_local_match_params_ = local_match_process.parameters();

  return params.create_document_data();
}


// -----------------------------------------------------------------------------
//: Create a XML document for the current parameter set
bxml_element* dborl_sks_match_shape_params::
create_document_data()
{
  // root
  bxml_element * root = new bxml_element("dborl_sks_match_shape_params");

  // data
  bxml_element * data0 = new bxml_element("data");
  root->append_data(data0);
  root->append_text("\n");

  data0->set_attribute("file_list", file_list_);
  data0->set_attribute("file_dir", file_dir_);
  data0->set_attribute("out_dir", out_dir_);
  data0->set_attribute("model_file", model_file_);
  data0->set_attribute("extension", extension_);
  data0->append_text("\n");

  // compute arc cost params
  bxml_element * data1 = new bxml_element("dbsks_compute_arc_cost_params");
  root->append_data(data1);
  root->append_text("\n");
  x_write(data1, this->dbsks_compute_arc_cost_params_);
  data1->append_text("\n");

  // Detect shape params
  bxml_element * data2 = new bxml_element("dbsks_detect_shape_params");
  root->append_data(data2);
  root->append_text("\n");
  x_write(data2, this->dbsks_detect_shape_params_);
  data2->append_text("\n");


  // Local match params
  bxml_element * data3 = new bxml_element("dbsks_local_match_params");
  root->append_data(data3);
  root->append_text("\n");
  x_write(data3, this->dbsks_local_match_params_);
  data3->append_text("\n");

  return root;
}

