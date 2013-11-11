#include <vgui/vgui_command.h>
#include <vcl_cstdio.h>
#include <dbsol/dbsol_interp_curve_2d.h>

#include "Lie_contour_match_tableau.h"
#include <manifold_extraction/Lie_cv_cor.h>

#define COLORARRAYSIZE 12

class Lie_contour_match_tableau_change_command : public vgui_command
{
 public:
  Lie_contour_match_tableau_change_command(Lie_contour_match_tableau* tab, 
                                          const void* intref, 
                                          const void* sizeref, 
                                          const int inc) : 
       cvmatch_tableau(tab), 
       interval_ref((int*) intref), 
       increment(inc), 
       size_ref((int *) sizeref) {}

  void execute() 
  { 
    vcl_cout << "interval: " << (*interval_ref) << " increment: " << increment << " size: " << *size_ref;
    int temp = (*interval_ref) + increment;
    vcl_cout << " temp: " << temp;
    if (temp < (*size_ref) && temp > 1) {
      vcl_cout << " temp is within bounds\n";
      (*interval_ref) = temp;
      vcl_cout << " new interval: " << *interval_ref << "\n";
    } else 
      vcl_cout << " temp is NOT within bounds\n";
    
    cvmatch_tableau->post_redraw(); 
  }

  Lie_contour_match_tableau *cvmatch_tableau;
  int* interval_ref;
  int increment;
  int* size_ref;

};

class Lie_contour_match_tableau_toggle_command : public vgui_command
{
 public:
  Lie_contour_match_tableau_toggle_command(Lie_contour_match_tableau* tab, const void* boolref) : 
       cvmatch_tableau(tab), bref((bool*) boolref) {}

  void execute() 
  { 
    *bref = !(*bref);
    cvmatch_tableau->post_redraw(); 
  }

  Lie_contour_match_tableau *cvmatch_tableau;
  bool* bref;
};

Lie_contour_match_tableau::Lie_contour_match_tableau()
{
  gl_mode = GL_RENDER;
  active_tool_sptr = NULL;

  curvematch_ = NULL;

  double ColorArray[][3] = {
    {1,0,0}, {1,0.5,0}, {1,1,0}, {0.5,1,0}, {0,1,0}, {0,1,0.5}, {0,1,1}, {0,0.5,1}, {0,0,1},
    {0.5,0,1}, {1,0,1}, {1,0,0.5}
  };

  ColorArray_.resize(COLORARRAYSIZE, 3);
  for (int i = 0; i<COLORARRAYSIZE; i++)
    for (int j = 0; j<3; j++)
      ColorArray_[i][j] = static_cast<int>(ColorArray[i][j]);

  interval_ = 1;
  draw_matching_lines_ = false;
  offsetx2_ = 100;
  offsety2_ = 100;
  dummy_size_ = 1000;

}

bool Lie_contour_match_tableau::handle( const vgui_event & e )
{
  if( e.type == vgui_DRAW ) {
    if (gl_mode == GL_SELECT)
      draw_select();
    else
      draw_render();
  }

  return false;
}

void Lie_contour_match_tableau::draw_select()
{
  glPushName( 0 ); // will be replaced by ids
  glPopName();
  
}

#define OX +0.5
#define OY +0.5

void Lie_contour_match_tableau::DrawPointMatching (vcl_vector<vgl_point_2d<double> >& pts1, vcl_vector<vgl_point_2d<double> >& pts2, 
                        double OffsetX1, double OffsetY1, double OffsetX2, double OffsetY2, bool show_matching_lines) {

 
  //just to draw the corresponding points with same colors
  if (pts1.size() != pts2.size()) {
    vcl_cout << "Problems in curve correspondences, corresponding point sizes do not match!\n";
    return;
  }

  for (unsigned int i = 0; i<pts1.size(); i++) {
    double pt1x = pts1[i].x()+OffsetX1+OX;
    double pt1y = pts1[i].y()+OffsetY1+OY;
    double pt2x = pts2[i].x()+OffsetX2+OX;
    double pt2y = pts2[i].y()+OffsetY2+OY;
    glColor3f (ColorArray_[i%COLORARRAYSIZE][0],
               ColorArray_[i%COLORARRAYSIZE][1],
               ColorArray_[i%COLORARRAYSIZE][2]);
    glBegin( GL_POINTS );
      glVertex2f( pt1x , pt1y );
      glVertex2f( pt2x , pt2y );
    glEnd();
  }

  if (show_matching_lines) {
    for (unsigned int i=0; i<pts1.size(); i+=interval_) {
      double pt1x = pts1[i].x()+OffsetX1+OX;
      double pt1y = pts1[i].y()+OffsetY1+OY;
      double pt2x = pts2[i].x()+OffsetX2+OX;
      double pt2y = pts2[i].y()+OffsetY2+OY;

      //glColor3f (0.5, 0.5, 0.5);
      glColor3f (ColorArray_[i%COLORARRAYSIZE][0],
              ColorArray_[i%COLORARRAYSIZE][1],
              ColorArray_[i%COLORARRAYSIZE][2]);
        glBegin( GL_LINE_STRIP );
        glVertex2f( pt1x , pt1y );
        glVertex2f( pt2x , pt2y );
      glEnd();
    }
  }
}

