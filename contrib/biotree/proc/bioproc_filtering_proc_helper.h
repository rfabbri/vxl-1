#ifndef FILTERING_PROC_HELPER_H_
#define FILTERING_PROC_HELPER_H_

// This is /contrib/biotree/proc/examples/bioproc_filtering_proc_helper.cxx

//: 
// \file    bioproc_filtering_proc_runner.cxx
// \brief   This header file contains the common methods used by proc examples
// \author  Gamze Tunali
// \date    April 29, 2006
// \verbatim
//    
//    

#include <vcl_ctime.h>
#include <vcl_vector.h>
#include <vgl/xio/vgl_xio_box_3d.h>
#include <vgl/xio/vgl_xio_vector_3d.h>

#include <proc/bioproc_filtering_proc.h>
#include <proc/bioproc_splr_filtering_proc.h>
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
#include <xmvg/xmvg_composite_filter_descriptor.h>
#include <imgr/file_formats/imgr_skyscan_log.h>
#include <xscan/xscan_scan.h>
#include <xscan/xscan_dummy_scan.h>
#include <vsl/vsl_basic_xml_element.h>

static int VERSION = 3;

// keeps the arguments used by proc
struct proc_run_args {
  vcl_vector<vgl_vector_3d<double> > &orients_;
  double f_sigma_;
  double f_radius_, f_length_;
  vgl_point_3d<double> f_centre_;
  vgl_box_3d<double> & box_;
  imgr_skyscan_log& log_;
  double resolution_;
  vcl_ofstream& xml_file_;
  vcl_string bin_scan_file_;
  proc_run_args(vcl_vector<vgl_vector_3d<double> > &orients,
           double f_radius, double f_length, vgl_point_3d<double> f_centre,
           vgl_box_3d<double> & box, imgr_skyscan_log& log,
           double resolution, vcl_ofstream& xml_file,
           vcl_string bin_scan_file) :
    orients_(orients), f_radius_(f_radius), f_length_(f_length),
    f_centre_(f_centre), box_(box),
    log_(log), resolution_(resolution),
    xml_file_(xml_file), bin_scan_file_(bin_scan_file) {}
};

//makes file extension check for scan, log and box files
int file_check(vcl_string logfile, vcl_string scanfile, vcl_string boxfile)
{
    
    vcl_size_t dot_pos = logfile.find_first_of(".");
    if(vcl_strcmp(logfile.substr(dot_pos+1, 3).data(), "log") != 0 && vcl_strcmp(logfile.substr(dot_pos+1, 3).data(), "LOG") != 0)
    {
      vcl_cout << "***********************************************" << vcl_endl;
      vcl_cout << "The specified log file extension is not correct" << vcl_endl;
      vcl_cout << "***********************************************" << vcl_endl;
      return 1;
    }
    dot_pos = scanfile.find_first_of(".");
    if(vcl_strcmp(scanfile.substr(dot_pos+1, 3).data(), "scn") != 0 && vcl_strcmp(scanfile.substr(dot_pos+1, 3).data(), "SCN") != 0)
    {
      vcl_cout << "************************************************" << vcl_endl;
      vcl_cout << "The specified scan file extension is not correct" << vcl_endl;
      vcl_cout << "************************************************" << vcl_endl;
      return 1;
    }
    dot_pos = boxfile.find_first_of(".");
    if(vcl_strcmp(boxfile.substr(dot_pos+1, 3).data(), "bx3") != 0 && vcl_strcmp(boxfile.substr(dot_pos+1, 3).data(), "BX3") != 0)
    {
      vcl_cout << "***********************************************" << vcl_endl;
      vcl_cout << "The specified box file extension is not correct" << vcl_endl;
      vcl_cout << "***********************************************" << vcl_endl;
      return 1;
    }
    return 0;
}

// computes the processing box around the given active box by using the 
// filters' bounding boxes
vgl_box_3d<double> 
compute_outer_box(vgl_box_3d<double> box, vcl_vector<xmvg_filter_descriptor> &d_list) 
{
  xmvg_composite_filter_descriptor comp_descr(d_list);
  vgl_box_3d<double> outer_box(box);
  vgl_box_3d<double> filter_bbox = comp_descr.bounding_box();
  outer_box.set_depth(outer_box.depth() + filter_bbox.depth());
  outer_box.set_width(outer_box.width() + filter_bbox.width());
  outer_box.set_height(outer_box.height() + filter_bbox.height());

  return outer_box;
}

