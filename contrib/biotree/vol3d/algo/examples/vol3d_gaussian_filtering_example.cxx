// This is /contrib/biotree/vol3d/vol3d_gaussian_filtering_example.cxx

//: 
// \file    vol3d_gaussian_filtering_example.cxx
// \brief   an example to use bioproc_gaussian_filtering_proc with reconstructed 3D data
//          It takes 3 arguments, first is the path to the XML script for necessary file 
//          paths and the second is path to a volume data file like *.gipl or a path with 
//          wild characters to the reconstructed images like: 
//          C:\\test_images\\filters\\newcast35um2_orig\\scan35um_rec####.bmp. The third 
//          arguments specifies if a readius detection is incorporated, if -r is one of the 
//          arguments, radius detection is going to be executed.
//
//          Note: If the filter needed to be run on the whole 3D area, the box should be 
//          given as an empty box. The box path (from xml script) should be valid, but the 
//          content should indicate an empty box like min=(0,0,0) and max=(0,0,0)
//          
// \author  Gamze Tunali
// \date    May 01, 2006
// \verbatim
//  Modifications
//   06-06-06 - H. Can Aras - Added functions used for debugging purposes
// \endverbatim

#include <vcl_ctime.h>
#include <vcl_vector.h>

#include <vil/vil_load.h>
#include <vil3d/vil3d_image_resource.h>
#include <vil3d/vil3d_load.h>

#include <vgl/vgl_box_2d.h>
#include <vgl/xio/vgl_xio_box_3d.h>
#include <vgl/xio/vgl_xio_vector_3d.h>
#include <vsl/vsl_basic_xml_element.h>

#include <vol3d/algo/io/vol3d_reader.h>
#include <vol3d/algo/vol3d_gaussian_filtering_proc.h>
#include <proc/io/proc_io_run_xml_parser.h>
#include <imgr/file_formats/imgr_skyscan_log.h>
#include <imgr/file_formats/imgr_skyscan_reconlog.h>
#include <xscan/xscan_scan.h>
#include <xscan/xscan_dummy_scan.h>


int VERSION = 2;
int KERNEL_SIZE = 10;

int file_check(vcl_string logfile, vcl_string scanfile, vcl_string boxfile)
{
  //file extension check
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
    return 1;
}

// When we convert world coordinates to SkyScan reconstructed coordinates, we get floating point
// values. However, we should have integer values to crop a region of interest from the
// reconstructed volume. This function rounds the floating point reconstructed coordinates to
// integers, and corrects the values if the values fall out of the dimension bounds.
void round_box_limits(int width, int height, int depth,
                      vgl_point_3d<double> box_min, vgl_point_3d<double> box_max, 
                      int &minx, int &miny, int &minz, int &maxx, int &maxy, int &maxz)
{
  if(box_min.x() - vcl_floor(box_min.x()) >= 0.5)
    minx = int(vcl_ceil(box_min.x()));
  else
    minx = int(vcl_floor(box_min.x()));

  if(box_max.x() - vcl_floor(box_max.x()) >= 0.5)
    maxx = int(vcl_ceil(box_max.x()));
  else
    maxx = int(vcl_floor(box_max.x()));

  if(box_min.y() - vcl_floor(box_min.y()) >= 0.5)
    miny = int(vcl_ceil(box_min.y()));
  else
    miny = int(vcl_floor(box_min.y()));

  if(box_max.y() - vcl_floor(box_max.y()) >= 0.5)
    maxy = int(vcl_ceil(box_max.y()));
  else
    maxy = int(vcl_floor(box_max.y()));

  if(box_min.z() - vcl_floor(box_min.z()) >= 0.5)
    minz = int(vcl_ceil(box_min.z()));
  else
    minz = int(vcl_floor(box_min.z()));

  if(box_max.z() - vcl_floor(box_max.z()) >= 0.5)
    maxz = int(vcl_ceil(box_max.z()));
  else
    maxz = int(vcl_floor(box_max.z()));

  if(minx < 0)
    minx = 0;
  if(maxx >= width)
    maxx = width-1;
  if(miny < 0)
    miny = 0;
  if(maxy >= height)
    maxy = height-1;
  if(minz < 0)
    minz = 0;
  if(maxz >= depth)
    maxz = depth-1;
}

// Saves the cropped volume as a plain text file
// Can be viewed by text editor
// Can be read and visualized using MatLab
// Format is simple: dimensions (w,h,d) on the first line followed by the cropped image planes
// Check vol_file_example under the folder
template <class T> void save_ROI(vil3d_image_view<T> view, const char *filename, 
                                 int minx, int miny, int minz, int maxx, int maxy, int maxz)
{
  FILE *fp = vcl_fopen(filename, "w");
  vcl_fprintf(fp, "%d %d %d\n", maxx-minx+1, maxy-miny+1, maxz-minz+1);
  int indexz = 0;
  for(int k = minz; k <= maxz; k++)
  {
    int indexy = 0;
    for(int j = miny; j <= maxy; j++)
    {
      int indexx = 0;
      for(int i = minx; i <= maxx; i++)
      {
        vcl_fprintf(fp, "%d ", static_cast<unsigned short>(view(i,j,k)));
        indexx++;
      }
      vcl_fprintf(fp, "\n");
      indexy++;
    }
    vcl_fprintf(fp, "\n");
    indexz++;
  }
  fclose(fp);
}

