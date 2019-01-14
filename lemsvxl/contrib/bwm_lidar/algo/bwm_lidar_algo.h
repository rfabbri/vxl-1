#ifndef bwm_lidar_algo_h_
#define bwm_lidar_algo_h_

#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/algo/vil_structuring_element.h>
#include <vil/algo/vil_binary_dilate.h>
#include <dbmsh3d/dbmsh3d_mesh.h>

//####################################################################

//Convert labelled lidar images to mesh.
dbmsh3d_mesh* lidar_to_mesh (vil_image_view<vxl_byte>& img1, 
                             vil_image_view<vxl_byte>& img2, 
                             vil_image_view<vxl_byte>& label_img);

void convert_label_image (vil_image_view<vxl_byte>& label_img, 
                          vil_image_view<bool>& gnd, 
                          vil_image_view<bool>& veg, 
                          vil_image_view<bool>& bld);

bool median_image_value (vil_image_view<vxl_byte>& img, double& median);

void extract_horizontal_polygons (vil_image_view<vxl_byte>& img1, 
                                  vil_image_view<bool>& bld,
                                  std::vector<std::vector<vgl_point_2d<double> > >& polys,
                                  std::vector<double>& heights,
                                  vil_image_view<int>& plane_mask);

//return total number of regions and label of each region in labeled.
unsigned int conn_label (vil_image_view<bool>&bld, 
                         vil_image_view<int>& labeled);

bool save_int_image (vil_image_view<int>& img, std::string filename);
bool save_bw_image (vil_image_view<bool>& img, std::string filename);

void segment_horizontal_planes (vil_image_view<vxl_byte>& img1,
                                vil_image_view<int>& labeled, 
                                const unsigned int b,
                                vil_image_view<bool>& planes, 
                                std::vector<double>& heights);

void hist (std::vector<vxl_byte>& value, 
           std::vector<double>& bins, 
           std::vector<int>& h);

void extract_region_perimeter (vil_image_view<bool>& planes, 
                               const unsigned int i,
                               std::vector<vgl_point_2d<double> >& poly_big);

void bw_fill_holes (vil_image_view<bool>& img);

void get_bw_perim (vil_image_view<bool>& planes2, vil_image_view<bool>& perim);

bool non_zero (vil_image_view<bool>& img);

//push the current reduced polygon contour into the vector of polys[]
void reduce_verts (std::vector<vgl_point_2d<double> >& poly_big,
                   std::vector<vgl_point_2d<double> >& poly);

#endif
