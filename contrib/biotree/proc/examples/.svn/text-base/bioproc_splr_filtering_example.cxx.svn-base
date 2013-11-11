//: 
// \file    bioproc_splr_filtering_example.cxx
// \brief   an example to use bioproc_splr_filtering_proc with volume rendering to show its 
//          response field.
// \author  K. Kang, P. Klein and H. Can Aras
// \date    2005-04-05, modified 2005_06_27
// 

#include <vcl_ctime.h>

#include <xmvg/xmvg_no_noise_filter_3d.h>
#include <xmvg/xmvg_composite_filter_3d.h>
#include <xscan/xscan_dummy_scan.h>
#include <proc/bioproc_splr_filtering_proc.h>
#include <splr/splr_grid_worldpt_roster.h>
#include <splr/splr_roster_to_grid_mapping.h>
#include <vcl_algorithm.h>

#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#include <bgui3d/bgui3d.h>
#include <bgui3d/bgui3d_examiner_tableau.h>

#include <Inventor/nodes/SoGroup.h>
#include <VolumeViz/nodes/SoVolumeRender.h>
#include <VolumeViz/nodes/SoVolumeData.h>
#include <VolumeViz/nodes/SoVolumeRender.h>
#include <VolumeViz/nodes/SoVolumeRendering.h>
#include <VolumeViz/nodes/SoTransferFunction.h>

#include <vcl_sstream.h>

int main(int argc, char** argv)
{
  vcl_stringstream ss;
  ss << "F:/MyDocs/Temp/toy_data_tif/toy####.tif";
  // scan
    xscan_dummy_scan scan(10, 40, 160, vnl_int_2(200, 200), vnl_double_2(100, 100),
      vgl_point_3d<double>(40.0, 0.0, 0.0), vnl_double_3(.0, .0, 1.), 
                          2*vnl_math::pi/2000, 2000,  ss.str().c_str());

  xscan_scan const & scan_ref = dynamic_cast<xscan_scan const&>(scan);
  // box
  double xmin, ymin, zmin, xmax, ymax, zmax;
  xmin = ymin = zmin = -0.1;
  xmax = ymax = zmax = 0.1;
  // bounding box
  vgl_box_3d<double> box(xmin, ymin, zmin, xmax, ymax, zmax);
  // resolution
  double resolution = 0.0125;
  // filter 3d
  double f_radius = 0.0125;
  double f_length = 0.1;
  vgl_point_3d<double> f_centre(0.0, 0.0, 0.0);
  vgl_vector_3d<double> f_orientation_z(0.0, 0.0, 1.0);
  vgl_vector_3d<double> f_orientation_y(0.0, 1.0, 0.0);
  vgl_vector_3d<double> f_orientation_x(1.0, 0.0, 0.0);
  xmvg_no_noise_filter_descriptor fdx(f_radius, f_length, f_centre, f_orientation_x);
  xmvg_no_noise_filter_descriptor fdy(f_radius, f_length, f_centre, f_orientation_y);
  xmvg_no_noise_filter_descriptor fdz(f_radius, f_length, f_centre, f_orientation_z);
  // construct the filters
  xmvg_no_noise_filter_3d fx(fdx);
  xmvg_no_noise_filter_3d fy(fdy);
  xmvg_no_noise_filter_3d fz(fdz);

  vcl_vector<xmvg_no_noise_filter_3d> filters;
  //filters.push_back(fx);
  //filters.push_back(fy);
  filters.push_back(fz);

  xmvg_composite_filter_3d<double, xmvg_no_noise_filter_3d> comp3d(filters);
  
  splr_grid_worldpt_roster grid(box, resolution);
  //place to store responses
  vcl_vector<xmvg_filter_response<double> > field(grid.num_points());
  unsigned int current_voxel_position = 0;

  time_t t1, t2;
  t1 = vcl_clock();
  for (double zcurrent = zmin; zcurrent <= zmax; zcurrent += resolution){
     vgl_box_3d<double> zslice(xmin, ymin, zcurrent, xmax, ymax, zcurrent);
      bioproc_splr_filtering_proc<double, xmvg_no_noise_filter_3d> proc(const_cast<xscan_scan&>(scan_ref), zslice, resolution, comp3d);
      //construct grid for fixed value of z
      splr_grid_worldpt_roster zgrid(zslice, resolution);
      //for each point on the grid, find the nearest sample point.
      // if there is not one near enough, use sample point 0)
      vcl_vector<biob_worldpt_index> which_sample;
      splr_roster_to_grid_mapping(proc.sample_locations(), &zgrid, which_sample);
      //for grid point i, which_sample[i] identifies the closest point in proc->sample_locations()
      // compute filter responses at those sample locations given by which_sample
      proc.execute(&which_sample);
      //get responses
      vcl_vector<xmvg_filter_response<double> > & zresponses = proc.responses();
      //copy responses for this z slice into all_responses
      for (unsigned long int i = 0; i < zgrid.num_points(); ++i){
        field[current_voxel_position++] = zresponses[i];
      }
  }
  t2 = vcl_clock();
  vcl_cout << grid.num_points() << "points in " << double(t2-t1) / CLOCKS_PER_SEC << " seconds\n";
  vcl_cout << double(t2-t1) / CLOCKS_PER_SEC / grid.num_points() << " seconds per point\n";
// initialize vgui
  vgui::init(argc, argv);

  bgui3d_init();
  
  // make scene contain camera and light
  SoGroup *root = new SoGroup;

  root->ref();

  SoVolumeRendering::init();

  //  vcl_vector<xmvg_filter_response<double> > field;
  //indices of proc.responses() correspond to indices of which_sample

  unsigned angle_index = 0;

  double max_intensity = - 1e23;
  double min_intensity = 1e23;

  for (unsigned int i = 0; i < grid.num_points(); ++i){
        double intensity = field[i][angle_index];
        max_intensity = vcl_max(max_intensity, intensity);
        min_intensity = vcl_min(min_intensity, intensity);
  }
 
  const size_t blocksize = grid.num_points();
  uint8_t * voxels = new uint8_t[blocksize];

  (void)memset(voxels, 0, blocksize);

  for(unsigned long i = 0; i < blocksize; i++)
    voxels[i] = static_cast<unsigned char>( (field[i][angle_index] - min_intensity)*255/(max_intensity - min_intensity));

  SbVec3s dim = SbVec3s(grid.nx(), grid.ny(), grid.nz());

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
