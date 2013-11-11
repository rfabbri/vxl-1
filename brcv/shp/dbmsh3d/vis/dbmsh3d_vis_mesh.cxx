//: This is lemsvxlsrc/brcv/shp/dbmsh3d/vis/dbmsh3d_vis_mesh.cxx
//  MingChing Chang
//  May 03, 2005.

#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <dbmsh3d/pro/dbmsh3d_cmdpara.h>
#include <dbmsh3d/algo/dbmsh3d_mesh_bnd.h>
#include <dbmsh3d/vis/dbmsh3d_vis_backpt.h>
#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>
#include <dbmsh3d/vis/dbmsh3d_vis_vertex.h>
#include <dbmsh3d/vis/dbmsh3d_vis_edge.h>
#include <dbmsh3d/vis/dbmsh3d_vis_face.h>
#include <dbmsh3d/vis/dbmsh3d_vis_mesh.h>

#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoTexture2.h>

#include <Inventor/sensors/SoSensor.h>
#include <Inventor/sensors/SoTimerSensor.h>

#include <Inventor/engines/SoTimeCounter.h>
#include <Inventor/engines/SoCalculator.h>

int _count_faces_indices (const vcl_vector<vcl_vector<int> >& faces)
{
  unsigned int total = 0;
  for (unsigned int i=0; i<faces.size(); i++)
    total += (faces[i].size() + 1); //the '-1' field
  return total;
}

void draw_ifs_geom (SoGroup* root, 
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

  SoIndexedFaceSet* indexedFaceSet = new SoIndexedFaceSet ();
  indexedFaceSet->vertexProperty = vp;
  indexedFaceSet->coordIndex.setValues (0, n_ind, ind);
  delete []ind;
  root->addChild (indexedFaceSet);
}

void draw_ifs_geom (SoGroup* root, vcl_set<dbmsh3d_vertex*>& V_set,
                    vcl_set<dbmsh3d_face*>& F_set)
{
  //convert to vcl_vector and draw.
  vcl_vector<vgl_point_3d<double> > pts_vector (V_set.size());
  vcl_vector<vcl_vector<int> > faces_vector (F_set.size());

  vcl_set<dbmsh3d_vertex*>::iterator vit = V_set.begin();
  for (unsigned int i=0; vit != V_set.end(); vit++, i++) {
    (*vit)->set_vid (i);
    pts_vector[i] = (*vit)->pt();
  }

  vcl_set<dbmsh3d_face*>::iterator fit = F_set.begin();
  for (unsigned int i=0 ; fit != F_set.end(); fit++, i++) {
    dbmsh3d_face* F = *fit;
    vcl_vector<dbmsh3d_vertex*> vertices;
    F->get_bnd_Vs (vertices);

    faces_vector[i].resize (vertices.size());
    for (unsigned j=0; j<vertices.size(); j++) {
      dbmsh3d_vertex* V = vertices[j];
      assert (V_set.find (V) != V_set.end());
      faces_vector[i][j] = V->vid();
    }
  }

  draw_ifs_geom (root, pts_vector, faces_vector);
}

void draw_faces_geom (SoGroup* root, vcl_map<int, dbmsh3d_vertex*>& V_map,
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

  draw_ifs_geom (root, pts_vector, faces_vector);
}

SoSeparator* draw_ifs (const vcl_vector<vgl_point_3d<double> >& pts,
                       const vcl_vector<vcl_vector<int> >& faces,
                       const int colorcode,
                       const bool b_shape_hints, const float transp)
{  
  SoSeparator* root = new SoSeparator;

  //Assign Material for search and change interactively...
  SoMaterial* material = new SoMaterial;
  material->setName (SbName("mesh_material"));

  if (colorcode == COLOR_GOLD) { //gold
    material->ambientColor.setValue (.3f, .1f, .1f);
    material->diffuseColor.setValue (.8f, .7f, .2f);
    material->specularColor.setValue (.4f, .3f, .1f);
    material->shininess = .4f;
  }
  else if (colorcode == COLOR_SILVER) { //silver
    material->ambientColor.setValue(.2f, .2f, .2f);
    material->diffuseColor.setValue(.5f, .5f, .5f);
    material->specularColor.setValue (.4f, .4f, .4f);
  }
  else if (colorcode != 0) {
    material->diffuseColor = color_from_code (colorcode);
  }

  material->transparency = transp;
  root->addChild (material);

  //Put the SoShapeHints,
  //see http://doc.coin3d.org/Coin/classSoShapeHints.html
  if (b_shape_hints) {
    SoShapeHints* hints = new SoShapeHints();
    hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    ///hints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
    hints->faceType = SoShapeHints::UNKNOWN_FACE_TYPE; //this may slow down the rendering!
    root->addChild (hints);
  }

  //Assign Style for search and change interactively...
  SoDrawStyle* ds = new SoDrawStyle;
  ds->setName( SbName("boundary_mesh_style") );
  root->addChild (ds);

  draw_ifs_geom (root, pts, faces);

  return root;
}

