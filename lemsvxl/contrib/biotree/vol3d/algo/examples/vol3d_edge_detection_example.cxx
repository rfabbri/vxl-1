// This is /contrib/biotree/vol3d/vol3d_edge_detection_example.cxx

//: 
// \file    vol3d_edge_detection_example.cxx
// \brief   
//          
// \author  H. Can Aras
// \date    June 01, 2006
// \verbatim
\
//    

#include <ctime>
#include <vector>
#include <vgl/xio/vgl_xio_box_3d.h>
#include <vgl/xio/vgl_xio_vector_3d.h>
#include <vil3d/vil3d_image_resource.h>
#include <vil3d/vil3d_load.h>


#include <vol3d/algo/vol3d_gaussian_filtering_proc.h>
#include <proc/io/proc_io_run_xml_parser.h>
#include <imgr/file_formats/imgr_skyscan_log.h>
#include <imgr/file_formats/imgr_skyscan_reconlog.h>
#include <xscan/xscan_scan.h>
#include <xscan/xscan_dummy_scan.h>
#include <vsl/vsl_basic_xml_element.h>

#include <det/det_edge_map.h>
#include <vsol/vsol_cylinder.h>

const double CYL_RADIUS = 0.75;
const double CYL_LENGTH = 0.1;
double SIGMA = 1.4114;
const int KERNEL_SIZE = 15;
double gauss_x[KERNEL_SIZE][KERNEL_SIZE][KERNEL_SIZE];
double gauss_y[KERNEL_SIZE][KERNEL_SIZE][KERNEL_SIZE];
double gauss_z[KERNEL_SIZE][KERNEL_SIZE][KERNEL_SIZE];
double ***grad_x;
double ***grad_y;
double ***grad_z;
unsigned short ***vol;

int file_check(std::string logfile, std::string scanfile, std::string boxfile)
{
  //file extension check
    std::size_t dot_pos = logfile.find_first_of(".");
    if(std::strcmp(logfile.substr(dot_pos+1, 3).data(), "log") != 0 && std::strcmp(logfile.substr(dot_pos+1, 3).data(), "LOG") != 0)
    {
      std::cout << "***********************************************" << std::endl;
      std::cout << "The specified log file extension is not correct" << std::endl;
      std::cout << "***********************************************" << std::endl;
      return 1;
    }
    dot_pos = scanfile.find_first_of(".");
    if(std::strcmp(scanfile.substr(dot_pos+1, 3).data(), "scn") != 0 && std::strcmp(scanfile.substr(dot_pos+1, 3).data(), "SCN") != 0)
    {
      std::cout << "************************************************" << std::endl;
      std::cout << "The specified scan file extension is not correct" << std::endl;
      std::cout << "************************************************" << std::endl;
      return 1;
    }
    dot_pos = boxfile.find_first_of(".");
    if(std::strcmp(boxfile.substr(dot_pos+1, 3).data(), "bx3") != 0 && std::strcmp(boxfile.substr(dot_pos+1, 3).data(), "BX3") != 0)
    {
      std::cout << "***********************************************" << std::endl;
      std::cout << "The specified box file extension is not correct" << std::endl;
      std::cout << "***********************************************" << std::endl;
      return 1;
    }
    return 1;
}

void create_gaussian_gradient_kernels()
{
  int offset = (KERNEL_SIZE-1) / 2;
  for(int x = -offset; x <= offset; x++)
  {
    for(int y = -offset; y <= offset; y++)
    {
      for(int z = -offset; z <= offset; z++)
      {
        gauss_x[x+offset][y+offset][z+offset] = (x/(std::pow(SIGMA,2.0)))
          *std::exp(-(std::pow(x,2.0)+std::pow(y,2.0)+std::pow(z,2.0))/(2*std::pow(SIGMA,2.0)))
          /(std::pow(std::sqrt(2*vnl_math::pi)*SIGMA,3.0));
        gauss_y[x+offset][y+offset][z+offset] = (y/(std::pow(SIGMA,2.0)))
          *std::exp(-(std::pow(x,2.0)+std::pow(y,2.0)+std::pow(z,2.0))/(2*std::pow(SIGMA,2.0)))
          /(std::pow(std::sqrt(2*vnl_math::pi)*SIGMA,3.0));
        gauss_z[x+offset][y+offset][z+offset] = (z/(std::pow(SIGMA,2.0)))
          *std::exp(-(std::pow(x,2.0)+std::pow(y,2.0)+std::pow(z,2.0))/(2*std::pow(SIGMA,2.0)))
          /(std::pow(std::sqrt(2*vnl_math::pi)*SIGMA,3.0));
      }
    }
  }
}

