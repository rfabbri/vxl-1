// This is dvidpro/process/vidpro1_save_vsol_process.h

#include <iostream>
#include <fstream>

#include "vidpro1_save_vsol_process.h"
#include <vnl/vnl_math.h>

#include <vsl/vsl_binary_io.h>


vidpro1_save_vsol_process::vidpro1_save_vsol_process() : bpro1_process()
{
  if( !parameters()->add( "Output file <filename...>" , "-fname" , bpro1_filepath("","*.cem") ))
  {
    std::cerr << "ERROR: Adding parameters in " __FILE__ << std::endl;
  }
}


//: Clone the process
bpro1_process*
vidpro1_save_vsol_process::clone() const
{
  return new vidpro1_save_vsol_process(*this);
}


std::vector< std::string > vidpro1_save_vsol_process::get_input_type()
{
  std::vector< std::string > to_return;
  to_return.push_back( "vsol2D" );
  return to_return;
}

std::vector< std::string > vidpro1_save_vsol_process::get_output_type()
{
  std::vector< std::string > to_return;
  to_return.clear();
  return to_return;
}

bool vidpro1_save_vsol_process::execute()
{
  bpro1_filepath output;
  parameters()->get_value( "-fname" , output );


  vidpro1_vsol2D_storage_sptr input_vsol;
  input_vsol.vertical_cast(input_data_[0][0]);

  vsl_b_ofstream bp_out(output.path);

  if (!bp_out){
    std::cout << " Error opening file  " << output.path << std::endl;
    return false;
  }

  std::cout << "Created " << output.path << " for writing vsol\n";

  input_vsol->b_write(bp_out);

  bp_out.close();

  return true;
}
