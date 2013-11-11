//: This is lemsvxlsrc/brcv/shp/dbmsh3d/vis/gdtvis_wf_arc.cxx
//  MingChing Chang
//  Aug. 09, 2005.

#include <dbmsh3d/gdt/gdt_mesh.h>

#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>
#include <dbmsh3d/vis/dbmsh3d_vis_edge.h>
#include <dbmsh3d/vis/dbmsh3d_vis_mesh.h>
#include <dbmsh3d/gdtvis/gdtvis_backpt.h>
#include <dbmsh3d/gdtvis/gdtvis_interval.h>

#include <Inventor/SbColor.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSeparator.h>

SoSeparator* gdt_draw_I_vispt (const gdt_interval* I, SbColor color, int nSamples)
{
  SoSeparator* vis = new SoSeparator;

  //Color
  SoBaseColor *basecolor = new SoBaseColor;
  basecolor->rgb = color;
  vis->addChild (basecolor);

  gdt_draw_I_geometry_vispt (vis, I, nSamples);

  return vis;
}

void gdt_draw_I_geometry_vispt (SoSeparator* vis, const gdt_interval* I, int nSamples)
{
  //Draw the geodesic query lines on interval I to a single SoLineSet
  //in n pairs: (ept1, spt1), (ept2, spt2), (ept3, spt3), ....(eptn, sptn).

  //Put all polyline points into coords.
  SoCoordinate3* coords = new SoCoordinate3;
  vis->addChild (coords);

  double step = I->etau() - I->stau();
  step /= (nSamples);

  //For each sample, put a geodesic line (spt-ept) to the SoLineSet.
  for (int i=0; i<=nSamples; i++) {
    double tau = I->stau() + i*step;
    double theta = I->theta_from_tau (tau);

    vgl_point_3d<double> ept = I->_point_from_tau (tau);
    vgl_point_3d<double> spt;
    if (I->prevI()==NULL || I->prevI()->edge()==I->edge()) { //If prevI is NULL or RF.
      spt = I->psrc()->pt();
    }
    else { //Else, prevI is another regular interval.
      gdt_interval* prev_I = I->prevI();
      double thetap = I->_get_prev_theta (theta);
      spt = prev_I->point_from_theta (thetap);
    }

    SbVec3f verts[2];
    verts[0] = SbVec3f ((float)ept.x(), (float)ept.y(), (float)ept.z());
    verts[1] = SbVec3f ((float)spt.x(), (float)spt.y(), (float)spt.z());
    coords->point.setValues (i*2, 2, verts);
  }

  gdtvis_interval_SoLineSet *visI = new gdtvis_interval_SoLineSet (I);

  // Separate each pair <ept, spt> for each line.
  for (int i=0; i<=nSamples; i++)
    visI->numVertices.set1Value (i, 2);

  vis->addChild (visI);
}

SoSeparator* gdt_draw_edge_I (dbmsh3d_gdt_edge* cur_edge, int nSamples)
{
  SoSeparator* group = new SoSeparator;

  //: draw some testing lines for each interval in random color
  vcl_map<double, gdt_ibase*>::iterator it = cur_edge->interval_section()->I_map()->begin();
  for (; it != cur_edge->interval_section()->I_map()->end(); it++) {
    gdt_interval* I = (gdt_interval*) (*it).second;
    if (I->is_dege())
      continue;
    group->addChild (gdt_draw_I_vispt (I, get_next_rand_color(), nSamples));
  }

  //: a special treatment for edge with 2 dege intervals.
  if (cur_edge->interval_section()->I_map()->size()==2) {
    it = cur_edge->interval_section()->I_map()->begin();
    gdt_interval* I = (gdt_interval*) (*it).second;

    vcl_map<double, gdt_ibase*>::iterator it2 = it;
    it2++;
    gdt_interval* I2 = (gdt_interval*) (*it2).second;

    if (I->is_dege() && I2->is_dege()) {
      //: draw a sphere in the middle.
      vgl_point_3d<double> pt = I->_point_from_tau (I->etau());

      group->addChild (draw_sphere (pt, 0.02f, SbColor (1.0f, 0.50f, 0.0f)));
    }
  }

  return group;
}