SoSeparator* draw_ifs (const vcl_vector<vgl_point_3d<double> >& pts,
                       const vcl_vector<vcl_vector<int> >& faces,
                       const SbColor& color,
                       const bool b_shape_hints, const float transp)
{  
  SoSeparator* root = new SoSeparator;

  //Assign Material for search and change interactively...
  SoMaterial* material = new SoMaterial;
  material->setName (SbName("mesh_material"));
  material->diffuseColor = color;
  material->transparency = transp;
  root->addChild (material);

  //Put the SoShapeHints,
  //see http://doc.coin3d.org/Coin/classSoShapeHints.html
  if (b_shape_hints) {
    SoShapeHints* hints = new SoShapeHints();
    hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    ///hints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
    hints->faceType = SoShapeHints::UNKNOWN_FACE_TYPE; //this may slow down the rendering!
    root->addChild (hints);
  }

  //Assign Style for search and change interactively...
  SoDrawStyle* ds = new SoDrawStyle;
  ds->setName( SbName("boundary_mesh_style") );
  root->addChild (ds);

  draw_ifs_geom (root, pts, faces);

  return root;
}

SoSeparator* draw_ifs (vcl_set<dbmsh3d_vertex*>& pts, vcl_set<dbmsh3d_face*>& faces,
                       const int colorcode, const bool b_shape_hints, const float transp)
{
  SoSeparator* root = new SoSeparator;

  //Assign Material for search and change interactively...
  SoMaterial* material = new SoMaterial;
  material->setName (SbName("mesh_material"));

  if (colorcode == COLOR_GOLD) { //gold
    material->ambientColor.setValue (.3f, .1f, .1f);
    material->diffuseColor.setValue (.8f, .7f, .2f);
    material->specularColor.setValue (.4f, .3f, .1f);
    material->shininess = .4f;
  }
  else if (colorcode == COLOR_SILVER) { //silver
    material->ambientColor.setValue(.2f, .2f, .2f);
    material->diffuseColor.setValue(.5f, .5f, .5f);
    material->specularColor.setValue (.4f, .4f, .4f);
  }
  else if (colorcode != 0) {
    material->diffuseColor = color_from_code (colorcode);
  }

  material->transparency = transp;
  root->addChild (material);

  //Put the SoShapeHints,
  //see http://doc.coin3d.org/Coin/classSoShapeHints.html
  if (b_shape_hints) {
    SoShapeHints* hints = new SoShapeHints();
    hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    ///hints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
    hints->faceType = SoShapeHints::UNKNOWN_FACE_TYPE; //this may slow down the rendering!
    root->addChild (hints);
  }

  //Assign Style for search and change interactively...
  SoDrawStyle* ds = new SoDrawStyle;
  ds->setName( SbName("boundary_mesh_style") );
  root->addChild (ds);

  draw_ifs_geom (root, pts, faces);

  return root;
}

SoSeparator* draw_ifs (vcl_set<dbmsh3d_vertex*>& pts, vcl_set<dbmsh3d_face*>& faces,
                            const SbColor& color, const bool b_shape_hints, const float transp)
{
  SoSeparator* root = new SoSeparator;

  //Assign Material for search and change interactively...
  SoMaterial* material = new SoMaterial;
  material->setName (SbName("mesh_material"));
  material->diffuseColor = color;
  material->transparency = transp;
  root->addChild (material);

  //Put the SoShapeHints,
  //see http://doc.coin3d.org/Coin/classSoShapeHints.html
  if (b_shape_hints) {
    SoShapeHints* hints = new SoShapeHints();
    hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    ///hints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
    hints->faceType = SoShapeHints::UNKNOWN_FACE_TYPE; //this may slow down the rendering!
    root->addChild (hints);
  }

  //Assign Style for search and change interactively...
  SoDrawStyle* ds = new SoDrawStyle;
  ds->setName( SbName("boundary_mesh_style") );
  root->addChild (ds);

  draw_ifs_geom (root, pts, faces);

  return root;
}

//: input:
//    const dbmsh3d_mesh* M
//
//  return:
//     SoVertexProperty* vp,
//     int *ind
void _draw_M_ifs_geom (dbmsh3d_mesh* M, SoVertexProperty* vp, int n_ind, int* ind)
{
  int nVertices = M->vertexmap().size();
  float (*xyz)[3] = new float[nVertices][3];

  //Assign vertices
  vcl_map<int, dbmsh3d_vertex*>::iterator it = M->vertexmap().begin();
  for (int i=0; it != M->vertexmap().end(); it++, i++) {
    dbmsh3d_vertex* v = (*it).second;

    xyz[i][0] = (float) v->pt().x();
    xyz[i][1] = (float) v->pt().y();
    xyz[i][2] = (float) v->pt().z();
    v->set_vid (i);
  }
 
  //Assign faces
  unsigned int k = 0;
  
  vcl_map<int, dbmsh3d_face*>::iterator fit = M->facemap().begin();
  for (; fit != M->facemap().end(); fit++) {
    dbmsh3d_face* F = (*fit).second;
    assert (F->vertices().size() > 2);

    for (unsigned int j=0; j<F->vertices().size(); j++) {
      //Note the asssumption about the vid
      const dbmsh3d_vertex* v = (const dbmsh3d_vertex*) F->vertices(j);
      ind[k] = v->vid();
      k++;
    }
    //Add the final '-1'
    ind[k] = -1;
    k++;
  }
  assert (k == n_ind);

  vp->vertex.setValues (0, nVertices, xyz);

  delete []xyz;
}

