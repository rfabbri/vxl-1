//MingChing Chang 041130
//  This is the visualization of the scaffold elements.

#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>
#include <dbmsh3d/vis/dbmsh3d_vis_mesh.h>
#include <dbsk3d/vis/dbsk3d_vis_backpt.h>
#include <dbsk3d/vis/dbsk3d_vis_fs_elm.h>
#include <dbsk3d/vis/dbsk3d_vis_ms_elm.h>

#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoTranslation.h>

SoSeparator* draw_ms_vertex (const dbsk3d_ms_node* MN, 
                             const SbColor color, const float size,
                             const bool show_id,
                             const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;

  //color
  SoBaseColor* basecolor = new SoBaseColor;
  basecolor->rgb = color;
  root->addChild (basecolor);

  draw_ms_vertex_geom (root, MN, size, user_defined_class);

  if (show_id) {
    char buf[64];
    vcl_sprintf (buf, "%d", MN->id());
    draw_text2d_geom (root, buf);
  }
  
  return root;
}

void draw_ms_vertex_geom (SoSeparator* root, const dbsk3d_ms_node* MN, 
                          const float size, const bool user_defined_class)
{
  //translation
  SoTranslation *trans = new SoTranslation;
  SbVec3f move( (float) MN->FV()->pt().x(), 
                (float) MN->FV()->pt().y(), 
                (float) MN->FV()->pt().z() );
  trans->translation.setValue (move);
  root->addChild(trans);

  //create the MN_vis
  if (user_defined_class) { //put backward pointer
    ms_node_SoSphere* MN_vis = new ms_node_SoSphere (MN);
    MN_vis->radius = size;
    root->addChild (MN_vis);
  }
  else {
    SoSphere* MN_vis = new SoSphere;
    MN_vis->radius = size;
    root->addChild (MN_vis);
  }
}

// #####################################################################################

SoSeparator* draw_ms_curve (const dbsk3d_ms_curve* MC, 
                            const SbColor color, const float width,
                            const bool idv, const bool show_id,
                            const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;

  //Color
  SoBaseColor* rgb = new SoBaseColor;
  rgb->rgb.setValue (color);
  root->addChild (rgb);
  
  //Line width
  SoDrawStyle* drawStyle = new SoDrawStyle;
  drawStyle->lineWidth.setValue (width);
  ///drawStyle->style = SoDrawStyle::LINES;
  root->addChild (drawStyle);

  if (MC->c_type() == C_TYPE_VIRTUAL) {
    //draw the virtual curve with a tiny shift.
    float dx = 0.01f;
    SoSeparator* group = new SoSeparator;

    if (MC->data_type()==C_DATA_TYPE_VERTEX || idv==false)
      draw_ms_curve_geom (group, MC, user_defined_class);
    else {
      assert (MC->data_type()==C_DATA_TYPE_EDGE);
      for (unsigned int i=0; i<MC->E_vec().size(); i++) {
        dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) MC->E_vec(i);
        draw_fs_edge_geom (group, FE, user_defined_class);
      }
    }
    
    root->addChild (shift_output (group, dx, 0, 0));

    //draw straight line.
    //root->addChild (draw_line (MC->s_MN()->pt(), MC->e_MN()->pt(), color));
  }
  else {
    if (MC->data_type()==C_DATA_TYPE_VERTEX || idv==false)
      draw_ms_curve_geom (root, MC, user_defined_class);
    else {
      assert (MC->data_type()==C_DATA_TYPE_EDGE);
      for (unsigned int i=0; i<MC->E_vec().size(); i++) {
        dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) MC->E_vec(i);
        draw_fs_edge_geom (root, FE, user_defined_class);
      }
    }    
  }

  if (show_id) {
    char id_buf[64];
    vcl_sprintf (id_buf, "%d", MC->id());

    //draw at the middle of the curve.
    const dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) MC->get_middle_V ();
    const vgl_point_3d<double> pt = FV->pt();
    root->addChild (draw_text2d (id_buf, (float)pt.x(), (float)pt.y(), (float)pt.z()));
  }

  return root;
}

