// MingChing Chang 040226
// VISUALIZATION OF THE fine-scale shocks

#include <vcl_cstdlib.h>

#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>
#include <dbmsh3d/vis/dbmsh3d_vis_mesh.h>
#include <dbsk3d/vis/dbsk3d_vis_backpt.h>
#include <dbsk3d/vis/dbsk3d_vis_fs_elm.h>
#include <dbsk3d/vis/dbsk3d_vis_fs_segre.h>

#include <Inventor/SbColor.h>
#include <Inventor/SbVec3f.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoShapeHints.h>

//: draw_option == -1: draw shock link and surface triangle in color tone
//  draw option == 1: draw in GRAY
//  draw option == 2: draw in random color
//  
SoSeparator* draw_segre_Q1_L_F (dbsk3d_fs_segre* fs_segre, 
                                const float th1, const float th2,
                                const int option,
                                const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;

  //surface triangle shape hints
  SoShapeHints* hints = new SoShapeHints();
  hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  root->addChild(hints);
  
  vcl_multimap<double, dbsk3d_fs_edge*>::iterator it = fs_segre->L_1st_queue().begin();
  for (unsigned int i=0; it != fs_segre->L_1st_queue().end(); it++, i++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) (*it).second;
    assert (FE->b_inf() == false);
    if (FE->cost() < th1)
      continue;
    if (FE->cost() > th2)
      continue;    

    //color
    SbColor color;
    if (option == -1) { //color tone
      double cvalue = 1.0 * i / fs_segre->L_1st_queue().size();
      color = get_color_tone ((float) 1.0-cvalue); 
    }
    else if (option == 1) 
      color = SbColor (0.5f, 0.5f, 0.5f);
    else if (option == 2) //random color
      color = get_next_rand_color (); 
    
    //shock link
    root->addChild (draw_fs_edge (FE, color, user_defined_class, false));

    //surface interpolant
    vcl_vector<dbmsh3d_vertex*> bnd_pts;
    bool result = FE->get_ordered_Gs_via_FF (bnd_pts);
    assert (result);

    SbVec3f* vertices = new SbVec3f[bnd_pts.size()];
    for (unsigned int i=0; i<bnd_pts.size(); i++) {
      dbmsh3d_vertex* G = bnd_pts[i];
      vertices[i].setValue (G->pt().x(), G->pt().y(), G->pt().z());      
    }
    root->addChild (draw_filled_polygon (vertices, bnd_pts.size(), color, 0.0f));
    delete vertices;
  }

  return root;
}

SoSeparator* draw_segre_Q1_batch (dbsk3d_fs_segre* fs_segre, 
                                  const float th1, const float th2,
                                  const bool type_I_only, const float len)
{
  SoSeparator* root = new SoSeparator;

  //Put all points into a vector.
  vcl_vector<vgl_point_3d<double> > ifs_pts;
  vcl_vector<vcl_vector<int> > ifs_faces;
  vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_point_3d<double> > > links;
  SbColor color = SbColor (1,0,0); ///SbColor (0.5f, 0.5f, 0.5f);
  
  //Go through all generators and assign vid to be [0 to n-1].  
  vcl_map<int, dbmsh3d_vertex*>::iterator vit = fs_segre->bnd_mesh()->vertexmap().begin();
  for (unsigned int i=0; vit != fs_segre->bnd_mesh()->vertexmap().end(); vit++, i++) {
    dbmsh3d_vertex* G = (*vit).second;
    G->set_vid (i);
    ifs_pts.push_back (G->pt());
  }

  vcl_multimap<double, dbsk3d_fs_edge*>::iterator it = fs_segre->L_1st_queue().begin();
  for (unsigned int i=0; it != fs_segre->L_1st_queue().end(); it++, i++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) (*it).second;
    assert (FE->b_inf() == false);
    if (FE->cost() < th1)
      continue;
    if (FE->cost() > th2)
      continue;    
    if (type_I_only && FE->detect_flow_type() != '1')
      continue; //Only draw type I triangles.

    //Shock link FE    
    vgl_point_3d<double> C;
    FE->compute_circum_cen (C);
    vgl_vector_3d<double> sv = FE->s_FV()->pt() - C;
    sv /= sv.length ();
    vgl_vector_3d<double> ev = FE->e_FV()->pt() - C;
    ev /= ev.length ();
    vgl_point_3d<double> s_FV = C + sv * len;
    vgl_point_3d<double> e_FV = C + ev * len;
    links.push_back (vcl_pair<vgl_point_3d<double>, vgl_point_3d<double> > (s_FV, e_FV));

    //Shock link FE's dual Delaunay triangles.
    vcl_vector<dbmsh3d_vertex*> genes;
    bool result = FE->get_ordered_Gs_via_FF (genes);
    assert (result);
    vcl_vector<int> face_vids;
    for (unsigned int i=0; i<genes.size(); i++)
      face_vids.push_back (genes[i]->vid());    
    ifs_faces.push_back (face_vids); //add to IFS faces.
  }

  root->addChild (draw_line_set (links, color));
  links.clear();
  root->addChild (draw_ifs (ifs_pts, ifs_faces, COLOR_GRAY, true));
  ifs_pts.clear();
  ifs_faces.clear();
  return root;
}