void _draw_M_mhe_geom (dbmsh3d_mesh* M, SoVertexProperty* vp, int n_ind, int* ind)
{
  int nVertices = M->vertexmap().size();
  float (*xyz)[3] = new float[nVertices][3];

  //Assign vertices
  vcl_map<int, dbmsh3d_vertex*>::iterator it = M->vertexmap().begin();
  for (int i=0; it != M->vertexmap().end(); it++, i++) {
    dbmsh3d_vertex* v = (*it).second;

    xyz[i][0] = (float) v->pt().x();
    xyz[i][1] = (float) v->pt().y();
    xyz[i][2] = (float) v->pt().z();
    v->set_vid (i);
  }
 
  //Assign faces
  unsigned int k = 0;
  
  vcl_map<int, dbmsh3d_face*>::iterator fit = M->facemap().begin();
  for (; fit != M->facemap().end(); fit++) {
    dbmsh3d_face* F = (*fit).second;
    vcl_vector<dbmsh3d_vertex*> vertices;
    F->get_bnd_Vs (vertices);
    assert (vertices.size() > 2);

    for (unsigned int j=0; j<vertices.size(); j++) {
      //Note the asssumption about the vid
      const dbmsh3d_vertex* v = (const dbmsh3d_vertex*) vertices[j];
      ind[k] = v->vid();
      k++;
    }
    //Add the final '-1'
    ind[k] = -1;
    k++;
  }
  assert (k == n_ind);

  vp->vertex.setValues (0, nVertices, xyz);

  delete []xyz;
}

void draw_M_ifs_geom (SoGroup* root, dbmsh3d_mesh* M)
{
  SoVertexProperty* vp = new SoVertexProperty;
  unsigned int n_ind = M->_count_faces_indices_ifs();
  int* ind = new int [n_ind];

  _draw_M_ifs_geom (M, vp, n_ind, ind);

  SoIndexedFaceSet* indexedFaceSet = new SoIndexedFaceSet ();
  indexedFaceSet->vertexProperty = vp;
  indexedFaceSet->coordIndex.setValues (0, n_ind, ind);

  delete []ind;

  root->addChild (indexedFaceSet);
}

void draw_M_mhe_geom (SoGroup* root, dbmsh3d_mesh* M)
{
  SoVertexProperty* vp = new SoVertexProperty;
  unsigned int n_ind = M->_count_faces_indices_mhe();
  int* ind = new int [n_ind];

  _draw_M_mhe_geom (M, vp, n_ind, ind);

  SoIndexedFaceSet* indexedFaceSet = new SoIndexedFaceSet ();
  indexedFaceSet->vertexProperty = vp;
  indexedFaceSet->coordIndex.setValues (0, n_ind, ind);

  delete []ind;

  root->addChild (indexedFaceSet);
}

SoSeparator* draw_M (dbmsh3d_mesh* M, const bool b_shape_hints,
                     const float transp, const int colorcode)
{
  SoSeparator* root = new SoSeparator;

  //Assign Material for search and change interactively...
  SoMaterial* material = new SoMaterial;
  material->setName (SbName("boundary_mesh_material"));

  if (colorcode == COLOR_GOLD) { //gold
    material->ambientColor.setValue (.3f, .1f, .1f);
    material->diffuseColor.setValue (.8f, .7f, .2f);
    material->specularColor.setValue (.4f, .3f, .1f);
    material->shininess = .4f;
  }
  else if (colorcode == COLOR_SILVER) { //silver
    material->ambientColor.setValue(.2f, .2f, .2f);
    material->diffuseColor.setValue(.5f, .5f, .5f);
    material->specularColor.setValue (.4f, .4f, .4f);
  }
  else if (colorcode != 0) {
    material->diffuseColor = color_from_code (colorcode);
  }

  material->transparency = transp;
  root->addChild (material);

  //Put the SoShapeHints,
  //see http://doc.coin3d.org/Coin/classSoShapeHints.html
  if (b_shape_hints) {
    SoShapeHints* hints = new SoShapeHints();
    hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    ///hints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
    hints->faceType = SoShapeHints::UNKNOWN_FACE_TYPE; //this may slow down the rendering!
    root->addChild (hints);
  }

  //Assign Style for search and change interactively...
  SoDrawStyle* ds = new SoDrawStyle;
  ds->setName( SbName("boundary_mesh_style") );
  root->addChild (ds);

  if (M->is_MHE())
    draw_M_mhe_geom (root, M);
  else
    draw_M_ifs_geom (root, M);

  return root;
}

// ##################################################################

//: Similar to the above but only draw the marked face.
void _draw_M_visited_ifs_geom (dbmsh3d_mesh* M, SoVertexProperty* vp, int* ind)
{
  int nVertices = M->vertexmap().size();
  float (*xyz)[3] = new float[nVertices][3];
  unsigned int n_ind = M->_count_visited_faces_indices_ifs();

  //Assign vertices
  vcl_map<int, dbmsh3d_vertex*>::iterator it = M->vertexmap().begin();
  for (int i=0; it != M->vertexmap().end(); it++, i++) {
    dbmsh3d_vertex* V = (*it).second;

    xyz[i][0] = (float) V->pt().x();
    xyz[i][1] = (float) V->pt().y();
    xyz[i][2] = (float) V->pt().z();
    V->set_vid (i);
  }
 
  //Assign faces
  unsigned int k = 0;
  
  vcl_map<int, dbmsh3d_face*>::iterator fit = M->facemap().begin();
  for (; fit != M->facemap().end(); fit++) {
    dbmsh3d_face* F = (*fit).second;
    if (F->b_visited() == false)
      continue; //skip the unmarked F.
    vcl_vector<dbmsh3d_vertex*> vertices;
    F->get_bnd_Vs (vertices);
    assert (vertices.size() > 2);

    for (unsigned int j=0; j<vertices.size(); j++) {
      //Note the asssumption about the vid
      const dbmsh3d_vertex* V = (const dbmsh3d_vertex*) vertices[j];
      ind[k] = V->vid();
      k++;
    }
    //Add the final '-1'
    ind[k] = -1;
    k++;
  }
  assert (k == n_ind);

  vp->vertex.setValues (0, nVertices, xyz);

  delete []xyz;
}

