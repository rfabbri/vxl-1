//: 
// \file  det_hair_nonmax_sup_from_response_example.cxx
// \brief  an example doing the nonmaxium suppression on filter response of hair
//         the difference of this example from det_hair_nonmax_sup_example.cxx
//         is, those one takes the filter response file detects the hair and do the
//         non maxima suppresion on it and displays the result
// \author    Kongbin Kang (kk at lems.brown.edu) & Gamze Tunali
// \date        2005-11-01
// 
#include <vcl_fstream.h>
#include <vcl_cassert.h>
#include <vcl_cstdio.h>
#include <vnl/vnl_math.h>
#include <vil3d/vil3d_image_resource.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_image_resource.h>
#include <vil3d/vil3d_load.h>
#include <vsol/vsol_cylinder.h>
#include <vsol/vsol_volume_3d.h>
#include <vsol/vsol_volume_3d_sptr.h>
#include <vsol/vsol_orient_box_3d.h>
#include <vgl/vgl_distance.h>
#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#include <bgui3d/bgui3d.h>
#include <biov/biov_examiner_tableau.h>

#include <xmvg/xmvg_composite_filter_descriptor.h>
#include <det/det_cylinder_map.h>
#include <det/det_cylinder_detect.h>
#include <det/det_nonmaxium_suppression.h>
#include <proc/io/proc_io_filter_xml_parser.h>

#include <biov/biov_examiner_tableau.h>
#include <Inventor/nodes/SoGroup.h>
#include <VolumeViz/nodes/SoVolumeRender.h>
#include <VolumeViz/nodes/SoVolumeData.h>
#include <VolumeViz/nodes/SoVolumeRender.h>
#include <VolumeViz/nodes/SoVolumeRendering.h>
#include <VolumeViz/nodes/SoTransferFunction.h>

const double CYL_RADIUS = 0.5;
const double CYL_LENGTH = 1.0;

