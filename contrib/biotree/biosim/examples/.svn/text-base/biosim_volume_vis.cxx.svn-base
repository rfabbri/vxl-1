#include <biosim_cylinder_pyramid.h>
#include <vsol/vsol_cylinder.h>
#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui.h>
#include <biov/biov_examiner_tableau.h>
#include <vcl_limits.h>

#include <Inventor/nodes/SoGroup.h>
#include <VolumeViz/nodes/SoVolumeRender.h>
#include <VolumeViz/nodes/SoVolumeData.h>
#include <VolumeViz/nodes/SoVolumeRender.h>
#include <VolumeViz/nodes/SoVolumeRendering.h>
#include <VolumeViz/nodes/SoTransferFunction.h>

#include <bgui3d/bgui3d.h>

biov_examiner_tableau_new setup_biov_examiner_tableau(SbVec3s dim,uint8_t * voxels)
    {


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
  // root->unref();
  return tab3d;
    }

int main (int argc,char *argv[])
    {
    int my_argc = argc+1;
    char ** my_argv = new char*[argc+1];
    for (int i = 0;i<argc;i++)
        my_argv[i] = argv[i];
    my_argv[argc] = "--mfc-use-gl";
    vgui::init(my_argc,my_argv);
    delete []my_argv;

    bgui3d_init();

    double density = 10,gaussian_sigma = 2;

    biosim_cylinder_pyramid cyl_pyr(density,gaussian_sigma);

    vbl_array_3d<double>vol = cyl_pyr.volume();

    vcl_vector<double>volume_dat;

    double min_val = vcl_numeric_limits<float >::max();
    double max_val = vcl_numeric_limits<float >::min();
    double avg;

  for(int k=0;k<80;k++)
  {
    for(int j=0;j<80;j++)
    {
      for(int i=0;i<80;i++)
      {
        volume_dat.push_back(vol[i][j][k]);
        if (min_val>vol[i][j][k])
    min_val = vol[i][j][k];
    if (max_val<vol[i][j][k])
max_val = vol[i][j][k];
      }
      }
    }

   avg = (min_val + max_val)/2;

  const size_t blocksize = volume_dat.size();
  uint8_t * voxels = new uint8_t[blocksize];

  (void)memset(voxels, 0, blocksize);

  for(unsigned long i = 0; i < blocksize; i++)
    voxels[i] = static_cast<uint8_t>(volume_dat[i] + 128 - avg);
  

//  outfile.flush();
 

SbVec3s dim = SbVec3s(80,80,80);

  biov_examiner_tableau_new tab3d = setup_biov_examiner_tableau(dim,voxels);

  vgui_shell_tableau_new shell(tab3d);
  int return_value = vgui::run(shell, 400, 400);
  delete [] voxels;
  return return_value;

    }
        
