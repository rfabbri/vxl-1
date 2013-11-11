// This is /contrib/biotree/proc/examples/bioproc_filtering_example_xml.cxx

//: 
// \file    bioproc_filtering_example_xml.cxx
// \brief   an example to use bioproc_splr_filtering_proc with volume rendering to show its 
//          response field on fishline phantom of SkyScan data, modified to use parallel beam
// \author  Gamze Tunali
// \date    March 10, 2006

#include <vcl_ctime.h>
#include <vcl_vector.h>
#include <vgl/xio/vgl_xio_box_3d.h>
#include <vgl/xio/vgl_xio_vector_3d.h>

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
#include <proc/bioproc_splr_filtering_proc.h>
#include <imgr/file_formats/imgr_skyscan_log.h>
#include <xscan/xscan_scan.h>
#include <xscan/xscan_dummy_scan.h>
#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui.h>
#include <bgui3d/bgui3d.h>
#include <bgui3d/bgui3d_examiner_tableau.h>

#include <vsl/vsl_basic_xml_element.h>

int VERSION = 2;

void run_gaussian_parallel(vcl_vector<vgl_vector_3d<double> > &orients,
                           double f_radius, double f_length, 
                           vgl_point_3d<double> f_centre,
                           vgl_box_3d<double> & box, 
                           vgl_box_3d<double> & small_box, 
                           imgr_skyscan_log& log,
                           double resolution, 
                           vcl_string bin_scan_file,
                           vcl_ostream& os)
{
  typedef xmvg_parallel_beam_filter_3d<xmvg_gaussian_integrating_filter_3d> filter_3d_t;
  vcl_vector<filter_3d_t> filters;
  
  for (unsigned i=0; i<orients.size(); i++) {
    xmvg_gaussian_filter_descriptor nnfd(f_radius, f_length, f_centre, orients[i]);
    xmvg_gaussian_integrating_filter_3d fp(nnfd);
    filter_3d_t f(fp);
    filters.push_back(f);
  }
 
  xmvg_composite_filter_3d<double, filter_3d_t> comp3d(filters);
  bioproc_splr_filtering_proc<double, filter_3d_t> proc(log, small_box, resolution, box, comp3d, bin_scan_file);
  proc.execute();
  x_write(os, proc);
  vcl_cout << "finished " << vcl_endl;
}

void run_gaussian_cone_beam(vcl_vector<vgl_vector_3d<double> > &orients,
                            double f_radius, double f_length, 
                            vgl_point_3d<double> f_centre,
                            vgl_box_3d<double> & box, 
                            vgl_box_3d<double> & small_box, 
                            imgr_skyscan_log& log,
                            double resolution, 
                           vcl_string bin_scan_file,
                            vcl_ostream& os)
{
  vcl_vector<xmvg_gaussian_filter_3d> filters;
  for (unsigned i=0; i<orients.size(); i++) {
    xmvg_gaussian_filter_descriptor fd(f_radius, f_length, f_centre, orients[i]);
    xmvg_gaussian_filter_3d f(fd);
    filters.push_back(f);
  }
  xmvg_composite_filter_3d<double, xmvg_gaussian_filter_3d> comp3d(filters);
  bioproc_splr_filtering_proc<double, xmvg_gaussian_filter_3d> proc(log, small_box, resolution, box, comp3d, bin_scan_file);
 
  // do the filtering
  proc.execute();
  x_write(os, proc);
}

