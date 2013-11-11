#ifndef dbmsh3d_scan_sim_h_
#define dbmsh3d_scan_sim_h_
//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/algo/dbmsh3d_scan_sim.h
//:
// \file
// \brief A 3D scan simulator.
//
//
// \author
//  MingChing Chang  July 23, 2007
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <dbmsh3d/dbmsh3d_mesh.h>
#include <dbmsh3d/dbmsh3d_sg3pi.h>

bool scan_sim (dbmsh3d_mesh* M, 
               const int n_views, const int view_min, const int view_max,
               const float inter_sl_ratio, const float intra_sl_ratio, 
               const int minY, const int maxY, const int minX, const int maxX,
               const float ptb, const char* prefix);

void scan_sim_cylinder (dbmsh3d_mesh* M, 
                        const int n_views, const int view_min, const int view_max,
                        const vgl_point_3d<double>& O, const double dOC,
                        const double delta_y, const double delta_x,
                        const int minY, const int maxY, const int minX, const int maxX,
                        const double max_noise, const char* prefix);

dbmsh3d_sg3pi* scan_sim_view (dbmsh3d_mesh* M, 
                              const vgl_point_3d<double>& C, const double theta,
                              const double delta_y, const double delta_x,
                              const int minY, const int maxY, const int minX, const int maxX,
                              const double max_noise);

bool save_scan_sim_af_file (const vgl_point_3d<double>& O, 
                            const double& dOC, const double& theta, 
                            const char* file_af);

#endif