void convolve_volume_with_gaussian(int dimx, int dimy, int dimz)
{
  int offset = (KERNEL_SIZE-1) / 2;
  // convolve the image with the Gaussian edge detector kernels
  for(int z = 0; z < dimz; z++)
  {
    std::cout << "processing slice " << z+1 << " of " << dimz << " slices for gradient" << std::endl;
    for(int y = 0; y < dimy; y++)
    {
      for(int x = 0; x < dimx; x++)
      {
        double val_x = 0;
        double val_y = 0;
        double val_z = 0;
        for(int k = -offset; k <= offset; k++)
        {
          for(int j = -offset; j <= offset; j++)
          {
            for(int i = -offset; i <= offset; i++)
            {
              if(x+i >=0 && x+i < dimx && y+j >= 0 && y+j < dimy && z+k >= 0 && z+k < dimz)
              {
                val_x = val_x + vol[x+i][y+j][z+k] * gauss_x[(KERNEL_SIZE-1)-(i+offset)][(KERNEL_SIZE-1)-(j+offset)][(KERNEL_SIZE-1)-(k+offset)];
                val_y = val_y + vol[x+i][y+j][z+k] * gauss_y[(KERNEL_SIZE-1)-(i+offset)][(KERNEL_SIZE-1)-(j+offset)][(KERNEL_SIZE-1)-(k+offset)];
                val_z = val_z + vol[x+i][y+j][z+k] * gauss_z[(KERNEL_SIZE-1)-(i+offset)][(KERNEL_SIZE-1)-(j+offset)][(KERNEL_SIZE-1)-(k+offset)];
              }
            }
          }
        }
        grad_x[x][y][z] = val_x;
        grad_y[x][y][z] = val_y;
        grad_z[x][y][z] = val_z;
      }
    }
  }
}

void save_result_as_binary(det_edge_map cm, int dimx, int dimy, int dimz, const char *o_file)
{
  int offset = (KERNEL_SIZE-1) / 2;
  double max_intensity = -10000000000.0;
  double min_intensity = 10000000000.0;
  double intensity;
  for(int k=offset;k<dimz-offset;k++)
  {
    for(int j=offset;j<dimy-offset;j++)
    {
      for(int i=offset;i<dimx-offset;i++)
      {
        intensity = cm[i][j][k].strength_;
        if(max_intensity < intensity)
          max_intensity = intensity;
        if(min_intensity > intensity)
          min_intensity = intensity;
      }
    }
  }
  // create cylinder and write to binary stream
  std::vector<vsol_cylinder_sptr> cylinders;
  std::vector<double> strengths;
  vsl_b_ofstream stream(o_file);

  int index = 0;
  for(int k=offset;k<dimz-offset;k++)
  {
    for(int j=offset;j<dimy-offset;j++)
    {
      for(int i=offset;i<dimx-offset;i++)
      {
        double x, y, z;
        if (cm[i][j][k].location_ != vgl_point_3d<double> (0.,0.,0.) && cm[i][j][k].strength_ > (max_intensity/3))
        {
          std::cout << cm[i][j][k].location_ << std::endl;
          std::cout << cm[i][j][k].strength_ << std::endl;

          x = i + cm[i][j][k].location_.x() - offset;
          y = j + cm[i][j][k].location_.y() - offset;
          z = k + cm[i][j][k].location_.z() - offset;

          // set center, radius, length and orientation
          vsol_cylinder_sptr cyl = new vsol_cylinder(vgl_point_3d<double> (x,y,z), CYL_RADIUS, CYL_LENGTH);
          vgl_vector_3d<double> direction (cm[i][j][k].dir_);
          //          vgl_vector_3d<double> direction (0.0, 0.0, 1.0);
          normalize(direction);
          cyl->set_orientation(direction);
          cylinders.push_back(cyl);
          strengths.push_back(cm[i][j][k].strength_);
        }
      }
    }
  }
  // write the version number
  vsl_b_write(stream, (int) 1);

  // write the number of cylinders
  vsl_b_write(stream, (int) cylinders.size());

  for (unsigned int i=0; i<cylinders.size(); i++){
    vsol_cylinder_sptr cyl = cylinders[i];
    // first write the strength
    vsl_b_write(stream, (double) strengths[i]);
    cyl->b_write(stream);
  }
  stream.close();
}