void Lie_contour_match_tableau::DrawCurveMatching (dbsol_interp_curve_2d_sptr curve1, 
                dbsol_interp_curve_2d_sptr curve2, FinalMapType* fmap, 
                double OffsetX1, double OffsetY1, double OffsetX2, double OffsetY2,
                bool bShowMatchingLines)
{
  int i;
  int indexP1, indexP2;
  //double ColorArray[][3] = {
  //  {1,0,0}, {1,0.5,0}, {1,1,0}, {0.5,1,0}, {0,1,0}, {0,1,0.5}, {0,1,1}, {0,0.5,1}, {0,0,1},
  //  {0.5,0,1}, {1,0,1}, {1,0,0.5}
  //};

  ///curve1group->eraseAllChildren(); //pushStyle (Hidden(true));
  ///curve2group->eraseAllChildren(); //pushStyle (Hidden(true));
  ///matchgroup->eraseAllChildren(); //pushStyle (Hidden(true));

  //1)Draw points on the curve 1
  for (i=0; i<curve1->size(); i++) {
          glColor3f( 0.0 , 1.0 , 0.0 );
          glPointSize( 3.0 );
          glBegin( GL_POINTS );

          

          glVertex2f (curve1->point_at_sample(i)->x()+OffsetX1+OX, curve1->point_at_sample(i)->y()+OffsetY1+OY);
          // glVertex2f (curve1->x(i)+OffsetX1+OX, curve1->y(i)+OffsetY1+OY);
          glEnd();
  }
  //2)Draw points on the curve 2
  for (i=0; i<curve2->size(); i++) {
          glColor3f( 0.0 , 0.0 , 1.0 );
          glPointSize( 3.0 );
          glBegin( GL_POINTS );
          glVertex2f (curve2->point_at_sample(i)->x()+OffsetX2+OX, curve2->point_at_sample(i)->y()+OffsetY2+OY);
          glEnd();
  }

  
  //5)Draw the matching lines
  if (draw_matching_lines_) {
    for (unsigned int i=0; i<(*fmap).size(); i+=interval_) {
      indexP1 = (*fmap)[i].first;
      indexP2 = (*fmap)[i].second;
      double pt1x = curve1->point_at_sample(indexP1)->x()+OffsetX1+OX;
      double pt1y = curve1->point_at_sample(indexP1)->y()+OffsetY1+OY;
      double pt2x = curve2->point_at_sample(indexP2)->x()+OffsetX2+OX;
      double pt2y = curve2->point_at_sample(indexP2)->y()+OffsetY2+OY;

      //glColor3f (0.5, 0.5, 0.5);
      glColor3f (ColorArray_[i%COLORARRAYSIZE][0],
              ColorArray_[i%COLORARRAYSIZE][1],
              ColorArray_[i%COLORARRAYSIZE][2]);
        glBegin( GL_LINE_STRIP );
        glVertex2f( pt1x , pt1y );
        glVertex2f( pt2x , pt2y );
      glEnd();
    }
  }
}

void Lie_contour_match_tableau::draw_render()
{
  if (cv_cor_.ptr() != NULL) {
    DrawPointMatching (cv_cor_->get_contour_pts1(), cv_cor_->get_contour_pts2(), 
                        0, curvematch_->curve1()->length()/2+ 10, offsetx2_, curvematch_->curve1()->length()/2+ 10, draw_matching_lines_);
  }
  else if (curvematch_.ptr() != NULL) {
      
      
    DrawCurveMatching (curvematch_->curve1(), curvematch_->curve2(), &(curvematch_->finalMap(min_idx_)),
      0, 0, offsetx2_, offsety2_, draw_matching_lines_);
  }
  
}

void Lie_contour_match_tableau::get_popup(const vgui_popup_params& params, vgui_menu &menu)
{
  vgui_menu submenu;
  vcl_string on = "[x] ", off = "[ ] ";
  char buffer[10000];
  vcl_sprintf(buffer, "Current interval: %d ", interval_);
  vcl_string initial = buffer;
  vcl_sprintf(buffer, "Current offsetx2: %d ", offsetx2_);
  vcl_string initialx = buffer;
  vcl_sprintf(buffer, "Current offsety2: %d ", offsety2_);
  vcl_string initialy = buffer;
  
  submenu.add( ((draw_matching_lines_)?on:off)+"Draw Matching Lines", 
               new Lie_contour_match_tableau_toggle_command(this, &draw_matching_lines_));
  submenu.add( (initial+ " draw less"), 
               new Lie_contour_match_tableau_change_command(this, &interval_, &size_, 5));
  submenu.add( (initial+ " draw more"), 
               new Lie_contour_match_tableau_change_command(this, &interval_, &size_, -5));
  submenu.add( (initialx+ " draw right"), 
               new Lie_contour_match_tableau_change_command(this, &offsetx2_, &dummy_size_, 5));
  submenu.add( (initialx+ " draw left"), 
               new Lie_contour_match_tableau_change_command(this, &offsetx2_, &dummy_size_, -5));
  submenu.add( (initialy+ " draw down"), 
               new Lie_contour_match_tableau_change_command(this, &offsety2_, &dummy_size_, 5));
  submenu.add( (initialy+ " draw up"), 
               new Lie_contour_match_tableau_change_command(this, &offsety2_, &dummy_size_, -5));
  

  //add this submenu to the popup menu
  menu.add("Curve Matching Tableau Options", submenu);
}


