//: This is lemsvxlsrc/brcv/shp/dbmsh3d/vis/dbmsh3d_vis_point_set.h
//  MingChing Chang
//  May 03, 2005.

#ifndef _dbmsh3d_vis_point_set_h_
#define _dbmsh3d_vis_point_set_h_

#include <dbmsh3d/dbmsh3d_pt_set.h>

class SoSeparator;
class SbColor;

void draw_ptset_geom (SoSeparator* root, const vcl_vector<vgl_point_3d<double> >& points);
void draw_idpts_geom (SoSeparator* root, const vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& idpts);

void draw_oriented_ptset_geom (SoSeparator* root, 
        const vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_vector_3d<double> > >& ori_pts);

void draw_oriented_ptset_normal_geom (SoSeparator* root, 
        const vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_vector_3d<double> > >& ori_pts,
        const float normal_len);

SoSeparator* draw_ptset (const vcl_vector<vgl_point_3d<double> >& pts, 
                         const int colorcode, const float size = 1.0f);

SoSeparator* draw_idpts (const vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& idpts, 
                         const int colorcode, const float size = 1.0f);

SoSeparator* draw_oriented_ptset (
                const vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_vector_3d<double> > >& ori_pts, 
                const int point_colorcode, const int normal_colorcode, const float normal_len,
                const float point_size = 1.0f, const float normal_width = 1.0f);

// #######################################################################

void draw_pt_set_geom (SoSeparator* root, dbmsh3d_pt_set* pt_set);

void draw_pt_set_geom_unmeshed (SoSeparator* root, dbmsh3d_pt_set* pt_set);

SoSeparator* draw_ptset (dbmsh3d_pt_set* pt_set, 
                         const int colorcode, const float size = 1.0f,
                         const bool only_unmeshed = false);

SoSeparator* draw_ptset_idv (dbmsh3d_pt_set* pt_set, 
                             const int colorcode, const float size = 1.0f,
                             const bool user_defined_class = true);

SoSeparator* draw_pt_set_color (dbmsh3d_pt_set* pt_set,                               
                                const vcl_vector<SbColor>& color_set, 
                                const float size = 1.0f,
                                const bool user_defined_class = true);

bool dbmsh3d_load_g3d (dbmsh3d_pt_set* pt_set, vcl_vector<SbColor>& gene_color_set, const char* file);

bool dbmsh3d_save_g3d (dbmsh3d_pt_set* pt_set, const char* file);
bool dbmsh3d_save_g3d (dbmsh3d_pt_set* pt_set, vcl_vector<SbColor>& gene_color_set, const char* file);

#endif