int main(int argc, char** argv)
{
  clock_t start, end;
  double elapsed;
  start = std::clock();

  proc_io_run_xml_parser parser;
  std::FILE *xmlFile;
  std::string fname="";
  std::string recon_fname = "";
  std::string out_fname = "";

  // Parse arguments
  for (int i = 1; i < argc; i++) {
    std::string arg (argv[i]);
    if (arg == std::string ("-x")) { fname = std::string(argv[++i]);}

    // this argument can be a volume data file like *.gipl or a path with wild characters to the 
    // reconstructed images like: C:\\test_images\\filters\\newcast35um2_orig\\scan35um_rec####.bmp
    else if (arg == std::string ("-b")) {recon_fname = std::string(argv[++i]);}
    // output bin file name
    else if (arg == std::string ("-o")) {out_fname = std::string(argv[++i]);}
    else
    {
      std::cout << "Usage: " << argv[0] << "[-x xml_script] " << std::endl;
      throw -1;
    }
  }

  if (  fname == ""){
    std::cout << "XML File not specified" << std::endl; 
    return(1);
  }

  if (  recon_fname == ""){
    std::cout << "3D Data File (Path) not specified" << std::endl; 
    return(1);
  }

  if (  out_fname == ""){
    std::cout << "Output File not specified" << std::endl; 
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

  // get the parameters from parser
  static std::string logfile = parser.log();
  static std::string scanfile = parser.scan();
  static std::string boxfile = parser.box();

  if (file_check(logfile, scanfile, boxfile) == 0)
    return 1;

  imgr_skyscan_log log(logfile.data());
  xscan_scan scan = log.get_scan();
  std::cout << "SCAN BEFORE\n" << scan << std::endl;

  std::ifstream scan_file(scanfile.c_str());
  scan_file >> scan;
  scan_file.close();

  std::cout << "SCAN AFTER\n" << scan << std::endl;

  log.set_scan(scan);

  //get the box
  std::ifstream box_file(boxfile.c_str());
  vgl_box_3d<double> box;
  box.read(box_file);
  box_file.close();
  std::cout << "BOX\n" << box << std::endl;

#if 0 //for debugging purposes (added by CAN)
  // increase box dimensions
  box.set_width(box.width()*5);
  box.set_height(box.height()*5);
  box.set_depth(box.depth()*2);
#endif

  std::cout << "BOX\n" << box << std::endl;

  imgr_skyscan_reconlog header(logfile.data(), scan);
  vgl_point_3d<double> box_min = header.bsc_to_fbpc(box.min_point());
  vgl_point_3d<double> box_max = header.bsc_to_fbpc(box.max_point());
  // adjust the box index, based on the starting section
  box_min.set(box_min.x(), box_min.y(), box_min.z() - header.start_slice_);
  box_max.set(box_max.x(), box_max.y(), box_max.z() - header.start_slice_);

  // if the reconstructed volume coordinates cannot be obtained successfully from the scan and box
  // files, the user can manually find these values by looking at the view images, and use them
  // by hard-coding here
#if 0 
  // hard-coding the recon coordinates of the close-by hairs here
  box_min.set(1107, 877, 424 - header.start_slice_);
  box_max.set(1197, 921, 471 - header.start_slice_);
#endif

  vgl_box_3d<double> recon_box(box_min, box_max);

  std::cout << recon_box << std::endl;

  vil3d_image_resource_sptr img_res_sptr = vil3d_load_image_resource(recon_fname.c_str()); 
  std::cout << "ni=" << img_res_sptr->ni() << " nj=" << img_res_sptr->nj() << 
    " nk=" << img_res_sptr->nk() << std::endl;

  // round the min and max positions
  int minx, miny, minz, maxx, maxy, maxz;
  if(box_min.x() - std::floor(box_min.x()) >= 0.5)
    minx = int(std::ceil(box_min.x()));
  else
    minx = int(std::floor(box_min.x()));

  if(box_max.x() - std::floor(box_max.x()) >= 0.5)
    maxx = int(std::ceil(box_max.x()));
  else
    maxx = int(std::floor(box_max.x()));

  if(box_min.y() - std::floor(box_min.y()) >= 0.5)
    miny = int(std::ceil(box_min.y()));
  else
    miny = int(std::floor(box_min.y()));

  if(box_max.y() - std::floor(box_max.y()) >= 0.5)
    maxy = int(std::ceil(box_max.y()));
  else
    maxy = int(std::floor(box_max.y()));

  if(box_min.z() - std::floor(box_min.z()) >= 0.5)
    minz = int(std::ceil(box_min.z()));
  else
    minz = int(std::floor(box_min.z()));

  if(box_max.z() - std::floor(box_max.z()) >= 0.5)
    maxz = int(std::ceil(box_max.z()));
  else
    maxz = int(std::floor(box_max.z()));

  vgl_point_3d<double> p1 = header.fbpc_to_bsc(vgl_point_3d<double> (minx, miny, minz+header.start_slice_));
  std::cout << p1 << std::endl;

  // boundaries are extended to get correct gradients at the boundaries too
  int offset = (KERNEL_SIZE-1) / 2;
  int dimx = maxx - minx + 1 + 2*offset;
  int dimy = maxy - miny + 1 + 2*offset;
  int dimz = maxz - minz + 1 + 2*offset;

  // allocate space for the data structures
  vol = new unsigned short**[dimx];
  grad_x = new double**[dimx];
  grad_y = new double**[dimx];
  grad_z = new double**[dimx];
  for(int i=0;i<dimx;i++)
  {
    vol[i] = new unsigned short*[dimy];
    grad_x[i] = new double*[dimy];
    grad_y[i] = new double*[dimy];
    grad_z[i] = new double*[dimy];
  }
  for(int i=0;i<dimx;i++)
  {
    for(int j=0;j<dimy;j++)
    {
      vol[i][j] = new unsigned short[dimz];
      grad_x[i][j] = new double[dimz];
      grad_y[i][j] = new double[dimz];
      grad_z[i][j] = new double[dimz];
    }
  }

  // get the cropped 3D volume data
  vil3d_image_view_base_sptr view3d = (img_res_sptr->get_view());
  if (view3d->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
  {
    vil3d_image_view<unsigned char> view = *view3d;
    int indexz = 0;
    for(int k = minz - offset; k <= maxz + offset; k++)
    {
      int indexy = 0;
      for(int j = miny - offset; j <= maxy + offset; j++)
      {
        int indexx = 0;
        for(int i = minx - offset; i <= maxx + offset; i++)
        {
          vol[indexx][indexy][indexz] = (unsigned short)(view(i,j,k));
          indexx++;
        }
        indexy++;
      }
      indexz++;
    }
  }
  else if(view3d->pixel_format() == VIL_PIXEL_FORMAT_UINT_16)
  {
    vil3d_image_view<unsigned short> view = *view3d;
    for(int k = minz - offset; k <= maxz + offset; k++)
      for(int j = miny - offset; j <= maxy + offset; j++)
        for(int i = minx - offset; i <= maxx + offset; i++)
          vol[i][j][k] = view(i,j,k);
  }

  create_gaussian_gradient_kernels();
  std::cout << "Gaussian kernels created..." << std::endl;
  convolve_volume_with_gaussian(dimx, dimy, dimz);
  std::cout << "Gradient computations finished..." << std::endl;

  int roidimx = dimx - 2*offset;
  int roidimy = dimy - 2*offset;
  int roidimz = dimz - 2*offset;
  
#if 0
  for(int k=offset;k<dimz-offset;k++)
  {
    char buffer[1024];
    std::sprintf(buffer, "D:\\MyDocs\\Temp\\vis\\img%03d.pgm",k);
    FILE *fp = std::fopen(buffer, "w");
    std::fprintf(fp,"P2\n%d %d\n%d\n", roidimx, roidimy, 255);
    for(int j=offset; j<dimy-offset; j++)
    {
      for(int i=offset; i<dimx-offset; i++)
      {
//        double val = std::sqrt(std::pow(grad_x[i][j][k],2.0) + std::pow(grad_y[i][j][k],2.0) + std::pow(grad_z[i][j][k],2.0));
        double val = std::abs(vol[i][j][k]);
        std::fprintf(fp,"%d ", int(val));
      }
      std::fprintf(fp,"\n");
    }
    std::fclose(fp);
  }
#endif

#if 0
  FILE *fpx = std::fopen("D:\\MyDocs\\Temp\\vol_edge_x.txt", "w");
  FILE *fpy = std::fopen("D:\\MyDocs\\Temp\\vol_edge_y.txt", "w");
  FILE *fpz = std::fopen("D:\\MyDocs\\Temp\\vol_edge_z.txt", "w");
  std::fprintf(fpx,"%d %d %d\n", roidimx, roidimy, roidimz);
  std::fprintf(fpy,"%d %d %d\n", roidimx, roidimy, roidimz);
  std::fprintf(fpz,"%d %d %d\n", roidimx, roidimy, roidimz);
  for(int k = offset; k < dimz-offset; k++)
  {
    for(int j = offset; j < dimy-offset; j++)
    {
      for(int i = offset; i < dimx-offset; i++)
      {
        std::fprintf(fpx,"%f ", grad_x[i][j][k]);
        std::fprintf(fpy,"%f ", grad_y[i][j][k]);
        std::fprintf(fpz,"%f ", grad_z[i][j][k]);
      }
      std::fprintf(fpx,"\n");
      std::fprintf(fpy,"\n");
      std::fprintf(fpz,"\n");
    }
    std::fprintf(fpx,"\n");
    std::fprintf(fpy,"\n");
    std::fprintf(fpz,"\n");
  }
  std::fclose(fpx);
  std::fclose(fpy);
  std::fclose(fpz);
#endif

#if 0
  FILE *fp = std::fopen("D:\\MyDocs\\Temp\\vol.txt", "w");
  std::fprintf(fp,"%d %d %d\n", roidimx, roidimy, roidimz);
  for(int k = offset; k < dimz-offset; k++)
  {
    for(int j = offset; j < dimy-offset; j++)
    {
      for(int i = offset; i < dimx-offset; i++)
      {
        std::fprintf(fp,"%d ", unsigned short(vol[i][j][k]));
      }
      std::fprintf(fp,"\n");
    }
    std::fprintf(fp,"\n");
  }
  std::fclose(fp);
#endif

  det_edge_map cm(dimx, dimy, dimz, grad_x, grad_y, grad_z);
  cm = cm.nonmaxium_suppression_for_edge_detection();
  save_result_as_binary(cm, dimx, dimy, dimz, out_fname.c_str());

  end = std::clock();
  elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;
  std::cout << "Elapsed time is " << elapsed << std::endl;
}
