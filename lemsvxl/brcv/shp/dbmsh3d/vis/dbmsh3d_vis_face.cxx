//: This is lemsvxlsrc/brcv/shp/dbmsh3d/vis/dbmsh3d_vis_mesh.cxx
//  MingChing Chang
//  May 03, 2005.

#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <dbmsh3d/dbmsh3d_mesh.h>

#include <dbmsh3d/vis/dbmsh3d_vis_backpt.h>
#include <dbmsh3d/vis/dbmsh3d_vis_face.h>
#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoFaceSet.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoTexture2.h>

//: Visualize the geometry of a polygonal face.
//
SoCoordinate3* _draw_F_ifs_geom (SoGroup* group, const dbmsh3d_face* F)
{
  const unsigned int nVertices = F->vertices().size();
  if (nVertices<3)
    return NULL;

  SoCoordinate3* coords = new SoCoordinate3;
  SbVec3f* verts = new SbVec3f[nVertices];  
  // Put each vertex position into a coordinate array.
  // Skip the vertex if it is too close to any other vertex.
  unsigned int count = 0;
  for (unsigned int j=0; j<nVertices; j++) {
    const dbmsh3d_vertex* V = (const dbmsh3d_vertex*) F->vertices(j);
    //Check if the vertex is too close to any previous vertex.
    if (_check_duplicate (verts, count, V->pt())) {
      verts[count] = SbVec3f (V->pt().x(), V->pt().y(), V->pt().z());
      count++;
    }
  }

  // Ignore the polygon with fewer than 3 sides.
  if (count > 2) {
    coords->point.setValues (0, count, verts); //nVertices
    group->addChild (coords);
  }
  else
    vul_printf (vcl_cout, "  -- Skip drawing face %d: only %d salient edges.\n", F->id(), count);
  delete[] verts;
  return coords;
}

//: Visualize the geometry of a polygonal face.
//
SoCoordinate3* _draw_F_mhe_geom (SoGroup* group, const dbmsh3d_face* F)
{
  vcl_vector<dbmsh3d_vertex*> vertices;
  F->get_bnd_Vs (vertices);
  const unsigned int nVertices = vertices.size();
  if (nVertices<3)
    return NULL;

  SoCoordinate3* coords = new SoCoordinate3;
  SbVec3f* verts = new SbVec3f[nVertices];  
  // Put each vertex position into a coordinate array.
  // Skip the vertex if it is too close to any other vertex.
  unsigned int count = 0;
  for (unsigned int j=0; j<nVertices; j++) {
    const dbmsh3d_vertex* V = (const dbmsh3d_vertex*) vertices[j];
    //Check if the vertex is too close to any previous vertex.
    if (_check_duplicate (verts, count, V->pt())) {
      verts[count] = SbVec3f (V->pt().x(), V->pt().y(), V->pt().z());
      count++;
    }
  }

  // Ignore the polygon with fewer than 3 sides.
  if (count > 2) {
    coords->point.setValues (0, count, verts); //nVertices
    group->addChild (coords);
  }
  else
    vul_printf (vcl_cout, "  -- Skip drawing face %d: only %d salient edges.\n", F->id(), count);
  delete[] verts;
  return coords;
}

//: Check if the vertex is too close to any previous vertex.
//  return true if no duplicate.
bool _check_duplicate (SbVec3f* verts, unsigned int count, const vgl_point_3d<double>& pt)
{ 
  for (unsigned int k=0; k<count; k++) {
    if (dbmsh3d_vis_too_close (verts[k][0], verts[k][1], verts[k][2], 
                               (float) pt.x(), (float) pt.y(), (float) pt.z()))
      return false;
  }
  return true;
}

void draw_F_geom (SoGroup* group, const dbmsh3d_face* F)
{
  if (F->vertices().size() != 0)
    _draw_F_ifs_geom (group, F);
  else
    _draw_F_mhe_geom (group, F);

  SoFaceSet* faceSet = new SoFaceSet ();
  group->addChild (faceSet);
}

void draw_F_geom_vispt (SoGroup* group, const dbmsh3d_face* F)
{
  if (F->vertices().size() != 0)
    _draw_F_ifs_geom (group, F);
  else
    _draw_F_mhe_geom (group, F);

  dbmsh3dvis_face_SoFaceSet* faceSet = new dbmsh3dvis_face_SoFaceSet (F);
  group->addChild (faceSet);
}

