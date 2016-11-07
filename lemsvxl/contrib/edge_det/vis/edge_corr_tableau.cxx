// This is vis/edge_corr_tableau.cxx

//:
// \file

#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_cstdio.h>
#include <vcl_limits.h>

#include <vgui/vgui.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_utils.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgl/vgl_distance.h>

#include "edge_corr_tableau.h"
#include <edge_det/pro/edge_corr_storage_sptr.h>
#include <edge_det/pro/edge_corr_storage.h>

class edge_corr_tableau_toggle_command : public vgui_command
{
 public:
  edge_corr_tableau_toggle_command(edge_corr_tableau* tab, const void* boolref) : 
       edgecorr_tab(tab), bref((bool*) boolref) {}

  void execute() 
  { 
    *bref = !(*bref);
    edgecorr_tab->post_redraw(); 
  }

  edge_corr_tableau *edgecorr_tab;
  bool* bref;
};

class edge_corr_tableau_set_int_params_command : public vgui_command
{
 public:
  edge_corr_tableau_set_int_params_command(edge_corr_tableau* tab, 
    const vcl_string& name, const void* intref) : edgecorr_tab(tab),  iref_((int*)intref), name_(name) {}

  void execute() 
  { 
    int param_val = *iref_;
    vgui_dialog param_dlg("Set Display Param");
    param_dlg.field(name_.c_str(), param_val);
    if(!param_dlg.ask())
      return;

    *iref_ = param_val;
    edgecorr_tab->post_redraw(); 
  }

  edge_corr_tableau *edgecorr_tab;
  int* iref_;
  vcl_string name_;
};

class edge_corr_tableau_set_display_params_command : public vgui_command
{
 public:
  edge_corr_tableau_set_display_params_command(edge_corr_tableau* tab, 
    const vcl_string& name, const void* floatref) : edgecorr_tab(tab),  floatref_((float*)floatref), name_(name) {}

  void execute() 
  { 
    float param_val = *floatref_;
    vgui_dialog param_dlg("Set Display Param");
    param_dlg.field(name_.c_str(), param_val);
    if(!param_dlg.ask())
      return;

    *floatref_ = param_val;
    edgecorr_tab->post_redraw(); 
  }

  edge_corr_tableau *edgecorr_tab;
  float* floatref_;
  vcl_string name_;
};

class edge_corr_tableau_set_current_color_command : public vgui_command
{
 public:
  edge_corr_tableau_set_current_color_command(edge_corr_tableau* tab) : edgecorr_tab(tab) {}

  void execute() 
  { 
    vgui_dialog style_dlg("Change Style");
    char TPcolor[50], FPcolor[50], TNcolor[50];
    vcl_sprintf (TPcolor, "%.3f %.3f %.3f", edgecorr_tab->TP_color_[0], 
                                            edgecorr_tab->TP_color_[1], 
                                            edgecorr_tab->TP_color_[2]);
    vcl_sprintf (FPcolor, "%.3f %.3f %.3f", edgecorr_tab->FP_color_[0], 
                                            edgecorr_tab->FP_color_[1], 
                                            edgecorr_tab->FP_color_[2]);
    vcl_sprintf (TNcolor, "%.3f %.3f %.3f", edgecorr_tab->TN_color_[0], 
                                            edgecorr_tab->TN_color_[1], 
                                            edgecorr_tab->TN_color_[2]);

    vcl_string TPcol(TPcolor), FPcol(FPcolor), TNcol(TNcolor);

    float point_size = edgecorr_tab->point_size();
    float line_width = edgecorr_tab->line_width();
    float line_length = edgecorr_tab->line_length();
    style_dlg.inline_color("TP Color", TPcol);
    style_dlg.inline_color("FP Color", FPcol);
    style_dlg.inline_color("TN Color", TNcol);

    style_dlg.field("Point Size", point_size);
    style_dlg.field("Line Width", line_width);
    style_dlg.field("Line Length", line_length);

    if(!style_dlg.ask())
      return;

    vcl_istringstream color_strm1(TPcol), color_strm2(FPcol), color_strm3(TNcol);
    color_strm1 >> TPcolor_[0] >> TPcolor_[1] >> TPcolor_[2];
    color_strm2 >> FPcolor_[0] >> FPcolor_[1] >> FPcolor_[2];
    color_strm3 >> TNcolor_[0] >> TNcolor_[1] >> TNcolor_[2];
    
    edgecorr_tab->set_style(TPcolor_, FPcolor_, TNcolor_, point_size, line_width, line_length);
    edgecorr_tab->post_redraw(); 
  }

  edge_corr_tableau *edgecorr_tab;
  vnl_vector_fixed<float,3> TPcolor_, FPcolor_, TNcolor_;
};

