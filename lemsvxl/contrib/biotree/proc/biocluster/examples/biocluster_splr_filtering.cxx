// This is /contrib/biotree/proc/examples/bioproc_filtering_example_xml.cxx

//: 
// \file    biocluster_splr_filtering_example_xml.cxx
// \brief   using the cluster together with pizza-slice symmetry splr
// \author  Philip Klein
// \date    April 17, 2006
#include <mpi.h>
#include <vcl_ctime.h>
#include <vcl_vector.h>
#include <vgl/xio/vgl_xio_box_3d.h>
#include <vgl/xio/vgl_xio_vector_3d.h>
#include <proc/bioproc_splr_filtering_proc.h>
#include <proc/bioproc_filtering_proc_helper.h>
#include <proc/io/proc_io_run_xml_parser.h>
#include <xmvg/xmvg_gaussian_edge_detector_descriptor.h>
#include <xmvg/xmvg_gaussian_edge_detector_x_3d.h>
#include <xmvg/xmvg_gaussian_edge_detector_y_3d.h>
#include <xmvg/xmvg_gaussian_edge_detector_z_3d.h>
#include <xmvg/xmvg_gaussian_filter_descriptor.h>
#include <xmvg/xmvg_gaussian_filter_3d.h>
#include <xmvg/xmvg_no_noise_filter_descriptor.h>
#include <xmvg/xmvg_no_noise_filter_3d.h>
#include <xmvg/xmvg_parallel_beam_filter_3d.h>
#include <xmvg/xmvg_gaussian_integrating_filter_3d.h>
#include <xmvg/xmvg_pipe_filter_3d.h>
#include <xmvg/xmvg_composite_filter_3d.h>
#include <xmvg/xmvg_filter_3d_wrapper.h>
#include <imgr/file_formats/imgr_skyscan_log.h>
#include <xscan/xscan_scan.h>
#include <xscan/xscan_dummy_scan.h>
#include <vsl/vsl_basic_xml_element.h>
#include <vcl_sstream.h>
//#include <proc/biocluster/biocluster_volpart_filtering_proc.h> //to get BioClusterException, some other

#define TRACE_ERROR     0
#define TRACE_INFO      1
#define TRACE_VERBOSE   2


class BioClusterException2 //named 2 because BioClusterException is contained within biocluster_volpart_filtering_proc.h, and I don't want to include that unnecessarily
{
public:
  // Error types
  typedef enum { Incomplete, MPIError, SystemError } BioClusterError;
private:
  // Bioproc error type
  BioClusterError m_error;
  // Exception message
  vcl_string m_message;
  // MPI specific error.
  int m_mpierror;
  // System specific error code.
  int m_syserror;
public:
  // Constructors
  // Default is thrown when a container tries to fetch the filter response on non-LeadProcessor
  BioClusterException2() : m_error (Incomplete), m_message ("Response field is only complete on processor 0"), m_mpierror (0), m_syserror (0) {}
  // Customizable exception instance
  BioClusterException2(BioClusterError err, vcl_string msg, int mpierr, int syserr) : m_error (err), m_message (msg), m_mpierror (mpierr), m_syserror (syserr) {}
  
  // Property accessors
  const char* GetMessage() { return m_message.data (); }
  int GetError() { return m_error; }
  int GetMPIError() { return m_mpierror; }
  int GetSysError() { return m_syserror; }
};

typedef biob_worldpt_field<xmvg_filter_response<double> >::values_t response_field_values_t;

void collect_responses(int num_filters, vcl_string output_filename_stem, int num_processors, response_field_values_t & response_field_values){
  for (int proc_number = 0; proc_number < num_processors; ++proc_number){
    //get the filename for the given processor number
    vcl_stringstream ss;
    ss << output_filename_stem << proc_number;
    //access the file
    vcl_ifstream response_file(ss.str().c_str());
    while (response_file){
      vcl_string buf;
      // get a line
      getline(response_file, buf);
      if (buf == "" || !response_file)
        break;
      //extract the worldpt_index and the response
      std::istringstream istring(buf);
      int worldpt_index;
      istring >> worldpt_index;
      xmvg_filter_response<double> response(num_filters, 0.);
      for (int i = 0; i < num_filters; i++){
        double response_value;
        istring >> response_value;
        response[i] = response_value;
      }
      //put the response in the response_field
      response_field_values[worldpt_index] = response;
    }//end of loop through the response file
  }//end of loop through processor numbers
}//end of procedure

struct run_args {
  vcl_vector<vgl_vector_3d<double> > &orients_;
  double f_radius_, f_length_;
  vgl_point_3d<double> f_centre_;
  vgl_box_3d<double> & box_;
  imgr_skyscan_log& log_;
  double resolution_;
  unsigned which_processor_, num_processors_;
  vcl_string output_filename_stem_;
  vcl_ofstream * xml_response_file_;
  vsl_basic_xml_element main_xml_element_;
  vcl_string bin_scan_file_;
  run_args(vcl_vector<vgl_vector_3d<double> > &orients,
           double f_radius, double f_length, vgl_point_3d<double> f_centre,
           vgl_box_3d<double> & box, imgr_skyscan_log& log,
           double resolution, unsigned which_processor, unsigned num_processors,
           vcl_string output_filename_stem, vcl_ofstream * xml_response_file,
           vsl_basic_xml_element main_xml_element,   vcl_string bin_scan_file) :
    orients_(orients), f_radius_(f_radius), f_length_(f_length),
    f_centre_(f_centre), box_(box),
    log_(log), resolution_(resolution), which_processor_(which_processor), 
    num_processors_(num_processors), output_filename_stem_(output_filename_stem),
    xml_response_file_(xml_response_file), main_xml_element_(main_xml_element) , bin_scan_file_(bin_scan_file) {}
};

