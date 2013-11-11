//: 
// \file   bioproc_response_vis_dialog_box.cxx
// \brief  visualizer for the responses (dialog box version)
// \author H. Can Aras and Kongbin Kang
// \date   08.12.2005
// 

#include <vil/vil_stream_fstream.h>
#if VXL_WIN32 
#include <vil/vil_stream_fstream_64.h>
#endif
#include <imgr/file_formats/imgr_skyscan_log.h>

#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_dialog.h>

#include <bgui3d/bgui3d.h>
#include <bgui3d/bgui3d_examiner_tableau.h>

#include <Inventor/nodes/SoGroup.h>
#include <VolumeViz/nodes/SoVolumeRender.h>
#include <VolumeViz/nodes/SoVolumeData.h>
#include <VolumeViz/nodes/SoVolumeRender.h>
#include <VolumeViz/nodes/SoVolumeRendering.h>
#include <VolumeViz/nodes/SoTransferFunction.h>

int main(int argc, char** argv)
{
  // initialize vgui
  vgui::init(argc, argv);

  vgui_dialog dlg("Specify responses file");
  dlg.set_ok_button("OK");
  dlg.set_cancel_button("CANCEL");
  static vcl_string inputf = "F:/MyDocs/Temp/responses.txt";
  static vcl_string marginx_str = "0";
  static vcl_string marginy_str = "0";
  static vcl_string marginz_str = "0";
  static vcl_string ext = "*.*";
  dlg.file("Input Filename:", ext, inputf);
  dlg.file("Margin-X:", ext, marginx_str);
  dlg.file("Margin-Y:", ext, marginy_str);
  dlg.file("Margin-Z:", ext, marginz_str);

  if (!dlg.ask())
    return 0;
  else
  {
    int marginx = atoi(marginx_str.c_str());
    int marginy = atoi(marginy_str.c_str());
    int marginz = atoi(marginz_str.c_str());

    double max_intensity = - 1e23;
    double min_intensity = 1e23;

    int dimx,dimy,dimz;
    double intensity;
    FILE *fp;
    fp = vcl_fopen(inputf.c_str(),"r");
    vcl_fscanf(fp,"%d %d %d\n", &dimx, &dimy, &dimz);

    assert(dimx > 2*marginx && dimy > 2*marginy && dimz > 2*marginz);

    vcl_vector<double> field;

    for(int k=0;k<dimz;k++)
    {
      for(int j=0;j<dimy;j++)
      {
        for(int i=0;i<dimx;i++)
        {
          vcl_fscanf(fp,"%lf ", &intensity);
          if(i>=marginx && i<dimx-marginx && j>=marginy && j<dimy-marginy && k>=marginz && k<dimz-marginz )
            field.push_back(intensity);

          if(max_intensity < intensity)
            max_intensity = intensity;
          if(min_intensity > intensity)
            min_intensity = intensity;
        }
        vcl_fscanf(fp,"\n");
      }
      vcl_fscanf(fp,"\n");
    }
    vcl_fclose(fp);


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
    root->addChild(transfunc);
    // Add VolumeRender to scene graph
    SoVolumeRender * volrend = new SoVolumeRender();
    root->addChild(volrend);
    bgui3d_examiner_tableau_new tab3d(root);
    root->unref();
    vgui_shell_tableau_new shell(tab3d);
    int return_value = vgui::run(shell, 400, 400);
    delete [] voxels;
    return return_value;
  }
}
