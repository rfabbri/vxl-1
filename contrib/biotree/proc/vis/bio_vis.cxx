//: 
// \file   bio_vis.cxx
// \brief  visualizer for the filter response,edge response & splr response visualization
// 

#include <imgr/file_formats/imgr_skyscan_log.h>
#include <vil/vil_stream_fstream.h>
#if VXL_WIN32 
#include <vil/vil_stream_fstream_64.h>
#endif
#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_shell_tableau.h>
#include <bgui3d/bgui3d.h>
#include <biov/biov_examiner_tableau.h>
#include <proc/io/proc_io_filter_xml_parser.h>
#include <biob/biob_grid_worldpt_roster.h>
#include <biob/biob_roster_to_grid_mapping.h>
#include <vcl_sstream.h>
#include <geom/geom_index_structure.h>
#include <geom/geom_rectangular_probe_volume.h>
#include <vgl/vgl_distance.h>
#include "bio_vis.h"

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

uint8_t * read_ede_responses(vcl_string fbase,unsigned int marginx,unsigned int marginy,unsigned int marginz,
                        SbVec3s & dim)
    {
    double max_intensity = - 1e23;
  double min_intensity = 1e23;
  double intensity;

 vcl_string fname = fbase + "_x_res.xml";
  proc_io_filter_xml_parser parser_x;
  if (!parse(fname, parser_x)) {
    vcl_cout << "Exitting!" << vcl_endl;
  }
  vcl_vector<xmvg_filter_response<double> > responses_x = parser_x.responses();
  // read y edge responses
  fname = fbase + "_y_res.xml";
  proc_io_filter_xml_parser parser_y;
  if (!parse(fname, parser_y)) {
    vcl_cout << "Exitting!" << vcl_endl;
  }
  vcl_vector<xmvg_filter_response<double> > responses_y = parser_y.responses();
  // read z edge responses
  fname = fbase + "_z_res.xml";
  proc_io_filter_xml_parser parser_z;
  if (!parse(fname, parser_z)) {
    vcl_cout << "Exitting!" << vcl_endl;
  }
  vcl_vector<xmvg_filter_response<double> > responses_z = parser_z.responses();

  int dimx = parser_x.dim_x();
  int dimy = parser_x.dim_y();
  int dimz = parser_x.dim_z();
  vcl_cout << "dimx: " << dimx << vcl_endl;
  vcl_cout << "dimy: " << dimy << vcl_endl;
  vcl_cout << "dimz: " << dimz << vcl_endl;

  assert(dimx > 2*marginx && dimy > 2*marginy && dimz > 2*marginz);

  double sharpening_coefficient = 2.8;
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
//        responses[index][0] = vcl_fabs(resp_y);
        responses[index][0] = vcl_sqrt(vcl_pow(resp_x,2.0)+vcl_pow(resp_y,2.0)+vcl_pow(resp_z,2.0));
        index++;
      }
    }
  }
          
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

  dim = SbVec3s(dimx-2*marginx, dimy-2*marginy, dimz-2*marginz);

  return voxels;

    }



