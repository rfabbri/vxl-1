//This is brcv/seg/dbdet/pro/dbdet_save_edg_process.cxx

#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcl_fstream.h>
#include <vcl_cmath.h>

#include "dbdet_save_edg_process.h"

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>

#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>

#include <dbdet/edge/dbdet_edgemap.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/algo/dbdet_load_edg.h>

dbdet_save_edg_process::dbdet_save_edg_process() : bpro1_process()
{
  if( !parameters()->add( "Output file <filename...>" , "-edgoutput" , bpro1_filepath("","*.edg")))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}

vcl_string dbdet_save_edg_process::name() 
{
  return "Save .EDG File";
}

vcl_vector< vcl_string > dbdet_save_edg_process::get_input_type() 
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "edge_map");
  return to_return;
}

vcl_vector< vcl_string > dbdet_save_edg_process::get_output_type() 
{
  vcl_vector< vcl_string > to_return;
  return to_return;
}

//: Clone the process
bpro1_process*
dbdet_save_edg_process::clone() const
{
  return new dbdet_save_edg_process(*this);
}


bool dbdet_save_edg_process::execute()
{  
  bpro1_filepath output;
  
  //get the input storage class
  dbdet_edgemap_storage_sptr input_edgemap;
  input_edgemap.vertical_cast(input_data_[0][0]);

  dbdet_edgemap_sptr edgemap = input_edgemap->get_edgemap();

  parameters()->get_value( "-edgoutput" , output );
  vcl_string output_file = output.path;

  //save this edge map onto a file
  bool retval = dbdet_save_edg(output_file, edgemap);

  return retval;
}