void draw_M_visited_ifs_geom (SoGroup* root, dbmsh3d_mesh* M)
{
  SoVertexProperty* vp = new SoVertexProperty;
  unsigned int n_ind = M->_count_visited_faces_indices_ifs();
  int* ind = new int [n_ind];

  _draw_M_visited_ifs_geom (M, vp, ind);

  SoIndexedFaceSet* indexedFaceSet = new SoIndexedFaceSet ();
  indexedFaceSet->vertexProperty = vp;
  indexedFaceSet->coordIndex.setValues (0, n_ind, ind);

  delete []ind;

  root->addChild (indexedFaceSet);
}

SoSeparator* draw_M_ifs_visited (dbmsh3d_mesh* M, const int colorcode,
                                 const bool b_shape_hints, const float transp)
{
  SoSeparator* root = new SoSeparator;

  //Assign Material for search and change interactively...
  SoMaterial* material = new SoMaterial;
  material->setName (SbName("boundary_mesh_material"));

  if (colorcode == COLOR_GOLD) { //gold
    material->ambientColor.setValue (.3f, .1f, .1f);
    material->diffuseColor.setValue (.8f, .7f, .2f);
    material->specularColor.setValue (.4f, .3f, .1f);
    material->shininess = .4f;
  }
  else if (colorcode == COLOR_SILVER) { //silver
    material->ambientColor.setValue(.2f, .2f, .2f);
    material->diffuseColor.setValue(.8f, .8f, .8f);
  }
  else if (colorcode != 0) {
    material->diffuseColor = color_from_code (colorcode);
  }

  material->transparency = transp;
  root->addChild (material);

  //Put the SoShapeHints,
  //see http://doc.coin3d.org/Coin/classSoShapeHints.html
  if (b_shape_hints) {
    SoShapeHints* hints = new SoShapeHints();
    hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    ///hints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
    hints->faceType = SoShapeHints::UNKNOWN_FACE_TYPE; //this may slow down the rendering!
    root->addChild (hints);
  }

  //Assign Style for search and change interactively...
  SoDrawStyle* ds = new SoDrawStyle;
  ds->setName( SbName("boundary_mesh_style") );
  root->addChild (ds);

  draw_M_visited_ifs_geom (root, M);

  return root;
}

// ##################################################################

#define NON_M_1RING_COLOR   SbColor(1.0f, 0.0f, 0.0f) //Red
#define NON_1RING_COLOR     SbColor(0.0f, 0.7f, 0.0f) //Green

//: option 1: draw non-manifold-1-ring ones in RED.
//  option 2: draw non-1-ring vertices in BLUE. 
//  option 3: draw both. 
SoSeparator* draw_M_topo_vertices (dbmsh3d_mesh* M, const int option,
                                   const float size, const bool user_defined_class)
{
  vul_printf (vcl_cout, "draw_M_topo_vertices(): ");
  unsigned int n_non_m_1ring_v = 0;
  unsigned int n_non_1ring_v = 0;
  SoSeparator* root = new SoSeparator;

  vcl_map<int, dbmsh3d_vertex*>::iterator it = M->vertexmap().begin();
  for (; it != M->vertexmap().end(); it++) {
    dbmsh3d_vertex* V = (*it).second;
    VTOPO_TYPE type = V->detect_vtopo_type ();   
    if ((option==1 || option==3) && type == VTOPO_NON_MANIFOLD_1RING) {
      n_non_m_1ring_v++;
      if (user_defined_class)
        root->addChild (draw_vertex_vispt_SoCube (V, NON_M_1RING_COLOR, size));
      else
        root->addChild (draw_vertex_SoCube (V, NON_M_1RING_COLOR, size));
    }
    else if ((option==2 || option==3) && type != VTOPO_2_MANIFOLD_1RING) {
      n_non_1ring_v++;
      if (user_defined_class)
        root->addChild (draw_vertex_vispt_SoCube (V, NON_1RING_COLOR, size));
      else
        root->addChild (draw_vertex_SoCube (V, NON_1RING_COLOR, size));
    }
  }
  vul_printf (vcl_cout, "%u non-manifold 1-ring vertices.\n", n_non_m_1ring_v);
  vul_printf (vcl_cout, "%u non-1-ring vertices.\n", n_non_1ring_v);

  return root;
}

// ##################################################################

//
SoSeparator* draw_M_edges_idv (dbmsh3d_mesh* M, const SbColor& color, 
                               const float width, const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;
  
  //color
  SoBaseColor *basecolor = new SoBaseColor;
  basecolor->rgb = color;
  root->addChild (basecolor);

  //line width
  SoDrawStyle* ds = new SoDrawStyle;
  ds->setName (SbName("mesh_edge_style"));
  ds->lineWidth.setValue (width);
  root->addChild(ds);

  if (user_defined_class) {
    vcl_map<int, dbmsh3d_edge*>::iterator it = M->edgemap().begin();
    for (; it != M->edgemap().end(); it++) {
      dbmsh3d_edge* E = (*it).second;
      draw_edge_geom (root, E, true);
    }
  }
  else {
    vcl_map<int, dbmsh3d_edge*>::iterator it = M->edgemap().begin();
    for (; it != M->edgemap().end(); it++) {
      dbmsh3d_edge* E = (*it).second;
      draw_edge_geom (root, E, false);
    }
  }

  return root;
}

