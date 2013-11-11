//: This is bwm_lidar_vis.h
//  lidar image meshing and visualization.
//  MingChing Chang
//  Dec 13, 2007.

#ifndef bwm_lidar_vis_h_
#define bwm_lidar_vis_h_

///#include "bwm_lidar_vis.h"
#include <vcl_set.h>
#include <vcl_vector.h>

#include <vil/vil_load.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_vector_fixed.h>
#include <vsol/vsol_point_2d_sptr.h>

#include <vpgl/vpgl_proj_camera.h>

#include <dbmsh3d/dbmsh3d_mesh.h>
#include <dbmsh3d/dbmsh3d_textured_mesh_mc.h>
#include <dbmsh3d/algo/dbmsh3d_mesh_algos.h>

#include "../algo/bwm_lidar_mesh.h"

#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>
#include <dbmsh3d/vis/dbmsh3d_vis_face.h>

#include <Inventor/SbColor.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/engines/SoTimeCounter.h>


//lidar image mesh and visualize
SoSeparator* vis_lidar_data (lidar_range_data* LRD, 
                             int shift_x = 0, int shift_y = 0, int shift_z = 0,
                             float ground_height = -1);

SoSeparator* draw_filled_polygon (vcl_vector<vsol_point_2d_sptr>& poly_points, const float height,
                                  const SbColor& color, const float fTransparency);

void _check_add_v (const double v, vcl_vector<double>& value);

int median_3x3 (vil_image_view<vxl_byte>& img_view_max, 
                const int i, const int j, const int plane);

SoSeparator* vis_lidar_labeled_image (vil_image_resource_sptr lidar_image_max, 
                                      vil_image_resource_sptr lidar_image_min, 
                                      vil_image_resource_sptr lidar_image_labeled);

//######################################################################

SoSeparator* vis_lidar_labeled_data (vil_image_view<int>& labels,
                                     vil_image_view<unsigned char>& labels_colored,
                                     const bool use_labels_colored,
                                     vil_image_view<unsigned char>& colors,
                                     vnl_matrix<double>& height,
                                     vnl_matrix<int>& occupied,
                                     const double& ground_height,
                                     vcl_vector<dbmsh3d_textured_mesh_mc*>& M_vec);

double estimate_ground_height (vil_image_view<int>& labels, 
                               vnl_matrix<double>& height);

SoSeparator* draw_buildings (vil_image_view<int>& labels, 
                             vnl_matrix<double>& height,
                             vil_image_view<unsigned char>& labels_colored,
                             vil_image_view<unsigned char>& colors,
                             const bool use_labels_colored,
                             const double ground_height,
                             vcl_vector<dbmsh3d_textured_mesh_mc*>& M_vec);

double median_3x3 (vnl_matrix<double>& height, const int ni, const int nj, const int i, const int j);

//Add vertices of this building into M.
void add_building_faces (dbmsh3d_textured_mesh_mc* M,
                         const vcl_vector<vsol_point_2d_sptr>& poly_points, 
                         const double median_height, const double ground_height);

SoSeparator* draw_vegetation_pixel (vil_image_view<int>& labels,
                                    vnl_matrix<double>& height,
                                    vil_image_view<unsigned char>& labels_colored,
                                    const double ground_height);

void median_filter_array_3x3 (vnl_matrix<double>& height, const int ni, const int nj);

SoSeparator* vis_lidar_pixel_color (vil_image_view<int>& labels, 
                                    vil_image_view<unsigned char>& colors, 
                                    vnl_matrix<double>& height, 
                                    vnl_matrix<int>& occupied,
                                    const double& ground_height);

//: This function converts a set of meshes into textured meshes using cameras and images
bool texturemap_meshes(vcl_vector<dbmsh3d_textured_mesh_mc*> &meshes, vcl_vector<vil_image_view<vxl_byte> > const& images, vcl_vector<vpgl_proj_camera<double> > const& cameras, vcl_string texture_image_dir);

//: This function saves a set of textured meshes into a vrml file.
bool save_mesh_vrml(vcl_string filename, vcl_vector<dbmsh3d_textured_mesh_mc*> &meshes, 
                    vgl_point_3d<double> virtual_camera_center);



#endif