void run_no_noise_parallel(vcl_vector<vgl_vector_3d<double> > &orients,
                           double f_radius, double f_length, 
                           vgl_point_3d<double> f_centre,
                           vgl_box_3d<double> & box, 
                           vgl_box_3d<double> & small_box, 
                           imgr_skyscan_log& log,
                           double resolution,
                           vcl_string bin_scan_file, vcl_ostream& os)
{
  typedef xmvg_parallel_beam_filter_3d<xmvg_pipe_filter_3d> filter_3d_t;
  vcl_vector<filter_3d_t> filters;

  for (unsigned i=0; i<orients.size(); i++) {
    xmvg_no_noise_filter_descriptor fd(f_radius, f_length, f_centre, orients[i]);
    xmvg_pipe_filter_3d fp(fd);
    filter_3d_t f(fp);
    filters.push_back(f);
  }
  
  xmvg_composite_filter_3d<double, filter_3d_t> comp3d(filters);
  bioproc_splr_filtering_proc<double, filter_3d_t> proc(log, small_box, resolution, box, comp3d, bin_scan_file);

  // do the filtering
  proc.execute();
  x_write(os, proc);
}

void run_no_noise_cone_beam(vcl_vector<vgl_vector_3d<double> > &orients,
                           double f_radius, double f_length, 
                           vgl_point_3d<double> f_centre,
                           vgl_box_3d<double> & box, 
                           vgl_box_3d<double> & small_box, 
                           imgr_skyscan_log& log,
                           double resolution, vcl_string bin_scan_file, vcl_ostream& os)
{
  vcl_vector<xmvg_no_noise_filter_3d> filters;

  for (unsigned i=0; i<orients.size(); i++) {
    xmvg_no_noise_filter_descriptor fd(f_radius, f_length, f_centre, orients[i]);
    xmvg_no_noise_filter_3d f(fd);
    filters.push_back(f);
  }
 
  xmvg_composite_filter_3d<double, xmvg_no_noise_filter_3d> comp3d(filters);
  bioproc_splr_filtering_proc<double, xmvg_no_noise_filter_3d> proc(log, small_box, resolution, box, comp3d, bin_scan_file);

  // do the filtering
  proc.execute();
  x_write(os, proc);
}

void run_edge_detector_x_cone_beam(double f_sigma, vgl_point_3d<double> f_centre,
                                  vgl_box_3d<double> & box, 
                                  vgl_box_3d<double> & small_box, 
                                  imgr_skyscan_log& log,
                                  double resolution, vcl_string bin_scan_file, vcl_ostream& os)
{
  xmvg_gaussian_edge_detector_descriptor edge_det_desc(f_sigma, f_centre);
  xmvg_gaussian_edge_detector_x_3d edge_det_x(edge_det_desc);
  vcl_vector<xmvg_gaussian_edge_detector_x_3d> filters;
  filters.push_back(edge_det_x);
  xmvg_composite_filter_3d<double, xmvg_gaussian_edge_detector_x_3d> comp3d(filters);
  bioproc_splr_filtering_proc<double, xmvg_gaussian_edge_detector_x_3d> proc(log, small_box, resolution, box, comp3d, bin_scan_file);
  
  // do the filtering
  proc.execute();
  x_write(os, proc);
}

void run_edge_detector_y_cone_beam(double f_sigma, vgl_point_3d<double> f_centre,
                                  vgl_box_3d<double> & box, 
                                  vgl_box_3d<double> & small_box, 
                                  imgr_skyscan_log& log,
                                  double resolution, vcl_string bin_scan_file, vcl_ostream& os)
{
  xmvg_gaussian_edge_detector_descriptor edge_det_desc(f_sigma, f_centre);
  xmvg_gaussian_edge_detector_y_3d edge_det_y(edge_det_desc);
  vcl_vector<xmvg_gaussian_edge_detector_y_3d> filters;
  filters.push_back(edge_det_y);
  xmvg_composite_filter_3d<double, xmvg_gaussian_edge_detector_y_3d> comp3d(filters);
  bioproc_splr_filtering_proc<double, xmvg_gaussian_edge_detector_y_3d> proc(log, small_box, resolution, box, comp3d, bin_scan_file);
  
  // do the filtering
  proc.execute();
  x_write(os, proc);
}

