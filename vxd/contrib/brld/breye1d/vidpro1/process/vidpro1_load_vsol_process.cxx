//This is vidpro1/process/vidpro1_load_vsol_process.cxx

#include <iostream>
#include <fstream>

#include "vidpro1_load_vsol_process.h"

#include <cstring>
#include <string>
#include <fstream>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_binary_loader.h>

#include <vidpro1/storage/vidpro1_vsol2D_storage.h>

vidpro1_load_vsol_process::vidpro1_load_vsol_process() : bpro1_process()
{
  if( !parameters()->add( "Input file <filename...>" , "-fname" , bpro1_filepath("","*") )) 
  {
    std::cerr << "ERROR: Adding parameters in " __FILE__ << std::endl;
  }
}


//: Clone the process
bpro1_process*
vidpro1_load_vsol_process::clone() const
{
  return new vidpro1_load_vsol_process(*this);
}

std::vector< std::string > vidpro1_load_vsol_process::get_input_type() 
{
  std::vector< std::string > to_return;
  return to_return;
}

std::vector< std::string > vidpro1_load_vsol_process::get_output_type() 
{
  std::vector< std::string > to_return;
  to_return.push_back( "vsol2D" );
  return to_return;
}

bool vidpro1_load_vsol_process::execute()
{
  bpro1_filepath input;
  parameters()->get_value( "-fname" , input);
  std::string input_file = input.path;
  

  vsl_b_ifstream bp_in(input_file.c_str());
  if (!bp_in) {
    std::cout << " Error opening file  " << input_file << std::endl;
    return false;
  }

  std::cout << "Opened " << input_file <<  " for reading" << std::endl;

  vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
  output_vsol->b_read(bp_in);
  bp_in.close();

//  output_vsol->print_summary(std::cout);


  //: clone

  vidpro1_vsol2D_storage_sptr output_vsol_2;
  output_vsol_2.vertical_cast(output_vsol->clone());

  output_vsol_2->set_frame(-10); //:< means its not in rep
  // try to copy by hand if doesnt work

  output_data_[0].push_back(output_vsol_2);

  return true;
}
