//: This is lemsvxlsrc/brcv/shp/dbmsh3d/pro/dbmsh3d_process.h
//  Creation: Feb, 12, 2007   Ming-Ching Chang

#ifndef dbmsh3d_process_h_
#define dbmsh3d_process_h_

#include <vcl_string.h>
#include <vul/vul_printf.h>

#include <vgl/algo/vgl_h_matrix_3d.h>

#include <dbul/dbul_dir_file.h>
#include <dbmsh3d/dbmsh3d_mesh.h>
#include <dbmsh3d/algo/dbmsh3d_fmm.h>
#include <dbmsh3d/algo/dbmsh3d_sg3pi_algo.h>
#include <dbmsh3d/algo/dbmsh3d_pt_set_bucket.h>
#include <dbmsh3d/pro/dbmsh3d_cmdpara.h>
#include <gdt/gdt_manager.h>

class dbmsh3d_pro_base
{
public:  
  typedef enum {
    BOGUS_PRO_DATA  = '?',
    PD_MESH = 'M',            //Use dbmsh3d_mesh data
    PD_PTS = 'P',             //Use <point> data
    PD_IDPTS = 'I',           //Use <id, point> data
    PD_ORIPTS = 'N',          //Use <point, normal> data
    PD_SG3PI = 'S',           //Use dbmsh3d_sg3pi data
  } PRO_DATA;

protected:
  //: File directory, prefix, and filename.
  //  For the input of "abc\def\prefix-ghi-jkl-mno.txt"
  //    dir            "abc\def"
  //    prefix         "prefix"
  //    dir_prefix     "abc\def\prefix"
  //    dir_file       "abc\def\prefix-ghi-jkl-mno"
  //    file           "prefix-ghi-jkl-mno"
  //    suffix         "txt"
  vcl_string      dir_file_;
  vcl_string      dir_prefix_;

  PRO_DATA        pro_data_;

  //: Point cloud or mesh or several types.
  vcl_vector<vgl_point_3d<double> > pts_;
  
  //: The efficient vertex data structure 
  //  for Indexed-face-set (IFS) visualization, File I/O, etc.
  vcl_vector<vcl_pair<int, vgl_point_3d<double> > > idpts_;

  vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_vector_3d<double> > > oripts_;

  vcl_vector<vcl_vector<int> > ifs_faces_;

  dbmsh3d_sg3pi*  sg3pi_;
  dbmsh3d_mesh*   mesh_;

  //: store the alignment matrix.
  vgl_h_matrix_3d<double> hmatrix_;

  float avg_samp_dist_;

public:
  //###### Constructor/Destructor ######
  dbmsh3d_pro_base () {
    dir_file_ = "";
    dir_prefix_ = "";
    pro_data_ = PD_MESH;  //Use dbmsh3d_mesh as default.

    sg3pi_ = new dbmsh3d_sg3pi ();
    mesh_ = new dbmsh3d_mesh ();

    hmatrix_.set_identity ();
    avg_samp_dist_ = -1.0f;
  }
  ~dbmsh3d_pro_base () {
    pts_.clear();
    idpts_.clear();
    oripts_.clear();
    ifs_faces_.clear();
    delete sg3pi_;
    delete mesh_;
  }

  //###### Data access functions ######
  vcl_string dir_prefix() const {
    return dir_prefix_;
  }
  vcl_string dir_file() const {
    return dir_file_;
  }
  vcl_string dir() const {
    return dbul_get_dir (dir_prefix_);
  }
  void set_dir_prefix (const vcl_string& str) {
    dir_prefix_ = str;
  }
  void set_dir_file (const vcl_string& str) {
    dir_file_ = str;
    dir_prefix_ = dbul_get_dir_prefix (str);
  }

  PRO_DATA pro_data() {
    return pro_data_;
  }
  void set_pro_data (const PRO_DATA data) {
    pro_data_ = data;
  }

  vcl_vector<vgl_point_3d<double> >& pts() {
    return pts_;
  }
  vgl_point_3d<double> pts (const int i) const {
    return pts_[i];
  }

  vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& idpts() {
    return idpts_;
  }
  const vcl_pair<int, vgl_point_3d<double> >& idpts (const unsigned int i) const {
    return idpts_[i];
  }
  vcl_pair<int, vgl_point_3d<double> >& idpts (const unsigned int i) {
    return idpts_[i];
  }

  vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_vector_3d<double> > >& oripts() {
    return oripts_;
  }
  vcl_pair<vgl_point_3d<double>, vgl_vector_3d<double> > oripts (const int i) const {
    return oripts_[i];
  }