template <class T>
void run_proc(bool radius_det, vcl_string radius_fname,
              vil3d_image_resource_sptr img_res_sptr,
              vcl_vector<vol3d_gaussian_kernel_3d> &kernel_list,
              //vgl_box_3d<double> &recon_box,
              vcl_ofstream& xml_file)
{

  vol3d_gaussian_filtering_proc<T> proc(img_res_sptr, kernel_list);
  // radius detection is decided by the input arguments
  if (radius_det) {
    if (radius_fname.size() > 0) {
        // it is going to use the given radius data while computing filter resp.
        vil3d_image_resource_sptr radius_res_sptr = vil3d_load_image_resource(radius_fname.c_str()); 
        vcl_cout << "Radius ni=" << radius_res_sptr->ni() << " nj=" << radius_res_sptr->nj() << 
        " nk=" << radius_res_sptr->nk() << vcl_endl;
        proc.execute_with_rad_det(radius_res_sptr);
      } else {
        // no radius data is given, it will compute it itself before filtering
        proc.execute_with_rad_det();
      }
    } else {
      // no radius detection, it will use the filter size from xml file
      proc.execute();
    } 
  x_write(xml_file, proc);
   
}

int main(int argc, char** argv)
{
  proc_io_run_xml_parser parser;
  vcl_FILE *xmlFile;
  vcl_string fname="";
  vcl_string recon_fname = "";
  vcl_string radius_fname = "";
  bool radius_det = false;

  // Parse arguments
  for (int i = 1; i < argc; i++) {
    vcl_string arg (argv[i]);
    if (arg == vcl_string ("-x")) { fname = vcl_string(argv[++i]);}

    // this argument can be a volume data file like *.gipl or a path with wild characters to the 
    // reconstructed images like: C:\\test_images\\filters\\newcast35um2_orig\\scan35um_rec####.bmp
    else if (arg == vcl_string ("-b")) {recon_fname = vcl_string(argv[++i]);}
    else if (arg == vcl_string ("-r")) {
      radius_det = true;  
      if ((i+1) < argc)
        radius_fname = vcl_string(argv[++i]);}
    else
    {
      vcl_cout << "Usage: " << argv[0] << "[-x xml_script] " << vcl_endl;
      throw -1;
    }
  }
  
  if (  fname == ""){
    vcl_cout << "XML File not specified" << vcl_endl; 
    return(1);
  }

  if (  recon_fname == ""){
    vcl_cout << "3D Data File (Path) not specified" << vcl_endl; 
    return(1);
  }
    
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
  static vcl_string outputfile = parser.output_file();

  if (file_check(logfile, scanfile, boxfile) == 0)
    return 1;
 
  // open output file to write the xml elements and the filter response
  vcl_ofstream xml_file(outputfile.data());
  xml_file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << "\n";

  // create a main node for the whole xml document
  vsl_basic_xml_element main_element("proc_filter_responses");
  main_element.x_write_open(xml_file);
  xml_file << "<proc_version>" << VERSION << "</proc_version>" << "\n";

  imgr_skyscan_log log(logfile.data());
  xscan_scan scan = log.get_scan();
  vcl_cout << "SCAN BEFORE\n" << scan << vcl_endl;

  vcl_ifstream scan_file(scanfile.c_str());
  scan_file >> scan;
  scan_file.close();

  vcl_cout << "SCAN AFTER\n" << scan << vcl_endl;
  x_write(xml_file, scan);


  log.set_scan(scan);
  x_write(xml_file, log);

  //get the box
  vcl_ifstream box_file(boxfile.c_str());
  vgl_box_3d<double> box;
  box.read(box_file);

  box_file.close();
  vcl_cout << "BOX\n" << box << vcl_endl;
  x_write(xml_file, box, "processing_box");
      
  // filter 3d
  double f_radius = filter_radius;
  double f_length = filter_length;

  imgr_skyscan_reconlog header(logfile.data(), scan);

  vcl_vector<vgl_vector_3d<double> > orientation_list = parser.filter_orient();

  // create gaussian filter kernels
  vcl_vector<vol3d_gaussian_kernel_3d> kernel_list;
  double sigma_r = f_radius;
  double sigma_z = f_length/2.0;
  for (unsigned i=0; i<orientation_list.size(); i++) {
    vol3d_gaussian_kernel_3d kernel(KERNEL_SIZE, header.voxel_size_, sigma_r, 
      sigma_z, orientation_list[i]);
    kernel_list.push_back(kernel);
  }

  vgl_point_3d<double> box_min, box_max;
  
  if (!box.is_empty()) {
    box_min = header.bsc_to_fbpc(box.min_point());
    box_max = header.bsc_to_fbpc(box.max_point());

    // adjust the box index, based on the starting section
    //box_min.set(box_min.x(), box_min.y(), box_min.z()); // - header.start_slice_);
    //box_max.set(box_max.x(), box_max.y(), box_max.z()); // - header.start_slice_);
  }
  // newcast35um, curvy region
  //box_min.set(510, 309, 250);
  //box_max.set(529, 348, 280);

  // mercox
  //box_min.set(33, 612, 617);
  //box_max.set(43, 707, 722);
  vgl_box_3d<int> recon_box(box_min.x(), box_min.y(), box_min.z(), box_max.x(), 
    box_max.y(), box_max.z());

  vol3d_reader reader(recon_fname, recon_box);
  vil3d_image_resource_sptr img_res_sptr = reader. vil_3d_image_resource();

  if (img_res_sptr->get_view()->pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
    run_proc<unsigned char> (radius_det, radius_fname, img_res_sptr,
                              kernel_list, xml_file);
  } else if (img_res_sptr->get_view()->pixel_format() == VIL_PIXEL_FORMAT_UINT_16) {
    run_proc<unsigned short> (radius_det, radius_fname, img_res_sptr,
                              kernel_list, xml_file);
  } else if (img_res_sptr->get_view()->pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {
    run_proc<float> (radius_det, radius_fname, img_res_sptr,
                              kernel_list, xml_file);
  }
   
  main_element.x_write_close(xml_file);
}