void draw_ms_curve_geom (SoSeparator* root, 
                         const dbsk3d_ms_curve* MC,
                         const bool user_defined_class)
{
  //Set up the points on the MC
  SoCoordinate3* coords = new SoCoordinate3;
  root->addChild (coords);
  
  vcl_vector<dbmsh3d_vertex*> N_vec;
  MC->get_V_vec (N_vec);
  assert (N_vec.size() > 1);

  for (unsigned int i=0; i<N_vec.size(); ++i) {
    vgl_point_3d<double> p = N_vec[i]->pt();
    coords->point.set1Value (i, (float)p.x(), (float)p.y(), (float)p.z());
  }

  if (user_defined_class) {
    ms_curve_SoLineSet *vertIndex = new ms_curve_SoLineSet (MC);
    root->addChild (vertIndex);
  }
  else {
    SoLineSet *vertIndex = new SoLineSet;
    root->addChild (vertIndex);
  }
}

SoSeparator* draw_ms_loop (dbsk3d_ms_curve* MC, 
                           const SbColor color, const float width,
                           const bool idv, const bool show_id,
                           const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;
  assert (MC->is_self_loop());

  //Color
  SoBaseColor* basecol = new SoBaseColor;
  basecol->rgb = color;
  root->addChild (basecol);

  //Line width
  SoDrawStyle*  drawStyle = new SoDrawStyle;
  drawStyle->lineWidth.setValue (width);
  root->addChild (drawStyle);

  if (MC->data_type()==C_DATA_TYPE_VERTEX || idv==false)
    draw_ms_loop_geom (root, MC, user_defined_class);
  else {
    assert (MC->data_type()==C_DATA_TYPE_EDGE);
    for (unsigned int i=0; i<MC->E_vec().size(); i++) {
      dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) MC->E_vec(i);
      draw_fs_edge_geom (root, FE, user_defined_class);
    }
  }

  if (show_id) {
    char id_buf[64];
    vcl_sprintf (id_buf, "%d", MC->id());

    //draw at the middle of the curve.
    const dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) MC->get_middle_V ();
    const vgl_point_3d<double> pt = FV->pt();
    root->addChild (draw_text2d (id_buf, (float)pt.x(), (float)pt.y(), (float)pt.z()));
  }

  return root;
}

void draw_ms_loop_geom (SoSeparator* root, 
                        const dbsk3d_ms_curve* MC,
                        const bool user_defined_class)
{
  assert (MC->is_self_loop());
  //Set up the points on the curve
  SoCoordinate3* coords = new SoCoordinate3;
  root->addChild( coords );
 
  vcl_vector<dbmsh3d_vertex*> N_vec;
  MC->get_V_vec (N_vec);
  assert (N_vec.size() > 1);

  for (unsigned int i=0; i<N_vec.size(); ++i) {
    vgl_point_3d<double> p = N_vec[i]->pt();
    coords->point.set1Value (i, (float)p.x(), (float)p.y(), (float)p.z());
  }

  //the last point to close the loop.
  vgl_point_3d<double> p = N_vec[0]->pt();
  coords->point.set1Value (N_vec.size(), (float)p.x(), (float)p.y(), (float)p.z());

  if (user_defined_class) {
    ms_curve_SoLineSet *vertIndex = new ms_curve_SoLineSet (MC);
    root->addChild (vertIndex);
  }
  else {
    SoLineSet *vertIndex = new SoLineSet;
    root->addChild (vertIndex);
  }
}

SoSeparator* draw_ms_sheet (dbsk3d_ms_sheet* MS, const SbColor color, const float transp,
                            const bool idv, const bool show_id, const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;

  SoMaterial *m = new SoMaterial;
  m->setName( SbName( "ms_sheet_material" ) );
  m->diffuseColor.setValue (color);
  m->emissiveColor.setValue(color/2);
  m->transparency = transp;
  root->addChild (m);

  if (idv) { //draw individual fs_faces.
    vcl_map<int, dbmsh3d_face*>::iterator it = MS->facemap().begin();
    for (; it != MS->facemap().end(); it++) {
      dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*it).second;
      draw_fs_face_geom (root, FF, user_defined_class);
    }
  }
  else
    draw_ms_sheet_geom (root, MS, user_defined_class);

  if (show_id) {
    char id_buf[64];
    vcl_sprintf (id_buf, "%d", MS->id());

    //show id at the middle of the sheet.
    const dbsk3d_fs_vertex* FV = MS->get_middle_FV ();
    const vgl_point_3d<double> pt = FV->pt();
    root->addChild (draw_text2d (id_buf, (float)pt.x(), (float)pt.y(), (float)pt.z()));
  }

  return root;
}

