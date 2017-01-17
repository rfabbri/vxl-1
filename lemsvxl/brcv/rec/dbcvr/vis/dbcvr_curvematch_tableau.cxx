/**************************************************************/
/*  Name: MingChing (mcchang)
//  File: dbgui_curvematch_tableau.h
//  Asgn: bvis
//  Date: Thu Aug 28 14:28:55 EDT 2003
***************************************************************/
#include <vgui/vgui_command.h>
#include <vcl_cstdio.h>

#include "dbcvr_curvematch_tableau.h"
#include <dbcvr/dbcvr_cv_cor.h>
#include <vsol/vsol_polygon_2d.h>
#define COLORARRAYSIZE 12

class dbcvr_curvematch_tableau_change_command : public vgui_command
{
 public:
  dbcvr_curvematch_tableau_change_command(dbcvr_curvematch_tableau* tab, 
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

  dbcvr_curvematch_tableau *cvmatch_tableau;
  int* interval_ref;
  int increment;
  int* size_ref;

};

class dbcvr_curvematch_tableau_toggle_command : public vgui_command
{
 public:
  dbcvr_curvematch_tableau_toggle_command(dbcvr_curvematch_tableau* tab, const void* boolref) : 
       cvmatch_tableau(tab), bref((bool*) boolref) {}

  void execute() 
  { 
    *bref = !(*bref);
    cvmatch_tableau->post_redraw(); 
  }

  dbcvr_curvematch_tableau *cvmatch_tableau;
  bool* bref;
};

dbcvr_curvematch_tableau::dbcvr_curvematch_tableau()
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
  draw_matching_lines_ = true;
  offsetx2_ = 50;
  offsety2_ = 50;
  dummy_size_ = 1000;

}

bool dbcvr_curvematch_tableau::handle( const vgui_event & e )
{
  if( e.type == vgui_DRAW ) {
    if (gl_mode == GL_SELECT)
      draw_select();
    else
      draw_render();
  }

  return false;
}

void dbcvr_curvematch_tableau::draw_select()
{
  glPushName( 0 ); // will be replaced by ids
  glPopName();
  
}

#define OX +0.5
#define OY +0.5

void dbcvr_curvematch_tableau::DrawPointMatching (vcl_vector<vgl_point_2d<double> >& pts1, vcl_vector<vgl_point_2d<double> >& pts2, 
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

      
        
        if (i == pts1.size()-1) {
          glColor3f (0.0, 0.0, 0.0);
          glLineWidth(8.0);

        } else {
          glColor3f (ColorArray_[i%COLORARRAYSIZE][0],
              ColorArray_[i%COLORARRAYSIZE][1],
              ColorArray_[i%COLORARRAYSIZE][2]);
          glLineWidth(3.0);
        }
        glBegin( GL_LINE_STRIP );

        glVertex2f( pt1x , pt1y );
        glVertex2f( pt2x , pt2y );
      glEnd();
    }
  }
}

void dbcvr_curvematch_tableau::DrawCurves(vcl_vector<vsol_point_2d_sptr>& curve1_pts, 
                                          vcl_vector<vsol_point_2d_sptr>& curve2_pts,
                                          double OffsetX1, double OffsetY1, double OffsetX2, double OffsetY2, bool open_cv_match)
{
  //1)Draw points on the curve 1
  glColor3f( 0.0 , 1.0 , 0.0 );
  glLineWidth( 3.0 );
  glBegin( GL_LINE_STRIP );
  for (unsigned i=1; i<curve1_pts.size(); i++) {
    glVertex2f (curve1_pts[i-1]->x()+OffsetX1+OX, curve1_pts[i-1]->y()+OffsetY1+OY);
    glVertex2f (curve1_pts[i]->x()+OffsetX1+OX, curve1_pts[i]->y()+OffsetY1+OY);
          
  }
  if (!open_cv_match && curve1_pts.size() > 0) {
    glVertex2f (curve1_pts[0]->x()+OffsetX1+OX, curve1_pts[0]->y()+OffsetY1+OY);
  }

  glEnd();

  //2)Draw points on the curve 2
  glColor3f( 0.0 , 0.0 , 1.0 );
  glLineWidth( 3.0 );
  glBegin( GL_LINE_STRIP );
  for (unsigned i=1; i<curve2_pts.size(); i++) {
    glVertex2f (curve2_pts[i-1]->x()+OffsetX2+OX, curve2_pts[i-1]->y()+OffsetY2+OY);
    glVertex2f (curve2_pts[i]->x()+OffsetX2+OX, curve2_pts[i]->y()+OffsetY2+OY);
          
  }
  if (!open_cv_match && curve2_pts.size() > 0) {
    glVertex2f (curve2_pts[0]->x()+OffsetX2+OX, curve2_pts[0]->y()+OffsetY2+OY);
  }

  glEnd();

}

