//: 
// \file    bioproc_edge_detector_example.cxx
// \brief   
// \author  H. Can Aras
// \date    2005-10-05
// 
#include <vcl_cmath.h>
#include <vcl_ctime.h>

#include <xmvg/xmvg_gaussian_edge_detector_x_3d.h>
#include <xmvg/xmvg_gaussian_edge_detector_y_3d.h>
#include <xmvg/xmvg_gaussian_edge_detector_z_3d.h>
#include <xmvg/xmvg_composite_filter_3d.h>
#include <xscan/xscan_dummy_scan.h>
#include <proc/bioproc_dummy_splat_resource.h>
#include <proc/bioproc_filtering_proc.h>

#include <vcl_sstream.h>

int main(int argc, char** argv)
{
  // scan
  xscan_dummy_scan scan(17.7, 261.5, 345.712, vnl_int_2(500, 500), vnl_double_2(250, 250),
    vgl_point_3d<double>(261.5, 0.0, 0.0), vnl_double_3(0.0, 0.0, 1.0), 
    2*vnl_math::pi/20, 20,  "D:/MyDocs/projects/BioTree/daily_news_2006/mar15/r180_h1000_along_z_2/toy####.tif");

//  xscan_dummy_scan scan(17.7, 261.5, 345.712, vnl_int_2(500, 500), vnl_double_2(250, 250),
//    vgl_point_3d<double>(261.5, 0.0, 0.0), vnl_double_3(0.0, 0.0, 1.0), 
//    2*vnl_math::pi/20, 20,  "./r180_h1000_along_z_2/toy####.tif");

  xscan_scan const & scan_ref = dynamic_cast<xscan_scan const&>(scan);
  // outer box
  double xmin, ymin, zmin, xmax, ymax, zmax;
  xmin = ymin = zmin = -0.5;
  xmax = ymax = zmax = 0.5;
  vgl_box_3d<double> box(xmin, ymin, zmin, xmax, ymax, zmax);
  // inner box
  vgl_vector_3d<double> scale(0.5, 0.5, 0.5);
  vgl_point_3d<double> centroid = box.centroid();
  double centroid_array[3] = {centroid.x(), centroid.y(), centroid.z()};
  vgl_box_3d<double> smallbox(centroid_array, scale.x()*box.width(), scale.y()*box.height(),  scale.z()*box.depth(), vgl_box_3d<double>::centre);
  // resolution
  double resolution = 0.020;
  // filter 3d
  double sigma = 0.020;
  vgl_point_3d<double> centre(0.0, 0.0, 0.0);
  xmvg_gaussian_edge_detector_descriptor edge_det_desc(sigma, centre);
  xmvg_gaussian_edge_detector_z_3d edge_det_z(edge_det_desc);

  vcl_vector<xmvg_gaussian_edge_detector_z_3d> filters;
  filters.push_back(edge_det_z);

  xmvg_composite_filter_3d<double, xmvg_gaussian_edge_detector_z_3d> comp3d(filters);

  bioproc_filtering_proc<double, xmvg_gaussian_edge_detector_z_3d> proc(scan_ref, smallbox, resolution, box, comp3d);

  biob_worldpt_field<xmvg_filter_response<double> > & responses = proc.worldpt_field();
  biob_grid_worldpt_roster bgwr = static_cast<const biob_grid_worldpt_roster&>(*(responses.roster()));
  //unused int dimx = bgwr.nx();
  //unused int dimy = bgwr.ny();
  //unused int dimz = bgwr.nz();
  //unused variable
  //int num_points = dimx * dimy * dimz;

  vcl_ofstream xml_file("D:/MyDocs/projects/BioTree/daily_news_2006/mar15/r180_h1000_along_z_2/edge_z_res.xml");
//  vcl_ofstream xml_file("./r180_h1000_along_z_2/edge_x_res.xml");
  // filtering
  time_t t1, t2;
  t1 = vcl_clock();
  proc.execute();
  t2 = vcl_clock();

  x_write(xml_file, proc);
  
  return 0;
}
