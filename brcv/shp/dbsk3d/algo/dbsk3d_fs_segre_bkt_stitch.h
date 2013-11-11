//: dbsk3d/algo/dbsk3d_fs_segre_bkt_stitch.h
//  MingChing Chang
//  May 31, 2007        Creation.

#ifndef dbsk3d_fs_segre_bkt_stitch_h_
#define dbsk3d_fs_segre_bkt_stitch_h_

#include <dbmsh3d/dbmsh3d_richmesh.h>

class rsdl_kd_tree;

// ###########################################################################
//     Top-level stitching functions
// ###########################################################################

//: Recover the original vertex id into a rich mesh.
bool bkt_recover_vid_richmesh (const char* prefix);

void recover_vid_richmesh (dbmsh3d_richmesh* RM, rsdl_kd_tree* kdtree);   

//: Read in the bucket bounding box info.
bool bkt_pre_stitch_buckets (const char* prefix, const float rbox_ratio);

bool bkt_stitchsurf_recover_vid (const char* prefix);

void filter_bucket_surf (dbmsh3d_richmesh* RM, const vgl_box_3d<double>& box,
                         const vgl_box_3d<double>& rbox,
                         const vcl_string& bkt_int_ply, 
                         dbmsh3d_richmesh* candM,
                         const vcl_vector<vcl_string>& vertex_property_list,
                         const vcl_vector<vcl_string>& face_property_list);

void add_face_check_richmesh_v (dbmsh3d_richmesh* candM, const dbmsh3d_face* F,
                                const bool flag);

bool bkt_merge_final_surface (const char* prefix, dbmsh3d_mesh* M);

//: Put all triangles from RM to M
void create_faces_from_richmesh_vid (dbmsh3d_richmesh* RM, dbmsh3d_mesh* M);

//: Put all triangles from RM to M, ensuring 2-manifold.
void create_faces_from_richmesh_vid_topo (dbmsh3d_richmesh* RM, dbmsh3d_mesh* M);

// ###########################################################################
//     List files and run files generation.
// ###########################################################################


//Generate the meshing run file for all buckets.
void gen_bkt1_seg_bat (const vcl_string& prefix,
                       const vcl_vector<vcl_string>& bucket_list,
                       const float msr);

//Generate the bucketing surface list file.
void gen_bkt1_seg_txt (const vcl_string& prefix,
                       const vcl_vector<vcl_string>& bucket_list);

//Generate the run file to view surfaces in buckets.
void gen_bkt1_seg_view_bat (const vcl_string& prefix);


//Generate the run file for creating richmesh storing original vertex ids.
void gen_bkt2_rmshid_bat (const vcl_string& prefix);

//Generate the run file for pre-stitching.
void gen_bkt3_prestitch_bat (const vcl_string& prefix);

//Generate the run file to compute the stitching surface.
void gen_bkt4_stitch_bat (const vcl_string& prefix, const float msr);

//Generate the run file for creating rich mesh of stitching surface.
void gen_bkt5_stitch_rmsh3d_bat (const vcl_string& prefix);

//Generate the list file of final surfaces.
void gen_bktfinal_txt (const vcl_string& prefix, const vcl_vector<vcl_string>& bucket_list);

//Generate the run file to view final surfaces.
void gen_bktfin_view_bat (const vcl_string& prefix);

//Generate the run file to compute the final surface.
void gen_bkt6_final_bat (const vcl_string& prefix);

//############################################################################

//Generate the list files of gaps between buckets (each for a stitching).
void gen_bktbnd_cellxyz_txt (const vcl_string& prefix, const vgl_box_3d<double>& bbox,
                             const int BUCKET_NX, const int BUCKET_NY, const int BUCKET_NZ);

//Generate the run file to collect the partial mesh to fill the gap between buckets.
void gen_bktbnd_cellxyz_pm_bat (const vcl_string& prefix, const vgl_box_3d<double>& bbox,
                                const int BUCKET_NX, const int BUCKET_NY, const int BUCKET_NZ);

//Generate the run file to compute surfaces to fill the gap between buckets.
void gen_bktbnd_cellxyz_seg_bat (const vcl_string& prefix, const vgl_box_3d<double>& bbox,
                                 const int BUCKET_NX, const int BUCKET_NY, const int BUCKET_NZ);

//Generate the list file for the final stitching.
void gen_bktsth_txt_cell (const vcl_string& prefix, 
                          const vcl_vector<vcl_string>& bucket_list,
                          const vgl_box_3d<double>& bbox,
                          const int BUCKET_NX, const int BUCKET_NY, const int BUCKET_NZ);

//Generate the final run file to stitch the surfaces together.
void gen_bktsth_bat (const vcl_string& prefix);


#endif
