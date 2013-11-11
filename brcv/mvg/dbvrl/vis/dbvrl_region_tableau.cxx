// This is brcv/mvg/dbvrl/vis/dbvrl_region_tableau.cxx
#include "dbvrl_region_tableau.h"
//:
// \file
// \author Matt Leotta
// \brief  See dbvrl_region_tableau.h for a description of this file.

#include <vcl_iostream.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_gl.h>
#include <vnl/vnl_identity_3x3.h>

#include <dbvrl/dbvrl_region.h>
#include <dbvrl/dbvrl_transform_2d.h>

//--------------------------------------------------------------------------------

dbvrl_region_tableau::dbvrl_region_tableau()
{
}


dbvrl_region_tableau::~dbvrl_region_tableau()
{
}


vcl_string dbvrl_region_tableau::type_name() const
{
  return "dbvrl_region_tableau";
}


//: Set the 2d homography
void 
dbvrl_region_tableau::set_transform(const dbvrl_transform_2d_sptr& xform)
{
  xform_ = xform;
}


//: Set the region
void 
dbvrl_region_tableau::set_region(const dbvrl_region_sptr& region)
{
  region_ = region;
}


//: draw the grid for the given transformation
void 
dbvrl_region_tableau::draw_grid(const vcl_vector<vgl_point_2d<double> >& verts, 
                              unsigned int nx, unsigned int ny)
{
  if(verts.size() != 4 )
    return;

  glLineWidth(2);
  glColor3f(1,0,0);
  
  glBegin(GL_LINE_LOOP);
  for (unsigned i=0; i<4; ++i)
    glVertex2f(verts[i].x(), verts[i].y() );
  glEnd();
  
  vgl_vector_2d<double> v1 = (verts[1] - verts[0])/ny;
  vgl_vector_2d<double> v2 = (verts[2] - verts[3])/ny;
  
  vgl_point_2d<double> p1 = verts[0] + v1;
  vgl_point_2d<double> p2 = verts[3] + v2;
  
  glLineWidth(1);
  glColor3f(1,1,1);
  glBegin(GL_LINES);
  for(int i=0; i<int(ny)-1; ++i, p1+=v1, p2+=v2){
    glVertex2f(p1.x(), p1.y());
    glVertex2f(p2.x(), p2.y());
  }
  glEnd();
  
  v1 = (verts[3] - verts[0])/nx;
  v2 = (verts[2] - verts[1])/nx;
  
  p1 = verts[0] + v1;
  p2 = verts[1] + v2;
  
  glBegin(GL_LINES);
  for(int i=0; i<int(nx)-1; ++i, p1+=v1, p2+=v2){
    glVertex2f(p1.x(), p1.y());
    glVertex2f(p2.x(), p2.y());
  }
  glEnd();
}


bool dbvrl_region_tableau::handle(vgui_event const& e)
{
  if( e.type == vgui_DRAW && region_){
    vcl_vector<vgl_point_2d<double> > pts = region_->corners();
    for(int i=0; i<4; ++i)
      pts[i] = (*xform_)(pts[i]);
    this->draw_grid(pts, 10, 10);
    return true;
  }
  return false;
}


bool dbvrl_region_tableau::get_bounding_box(float low[3], float high[3]) const
{
  return false;
}

//--------------------------------------------------------------------------------