//: Constructor
edge_corr_tableau::edge_corr_tableau(edge_corr_storage_sptr edge_corr): 
  edgemap1_(edge_corr->edge_map1_), 
  edgemap2_(edge_corr->edge_map2_),
  matched1_(edge_corr->match1),
  matched2_(edge_corr->match2),
  display_points_(false),
  point_size_(3.0),
  line_width_(1.0),
  line_length_(1.0),
  TP_color_(0.0,1.0,0.0),
  FP_color_(1.0,0.0,0.0),
  TN_color_(0.0,0.0,1.0),
  threshold1_(0.0),
  threshold2_(0.0),
  left_click_(vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true)),
  cur_edgel(0),
  local_zoom_factor(1),
  disp_correspondence(true)
{
}

edge_corr_tableau::~edge_corr_tableau()
{
}

void edge_corr_tableau::set_style( vnl_vector_fixed<float,3> TPcolor, 
                                   vnl_vector_fixed<float,3> FPcolor, 
                                   vnl_vector_fixed<float,3> TNcolor,
                                   float point_size, float line_width, float line_length)
{
  TP_color_= TPcolor; 
  FP_color_= FPcolor;
  TN_color_= TNcolor;
  point_size_=point_size; 
  line_width_=line_width; 
  line_length_=line_length;
}

bool edge_corr_tableau::handle( const vgui_event & e )
{

  //apply local zoom
  glTranslatef(-0.5, -0.5, 0);
  glScalef(local_zoom_factor, local_zoom_factor, 1);
  glTranslatef(0.5, 0.5, 0);

  if (e.type == vgui_MOTION)
  {
    // Get X,Y position to display on status bar:
    float ix, iy;
    vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

    // Display on status bar:
    vgui::out << "(" << ix << ", " << iy << ")" << vcl_endl;
  }

  ////handle queries
  //if (left_click_(e))
  //{
  //  float ix, iy;
  //  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

  //  // I) Find edgel closest to ix,iy
  //  // a) find the cell that this point belongs to
  //  int xx = dbdet_round(ix);
  //  int yy = dbdet_round(iy);

  //  //vcl_cout << "ix,iy: " << ix << "," << iy << vcl_endl;

  //  //reset cur_edgel
  //  cur_edgel = 0;

  //  // b) find the closest edgel in the neighboring cells
  //  double dmin = vcl_numeric_limits<double>::infinity();
  //  for (int xcell = xx-2; xcell <= xx+2; xcell++){
  //    for (int ycell = yy-2; ycell <= yy+2; ycell++){
  //      if (xcell < 0 || ycell < 0 || xcell >= (int)edgemap_->width() || ycell >= (int)edgemap_->height()) 
  //        continue;

  //      for (unsigned i=0; i<edgemap_->cell(xcell, ycell).size(); i++){
  //        dbdet_edgel* edgel = edgemap_->cell(xcell, ycell)[i];

  //        double dx = edgel->pt.x() - (double)ix; 
  //        double dy = edgel->pt.y() - (double)iy; 
  //        double d = dx*dx+dy*dy;
  //        if (d < dmin){
  //          dmin = d;
  //          cur_edgel = edgel;
  //        }
  //      }
  //    }
  //  }
  //  //output edgel info
  //  if (cur_edgel){
  //    vcl_cout << "Edgel " << cur_edgel->id << " : (x, y, theta, strength) = (";
  //    vcl_cout << cur_edgel->pt.x() << ", " << cur_edgel->pt.y() << ", " ;
  //    vcl_cout << cur_edgel->tangent << ", " << cur_edgel->strength << ")" << vcl_endl; 
  //    vcl_cout << "L Attr: " << cur_edgel->left_app->print_info() << vcl_endl;
  //    vcl_cout << "R Attr: " << cur_edgel->right_app->print_info() << vcl_endl;
  //  }

  //  // hightlight the selected edgel
  //  post_overlay_redraw();

  //  return true;
  //}

  ////actually draw the selected edgel
  //if( e.type == vgui_OVERLAY_DRAW ) 
  //{
  //  //if edgel selected, draw the groups it forms
  //  if (cur_edgel)
  //  {
  //    if (cur_edgel->strength>=threshold_ &&
  //        vcl_fabs(cur_edgel->deriv) >= d2f_thresh_)
  //    {
  //      glColor3f( 1.0-curr_color_[0], 1.0-curr_color_[1], 1.0-curr_color_[2] );
  //      glLineWidth (line_width_);
  //      glBegin( GL_LINE_STRIP );
  //      glVertex2d(cur_edgel->pt.x() - 0.5*line_length_*vcl_cos(cur_edgel->tangent),
  //                 cur_edgel->pt.y() - 0.5*line_length_*vcl_sin(cur_edgel->tangent));
  //      
  //      glVertex2d(cur_edgel->pt.x() + 0.5*line_length_*vcl_cos(cur_edgel->tangent),
  //                 cur_edgel->pt.y() + 0.5*line_length_*vcl_sin(cur_edgel->tangent));
  //      glEnd();
  //    }
  //  }

  //  return true;
  //}

  //handle standard display
  if( e.type == vgui_DRAW )
  {
    draw_edgels();
    return true;
  }

  return false;
}

