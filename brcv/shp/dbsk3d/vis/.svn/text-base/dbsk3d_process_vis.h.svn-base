//: This is lemsvxlsrc/brcv/shp/dbsk3d/vis/dbsk3d_process_vis.h
//  Creation: Dec 24, 2005   Ming-Ching Chang

#ifndef dbsk3d_process_vis_h_
#define dbsk3d_process_vis_h_

#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>
#include <dbmsh3d/cmdproc/dbmsh3d_process_vis.h>
#include <dbsk3d/pro/dbsk3d_process.h>

#include <dbsk3d/vis/dbsk3d_vis_fs_mesh.h>
#include <dbsk3d/vis/dbsk3d_vis_ms_hypg.h>

#include <Inventor/SbColor.h>

class SoSeparator;
class SoSwitch;

//###### Visualization parameters ######

//: The Medail Scaffold Visualization Options
class dbsk3d_ms_vis_param
{
public:
  SbColor A1A3_vertex_color_;
  SbColor A1A3_vertex_color_2_;
  SbColor A14_vertex_color_;
  SbColor A14_vertex_color_2_;
  SbColor Dege_vertex_color_;
  SbColor Dege_vertex_color_2_;
  SbColor Loop_vertex_color_;
  SbColor Loop_vertex_color_2_; 

  SbColor A3_rib_color_;
  SbColor A3_rib_color_2_;
  SbColor A13_axial_color_;
  SbColor A13_axial_color_2_;
  SbColor Dege_axial_color_;
  SbColor Dege_axial_color_2_;
  SbColor A3_loop_color_;
  SbColor A3_loop_color_2_;

  float   ball_size_;
  float   curve_width_;
  float   sheet_transp_;

  bool    vertex_show_id_;
  bool    curve_show_id_;
  bool    sheet_show_id_;

  vcl_vector<SbColor>   vertices_ctable_;
  vcl_vector<SbColor>   curves_ctable_;
  vcl_vector<SbColor>   sheets_ctable_;

  dbsk3d_ms_vis_param () {
    A1A3_vertex_color_ = SbColor (0.5f, 1.0f, 0.0f);
    A1A3_vertex_color_2_ = SbColor (0.25f, 0.5f, 0.0f);
    A14_vertex_color_ = SbColor (1.0f, 0.0f, 1.0f);
    A14_vertex_color_2_ = SbColor (0.5f, 0.0f, 0.5f);
    Dege_vertex_color_ = SbColor (0.4f, 0.8f, 0.8f);
    Dege_vertex_color_2_ = SbColor (0.2f, 0.4f, 0.4f);
    Loop_vertex_color_ = SbColor (0.5f, 1.0f, 1.0f);
    Loop_vertex_color_2_ = SbColor (0.25f, 0.5f, 0.5f);

    A3_rib_color_ = SbColor (0.0f, 0.0f, 1.0f); 
    A3_rib_color_2_ = SbColor (0.0f, 0.0f, 0.5f);
    A13_axial_color_ = SbColor (1.0f, 0.0f, 0.0f); 
    A13_axial_color_2_ = SbColor (0.5f, 0.0f, 0.0f); 
    Dege_axial_color_ = SbColor (1.0f, 0.5f, 0.0f); 
    Dege_axial_color_2_ = SbColor (0.5f, 0.25f, 0.0f); 
    A3_loop_color_ = SbColor (0.0f, 0.0f, 1.0f); 
    A3_loop_color_2_ = SbColor (0.0f, 0.0f, 0.5f);

    ball_size_       = 0.03f;
    curve_width_     = 5.0f; //3.0f
    sheet_transp_    = 0.0f;

    vertex_show_id_  = false;
    curve_show_id_   = false;
    sheet_show_id_   = false;
  }

  ~dbsk3d_ms_vis_param () {
    vertices_ctable_.clear();
    curves_ctable_.clear();
    sheets_ctable_.clear();
  }
};

//: subclass dbsk3d_pro and add 
//  new functions to do process with visualization

class dbsk3d_pro_vis : public dbsk3d_pro, public dbmsh3d_vis_base
{
public:
  //: Drawing fine-scale shock sheet transparency.
  float   ssheet_transp_;

  //the visualization parameter structures
  dbsk3d_ms_vis_param   ms_vis_param_;

  float                 view_dx_, view_dy_, view_dz_;

public:
  //###### Constructor and Destructor ######
  dbsk3d_pro_vis() : dbsk3d_pro(), dbmsh3d_vis_base() {
    ssheet_transp_  = 0.0f;
    view_dx_ = 0.0f;
    view_dy_ = 0.0f;
    view_dz_ = 0.0f;
  }

  virtual ~dbsk3d_pro_vis () 
  {
  }

  //: Estimate visualization parameters.
  bool compute_dx_dy_dz (const float ratio = 1.15f);  
  void compute_vertex_node_size ();
  
  //###### Visualization Functions in dbmsh3d_vis_base ######
  virtual SoSeparator* vis_ptset (const int colorcode = COLOR_NOT_SPECIFIED, 
                                  const bool draw_idv = false);
  virtual SoSeparator* vis_ptset_idv ();
  virtual SoSeparator* vis_ptset_color ();

  virtual SoSeparator* vis_mesh (const bool draw_idv, 
                             const bool b_shape_hints = false,
                             const int colorcode = 0,
                             const bool b_draw_isolated_pts = true);
  virtual SoSeparator* vis_mesh_edges (const int colorcode, const float width = 1.0f,
                                       const bool draw_idv = false);

