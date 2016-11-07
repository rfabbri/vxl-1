#ifndef _dbmsh3d_fileio_h_
#define _dbmsh3d_fileio_h_
//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/algo/dbmsh3d_fileio.h
//:
// \file
// \brief mesh file I/O
//
// 
// \author
//  MingChing Chang  Apr 22, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vcl_cassert.h>
#include <vcl_string.h>

#include <dbmsh3d/dbmsh3d_mesh.h>
#include <dbmsh3d/dbmsh3d_richmesh.h>

//: List of supported mesh file formats
typedef enum {
  BOGUS_DBMSH3D_FILE  = 0,
  DBMSH3D_FILE_XYZ,
  DBMSH3D_FILE_XYZN1,
  DBMSH3D_FILE_XYZNW,
  DBMSH3D_FILE_P3D,
  DBMSH3D_FILE_3PI,
  DBMSH3D_FILE_PLY,
  DBMSH3D_FILE_PLY2,
  DBMSH3D_FILE_OFF,
  DBMSH3D_FILE_OBJ,
  DBMSH3D_FILE_M,
  DBMSH3D_FILE_IV,
  DBMSH3D_FILE_WRL,
  DBMSH3D_FILE_VTK,
  DBMSH3D_FILE_CMS,   //coarse-scale medial scaffold.
  DBMSH3D_FILE_SG,    //medial scaffold graph.
} DBMSH3D_FILE_TYPE;

class SbColor;

// ply format mode
enum dbmsh3d_storage_mode {
    BINARY,
    ASCII  
};

// #################################################################
//    POINT CLOUD FILE I/O
// #################################################################

bool dbmsh3d_load_xyz (dbmsh3d_pt_set* pointset, const char* file);
bool dbmsh3d_save_xyz (dbmsh3d_pt_set* pointset, const char* file);

bool dbmsh3d_load_xyz (vcl_vector<vgl_point_3d<double> >& pts, const char* file);
bool dbmsh3d_save_xyz (vcl_vector<vgl_point_3d<double> >& pts, const char* file);

bool dbmsh3d_load_xyz (vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& idpts, const char* file);
bool dbmsh3d_save_xyz (vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& idpts, const char* file);

bool dbmsh3d_load_xyzn1 (vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_vector_3d<double> > >& ori_pts,
                         const char* file);
bool dbmsh3d_save_xyzn1 (vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_vector_3d<double> > >& ori_pts,
                         const char* file);

bool dbmsh3d_load_xyznw (vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_vector_3d<double> > >& ori_pts,
                         const char* file);
bool dbmsh3d_save_xyznw (vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_vector_3d<double> > >& ori_pts,
                         const char* file);
bool dbmsh3d_load_pts (dbmsh3d_pt_set* pointset, const char* file);

bool dbmsh3d_load_p3d (dbmsh3d_pt_set* pointset, const char* file);
bool dbmsh3d_save_p3d (dbmsh3d_pt_set* pointset, const char* file);

bool dbmsh3d_load_p3d (vcl_vector<vgl_point_3d<double> >& pts, const char* file);
bool dbmsh3d_save_p3d (vcl_vector<vgl_point_3d<double> >& pts, const char* file);

bool dbmsh3d_load_p3d (vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& idpts, const char* file);
bool dbmsh3d_save_p3d (vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& idpts, const char* file);

int read_num_genes_sphere_from_file (const char* file);

//: read the .CON file as a contour on a slice
bool dbmsh3d_load_con (dbmsh3d_pt_set* pointset, const char* pcFile_CON, double z);

// #################################################################
//    MESH (INDEXED FACE SET) FILE I/O
// #################################################################

bool save_unmeshed_p3d (dbmsh3d_mesh* M, const char* file);

bool dbmsh3d_load_ply2 (vcl_vector<vgl_point_3d<double> >* pts, 
                        vcl_vector<vcl_vector<int> >* faces, const char* file);
bool dbmsh3d_save_ply2 (const vcl_vector<vgl_point_3d<double> >& pts, 
                        const vcl_vector<vcl_vector<int> >& faces, const char* file);

//: Load a mesh files (only PLY and PLY2 are currently supported)
// "format" should be upper case
bool dbmsh3d_load(dbmsh3d_mesh* M, const char* file, const char* format = "DEFAULT");