//: draw all intervals.
SoSeparator* gdt_draw_intervals (dbmsh3d_gdt_mesh* gdt_mesh, int nSamples, float lineWidth)
{
  SoSeparator* vis = new SoSeparator;

  //line width
  SoDrawStyle* ds = new SoDrawStyle;
  ds->setName (SbName("gdt_mesh_edge_style"));
  ds->lineWidth.setValue (lineWidth);
  vis->addChild(ds);

  vcl_map<int, dbmsh3d_edge*>::iterator it = gdt_mesh->edgemap().begin();
  for (; it != gdt_mesh->edgemap().end(); it++) {
    dbmsh3d_gdt_edge* cur_edge = (dbmsh3d_gdt_edge*) (*it).second;

    vis->addChild (gdt_draw_edge_I (cur_edge, nSamples));
  }

  return vis;
}

SoSeparator* gdtf_draw_I_on_front (dbmsh3d_gdt_mesh* gdt_mesh, int nSamples, float lineWidth)
{
  SoSeparator* vis = new SoSeparator;

  //line width
  SoDrawStyle* ds = new SoDrawStyle;
  ds->setName (SbName("gdt_mesh_edge_style"));
  ds->lineWidth.setValue (lineWidth);
  vis->addChild(ds);

  vcl_map<int, dbmsh3d_edge*>::iterator it = gdt_mesh->edgemap().begin();
  for (; it != gdt_mesh->edgemap().end(); it++) {
    dbmsh3d_gdt_edge* cur_edge = (dbmsh3d_gdt_edge*) (*it).second;

    if (cur_edge->is_visited (gdt_mesh->i_traverse_flag())) {
      SbColor color = SbColor (0.0f, 0.8f, 0.0f);
      SoSeparator* vis = draw_edge (cur_edge, color);
      vis->addChild (vis);

      //: for edges on the front, draw intervals.
      vis = gdt_draw_edge_I (cur_edge, nSamples);
      vis->addChild (vis);
    }
  }

  return vis;
}

SoSeparator* gdt_draw_I_on_front_flag (dbmsh3d_gdt_mesh* gdt_mesh, int nSamples, float lineWidth)
{
  SoSeparator* vis = new SoSeparator;

  //line width
  SoDrawStyle* ds = new SoDrawStyle;
  ds->setName (SbName("gdt_mesh_edge_style"));
  ds->lineWidth.setValue (lineWidth);
  vis->addChild(ds);

  vcl_map<int, dbmsh3d_edge*>::iterator it = gdt_mesh->edgemap().begin();
  for (; it != gdt_mesh->edgemap().end(); it++) {
    dbmsh3d_gdt_edge* cur_edge = (dbmsh3d_gdt_edge*) (*it).second;

    //: draw each interval
    vcl_map<double, gdt_ibase*>::iterator it = cur_edge->interval_section()->I_map()->begin();
    for (; it != cur_edge->interval_section()->I_map()->end(); it++) {
      gdt_interval* I = (gdt_interval*) (*it).second;

      #if GDT_ALGO_I
      if (I->i_invalid_on_front() != 1)
        continue;
      #endif

      double stau = I->stau();
      double etau = I->etau();
      vgl_point_3d<double> spt = I->_point_from_tau (stau);
      vgl_point_3d<double> ept = I->_point_from_tau (etau);

      SbColor color = SbColor (0.0f, 1.0f, 0.0f);

      ///: draw the interval line 
      vis->addChild (draw_line (spt, ept, color));

      //: draw the test query shortest paths
      color = get_next_rand_color ();

      if (!I->is_dege())
        vis->addChild (gdt_draw_I_vispt (I, color, nSamples));
    }
  }

  return vis;
}



