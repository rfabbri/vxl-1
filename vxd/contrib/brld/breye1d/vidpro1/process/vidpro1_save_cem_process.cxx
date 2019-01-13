// This is dvidpro1/process/vidpro1_save_cem_process.h

#include <iostream>
#include <fstream>

#include <vidpro1/process/vidpro1_save_cem_process.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vnl/vnl_math.h>
#include <bsold/bsold_file_io.h>

vidpro1_save_cem_process::vidpro1_save_cem_process() : bpro1_process()
{
  if( !parameters()->add( "Output file <filename...>" , "-cemoutput" , bpro1_filepath("","*.cem") ))
  {
    std::cerr << "ERROR: Adding parameters in " __FILE__ << std::endl;
  }
}


//: Clone the process
bpro1_process*
vidpro1_save_cem_process::clone() const
{
  return new vidpro1_save_cem_process(*this);
}


std::vector< std::string > vidpro1_save_cem_process::get_input_type()
{
  std::vector< std::string > to_return;
  to_return.push_back( "vsol2D" );
  return to_return;
}

std::vector< std::string > vidpro1_save_cem_process::get_output_type()
{
  std::vector< std::string > to_return;
  to_return.clear();
  return to_return;
}

bool vidpro1_save_cem_process::execute()
{
  bpro1_filepath output;
  parameters()->get_value( "-cemoutput" , output );
  return saveCEM(output.path);
}

bool vidpro1_save_cem_process::saveCEM (std::string filename)
{
  // get input storage class
    vidpro1_vsol2D_storage_sptr input_vsol;
  input_vsol.vertical_cast(input_data_[0][0]);

  std::vector< vsol_spatial_object_2d_sptr > vsol_list = input_vsol->all_data();

  return bsold_save_cem(vsol_list, filename);
}


