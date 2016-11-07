//: This is lemsvxlsrc/brcv/shp/dbmsh3d/vis/gdtvis_wf_arc.cxx
//  MingChing Chang
//  Aug. 09, 2005.

#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>
#include <dbmsh3d/gdtvis/gdtvis_backpt.h>
#include <dbmsh3d/gdtvis/gdtvis_welm.h>

#include <Inventor/SbColor.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSeparator.h>

//: draw all intervals.
SoSeparator* gdt_draw_welms (dbmsh3d_gdt_mesh* gdt_mesh, int nSamples, float lineWidth)
{
  SoSeparator* vis = new SoSeparator;

  //line width
  SoDrawStyle* ds = new SoDrawStyle;
  ds->setName (SbName("gdt_welm_style"));
  ds->lineWidth.setValue (lineWidth);
  vis->addChild(ds);

  //Loop through each welm on each mesh edge.
  vcl_map<int, dbmsh3d_edge*>::iterator it = gdt_mesh->edgemap().begin();
  for (; it != gdt_mesh->edgemap().end(); it++) {
    dbmsh3d_gdt_edge* E = (dbmsh3d_gdt_edge*) (*it).second;

    //For each E, loop through each W associated with it.
    vcl_map<double, gdt_ibase*>::iterator it = E->interval_section()->I_map()->begin();
    for (; it != E->interval_section()->I_map()->end(); it++) {
      gdt_welm* W = (gdt_welm*) (*it).second;
      if (W->is_dege())
        continue;
      assert (!W->_is_RF());
      vis->addChild (gdt_draw_W_vispt (W, get_next_rand_color(), nSamples));
    }

  }

  //Loop through each welm on each mesh vertex (pseudo-source).
  vcl_map<int, dbmsh3d_vertex*>::iterator vit = gdt_mesh->vertexmap().begin();
  for (; vit != gdt_mesh->vertexmap().end(); vit++) {
    dbmsh3d_gdt_vertex_3d* V = (dbmsh3d_gdt_vertex_3d*) (*vit).second;

    //For each V, loop through each RF from it.
    vcl_set<gdt_interval*>::iterator it = V->childIs().begin();
    while (it != V->childIs().end()) {
      gdt_welm* RF = (gdt_welm*) (*it);
      //Only draw if the RF is finalized.
      if (RF->is_final())
        vis->addChild (gdt_draw_RF_vispt (RF, get_next_rand_color(), nSamples));
      it++;
    }
  }

  return vis;
}

SoSeparator* gdt_draw_W_vispt (gdt_welm* W, SbColor color, int nSamples)
{
  SoSeparator* vis = new SoSeparator;

  //Color
  SoBaseColor *basecolor = new SoBaseColor;
  basecolor->rgb = color;
  vis->addChild (basecolor);

  gdt_draw_W_geometry_vispt (vis, W, nSamples);

  return vis;
}

//: Similar to the backward interval drawing, but draw forward.
void gdt_draw_W_geometry_vispt (SoSeparator* vis, gdt_welm* W, int nSamples)
{
  //Draw the geodesic query lines on interval I to a single SoLineSet
  //in n pairs: (ept1, spt1), (ept2, spt2), (ept3, spt3), ....(eptn, sptn).

  //Put all polyline points into coords.
  SoCoordinate3* coords = new SoCoordinate3;
  vis->addChild (coords);

  //Determine the local orientation.
  double alphaCL, alphaCR;
  dbmsh3d_gdt_vertex_3d* vO;
  if (W->_get_nextF_alphaLR_vO (alphaCL, alphaCR, &vO) == false)
    return;

  double step = W->etau() - W->stau();
  step /= (nSamples);

  //For each sample, put a geodesic line (sP-eP) to the SoLineSet.
  for (int i=0; i<nSamples; i++) {
    double tau = W->stau() + i*step;
    vgl_point_3d<double> sP = W->_point_from_tau (tau);

    //The eP can be on the nextI or on the shock.
    vgl_point_3d<double> eP = W->_get_eP_from_tau (tau, alphaCL, alphaCR, vO);

    SbVec3f verts[2];
    verts[0] = SbVec3f (eP.x(), eP.y(), eP.z());
    verts[1] = SbVec3f (sP.x(), sP.y(), sP.z());
    coords->point.setValues (i*2, 2, verts);
  }

  gdtvis_welm_SoLineSet *visW = new gdtvis_welm_SoLineSet (W);

  // Separate each pair <eP, sP> for each line.
  for (int i=0; i<nSamples; i++)
    visW->numVertices.set1Value (i, 2);

  vis->addChild (visW);
}

SoSeparator* gdt_draw_RF_vispt (gdt_welm* RF, SbColor color, int nSamples)
{
  SoSeparator* vis = new SoSeparator;

  //Color
  SoBaseColor *basecolor = new SoBaseColor;
  basecolor->rgb = color;
  vis->addChild (basecolor);

  gdt_draw_RF_geometry_vispt (vis, RF, nSamples);

  return vis;
}

void gdt_draw_RF_geometry_vispt (SoSeparator* vis, gdt_welm* RF, int nSamples)
{
  //Draw the geodesic query lines on interval I to a single SoLineSet
  //in n pairs: (ept1, spt1), (ept2, spt2), (ept3, spt3), ....(eptn, sptn).

  //Put all polyline points into coords.
  SoCoordinate3* coords = new SoCoordinate3;
  vis->addChild (coords);
  //unused SbVec3f verts[2];

  double step = RF->etau() - RF->stau();
  step /= (nSamples);

  //For each sample, put a geodesic line (sP-eP) to the SoLineSet.
  for (int i=0; i<nSamples; i++) {
    double tau = RF->stau() + i*step;
    vgl_point_3d<double> sP = RF->psrc()->pt();

    //The eP can be on the nextI or on the shock.
    vgl_point_3d<double> eP = RF->_point_from_tau (tau);

    SbVec3f verts[2];
    verts[0] = SbVec3f (eP.x(), eP.y(), eP.z());
    verts[1] = SbVec3f (sP.x(), sP.y(), sP.z());
    coords->point.setValues (i*2, 2, verts);
  }

  gdtvis_welm_SoLineSet *visRF = new gdtvis_welm_SoLineSet (RF);

  // Separate each pair <eP, sP> for each line.
  for (int i=0; i<nSamples; i++)
    visRF->numVertices.set1Value (i, 2);

  vis->addChild (visRF);
}





