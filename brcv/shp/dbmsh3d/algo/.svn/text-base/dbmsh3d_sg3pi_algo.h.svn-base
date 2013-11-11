#ifndef dbmsh3d_sg3pi_algo_h_
#define dbmsh3d_sg3pi_algo_h_
//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/algo/dbmsh3d_sg3pi_algo.h
//:
// \file
// \brief Algorithm for Shape Grabber raw scan file I/O and processing.
//
// \author
//  MingChing Chang  Feb 04, 2007
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------


#include <dbmsh3d/dbmsh3d_mesh.h>
#include <dbmsh3d/dbmsh3d_sg3pi.h>

#define INTRA_SCANLINE_TH     2
#define INTER_SCANLINE_TH     2

// #################################################################
//    SHAPE GRABBER .3PI RAW SCAN FILE I/O
// #################################################################

bool dbmsh3d_load_sg3pi (dbmsh3d_sg3pi* sg3pi, const char* file);
bool dbmsh3d_save_sg3pi (dbmsh3d_sg3pi* sg3pi, const char* file);

// #################################################################
//    SMOOTHING the SHAPE GRABBER .3PI RAW SCANS
// #################################################################

void dcs_smooth_scanlines_3pi (dbmsh3d_sg3pi* sg3pi, const unsigned int nsteps,
                               const float DCS_psi, const float DCS_th_ratio);

void dcs_smooth_across_scanlines_3pi (dbmsh3d_sg3pi* sg3pi, const unsigned int nsteps,
                                      const float DCS_psi, const float DCS_th_ratio);

void dcs_smooth_scanlines_2dirs_3pi (dbmsh3d_sg3pi* sg3pi, const float DCS_psi, const float DCS_th_ratio,
                                     const int n_intra, const int n_inter);

//: Discrete curve shortening only in z(x,y) value.
bool dbgl_curve_shorten_z (vcl_vector<dbmsh3d_sg3pi_pt*>& scanline, 
                           const float psi, const unsigned int nsteps);

void apply_median_filter_1 (dbmsh3d_sg3pi* sg3pi);
void apply_median_filter_2 (dbmsh3d_sg3pi* sg3pi);

void median_filter_3pi (dbmsh3d_sg3pi* sg3pi, const int option, const int nsteps);

void gaussian_smooth_scanlines_3pi (dbmsh3d_sg3pi* sg3pi, const int nsteps, const float G_th_ratio);

void gaussian_smooth_2d_3pi (dbmsh3d_sg3pi* sg3pi, const int nsteps,
                             const float G_sigma_ratio,
                             const float G_radius_ratio,
                             const float G_th_ratio);

// #################################################################
//    SHAPE GRABBER .3PI RAW SCAN MESHING
// #################################################################

void build_mesh_faces_3pi (dbmsh3d_sg3pi* sg3pi, dbmsh3d_mesh* M);

void mesh_between_scanlines (const vcl_vector<dbmsh3d_sg3pi_pt*>& scanline0, 
                             const vcl_vector<dbmsh3d_sg3pi_pt*>& scanline1, 
                             const double intra_scanline_th,
                             dbmsh3d_mesh* M);

void sg_detect_bbox (const dbmsh3d_sg3pi* sg3pi, vgl_box_3d<double>& box);

void sg_crop_3pi (dbmsh3d_sg3pi* sg3pi, 
                  const double minX, const double minY, const double minZ, 
                  const double maxX, const double maxY, const double maxZ);

#endif