template <class filter_descriptor_t, class basic_filter_t, template <class X> class filter_wrapper_t, template <class T, class F> class proc_t>
void run//<filter_descriptor_t, basic_filter_t, filter_wrapper_t, proc_t>
(run_args & args){
  typedef filter_wrapper_t<basic_filter_t> filter_3d_t;
  vcl_vector<filter_3d_t> filters;
  vcl_vector<xmvg_filter_descriptor> descr;
  for (unsigned i=0; i<args.orients_.size(); i++) {
    filter_descriptor_t nnfd(args.f_radius_, args.f_length_, args.f_centre_, args.orients_[i]);
    descr.push_back(nnfd);
    basic_filter_t fp(nnfd);
    filter_3d_t f(fp);
    filters.push_back(f);
  }
  // compute the outer box
  vgl_box_3d<double> outer_box = compute_outer_box(args.box_, descr);
  xmvg_composite_filter_3d<double, filter_3d_t> comp3d(filters);
  proc_t<double, filter_3d_t> proc(args.log_, args.box_, args.resolution_, outer_box, comp3d, args.bin_scan_file_, args.which_processor_, args.num_processors_);
  //create a file whose name incorporates processor number
  vcl_stringstream ss;
  ss << args.output_filename_stem_;
  ss << args.which_processor_;
  vcl_ofstream outputfile(ss.str().c_str());
  proc.execute(&outputfile);
  outputfile.close();
  MPI_Barrier(MPI_COMM_WORLD);//wait for all processors to finish
  if (args.which_processor_ == 0){
    collect_responses(comp3d.size(), args.output_filename_stem_, args.num_processors_, proc.response_field().values());
    x_write(*(args.xml_response_file_), proc);
    args.main_xml_element_.x_write_close(*args.xml_response_file_);
    args.xml_response_file_->close();
  }
}