SoSeparator* draw_F (const dbmsh3d_face* F,
                     const SbColor& color, const float transp,
                     const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;
  SoMaterial *material = new SoMaterial;
  material->diffuseColor.setValue (color);
  material->emissiveColor.setValue (color/2);
  material->transparency = transp;
  root->addChild (material);

  if (user_defined_class)
    draw_F_geom_vispt (root, F);
  else
    draw_F_geom (root, F);

  return root;
}

SoSeparator* draw_F (const dbmsh3d_face* F,
                     const SoMaterial* material,
                     const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;
  root->addChild ((SoMaterial*) material);

  if (user_defined_class)
    draw_F_geom_vispt (root, F);
  else
    draw_F_geom (root, F);

  return root;
}

SoSeparator* draw_F_with_id (const dbmsh3d_face* F, const SbColor& color, 
                             const float transp,
                             const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;
  SoMaterial *material = new SoMaterial;
  material->diffuseColor.setValue (color);
  material->emissiveColor.setValue (color/2);
  material->transparency = transp;
  root->addChild (material);

  if (user_defined_class)
    draw_F_geom_vispt (root, F);
  else
    draw_F_geom (root, F);

  //show id
  char buf[64];
  vcl_sprintf (buf, "%d", F->id());
  vgl_point_3d<double> cen = F->compute_center_pt ();
  root->addChild (draw_text2d (buf, cen.x(), cen.y(), cen.z(), color));

  return root;
}

// Global constants
const int texturewidth = 256;
const int textureheight = 256;

// Global pointer
unsigned char * bitmap = new unsigned char[texturewidth*textureheight];

// Function to generate a julia set
// Parameters:
//  double cr   - r julia set point
//  double ci   - i julia set point
//  float zoon  - lenth of the square to display (zoom*zoom), center (0,0)
//  int width   - width of the bitmap
//  int height  - height of the bitmap
//  int mult    - value to multiply each color by (remember (mult*n)<=256).
//  unsigned char * bmp - pointer to the bitmap
//  int n       - number of itterations 
void
julia(double cr, double ci, float zoom, int width, int height, int mult, 
      unsigned char * bmp, int n)
{
  double zr, zr_old, zi;
  int w;

  for (int y=0; y<height/2; y++)
    for (int x=0; x<width; x++) {
      zr = ((double)(x)/(double)width)*zoom-zoom/2;
      zi = ((double)(y)/(double)height)*zoom-zoom/2;
      for (w = 0; (w < n) && (zr*zr+zi*zi)<n; w++) {
        zr_old = zr;
        zr = zr*zr - zi*zi + cr;
        zi = 2*zr_old*zi + ci;
      }
      bmp[y*width+x] = w*mult;
      bmp[((height-y)*width-1)-x] = w*mult;
    }
}

SoSeparator* draw_F_textured (const dbmsh3d_textured_face_mc* F,
                              const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;

  // Generate a julia set to use as a texturemap
  ///julia(0.33, 0.43, 2.5, texturewidth, textureheight, 2, bitmap, 128);

  int width, height, plane;
  unsigned char* bitmap;
  bool r = F->get_texture_image (bitmap, width, height, plane);
  assert (r);

  SoTexture2* texture = new SoTexture2;
  texture->image.setValue(SbVec2s(height, width), plane, bitmap); //1
  delete bitmap;
  ///texture->model = SoTexture2::BLEND;
  ///texture->blendColor.setValue(1.0, 0.0, 0.0);

  // Create scene graph
  SoShapeHints * hints = new SoShapeHints;
  // Enable backface culling
  hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  ///hints->shapeType = SoShapeHints::SOLID; //solid: draw only one side.

  ///root->ref();  // increments the root's reference counter
  root->addChild (hints);
  root->addChild (texture);
  draw_F_geom (root, F);

  return root;
}


/*SoSeparator* draw_F_with_id (const dbmsh3d_face* F,
                             const SoMaterial* material,
                             const SbColor& color,
                             const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;
  root->addChild ((SoMaterial*) material);

  if (user_defined_class)
    draw_F_geom_vispt (root, F);
  else
    draw_F_geom (root, F);

  //show id
  char buf[64];
  vcl_sprintf (buf, "%d", F->id());
  vgl_point_3d<double> C = F->compute_center_pt ();
  root->addChild (draw_text2d (buf, C.x(), C.y(), C.z(), color));

  return root;
}*/





