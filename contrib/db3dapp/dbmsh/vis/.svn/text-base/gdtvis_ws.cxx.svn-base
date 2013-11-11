//: This is lemsvxlsrc/brcv/shp/dbmsh3d/vis/gdtvis_ws.cxx
//  MingChing Chang
//  Aug. 09, 2005.

#include <dbmsh3d/dbmsh3d_vertex.h>

#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>
#include <dbmsh3d/vis/dbmsh3d_vis_vertex.h>

#include <gdt/vis/gdtvis_ws.h>
#include <gdt/vis/gdtvis_welm.h>

#include <Inventor/SbColor.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoDrawStyle.h>

SoSeparator* gdtws_draw_W_in_Qw (gdt_ws_manager* ws_manager, const SbColor color, 
                                 const int nSamples, float lineWidth)
{
  SoSeparator* vis = new SoSeparator;

  //line width
  SoDrawStyle* ds = new SoDrawStyle;
  ds->setName (SbName("gdt_mesh_edge_style"));
  ds->lineWidth.setValue (lineWidth);
  vis->addChild(ds);

  //: Go through each interval on Qw and draw it  
  vcl_multimap<double, gdt_welm*>::iterator it = ws_manager->Qw()->begin();
  for (; it != ws_manager->Qw()->end(); it++) {
    gdt_welm* W = (*it).second;
    if (W->is_dege())
      continue;

    if (W->_is_RF())
      vis->addChild (gdt_draw_RF_vispt (W, color, nSamples));
    else
      vis->addChild (gdt_draw_W_vispt (W, color, nSamples));
  }

  return vis;
}

SoSeparator* gdt_draw_snodes_vispt (gdt_ws_manager* ws_manager,
                                    const float size)
{
  SoSeparator* vis = new SoSeparator;

  vcl_map<int, dbmsh3d_gdt_vertex_3d*>::iterator it = ws_manager->snodemap().begin();
  for (; it != ws_manager->snodemap().end(); it++) {
    dbmsh3d_vertex* vertex = (*it).second;
    //draw boundary vertex in light blue
    vis->addChild (draw_vertex_vispt_SoCube (vertex, SbColor(0.0f, 0.5f, 0.5f), size));
  }

  return vis;
}




