//: This is dbsk3dr/vis/dbsk3dr_draw.h
//  Nov 30, 2004
//  MingChing Chang

#ifndef dbsk3dr_draw_h_
#define dbsk3dr_draw_h_

#include <vcl_vector.h>

class dbsk3dr_match;
class SoSeparator;
class SbColor;

void generate_match_vertices_ctable (unsigned long seed, 
                                     dbsk3dr_match* SM, 
                                     vcl_vector<SbColor>& color_table1,
                                     vcl_vector<SbColor>& color_table2);

void generate_match_curves_ctable (unsigned long seed, 
                                   dbsk3dr_match* SM, 
                                   vcl_vector<SbColor>& color_table1,
                                   vcl_vector<SbColor>& color_table2);

SoSeparator* draw_sg_match (dbsk3dr_match* SM, 
                            const float fBallRadius, const float curve_width,
                            const bool showid, const float shift, 
                            const vcl_vector<SbColor>& v_color_table1,
                            const vcl_vector<SbColor>& v_color_table2, 
                            const vcl_vector<SbColor>& c_color_table1,
                            const vcl_vector<SbColor>& c_color_table2);

SoSeparator* draw_match_MC_alignment (dbsk3d_ms_curve* MC1, dbsk3d_ms_curve* MC2, 
                                      vcl_vector< vcl_pair<int,int> >& alignment);

#endif
