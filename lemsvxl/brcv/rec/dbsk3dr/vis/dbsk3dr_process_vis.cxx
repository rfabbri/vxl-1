//: Ming-Ching Chang
//  16 February 2005

#include <dbmsh3d/vis/dbmsh3d_vis_point_set.h>
#include <dbmsh3d/vis/dbmsh3d_vis_mesh.h>

//: Note the order here!!
#include <dbsk3dr/vis/dbsk3dr_process_vis.h>
#include <dbsk3d/cmdproc/dbsk3d_process_vis.h>

#include <dbsk3dr/vis/dbsk3dr_draw.h>

#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoSeparator.h>

//: Constructor and Destructor
//  Allocate the visualization object.
dbsk3dr_pro_vis::dbsk3dr_pro_vis (dbsk3d_pro_vis* pv0, dbsk3d_pro_vis* pv1) 
    : dbsk3dr_pro (pv0, pv1) 
{
}

dbsk3dr_pro_vis::~dbsk3dr_pro_vis () 
{
}

dbsk3d_pro_vis* dbsk3dr_pro_vis::pv0() 
{
  return (dbsk3d_pro_vis*) pro_[0];
}
  
dbsk3d_pro_vis* dbsk3dr_pro_vis::pv1() 
{
  return (dbsk3d_pro_vis*) pro_[1];
}

//###############################################################

//: Visualize the medial scaffold graph matching results.
SoSeparator* dbsk3dr_pro_vis::vis_sg_match_results (const float shift)
{
  generate_match_color_tables ();

  return draw_sg_match (shock_match_, 
                        pv0()->ms_vis_param_.ball_size_, 
                        pv0()->ms_vis_param_.curve_width_, 
                        true, shift,
                        pv0()->ms_vis_param_.vertices_ctable_,
                        pv1()->ms_vis_param_.vertices_ctable_,
                        pv0()->ms_vis_param_.curves_ctable_,
                        pv1()->ms_vis_param_.curves_ctable_);
}

//: Generate color table (size w.r.t graph size) to visualize matching results.
void dbsk3dr_pro_vis::generate_match_color_tables ()
{
  generate_match_vertices_ctable (1234567, shock_match_,
        pv0()->ms_vis_param_.vertices_ctable_,
        pv1()->ms_vis_param_.vertices_ctable_);

  generate_match_curves_ctable (12345, shock_match_,
        pv0()->ms_vis_param_.curves_ctable_,
        pv1()->ms_vis_param_.curves_ctable_);
}










