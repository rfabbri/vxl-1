//: 
// \file   bioproc_splr_response_vis.cxx
// \brief  visualizer for splr responses (command line version)
// \author H. Can Aras and Kongbin Kang, modified by P. Klein
// \date   August 2005, modified April 2006
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
#include <biob/biob_grid_worldpt_roster.h>
#include <biob/biob_roster_to_grid_mapping.h>
#include <Inventor/nodes/SoGroup.h>
#include <VolumeViz/nodes/SoVolumeRender.h>
#include <VolumeViz/nodes/SoVolumeData.h>
#include <VolumeViz/nodes/SoVolumeRender.h>
#include <VolumeViz/nodes/SoVolumeRendering.h>
#include <VolumeViz/nodes/SoTransferFunction.h>
#include <Inventor/nodes/SoCylinder.h>
#include <vcl_sstream.h>
#include <geom/geom_index_structure.h>
#include <geom/geom_rectangular_probe_volume.h>
#include <vgl/vgl_distance.h>

double get_double(char* str){
  double x;
  vcl_stringstream arg(str);
  arg >> x;
  return x;
}
   

int main(int argc, char** argv)
{
  if(argc < 4){
    vcl_cout << "Usage: "<< argv[0] << "fname resolution filter_num";
    return 1;
  }
  vcl_cout << "Note that the data are only valid for filters that are invariant under turntable rotations\n";



  // create the parser and read the responses
  proc_io_filter_xml_parser parser;
  vcl_string fname = argv[1];
  if (!parse(fname, parser)) {
    vcl_cout << "failed to load response file\n";
    return 1;
  }
  double resolution = get_double(argv[2]);
  biob_grid_worldpt_roster grid(parser.active_box(), resolution);
  int filter_num = atoi(argv[3]);

  double max_intensity = - 1e23;
  double min_intensity = 1e23;


  biob_worldpt_field<xmvg_filter_response<double> > response_field = parser.splr_response_field();
  vcl_cout << "(bioproc_splr_response_vis.cxx)" <<  response_field.roster()->num_points() << "\n";
  vcl_cout << "(bioproc_splr_response_vis.cxx)" <<  response_field.roster()->point(biob_worldpt_index(0)) << "\n";
  geom_index_structure geom(response_field.roster(), resolution);
    
  /*    vcl_vector<biob_worldpt_index> roster_to_grid;
  biob_roster_to_grid_mapping(response_field.roster(), grid, roster_to_grid);
  */
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
  for(int k=0;k<static_cast<int>(grid.nz());k++) {
    for(int j=0;j<static_cast<int>(grid.ny());j++) {
      for(int i=0;i<static_cast<int>(grid.nx());i++) {
        voxels[counter] = static_cast<uint8_t>((grid_response_values[grid.index_3d_2_1d(i,j,k).index()] - min_intensity)*255/(max_intensity - min_intensity));
        ++counter;
      }
    }
  }
  SbVec3s dim = SbVec3s(grid.nx(), grid.ny(), grid.nz());

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