//
SoSeparator* draw_M_edges (dbmsh3d_mesh* M, const SbColor& color, const float width)
{
  SoSeparator* root = new SoSeparator;
  
  //color
  SoBaseColor *basecolor = new SoBaseColor;
  basecolor->rgb = color;
  root->addChild (basecolor);

  //line width
  SoDrawStyle* ds = new SoDrawStyle;
  ds->setName (SbName("mesh_edge_style"));
  ds->lineWidth.setValue (width);
  root->addChild(ds);

  
  if (M->is_MHE())
    draw_M_mhe_edges_geom (root, M);
  else
    draw_M_ifs_edges_geom (root, M);

  return root;
}

//: draw mesh edges in indexed-line-set.
//
void draw_M_mhe_edges_geom (SoSeparator* root, dbmsh3d_mesh* M)
{
  unsigned int nVertices = M->edgemap().size() * 2;
  float (*xyz)[3] = new float[nVertices][3];
  unsigned int nLinesIndices = M->edgemap().size() * 3; //sid : eid : -1
  int* ind = new int [nLinesIndices];

  //Assign vertices and lines
  vcl_map<int, dbmsh3d_edge*>::iterator eit = M->edgemap().begin();
  for (unsigned int i=0; eit != M->edgemap().end(); eit++, i++) {
    dbmsh3d_edge* E = (*eit).second;
    vgl_point_3d<double> Ps = E->sV()->pt();
    vgl_point_3d<double> Pe = E->eV()->pt();
    xyz[i*2][0] = (float) Ps.x();
    xyz[i*2][1] = (float) Ps.y();
    xyz[i*2][2] = (float) Ps.z();
    xyz[i*2+1][0] = (float) Pe.x();
    xyz[i*2+1][1] = (float) Pe.y();
    xyz[i*2+1][2] = (float) Pe.z();
    ind[i*3] = i*2;
    ind[i*3+1] = i*2+1;
    ind[i*3+2] = -1;
  }

  SoVertexProperty* vp = new SoVertexProperty;
  vp->vertex.setValues (0, nVertices, xyz);

  SoIndexedLineSet* indexedLineSet = new SoIndexedLineSet ();
  indexedLineSet->vertexProperty = vp;
  indexedLineSet->coordIndex.setValues (0, nLinesIndices, ind);

  delete []ind;
  delete []xyz;

  root->addChild (indexedLineSet);
}

void draw_M_ifs_edges_geom (SoSeparator* root, dbmsh3d_mesh* M)
{
  unsigned nEdges = M->count_ifs_dup_edges ();
  unsigned int nVertices = nEdges * 2;
  float (*xyz)[3] = new float[nVertices][3];
  unsigned int nLinesIndices = nEdges * 3; //sid : eid : -1
  int* ind = new int [nLinesIndices];

  //Assign vertices and lines
  unsigned int i=0;
  vcl_map<int, dbmsh3d_face*>::iterator fit = M->facemap().begin();
  for (; fit != M->facemap().end(); fit++) {
    dbmsh3d_face* F = (*fit).second;
    //Go through each implicit incident E and draw it.
    for (int j=0; j<int(F->vertices().size()); j++) {
      int k = (j+1) % F->vertices().size();
      dbmsh3d_vertex* sV = F->vertices(j); 
      dbmsh3d_vertex* eV = F->vertices(k);
      vgl_point_3d<double> Ps = sV->pt();
      vgl_point_3d<double> Pe = eV->pt();
      xyz[i*2][0] = (float) Ps.x();
      xyz[i*2][1] = (float) Ps.y();
      xyz[i*2][2] = (float) Ps.z();
      xyz[i*2+1][0] = (float) Pe.x();
      xyz[i*2+1][1] = (float) Pe.y();
      xyz[i*2+1][2] = (float) Pe.z();
      ind[i*3] = i*2;
      ind[i*3+1] = i*2+1;
      ind[i*3+2] = -1;
      i++;
    }
  }
  assert (i == nEdges);

  SoVertexProperty* vp = new SoVertexProperty;
  vp->vertex.setValues (0, nVertices, xyz);

  SoIndexedLineSet* indexedLineSet = new SoIndexedLineSet ();
  indexedLineSet->vertexProperty = vp;
  indexedLineSet->coordIndex.setValues (0, nLinesIndices, ind);

  delete []ind;
  delete []xyz;

  root->addChild (indexedLineSet);
}

SoSeparator* draw_M_bndcurve (dbmsh3d_mesh* M, const int colorcode, const float width)
{
  SoSeparator* root = new SoSeparator;
  //Color
  SoBaseColor *basecolor = new SoBaseColor;
  basecolor->rgb = color_from_code (colorcode);
  root->addChild (basecolor);

  //Line width
  SoDrawStyle*  drawStyle = new SoDrawStyle;
  drawStyle->lineWidth.setValue (width);
  root->addChild (drawStyle);

  dbmsh3d_bnd_chain_set* bnd_chain_set = new dbmsh3d_bnd_chain_set (M);
  bnd_chain_set->detect_bnd_chains ();

  //Draw each bnd_chain in polyline
  vcl_vector<dbmsh3d_bnd_chain*>::iterator bit = bnd_chain_set->chainset().begin();
  for (; bit != bnd_chain_set->chainset().end(); bit++) {
    dbmsh3d_bnd_chain* BC = (*bit);

    vcl_vector<vgl_point_3d<double> > polyline_vertices;
    //Trace each bnd_chain to a vector of points.
    BC->trace_polyline (polyline_vertices);
    draw_polyline_geom (root, polyline_vertices);
  }

  delete bnd_chain_set;
  return root;
}