void edge_corr_tableau::draw_edgels()
{
  //draw edgemap 1
  if( edgemap1_.ptr() == 0 ) 
    return;

  //go over each cell and draw each edgel in the cell
  dbdet_edgemap_const_iter it = edgemap1_->edge_cells.begin();
  for (; it!=edgemap1_->edge_cells.end(); it++){
    for (unsigned j=0; j<(*it).size(); j++){
      dbdet_edgel* e = (*it)[j];

      //apply the selected thresholds
      if (e->strength>=threshold1_)
      {
        if (matched1_[e->id]>=0) //has a match
          glColor3f( TP_color_[0], TP_color_[1], TP_color_[2] );
        else
          glColor3f( FP_color_[0], FP_color_[1], FP_color_[2] );

        glLineWidth (line_width_);
        //gl2psLineWidth(line_width_);
        glBegin( GL_LINE_STRIP );
        glVertex2d(e->pt.x() - 0.5*line_length_*vcl_cos(e->tangent),
                   e->pt.y() - 0.5*line_length_*vcl_sin(e->tangent));
        
        glVertex2d(e->pt.x() + 0.5*line_length_*vcl_cos(e->tangent),
                   e->pt.y() + 0.5*line_length_*vcl_sin(e->tangent));
        glEnd();
      }
    }
  }

  //draw edgemap 2
  if( edgemap2_.ptr() == 0 ) 
    return;

  //go over each cell and draw each edgel in the cell
  dbdet_edgemap_const_iter it2 = edgemap2_->edge_cells.begin();
  for (; it2!=edgemap2_->edge_cells.end(); it2++){
    for (unsigned j=0; j<(*it2).size(); j++){
      dbdet_edgel* e = (*it2)[j];

      //apply the selected thresholds
      if (e->strength>=threshold2_)
      {
        if (matched2_[e->id]>=0) //has a match
          glColor3f( TP_color_[0], TP_color_[1], TP_color_[2] );
        else
          glColor3f( TN_color_[0], TN_color_[1], TN_color_[2] );

        glLineWidth (line_width_);
        //gl2psLineWidth(line_width_);
        glBegin( GL_LINE_STRIP );
        glVertex2d(e->pt.x() - 0.5*line_length_*vcl_cos(e->tangent),
                   e->pt.y() - 0.5*line_length_*vcl_sin(e->tangent));
        
        glVertex2d(e->pt.x() + 0.5*line_length_*vcl_cos(e->tangent),
                   e->pt.y() + 0.5*line_length_*vcl_sin(e->tangent));
        glEnd();
      }
    }
  }

  //display correspondences
  if (disp_correspondence)
  {
    for (unsigned i=0; i<edgemap1_->edgels.size(); i++)
    {
      dbdet_edgel* e1 = edgemap1_->edgels[i];
      if (matched1_[i]>=0)
      {
        dbdet_edgel* e2 = edgemap2_->edgels[matched1_[i]];

        glColor3f(0.8f, 0.8f, 0.8f);
        glLineWidth (1);

        glBegin( GL_LINE_STRIP );
        glVertex2d(e1->pt.x(), e1->pt.y());
        glVertex2d(e2->pt.x(), e2->pt.y());
        glEnd();
      }
    }
  }

}

void 
edge_corr_tableau::get_popup(const vgui_popup_params& /*params*/, vgui_menu &menu)
{
  vgui_menu submenu;
  vcl_string on = "[x] ", off = "[ ] ";

  //submenu.add( "Set threshold 1", new edge_corr_tableau_set_display_params_command(this, "Edge strength", &threshold1_));
  //submenu.add( "Set threshold 2", new edge_corr_tableau_set_display_params_command(this, "Edge strength", &threshold2_));

  //submenu.separator();

  //submenu.add( ((display_points_)?on:off)+"Show Points", 
  //             new edge_corr_tableau_toggle_command(this, &display_points_));

  submenu.add( ((disp_correspondence)?on:off)+ "Show Correspondence",
               new edge_corr_tableau_toggle_command(this, &disp_correspondence));
  
  submenu.add( "Set Style", new edge_corr_tableau_set_current_color_command(this));

  submenu.separator();

  submenu.add( "Set Local Zoom Factor", new edge_corr_tableau_set_int_params_command(this, "Zoom Factor", &local_zoom_factor));

  //add this submenu to the popup menu
  menu.add("Tableau Options", submenu);
}
