//: 
// \file  vol3d_radius_detection_example.cxx
// \brief  a example of testing radius of cylinder
// \author    Kongbin Kang
// \date        2006-05-29
//
#include <vcl_fstream.h>
#include <vbl/vbl_array_3d.h>
#include <vol3d/vol3d_radius_filter.h>
#include <vol3d/algo/vol3d_radius_detection.h>
#include <vol3d/vol3d_gaussian_kernel_3d.h>
#include <det/det_cylinder_map.h>
#include <det/det_nonmaxium_suppression.h>
#include <vsol/vsol_cylinder.h>
#include <biosim/biosim_cylinder_pyramid.h>

#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui.h>
#include <biov/biov_examiner_tableau.h>

#include <Inventor/nodes/SoGroup.h>
#include <VolumeViz/nodes/SoVolumeRender.h>
#include <VolumeViz/nodes/SoVolumeData.h>
#include <VolumeViz/nodes/SoVolumeRender.h>
#include <VolumeViz/nodes/SoVolumeRendering.h>
#include <VolumeViz/nodes/SoTransferFunction.h>

#include <bgui3d/bgui3d.h>


int main(int argc, char * argv[])
{

  if(argc != 2)
  {
    vcl_cout << "usage: "<< argv[0] << " outfile\n";
    exit(1);
  }


  double const density = 10, sigma = 1;
  
  vol3d_radius_detection rd(2, 8);

  vcl_valarray<int> probes = rd.probes();

  // need to be deleted
  vcl_vector<vol3d_gaussian_kernel_3d*> filters(probes.size());

  biosim_cylinder_pyramid pyramid(density, sigma);
  
  vbl_array_3d<double> vol = pyramid.volume();

  vbl_array_3d<double> rads = rd.radius(vol, density, sigma);

  vcl_ofstream outfile("temp");

  for(int i =0; i < 80; i++)
  {
    for(int j =0; j < 80; j++)
      outfile << rads[i][j][58]<<" ";
    outfile <<"\n";
  }
  
  outfile.flush();
  
  vbl_array_3d<unsigned char> result_vol(80, 80, 80);

  double min_val = 1e23, max_val = -1e23;
  det_cylinder_map cm(80, 80, 80);

  for(int i = 20; i < 60; i ++)
    for(int j = 20; j < 60; j++)
      for(int k = 20; k < 60; k ++)
      {
        double sigma_r = rads[i][j][k] / 1.414;
     
        if(sigma_r > 0){
          vgl_vector_3d<double> dir(0, 0, 1.);

          vol3d_gaussian_kernel_3d filter((int)4*sigma_r, 1, sigma_r, 
              2, dir);

          int w = (filter.dim()-1)/2;

          double response = 0;

          for(int l = -w; l <= w; l++)
            for(int m = -w; m <= w; m++)
              for(int n = -w; n <= w; n++)
                response += filter.get(l, m, n) * vol[i+l][j+m][k+n];


          cm[i][j][k].strength_ = response;
          cm[i][j][k].radius_ = rads[i][j][k];
          cm[i][j][k].dir_ = vgl_vector_3d<double>(0, 0, 1.);  

       }
          
      }

  
  det_nonmaxium_suppression sup(3);
  
  det_cylinder_map result_cm = sup.apply(cm, 0);

  int dimx = result_cm.nx();
  int dimy = result_cm.ny();
  int dimz = result_cm.nz();

  vcl_vector<double> field;
  for(int k=0;k<dimz;k++)
  {
    for(int j=0;j<dimy;j++)
    {
      for(int i=0;i<dimx;i++)
      {

       
        double intensity = result_cm[i][j][k].strength_;


        field.push_back(intensity);
        if(max_val < intensity)
          max_val = intensity;
        if(min_val > intensity)
          min_val = intensity;
      }

    }
  }

  // create cylinder and write to binary stream
  vcl_vector<vsol_cylinder_sptr> cylinders;
  vcl_vector<double> strengths;
  vsl_b_ofstream stream(argv[1]);
  
  for(int k=0;k<dimz;k++)
  {
    for(int j=0;j<dimy;j++)
    {
      for(int i=0;i<dimx;i++)
      {
        double x, y, z;
        if (result_cm[i][j][k].location_ != vgl_point_3d<double> (0.,0.,0.)){
        
            x = i + result_cm[i][j][k].location_.x();
            y = j + result_cm[i][j][k].location_.y();
            z = k + result_cm[i][j][k].location_.z();

            double radius = result_cm[i][j][k].radius_;
            
            // set center, radius, length and orientation
            vsol_cylinder_sptr cyl = new vsol_cylinder(vgl_point_3d<double> (x,y,z), radius, 1);
            cyl->set_orientation(vgl_vector_3d<double> (result_cm[i][j][k].dir_));
            cylinders.push_back(cyl);
            strengths.push_back(result_cm[i][j][k].strength_);
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


  //vcl_cout << result_cm;

  const size_t blocksize = field.size();
  uint8_t * voxels = new uint8_t[blocksize];

  (void)memset(voxels, 0, blocksize);

  for(unsigned long i = 0; i < blocksize; i++)
    voxels[i] = static_cast<uint8_t>((field[i] - min_val)*255/(max_val - min_val));

  SbVec3s dim = SbVec3s(dimx, dimy, dimz);

  // vgui init
  vgui::init(argc, argv);
 
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
