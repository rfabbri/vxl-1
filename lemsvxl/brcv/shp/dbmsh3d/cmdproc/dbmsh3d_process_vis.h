//: This is lemsvxlsrc/brcv/shp/dbmsh3d/vis/dbmsh3d_process_vis.h
//  Creation: Feb, 12, 2007   Ming-Ching Chang

#ifndef dbmsh3d_process_vis_h_
#define dbmsh3d_process_vis_h_

#include <vcl_string.h>
#include <dbmsh3d/pro/dbmsh3d_process.h>

#include <Inventor/SbColor.h>
class SoSeparator;

class dbmsh3d_vis_base
{
public:
  //: Draw object using user defined class to enable click and get info.
  bool  user_defined_class_;
  //: Drawing background color code.
  int   bg_ccode_;
  //: Drawing point size.
  float pt_size_;
  //: Drawing cube size.
  float cube_size_;
  //: Drawing mesh transparency.
  float m_transp_;
  //: drawing mesh vertex color code.
  int   m_vertex_ccode_;
  //: drawing mesh vertex color code.
  int   m_face_ccode_;
  //: drawing mesh edge width.
  float m_edge_width_;
  //: drawing mesh boundary width.
  float m_bndcv_width_;
  //: drawing vector length.
  float vector_len_;

  //: The set of mesh vertex colors.
  vcl_vector<SbColor> mesh_vertex_color_set_;
  //: The set of mesh face colors.
  vcl_vector<SbColor> mesh_face_color_set_;

public:
  //###### Constructor/Destructor ######
  dbmsh3d_vis_base () {
    user_defined_class_ = true; 
    bg_ccode_ = COLOR_WHITE;
    pt_size_ = 1.0f;
    cube_size_ = 0.01f;  
    m_transp_ = 0.0f;   
    m_vertex_ccode_ = COLOR_BLACK;
    m_face_ccode_ = COLOR_CYAN;
    m_edge_width_ = 1.0f;
    m_bndcv_width_ = 3.0f;
    vector_len_ = 0.01f;  

    mesh_vertex_color_set_.clear();
    mesh_face_color_set_.clear();
  }
  //###### Visualization Functions ######
  virtual SoSeparator* vis_ptset (const int colorcode = COLOR_NOT_SPECIFIED, 
                                  const bool b_draw_idv = false) =0;
  virtual SoSeparator* vis_ptset_idv () =0;
  virtual SoSeparator* vis_ptset_color () =0;

  virtual SoSeparator* vis_mesh (const bool b_draw_individual, 
                                 const bool b_shape_hints = false,
                                 const int colorcode = 0,
                                 const bool b_draw_isolated_pts = true) =0;
  virtual SoSeparator* vis_mesh_edges (const int colorcode, const float width = 1.0f, 
                                       const bool draw_idv = false) =0;
  virtual SoSeparator* vis_mesh_pts_edges (const int colorcode = COLOR_NOT_SPECIFIED,
                                           const bool draw_idv = false,
                                           const float pt_size = 3.0f) =0;

  virtual SoSeparator* vis_mesh_bndcurve (const int colorcode, const float width) =0;
  virtual SoSeparator* vis_mesh_color () =0;
  virtual SoSeparator* vis_mesh_options (int option, const bool draw_idv = false, const bool showid = false) =0;
  virtual SoSeparator* vis_mesh_anim (const int nF_batch) =0;
};

//: The mesh processing & visualization class
class dbmsh3d_pro_vis : public dbmsh3d_pro, public dbmsh3d_vis_base
{
public:
  //###### Constructor/Destructor ######
  dbmsh3d_pro_vis() : dbmsh3d_pro(), dbmsh3d_vis_base() {

  }
  virtual ~dbmsh3d_pro_vis () {
  }
  
  //###### Visualization Functions in dbmsh3d_vis_base ######
  virtual SoSeparator* vis_ptset (const int colorcode = COLOR_NOT_SPECIFIED, 
                          const bool b_draw_idv = false);
  virtual SoSeparator* vis_ptset_idv ();
  virtual SoSeparator* vis_ptset_color ();

  // todo: re-organize the three functions.
  virtual SoSeparator* vis_mesh (const bool b_draw_individual, 
                         const bool b_shape_hints = false,
                         const int colorcode = 0,
                         const bool b_draw_isolated_pts = true);
  virtual SoSeparator* vis_mesh_edges (const int colorcode, const float width = 1.0f,
                                       const bool draw_idv = false);

  virtual SoSeparator* vis_mesh_pts_edges (const int colorcode = COLOR_NOT_SPECIFIED,
                                           const bool draw_idv = false,
                                           const float pt_size = 3.0f);

  virtual SoSeparator* vis_mesh_bndcurve (const int colorcode, const float width);
  virtual SoSeparator* vis_mesh_color ();
  virtual SoSeparator* vis_mesh_options (int option, const bool draw_idv = false, const bool showid = false);
  virtual SoSeparator* vis_mesh_anim (const int nF_batch);  

  //###### Visualization Functions ######
  //: Compute mesh vertex color by the value. Result in mesh_vertex_color_set_[]
  void compute_mesh_vertex_color_set (const vcl_vector<double>& values);

  //: Compute mesh face color by value using mesh_vertex_color_set_[].
  //  Result in mesh_face_color_set_[]
  void compute_face_color_by_value (const vcl_vector<double>& values);

  //###### File Visualization ######  
  bool load_g3d (vcl_string filename = "");
  void save_g3d (vcl_string filename = "");

  SoSeparator* view_xyz (vcl_string filename = "", const int colorcode = 0);
  SoSeparator* view_p3d (vcl_string filename = "", const int colorcode = 0);
  SoSeparator* view_g3d (vcl_string filename = "");

  SoSeparator* view_ply (vcl_string filename = "", bool b_shape_hints = false, 
                         const int colorcode = 0,
                         const bool b_draw_isolated_pts = true);
  SoSeparator* view_ply2 (vcl_string filename = "", bool b_shape_hints = false, 
                          const int colorcode = 0,
                         const bool b_draw_isolated_pts = true);

  SoSeparator* view_iv (vcl_string filename = "");
  SoSeparator* view_wrl (vcl_string filename = "");

  //Read the list file and view each file.
  SoSeparator* vis_list_file (vcl_string filename, const int opion, const int vis_opion);
};



#endif