  vcl_vector<vcl_vector<int> >& ifs_faces() {
    return ifs_faces_;
  }
  int ifs_faces (const int i, const int j) {
    return ifs_faces_[i][j];
  }


  dbmsh3d_sg3pi* sg3pi () {
    return sg3pi_;
  }
  dbmsh3d_mesh* mesh () {
    return mesh_;
  }
  void set_mesh (dbmsh3d_mesh* mesh) {
    mesh_ = mesh;
  }
  
  vgl_h_matrix_3d<double>& hmatrix() {
    return hmatrix_;
  }
  void set_hmatrix (const vgl_h_matrix_3d<double>& hmatrix) {
    hmatrix_ = hmatrix;
  }

  double get_avg_samp_dist ();
  
  //###### Reset the Data Object ######
  void reset_data () {
    if (pro_data_ == PD_PTS)
      pts_.clear();
    else if (pro_data_ == PD_IDPTS)
      idpts_.clear();
    else if (pro_data_ == PD_ORIPTS)
      oripts_.clear();
    else if (pro_data_ == PD_SG3PI)
      reset_sg3pi ();
    else if (pro_data_ == PD_MESH)
      reset_mesh ();
    else 
      assert (0);
  }
  void reset_sg3pi () {
    delete sg3pi_;
    sg3pi_ = new dbmsh3d_sg3pi ();
  }
  virtual void reset_mesh () {    
    delete mesh_;
    mesh_ = new dbmsh3d_mesh ();
  }

  //###### Transfer Data between Objects ######
  void move_pts_to_mesh ();
  void move_mesh_to_pts ();
  void move_sg3pi_to_pts ();
  void move_sg3pi_to_mesh ();

  void _copy_oripts_to_pts ();
  void _copy_pts_to_mesh ();
  void _copy_mesh_to_pts ();
  void _copy_mesh_to_ifs_faces ();
  void _copy_sg3pi_to_pts ();
  void _copy_sg3pi_to_mesh ();

  void _copy_valid_V_to_pts (const bool v);

  //###### File I/O ######
  //: Load input points from various input file types.
  bool load_points_files (vcl_string dirfile = "");

  //: Load input faces (already meshed) from various file types.
  bool load_faces_files (vcl_string dirfile = "");

  //: Load input meshe from various file types.
  bool load_meshes_files (vcl_string dirfile = "");
  
  void save_xyz (vcl_string dirfile = "");
  bool load_xyz (vcl_string dirfile = "");

  void save_p3d (vcl_string dirfile = "");
  bool load_p3d (vcl_string dirfile = "");

  void save_xyzn1 (vcl_string dirfile = "");
  bool load_xyzn1 (vcl_string dirfile = "");
  
  ///void save_xyznw (vcl_string dirfile = "");
  ///bool load_xyznw (vcl_string dirfile = "");

  void save_3pi (vcl_string dirfile = "");
  bool load_3pi (vcl_string dirfile = "");

  void save_ply (vcl_string dirfile = "", const bool ascii_mode = true);
  bool load_ply (vcl_string dirfile = "");
  bool load_ply_v (vcl_string dirfile = "");
  bool load_ply_f (vcl_string dirfile = "");

  void save_ply2 (vcl_string dirfile = "");
  bool load_ply2 (vcl_string dirfile = "", const bool IFS_to_MHE = false);
  bool load_ply2_v (vcl_string dirfile = "");
  bool load_ply2_f (vcl_string dirfile = "");

  void save_off (vcl_string dirfile = "");
  bool load_off (vcl_string dirfile = "");

  void save_obj (vcl_string dirfile = "");
  bool load_obj (vcl_string dirfile = "");

  void save_iv_ifs (vcl_string dirfile = "");
  bool load_iv_ifs (vcl_string dirfile = "");

  void save_m (vcl_string dirfile = "");
  bool load_m (vcl_string dirfile = "");

  //###### Alignment, affine transformation ######
  bool is_hmatrix_identity () const {
    for (unsigned int r=0; r<4; r++) {
      for (unsigned int c=0; c<4; c++) {
        if (r==c) {
          if (hmatrix_.get(r,c) != 1)
            return false;
        }
        else {
          if (hmatrix_.get(r,c) != 0)
            return false;
        }
      }
    }
    return true;
  }

  //: Fix orientation of Polywork alignment matrix.
  void fix_hmatrix_orient ();

  virtual void apply_xform_hmatrix ();