SoSeparator* draw_M_faces_idv (dbmsh3d_mesh* M, 
                               const bool b_shape_hints, const float trans,
                               const int colorcode, const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;

  SoMaterial* material = new SoMaterial;
  material->setName (SbName("boundary_mesh_material"));
  
  if (colorcode != 0) //if the color is non-black, assign it
    material->diffuseColor = color_from_code (colorcode);
  material->transparency = trans;
  root->addChild (material);

  //: put the SoShapeHints,
  //  see http://doc.coin3d.org/Coin/classSoShapeHints.html
  if (b_shape_hints) {
    SoShapeHints* hints = new SoShapeHints();
    hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    root->addChild (hints);
  }

  //: Assign Style for search and change interactively...
  SoDrawStyle* ds = new SoDrawStyle;
  ds->setName( SbName("boundary_mesh_style") );
  root->addChild (ds);

  if (user_defined_class) {
    vcl_map<int, dbmsh3d_face*>::iterator it = M->facemap().begin();
    for (; it != M->facemap().end(); it++) {
      dbmsh3d_face* F = (*it).second;
      draw_F_geom_vispt (root, F);
    }
  }
  else {
    vcl_map<int, dbmsh3d_face*>::iterator it = M->facemap().begin();
    for (; it != M->facemap().end(); it++) {
      dbmsh3d_face* F = (*it).second;
      draw_F_geom (root, F);
    }
  }

  return root;
}

SoSeparator* draw_M_color (dbmsh3d_mesh* M, 
                           const bool b_shape_hints, const float trans,
                           const vcl_vector<SbColor>& color_set,
                           const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;

  SoMaterial* material = new SoMaterial;
  material->setName (SbName("boundary_mesh_material"));
  
  //Put the SoShapeHints, http://doc.coin3d.org/Coin/classSoShapeHints.html
  if (b_shape_hints) {
    SoShapeHints* hints = new SoShapeHints();
    hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    root->addChild (hints);
  }

  vcl_map<int, dbmsh3d_face*>::iterator it = M->facemap().begin();
  for (; it != M->facemap().end(); it++) {
    dbmsh3d_face* F = (*it).second;
    root->addChild (draw_F (F, color_set[F->id()], trans, user_defined_class));
  }

  return root;
}

SoSeparator* draw_mesh_vertices (dbmsh3d_mesh* M, const float size)
{
  SoSeparator* root = new SoSeparator;

  vcl_map<int, dbmsh3d_vertex*>::iterator it = M->vertexmap().begin();
  for (; it != M->vertexmap().end(); it++) {
    dbmsh3d_vertex* V = (*it).second;
    root->addChild (draw_vertex_geom_vispt_SoCube (V, size));
  }

  return root;
}

//######################################################################

SoSeparator* draw_M_textured (dbmsh3d_textured_mesh_mc* M)
{
  SoSeparator* root = new SoSeparator;

  vcl_map<int, dbmsh3d_face*>::iterator it = M->facemap().begin();
  for (; it != M->facemap().end(); it++) {
    dbmsh3d_textured_face_mc* F = (dbmsh3d_textured_face_mc*) (*it).second;
    root->addChild (draw_F_textured (F));
  }

  return root;
}

//######################################################################
//       Visualize Mesh Geometry & Topology Info

#define FACE_ID_COLOR SbColor (0.0f, 1.0f, 0.0f); //Green

SoSeparator* draw_M_bnd_faces_cost_col (dbmsh3d_mesh* M, const bool draw_idv,
                                        const bool showid, const float transp)
{
  vul_printf (vcl_cout, "draw_M_bnd_faces_cost_col()\n");

  SoSeparator* root = new SoSeparator;

  SoDrawStyle* ds = new SoDrawStyle;
  ds->setName (SbName("boundary_mesh_style"));
  root->addChild (ds);

  ///SoMaterial* obtuseMaterial = new SoMaterial;
  ///obtuseMaterial->diffuseColor.setValue (color_from_code (COLOR_GRAYBLUE));
  ///obtuseMaterial->transparency = transp;

  ///SoMaterial* acuteMaterial = new SoMaterial;
  ///acuteMaterial->diffuseColor.setValue (color_from_code (COLOR_LIGHTGRAY));
  ///acuteMaterial->transparency = transp;

  //Make displpay non-orientable.
  SoShapeHints* hints = new SoShapeHints();
  hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  root->addChild (hints);

  unsigned int n_acute = 0;  
  unsigned int n_obtuse = 0;

  if (showid == false) {
    //draw all obtuse triangles in a batch.
    vcl_map<int, dbmsh3d_face*>::iterator it = M->facemap().begin();
    for (; it != M->facemap().end(); it++) {
      dbmsh3d_face* F = (*it).second;
      vcl_vector<dbmsh3d_vertex*> vertices;
      F->get_bnd_Vs (vertices);
      if (is_tri_non_acute(vertices)) {
        n_obtuse++;
        F->set_visited (1);
      }
      else
        F->set_visited (0);
    }
    root->addChild (draw_M_ifs_visited (M, COLOR_GRAYBLUE, false, transp));

    //draw all acute triangles in a batch.
    it = M->facemap().begin();
    for (; it != M->facemap().end(); it++) {
      dbmsh3d_face* F = (*it).second;
      vcl_vector<dbmsh3d_vertex*> vertices;
      F->get_bnd_Vs (vertices);
      if (is_tri_non_acute(vertices) == false) {
        n_acute++;
        F->set_visited (1);
      }
      else
        F->set_visited (0);
    }
    root->addChild (draw_M_ifs_visited (M, COLOR_LIGHTGRAY, false, transp));
  }
  else {
    //id text color
    ///SoBaseColor* idbasecolor = new SoBaseColor;
    ///idbasecolor->rgb = FACE_ID_COLOR;

    vcl_map<int, dbmsh3d_face*>::iterator it = M->facemap().begin();
    for (; it != M->facemap().end(); it++) {
      dbmsh3d_face* F = (*it).second;
      vcl_vector<dbmsh3d_vertex*> vertices;
      F->get_bnd_Vs (vertices);
      //Determine color according to its cost type.
      if (is_tri_non_acute(vertices)) {
        root->addChild (draw_F_with_id (F, color_from_code (COLOR_GRAYBLUE), transp));
        n_obtuse++;
      }
      else {
        root->addChild (draw_F_with_id (F, color_from_code (COLOR_LIGHTGRAY), transp));
        n_acute++;
      }
    }
  }
  vul_printf (vcl_cout, "\t%u acute, %u obtuse faces drawn (totally %u).\n", 
               n_acute, n_obtuse, M->facemap().size());
  return root;
}