SoSeparator* draw_segre_Q2_L_F (dbsk3d_fs_segre* fs_segre, 
                                const float th1, const float th2,
                                const int option,
                                const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;

  //surface triangle shape hints
  SoShapeHints* hints = new SoShapeHints();
  hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  root->addChild(hints);

  vcl_multimap<double, dbsk3d_fs_edge*>::iterator it = fs_segre->L_2nd_queue().begin();
  for (unsigned int i=0; it != fs_segre->L_2nd_queue().end(); it++, i++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) (*it).second;
    assert (FE->b_inf() == false);
    if (FE->cost() < th1)
      continue;
    if (FE->cost() > th2)
      continue;

    //color
    SbColor color;
    if (option == -1) { //color tone
      double cvalue = 1.0 * i / fs_segre->L_2nd_queue().size();
      color = get_color_tone ((float) 1.0-cvalue); 
    }
    else if (option == 1) 
      color = SbColor (0.5f, 0.5f, 0.5f);
    else if (option == 2) //random color
      color = get_next_rand_color (); 
    
    //shock link
    root->addChild (draw_fs_edge (FE, color, false, user_defined_class));

    //surface interpolant
    vcl_vector<dbmsh3d_vertex*> bnd_pts;
    bool result = FE->get_ordered_Gs_via_FF (bnd_pts);      
    assert (result);

    SbVec3f* vertices = new SbVec3f[bnd_pts.size()];
    for (unsigned int i=0; i<bnd_pts.size(); i++) {
      dbmsh3d_vertex* G = bnd_pts[i];
      vertices[i].setValue (G->pt().x(), G->pt().y(), G->pt().z());      
    }
    root->addChild (draw_filled_polygon (vertices, bnd_pts.size(), color, 0.0f));
    delete vertices;
  }

  return root;
}

SoSeparator* draw_segre_oversize_L_F (dbsk3d_fs_mesh* fs_mesh,
                                      const float size_th,
                                      const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;

  vcl_map<int, dbmsh3d_edge*>::iterator lit = fs_mesh->edgemap().begin();
  for (; lit != fs_mesh->edgemap().end(); lit++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) (*lit).second;
    if (FE->b_inf())
      continue;

    //Compute the surface triangle parameters:
    double max_side, perimeter;
    unsigned int nG;
    double Side[3];
    dbmsh3d_vertex* Gene[3];
    bool result = FE->get_tri_params (max_side, perimeter, nG, Side, Gene);
    assert (result);

    //Only draw the oversized shock links.
    if (max_side < size_th)
      continue;

    SbColor color = SbColor (0.0f, 0.0f, 0.75f);

    //shock link
    root->addChild (draw_fs_edge (FE, color, false, user_defined_class));

    //surface interpolant
    vcl_vector<dbmsh3d_vertex*> bnd_pts;
    result = FE->get_ordered_Gs_via_FF (bnd_pts);      
    assert (result);

    SbVec3f* vertices = new SbVec3f[bnd_pts.size()];
    for (unsigned int i=0; i<bnd_pts.size(); i++) {
      dbmsh3d_vertex* G = bnd_pts[i];
      vertices[i].setValue (G->pt().x(), G->pt().y(), G->pt().z());      
    }
    root->addChild (draw_filled_polygon (vertices, bnd_pts.size(), color, 0.0f));
    delete vertices;    
  }

  return root;
}