// templated method for running the proc for centerline 
template <class filter_3d_t, class D, class T, template <class T, class F> class proc_t>
void run_centerline(proc_run_args args)
{
  vcl_vector<filter_3d_t> filters;
  vcl_vector<xmvg_filter_descriptor> descr;

  for (unsigned i=0; i<args.orients_.size(); i++) {
    D fd(args.f_radius_, args.f_length_, args.f_centre_, args.orients_[i]);
    descr.push_back(fd);
    T fp(fd);
    filter_3d_t f(fp);
    filters.push_back(f);
  }
 
  vgl_box_3d<double> outer_box = compute_outer_box(args.box_, descr);
  xmvg_composite_filter_3d<double, filter_3d_t> comp3d(filters);
  proc_t<double, filter_3d_t> proc(args.log_, args.box_, args.resolution_, outer_box, 
    comp3d, args.bin_scan_file_);
  proc.execute();
  x_write(args.xml_file_, proc);
  
}

// templated method for running the proc for edge detection 
template <class filter_3d_t, class D, class T, template <class T, class F> class proc_t>
void run_edge_detector(proc_run_args args)
{
  D edge_det_descr(args.f_sigma_, args.f_centre_);
  T edge_det(edge_det_descr);
  vcl_vector<filter_3d_t> filters;
  filters.push_back(edge_det);
  xmvg_composite_filter_3d<double, filter_3d_t> comp3d(filters);

  // create a vector of one element filter descriptor
  vcl_vector<xmvg_filter_descriptor> descr;
  descr.push_back(edge_det_descr);
  vgl_box_3d<double> outer_box = compute_outer_box(args.box_, descr);

  proc_t<double, filter_3d_t> proc(args.log_, args.box_, args.resolution_, outer_box, 
    comp3d, args.bin_scan_file_);
  proc.execute();
  x_write(args.xml_file_, proc);
  
}

// runs the proc with given arguments and the algorithm types
// returns 1 if there is an error, 0 otherwise
int run_proc(PROC_FILTER_TYPE filter_type,
              PROC_SPLAT_TYPE splatting_type, 
              PROC_ALGO_TYPE algo_type,
              proc_run_args proc_args) 
{
  if (filter_type == FILTER_UNDEF) {
    vcl_cerr << "bioproc_filtering_example_xml: Undefined Filter Type" << vcl_endl;
    vcl_cerr << "bioproc_filtering_example_xml: valid Filter Types {gaussian, no_noise, edge_detector}" << vcl_endl;
    return 1;
  }
  
  if (splatting_type == SPLAT_UNDEF) {
    vcl_cerr << "bioproc_filtering_example_xml: Undefined Splat Type" << vcl_endl;
    vcl_cerr << "bioproc_filtering_example_xml: valid Splat Types {parallel, conebeam}" << vcl_endl;
    return 1;
  }

  if (algo_type == ALGO_UNDEF) {
    vcl_cerr << "bioproc_filtering_example_xml: Undefined Algorithm Type" << vcl_endl;
    vcl_cerr << "bioproc_filtering_example_xml: valid Algorithm Types {proc, proc_splr}" << vcl_endl;
    return 1;
  }

  if (filter_type == GAUSSIAN) {
    if (splatting_type == PARALLEL) {
      typedef xmvg_parallel_beam_filter_3d<xmvg_gaussian_integrating_filter_3d> filter_3d_t;
      if (algo_type == PROC)
        run_centerline<filter_3d_t, xmvg_gaussian_filter_descriptor, xmvg_gaussian_integrating_filter_3d, bioproc_filtering_proc>(proc_args);
      else if (algo_type == PROC_SPLR)
        run_centerline<filter_3d_t, xmvg_gaussian_filter_descriptor, xmvg_gaussian_integrating_filter_3d, bioproc_splr_filtering_proc>(proc_args);
    } else if (splatting_type == CONE_BEAM) {
      if (algo_type == PROC)
        run_centerline<xmvg_gaussian_filter_3d, xmvg_gaussian_filter_descriptor, xmvg_gaussian_filter_3d, bioproc_filtering_proc>(proc_args);
      else if (algo_type == PROC_SPLR)
        run_centerline<xmvg_gaussian_filter_3d, xmvg_gaussian_filter_descriptor, xmvg_gaussian_filter_3d, bioproc_splr_filtering_proc>(proc_args);
    }
  } else if (filter_type == NO_NOISE) {
    if (splatting_type == PARALLEL) {
      typedef xmvg_parallel_beam_filter_3d<xmvg_pipe_filter_3d> filter_3d_t;
      if (algo_type == PROC)
        run_centerline<filter_3d_t, xmvg_no_noise_filter_descriptor, xmvg_pipe_filter_3d, bioproc_filtering_proc>(proc_args);
      else if (algo_type == PROC_SPLR)
        run_centerline<filter_3d_t, xmvg_no_noise_filter_descriptor, xmvg_pipe_filter_3d, bioproc_splr_filtering_proc>(proc_args);
    } else if (splatting_type == CONE_BEAM) {
      if (algo_type == PROC)
        run_centerline<xmvg_no_noise_filter_3d, xmvg_no_noise_filter_descriptor, xmvg_no_noise_filter_3d, bioproc_filtering_proc>(proc_args);
      else if (algo_type == PROC_SPLR)
        run_centerline<xmvg_no_noise_filter_3d, xmvg_no_noise_filter_descriptor, xmvg_no_noise_filter_3d, bioproc_splr_filtering_proc>(proc_args);
    } 
  }
  else if (filter_type == EDGE_DETECTOR_X)
    {
      if ((splatting_type == CONE_BEAM) && (algo_type == PROC)) {
        run_edge_detector<xmvg_gaussian_edge_detector_x_3d, xmvg_gaussian_edge_detector_descriptor, xmvg_gaussian_edge_detector_x_3d, bioproc_filtering_proc>(proc_args);
      } else 
        return 1;
    }
    else if (filter_type == EDGE_DETECTOR_Y)
    {
      if ((splatting_type == CONE_BEAM) && (algo_type == PROC)) {
        run_edge_detector<xmvg_gaussian_edge_detector_y_3d, xmvg_gaussian_edge_detector_descriptor, xmvg_gaussian_edge_detector_y_3d, bioproc_filtering_proc>(proc_args);
      } else 
        return 1;
    }
    else if (filter_type == EDGE_DETECTOR_Z)
    {
      if ((splatting_type == CONE_BEAM) && (algo_type == PROC)) {
        run_edge_detector<xmvg_gaussian_edge_detector_z_3d, xmvg_gaussian_edge_detector_descriptor, xmvg_gaussian_edge_detector_z_3d, bioproc_filtering_proc>(proc_args);
      } else 
        return 1;
    }

  return 0;
}