void run_edge_detector_z_cone_beam(double f_sigma, vgl_point_3d<double> f_centre,
                                  vgl_box_3d<double> & box, 
                                  vgl_box_3d<double> & small_box, 
                                  imgr_skyscan_log& log,
                                  double resolution, vcl_string bin_scan_file, vcl_ostream& os)
{
  xmvg_gaussian_edge_detector_descriptor edge_det_desc(f_sigma, f_centre);
  xmvg_gaussian_edge_detector_z_3d edge_det_z(edge_det_desc);
  vcl_vector<xmvg_gaussian_edge_detector_z_3d> filters;
  filters.push_back(edge_det_z);
  xmvg_composite_filter_3d<double, xmvg_gaussian_edge_detector_z_3d> comp3d(filters);
  bioproc_splr_filtering_proc<double, xmvg_gaussian_edge_detector_z_3d> proc(log, small_box, resolution, box, comp3d, bin_scan_file);

  // do the filtering
  proc.execute(); 
  x_write(os, proc);
}

int file_check(vcl_string logfile, vcl_string scanfile, vcl_string boxfile)
{
//file extension check
    vcl_size_t dot_pos = logfile.find_first_of(".");
    if(vcl_strcmp(logfile.substr(dot_pos+1, 3).data(), "log") != 0 && vcl_strcmp(logfile.substr(dot_pos+1, 3).data(), "LOG") != 0)
    {
      vcl_cout << "***********************************************" << vcl_endl;
      vcl_cout << "The specified log file extension is not correct" << vcl_endl;
      vcl_cout << "***********************************************" << vcl_endl;
      return 0;
    }
    dot_pos = scanfile.find_first_of(".");
    if(vcl_strcmp(scanfile.substr(dot_pos+1, 3).data(), "scn") != 0 && vcl_strcmp(scanfile.substr(dot_pos+1, 3).data(), "SCN") != 0)
    {
      vcl_cout << "************************************************" << vcl_endl;
      vcl_cout << "The specified scan file extension is not correct" << vcl_endl;
      vcl_cout << "************************************************" << vcl_endl;
      return 0;
    }
    dot_pos = boxfile.find_first_of(".");
    if(vcl_strcmp(boxfile.substr(dot_pos+1, 3).data(), "bx3") != 0 && vcl_strcmp(boxfile.substr(dot_pos+1, 3).data(), "BX3") != 0)
    {
      vcl_cout << "***********************************************" << vcl_endl;
      vcl_cout << "The specified box file extension is not correct" << vcl_endl;
      vcl_cout << "***********************************************" << vcl_endl;
      return 0;
    }
    return 1;
}

