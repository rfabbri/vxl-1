// This is dbgui/dbgui_shockshape_tableau.cxx
#include "dbgui_shockshape_tableau.h"
//:
// \file
// \author Nhon Trinh
// \date 10/30/04
// \brief  See dbgui_shockshape_tableau.h for a description of this file.

#include <vcl_iostream.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_gl.h>


//--------------------------------------------------------------------------------

//: constructor
dbgui_shockshape_tableau::dbgui_shockshape_tableau()
{
}

//: destructor
dbgui_shockshape_tableau::~dbgui_shockshape_tableau()
{
}

//: return name
vcl_string dbgui_shockshape_tableau::type_name() const
{
  return "dbgui_shockshape_tableau";
}


////: draw the grid for the given transformation
//void 
//dbgui_bvrl_tableau::draw_grid(const vcl_vector<vgl_point_2d<double> >& verts, 
//                              unsigned int nx, unsigned int ny)
//{
//  if(verts.size() != 4 )
//    return;
//
//  glLineWidth(2);
//  glColor3f(1,0,0);
//  
//  glBegin(GL_LINE_LOOP);
//  for (unsigned i=0; i<4; ++i)
//    glVertex2f(verts[i].x(), verts[i].y() );
//  glEnd();
//  
//  vgl_vector_2d<double> v1 = (verts[1] - verts[0])/ny;
//  vgl_vector_2d<double> v2 = (verts[2] - verts[3])/ny;
//  
//  vgl_point_2d<double> p1 = verts[0] + v1;
//  vgl_point_2d<double> p2 = verts[3] + v2;
//  
//  glLineWidth(1);
//  glColor3f(1,1,1);
//  glBegin(GL_LINES);
//  for(int i=0; i<ny-1; ++i, p1+=v1, p2+=v2){
//    glVertex2f(p1.x(), p1.y());
//    glVertex2f(p2.x(), p2.y());
//  }
//  glEnd();
//  
//  v1 = (verts[3] - verts[0])/nx;
//  v2 = (verts[2] - verts[1])/nx;
//  
//  p1 = verts[0] + v1;
//  p2 = verts[1] + v2;
//  
//  glBegin(GL_LINES);
//  for(int i=0; i<nx-1; ++i, p1+=v1, p2+=v2){
//    glVertex2f(p1.x(), p1.y());
//    glVertex2f(p2.x(), p2.y());
//  }
//  glEnd();
//}


//bool dbgui_shockshape_tableau::handle(vgui_event const& e)
//{
//  if( e.type == vgui_DRAW){
//    
//    return true;
//  }
//  return false;
//}


// end file

