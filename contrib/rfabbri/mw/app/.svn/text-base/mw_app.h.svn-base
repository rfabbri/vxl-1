// This is mw_app.h
#ifndef mw_app_h
#define mw_app_h
//:
//\file
//\brief Multiview application
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date Sat Apr 16 22:49:00 EDT 2005
//
#include <vcl_string.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_fundamental_matrix.h>
#include <mw/mw_util.h>
#include <dbdif/dbdif_frenet.h>
#include <dbsol/dbsol_geno_curve_2d.h>

void mw_misc();
void mw_load_current_working_repository();
void mw_load_current_working_repository_curve_tracing_tool();
void mw_load_mcs_instance();
void load_ct_spheres_dataset();
void example_project();
void example_project_and_reconstruct();
void call_show_contours_process(char *fname);
void test_point_reconstruct();
// same thing but with rig datastructure:
void test_point_reconstruct_rig();
void test_k_formula();
void test_formulas_circle();
void test_formulas_helix();
void test_formulas_space_curve1();
void test_geometry_numerics();
void test_geometry_numerics2();
void test_geometry_numerics2_2(unsigned n_iter, unsigned n_iter_position);
void arc_positional_descent_test(unsigned n_iter, unsigned n_iter_position);


void test_formulas(
  vcl_vector<dbdif_3rd_order_point_3d> &C,
  vcl_vector<double> &theta,
  vcl_vector <vsol_point_2d_sptr> &gama1_img
  );

void write_geno_info_super_sample(const dbsol_geno_curve_2d &gc, char *suffix);

void
write_geno_info( const dbsol_geno_curve_2d &gc, const dbsol_geno_curve_2d &gc_circle, 
    const char *ssuffix);
void write_geno_arc_info( const dbsol_geno_curve_2d &gc_circle, const char *ssuffix);

#define MANAGER bvis1_manager::instance()


#endif // mw_app_h

