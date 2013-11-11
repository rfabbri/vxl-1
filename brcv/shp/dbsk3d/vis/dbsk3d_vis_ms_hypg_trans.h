//: This is dbsk3d/vis/dbsk3d_vis_ms_hypg_trans.h
//  Visualization of the medial scaffold transitions.
//  Nov 30, 2004
//  MingChing Chang

#ifndef dbsk3d_vis_ms_hypg_trans_h_
#define dbsk3d_vis_ms_hypg_trans_h_

#include <dbsk3d/dbsk3d_ms_hypg.h>
#include <dbsk3d/algo/dbsk3d_ms_hypg_trans.h>

class SbColor;
class SoSeparator;

SoSeparator* vis_SH_elms_n_SCxforms (dbsk3d_ms_hypg* ms_hypg, const int n_xform_th, 
                                     const float curve_width, 
                                     const float r, const bool show_id);

SoSeparator* vis_A5_curve_contract_xform (dbsk3d_ms_hypg* ms_hypg, int MCid, 
                                          const float curve_width, const float r,
                                          const bool show_id);

SoSeparator* vis_A12A3I_curve_contract_xform (dbsk3d_ms_hypg* ms_hypg, int MCid, 
                                              const float curve_width, const float r,
                                              const bool show_id);

SoSeparator* vis_all_merge_xforms (dbsk3d_ms_hypg_trans* mht, 
                                   const int tab_th, const int A5_th,
                                   const int cc_th, const int sc_th, 
                                   const int nnm_th, const int ncm_th, const int ccm_th,
                                   const int ncm_a1a5_th, const int cmxth, 
                                   const float curve_width, const float ball_size);

SoSeparator* vis_A1A3II_n_n_merge_xform (dbsk3d_ms_node* MN1, dbsk3d_ms_node* MN2, 
                                         const float cost, const float curve_width);

SoSeparator* vis_A12A3I_n_c_merge_xform (dbsk3d_ms_node* MN1, dbsk3d_ms_curve* MC2, 
                                         const float cost, const float cmxth,
                                         const float curve_width, const float ball_size);

SoSeparator* vis_A14_c_c_merge_xform (dbsk3d_ms_curve* MC1, dbsk3d_ms_curve* MC2, 
                                      const float cost, const float cmxth,
                                      const float curve_width, const float cube_size);

SoSeparator* vis_A1A3II_c_c_merge_xform (dbsk3d_ms_curve* MC1, dbsk3d_ms_curve* MC2, 
                                         const float cost, const float cmxth,
                                         const float curve_width, const float cube_size);

SoSeparator* vis_A1A5_n_c_merge_xform (dbsk3d_ms_node* MN1, dbsk3d_ms_curve* MC2, 
                                       const float cost, const float cmxth,
                                       const float curve_width, const float cube_size);

#endif