uint8_t *  read_filter_responses(vcl_string fname,unsigned int filter_num,unsigned int marginx,unsigned int marginy,
                           unsigned int marginz,SbVec3s & dim)
    {
                        
 double max_intensity = - 1e23;
  double min_intensity = 1e23;

  int dimx,dimy,dimz;
  double intensity;

  // create the parser and read the responses
  proc_io_filter_xml_parser parser;

  if (!parse(fname, parser)) {
    vcl_cout << "Exitting!" << vcl_endl;
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
  uint8_t * voxels;
   voxels = new uint8_t[blocksize];

  (void)memset(voxels, 0, blocksize);

  vcl_cout << field.size() << vcl_endl;
  for(unsigned long i = 0; i < blocksize; i++)
    voxels[i] = static_cast<uint8_t>((field[i] - min_intensity)*255/(max_intensity - min_intensity));

 dim = SbVec3s(dimx-2*marginx, dimy-2*marginy, dimz-2*marginz);

 return voxels;

  }

uint8_t *  read_splr_responses(vcl_string fname,unsigned int filter_num,double resolution,
                           SbVec3s & dim)
    {
proc_io_filter_xml_parser parser;

  if (!parse(fname, parser)) {
    vcl_cout << "failed to load response file\n";

  }
  
  biob_grid_worldpt_roster grid(parser.active_box(), resolution);
  

  double max_intensity = - 1e23;
  double min_intensity = 1e23;


  biob_worldpt_field<xmvg_filter_response<double> > response_field = parser.splr_response_field();
  vcl_cout << "(bioproc_splr_response_vis.cxx)" <<  response_field.roster()->num_points() << "\n";
  vcl_cout << "(bioproc_splr_response_vis.cxx)" <<  response_field.roster()->point(biob_worldpt_index(0)) << "\n";
  geom_index_structure geom(response_field.roster(), resolution);
    
 
  int filters_size = parser.filter_num();
  assert (filter_num <= filters_size);
  biob_worldpt_index not_found = biob_worldpt_index(response_field.roster()->num_points());
  vcl_vector<double> grid_response_values(grid.num_points());
  for (unsigned int i = 0; i < grid.num_points(); ++i){
    biob_worldpt_index closest_sample_pt = not_found;
    double best_distance = 9e9;//should use infinity
    worldpt point = grid.point(biob_worldpt_index(i));
    geom_probe_volume_sptr probe_volume = static_cast<geom_probe_volume*>(
        new geom_rectangular_probe_volume(vgl_box_3d<double>(point, 2*resolution, 2*resolution, 2*resolution, vgl_box_3d<double>::centre)));
    biob_worldpt_index_enumerator_sptr neighbors = geom.enclosed_by(probe_volume);
    while (neighbors->has_next()){
      biob_worldpt_index pti = neighbors->next();
      double distance = vgl_distance(point, response_field.roster()->point(pti));
      if (distance < best_distance){
        closest_sample_pt = pti;
        best_distance = distance;
      }
    }
    double intensity = closest_sample_pt == not_found ? 0. : response_field.values()[closest_sample_pt.index()][filter_num-1];
    if(max_intensity < intensity){
      max_intensity = intensity;
    }
    if(min_intensity > intensity)
      min_intensity = intensity;
    grid_response_values[i] = intensity;
  }

  const size_t blocksize = grid_response_values.size();
  uint8_t * voxels = new uint8_t[blocksize];
  (void)memset(voxels, 0, blocksize);
  unsigned int counter = 0;
  for(int k=0;k<grid.nz();k++) {
    for(int j=0;j<grid.ny();j++) {
      for(int i=0;i<grid.nx();i++) {
        voxels[counter] = static_cast<uint8_t>((grid_response_values[grid.index_3d_2_1d(i,j,k).index()] - min_intensity)*255/(max_intensity - min_intensity));
        ++counter;
      }
    }
  }
 dim = SbVec3s(grid.nx(), grid.ny(), grid.nz());
return voxels;

    }

int main(int argc, char* argv[])
{
 int my_argc = argc+1;
  char** my_argv = new char*[argc+1];
  for (int i=0; i<argc; i++)
    my_argv[i] = argv[i];
  my_argv[argc] = "--mfc-use-gl";
  vgui::init(my_argc, my_argv);
  delete []my_argv;


static bool click_filter;
static bool click_edge;
static bool click_splr;

    vgui_dialog dlg("click the type of visualizer");
    dlg.checkbox("filter response visualizer:",click_filter);
    dlg.checkbox("edge response visualizer:",click_edge);
    dlg.checkbox("splr response visualizer:",click_splr);

    if (!dlg.ask())
    return 0;
    

    if (click_filter)
        {
        static vcl_string fname = " ";
        static vcl_string ext = "*.*";
        static bool mfc_use_gl;
        unsigned int marginx = 0;
        unsigned int marginy = 0;
        unsigned int marginz = 0;
        unsigned int filter_num = 1;

        vgui_dialog dlg1("filter response visualizer");

        dlg1.file("file name :",ext,fname);
        dlg1.field("x margin :",marginx);
        dlg1.field("y margin :",marginy);
        dlg1.field("z margin :",marginz);
        dlg1.field("filter number :",filter_num);
        dlg1.checkbox("MFC use GL :",mfc_use_gl);

        if (!dlg1.ask())
            return 0;

     

        else
            {
         
 bgui3d_init();
 
   uint8_t *voxels;
  SbVec3s dim;

voxels = read_filter_responses(fname,filter_num,marginx,marginy,marginz,dim);

  biov_examiner_tableau_new tab3d = setup_biov_examiner_tableau(dim,voxels);

  vgui_shell_tableau_new shell(tab3d);
  int return_value = vgui::run(shell, 400, 400);
  delete [] voxels;
  return return_value;
            }
        }

    if (click_edge)
        {
        static vcl_string fbase = " ";
        static vcl_string ext = "*.*";
        static unsigned int marginx = 0;
        static unsigned int marginy = 0;
        static unsigned int marginz = 0;

        vgui_dialog dlg2("Edge response visualization");

        dlg2.file("file name base :",ext,fbase);
        dlg2.field("x margin :",marginx);
        dlg2.field("y margin :",marginy);
        dlg2.field("z margin :",marginz);


        if (!dlg2.ask())
            return 0;

        else
            {
  bgui3d_init();

  uint8_t *voxels;
  SbVec3s dim;

voxels = read_ede_responses(fbase,marginx,marginy,marginz,dim);
                       
  biov_examiner_tableau_new tab3d = setup_biov_examiner_tableau(dim,voxels);

  vgui_shell_tableau_new shell(tab3d);
  int return_value = vgui::run(shell, 400, 400);
  delete [] voxels;
  return return_value;
        }
  }

     if (click_splr)
        {
        static vcl_string fname = " ";
        static vcl_string ext = "*.*";
        double resolution = 0;
        unsigned int filter_num = 0;
        

        vgui_dialog dlg3("splr response visualizer");

        dlg3.file("file name :",ext,fname);
        dlg3.field("resolution :",resolution);
        dlg3.field("filter number :",filter_num);
        

        if (!dlg3.ask())
            return 0;

     

        else
            {
         
 bgui3d_init();
 
   uint8_t *voxels;
  SbVec3s dim;

voxels = read_splr_responses(fname,filter_num,resolution,dim);

  biov_examiner_tableau_new tab3d = setup_biov_examiner_tableau(dim,voxels);

  vgui_shell_tableau_new shell(tab3d);
  int return_value = vgui::run(shell, 400, 400);
  delete [] voxels;
  return return_value;
            }
        }

}

