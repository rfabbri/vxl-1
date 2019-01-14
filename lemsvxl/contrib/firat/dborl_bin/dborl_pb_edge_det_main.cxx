#include <iostream>
#include <fstream>
#include <cstdio>
#include <vul/vul_file.h>

#include "dborl_pb_edge_det_params.h"
#include "dborl_pb_edge_det_params_sptr.h"

#include <dborl/dborl_index_sptr.h>
#include <dborl/algo/dborl_index_parser.h>
#include <dborl/dborl_index.h>
#include <dborl/dborl_index_node.h>

//#include "pb_octave_wrapper.h"

int main (int argc, char *argv[]) 
{
  // constructs with the default values
  dborl_pb_edge_det_params_sptr params = new dborl_pb_edge_det_params ("dborl_pb_edge_det");  
  if (!params->parse_command_line_args(argc, argv))
    return 0;

  // always print the params file if an executable to work with ORL web interface
  if (!params->print_params_xml(params->print_params_file()))
    std::cout << "problems in writing params file to: " << params->print_params_file() << std::endl;

  if (params->exit_with_no_processing() || params->print_params_only())
    return 0;

  // always call this method to actually parse the input parameter file whose name is extracted from the command line
  if (!params->parse_input_xml())
    return 0;

  //creating a dborl_index instance which will store the information coming from the index file
  dborl_index_sptr ind;

  std::string index_filename = params->index_filename_();

  //creating a parser for the index file
  /*dborl_index_parser parser;
  parser.clear();

  std::FILE *xmlFile = std::fopen(index_filename.c_str(), "r");
  std::cout << index_filename.c_str() << std::endl;
  if (xmlFile == NULL){
    std::cout << "dborl_pb_edge_det::parse_index() -- " << index_filename << "-- error on opening" << std::endl;
    return 0;
  }

  if (!parser.parseFile(xmlFile)) {
     std::cout << XML_ErrorString(parser.XML_GetErrorCode()) << " at line " <<
        parser.XML_GetCurrentLineNumber() << std::endl;
     return 0;
   }

  fclose(xmlFile);
  ind = parser.get_index();
  
  if (!ind)
    return 0;

  if (!(ind->get_type() == dborl_index_type::flat_image)) {
    std::cout << "Index is not of flat_image type! Exiting!\n";
    return 0;
  }*/
  
  std::string input_image_file_name = params->input_dir_() + '/' + params->input_name_();
  //pb_octave_wrapper pb("/vision/scratch/firat/pb_temp_dir/");
  
  //pb.detect(input_image_file_name, params->output_dir_());
  return 0;
}