template <class T>
vbl_array_3d<double> 
convert_to_vbl_array(vil3d_image_view_base_sptr view_sptr,
                     int min_x, int min_y, int min_z,
                     int max_x, int max_y, int max_z)
{
  int x = max_x - min_x;
  int y = max_y - min_y;
  int z = max_z - min_z;
  vbl_array_3d<double> volume(x, y, z);
  


  for (int k=0; k<z; k++) {
    for (int j=0; j<y; j++) {
      for (int i=0; i<x; i++) {  
        vil3d_image_view<T> view = *view_sptr;
        if ((i > 0) && (j>0) && (k>0) && 
            (i < static_cast<int> (view.ni())) && 
            (j < static_cast<int> (view.nj())) && 
            (k < static_cast<int> (view.nk())))  {
          T voxel_val = 0;
          voxel_val = view(i+min_x, j+min_y, k+min_z);
          double val = voxel_val;
          volume[i][j][k] = val;
        } else {
          volume[i][j][k] = 0.0;
        }
        
      }
    }
  }
  return volume;
}
int main(int argc, char *argv[])
{
  if(argc < 7){
    vcl_cout << "Usage: "<< argv[0] << " fname xmargin ymargin zmargin neighbour_size radius_fname out_file\n";
    return 1;
  }

  int marginx = atoi(argv[2]);
  int marginy = atoi(argv[3]);
  int marginz = atoi(argv[4]);
  int neighb_size = atoi(argv[5]);
  char* rad_file = argv[6];
  char* o_file = argv[7];
  
    int my_argc = argc+1;
  char** my_argv = new char*[argc+1];
  for (int i=0; i<argc; i++)
    my_argv[i] = argv[i];
  my_argv[argc] = "--mfc-use-gl";
  vgui::init(my_argc, my_argv);
  delete []my_argv;
 
  double max_intensity = - 1e23;
  double min_intensity = 1e23;
  double intensity;

  // create the parser and read the responses
  proc_io_filter_xml_parser parser;
  vcl_string fname = argv[1];
  if (!parse(fname, parser)) {
    vcl_cout << "Exitting!" << vcl_endl;
    return 1;
  }
  int dimx = parser.dim_x();
  int dimy = parser.dim_y();
  int dimz = parser.dim_z(); 
  
  vcl_vector<xmvg_filter_response<double> > responses = parser.responses();
  
  xmvg_composite_filter_descriptor fds = parser.composite_filter_descr();

  assert(dimx > 2*marginx && dimy > 2*marginy && dimz > 2*marginz);
  det_cylinder_detect detector;
  det_cylinder_map cm;

  vil3d_image_resource_sptr img_res_sptr = vil3d_load_image_resource(rad_file); 
  vcl_cout << "ni=" << img_res_sptr->ni() << " nj=" << img_res_sptr->nj() << 
    " nk=" << img_res_sptr->nk() << vcl_endl;
  vil3d_image_view_base_sptr view = img_res_sptr->get_view();
  if (view->pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
      vil3d_image_view<unsigned char> radius = *view;
    cm = detector.apply(dimx, dimy, dimz, responses, fds, &radius);
  } else {
    vcl_cerr << "Unsupported VIL format = " << view->pixel_format() << vcl_endl;
  }
  
  for(int k=0;k<dimz;k++)
    {
    //fs << "z=" << k << vcl_endl;
    for(int j=0;j<dimy;j++)
    {
      for(int i=0;i<dimx;i++)
      {
        double intensity = cm[i][j][k].strength_;
        //fs << intensity << " ";
      }
      //fs << vcl_endl;
    }
    //fs << vcl_endl;
  }
  
  det_nonmaxium_suppression nms(neighb_size);
  if (view->pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
     //vil3d_image_view<unsigned char> radius = *view;
     vbl_array_3d<double> radius_map = convert_to_vbl_array<unsigned char> (view, 0, 0, 0, 
                     view->ni(), view->nj(), view->nk());
     cm = nms.apply_with_radius(cm, radius_map);
  }
  //cm = nms.apply(cm);

  vcl_vector<double> field;
  for(int k=0;k<dimz;k++)
  {
    for(int j=0;j<dimy;j++)
    {
      for(int i=0;i<dimx;i++)
      {
        
        intensity = cm[i][j][k].strength_;

//        vcl_cout << "[" << cm[i][j][k].location_.x() << "," << cm[i][j][k].location_.y() << "," << cm[i][j][k].location_.z()<< "] ";
        if(i>=marginx && i<dimx-marginx && 
            j>=marginy && j<dimy-marginy &&
            k>=marginz && k<dimz-marginz ){       
          field.push_back(intensity);
          if(max_intensity < intensity)
            max_intensity = intensity;
          if(min_intensity > intensity)
            min_intensity = intensity;
        }
      }
        vcl_cout << "\n";
    }
    vcl_cout << "\n";
  }
  
  // create cylinder and write to binary stream
  vcl_vector<vsol_cylinder_sptr> cylinders;
  vcl_vector<double> strengths;
  vsl_b_ofstream stream(o_file);
  
  for(int k=0;k<dimz;k++)
  {
    for(int j=0;j<dimy;j++)
    {
      for(int i=0;i<dimx;i++)
      {
        double x, y, z;
        if (cm[i][j][k].location_ != vgl_point_3d<double> (0.,0.,0.)){// && (cm[i][j][k].strength_ > max_intensity/3)){
        
          x = i + cm[i][j][k].location_.x();
          y = j + cm[i][j][k].location_.y();
          z = k + cm[i][j][k].location_.z();
            
          // set center, radius, length and orientation
          double r = CYL_RADIUS;
          if (cm[i][j][k].radius_ > 0) 
          r = cm[i][j][k].radius_;
          vsol_cylinder_sptr cyl = new vsol_cylinder(vgl_point_3d<double> (x,y,z), r, CYL_LENGTH);
          cyl->set_orientation(vgl_vector_3d<double> (cm[i][j][k].dir_));
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
const size_t blocksize = field.size();
  uint8_t * voxels = new uint8_t[blocksize];

  (void)memset(voxels, 0, blocksize);

  for(unsigned long i = 0; i < blocksize; i++)
    voxels[i] = static_cast<uint8_t>((field[i] - min_intensity)*255/(max_intensity - min_intensity));

  SbVec3s dim = SbVec3s(dimx-2*marginx, dimy-2*marginy, dimz-2*marginz);

  // visualization
  bgui3d_init();
  // make scene contain camera and light
  SoGroup *root = new SoGroup;
  root->ref();
  SoVolumeRendering::init();
  // Add SoVolumeData to scene graph
  SoVolumeData * volumedata = new SoVolumeData();
  volumedata->setVolumeData(dim, voxels, SoVolumeData::UNSIGNED_BYTE);
  root->addChild(volumedata);
  
  // Add TransferFunction (color map) to scene graph
  SoTransferFunction * transfunc = new SoTransferFunction();
  transfunc->predefColorMap = SoTransferFunction::PHYSICS;
  root->addChild(transfunc);
  
  // Add VolumeRender to scene graph
  SoVolumeRender * volrend = new SoVolumeRender();
  root->addChild(volrend);
  biov_examiner_tableau_new tab3d(root, transfunc);
  root->unref();
  vgui_shell_tableau_new shell(tab3d);
  int return_value = vgui::run(shell, 400, 400);
  delete [] voxels;
  return return_value;

}