  virtual SoSeparator* vis_mesh_pts_edges (const int colorcode = COLOR_NOT_SPECIFIED,
                                           const bool draw_idv = false,
                                           const float pt_size = 1.0f); //1.0f

  virtual SoSeparator* vis_mesh_bndcurve (const int colorcode, const float width);
  virtual SoSeparator* vis_mesh_color ();
  //: Visualize the re-meshed surface according to the input option.
  virtual SoSeparator* vis_mesh_options (int option, const bool draw_idv = false, 
                                         const bool showid = false);
  virtual SoSeparator* vis_mesh_anim (const int nF_batch);  

  //###### Visualization Functions ######
  SoSeparator* vis_unasgn_genes (const float size);

  //: Processes with visualization
  //  return the visualization in SoSeparator.
  SoSeparator* vis_vertices_of_holes ();
  SoSeparator* vis_shocks_of_holes (int option);

  SoSeparator* vis_fs_mesh_valid (const bool draw_idv = false);
  SoSeparator* vis_fs_mesh (const int colorcode = 0);
  SoSeparator* vis_fs_mesh (const bool draw_valid, const bool draw_invalid, 
                            const bool draw_unbounded = false);

  SoSeparator* display_sheet_flow ();
  SoSeparator* reconstruct();

  SoSeparator* vis_fs_mesh_color_by_radius ();

  SoSeparator* vis_ms_graph (const bool idv = false);
  SoSeparator* vis_sg_sa (const bool idv = false);
  SoSeparator* vis_ms_graph_2 (const bool idv = false);
  SoSeparator* vis_sg_sa_2 (const bool idv = false);

  //-v 0: draw the medial scaffold graph and randomly color each sheet but avoiding color close to red or blue.
  //   1: draw the medial scaffold graph and draw the whole ms_sheets in a single color.
  //   2: only draw the (medial scaffold) graph and skip drawing all sheets.
  //   3: only draw the ms_sheets in random colors and skip drawing the (medial scaffold) graph.
  //   4: only draw the ms_sheets in a single color and skip drawing the (medial scaffold) graph.
  //   5: color each ms_sheet by cost order (in pseudo color).
  //   6: color each ms_sheet by cost (in pseudo color).
  //   7: color each ms_sheet by geodesic distance.
  //   8: not drawing anything.
  SoSeparator* vis_ms_hypg (const int option = 0, 
                            const int color_seed = DBMSH3D_SHEET_COLOR_SEED, const bool idv = false);

  //: Visualize the corresponding bnd mesh patches of shock sheets in color.
  SoSeparator* vis_ms_hypg_bnd_mesh (const int option);

  SoSeparator* vis_ms_hypg_ctable (const bool idv = false);
  SoSeparator* vis_ms_hypg_2_ctable (const bool idv = false);

  SoSeparator* vis_ms_graph_sa (const bool idv = false);
  SoSeparator* vis_ms_graph_sa_2 (const bool idv = false);

  //###### Visualization of Files ######
  
  SoSeparator* view_vor ();
  SoSeparator* view_cms (vcl_string filename = "");
  SoSeparator* view_sg (vcl_string filename = "");
  
  SoSeparator* vis_list_file (vcl_string filename, 
                              const int option, const int vis_option);
  
  void parse_run_file (const char* run_file);
};


//###########################################################################
//  The Bucketing+Stitching Wrapping Function.

SoSeparator* run_seg_adpt_bucketing (const vcl_vector<vgl_point_3d<double> >& input_pts,
                                     vcl_vector<vcl_vector<int> >& finalM_faces,
                                     const vcl_string prefix,
                                     const int npbkt, const bool b_check_dup,
                                     const float bdsphr_rr, const int bdsphr_sr,
                                     const float seg_msr, const int seg_topo_opt);

void run_seg_adpt_bucketing_2 (vcl_vector<vgl_point_3d<double> >& pts,
                             const int M, const vcl_string prefix,
                             const bool b_check_dup, const float bktbr,
                             const float msr);


void run_seg_adpt_bucketing_3 (const vcl_vector<vgl_point_3d<double> >& input_pts,
                               const int npbkt, const vcl_string& dirprefix,
                               const bool b_check_dup, const float bktbr,
                               const float bdsphr_rr, const float bdsphr_sr, 
                               const float seg_msr, const int seg_topo_opt);

void run_seg_adpt_bucketing_4 (const vcl_vector<vgl_point_3d<double> >& input_pts,
                                  vcl_vector<vcl_vector<int> >& finalM_faces,
                                  const vcl_string& dirprefix,
                                  const int npbkt, const bool b_check_dup, const float bktbr,
                                  const float bdsphr_rr, const int bdsphr_sr,
                                  const float seg_msr, const int seg_topo_opt,
                                  const int n_erode);

void run_seg_cell_bucketing (dbmsh3d_pt_set* pt_set, const vcl_string prefix,
                             const int BUCKET_NX, const int BUCKET_NY, const int BUCKET_NZ,
                             const float msr);

void run_seg_cell_bucketing_2 (dbmsh3d_pt_set* pt_set, const vcl_string prefix,
                               const int BUCKET_NX, const int BUCKET_NY, const int BUCKET_NZ,
                               const float msr);





#endif



