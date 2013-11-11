//MingChing Chang 040302
// 3D ShockGraph-Boundary Reconstruction

#ifndef _dbsk3d_ms_recon_h_
#define _dbsk3d_ms_recon_h_

#include <vcl_vector.h>

#include <dbmsh3d/dbmsh3d_graph.h>
#include <dbsk3d/dbsk3d_ms_hypg.h>
#include <dbsk3d/dbsk3d_sg_sa.h>

//=========== Boundary Reconstruction and Ridge Detection ===========

//Ridge boundary curve detection
bool _add_to_ridgebnd (vcl_vector<vgl_point_3d<double> >& ridgebnd_curve,
                       const vgl_point_3d<double> Ga);

void get_ridge_bnd_curves (dbsk3d_ms_curve* SCurve, 
                           vcl_vector<vgl_point_3d<double> >& ridgebnd_curve_a, 
                           vcl_vector<vgl_point_3d<double> >& ridgebnd_curve_b);

void get_curoff_patch_bnd_curves (dbsk3d_ms_curve* SCurve, dbsk3d_ms_sheet* SSheet,
                                  vcl_vector<vgl_point_3d<double> >& surfbnd_curve_a, 
                                  vcl_vector<vgl_point_3d<double> >& surfbnd_curve_b);

//Ridge estimation
vgl_vector_3d<double> get_init_ridge_vector (const dbsk3d_fs_edge* A3RibElm,
                                             const dbsk3d_fs_face* patchElm,
                                             const vgl_point_3d<double>& C);

vgl_vector_3d<double> get_ridge_vector (const dbsk3d_fs_edge* A3RibElm,
                                        const dbsk3d_fs_face* patchElm,
                                        const vgl_point_3d<double>& C,
                                        const dbsk3d_fs_edge* prevRibElm);


bool save_ms_bnd_link_g3d (dbmsh3d_graph* ms_graph, dbmsh3d_mesh* bndset, vcl_string filename);

void get_A3Rib_bnd_trace_points (dbmsh3d_graph* ms_graph, 
                                 vcl_vector <vgl_point_3d<double> >* pts);
void get_A13Axial_bnd_trace_points (dbmsh3d_graph* ms_graph, 
                                    vcl_vector <vgl_point_3d<double> >* pts);

#endif