void draw_ms_sheet_geom (SoSeparator* root, dbsk3d_ms_sheet* MS,
                         const bool user_defined_class)
{
  //convert to vcl_vector and draw.
  vcl_map<int, dbmsh3d_vertex*> V_map;
  MS->get_fine_scale_vertices (V_map);

  if (user_defined_class)
    draw_SS_faces_geom (root, MS, V_map, MS->facemap());
  else
    draw_faces_geom (root, V_map, MS->facemap());

  /*SoVertexProperty* vp = new SoVertexProperty;
  unsigned int n_ind = MS->_count_faces_indices_mhe();
  int* ind = new int [n_ind];

  _draw_M_mhe_geom (MS, vp, n_ind, ind);

  if (user_defined_class) {
    ms_sheet_SoIndexedFaceSet* ifs = new ms_sheet_SoIndexedFaceSet (MS);
    ifs->vertexProperty = vp;
    ifs->coordIndex.setValues (0, n_ind, ind);
    root->addChild (ifs);
  }
  else {
    SoIndexedFaceSet* ifs = new SoIndexedFaceSet ();
    ifs->vertexProperty = vp;
    ifs->coordIndex.setValues (0, n_ind, ind);
    root->addChild (ifs);
  }

  delete []ind;*/
}

void draw_SS_faces_geom (SoGroup* root, dbsk3d_ms_sheet* MS,
                         vcl_map<int, dbmsh3d_vertex*>& V_map,
                         vcl_map<int, dbmsh3d_face*>& F_map)
{
  //convert to vcl_vector and draw.
  vcl_vector<vgl_point_3d<double> > pts_vector (V_map.size());
  vcl_vector<vcl_vector<int> > faces_vector (F_map.size());

  vcl_map<int, dbmsh3d_vertex*>::iterator vit = V_map.begin();
  for (unsigned int i=0; vit != V_map.end(); vit++, i++) {
    dbmsh3d_vertex* V = (*vit).second;
    V->set_vid (i);
    pts_vector[i] = V->pt();
  }

  vcl_map<int, dbmsh3d_face*>::iterator fit = F_map.begin();
  for (unsigned int i=0 ; fit != F_map.end(); fit++, i++) {
    dbmsh3d_face* F = (*fit).second;
    vcl_vector<dbmsh3d_vertex*> vertices; 
    F->get_bnd_Vs (vertices);
    faces_vector[i].resize (vertices.size());

    for (unsigned j=0; j<vertices.size(); j++) {
      dbmsh3d_vertex* V = vertices[j];
      assert (V_map.find (V->id()) != V_map.end());
      faces_vector[i][j] = V->vid();
    }
  }

  draw_SS_ifs_geom (root, MS, pts_vector, faces_vector);
}

void draw_SS_ifs_geom (SoGroup* root, dbsk3d_ms_sheet* MS,
                       const vcl_vector<vgl_point_3d<double> >& pts,
                       const vcl_vector<vcl_vector<int> >& faces)
{
  //Assign vertices
  int nVertices = pts.size();
  float (*xyz)[3] = new float[nVertices][3];
  for (unsigned int i=0; i<pts.size(); i++) {
    xyz[i][0] = (float) pts[i].x();
    xyz[i][1] = (float) pts[i].y();
    xyz[i][2] = (float) pts[i].z();
  }
 
  //Assign faces
  SoVertexProperty* vp = new SoVertexProperty;
  unsigned int n_ind = _count_faces_indices (faces);
  int* ind = new int [n_ind];
  unsigned int k = 0;  
  for (unsigned int i=0; i<faces.size(); i++) {
    assert (faces[i].size() > 2);
    for (unsigned int j=0; j<faces[i].size(); j++) {
      ind[k] = faces[i][j];
      k++;
    }    
    ind[k] = -1; //Add the final '-1'
    k++;
  }
  assert (k == n_ind);
  vp->vertex.setValues (0, nVertices, xyz);
  delete []xyz;

  ms_sheet_SoIndexedFaceSet* ifs = new ms_sheet_SoIndexedFaceSet (MS);
  ifs->vertexProperty = vp;
  ifs->coordIndex.setValues (0, n_ind, ind);
  delete []ind;
  root->addChild (ifs);
}




