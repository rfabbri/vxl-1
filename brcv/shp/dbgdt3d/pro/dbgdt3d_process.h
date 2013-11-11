//: This is lemsvxlsrc/brcv/shp/dbgdt3d/pro/dbgdt3d_process.h
//  Creation: Feb, 12, 2007   Ming-Ching Chang

#ifndef dbgdt3d_process_h_
#define dbgdt3d_process_h_

#include <vcl_string.h>
#include <vul/vul_printf.h>

#include <vgl/algo/vgl_h_matrix_3d.h>

#include <dbul/dbul_dir_file.h>
#include <dbmsh3d/dbmsh3d_mesh.h>
#include <dbmsh3d/algo/dbmsh3d_fmm.h>
#include <dbmsh3d/algo/dbmsh3d_sg3pi_algo.h>
#include <dbmsh3d/algo/dbmsh3d_pt_set_bucket.h>
#include <dbmsh3d/pro/dbmsh3d_cmdpara.h>
#include <dbmsh3d/pro/dbmsh3d_process.h>
#include <dbgdt3d/dbgdt3d_manager.h>


//: The mesh processing class
class dbgdt3d_pro : public dbmsh3d_pro_base
{
protected:

  //meshes of several types  
  dbmsh3d_gdt_mesh*     gdt_mesh_;
  
  gdt_fs_manager*       fs_manager_;
  gdt_f_manager*        f_manager_;
  gdt_i_manager*        i_manager_;
  gdt_ws_manager*       ws_manager_;

public:
  //====== Constructor/Destructor ======
  dbgdt3d_pro () : dbmsh3d_pro_base () {
    gdt_mesh_ = new dbmsh3d_gdt_mesh ();

    fs_manager_ = new gdt_fs_manager (gdt_mesh_, dbmsh3d_cmd_verbose());
    f_manager_ = new gdt_f_manager (gdt_mesh_, dbmsh3d_cmd_verbose());
    i_manager_ = new gdt_i_manager (gdt_mesh_, dbmsh3d_cmd_verbose());
    ws_manager_ = new gdt_ws_manager (gdt_mesh_, dbmsh3d_cmd_verbose());

  }
  ~dbgdt3d_pro () {
    delete gdt_mesh_;

    delete fs_manager_;
    delete f_manager_;
    delete i_manager_;
    delete ws_manager_;
  }

  //====== Data access functions ======

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
  bool load_ply2_gdt (vcl_string dirfile = "");

  //: ====== Processing ======
  void reset_gdt_mesh () {
    delete gdt_mesh_;
    gdt_mesh_ = new dbmsh3d_gdt_mesh ();
  }
};

#endif


