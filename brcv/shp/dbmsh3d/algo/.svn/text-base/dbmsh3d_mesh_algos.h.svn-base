// This is dbmsh3d/algo/dbmsh3d_mesh_algos.h

#ifndef dbmsh3d_mesh_algos_h_
#define dbmsh3d_mesh_algos_h_

//--------------------------------------------------------------------------------
//:
// \file
// \brief Place holder for various algos for meshes
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date September 22, 2006
//
// \verbatim
//  Modifications:
//    Terry Orechia and Ming  Jan 2,  2006:   add merge_mesh_coplanar_faces().
// \endverbatim
//--------------------------------------------------------------------------------

#include <dbmsh3d/dbmsh3d_mesh.h>
#include <dbmsh3d/dbmsh3d_richmesh.h>

//check integrity: remove erroneous faces.
bool remove_erroneous_Fs_IFS (dbmsh3d_mesh* M);

void merge_mesh_coplanar_faces (dbmsh3d_mesh* M);

void add_mesh_faces_IFS (dbmsh3d_mesh* M, vcl_vector<vcl_vector<int> >& IFS_faces);

void get_faces_intersect_box (const vcl_vector<vcl_vector<int> >& input_faces, 
                              const vcl_vector<vgl_point_3d<double> >& input_pts, 
                              const vgl_box_3d<double>& box,
                              vcl_vector<vcl_vector<int> >& in_faces);

int erode_mesh_boundary (dbmsh3d_mesh* M, const int steps);

dbmsh3d_mesh* build_mesh_from_IFS (const vcl_vector<vgl_point_3d<double> >& input_pts,
                                   const vcl_vector<vcl_vector<int> >& input_faces);

//: Check if introducing a face of vector<vids> to mesh M violates 2-manifold topology.
bool check_F_M_topo (vcl_vector<int>& vids, dbmsh3d_mesh* M);


//Label mesh components and save large components to various mesh files.
bool cclabel_save_mesh (dbmsh3d_mesh* M, const vcl_string& prefix,
                        const int th_n, const int option = 1);

int label_mesh_component (dbmsh3d_mesh* M, dbmsh3d_face* seedF, const int label);

//Delete the labelled mesh faces from M.
int dbmsh3d_del_label_faces (dbmsh3d_mesh* M, const int label);

int remove_extraneous_faces (dbmsh3d_mesh* M);

vgl_point_3d<double> compute_centroid (dbmsh3d_mesh* M);
double compute_var (dbmsh3d_mesh* M, vgl_point_3d<double>& C);
double compute_cen_var (dbmsh3d_mesh* M, vgl_point_3d<double>& C);

//#################################################################
// Mesh Traversing Algorithms

//: Collect the set of faces given a seedF and a bounding curve (links).
//  Assume M->reset_traverse_f() is already done.
void collect_Fs_given_seed_bnd (dbmsh3d_face* seedF, vcl_set<dbmsh3d_edge*>& bnd_E_set, 
                                dbmsh3d_mesh* M, vcl_set<dbmsh3d_face*>& Fset);

//#################################################################

dbmsh3d_mesh* create_twist_cross ();

//#################################################################

class dbmsh3d_mesh_algos
{
public:
  virtual ~dbmsh3d_mesh_algos(){}

  //: Extract boundary of vertices of a manifold mesh
  // Return false if mesh is not 2-manifold
  static bool find_boundary(dbmsh3d_mesh& mesh, 
    vcl_vector< vcl_vector< dbmsh3d_vertex* > >& boundary_list);

  //: Triangulate two "parallel" curves in space and create a "band-like" mesh
  static bool make_band_trimesh(const vcl_vector<dbmsh3d_vertex* >& poly1,
    const vcl_vector<dbmsh3d_vertex* >& poly2,
    vcl_vector<dbmsh3d_face* >& new_faces);


  //: Compute volume of a mesh when it is a polyhedron
  // Status: TODO
  static double polyhedron_volume(dbmsh3d_mesh& mesh);

  //: Compute centroid of a point set
  static vgl_point_3d<double > compute_centroid(dbmsh3d_mesh* mesh);

  //: Create a new mesh whose faces are a subset of the original mesh
  // Assumption: all faces in the face_list belong to ``mesh"
  static bool submesh(dbmsh3d_mesh& mesh, 
    const vcl_vector<dbmsh3d_face* >& face_list, dbmsh3d_mesh& new_mesh);

  //: crop a mesh with a retangular box
  // only faces that lie completely inside the box will retain
  static bool crop_mesh(dbmsh3d_mesh& source_mesh,
    const vgl_box_3d<double >& box,
    dbmsh3d_mesh& new_mesh);
    

  //: Compute distance between two meshes
  // Compute closest distances from each vertex of "ref_mesh" to "data_mesh"
  // and stored the distance at each vertex with property "verror"
  static bool mesh2mesh_distance_map(dbmsh3d_richmesh& ref_mesh, 
    dbmsh3d_mesh& data_mesh);




protected:
  dbmsh3d_mesh_algos(){}

  // support functions
    //: re-arrange a 3D polyline/polygon so that it initial vertex and its
  // orientation match with a reference 3D polyline/polygon
  // Return re-arranged copy of moving_poly
  static vcl_vector<dbmsh3d_vertex* > align_2_polys(
    const vcl_vector<dbmsh3d_vertex* >& ref_poly,
    const vcl_vector<dbmsh3d_vertex* >& moving_poly);  

};

void unify_mesh_orientation (dbmsh3d_mesh *M, dbmsh3d_face* seedF);

#endif //dbmsh3d_mesh_algos_h_



