//: 
// \file    det_edge_nonmax_supp_example.cxx
// \brief  
// \author  Can Aras (can@lems.brown.edu)
// \date    2006-01-26
// 
#include <vcl_fstream.h>
#include <vcl_cassert.h>
#include <vcl_cstdio.h>

#include <xmvg/xmvg_composite_filter_descriptor.h>
#include <det/det_cylinder_map.h>
#include <det/det_edge_nonmaximum_suppression.h>
#include <proc/io/proc_io_filter_xml_parser.h>
#include <vsol/vsol_cylinder.h>

#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#include <bgui3d/bgui3d.h>
#include <biov/biov_examiner_tableau.h>

#include <Inventor/nodes/SoGroup.h>
#include <VolumeViz/nodes/SoVolumeRender.h>
#include <VolumeViz/nodes/SoVolumeData.h>
#include <VolumeViz/nodes/SoVolumeRender.h>
#include <VolumeViz/nodes/SoVolumeRendering.h>
#include <VolumeViz/nodes/SoTransferFunction.h>

const double CYL_RADIUS = 0.75;
const double CYL_LENGTH = 0.1;

int main(int argc, char *argv[])
{
  if(argc < 6){
    vcl_cout << "Usage: "<< argv[0] << "filename_base xmargin ymargin zmargin out_file\n";
    return 1;
  }

  // initialize vgui
  // Ming: force option "--mfc-use-gl" to use gl
  //       so that it is MUCH faster if running on a
  //       computer with nice graphic card.
  //vgui::init(argc, argv);
  int my_argc = argc+1;
  char** my_argv = new char*[argc+1];
  for (int i=0; i<argc; i++)
    my_argv[i] = argv[i];
  my_argv[argc] = "--mfc-use-gl";
  vgui::init(my_argc, my_argv);
  delete []my_argv;

  // initialize bgui_3d
  bgui3d_init();

  int marginx = atoi(argv[2]);
  int marginy = atoi(argv[3]);
  int marginz = atoi(argv[4]);
  char* o_file = argv[5];
 
  double max_intensity = - 1e23;
  double min_intensity = 1e23;
  double intensity;

  // create the parser and read the responses
  vcl_string fbase = argv[1];
  // read x edge responses
  vcl_string fname = fbase + "_x_res.xml";
  proc_io_filter_xml_parser parser_x;
  if (!parse(fname, parser_x)) {
    vcl_cout << "Exitting!" << vcl_endl;
    return 1;
  }
  vcl_vector<xmvg_filter_response<double> > responses_x = parser_x.responses();
  // read y edge responses
  fname = fbase + "_y_res.xml";
  proc_io_filter_xml_parser parser_y;
  if (!parse(fname, parser_y)) {
    vcl_cout << "Exitting!" << vcl_endl;
    return 1;
  }
  vcl_vector<xmvg_filter_response<double> > responses_y = parser_y.responses();
  // read z edge responses
  fname = fbase + "_z_res.xml";
  proc_io_filter_xml_parser parser_z;
  if (!parse(fname, parser_z)) {
    vcl_cout << "Exitting!" << vcl_endl;
    return 1;
  }
  vcl_vector<xmvg_filter_response<double> > responses_z = parser_z.responses();

  int dimx = parser_x.dim_x();
  int dimy = parser_x.dim_y();
  int dimz = parser_x.dim_z();
  vcl_cout << "dimx: " << dimx << vcl_endl;
  vcl_cout << "dimy: " << dimy << vcl_endl;
  vcl_cout << "dimz: " << dimz << vcl_endl;

  det_edge_map cm(dimx, dimy, dimz, responses_x, responses_y, responses_z, 1.0);
  cm = cm.nonmaxium_suppression_for_edge_detection();

  double maximum_strength = 0;
  for(int k=0;k<dimz;k++)
  {
    for(int j=0;j<dimy;j++)
    {
      for(int i=0;i<dimx;i++)
      {
        if(i>=marginx && i<dimx-marginx && 
          j>=marginy && j<dimy-marginy &&
          k>=marginz && k<dimz-marginz )
        {
          intensity = cm[i][j][k].strength_;
          if(intensity > maximum_strength)
            maximum_strength = intensity;
        }
      }
    }
  }

  // for visualization below this point
  vcl_vector<double> field;
  for(int k=0;k<dimz;k++)
  {
    for(int j=0;j<dimy;j++)
    {
      for(int i=0;i<dimx;i++)
      {
        
        intensity = cm[i][j][k].strength_;
        if(intensity < maximum_strength/5)
          intensity = 0;

        if(i>=marginx && i<dimx-marginx && 
            j>=marginy && j<dimy-marginy &&
            k>=marginz && k<dimz-marginz )
        {
          field.push_back(intensity);

          if(max_intensity < intensity)
            max_intensity = intensity;
          if(min_intensity > intensity)
            min_intensity = intensity;
        }
      }
    }
  }

  // create cylinder and write to binary stream
  vcl_vector<vsol_cylinder_sptr> cylinders;
  vcl_vector<double> strengths;
  vsl_b_ofstream stream(o_file);

  int index = 0;
  for(int k=0;k<dimz;k++)
  {
    for(int j=0;j<dimy;j++)
    {
      for(int i=0;i<dimx;i++)
      {
        double x, y, z;
        if (cm[i][j][k].location_ != vgl_point_3d<double> (0.,0.,0.) && cm[i][j][k].strength_ > (max_intensity/5))
        {
          vcl_cout << cm[i][j][k].location_ << vcl_endl;
          vcl_cout << cm[i][j][k].strength_ << vcl_endl;

          x = i + cm[i][j][k].location_.x();
          y = j + cm[i][j][k].location_.y();
          z = k + cm[i][j][k].location_.z();

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
  //////////////////////////////////////////

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
