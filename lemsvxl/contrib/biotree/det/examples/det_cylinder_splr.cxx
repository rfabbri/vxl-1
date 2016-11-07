//: 
// \file  det_hair_nonmax_sup_from_response_example.cxx
// \brief  an example doing the nonmaxium suppression on filter response of hair
//         the difference of this example from det_hair_nonmax_sup_example.cxx
//         is, those one takes the filter response file detects the hair and do the
//         non maxima suppresion on it and displays the result
// \author    Kongbin Kang (kk at lems.brown.edu) & Gamze Tunali
// \date        2005-11-01
// 
#include <vcl_fstream.h>
#include <vcl_cassert.h>
#include <vcl_cstdio.h>
#include <biob/biob_grid_worldpt_roster.h>
#include <biob/biob_roster_to_grid_mapping.h>

#include <xmvg/xmvg_composite_filter_descriptor.h>
#include <det/det_cylinder_map.h>
#include <det/det_suppression_and_interpolation.h>
#include <proc/io/proc_io_filter_xml_parser.h>
#include <vsol/vsol_cylinder.h>
#include <geom/geom_index_structure.h>
#include <geom/geom_rectangular_probe_volume.h>
#include <vgl/vgl_distance.h>

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

double get_double(char* str){
  double x;
  vcl_stringstream arg(str);
  arg >> x;
  return x;
}
const double CYL_RADIUS = 0.5;
const double CYL_LENGTH = 1.0;

int main(int argc, char *argv[])
{
  if(argc != 3){
    vcl_cout << "Usage: "<< argv[0] << " response_file resolution\n";
    return 1;
  }

  char* o_file = argv[6];
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
 
  double max_intensity = - 1e23;
  double min_intensity = 1e23;
  double intensity;
  
  // create the parser and read the responses
  proc_io_filter_xml_parser parser;
  vcl_string fname = argv[1];
  if (!parse(fname, parser)) {
    vcl_cout << "failed to load response file\n";
    return 1;
  }
  double resolution = get_double(argv[2]);
  biob_grid_worldpt_roster grid(parser.active_box(), resolution);
  biob_worldpt_field<xmvg_filter_response<double> > response_field = parser.splr_response_field();
  biob_worldpt_field<vgl_vector_3d<double> > orientation_field(response_field.roster());
  //hack because we're only using z-direction.
  vgl_vector_3d<double> z_direction(0., 0., 1.);
  for (unsigned i = 0; i < orientation_field.roster()->num_points(); ++i){
    orientation_field.values()[i] = response_field.values()[i][0] * z_direction;
  }
  det_suppression_and_interpolation interpolator;
  double min_angle = 2*3.14/360 * 50;
  double strength_threshold = 100000.0;
  biob_worldpt_field<vgl_vector_3d<double> > interpolated_response = interpolator.apply(orientation_field, resolution, resolution, min_angle, strength_threshold);
  //  biob_worldpt_field<vgl_vector_3d<double> > interpolated_response = interpolator.apply(orientation_field, .9*resolution, .9*resolution, );
  geom_index_structure geom(interpolated_response.roster(), resolution);
                                                                           
  /*    vcl_vector<biob_worldpt_index> roster_to_grid;
  biob_roster_to_grid_mapping(interpolated_response.roster(), grid, roster_to_grid);
  */
  int filters_size = parser.filter_num();
                                                                           int filter_num = 1;                                                                           
  assert (filter_num <= filters_size);
  biob_worldpt_index not_found = biob_worldpt_index(interpolated_response.roster()->num_points());
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
      double distance = vgl_distance(point, interpolated_response.roster()->point(pti));
      if (distance < best_distance){
        closest_sample_pt = pti;
        best_distance = distance;
      }
    }
    double intensity = closest_sample_pt == not_found ? 0. : interpolated_response.values()[closest_sample_pt.index()].length();
    if (intensity > 1e06){
      vcl_cout << "(det_cylinder_splr.cxx) " << point << "\n";
    }
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
  SbVec3s dim = SbVec3s(grid.nx(), grid.ny(), grid.nz());

  // initialize vgui
  // Ming: force option "--mfc-use-gl" to use gl
  //       so that it is MUCH faster if running on a
  //       computer with nice graphic card.
  //vgui::init(argc, argv);

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
