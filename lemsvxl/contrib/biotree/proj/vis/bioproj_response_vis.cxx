//: 
// \file   bioproc_response_vis.cxx
// \brief  visualizer for the responses (command line version)
//         command line arguments are:
//         [single/combined] [full file name/file name base] marginx marginy marginz
// \author H. Can Aras
// \date   2006-07-20
// 


#include <resc/imgr/file_formats/imgr_skyscan_log.h>
#include <vil/vil_stream_fstream.h>
#if VXL_WIN32 
#include <vil/vil_stream_fstream_64.h>
#endif
#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#include <bgui3d/bgui3d.h>
#include <biov/biov_examiner_tableau.h>

#include <biov/biov_examiner_tableau.h>
#include <Inventor/nodes/SoGroup.h>
#include <VolumeViz/nodes/SoVolumeRender.h>
#include <VolumeViz/nodes/SoVolumeData.h>
#include <VolumeViz/nodes/SoVolumeRender.h>
#include <VolumeViz/nodes/SoVolumeRendering.h>
#include <VolumeViz/nodes/SoTransferFunction.h>

int main(int argc, char** argv)
{
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

  double max_intensity = - 1e23;
  double min_intensity = 1e23;

  int marginx = atoi(argv[3]);
  int marginy = atoi(argv[4]);
  int marginz = atoi(argv[5]);

  int dimx1,dimy1,dimz1;
  int dimx2,dimy2,dimz2;
  int dimx3,dimy3,dimz3;
  double intensity;
  double intensity1, intensity2, intensity3;

  vcl_vector<double> field;
  if(strcmp(argv[1], "combined") == 0)
  {
    vcl_string fnamebase = argv[2];
    vcl_string fname1 = fnamebase + "_x.txt";
    vcl_string fname2 = fnamebase + "_y.txt";
    vcl_string fname3 = fnamebase + "_z.txt";

    FILE *fp1 = vcl_fopen(fname1.c_str(),"r");
    FILE *fp2 = vcl_fopen(fname2.c_str(),"r");
    FILE *fp3 = vcl_fopen(fname3.c_str(),"r");
    vcl_fscanf(fp1, "%d %d %d", &dimx1, &dimy1, &dimz1);
    vcl_fscanf(fp2, "%d %d %d", &dimx2, &dimy2, &dimz2);
    vcl_fscanf(fp3, "%d %d %d", &dimx3, &dimy3, &dimz3);

    int index=0;
    for(int k=0;k<dimz1;k++) {
      vcl_cout << k << vcl_endl;
      for(int j=0;j<dimy1;j++) {
        for(int i=0;i<dimx1;i++) {
          vcl_fscanf(fp1,"%lf ", &intensity1);
          vcl_fscanf(fp2,"%lf ", &intensity2);
          vcl_fscanf(fp3,"%lf ", &intensity3);
          intensity = vcl_sqrt(vcl_pow(intensity1, 2.0) + vcl_pow(intensity2, 2.0) + vcl_pow(intensity3, 2.0));
          //        intensity = vcl_fabs(intensity3);
          if(i>=marginx && i<dimx1-marginx && j>=marginy && j<dimy1-marginy && k>=marginz && k<dimz1-marginz )
          {
            if(max_intensity < intensity)
              max_intensity = intensity;
            if(min_intensity > intensity)
              min_intensity = intensity;
            field.push_back(intensity);
          }
        }
      }
    }
    vcl_fclose(fp1);
    vcl_fclose(fp2);
    vcl_fclose(fp3);
  }
  else if(strcmp(argv[1],"single") == 0)
  {
    vcl_string fname = argv[2];

    FILE *fp = vcl_fopen(fname.c_str(),"r");
    vcl_fscanf(fp, "%d %d %d", &dimx1, &dimy1, &dimz1);

    int index=0;
    for(int k=0;k<dimz1;k++) {
      vcl_cout << k << vcl_endl;
      for(int j=0;j<dimy1;j++) {
        for(int i=0;i<dimx1;i++) {
          vcl_fscanf(fp,"%lf ", &intensity);
          intensity = vcl_fabs(intensity);
//          int ridge;
//          vcl_fscanf(fp,"%d ", &ridge);
//          if(ridge == 1)
//            intensity = double(ridge);
//          else
//            intensity = 0.0;
          if(i>=marginx && i<dimx1-marginx && j>=marginy && j<dimy1-marginy && k>=marginz && k<dimz1-marginz )
          {
            if(max_intensity < intensity)
              max_intensity = intensity;
            if(min_intensity > intensity)
              min_intensity = intensity;
            field.push_back(intensity);
          }
        }
      }
    }
    vcl_fclose(fp);
  }
  else
    vcl_cout << "First argument not recognized..." << vcl_endl;

  vcl_cout << "Max value: " << max_intensity << vcl_endl;

  const size_t blocksize = field.size();
  uint8_t * voxels = new uint8_t[blocksize];

  (void)memset(voxels, 0, blocksize);

  vcl_cout << field.size() << vcl_endl;
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