SoSeparator* draw_M_bnd_faces_topo_col (dbmsh3d_mesh* M, const bool draw_idv, 
                                        const bool showid, const float transp,
                                        const bool user_defined_class)
{
  vul_printf (vcl_cout, "draw_M_bnd_faces_topo_col()\n");
  SoSeparator* root = new SoSeparator;

  SoDrawStyle* ds = new SoDrawStyle;
  ds->setName (SbName("boundary_mesh_style"));
  root->addChild (ds);

  //Make displpay non-orientable.
  SoShapeHints* hints = new SoShapeHints();
  hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  root->addChild (hints);
  
  unsigned int n_111=0, n_112=0, n_122=0, n_222=0; 
  unsigned int n_113=0, n_133=0, n_333=0;
  unsigned int n_123=0, n_223=0, n_233=0, n_e4p=0, n_error=0; 

  VIS_COLOR_CODE colorcode;
  SbColor color;

  vcl_map<int, dbmsh3d_face*>::iterator it = M->facemap().begin();
  for (; it != M->facemap().end(); it++) {
    dbmsh3d_face* F = (*it).second;
    TRIFACE_TYPE type = F->tri_get_topo_type();

    //Determine color according to its topological type.
    colorcode = get_M_face_topo_color (type);
    switch (type) {
    case TRIFACE_111:
      n_111++;
    break;
    case TRIFACE_112: 
      n_112++;
    break;
    case TRIFACE_122:
      n_122++;
    break;
    case TRIFACE_222:
      n_222++;
    break;
    case TRIFACE_113P:
      n_113++;
    break;
    case TRIFACE_13P3P:
      n_133++;
    break;
    case TRIFACE_3P3P3P:
      n_333++;
    break;    
    case TRIFACE_123P:
      n_123++;
    break;
    case TRIFACE_223P:
      n_223++;
    break;
    case TRIFACE_23P3P:
      n_233++;
    break;
    case TRIFACE_E4P:
      n_e4p++;
    break;
    default:
      assert (0);
      n_error++;
    break;
    }

    //Skip drawing individual 222 triangles
    if (draw_idv == false && type == TRIFACE_222) {
      F->set_i_visited (1);
    }
    else {
      F->set_i_visited (0);
      color = color_from_code (colorcode);
      if (showid == false)
        root->addChild (draw_F (F, color, transp, user_defined_class));
      else
        root->addChild (draw_F_with_id (F, color, transp, user_defined_class));
    }
  }
  
  //draw all 2-2-2 triangles in a batch.  
  if (draw_idv == false) {
    colorcode = get_M_face_topo_color (TRIFACE_222);
    root->addChild (draw_M_ifs_visited (M, colorcode, false, transp));
  }

  vul_printf (vcl_cout, "\tAmong %u mesh faces:\n", M->facemap().size());
  vul_printf (vcl_cout, "\t%u 2-manifold faces: %u (222) interior DARKGRAY,\n", n_222+n_112+n_122, n_222);
  vul_printf (vcl_cout, "\t  + boundary: %u (112) DARKGREEN + %u (122) DARKBLUE.\n", n_112, n_122);

  vul_printf (vcl_cout, "\t%u extraneous:\n", n_113+n_133+n_333);
  vul_printf (vcl_cout, "\t  %u (113) PINK + %u (133) RED + %u (333) DARKRED.\n", n_133, n_133, n_333);

  vul_printf (vcl_cout, "\t%u near ridge faces:\n", n_123+n_223+n_233);
  vul_printf (vcl_cout, "\t  %u (123) CYAN + %u (223) BLUE + %u (233) GREEN.\n", n_123, n_223, n_233);  

  vul_printf (vcl_cout, "\t%u isolated (111) YELLOW.\n", n_111);
  vul_printf (vcl_cout, "\t%u degenerate polygon (e4+) GRAY.\n", n_e4p);
  vul_printf (vcl_cout, "\t%u error in GOLD.\n", n_error);

  return root;
}

