//: This is lemsvxlsrc/brcv/shp/dbmsh3d/vis/dbmsh3d_vis_point_set.cxx
//  MingChing Chang
//  May 03, 2005.

#include <vcl_iostream.h>
#include <vul/vul_printf.h>
#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>
#include <dbmsh3d/vis/dbmsh3d_vis_backpt.h>
#include <dbmsh3d/vis/dbmsh3d_vis_vertex.h>
#include <dbmsh3d/vis/dbmsh3d_vis_point_set.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoTranslation.h>

//######################################################################

void draw_ptset_geom (SoSeparator* root, const vcl_vector<vgl_point_3d<double> >& points)
{
  //Put in the point cloud coordinates.
  SoCoordinate3 * coord3 = new SoCoordinate3;
  for (unsigned int i=0; i < points.size(); i++) {
    coord3->point.set1Value (i, points[i].x(), points[i].y(), points[i].z());
  }
  root->addChild (coord3);

  SoPointSet* ps = new SoPointSet;
  root->addChild(ps);
}

void draw_idpts_geom (SoSeparator* root, const vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& idpts)
{
  //Put in the point cloud coordinates.
  SoCoordinate3 * coord3 = new SoCoordinate3;
  for (unsigned int i=0; i < idpts.size(); i++) {
    coord3->point.set1Value (i, idpts[i].second.x(), idpts[i].second.y(), idpts[i].second.z());
  }
  root->addChild (coord3);

  SoPointSet* ps = new SoPointSet;
  root->addChild(ps);
}

void draw_oriented_ptset_geom (SoSeparator* root, 
        const vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_vector_3d<double> > >& ori_pts)
{
  //Put in the point cloud coordinates.
  SoCoordinate3 * coord3 = new SoCoordinate3;
  for (unsigned int i=0; i < ori_pts.size(); i++) {
    vgl_point_3d<double> P = ori_pts[i].first;
    coord3->point.set1Value (i, P.x(), P.y(), P.z());
  }
  root->addChild (coord3);

  SoPointSet* ps = new SoPointSet;
  root->addChild(ps);
}

