//: 
// \file    det_edge_nonmax_supp_example_txt_version.cxx
// \brief   First argument: File name base for the response files along x, y, and z
//          e.g. the file names are [first argument]_[x or y or z].txt
//          Second/third/fourth arguments: Number of voxels to be cut from the boundaries for
//          x, y and z directions respectively
//          Fifth argument: Output file name base, the extension is decided based on the
//          last argument
//          Sixth argument: Can be "bin" or "txt". If "bin", vsol cylinders are written to a
//          bin file. If "txt", sub-voxel location of the edges together with the strength
//          values are written as a text file (for Jesse's benefit)
//
// \author  Can Aras (can@lems.brown.edu)
// \date    2006-01-26
// 
#include <vcl_fstream.h>
#include <vcl_cassert.h>
#include <vcl_cstdio.h>

#include <vsl/vsl_binary_io.h>

#include <det/det_edge_nonmaximum_suppression.h>
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

const double CYL_RADIUS = 0.6;
const double CYL_LENGTH = 0.1;

int main(int argc, char *argv[])
{
  if(argc < 7){
    vcl_cout << "Usage: "<< argv[0] << "filename_base xmargin ymargin zmargin out_file_base out_file_type\n";
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
  char* o_file_base = argv[5];
  char* o_file_type = argv[6];

  if((vcl_strcmp(o_file_type, "bin") != 0) && (vcl_strcmp(o_file_type, "txt") != 0))
  {
    vcl_cout << "Unrecognized output file extension" << vcl_endl;
    exit(-1);
  }
 
  double max_intensity = - 1e23;
  double min_intensity = 1e23;
  double intensity;

  int dimx1,dimy1,dimz1;
  int dimx2,dimy2,dimz2;
  int dimx3,dimy3,dimz3;

  vcl_string fbase = argv[1];
  vcl_string fnamex = fbase + "_x.txt";
  vcl_string fnamey = fbase + "_y.txt";
  vcl_string fnamez = fbase + "_z.txt";

  FILE *fpx = vcl_fopen(fnamex.c_str(), "r");
  FILE *fpy = vcl_fopen(fnamey.c_str(), "r");
  FILE *fpz = vcl_fopen(fnamez.c_str(), "r");

  vcl_fscanf(fpx, "%d %d %d\n", &dimx1, &dimy1, &dimz1);
  vcl_fscanf(fpy, "%d %d %d\n", &dimx2, &dimy2, &dimz2);
  vcl_fscanf(fpz, "%d %d %d\n", &dimx3, &dimy3, &dimz3);

  assert(dimx1==dimx2 && dimx2 == dimx3 && 
         dimy1==dimy2 && dimy2 == dimy3 && 
         dimz1==dimz2 && dimz2 == dimz3);

  vbl_array_3d<double> responses_x(dimx1, dimy1, dimz1);
  vbl_array_3d<double> responses_y(dimx1, dimy1, dimz1);
  vbl_array_3d<double> responses_z(dimx1, dimy1, dimz1);

  for(int k=0; k<dimz1; k++)
  {
    vcl_cout << k << vcl_endl;
    for(int j=0; j<dimy1; j++)
    {
      for(int i=0; i<dimx1; i++)
      {
        double resp;
        vcl_fscanf(fpx, "%lf ", &resp);
        responses_x(i, j, k) = resp;
        vcl_fscanf(fpy, "%lf ", &resp);
        responses_y(i, j, k) = resp;
        vcl_fscanf(fpz, "%lf ", &resp);
        responses_z(i, j, k) = resp;
      }
    }
  }
  vcl_fclose(fpx);
  vcl_fclose(fpy);
  vcl_fclose(fpz);

  det_edge_map cm(responses_x, responses_y, responses_z);
  cm = cm.nonmaxium_suppression_for_edge_detection();

  double maximum_strength = 0;
  for(int k=0;k<dimz1;k++)
  {
    for(int j=0;j<dimy1;j++)
    {
      for(int i=0;i<dimx1;i++)
      {
        if(i>=marginx && i<dimx1-marginx && 
          j>=marginy && j<dimy1-marginy &&
          k>=marginz && k<dimz1-marginz )
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
  for(int k=0;k<dimz1;k++)
  {
    for(int j=0;j<dimy1;j++)
    {
      for(int i=0;i<dimx1;i++)
      {
        
        intensity = cm[i][j][k].strength_;
        if(intensity < maximum_strength/5)
          intensity = 0;

        if(i>=marginx && i<dimx1-marginx && 
            j>=marginy && j<dimy1-marginy &&
            k>=marginz && k<dimz1-marginz )
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

  if(vcl_strcmp(o_file_type, "bin") == 0)
  {
    vcl_string o_file(o_file_base);
    o_file = o_file + ".bin";
    // create cylinder and write to binary stream
    vcl_vector<vsol_cylinder_sptr> cylinders;
    vcl_vector<double> strengths;
    vsl_b_ofstream stream(o_file.c_str());

    int index = 0;
    for(int k=0;k<dimz1;k++)
    {
      for(int j=0;j<dimy1;j++)
      {
        for(int i=0;i<dimx1;i++)
        {
          double x, y, z;
          if (cm[i][j][k].location_ != vgl_point_3d<double> (0.,0.,0.) && cm[i][j][k].strength_ > (max_intensity/3))
          {
            vcl_cout << cm[i][j][k].location_ << vcl_endl;
            vcl_cout << cm[i][j][k].strength_ << vcl_endl;

            x = i + cm[i][j][k].location_.x();
            y = j + cm[i][j][k].location_.y();
            z = k + cm[i][j][k].location_.z();
            // set center, radius, length and orientation
            vsol_cylinder_sptr cyl = new vsol_cylinder(vgl_point_3d<double> (x,y,z), CYL_RADIUS, CYL_LENGTH);
            vgl_vector_3d<double> direction (cm[i][j][k].dir_);
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
  else if(vcl_strcmp(o_file_type, "txt") == 0)
  {
    vcl_string o_file(o_file_base);
    o_file = o_file + ".txt";
    FILE *fp = vcl_fopen(o_file.c_str(), "w");

    int index = 0;
    for(int k=0;k<dimz1;k++)
    {
      for(int j=0;j<dimy1;j++)
      {
        for(int i=0;i<dimx1;i++)
        {
          double x, y, z;
          if (cm[i][j][k].location_ != vgl_point_3d<double> (0.,0.,0.) && cm[i][j][k].strength_ > (max_intensity/3))
          {
            vcl_cout << cm[i][j][k].location_ << vcl_endl;
            vcl_cout << cm[i][j][k].strength_ << vcl_endl;

            x = i + cm[i][j][k].location_.x();
            y = j + cm[i][j][k].location_.y();
            z = k + cm[i][j][k].location_.z();
            
            fprintf(fp, "%f %f %f %f\n", float(x), float(y), float(z), float(cm[i][j][k].strength_));
          }
        }
      }
    }
    vcl_fclose(fp);
  }
  //////////////////////////////////////////

  const size_t blocksize = field.size();
  uint8_t * voxels = new uint8_t[blocksize];

  (void)memset(voxels, 0, blocksize);

  for(unsigned long i = 0; i < blocksize; i++)
    voxels[i] = static_cast<uint8_t>((field[i] - min_intensity)*255/(max_intensity - min_intensity));

  SbVec3s dim = SbVec3s(dimx1-2*marginx, dimy1-2*marginy, dimz1-2*marginz);

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