// manages the flow of the proc running process. First it reads the xml elements 
// from the parser, creates log, scan and box and prepares the resulting XML 
// file and runs the proc and closes the XML document. Returns 0 for successful 
// completion, 1 otherwise.
int proc_flow_manage(proc_io_run_xml_parser& parser, 
                     vcl_FILE *xmlFile, 
                     vcl_string fname, 
                     vcl_string bin_fname) 
{
  // get the parameters from parser
  static vcl_string logfile = parser.log();
  static vcl_string scanfile = parser.scan();
  static vcl_string boxfile = parser.box();
  double f_radius = parser.filter_radius()/1000.0;
  double f_length = parser.filter_length()/1000.0;
  double resolution = parser.res()/1000.0;
  vgl_point_3d<double> f_centre(0.0, 0.0, 0.0);
  static vcl_string outputfile = parser.output_file();

  if (file_check(logfile, scanfile, boxfile) == 1)
    return 1;
 
  // set log and scan
  imgr_skyscan_log log(logfile.data());
  xscan_scan scan = log.get_scan();
  vcl_ifstream scan_file(scanfile.c_str());
  scan_file >> scan;
  scan_file.close();
  log.set_scan(scan);
  
  //get the box
  vcl_ifstream box_file(boxfile.c_str());
  vgl_box_3d<double> box;
  box.read(box_file);
  box_file.close();

  PROC_FILTER_TYPE filter_type = parser.filter_type();
  PROC_SPLAT_TYPE splatting_type = parser.splatting_type();
  PROC_ALGO_TYPE algo_type = parser.algo_type();

    // open output file to write the xml elements and the filter response
  vcl_ofstream xml_file(outputfile.data()); 
  xml_file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << "\n";

  // create a main node for the whole xml document
  vsl_basic_xml_element main_element("proc_filter_responses");
  main_element.x_write_open(xml_file);
  xml_file << "<proc_version>" << VERSION << "</proc_version>" << "\n";

  x_write(xml_file, scan);
  x_write(xml_file, log);
  x_write(xml_file, box, "active_box");

  vcl_vector<vgl_vector_3d<double> > orientation_list = parser.filter_orient();
  proc_run_args args(orientation_list, f_radius, f_length, f_centre, box, log, resolution, xml_file, bin_fname);
  if (run_proc(filter_type, splatting_type, algo_type, args) == 1)
    return 1;
    
  main_element.x_write_close(xml_file);
  xml_file.close();
  return 0;
}

#endif