VIS_COLOR_CODE get_M_face_topo_color (const TRIFACE_TYPE type)
{
  switch (type) {
  case TRIFACE_111:
    return COLOR_YELLOW;
  case TRIFACE_112: 
    return COLOR_DARKGREEN;
  case TRIFACE_122:
    return COLOR_DARKBLUE;
  case TRIFACE_222:
    return COLOR_GRAY;
  case TRIFACE_113P:
    return COLOR_PINK;
  case TRIFACE_13P3P:
    return COLOR_RED;
  case TRIFACE_3P3P3P:
    return COLOR_DARKRED;
  case TRIFACE_123P:
    return COLOR_CYAN;
  case TRIFACE_223P:
    return COLOR_BLUE;
  case TRIFACE_23P3P:
    return COLOR_GREEN;
  case TRIFACE_E4P:
    return COLOR_DARKGRAY;
  default:
    return COLOR_GOLD;
  }
}

// #############################  Animations  #################################

static int timer_num_counter = 0;

//: for each timer event, turn on one more SoSwitch.
static void timerCallback (void *data, SoSensor* sensor)
{

  //Animation parameters.
  //-n2: Delay time before animation starts: def. 100 (around 3 secs).
  int N_DELAY = dbmsh3d_cmd_n2();
  if (N_DELAY == 1)
    N_DELAY = 100;
  //-n3: Re-iterate delay time: def. 100 (around 3 secs).
  int N_RESET_ITER = dbmsh3d_cmd_n3();
  if (N_RESET_ITER == 1)
    N_RESET_ITER = 100;

  SoSeparator* animRoot = (SoSeparator*) data;  
  // animRoot.numChildren = total iterations.
  int total_iters = animRoot->getNumChildren(); 

  int counter = timer_num_counter - N_DELAY;

  int show_iter = int(counter / total_iters) % 2 == 0;

  int reset_iter = (counter-total_iters) % N_RESET_ITER == N_RESET_ITER-1;
    //int(counter / total_iters) % N_RESET_ITER == N_RESET_ITER-1;

  if (counter >=0 && counter < total_iters) { //show_iter) { //
    //If haven't gone through all the iterations show the next one
    SoSwitch* curSwitch = (SoSwitch*) animRoot->getChild( counter );
    curSwitch->whichChild = SO_SWITCH_ALL;
  }
  timer_num_counter++;

  if (reset_iter) { //All the iterations have been shown, hide them all
    for (int i=0; i<total_iters; i++) {
      SoSwitch* curSwitch = (SoSwitch*) animRoot->getChild (i);
      curSwitch->whichChild = SO_SWITCH_NONE;
    }
    timer_num_counter = 0;
  }

}

SoSeparator* draw_M_bnd_faces_anim (dbmsh3d_mesh* M, const int nF_batch)
{  
  vul_printf (vcl_cout, "  draw_M_bnd_faces_anim().\n", nF_batch);
  SoSeparator* root = new SoSeparator;

  SoDrawStyle* ds = new SoDrawStyle;
  ds->setName (SbName("boundary_mesh_style"));
  root->addChild (ds);

  SoShapeHints* hints = new SoShapeHints();
  hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  root->addChild (hints);

  SoSeparator* animRoot = new SoSeparator;
  root->addChild (animRoot);
    
  unsigned int frame = 0;
  vcl_vector<dbmsh3d_face*> faces;
  vcl_map<int, dbmsh3d_face*>::iterator it = M->facemap().begin();
  for (unsigned int i=0; it != M->facemap().end(); it++, i++) {
    dbmsh3d_face* F = (*it).second;
    faces.push_back (F);    

    if (i % nF_batch == nF_batch-1) {
      draw_faces_in_switch (animRoot, faces);
      faces.clear();
      frame++;
    }
  }
  //draw the last remaining faces
  draw_faces_in_switch (animRoot, faces);
  faces.clear();
  frame++;
  vul_printf (vcl_cout, "    total animation frames: %u.\n", frame);

  SoTimerSensor* timer = new SoTimerSensor (timerCallback, animRoot);
  timer->setInterval (0.01f);
  timer->schedule();

  return root;
}

void draw_faces_in_switch (SoSeparator* root, const vcl_vector<dbmsh3d_face*>& faces)
{
  SoSwitch* sw = new SoSwitch;
  sw->whichChild = SO_SWITCH_NONE;  
  root->addChild (sw);

  //Prepare the set of all vertices for the input faces.
  vcl_set<dbmsh3d_vertex*> vertex_set;
  for (unsigned int i=0; i<faces.size(); i++) {
    dbmsh3d_face* F = faces[i];
    vcl_vector<dbmsh3d_vertex*> vs;
    F->get_bnd_Vs (vs);
    for (unsigned int j=0; j<vs.size(); j++)
      vertex_set.insert (vs[j]);
  }

  //Put all points into a vector.  
  vcl_vector<vgl_point_3d<double> > ifs_pts;
  vcl_set<dbmsh3d_vertex*>::iterator vit = vertex_set.begin();
  for (unsigned int i=0; vit != vertex_set.end(); vit++, i++) {
    dbmsh3d_vertex* V = (*vit);
    V->set_vid (i);
    ifs_pts.push_back (V->pt());
  }
  vertex_set.clear();

  //Put all faces into the IFS.
  vcl_vector<vcl_vector<int> > ifs_faces;
  for (unsigned int i=0; i<faces.size(); i++) {
    dbmsh3d_face* F = faces[i];  
    vcl_vector<dbmsh3d_vertex*> vs;
    F->get_bnd_Vs (vs);
    vcl_vector<int> face_vids;
    for (unsigned int j=0; j<vs.size(); j++)
      face_vids.push_back (vs[j]->vid());
    ifs_faces.push_back (face_vids);
  }

  sw->addChild (draw_ifs (ifs_pts, ifs_faces, COLOR_SILVER, true));
  ifs_pts.clear();
  ifs_faces.clear();
}


