//: 
// \file    vol3d_radius_detection_example.cxx
// \brief   a example of testing radius of cylinder
// \author  Kongbin Kang
// \date    2006-05-29
//
#include <vbl/vbl_array_3d.h>
#include <vol3d/vol3d_radius_filter.h>
#include <vol3d/algo/vol3d_radius_detection.h>

#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui.h>
#include <biov/biov_examiner_tableau.h>
#include <biosim/biosim_deformed_torus.h>
#include <Inventor/nodes/SoGroup.h>
#include <VolumeViz/nodes/SoVolumeRender.h>
#include <VolumeViz/nodes/SoVolumeData.h>
#include <VolumeViz/nodes/SoVolumeRender.h>
#include <VolumeViz/nodes/SoVolumeRendering.h>
#include <VolumeViz/nodes/SoTransferFunction.h>
#include <vil3d/vil3d_new.h>

#include <bgui3d/bgui3d.h>


int main(int argc, char * argv[])
{
  

biosim_deformed_torus def_tor(80,80,80);

vbl_array_3d<double>vol = def_tor.volume();


vcl_string torus_txt_file = "C:\\torus_x_y_values.txt"; 
          vcl_ofstream fstream(torus_txt_file.c_str());

  for(int k = 0; k<vol.get_row3_count(); k++)
      {
    for(int i = 0; i <vol.get_row1_count(); i++)
        {
      for(int j = 0; j < vol.get_row2_count(); j++)
          {
          
fstream <<int( vol[i][j][k]/255) << " " ;
          }
      fstream << vcl_endl;
        }
fstream << "end of row " << k << vcl_endl;
    }
fstream.close();


vil3d_image_view<vxl_uint_16>img_3d(def_tor.dimx(),def_tor.dimy(),def_tor.dimz(),1);
vil3d_image_resource_sptr img_res = vil3d_new_image_resource(def_tor.dimx(),def_tor.dimy(),def_tor.dimz(),1,VIL_PIXEL_FORMAT_UINT_16);

for (unsigned int i = 0;i<def_tor.dimx();i++)
    {
for (unsigned int j = 0;j<def_tor.dimy();j++)
    {
for (unsigned int k = 0;k<def_tor.dimz();k++)
    {
    vxl_uint_16  &vx = img_3d(i,j,k,0);
vx = vol[i][j][k]; 
    }
    }
    }



bool success = img_res->put_view(img_3d);
  vol3d_radius_detection rd(2, 8);

//  vbl_array_3d<double> rads = rd.radius(img_res, 255, double(0));
  vbl_array_3d<double> rads = rd.radius(vol,255,double(0));

  vbl_array_3d<unsigned char> result_vol(80, 80, 80);

  vcl_string txt_file = "C:\\radius_labeling.txt"; 
          vcl_ofstream fstr(txt_file.c_str());

  for(int k = 0; k<rads.get_row3_count(); k++)
      {
    for(int i = 0; i <rads.get_row1_count(); i++)
        {
      for(int j = 0; j < rads.get_row2_count(); j++)
          {
          
fstr << rads[i][j][k] << " " ;
          }
      fstr<< vcl_endl;
        }
fstr << "end of row " << k << vcl_endl;
    }
fstr.close();

 vcl_string txt_file_2 = "C:\\radius_labeling_in_x_z_plane.txt"; 
          vcl_ofstream stream(txt_file_2.c_str());

  for(int j = 0; j < rads.get_row2_count(); j++)
      {
      for(int k = 0; k<rads.get_row3_count(); k++)
        {
      for(int i = 0; i <rads.get_row1_count(); i++)   
          {
          
stream << rads[i][j][k] << " " ;
          }
      stream << vcl_endl;
        }
stream << "end of row " << j << vcl_endl;
    }
stream.close();

  for(int i = 0; i < 80; i ++)
    for(int j = 0; j < 80; j++)
      for(int k = 0; k < 80; k ++)
        result_vol[i][j][k] = (unsigned char)rads[i][j][k]*40;
   
  const size_t blocksize = result_vol.size();

  SbVec3s dim = SbVec3s(80, 80, 80);
#if 0
  uint8_t * voxels = new uint8_t[blocksize];

  (void)memset(voxels, 0, blocksize);

  for(unsigned long i = 0; i < blocksize; i++)
    voxels[i] = result_vol.data_block()[i];
#endif

  vgui::init(argc, argv);

  // visualization
  bgui3d_init();
  // make scene contain camera and light
  SoGroup *root = new SoGroup;
  root->ref();
  SoVolumeRendering::init();
  // Add SoVolumeData to scene graph
  SoVolumeData * volumedata = new SoVolumeData();

  volumedata->setVolumeData(dim, result_vol.data_block(), SoVolumeData::UNSIGNED_BYTE);

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

  return 0;
}
