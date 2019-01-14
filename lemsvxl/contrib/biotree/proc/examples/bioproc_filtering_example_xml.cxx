// This is /contrib/biotree/proc/examples/bioproc_filtering_example_xml.cxx

//: 
// \file    bioproc_filtering_example_xml.cxx
// \brief   an example to use bioproc_filtering_proc with volume rendering to show its 
//          response field on fishline phantom of SkyScan data, modified to use parallel beam
// \author  Gamze Tunali
// \date    Dec 29, 2005
// \verbatim
//    03-28-2006 K. Kang: change it to accept parameters from command line 
//    instead of gui and make it ready for batch process. 
//    

#include <ctime>
#include <vector>
#include <vgl/xio/vgl_xio_box_3d.h>
#include <vgl/xio/vgl_xio_vector_3d.h>
#include <vgl/vgl_box_3d.h>

#include <proc/bioproc_filtering_proc.h>
#include <proc/bioproc_splr_filtering_proc.h>
#include <proc/bioproc_filtering_proc_helper.h>
#include <proc/io/proc_io_run_xml_parser.h>
#include <imgr/file_formats/imgr_skyscan_log.h>
#include <xscan/xscan_scan.h>
#include <xscan/xscan_dummy_scan.h>
#include <vsl/vsl_basic_xml_element.h>


int main(int argc, char** argv)
{
  proc_io_run_xml_parser parser;
  std::FILE *xmlFile;
  std::string fname="", bin_fname = "";

  // Parse arguments
  for (int i = 1; i < argc; i++) {
    std::string arg (argv[i]);
    std::cout << arg << std::endl;
    if (arg == std::string ("-x")) { fname = std::string(argv[++i]);}
    else if (arg == std::string ("-b")) { bin_fname = std::string (argv[++i]); }
    else
    {
      std::cout << "Usage: " << argv[0] << "[-x xml_script] [-b binary_scan_file]" << std::endl;
      throw -1;
    }
  }
  
  if (  fname == ""){
    std::cout << "File not specified" << std::endl; 
    return(1);
  }
   
  xmlFile = std::fopen(fname.c_str(), "r");
  if (!xmlFile){
    fprintf(stderr, " %s error on opening", fname.c_str() );
    return(1);
  }

  if (!parser.parseFile(xmlFile)) {
    fprintf(stderr,
          "%s at line %d\n",
          XML_ErrorString(parser.XML_GetErrorCode()),
          parser.XML_GetCurrentLineNumber()
          );
    return 1;
  }

  std::cout << "parsing finished!" << std::endl;

  return proc_flow_manage(parser, xmlFile, fname, bin_fname);
 
}