int main(int argc, char** argv)
{
  proc_io_run_xml_parser parser;
  if(argc < 2 || argc > 3) {
    vcl_cout <<"usage: "<<argv[0] << "xml_run_script [binary_scan_file]\n";
    vcl_exit(1);
  }
  vcl_string fname = argv[1];//path of script file
  vcl_string bin_file; //path of binary image data file (if given on command-line
  if (argc == 3){
    bin_file = argv[2];
  }
  vcl_FILE *xmlFile;
  xmlFile = vcl_fopen(fname.c_str(), "r");
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
  vcl_cout << "parsing finished!" << vcl_endl;

  // get the parameters from parser
  static vcl_string logfile = parser.log();
  static vcl_string scanfile = parser.scan();
  static vcl_string boxfile = parser.box();
  double filter_radius = parser.filter_radius();
  double filter_length = parser.filter_length();
  double res = parser.res();
  static vcl_string outputfile = parser.output_file();
  vgl_vector_3d<double> scale(parser.scale_x(), parser.scale_y(), parser.scale_z());

  if (file_check(logfile, scanfile, boxfile) == 0)
    return 0;
 
  // open output file to write the xml elements and the filter response
  vcl_ofstream xml_file(outputfile.data());
  xml_file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << "\n";

  // create a main node for the whole xml document
  vsl_basic_xml_element main_element("proc_filter_responses");
  main_element.x_write_open(xml_file);
  xml_file << "<proc_version>" << VERSION << "</proc_version>" << "\n";

  // get the scan from the file and correct the created scan
  imgr_skyscan_log log(logfile.data());
  xscan_scan scan = log.get_scan();
  vcl_cout << "SCAN BEFORE\n" << scan << vcl_endl;

  vcl_ifstream scan_file(scanfile.c_str());
  scan_file >> scan;
  scan_file.close();
  //xscan_scan const & scan_ref = dynamic_cast<xscan_scan const&>(scan);
  vcl_cout << "SCAN AFTER\n" << scan << vcl_endl;
  x_write(xml_file, scan);

  log.set_scan(scan);
  x_write(xml_file, log);
  // get the box
  vcl_ifstream box_file(boxfile.c_str());
  vgl_box_3d<double> box;
  box.read(box_file);
  box_file.close();
  vcl_cout << "BOX\n" << box << vcl_endl;
  x_write(xml_file, box, "processing_box");
  vcl_cout << "hi\n";
  vgl_point_3d<double> centroid = box.centroid();
  vcl_cout << "bioproc_splr_filtering_example_xml  centroid " << centroid.x() << " " << centroid.y() << " " << centroid.z() << "\n";
  double centroid_array[3] = {centroid.x(), centroid.y(), centroid.z()};

  vgl_box_3d<double> smallbox(centroid_array, 
     scale.x() * box.width(),
     scale.y() * box.height(), 
     scale.z() * box.depth(),
     vgl_box_3d<double>::centre);
    x_write(xml_file, smallbox, "active_box");
    x_write(xml_file, scale, "scale_factor");
   

   
  // resolution
  double resolution = res / 1000.0;
  vsl_basic_xml_element res_elm("resolution");
  res_elm.append_cdata(resolution);
  res_elm.x_write(xml_file);

  // filter 3d
  double f_radius = filter_radius / 1000.0;
  double f_length = filter_length / 1000.0;

  vgl_point_3d<double> f_centre(0.0, 0.0, 0.0);

  double a = (1. + vcl_sqrt(5.0))/2.;

  PROC_FILTER_TYPE filter_type = parser.filter_type();
  PROC_SPLAT_TYPE splatting_type = parser.splatting_type();
  vcl_vector<vgl_vector_3d<double> > orientation_list = parser.filter_orient();

  if (filter_type == GAUSSIAN) {
    if (splatting_type == PARALLEL) 
      run_gaussian_parallel(orientation_list, f_radius, f_length, f_centre,
                            box, smallbox, log, resolution, bin_file, xml_file);
    else if (splatting_type == CONE_BEAM) 
      run_gaussian_cone_beam(orientation_list, f_radius, f_length, f_centre,
                              box, smallbox, log, resolution, bin_file, xml_file);
    else 
      return 0;
    
  } else if (filter_type == NO_NOISE) {
      if (splatting_type == PARALLEL)
        run_no_noise_parallel(orientation_list, f_radius, f_length, f_centre,
                              box, smallbox, log, resolution, bin_file, xml_file);
      else if (splatting_type == CONE_BEAM)
        run_no_noise_cone_beam(orientation_list, f_radius, f_length, f_centre,
                              box, smallbox, log, resolution, bin_file, xml_file);
      else 
        return 0;
  } else if (filter_type == EDGE_DETECTOR_X) {
      if (splatting_type == CONE_BEAM)
        run_edge_detector_x_cone_beam(f_radius, f_centre, box, smallbox, log, resolution, bin_file, xml_file);
      else 
        return 0;
  } else if (filter_type == EDGE_DETECTOR_Y) {
      if (splatting_type == CONE_BEAM)
        run_edge_detector_y_cone_beam(f_radius, f_centre, box, smallbox, log, resolution, bin_file, xml_file);
      else 
        return 0;
  } else if (filter_type == EDGE_DETECTOR_Z) {
      if (splatting_type == CONE_BEAM)
        run_edge_detector_z_cone_beam(f_radius, f_centre, box, smallbox, log, resolution, bin_file, xml_file);
      else 
        return 0;
  }

  main_element.x_write_close(xml_file);
  xml_file.close();
}