  bool load_hmatrix (vcl_string dirfile = "");

  //###### Virtual Visualization Functions ######
};


//: The mesh processing class
class dbmsh3d_pro : public dbmsh3d_pro_base
{
protected:

  //meshes of several types  
  dbmsh3d_fmm_mesh*     fmm_mesh_;
  dbmsh3d_gdt_mesh*     gdt_mesh_;
  
  gdt_fs_manager*       fs_manager_;
  gdt_f_manager*        f_manager_;
  gdt_i_manager*        i_manager_;
  gdt_ws_manager*       ws_manager_;

public:
  bool        b_view_p3d_;
  bool        b_view_g3d_;
  bool        b_view_xyz_;
  bool        b_view_xyzn1_;
  bool        b_view_xyznw_;
  bool        b_view_ply_;
  bool        b_view_ply2_;
  bool        b_view_off_;
  bool        b_view_obj_;
  bool        b_view_m_;
  bool        b_view_3pi_;
  bool        b_view_iv_;
  bool        b_view_wrl_;

  //====== Constructor/Destructor ======
  dbmsh3d_pro () : dbmsh3d_pro_base () {
    fmm_mesh_ = new dbmsh3d_fmm_mesh ();
    gdt_mesh_ = new dbmsh3d_gdt_mesh ();

    fs_manager_ = new gdt_fs_manager (gdt_mesh_, dbmsh3d_cmd_verbose());
    f_manager_ = new gdt_f_manager (gdt_mesh_, dbmsh3d_cmd_verbose());
    i_manager_ = new gdt_i_manager (gdt_mesh_, dbmsh3d_cmd_verbose());
    ws_manager_ = new gdt_ws_manager (gdt_mesh_, dbmsh3d_cmd_verbose());

    b_view_p3d_ = false;
    b_view_g3d_ = false;
    b_view_xyz_ = false;
    b_view_xyzn1_ = false;
    b_view_xyznw_ = false;
    b_view_ply_ = false;
    b_view_ply2_ = false;
    b_view_off_ = false;
    b_view_obj_ = false;
    b_view_m_ = false;
    b_view_3pi_ = false;
    b_view_iv_ = false;
    b_view_wrl_ = false;
  }
  ~dbmsh3d_pro () {
    delete fmm_mesh_;
    delete gdt_mesh_;

    delete fs_manager_;
    delete f_manager_;
    delete i_manager_;
    delete ws_manager_;
  }

  //====== Data access functions ======

  dbmsh3d_fmm_mesh* fmm_mesh () {
    return fmm_mesh_;
  }
  void set_fmm_mesh (dbmsh3d_fmm_mesh* fmm_mesh) {
    fmm_mesh_ = fmm_mesh;
  }

  dbmsh3d_gdt_mesh* gdt_mesh () {
    return gdt_mesh_;
  }
  void set_gdt_mesh (dbmsh3d_gdt_mesh* gdt_mesh) {
    gdt_mesh_ = gdt_mesh;
  }

  gdt_fs_manager* fs_manager() {
    return fs_manager_;
  }
  void set_fs_manager (gdt_fs_manager* manager) {
    fs_manager_ = manager;
  }
  gdt_f_manager* f_manager() {
    return f_manager_;
  }
  void set_f_manager (gdt_f_manager* manager) {
    f_manager_ = manager;
  }
  gdt_i_manager* i_manager() {
    return i_manager_;
  }
  void set_i_manager (gdt_i_manager* manager) {
    i_manager_ = manager;
  }
  gdt_ws_manager* ws_manager() {
    return ws_manager_;
  }
  void set_ws_manager (gdt_ws_manager* manager) {
    ws_manager_ = manager;
  }  

  //: ====== File I/O ======
  bool load_ply2_fmm (vcl_string dirfile = "");
  bool load_ply2_gdt (vcl_string dirfile = "");

  //: ====== Processing ======

  void reset_fmm_mesh () {
    delete fmm_mesh_;
    fmm_mesh_ = new dbmsh3d_fmm_mesh ();
  }
  void reset_gdt_mesh () {
    delete gdt_mesh_;
    gdt_mesh_ = new dbmsh3d_gdt_mesh ();
  }
  
  //  return true if successful.
  bool sample_iv_to_pts (int opt, float th, float noise);

  void process_pts (const float tx, const float ty, const float tz, 
                    const float rx, const float ry, const float rz,
                    const float scale, const bool shift_1stoct, 
                    const bool checkdup, const int subsam_pts);

};

#endif


