//: 
// \file  det_hair_nonmax_sup_example.cxx
// \brief  an example doing the nonmaxium suppression on filter response of hair
// \author    Kongbin Kang (kk at lems.brown.edu)
// \date        2005-10-07
// 
#include <vcl_fstream.h>
#include <vcl_cassert.h>
#include <vcl_cstdio.h>

#include <xmvg/xmvg_composite_filter_descriptor.h>
#include <det/det_cylinder_map.h>
#include <det/det_nonmaxium_suppression.h>
#include <proc/io/proc_io_filter_xml_parser.h>

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

int main(int argc, char *argv[])
{
  if(argc < 3){
    vcl_cout << "Usage: "<< argv[0] << " fname sigma\n";
    return 1;
  }

  int sigma = atoi(argv[2]);
  assert(sigma > 0 && sigma%2 != 0);
  
  vgui::init(argc, argv);

 
  double max_intensity = - 1e23;
  double min_intensity = 1e23;

  double intensity;
  
  vcl_ifstream file(argv[1]);

  if(!file){
    vcl_cout << "failed on opening file to read\n";
    return 2;
  }
  
  det_cylinder_map cm;

  file >> cm;

  det_nonmaxium_suppression sup(sigma);

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

       
        intensity = result_cm[i][j][k].strength_;
        if(intensity > 1000)
          vcl_cout <<"[ " << result_cm[i][j][k].location_.x() + i << ", " << result_cm[i][j][k].location_.y() + j << ", " << result_cm[i][j][k].location_.z() + k << "]";

   
          field.push_back(intensity);
          if(max_intensity < intensity)
            max_intensity = intensity;
          if(min_intensity > intensity)
            min_intensity = intensity;
     }

    }
       vcl_cout << "\n";

  }


  vcl_cout << result_cm;

  const size_t blocksize = field.size();
  uint8_t * voxels = new uint8_t[blocksize];

  (void)memset(voxels, 0, blocksize);

  for(unsigned long i = 0; i < blocksize; i++)
    voxels[i] = static_cast<uint8_t>((field[i] - min_intensity)*255/(max_intensity - min_intensity));

  SbVec3s dim = SbVec3s(dimx, dimy, dimz);

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
