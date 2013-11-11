// This is rec/dborl/algo/vox_average_two_xgraphs/vox_average_two_xgraphs_one_to_many.cxx


//:
// \file

#include "vox_average_two_xgraphs_one_to_one.h"
#include "vox_average_two_xgraphs_params.h"


#include <dbsksp/pro/dbsksp_xgraph_storage.h>
#include <dbsksp/pro/dbsksp_average_xgraph_storage.h>

#include <dbsk2d/pro/dbsk2d_load_esf_process.h>
#include <dbsksp/pro/dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process.h>
#include <dbsksp/pro/dbsksp_average_two_xgraphs_process.h>
#include <dbsksp/xio/dbsksp_xio_xshock_graph.h>

#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <dbsksp/algo/dbsksp_screenshot.h>

#include <vsol/vsol_box_2d.h>
#include <vnl/vnl_math.h>
#include <vil/vil_save.h>

//==============================================================================
// vox_average_two_xgraphs_one_to_one
//==============================================================================

//: Constructor
vox_average_two_xgraphs_one_to_one::
vox_average_two_xgraphs_one_to_one(vox_average_two_xgraphs_params_sptr params):
  params_(params)
{

}

//------------------------------------------------------------------------------
  // Called by main statement to intiate algorithm on vox
bool vox_average_two_xgraphs_one_to_one::
process()
{
  bool status(false);

  // Reading of index file is also performed in initialize process
  status = this->initialize();
  if ( status )
  {
    // Move on to matching
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
bool vox_average_two_xgraphs_one_to_one::
initialize()
{
  this->esf_file_[0] = this->params_->model_object_dir_() + "/" 
    + this->params_->model_object_name_() + ".esf";

  this->esf_file_[1] = this->params_->query_object_dir_() + "/"
    + this->params_->query_object_name_() + ".esf";

  // location of output dir
  this->output_folder_ = this->params_->output_average_xgraph_folder_();
  if (!vul_file::is_directory(this->output_folder_))
  {
    vul_file::make_directory(this->output_folder_);
  }

  vcl_string name1 = vul_file::strip_extension(vul_file::strip_directory(this->esf_file_[0]));
  vcl_string name2 = vul_file::strip_extension(vul_file::strip_directory(this->esf_file_[1]));
  this->base_name_ = this->output_folder_ + "/" + name1 + "+" + name2;

  // save input file
  this->params_->print_input_xml(this->base_name_ + "-input.xml");


  return true;
}



//------------------------------------------------------------------------------
//: Actual matching performed
bool vox_average_two_xgraphs_one_to_one::
perform_averaging()
{
  // 0) Make some annoucement
  vcl_cout 
    << "\nAveraging two shock graphs:"
    << "\n  esf1 = " << vul_file::strip_directory(esf_file_[0])
    << "\n  esf2 = " << vul_file::strip_directory(esf_file_[1]) << "\n";

  // 1) Load the two esf files
  vcl_cout 
    << "\n>>Loading two esf files ...";
  
  // Output container:
  dbsk2d_shock_storage_sptr sk2d_storage[2];

  // Status
  bool load_status(true);

  // Loading one esf file at a time
  load_status &= this->load_esf(this->esf_file_[0], sk2d_storage[0]);
  load_status &= this->load_esf(this->esf_file_[1], sk2d_storage[1]);
  if (!load_status)
  {
    vcl_cout << "[ Failed ]<<\n";
    return false;
  }
  else
  {
    vcl_cout << "[ OK ]<<\n";
  }

  // 2) Convert dbsk2d_shock_graph to dbsksp_xshock_graph
  vcl_cout << "\n>>Convert dbsk2d_shock_graph to dbsksp_xshock_graph ... ";

  // Output container
  dbsksp_xgraph_storage_sptr xgraph_storage[2];

  // Status
  bool convert_status(true);
  convert_status &= this->convert_sk2d_to_sksp(sk2d_storage[0], xgraph_storage[0]);
  convert_status &= this->convert_sk2d_to_sksp(sk2d_storage[1], xgraph_storage[1]);
  
  if (!convert_status)
  {
    vcl_cout << "[ Failed ]<<\n";
    return false;
  }
  else
  {
    vcl_cout << "[ OK ]<<\n";
  }

  //3) Average the two xgraphs
  vcl_cout << "\n>>Compute average of two converted xgraphs...";
  
  bool average_status = this->compute_average(xgraph_storage[0], xgraph_storage[1], this->average_xgraph_storage_);

  if (!average_status)
  {
    vcl_cout << "[ Failed ]<<\n";
    return false;
  }
  else
  {
    vcl_cout << "[ OK ]<<\n";
  }

  return true;
}



//------------------------------------------------------------------------------
//: Load esf file
bool vox_average_two_xgraphs_one_to_one::
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
bool vox_average_two_xgraphs_one_to_one::
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
//: Compute average of two xgraphs
bool vox_average_two_xgraphs_one_to_one::
compute_average(const dbsksp_xgraph_storage_sptr& xgraph1,
    const dbsksp_xgraph_storage_sptr xgraph2,
    dbsksp_average_xgraph_storage_sptr& average_xgraph)
{
  dbsksp_average_two_xgraphs_process average_process;

  
  // set parameters for storage
  float scurve_matching_R = this->params_->average_xgraph_scurve_matching_R_(); //6.0f;
  float scurve_sample_ds = this->params_->average_xgraph_scurve_sample_ds_(); //1.0f;
  float weight1 = this->params_->average_xgraph_weight1_();
  float weight2 = this->params_->average_xgraph_weight2_();
  
  average_process.parameters()->set_value("-weight1", weight1);
  average_process.parameters()->set_value("-weight2", weight2);
  
  average_process.parameters()->set_value("-scurve-matching-R" , scurve_matching_R);
  average_process.parameters()->set_value("-scurve-sample-ds", scurve_sample_ds);
  average_process.parameters()->set_value("-base_name", this->base_name_);
  

  average_process.clear_input();
  average_process.clear_output();
  average_process.add_input(xgraph1);
  average_process.add_input(xgraph2);
  
  bool success = average_process.execute();

  if (!success)
  {
    return false;
  }
  else
  {
    average_xgraph.vertical_cast(average_process.get_output()[0]); 
    return true;
  }

  return false;
}




//------------------------------------------------------------------------------
//: save a screenshot of an to a file
bool vox_average_two_xgraphs_one_to_one::
save_screenshot(const dbsksp_xshock_graph_sptr& xgraph, const vcl_string& out_png_filename)
{
  // compute bounding box for the xgraph
  xgraph->update_bounding_box();
  vsol_box_2d_sptr bbox = xgraph->bounding_box();

  // construct a canvas to draw the xgraph
  int border = 20;
  int frame_width = vnl_math_rnd(bbox->width() + 2*border);
  int frame_height = vnl_math_rnd(bbox->height() + 2*border);
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












//------------------------------------------------------------------------------
//: Write out results
bool vox_average_two_xgraphs_one_to_one::
write_out()
{
  vcl_string name1 = vul_file::strip_extension(vul_file::strip_directory(this->esf_file_[0]));
  vcl_string name2 = vul_file::strip_extension(vul_file::strip_directory(this->esf_file_[1]));

  ////
  //if (!vul_file::is_directory(this->output_folder_))
  //{
  //  vul_file::make_directory(this->output_folder_);
  //}

  //vcl_string base_name = this->output_folder_ + "/" + name1 + "+" + name2;

  // save average file
  vcl_string xgraph_file = this->base_name_ + "-average.xml";
  bool success = x_write(xgraph_file, this->average_xgraph_storage_->xgraph());

  if (!success)
  {
    vcl_cout << "\nERROR: Failed to write out average xgraph xml file.\n";
    return false;
  }

  // save computation data
  vcl_string data_file = this->base_name_ + "-data.txt";

  vcl_ofstream ofs(data_file.c_str());
  if (!ofs)
  {
    vcl_cout << "\nERROR: can't open data file for writing.\n";
    return false;
  }

  ofs << "object1 " << name1 << "\n"
    << "object2 " << name2 << "\n"
    << "distance-object1-to-object2 " << this->average_xgraph_storage_->distance_btw_parents() << "\n"
    << "distance-average-to-object1 " << this->average_xgraph_storage_->distance_to_parent(0) << "\n"
    << "distance-average-to-object2 " << this->average_xgraph_storage_->distance_to_parent(1) << "\n";
  ofs.close();


  // save screenshot of the shock graphs
  this->save_screenshot(this->average_xgraph_storage_->xgraph(), this->base_name_ + "-average.png");
  this->save_screenshot(this->average_xgraph_storage_->parent_xgraph(0), this->base_name_ + "-parent1.png");
  this->save_screenshot(this->average_xgraph_storage_->parent_xgraph(1), this->base_name_ + "-parent2.png");
  
  
  
     
  return true;
}









