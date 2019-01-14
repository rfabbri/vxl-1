//This is brcv/seg/dbdet/pro/dbdet_load_cem_process.cxx

#include "dbdet_load_cem_process.h"

#include <iostream>
#include <cassert>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>

#include <vgl/vgl_point_2d.h>

#include <dbdet/pro/dbdet_sel_storage_sptr.h>
#include <dbdet/pro/dbdet_sel_storage.h>

#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>

#include <dbdet/algo/dbdet_cem_file_io.h>
#include <dbdet/sel/dbdet_curve_fragment_graph.h>


#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <bsold/bsold_file_io.h>


dbdet_load_cem_process::dbdet_load_cem_process() : bpro1_process(), num_frames_(0)
{
  if( !parameters()->add( "Input file <filename...>" , "-cem_filename" , bpro1_filepath("","*.cem") ) ||
      !parameters()->add( "Load as vsol"             , "-bvsol"        , false ) ||
      !parameters()->add( "Order by filename"        , "-orderbf", false)
    )
  {
    std::cerr << "ERROR: Adding parameters in " __FILE__ << std::endl;
  }
}

std::string dbdet_load_cem_process::name() 
{
  return "Load .CEM File";
}

std::vector< std::string > dbdet_load_cem_process::get_input_type() 
{
  std::vector< std::string > to_return;
  return to_return;
}

std::vector< std::string > dbdet_load_cem_process::get_output_type() 
{
  std::vector< std::string > to_return;
  bool bvsol;
  parameters()->get_value( "-bvsol" , bvsol );

  if (bvsol)
    to_return.push_back( "vsol2D" );
  else {
    to_return.push_back( "edge_map");
    to_return.push_back( "sel");
  }

  return to_return;
}

//: Clone the process
bpro1_process*
dbdet_load_cem_process::clone() const
{
  return new dbdet_load_cem_process(*this);
}


bool dbdet_load_cem_process::execute()
{
  bpro1_filepath cem_filename;
  parameters()->get_value( "-cem_filename", cem_filename);
  std::string input_file_path = cem_filename.path;

  int num_of_files = 0;

  output_data_.clear();

  // make sure that input_file_path is sane
  if (input_file_path == "") { return false; }

  //std::cout << vul_file::dirname(input_file_path);

  // test if fname is a directory
  if (vul_file::is_directory(input_file_path))
  {
    vul_file_iterator fn=input_file_path+"/*.cem";
    std::vector<std::string> file_name;
    for ( ; fn; ++fn) 
    {
      file_name.push_back(fn());
      num_of_files++;
    }

    bool orderbf;
    parameters()->get_value( "-orderbf" , orderbf );
    if(orderbf)
      std::sort(file_name.begin(), file_name.end());

    for (int i=0 ; i<file_name.size(); ++i) 
    {
      load_CEM(file_name[i]);
    }

    std::cout << "Tota frames loaded: " << output_data_.size() << std::endl;
    //this is the number of frames to be outputted
    num_frames_ = num_of_files;
  }
  else {
    std::string input_file = input_file_path;
    bool successful = load_CEM(input_file);
    num_frames_ = 1;
    
    if (!successful)
      return false;
  }

  //reverse the order of the objects so that they come out in the right order
  std::reverse(output_data_.begin(),output_data_.end());

  return true;
}

bool dbdet_load_cem_process::load_CEM(std::string input_file)
{
  bool bvsol=false;  
  parameters()->get_value( "-bvsol", bvsol );

  if (bvsol){
    std::vector< vsol_spatial_object_2d_sptr > contours;
    bool retval = bsold_load_cem(contours, input_file);

    if (!retval) return false;

    // create the output storage class
    vidpro1_vsol2D_storage_sptr new_cem = vidpro1_vsol2D_storage_new();
    new_cem->add_objects(contours, input_file);
    output_data_.push_back(std::vector< bpro1_storage_sptr > (1,new_cem));
  }
  else {
    // create the sel storage class
    dbdet_sel_storage_sptr output_sel = dbdet_sel_storage_new();

    //get pointers to the data structures in it
    dbdet_curve_fragment_graph &CFG = output_sel->CFG();
    dbdet_edgemap_sptr EM = dbdet_load_cem(input_file, CFG);
    output_sel->set_EM(EM);

    if (!EM) return false;

    //create the edgemap storage class
    dbdet_edgemap_storage_sptr output_edgemap = dbdet_edgemap_storage_new();
    output_edgemap->set_edgemap(EM);

    std::vector<bpro1_storage_sptr> storage_list;
    storage_list.push_back(output_edgemap);
    storage_list.push_back(output_sel);
    output_data_.push_back(storage_list);
  }
 
  std::cout << "Loaded: " << input_file.c_str() << ".\n";

  return true;
}

