//: This is lemsvxlsrc/brcv/rec/dbmsh3dr/vis/dbmsh3dr_process_vis.cxx
//  Creation: Feb 27, 2007   Ming-Ching Chang

//Note the order here
#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>
#include <dbmsh3d/vis/dbmsh3d_vis_vertex.h>
#include <dbmsh3d/vis/dbmsh3d_vis_point_set.h>
#include <dbmsh3d/vis/dbmsh3d_vis_mesh.h>

#include <dbmsh3dr/vis/dbmsh3dr_process_vis.h>
#include <dbmsh3d/cmdproc/dbmsh3d_process_vis.h>

#include <dbmsh3dr/vis/dbmsh3dr_vis.h>

#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>

//: Constructor and Destructor
//  Allocate the visualization object.
dbmsh3dr_pro_vis::dbmsh3dr_pro_vis (dbmsh3d_pro_vis* pv0, dbmsh3d_pro_vis* pv1) 
    : dbmsh3dr_pro (pv0, pv1) 
{
}

dbmsh3dr_pro_vis::~dbmsh3dr_pro_vis () 
{
}

dbmsh3d_pro_vis* dbmsh3dr_pro_vis::pv0() 
{
  return (dbmsh3d_pro_vis*) pro_[0];
}
  
dbmsh3d_pro_vis* dbmsh3dr_pro_vis::pv1() 
{
  return (dbmsh3d_pro_vis*) pro_[1];
}

dbmsh3d_pro_vis* dbmsh3dr_pro_vis::pv (const unsigned int i) 
{
  return (dbmsh3d_pro_vis*) pro_[i];
}


//########################################################################


SoSeparator* dbmsh3dr_pro_vis::vis_meshes (const int vis_option)
{
  SoSeparator* root = new SoSeparator;
  
  const int N_DATA = data_files_.size();
  //Visualization: either in random colors or a single color.
  VIS_COLOR_CODE ccode = COLOR_RANDOM;
  if (vis_option == 1)
    ccode = COLOR_SILVER;
  init_rand_color (DBMSH3D_MESH_COLOR_SEED);

  for (int i=0; i<N_DATA; i++) {
    pv(i)->mesh()->MHE_to_IFS ();
    root->addChild (pv(i)->vis_mesh (false, true, ccode, true));
  }

  return root;
}

//: Draw a line connecting corresponding points between bnd_1 and bnd_2.
SoSeparator* dbmsh3dr_pro_vis::vis_conn ()
{
  SoSeparator* root = new SoSeparator;

  SoBaseColor *basecolor = new SoBaseColor;
  basecolor->rgb = SbColor (1, 0, 0);
  root->addChild (basecolor);

  assert (pro_[0]->mesh()->vertexmap().size() == pro_[1]->mesh()->vertexmap().size());

  for (unsigned int i=0; i<pro_[0]->mesh()->vertexmap().size(); i++) {
    dbmsh3d_vertex* V1 = pro_[0]->mesh()->vertexmap(i);
    dbmsh3d_vertex* V2 = pro_[1]->mesh()->vertexmap(i);
    draw_line_geom (root, V1->pt(), V2->pt());
  }
  return root;
}

SoSeparator* dbmsh3dr_pro_vis::vis_oripts ()
{
  return draw_oriented_ptset (pv0()->oripts(), 
                              pv0()->m_vertex_ccode_, 
                              pv0()->m_vertex_ccode_, 
                              pv0()->vector_len_, 
                              pv0()->pt_size_, 1);
}

SoSeparator* dbmsh3dr_pro_vis::vis_oripts2 ()
{
  return draw_oriented_ptset (pv1()->oripts(), 
                              pv1()->m_vertex_ccode_, 
                              pv1()->m_vertex_ccode_, 
                              pv1()->vector_len_, 
                              pv1()->pt_size_, 1);
}

SoSeparator* dbmsh3dr_pro_vis::vis_min_dist_pt_color_SoPoint (const float size,
                                                              const bool show_line)
{
  SoSeparator* root = new SoSeparator;

  //point size
  SoDrawStyle * drawstyle = new SoDrawStyle;
  drawstyle->pointSize = size;
  root->addChild(drawstyle);

  vcl_map<int, dbmsh3d_vertex*>::iterator it = pro_[1]->mesh()->vertexmap().begin();
  for (int i=0; it != pro_[1]->mesh()->vertexmap().end(); it++, i++) {
    dbmsh3d_vertex* v = (*it).second;
    SoSeparator* sep = new SoSeparator;

    //assign color for each
    SoBaseColor* basecol = new SoBaseColor;
    basecol->rgb = pv1()->mesh_vertex_color_set_[i];
    sep->addChild (basecol);

    SoTranslation* trans = new SoTranslation;
    trans->translation.setValue (v->pt().x(), v->pt().y(), v->pt().z());
    sep->addChild (trans);

    SoPointSet* pt = new SoPointSet;
    sep->addChild (pt);    
    root->addChild (sep);
  }  

  //draw lines connecting point pairs.
  if (show_line) {
    it = pro_[1]->mesh()->vertexmap().begin();
    for (int i=0; it != pro_[1]->mesh()->vertexmap().end(); it++, i++) {
      dbmsh3d_vertex* V2 = (*it).second;
      SbColor color = pv1()->mesh_vertex_color_set_[i];
      int id = min_ids_[i];
      dbmsh3d_vertex* V = pro_[0]->mesh()->vertexmap (id);
      root->addChild (draw_line (V->pt(), V2->pt(), color));
    }
  }

  return root;
}


SoSeparator* dbmsh3dr_pro_vis::vis_min_dist_pt_color_SoCube (const float size)
{
  SoSeparator* root = new SoSeparator;
  SbColor color;
  int id;

  vcl_map<int, dbmsh3d_vertex*>::iterator it = pro_[1]->mesh()->vertexmap().begin();
  for (int i=0; it != pro_[1]->mesh()->vertexmap().end(); it++, i++) {
    dbmsh3d_vertex* V2 = (*it).second;
    color = pv1()->mesh_vertex_color_set_[i];
    id = min_ids_[i];
    dbmsh3d_vertex* V = pro_[0]->mesh()->vertexmap (id);
    
    if (pv0()->user_defined_class_)
      root->addChild (draw_vertex_vispt_SoPoint (V2, color, size));
    else
      root->addChild (draw_vertex_SoCube (V2, color, size));
    root->addChild (draw_line (V->pt(), V2->pt(), color));
  }

  return root;
}

SoSeparator* dbmsh3dr_pro_vis::vis_matching_vector ()
{
  SoSeparator* root = new SoSeparator;
  //Get the vertex color of the 2nd mesh.
  pv1()->compute_mesh_vertex_color_set (min_dists_);

  vcl_map<int, dbmsh3d_vertex*>::iterator it = pro_[1]->mesh()->vertexmap().begin();
  for (int i=0; it != pro_[1]->mesh()->vertexmap().end(); it++, i++) {
    dbmsh3d_vertex* V2 = (*it).second;
    SbColor color = pv1()->mesh_vertex_color_set_[i];
    vgl_point_3d<double> P = closest_pts_[i];    
    root->addChild (draw_line (P, V2->pt(), color));
  }
  return root;
}
