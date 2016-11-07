// This is dbmsh3d/algo/dbmsh3d_pt_mesh_dist.h
//:
// \file
// \brief Compute point to mesh distance
//        
// \author MingChing Chang
// \date   Mar 07, 2007
//
// \verbatim
//  Modifications
// \endverbatim

#ifndef dbmsh3d_pt_mesh_dist_h_
#define dbmsh3d_pt_mesh_dist_h_

#include <dbmsh3d/dbmsh3d_mesh.h>

class rsdl_kd_tree;

//: Compute the closest distance d of P to a mesh face polygon FP:
//  If the foot point F is inside the face FP, d = PF, F = G.
//  Otherwise d = PG, where G is on the boundary of FP.
double dbmsh3d_pt_face_dist_3d (const vgl_point_3d<double>& P, const dbmsh3d_face* FP,
                                vgl_point_3d<double>& G);

rsdl_kd_tree* dbmsh3d_build_kdtree_pts (const vcl_vector<vgl_point_3d<double> >& pts);

rsdl_kd_tree* dbmsh3d_build_kdtree_idpts (const vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& idpts);

rsdl_kd_tree* dbmsh3d_build_kdtree_vertices (dbmsh3d_pt_set* PS);

double dbmsh3d_pt_mesh_dist (const vgl_point_3d<double>& P, 
                             dbmsh3d_vertex* closestV, vgl_point_3d<double>& G);

//: Approximate the closest point G from a point P to a mesh M:
//  Use kd-tree to store all vertices of M and determine the 
//  closest n vertices [V1, ..., vn] from P.
//  Search the 1-ring faces of each Vi using the above point-triangle distance.
//  d (P, M) = min [d (P, Vi), d (P, Gj)].
//
double dbmsh3d_pt_mesh_dist (const vgl_point_3d<double>& P, 
                             dbmsh3d_mesh* M, rsdl_kd_tree* kd_tree,
                             const int top_n, vgl_point_3d<double>& G);

//: Brute-forcely compute the exact closest point G from a point P to a mesh M.
double dbmsh3d_pt_mesh_dist_bf (const vgl_point_3d<double>& P, 
                                dbmsh3d_mesh* M, 
                                vgl_point_3d<double>& G);

//: Return the approximate closest distance between each vertex of M2
//  to the closest point (can be vertex or interior pt on a face) of M1
//
double dbmsh3d_mesh_mesh_dist (dbmsh3d_mesh* M, rsdl_kd_tree* kd_tree, dbmsh3d_mesh* M2, 
                               const int top_n, vcl_vector<double>& min_dists,
                               vcl_vector<vgl_point_3d<double> >& closest_pts);

double estimate_avg_samp_dist (const vcl_vector<vgl_point_3d<double> >& pts, const int k = 4);

double estimate_avg_samp_dist (dbmsh3d_pt_set* PS, const int k = 4);

double dbmsh3d_mesh_mesh_avg_dist (dbmsh3d_mesh* M, rsdl_kd_tree* kd_tree, 
                                   dbmsh3d_mesh* M2, const int top_n, const double dist_th);

#endif