int main(int argc, char** argv)
{
  // Initialize defaults
  int returnStatus = 0;
  int verbose = TRACE_ERROR;
  vcl_string fname;//file path for xml script
  vcl_string binary_image_file; //file path for binary image file
  vcl_string tag1;//will be assigned "-x"
  vcl_string tag2;//if optional arg provided, will be assigned "-b"
  proc_io_run_xml_parser parser;
  vcl_FILE *xmlFile;
  try
    {
      // Fetch the total number of processors and our rank - a value from 0 to m_TotalProcessors - 1
      char*  m_ComputerName = new char [MPI_MAX_PROCESSOR_NAME];
      int nameSize;
      int m_TotalProcessors;
      int m_MyRank;
      
      // Initialize MPI interface
#ifdef MPI_CPP_BINDING
      MPI::Init (argc, argv);
      m_TotalProcessors = MPI::COMM_WORLD.Get_size ();
      m_MyRank = MPI::COMM_WORLD.Get_rank ();
      MPI::Get_processor_name (m_ComputerName, nameSize);
#else
      MPI_Init (&argc, &argv);
      MPI_Comm_size (MPI_COMM_WORLD, &m_TotalProcessors);
      MPI_Comm_rank (MPI_COMM_WORLD, &m_MyRank);
      MPI_Get_processor_name (m_ComputerName, &nameSize);
#endif  // MPI_CPP_BINDING
      // Parse arguments
      // argument is xml script
      if (argc >= 3){
        tag1 = argv[1];//should be "-x"
        fname = argv[2];
      }
      if (argc == 5){
        tag2 = argv[3];//should be "-b"
        binary_image_file = argv[4];
      }
      if (tag1 != "-x" || argc == 5 && tag2 != "-b" || argc != 3 && argc != 5){
          vcl_cout << "Usage: " << argv[0] << "-x <xml_script_filename> [-b <binary_image_file_path>]\n";
          throw -1;
      }
      
      xmlFile = vcl_fopen(fname.c_str(), "r");
      if (!xmlFile){
        vcl_cout << fname << " error on opening" << vcl_endl;
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
      vcl_cout << "parsing finished!" << vcl_endl;
      
      // get the parameters from parser
      vcl_string logfile = parser.log();
      vcl_string scanfile = parser.scan();
      vcl_string boxfile = parser.box();
      double filter_radius = parser.filter_radius();
      double filter_length = parser.filter_length();
      double res = parser.res();
      vcl_string outputfile = parser.output_file();
      
      if (file_check(logfile, scanfile, boxfile) == 0)
        return 1;
      
      //find the directory containing the output file
      int pos = outputfile.rfind("/", outputfile.size()-1);
      if (pos == vcl_string::npos){
        pos = -1;
      }
      vcl_string output_file_path_only = outputfile.substr(0, pos+1);
      vcl_string output_filename_stem = output_file_path_only + "temp";
      
      imgr_skyscan_log log(logfile.data());
      xscan_scan scan = log.get_scan();
      vcl_cout << "SCAN BEFORE\n" << scan << vcl_endl;
      
      vcl_ifstream scan_file(scanfile.c_str());
      scan_file >> scan;
      scan_file.close();
      vcl_cout << "SCAN AFTER\n" << scan << vcl_endl;
      
      //imgr_skyscan_log log(logfile.data()); 
      log.set_scan(scan);
      //get the box
      vcl_ifstream box_file(boxfile.c_str());
      vgl_box_3d<double> box;
      box.read(box_file);
      box_file.close();
      vcl_cout << "BOX\n" << box << vcl_endl;
      
      vcl_ofstream * maybe_xml_response_file = 0;
      vsl_basic_xml_element main_element("proc_filter_responses");
      if (m_MyRank == 0){
        // open output file to write the xml elements and the filter response
        maybe_xml_response_file = new vcl_ofstream(outputfile.data());
        *maybe_xml_response_file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << "\n";
        // create a main node for the whole xml document
        main_element.x_write_open(*maybe_xml_response_file);
        *maybe_xml_response_file << "<proc_version>" << VERSION << "</proc_version>" << "\n";
        x_write(*maybe_xml_response_file, scan);
        x_write(*maybe_xml_response_file, log);
        x_write(*maybe_xml_response_file, box, "active_box");
      }
      
      // resolution
      double resolution = res / 1000.0;
      
      // filter 3d
      double f_radius = filter_radius / 1000.0;
      double f_length = filter_length / 1000.0;
      
      vgl_point_3d<double> f_centre(0.0, 0.0, 0.0);
      
      PROC_FILTER_TYPE filter_type = parser.filter_type();
      PROC_SPLAT_TYPE splatting_type = parser.splatting_type();
      vcl_vector<vgl_vector_3d<double> > orientation_list = parser.filter_orient();
      run_args args(orientation_list, f_radius, f_length, f_centre, box, log, resolution, 
        m_MyRank, m_TotalProcessors, output_filename_stem, maybe_xml_response_file, 
        main_element, binary_image_file);
      vcl_cout << "about to run\n";
      if (filter_type == GAUSSIAN) {
        if (splatting_type == PARALLEL)
          run<xmvg_gaussian_filter_descriptor,xmvg_gaussian_integrating_filter_3d, xmvg_parallel_beam_filter_3d, bioproc_splr_filtering_proc>(args);
        else if (splatting_type == CONE_BEAM)
          run<xmvg_gaussian_filter_descriptor,xmvg_gaussian_filter_3d, xmvg_filter_3d_wrapper, bioproc_splr_filtering_proc>(args);
        else 
          return 1;
      } else if (filter_type == NO_NOISE) {
        if (splatting_type == PARALLEL)
          run<xmvg_no_noise_filter_descriptor, xmvg_pipe_filter_3d, xmvg_parallel_beam_filter_3d, bioproc_splr_filtering_proc>(args);
        else if (splatting_type == CONE_BEAM)
          run<xmvg_no_noise_filter_descriptor, xmvg_no_noise_filter_3d, xmvg_filter_3d_wrapper, bioproc_splr_filtering_proc>(args);
        else 
          return 1;
      }
      else if (filter_type == EDGE_DETECTOR_X)
        {
          if (splatting_type == CONE_BEAM)
            assert(false);//not implemented: run<xmvg_gaussian_edge_detector_descriptor, xmvg_gaussian_edge_detector_x_3d, xmvg_filter_3d_wrapper, bioproc_splr_filtering_proc>(args);
          else 
            return 1;
        }
      else if (filter_type == EDGE_DETECTOR_Y)
        {
          if (splatting_type == CONE_BEAM)
            assert(false);//not implemented: run<xmvg_gaussian_edge_detector_descriptor, xmvg_gaussian_edge_detector_y_3d, xmvg_filter_3d_wrapper, bioproc_splr_filtering_proc>(args);
          else 
            return 1;
        }
      else if (filter_type == EDGE_DETECTOR_Z)
        {
          if (splatting_type == CONE_BEAM)
            assert(false);//not implemented: run<xmvg_gaussian_edge_detector_descriptor, xmvg_gaussian_edge_detector_z_3d, xmvg_filter_3d_wrapper, bioproc_splr_filtering_proc>(args);
          else 
            return 1;
        }
    } catch (int i) {
    // Error detected in this method
    returnStatus = i;
  } catch (BioClusterException2& e) {
    // Error thrown by bioproc class
    if (e.GetError () != e.Incomplete)
      {
        if (verbose >= TRACE_ERROR) 
          vcl_cout << "BioProcException: " //<< e.GetMessage()
                   << " err: " << e.GetError() << " mpi: " 
                   << e.GetMPIError() << " sys: " 
                   << e.GetSysError() << vcl_endl;
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