void dbcvr_curvematch_tableau::DrawCurveMatching (bsol_intrinsic_curve_2d_sptr curve1, 
                bsol_intrinsic_curve_2d_sptr curve2, FinalMapType* fmap, 
                double OffsetX1, double OffsetY1, double OffsetX2, double OffsetY2,
                bool bShowMatchingLines)
{
  //int i;
  int indexP1, indexP2;
  //double ColorArray[][3] = {
  //  {1,0,0}, {1,0.5,0}, {1,1,0}, {0.5,1,0}, {0,1,0}, {0,1,0.5}, {0,1,1}, {0,0.5,1}, {0,0,1},
  //  {0.5,0,1}, {1,0,1}, {1,0,0.5}
  //};

  ///curve1group->eraseAllChildren(); //pushStyle (Hidden(true));
  ///curve2group->eraseAllChildren(); //pushStyle (Hidden(true));
  ///matchgroup->eraseAllChildren(); //pushStyle (Hidden(true));

  

  
/*
  //3)Draw the matching on curve 1
  int size = (*fmap).size();
  int prev_index1 = (*fmap)[size-1].first;

  for (i=size-2; i>=0; i--) {
    int index1 = (*fmap)[i].first;

    int step = index1 - prev_index1;
    if (step==1) {
      //Matching line on curve 1
      double pt1x = curve1->x(prev_index1)+OffsetX1+OX;
      double pt1y = curve1->y(prev_index1)+OffsetY1+OY;
      double pt2x = curve1->x(index1)+OffsetX1+OX;
      double pt2y = curve1->y(index1)+OffsetY1+OY;

      glColor3f (ColorArray[index1%COLORARRAYSIZE][0],
              ColorArray[index1%COLORARRAYSIZE][1],
              ColorArray[index1%COLORARRAYSIZE][2]);
        glBegin( GL_LINE_STRIP );
        glVertex2f( pt1x , pt1y );
        glVertex2f( pt2x , pt2y );
      glEnd();
    }
    else if (step==2) {
      int indexm = prev_index1+1;
      double pt1x = curve1->x(prev_index1)+OffsetX1+OX;
      double pt1y = curve1->y(prev_index1)+OffsetY1+OY;
      double pt2x = curve1->x(indexm)+OffsetX1+OX;
      double pt2y = curve1->y(indexm)+OffsetY1+OY;
      double pt3x = curve1->x(index1)+OffsetX1+OX;
      double pt3y = curve1->y(index1)+OffsetY1+OY;

      glColor3f (ColorArray[index1%COLORARRAYSIZE][0],
              ColorArray[index1%COLORARRAYSIZE][1],
              ColorArray[index1%COLORARRAYSIZE][2]);
        glBegin( GL_LINE_STRIP );
        glVertex2f( pt1x , pt1y );
        glVertex2f( pt2x , pt2y );
        glVertex2f( pt3x , pt3y );
      glEnd();
    }
    else if (step==3) {
      int index2 = prev_index1+1;
      int index3 = prev_index1+2;
      double pt1x = curve1->x(prev_index1)+OffsetX1+OX;
      double pt1y = curve1->y(prev_index1)+OffsetY1+OY;
      double pt2x = curve1->x(index2)+OffsetX1+OX;
      double pt2y = curve1->y(index2)+OffsetY1+OY;
      double pt3x = curve1->x(index3)+OffsetX1+OX;
      double pt3y = curve1->y(index3)+OffsetY1+OY;
      //double pt4x = curve1->x(index1)+OffsetX1+OX;
      //double pt4y = curve1->y(index1)+OffsetY1+OY;

      glColor3f (ColorArray[index1%COLORARRAYSIZE][0],
              ColorArray[index1%COLORARRAYSIZE][1],
              ColorArray[index1%COLORARRAYSIZE][2]);
        glBegin( GL_LINE_STRIP );
        glVertex2f( pt1x , pt1y );
        glVertex2f( pt2x , pt2y );
        glVertex2f( pt3x , pt3y );
      glEnd();
    }

    prev_index1 = index1;
  }

  //4)Draw the matching on curve 2
  int prev_index2 = (*fmap)[size-1].second;

  for (i=size-2; i>=0; i--) {
    int index1 = (*fmap)[i].first;
    int index2 = (*fmap)[i].second;

    int step = index2 - prev_index2;
    if (step==1) {
      double pt1x = curve2->x(prev_index2)+OffsetX2+OX;
      double pt1y = curve2->y(prev_index2)+OffsetY2+OY;
      double pt2x = curve2->x(index2)+OffsetX2+OX;
      double pt2y = curve2->y(index2)+OffsetY2+OY;

      glColor3f (ColorArray[index1%COLORARRAYSIZE][0],
              ColorArray[index1%COLORARRAYSIZE][1],
              ColorArray[index1%COLORARRAYSIZE][2]);
        glBegin( GL_LINE_STRIP );
        glVertex2f( pt1x , pt1y );
        glVertex2f( pt2x , pt2y );
      glEnd();
    }
    else if (step==2) {
      int indexm = prev_index2+1;
      double pt1x = curve2->x(prev_index2)+OffsetX2+OX;
      double pt1y = curve2->y(prev_index2)+OffsetY2+OY;
      double pt2x = curve2->x(indexm)+OffsetX2+OX;
      double pt2y = curve2->y(indexm)+OffsetY2+OY;
      double pt3x = curve2->x(index2)+OffsetX2+OX;
      double pt3y = curve2->y(index2)+OffsetY2+OY;

      glColor3f (ColorArray[index1%COLORARRAYSIZE][0],
              ColorArray[index1%COLORARRAYSIZE][1],
              ColorArray[index1%COLORARRAYSIZE][2]);
        glBegin( GL_LINE_STRIP );
        glVertex2f( pt1x , pt1y );
        glVertex2f( pt2x , pt2y );
        glVertex2f( pt3x , pt3y );
      glEnd();
    }
    else if (step==3) {
      int indexm = prev_index2+1;
      int indexn = prev_index2+2;
      double pt1x = curve2->x(prev_index2)+OffsetX2+OX;
      double pt1y = curve2->y(prev_index2)+OffsetY2+OY;
      double pt2x = curve2->x(indexm)+OffsetX2+OX;
      double pt2y = curve2->y(indexm)+OffsetY2+OY;
      double pt3x = curve2->x(indexn)+OffsetX2+OX;
      double pt3y = curve2->y(indexn)+OffsetY2+OY;
      double pt4x = curve2->x(index2)+OffsetX2+OX;
      double pt4y = curve2->y(index2)+OffsetY2+OY;

      glColor3f (ColorArray[index1%COLORARRAYSIZE][0],
              ColorArray[index1%COLORARRAYSIZE][1],
              ColorArray[index1%COLORARRAYSIZE][2]);
        glBegin( GL_LINE_STRIP );
        glVertex2f( pt1x , pt1y );
        glVertex2f( pt2x , pt2y );
        glVertex2f( pt3x , pt3y );
        glVertex2f( pt4x , pt4y );
      glEnd();
    }

    prev_index2 = index2;
  }
*/
  //5)Draw the matching lines
  if (draw_matching_lines_) {
    for (unsigned int i=0; i<(*fmap).size(); i+=interval_) {
      indexP1 = (*fmap)[i].first;
      indexP2 = (*fmap)[i].second;
      double pt1x = curve1->x(indexP1)+OffsetX1+OX;
      double pt1y = curve1->y(indexP1)+OffsetY1+OY;
      double pt2x = curve2->x(indexP2)+OffsetX2+OX;
      double pt2y = curve2->y(indexP2)+OffsetY2+OY;

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

void dbcvr_curvematch_tableau::draw_render()
{
  if (cv_cor_.ptr() != NULL) {
    DrawCurves(curve1_pts_, curve2_pts_, 0, 0, offsetx2_, offsety2_, cv_cor_->open_curve_matching_);

    DrawPointMatching (cv_cor_->get_contour_pts1(), cv_cor_->get_contour_pts2(), 
                       // 0, curvematch_->curve1()->length()/2, offsetx2_, offsety2_, draw_matching_lines_);
                       0, 0, offsetx2_, offsety2_, draw_matching_lines_);

    
                                          
  }
  else if (curvematch_.ptr() != NULL) {  // the information whether did open curve matching or close curve matching is unknown if cv_cor_ is not available
    
    DrawCurveMatching (curvematch_->curve1(), curvematch_->curve2(), curvematch_->finalMap(),
      0, 0, offsetx2_, offsety2_, draw_matching_lines_);
  }
  
}

void dbcvr_curvematch_tableau::get_popup(const vgui_popup_params& params, vgui_menu &menu)
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
               new dbcvr_curvematch_tableau_toggle_command(this, &draw_matching_lines_));
  submenu.add( (initial+ " draw less"), 
               new dbcvr_curvematch_tableau_change_command(this, &interval_, &size_, 5));
  submenu.add( (initial+ " draw more"), 
               new dbcvr_curvematch_tableau_change_command(this, &interval_, &size_, -5));
  submenu.add( (initialx+ " draw right"), 
               new dbcvr_curvematch_tableau_change_command(this, &offsetx2_, &dummy_size_, 5));
  submenu.add( (initialx+ " draw left"), 
               new dbcvr_curvematch_tableau_change_command(this, &offsetx2_, &dummy_size_, -5));
  submenu.add( (initialy+ " draw down"), 
               new dbcvr_curvematch_tableau_change_command(this, &offsety2_, &dummy_size_, 5));
  submenu.add( (initialy+ " draw up"), 
               new dbcvr_curvematch_tableau_change_command(this, &offsety2_, &dummy_size_, -5));
  

  //add this submenu to the popup menu
  menu.add("Curve Matching Tableau Options", submenu);
}

void dbcvr_curvematch_tableau::set_curve_cor (dbcvr_cv_cor_sptr new_curve_cor) {
    cv_cor_ = new_curve_cor;
    size_ = (cv_cor_->get_contour_pts1()).size();

    for (unsigned i = 0; i < cv_cor_->poly1_->size(); i++)
      curve1_pts_.push_back(cv_cor_->poly1_->vertex(i));

    for (unsigned i = 0; i < cv_cor_->poly2_->size(); i++)
      curve2_pts_.push_back(cv_cor_->poly2_->vertex(i));

}

