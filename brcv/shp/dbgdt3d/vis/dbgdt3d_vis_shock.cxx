//: This is lemsvxlsrc/brcv/shp/dbmsh3d/vis/dbmsh3d_vis_mesh.cxx
//  MingChing Chang
//  May 03, 2005.

#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>
#include <dbgdt3d/vis/dbgdt3d_vis_backpt.h>
#include <dbgdt3d/vis/dbgdt3d_vis_shock.h>

#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoMaterial.h>

SoSeparator* gdt_draw_shocks (gdt_ws_manager* ws_manager, const float lineWidth)
{
  SoSeparator* root = new SoSeparator;

  vcl_map<int, gdt_shock*>::iterator it = ws_manager->shockmap().begin();
  for (; it != ws_manager->shockmap().end(); it++) {
    gdt_shock* S = (*it).second;

    if (S->b2() == 0) //draw contact shock in orange
      root->addChild (draw_shock (S, SbColor(1.0f, 0.5f, 0.0f), lineWidth));
    else if (S->a() == 0) //draw line shock in red
      root->addChild (draw_shock (S, SbColor(1.0f, 0.0f, 0.0f), lineWidth));
    else //draw hyperbola shock in dark purple
      root->addChild (draw_shock (S, SbColor(0.5f, 0.0f, 0.5f), lineWidth));
  }

  return root;
}

SoSeparator* draw_shock (gdt_shock* S, SbColor color, const float lineWidth)
{
  SoSeparator* vis = new SoSeparator;

  //Backtrack the shock segment path.
  vcl_vector<vgl_point_3d<double> > path_vertices;
  
  //Only put in the S->ENode if its id < 0, so S-V vertex is not drawn twice.
  if (S->Enode() && S->Enode()->id() < 0)
    path_vertices.push_back (S->Enode()->pt());

  //Use the stored selms_[] to query the intersectiong point P.
  for (int i=S->selms().size()-1; i>0; i--) {
    gdt_selm* selm = S->selms()[i];
    assert (selm->tauType_ != SELM_TAU_NONE);

    vgl_point_3d<double> P;
    if (selm->tauType_ == SELM_TAU_ON_EA)
      P = selm->Wa_->_point_from_tau (selm->tauE_);
    else
      P = selm->Wb_->_point_from_tau (selm->tauE_);

    path_vertices.push_back (P);
  }

  path_vertices.push_back (S->Snode()->pt());

  //Color
  SoBaseColor *basecolor = new SoBaseColor;
  basecolor->rgb = color;
  vis->addChild (basecolor);

  //Line width
  SoDrawStyle*  drawStyle = new SoDrawStyle;
  drawStyle->lineWidth.setValue (lineWidth);
  vis->addChild (drawStyle);

  gdt_draw_shock_geometry_vispt (vis, S, path_vertices);

  return vis;
}

void gdt_draw_shock_geometry_vispt (SoSeparator* vis, const gdt_shock* S, 
                                    const vcl_vector<vgl_point_3d<double> >& path_vertices)
{
  //Put all polyline points into coords.
  SoCoordinate3* coords = new SoCoordinate3;
  vis->addChild (coords);
  for (unsigned int i=0; i<path_vertices.size(); i++) {
    vgl_point_3d<double> p = path_vertices[i];
    coords->point.set1Value (i, (float) p.x(), (float) p.y(), (float) p.z());
  }

  gdtvis_shock_SoLineSet *visS = new gdtvis_shock_SoLineSet (S);
  vis->addChild (visS);
}








