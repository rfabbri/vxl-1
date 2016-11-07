//: 
// \file   bioproc_response_vis.cxx
// \brief  visualizer for the responses (command line version)
// \author H. Can Aras and Kongbin Kang
// \date   August 2005
// 

#include <imgr/file_formats/imgr_skyscan_log.h>
#include <vil/vil_stream_fstream.h>
#if VXL_WIN32 
#include <vil/vil_stream_fstream_64.h>
#endif
#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#include <bgui3d/bgui3d.h>
#include <biov/biov_examiner_tableau.h>
#include <proc/io/proc_io_filter_xml_parser.h>

#include <Inventor/nodes/SoGroup.h>
#include <VolumeViz/nodes/SoVolumeRender.h>
#include <VolumeViz/nodes/SoVolumeData.h>
#include <VolumeViz/nodes/SoVolumeRender.h>
#include <VolumeViz/nodes/SoVolumeRendering.h>
#include <VolumeViz/nodes/SoTransferFunction.h>
#include <Inventor/nodes/SoCylinder.h>

int main(int argc, char** argv)
{
  if(argc < 5){
    vcl_cout << "Usage: "<< argv[0] << "fname xmargin ymargin zmargin filter_num";
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
  int filter_num = atoi(argv[5]);

  double max_intensity = - 1e23;
  double min_intensity = 1e23;

  int dimx,dimy,dimz;
  double intensity;

  // create the parser and read the responses
  proc_io_filter_xml_parser parser;
  vcl_string fname = argv[1];
  if (!parse(fname, parser)) {
    vcl_cout << "Exitting!" << vcl_endl;
    return 1;
  }
  dimx = parser.dim_x();
  dimy = parser.dim_y();
  dimz = parser.dim_z();
  int filters_size = parser.filter_num();
  assert (filter_num <= filters_size);

  vcl_vector<xmvg_filter_response<double> > responses = parser.responses();
  assert(dimx > 2*marginx && dimy > 2*marginy && dimz > 2*marginz);

  vcl_vector<double> field;
          
  int index=0;
  for(int k=0;k<dimz;k++) {
    for(int j=0;j<dimy;j++) {
      for(int i=0;i<dimx;i++) {
        intensity = responses[index++][filter_num-1];
        if(i>=marginx && i<dimx-marginx &&  j>=marginy && j<dimy-marginy && k>=marginz && k<dimz-marginz ){
          if(max_intensity < intensity)
            max_intensity = intensity;
          if(min_intensity > intensity)
            min_intensity = intensity;
          field.push_back(intensity);
        }
      }
    }
  }

  const size_t blocksize = field.size();
  uint8_t * voxels = new uint8_t[blocksize];

  (void)memset(voxels, 0, blocksize);

  vcl_cout << field.size() << vcl_endl;
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