//: save a mesh to a .ply file 
// ascii_mode = true : save an ascii PLY file
// ascii_mode = false : save a binary PLY file
bool dbmsh3d_save_ply (dbmsh3d_mesh* M, const char* file, bool ascii_mode = true);

//: load a .ply file (ascii or binary)
bool dbmsh3d_load_ply (dbmsh3d_mesh* M, const char* file);
bool dbmsh3d_load_ply_v (dbmsh3d_mesh* M, const char* file); 
bool dbmsh3d_load_ply_f (dbmsh3d_mesh* M, const char* file);

//: save mesh to a .ply2 file
bool dbmsh3d_save_ply2 (dbmsh3d_mesh* M, const char* file);
//: load a .ply2 file
bool dbmsh3d_load_ply2 (dbmsh3d_mesh* M, const char* file);
bool dbmsh3d_load_ply2_v (dbmsh3d_mesh* M, const char* file);
bool dbmsh3d_load_ply2_f (dbmsh3d_mesh* M, const char* file);

void setup_IFS_M_label_Fs_vids (dbmsh3d_mesh* M, const int label, 
                                vcl_vector<dbmsh3d_vertex*>& vertices,
                                vcl_vector<dbmsh3d_face*>& faces);

//Save the labelled mesh faces into file.
bool dbmsh3d_save_label_faces_ply2 (dbmsh3d_mesh* M, const int label, const char* file);

//: Load/Save Geomview OFF file.
bool dbmsh3d_load_off (dbmsh3d_mesh* M, const char* file);
bool dbmsh3d_save_off (dbmsh3d_mesh* M, const char* file);

//: Load/Save Wavefront OBJ file.
bool dbmsh3d_load_obj (dbmsh3d_mesh* M, const char* file);
bool dbmsh3d_save_obj (dbmsh3d_mesh* M, const char* file);

//: Load/Save Hugues Hoppe's .m file.
bool dbmsh3d_load_m (dbmsh3d_mesh* M, const char* file);
bool dbmsh3d_save_m (dbmsh3d_mesh* M, const char* file);

bool dbmsh3d_load_wrl_iv (dbmsh3d_mesh* M, const char* file);
bool dbmsh3d_save_wrl_iv (dbmsh3d_mesh* M, const char* file);

//: Read IV as ASCII indexed-face-set.
bool dbmsh3d_load_iv_ifs (dbmsh3d_mesh* M, const char* file);
bool dbmsh3d_load_iv_ifs_intp (const char *pcIVFile, const char *pcG3DFile, int option, 
                               float fMaxPerturb, float fIVRecursiveThreshold);
bool dbmsh3d_save_iv_ifs (dbmsh3d_mesh* M, const char* file);

bool dbmsh3d_load_vtk (dbmsh3d_mesh* M, const char* file);
bool dbmsh3d_save_vtk (dbmsh3d_mesh* M, const char* file);

bool dbmsh3d_read_list_file (const char* file, 
                             vcl_vector<vcl_string>& data_files, 
                             vcl_vector<vcl_string>& align_files);

bool dbmsh3d_save_list_file (const vcl_string& list_file, 
                             const vcl_vector<vcl_string>& data_files, 
                             const vcl_vector<vcl_string>& align_files);

bool dbmsh3d_save_list_view_run_file (const vcl_string& list_view_run, 
                                      const vcl_string& list_file);

// ============================================================================
// Rich Mesh
// ============================================================================

//: Load a rich mesh given a list of vertex properties and face properties to read
bool dbmsh3d_load_ply (dbmsh3d_richmesh* M, 
                       const char* file, 
                       const vcl_vector<vcl_string >& vertex_property_list,
                       const vcl_vector<vcl_string >& face_property_list);

//: Load a rich mesh given a list of vertex properties and face properties to read
bool dbmsh3d_save_ply (dbmsh3d_richmesh* M, 
                       const char* file, 
                       const vcl_vector<vcl_string >& vertex_property_list,
                       const vcl_vector<vcl_string >& face_property_list,
                       dbmsh3d_storage_mode mode = ASCII);



bool dbmsh3d_save_xml (dbmsh3d_mesh* mesh, const char* file);

// #################################################################
//    PROCESS NUAGES SLICE FILES
// #################################################################

void sli_cons_to_nuages_cnt (vcl_string sli_file, vcl_string cnt_file);


#endif

