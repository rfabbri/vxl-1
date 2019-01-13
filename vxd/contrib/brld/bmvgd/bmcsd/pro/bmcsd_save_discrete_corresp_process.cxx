#include <iostream>
#include <fstream>

#include "bmcsd_save_discrete_corresp_process.h"

#include <cstring>
#include <string>
#include <fstream>

#include <bmcsd/pro/bmcsd_discrete_corresp_storage.h>
#include <bmcsd/pro/bmcsd_discrete_corresp_storage_3.h>


bmcsd_save_discrete_corresp_process::bmcsd_save_discrete_corresp_process() : bpro1_process()
{
  if( 
      !parameters()->add( "Output filename" , "-nameprefix" , bpro1_filepath("","*.*") ) ||
      !parameters()->add( "Binocular correspondence?" , "-binocular_ftype" , false)  ||
      !parameters()->add( "Trinocular correspondence?" , "-trinocular_ftype" , true)
      )
  {
    std::cerr << "ERROR: Adding parameters in " __FILE__ << std::endl;
  }
}


//: Clone the process
bpro1_process*
bmcsd_save_discrete_corresp_process::clone() const
{
  return new bmcsd_save_discrete_corresp_process(*this);
}

std::vector< std::string > bmcsd_save_discrete_corresp_process::get_input_type() 
{
  std::vector< std::string > to_return;

  bool binocular_ftype=false;
  parameters()->get_value( "-binocular_ftype", binocular_ftype);
  if (binocular_ftype)
    to_return.push_back( "mw pt corresp" );
  else
    to_return.push_back( "mw_3_pt_corresp" );

  return to_return;
}

std::vector< std::string > bmcsd_save_discrete_corresp_process::get_output_type() 
{
  std::vector< std::string > to_return;
  return to_return;
}

//: Loads a _full_ perspective camera (not a pure vpgl_proj_camera)
bool bmcsd_save_discrete_corresp_process::execute()
{
  bpro1_filepath input;
  parameters()->get_value( "-nameprefix" , input);
  std::string fname = input.path;

  vsl_b_ofstream bp_out(fname.c_str());
  if (!bp_out) {
    std::cerr << "ERROR: while opening " << fname << " for reading\n";
    return false;
  }
  std::cout << "Created " << fname << " for writing point correspondence\n";

  bool binocular_ftype=false;
  parameters()->get_value( "-binocular_ftype", binocular_ftype);
  if (binocular_ftype) {
    bmcsd_discrete_corresp_storage_sptr input_corr;
    input_corr.vertical_cast(input_data_[0][0]);
    input_corr->corresp()->b_write(bp_out);
  } else {
    bmcsd_discrete_corresp_storage_3_sptr input_corr;
    input_corr.vertical_cast(input_data_[0][0]);
    input_corr->corresp()->b_write(bp_out);
  }

  bp_out.close();

  return true;
}
