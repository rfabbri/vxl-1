//: This is lemsvxlsrc/brcv/shp/dbskr/vis/dbsk3d_process_vis.h
//  Revision: Dec 27, 2005   Ming-Ching Chang

#ifndef dbsk3dr_process_vis_h_
#define dbsk3dr_process_vis_h_

#include <dbsk3dr/pro/dbsk3dr_process.h>

#include <Inventor/SbColor.h>

class dbsk3d_pro_vis;
class SoSeparator;

class dbsk3dr_pro_vis : public dbsk3dr_pro
{
public:
  //###### Constructor and Destructor ######
  dbsk3dr_pro_vis (dbsk3d_pro_vis* pv0, dbsk3d_pro_vis* pv1);
  ~dbsk3dr_pro_vis ();

  //: Virtual function to access the dual_sets_vis
  dbsk3d_pro_vis* pv0();
  dbsk3d_pro_vis* pv1();

public:
  //###### Processes with visualization ######

  //: Visualize the medial scaffold graph matching results.
  SoSeparator* vis_sg_match_results (const float shift);

  //: Generate color table (size w.r.t graph size) to visualize matching results.
  void generate_match_color_tables ();
};


//#####################################################################
// Smoothing + Meshing + Fusing of Scans

//: Generate run file for smooth.+meshing+registeration+error estim.
void gen_smre_run_files (const vcl_string& f1, const vcl_string& f2,
                         const int smre, const int smreo);

void gen_view_f1_f2_xyz_bat (const vcl_string& f1, const vcl_string& f2);
void gen_view_f1_f2_gsm_xyz_bat (const vcl_string& f1, const vcl_string& f2);
void gen_view_f1_f2_dcs_xyz_bat (const vcl_string& f1, const vcl_string& f2);
void gen_view_f1_f2_gsm_ply_bat (const vcl_string& f1, const vcl_string& f2);
void gen_view_f1_f2_dcs_ply_bat (const vcl_string& f1, const vcl_string& f2);
void gen_view_f1_f2_shift_bat (const vcl_string& f1, const vcl_string& f2,
                               const vcl_string& key);

//#####################################################################

void gen_smreb_bats (const vcl_string& f1, const vcl_string& f2);

#endif
