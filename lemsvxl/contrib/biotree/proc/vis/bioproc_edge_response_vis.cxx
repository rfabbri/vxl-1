//: 
// \file   bioproc_edge_response_vis.cxx
// \brief  visualizer for the edge responses
//         The user must have three response files resulting from x, y, and z detectors.
//         The file names must end with ..._x_res.xml, ..._y_res.xml and ..._z_res.xml 
//         and have the same common basename ...
//         the user must provide ... as the first argument.
// \author H. Can Aras
// \date   2006-02-06
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
    vcl_cout << "Usage: "<< argv[0] << "filename_base xmargin ymargin zmargin\n";
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

  assert(dimx > 2*marginx && dimy > 2*marginy && dimz > 2*marginz);

  double sharpening_coefficient = 1.0;
  // temporarily filled with x responses to create the response vector
  vcl_vector<xmvg_filter_response<double> > responses(responses_x);
  int index = 0;
  for(int k=0;k<dimz;k++)
  {
    for(int j=0;j<dimy;j++)
    {
      for(int i=0;i<dimx;i++)
      {
        double resp_x = responses_x[index][0] * sharpening_coefficient;
        double resp_y = responses_y[index][0] * sharpening_coefficient;
        double resp_z = responses_z[index][0];
//        responses[index][0] = vcl_fabs(resp_z);
        responses[index][0] = vcl_sqrt(vcl_pow(resp_x,2.0)+vcl_pow(resp_y,2.0)+vcl_pow(resp_z,2.0));
        index++;
      }
    }
  }

  //write responses to a file for debugging reasons
#if 0
  FILE *fp = vcl_fopen("F:\\MyDocs\\Temp\\responses.txt", "w");
  vcl_fprintf(fp, "%d %d %d\n", dimx, dimy, dimz);
  index = 0;
  for(int k=0;k<dimz;k++)
  {
    for(int j=0;j<dimy;j++)
    {
      for(int i=0;i<dimx;i++)
      {
        intensity = responses[index++][0];
        vcl_fprintf(fp, "%f ", intensity);
      }
      vcl_fprintf(fp,"\n");
    }
    vcl_fprintf(fp,"\n");
  }
  vcl_fclose(fp);
#endif
          
  index=0;
  for(int k=0;k<dimz;k++) {
    for(int j=0;j<dimy;j++) {
      for(int i=0;i<dimx;i++) {
        intensity = responses[index++][0];
        if(i>=marginx && i<dimx-marginx &&  j>=marginy && j<dimy-marginy && k>=marginz && k<dimz-marginz ){
          if(max_intensity < intensity)
            max_intensity = intensity;
          if(min_intensity > intensity)
            min_intensity = intensity;
        }
      }
    }
  }

  vcl_cout << max_intensity << vcl_endl;

  const size_t blocksize = responses.size();
  uint8_t * voxels = new uint8_t[blocksize];

  (void)memset(voxels, 0, blocksize);

  for(unsigned long i = 0; i < blocksize; i++)
    voxels[i] = static_cast<uint8_t>((responses[i][0] - min_intensity)*255/(max_intensity - min_intensity));

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