//: draw all normal lines in a single SoIndexedLineSet object.
void draw_oriented_ptset_normal_geom (SoSeparator* root, 
        const vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_vector_3d<double> > >& ori_pts,
        const float normal_len)
{
  unsigned int nVertices = ori_pts.size() * 2;
  float (*xyz)[3] = new float[nVertices][3];
  unsigned int nLinesIndices = ori_pts.size() * 3; //sid : eid : -1
  int* ind = new int [nLinesIndices];

  //Assign vertices and lines
  for (unsigned int i=0; i < ori_pts.size(); i++) {
    vgl_point_3d<double> P = ori_pts[i].first;
    vgl_vector_3d<double> N = ori_pts[i].second;
    vgl_point_3d<double> P1 = P + N * normal_len;
    xyz[i*2][0] = (float) P.x();
    xyz[i*2][1] = (float) P.y();
    xyz[i*2][2] = (float) P.z();
    xyz[i*2+1][0] = (float) P1.x();
    xyz[i*2+1][1] = (float) P1.y();
    xyz[i*2+1][2] = (float) P1.z();
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

SoSeparator* draw_ptset (const vcl_vector<vgl_point_3d<double> >& pts, 
                          const int colorcode, const float size)
{
  SoSeparator* root = new SoSeparator;

  //point size
  SoDrawStyle * drawstyle = new SoDrawStyle;
  drawstyle->pointSize = size;
  root->addChild(drawstyle);

  SoBaseColor* basecol = new SoBaseColor;
  basecol->rgb = color_from_code (colorcode);
  root->addChild (basecol);

  draw_ptset_geom (root, pts);

  return root;
}

SoSeparator* draw_idpts (const vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& idpts, 
                         const int colorcode, const float size)
{
  SoSeparator* root = new SoSeparator;

  //point size
  SoDrawStyle * drawstyle = new SoDrawStyle;
  drawstyle->pointSize = size;
  root->addChild(drawstyle);

  SoBaseColor* basecol = new SoBaseColor;
  basecol->rgb = color_from_code (colorcode);
  root->addChild (basecol);

  draw_idpts_geom (root, idpts);

  return root;
}

SoSeparator* draw_oriented_ptset (
                const vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_vector_3d<double> > >& ori_pts, 
                const int point_colorcode, const int normal_colorcode, const float normal_len,
                const float point_size, const float normal_width)
{
  SoSeparator* root = new SoSeparator;
  SoSeparator* point_root = new SoSeparator;
  SoSeparator* normal_root = new SoSeparator;
  root->addChild (point_root);
  root->addChild (normal_root);

  //point size, normal line width
  SoDrawStyle * drawstyle = new SoDrawStyle;
  drawstyle->pointSize = point_size;
  drawstyle->lineWidth = normal_width;
  point_root->addChild (drawstyle);
  normal_root->addChild (drawstyle);

  SoBaseColor* point_basecol = new SoBaseColor;
  point_basecol->rgb = color_from_code (point_colorcode);
  point_root->addChild (point_basecol);

  SoBaseColor* normal_basecol = new SoBaseColor;
  normal_basecol->rgb = color_from_code (normal_colorcode);
  normal_root->addChild (normal_basecol);

  draw_oriented_ptset_geom (point_root, ori_pts);
  draw_oriented_ptset_normal_geom (normal_root, ori_pts, normal_len);

  return root;
}

//######################################################################

//: Draw the whole point set.
void draw_pt_set_geom (SoSeparator* root, dbmsh3d_pt_set* pt_set)
{
  //Put in the point cloud coordinates.
  SoCoordinate3 * coord3 = new SoCoordinate3;
  vcl_map<int, dbmsh3d_vertex*>::iterator it = pt_set->vertexmap().begin();
  for (int i=0; it != pt_set->vertexmap().end(); it++, i++) {
    dbmsh3d_vertex* V = (*it).second;
    coord3->point.set1Value (i, V->pt().x(), V->pt().y(), V->pt().z());
  }
  root->addChild (coord3);

  SoPointSet* ps = new SoPointSet;
  root->addChild(ps);
}

//: Draw only the unmeshed point set.
void draw_pt_set_geom_unmeshed (SoSeparator* root, dbmsh3d_pt_set* pt_set)
{
  //Put in the point cloud coordinates.
  SoCoordinate3 * coord3 = new SoCoordinate3;
  vcl_map<int, dbmsh3d_vertex*>::iterator it = pt_set->vertexmap().begin();
  int count = 0;
  for (int i=0; it != pt_set->vertexmap().end(); it++, i++) {
    dbmsh3d_vertex* V = (*it).second;
    if (V->b_meshed() == false) {
      coord3->point.set1Value (count, V->pt().x(), V->pt().y(), V->pt().z());
      count++;
    }
  }

  if (count != 0) {
    root->addChild (coord3);
    SoPointSet* ps = new SoPointSet;
    root->addChild(ps);
  }
}

//: Draw the whole point set.
SoSeparator* draw_ptset (dbmsh3d_pt_set* pt_set, 
                          const int colorcode, const float size,
                          const bool only_unmeshed)
{
  SoSeparator* root = new SoSeparator;

  //point size
  SoDrawStyle * drawstyle = new SoDrawStyle;
  drawstyle->pointSize = size;
  root->addChild(drawstyle);

  SoBaseColor* basecol = new SoBaseColor;
  basecol->rgb = color_from_code (colorcode);
  root->addChild (basecol);

  if (only_unmeshed)
    draw_pt_set_geom_unmeshed (root, pt_set);
  else
    draw_pt_set_geom (root, pt_set);

  return root;
}


//: draw each point individually using a SoCube.
SoSeparator* draw_ptset_idv(dbmsh3d_pt_set* pt_set, 
                              const int colorcode, const float size,
                             const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;

  SbColor color = color_from_code (colorcode);

  if (user_defined_class) {
    vcl_map<int, dbmsh3d_vertex*>::iterator it = pt_set->vertexmap().begin();
    for (int i=0; it != pt_set->vertexmap().end(); it++, i++) {
      dbmsh3d_vertex* v = (*it).second;
      SoSeparator* sep = draw_vertex_vispt_SoCube (v, color, size);
      root->addChild (sep);
    }
  }
  else {
    vcl_map<int, dbmsh3d_vertex*>::iterator it = pt_set->vertexmap().begin();
    for (int i=0; it != pt_set->vertexmap().end(); it++, i++) {
      dbmsh3d_vertex* v = (*it).second;
      SoSeparator* sep = draw_vertex_SoCube (v, color, size);
      root->addChild (sep);
    }
  }

  return root;
}

SoSeparator* draw_pt_set_color (dbmsh3d_pt_set* pt_set,
                                const vcl_vector<SbColor>& color_set,
                                const float size,
                                const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;

  //: assign size
  SoDrawStyle * drawstyle = new SoDrawStyle;
  drawstyle->pointSize = size;
  root->addChild(drawstyle);

  assert (pt_set->vertexmap().size() == color_set.size());

  if (user_defined_class) {
    vcl_map<int, dbmsh3d_vertex*>::iterator it = pt_set->vertexmap().begin();
    for (int i=0; it != pt_set->vertexmap().end(); it++, i++) {
      dbmsh3d_vertex* v = (*it).second;
      SoSeparator* sep = new SoSeparator;

      //assign color for each
      SoBaseColor* basecol = new SoBaseColor;
      basecol->rgb = color_set[i];
      sep->addChild (basecol);

      SoTranslation* trans = new SoTranslation;
      trans->translation.setValue (v->pt().x(), v->pt().y(), v->pt().z());
      sep->addChild (trans);

      dbmsh3dvis_vertex_SoPointSet* pt = new dbmsh3dvis_vertex_SoPointSet (v);
      sep->addChild (pt);    
      root->addChild (sep);
    }  
  }
  else {  
    vcl_map<int, dbmsh3d_vertex*>::iterator it = pt_set->vertexmap().begin();
    for (int i=0; it != pt_set->vertexmap().end(); it++, i++) {
      dbmsh3d_vertex* v = (*it).second;
      SoSeparator* sep = new SoSeparator;

      //assign color for each
      SoBaseColor* basecol = new SoBaseColor;
      basecol->rgb = color_set[i];
      sep->addChild (basecol);

      SoTranslation* trans = new SoTranslation;
      trans->translation.setValue (v->pt().x(), v->pt().y(), v->pt().z());
      sep->addChild (trans);

      SoPointSet* pt = new SoPointSet;
      sep->addChild (pt);    
      root->addChild (sep);
    }  
  }

  return root;
}

// ============================== G3D ==============================

bool dbmsh3d_load_g3d (dbmsh3d_pt_set* pt_set, vcl_vector<SbColor>& gene_color_set, const char* file)
{
  FILE* fp;
  if ((fp = vcl_fopen(file, "r")) == NULL) {
    vul_printf (vcl_cout, "Can't open input G3d file %s\n", file);
    return false; 
  }

  int iID;
  vcl_fscanf (fp, "%d\n", &iID);

  unsigned int iNumGenes;
  vcl_fscanf (fp, "%u\n", &iNumGenes);
  vul_printf (vcl_cout, "%u points\n", iNumGenes);

  double x, y, z;
  float col[3];

  for (unsigned int i=0; i < iNumGenes; i++) {
    dbmsh3d_vertex *ptGene = pt_set->_new_vertex ();

    vcl_fscanf (fp, "%lf %lf %lf %f %f %f\n", &x, &y, &z, &col[0], &col[1], &col[2]);
    ptGene->get_pt().set (x, y, z);
    SbColor c (col[0], col[1], col[2]);
    gene_color_set.push_back (c);
    
    pt_set->_add_vertex (ptGene);
  }
  assert (iNumGenes == pt_set->vertexmap().size());
  assert (iNumGenes == gene_color_set.size());

  fclose (fp);
  return true;
}

bool dbmsh3d_save_g3d (dbmsh3d_pt_set* pt_set, const char* file)
{
  FILE* fp;
  if ((fp = vcl_fopen(file, "w")) == NULL) {
    vul_printf (vcl_cout, "Can't open input G3d file %s\n", file);
    return false; 
  }

  vul_printf (vcl_cout, "Saving %s...\n", file);
  vcl_fprintf (fp, "%d\n", 3);
  vcl_fprintf (fp, "%u\n", pt_set->vertexmap().size());

  vcl_map<int, dbmsh3d_vertex*>::iterator it = pt_set->vertexmap().begin();
  for (unsigned int i=0; it != pt_set->vertexmap().end(); it++, i++) {
    dbmsh3d_vertex* v = (*it).second;
    vcl_fprintf (fp, "%.16f %.16f %.16f %f %f %f\n", 
                 v->pt().x(), v->pt().y(), v->pt().z(), 1.0f, 1.0f, 1.0f);
  }

  fclose (fp);
  return true;
}

bool dbmsh3d_save_g3d (dbmsh3d_pt_set* pt_set, vcl_vector<SbColor>& gene_color_set, const char* file)
{
  assert (pt_set->vertexmap().size() == gene_color_set.size());

  FILE* fp;
  if ((fp = vcl_fopen(file, "w")) == NULL) {
    vul_printf (vcl_cout, "Can't open input G3d file %s\n", file);
    return false; 
  }

  vul_printf (vcl_cout, "Saving %s...\n", file);
  vcl_fprintf (fp, "%d\n", 3);
  vcl_fprintf (fp, "%u\n", pt_set->vertexmap().size());

  vcl_map<int, dbmsh3d_vertex*>::iterator it = pt_set->vertexmap().begin();
  for (unsigned int i=0; it != pt_set->vertexmap().end(); it++, i++) {
    dbmsh3d_vertex* v = (*it).second;
    vcl_fprintf (fp, "%.16f %.16f %.16f %f %f %f\n", 
                 v->pt().x(), v->pt().y(), v->pt().z(), 
                 gene_color_set[i][0], gene_color_set[i][1], gene_color_set[i][2]); ///1.0f, 1.0f, 1.0f);
  }

  fclose (fp);
  return true;
}




