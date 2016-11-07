//This is brcv/seg/dbdet/pro/dbdet_load_cvlet_map_process.cxx

#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcl_fstream.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>

#include "dbdet_load_cvlet_map_process.h"

#include <dbdet/dbdet_config.h>
#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>
#include <dbdet/pro/dbdet_sel_storage.h>
#include <dbdet/pro/dbdet_sel_storage_sptr.h>

#include <dbdet/edge/dbdet_edgemap.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/sel/dbdet_curvelet_map.h>
#include <dbdet/algo/dbdet_cvlet_map_io.h>

dbdet_load_cvlet_map_process::dbdet_load_cvlet_map_process() : bpro1_process(), num_frames_(0)
{
  if (
#ifdef HAS_BOOST
      !parameters()->add( "Input file <filename...> (gzipped .gz supported)" , "-cvlet_input" , bpro1_filepath("","*.cvlet*") )
#else
      !parameters()->add( "Input file <filename...> (gzipped .gz NOT supported)" , "-cvlet_input" , bpro1_filepath("","*.cvlet") )
#endif
      )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}

vcl_string dbdet_load_cvlet_map_process::name() 
{
  return "Load .CVLET File";
}

vcl_vector< vcl_string > dbdet_load_cvlet_map_process::get_input_type() 
{
  vcl_vector< vcl_string > to_return;
  return to_return;
}

vcl_vector< vcl_string > dbdet_load_cvlet_map_process::get_output_type() 
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "sel");
  to_return.push_back( "edge_map");
  return to_return;
}

//: Clone the process
bpro1_process*
dbdet_load_cvlet_map_process::clone() const
{
  return new dbdet_load_cvlet_map_process(*this);
}


bool dbdet_load_cvlet_map_process::execute()
{
  bpro1_filepath input;
  parameters()->get_value( "-cvlet_input" , input);
  vcl_string input_file_path = input.path;

  int num_of_files = 0;

  output_data_.clear();

  // make sure that input_file_path is sane
  if (input_file_path == "") { return false; }

  //vcl_cout << vul_file::dirname(input_file_path);

  // test if fname is a directory
  if (vul_file::is_directory(input_file_path))
  {
    vul_file_iterator fn=input_file_path+"/*.cvlet*";
    for ( ; fn; ++fn) 
    {
      vcl_string input_file = fn();
      load_cvlet_file(input_file);
      num_of_files++;
    }

    //this is the number of frames to be outputted
    num_frames_ = num_of_files;
  }
  else {
    vcl_string input_file = input_file_path;
    bool successful = load_cvlet_file(input_file);
    num_frames_ = 1;

    if (!successful)
      return false;
  }

  //reverse the order of the objects so that they come out in the right order
  vcl_reverse(output_data_.begin(),output_data_.end());

  return true;
}

bool dbdet_load_cvlet_map_process::load_cvlet_file(vcl_string input_file)
{
  // edge_map 
  dbdet_edgemap_sptr edge_map;

  // create the output storage classes
  dbdet_edgemap_storage_sptr output_edgemap = dbdet_edgemap_storage_new();
  dbdet_sel_storage_sptr output_sel = dbdet_sel_storage_new();

  bool retval = dbdet_load_cvlet_map(input_file, edge_map, output_sel->CM());
  if (!retval)
    return false;

  output_edgemap->set_edgemap(edge_map);
  output_sel->set_EM(edge_map);

  //finally add it to the output data
  vcl_vector< bpro1_storage_sptr > output_storage_set;
  output_storage_set.push_back(output_sel);
  output_storage_set.push_back(output_edgemap);
  output_data_.push_back(output_storage_set);

  vcl_cout << "Loaded: " << input_file.c_str() << ".\n";

  return true;
}

