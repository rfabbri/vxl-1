// This is /contrib/biotree/proc/examples/bioproc_filtering_example_xml.cxx

//: 
// \file    bioproc_filtering_example_xml.cxx
// \brief   an example to use biocluster_volpart_filtering_proc with volume rendering to show its 
//          response field on fishline phantom of SkyScan data, modified to use parallel beam
// \author  Gamze Tunali
// \date    Dec 29, 2005
#include <mpi.h>
#include <ctime>
#include <vector>
#include <vgl/xio/vgl_xio_box_3d.h>
#include <vgl/xio/vgl_xio_vector_3d.h>

#include <proc/io/proc_io_run_xml_parser.h>
#include <proc/bioproc_filtering_proc_helper.h>
#include <proc/biocluster/biocluster_volpart_filtering_proc.h>
#include <imgr/file_formats/imgr_skyscan_log.h>
#include <xscan/xscan_scan.h>
#include <xscan/xscan_dummy_scan.h>

#include <vsl/vsl_basic_xml_element.h>

int main(int argc, char** argv)
{
  // Initialize defaults
  int returnStatus = 0;
  int verbose = TRACE_ERROR;
  bool binary = true;
  proc_io_run_xml_parser parser;
  std::string fname = "", bin_fname="";
  std::FILE *xmlFile;
  try
    {
      // Initialize MPI interface
      #ifdef MPI_CPP_BINDING
        MPI::Init (argc, argv);
      #else
        MPI_Init (&argc, &argv);
      #endif  // MPI_CPP_BINDING

      // Parse arguments
      for (int i = 1; i < argc; i++) {
        std::string arg (argv[i]);
        if (arg == std::string ("-v")) verbose = atoi (argv[++i]);
        else  if (arg == std::string ("-x")) { fname = std::string(argv[++i]);}
        else if (arg == std::string ("-b")) { binary = true; bin_fname = std::string (argv[++i]); }
        else
        {
          std::cout << "Usage: " << argv[0] << "[-help] [-v {0|1|2}] [-x xml_script]" << std::endl;
          throw -1;
        }
      }
  
    xmlFile = std::fopen(fname.c_str(), "r");
    if (!xmlFile){
      std::cout << fname << " error on opening" << std::endl;
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

   if (proc_flow_manage(parser, xmlFile, fname, bin_fname) == 1)
     return 1;
    // get the parameters from parser
  /*  std::string logfile = parser.log();
    std::string scanfile = parser.scan();
    std::string boxfile = parser.box();
    double filter_radius = parser.filter_radius();
    double filter_length = parser.filter_length();
    double res = parser.res();
    std::string outputfile = parser.output_file();

    if (file_check(logfile, scanfile, boxfile) == 0)
      return 1;
 
    // open output file to write the xml elements and the filter response
    std::ofstream xml_file(outputfile.data());
    xml_file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << "\n";

    // create a main node for the whole xml document
    vsl_basic_xml_element main_element("proc_filter_responses");
    main_element.x_write_open(xml_file);
    xml_file << "<proc_version>" << VERSION << "</proc_version>" << "\n";

    // get the scan from the file and correct the created scan
    imgr_skyscan_log log(logfile.data());
    xscan_scan scan = log.get_scan();
    std::cout << "SCAN BEFORE\n" << scan << std::endl;

    std::ifstream scan_file(scanfile.c_str());
    scan_file >> scan;
    scan_file.close();
    std::cout << "SCAN AFTER\n" << scan << std::endl;
    x_write(xml_file, scan);

    log.set_scan(scan);
    x_write(xml_file, log);

    //get the box
    std::ifstream box_file(boxfile.c_str());
    vgl_box_3d<double> box;
    box.read(box_file);
    box_file.close();
    std::cout << "BOX\n" << box << std::endl;
    x_write(xml_file, box, "active_box");

    // resolution
    double resolution = res / 1000.0;
      
    // filter 3d
    double f_radius = filter_radius / 1000.0;
    double f_length = filter_length / 1000.0;

    vgl_point_3d<double> f_centre(0.0, 0.0, 0.0);

    PROC_FILTER_TYPE filter_type = parser.filter_type();
    PROC_SPLAT_TYPE splatting_type = parser.splatting_type();
    PROC_ALGO_TYPE algo_type = parser.algo_type();

    std::vector<vgl_vector_3d<double> > orientation_list = parser.filter_orient();
    proc_run_args args(orientation_list, f_radius, f_length, f_centre, box, log, resolution, xml_file, bin_fname);
    if (run_proc(filter_type, splatting_type, algo_type, args) == 1)
      return 1;

    main_element.x_write_close(xml_file);*/

  } catch (int i) {
        // Error detected in this method
        returnStatus = i;
  } catch (BioClusterException& e) {
        // Error thrown by bioproc class
        if (e.GetError () != e.Incomplete)
        {
          if (verbose >= TRACE_ERROR) 
              std::cout << "BioProcException: " //<< e.GetMessage()
              << " err: " << e.GetError() << " mpi: " 
              << e.GetMPIError() << " sys: " 
              << e.GetSysError() << std::endl;
          returnStatus = e.GetError();
        }
  }
  
  // Terminate MPI
  #ifdef MPI_CPP_BINDING
    MPI::Finalize ();
  #else
    MPI_Finalize ();
  #endif  // MPI_CPP_BINDING
    return (returnStatus);
}
