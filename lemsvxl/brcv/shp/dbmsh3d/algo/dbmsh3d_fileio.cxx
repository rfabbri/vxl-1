//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/dbmsh3d_fileio.cxx 
//:
// \file
// \brief 3D mesh file I/O
//
//
// \author
//  MingChing Chang  Feb 10, 2005
//
// \verbatim
//  Modifications
//    6/13/2005   Nhon Trinh     implemented dbmsh3d_load_ply(...)
// \endverbatim
//
//-------------------------------------------------------------------------

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <cstring>
#include <algorithm>
#include <iostream>
#include <vul/vul_printf.h>
#include <vul/vul_file.h>
#include <vul/vul_string.h>
#include <vgl/vgl_point_3d.h>

#include <vnl/vnl_numeric_traits.h>

#include <buld/buld_dir_file.h>
#include <dbmsh3d/algo/dbmsh3d_fileio.h>

#include <rply/rply.h>

// #################################################################
//    POINT CLOUD FILE I/O
// #################################################################

// ============================== XYZ ==============================
  
bool dbmsh3d_load_xyz (dbmsh3d_pt_set* pointset, const char* file)
{
  FILE* fp;
  if ((fp = std::fopen (file, "r")) == NULL) {
    ///vul_printf (std::cout, "  can't open input .XYZ file %s\n", file);
    return false; 
  }
  assert (pointset->vertexmap().size() == 0);

  //Read in (x, y, z) points.
  int ret;
  do {
    double x, y, z;
    ret = std::fscanf (fp, "%lf %lf %lf\n", &x, &y, &z);
    if (ret != EOF) {
      dbmsh3d_vertex* V = pointset->_new_vertex ();
      V->get_pt().set (x, y, z);
      pointset->_add_vertex (V);
    }
  }
  while (ret != EOF);

  vul_printf (std::cout, "  %d points loaded from %s.\n", 
               pointset->vertexmap().size(), file);

  fclose (fp);
  return true;
}

bool dbmsh3d_save_xyz (dbmsh3d_pt_set* pointset, const char* file)
{
  FILE* fp;
  if ((fp = std::fopen (file, "w")) == NULL) {
    ///vul_printf (std::cout, "  can't open output .XYZ file %s\n", file);
    return false; 
  }
  vul_printf (std::cout, "  saving %s : %d points ...\n", 
               file, pointset->vertexmap().size());
  
  std::map<int, dbmsh3d_vertex*>::iterator it = pointset->vertexmap().begin();
  for (; it != pointset->vertexmap().end(); it++) {
    dbmsh3d_vertex* v = (*it).second;
    std::fprintf (fp, "%.16f %.16f %.16f\n", v->pt().x(), v->pt().y(), v->pt().z());
  }

  std::fclose (fp);
  vul_printf (std::cout, "  done.\n");
  return true;
}

bool dbmsh3d_load_xyz (std::vector<vgl_point_3d<double> >& pts, const char* file)
{
  FILE* fp;
  if ((fp = std::fopen (file, "r")) == NULL) {
    ///vul_printf (std::cout, "  can't open input .XYZ file %s\n", file);
    return false; 
  }
  assert (pts.size() == 0);

  //Read in (x, y, z) points.
  int ret;
  vgl_point_3d<double> P;  
  do {
    double x, y, z;
    ret = std::fscanf (fp, "%lf %lf %lf\n", &x, &y, &z);
    if (ret != EOF) {
      P.set (x, y, z);
      pts.push_back (P);
    }
  }
  while (ret != EOF);

  vul_printf (std::cout, "  %d points loaded from %s.\n", pts.size(), file);

  fclose (fp);
  return true;
}

bool dbmsh3d_save_xyz (std::vector<vgl_point_3d<double> >& pts, const char* file)
{
  std::FILE* fp;
  if ((fp = std::fopen (file, "w")) == NULL) {
    ///vul_printf (std::cout, "  can't open output .XYZ file %s\n", file);
    return false; 
  }
  vul_printf (std::cout, "  saving %s : %d points ...\n", file, pts.size());
  
  std::vector<vgl_point_3d<double> >::iterator it = pts.begin();
  for (; it != pts.end(); it++) {
    vgl_point_3d<double> P = (*it);
    std::fprintf (fp, "%.16f %.16f %.16f\n", P.x(), P.y(), P.z());
  }

  fclose (fp);
  vul_printf (std::cout, "  done.\n");
  return true;
}

bool dbmsh3d_load_xyz (std::vector<std::pair<int, vgl_point_3d<double> > >& idpts, const char* file)
{
  FILE* fp;
  if ((fp = std::fopen (file, "r")) == NULL) {
    ///vul_printf (std::cout, "  can't open input .XYZ file %s\n", file);
    return false; 
  }
  assert (idpts.size() == 0);

  //Read in (x, y, z) points.
  int ret;
  int id = 0;
  vgl_point_3d<double> P;  
  do {
    double x, y, z;
    ret = std::fscanf (fp, "%lf %lf %lf\n", &x, &y, &z);
    if (ret != EOF) {
      P.set (x, y, z);
      idpts.push_back (std::pair<int, vgl_point_3d<double> > (id, P));
      id++;
    }
  }
  while (ret != EOF);

  vul_printf (std::cout, "  %d points loaded from %s.\n", idpts.size(), file);

  fclose (fp);
  return true;
}
bool dbmsh3d_save_xyz (std::vector<std::pair<int, vgl_point_3d<double> > >& idpts, const char* file)
{
  std::FILE* fp;
  if ((fp = std::fopen (file, "w")) == NULL) {
    ///vul_printf (std::cout, "  can't open output .XYZ file %s\n", file);
    return false; 
  }
  vul_printf (std::cout, "  saving %s : %d points ...\n", file, idpts.size());
  
  std::vector<std::pair<int, vgl_point_3d<double> > >::iterator it = idpts.begin();
  for (; it != idpts.end(); it++) {
    vgl_point_3d<double> P = (*it).second;
    std::fprintf (fp, "%.16f %.16f %.16f\n", P.x(), P.y(), P.z());
  }

  fclose (fp);
  vul_printf (std::cout, "  done.\n");
  return true;
}

//: Oriented points: point + normal.
bool dbmsh3d_load_xyzn1 (std::vector<std::pair<vgl_point_3d<double>, vgl_vector_3d<double> > >& ori_pts,
                         const char* file)
{
  FILE* fp;
  if ((fp = std::fopen (file, "r")) == NULL) {
    ///vul_printf (std::cout, "  can't open input .XYZN1 file %s\n", file);
    return false; 
  }
  assert (ori_pts.size() == 0);

  //Read in # points
  int num_pts;
  std::fscanf (fp, "%d\n", &num_pts);

  //Read in (x, y, z) points.
  int ret;
  vgl_point_3d<double> P;  
  vgl_vector_3d<double> N;  
  do {
    double x, y, z, nx, ny, nz;
    //w is the local feature size, not used.
    ret = std::fscanf (fp, "%lf %lf %lf %lf %lf %lf\n", &x, &y, &z, &nx, &ny, &nz);
    if (ret != EOF) {
      P.set (x, y, z);
      N.set (nx, ny, nz);
      ori_pts.push_back (std::pair<vgl_point_3d<double>, vgl_vector_3d<double> >(P, N));
    }
  }
  while (ret != EOF);

  assert (ori_pts.size() == num_pts);

  vul_printf (std::cout, "  %d oriented points loaded from %s.\n", 
              ori_pts.size(), file);

  fclose (fp);
  return true;
}

bool dbmsh3d_save_xyzn1 (std::vector<std::pair<vgl_point_3d<double>, vgl_vector_3d<double> > >& ori_pts,
                         const char* file)
{
  std::FILE* fp;
  if ((fp = std::fopen (file, "w")) == NULL) {
    ///vul_printf (std::cout, "  can't open output .XYZNW file %s\n", file);
    return false; 
  }
  vul_printf (std::cout, "  saving %s : %d oriented points ...\n", 
              file, ori_pts.size());
  
  //# of points.
  std::fprintf (fp, "%lu\n", ori_pts.size());

  std::vector<std::pair<vgl_point_3d<double>, vgl_vector_3d<double> > >::iterator it = ori_pts.begin();
  for (; it != ori_pts.end(); it++) {
    vgl_point_3d<double> P = (*it).first;
    vgl_vector_3d<double> N = (*it).second;
    std::fprintf (fp, "%.16f %.16f %.16f %.16f %.16f %.16f\n", 
                 P.x(), P.y(), P.z(), N.x(), N.y(), N.z());
  }

  fclose (fp);
  vul_printf (std::cout, "  done.\n");
  return true;
}

//: Oriented points: point + normal + local feature size.
bool dbmsh3d_load_xyznw (std::vector<std::pair<vgl_point_3d<double>, vgl_vector_3d<double> > >& ori_pts,
                         const char* file)
{
  FILE* fp;
  if ((fp = std::fopen (file, "r")) == NULL) {
    ///vul_printf (std::cout, "  can't open input .XYZNW file %s\n", file);
    return false; 
  }
  assert (ori_pts.size() == 0);

  //Read in (x, y, z) points.
  int ret;
  vgl_point_3d<double> P;  
  vgl_vector_3d<double> N;  
  do {
    double x, y, z, nx, ny, nz, w;
    //w is the local feature size, not used.
    ret = std::fscanf (fp, "%lf %lf %lf %lf %lf %lf %lf\n", &x, &y, &z, &nx, &ny, &nz, &w);
    if (ret != EOF) {
      P.set (x, y, z);
      N.set (nx, ny, nz);
      ori_pts.push_back (std::pair<vgl_point_3d<double>, vgl_vector_3d<double> >(P, N));
    }
  }
  while (ret != EOF);

  vul_printf (std::cout, "  %d oriented points loaded from %s.\n", 
              ori_pts.size(), file);

  fclose (fp);
  return true;
}

bool dbmsh3d_save_xyznw_vector (std::vector<std::pair<vgl_point_3d<double>, vgl_vector_3d<double> > >& oriented_points,
                                const char* file)
{
  std::FILE* fp;
  if ((fp = std::fopen (file, "w")) == NULL) {
    ///vul_printf (std::cout, "  can't open output .XYZNW file %s\n", file);
    return false; 
  }
  vul_printf (std::cout, "  saving %s : %d oriented points ...\n", 
               file, oriented_points.size());
  
  std::vector<std::pair<vgl_point_3d<double>, vgl_vector_3d<double> > >::iterator it = oriented_points.begin();
  for (; it != oriented_points.end(); it++) {
    vgl_point_3d<double> P = (*it).first;
    vgl_vector_3d<double> N = (*it).second;
    double w = 1.0;
    std::fprintf (fp, "%.16f %.16f %.16f %.16f %.16f %.16f %lf\n", 
                 P.x(), P.y(), P.z(), N.x(), N.y(), N.z(), w);
  }

  fclose (fp);
  vul_printf (std::cout, "  done.\n");
  return true;
}

bool dbmsh3d_load_pts (dbmsh3d_pt_set* pointset, const char* file)
{
  FILE* fp;
  if ((fp = std::fopen (file, "r")) == NULL)
    return false; 
  assert (pointset->vertexmap().size() == 0);

  vul_printf (std::cout, "reading %s...\n\t", file);

  //Read in (x, y, z) points.
  int ret;
  do {
    //read in # of points in this chunk.
    int n;
    ret = std::fscanf (fp, "%d", &n);
    if (ret != EOF) {
      vul_printf (std::cout, "%d ", n);
      for (int i=0; i<n; i++) {
        //read each point in this chunk.
        double x, y, z;
        int r, g, b;
        int t;
        ret = std::fscanf (fp, "%lf %lf %lf %d %d %d %d\n", &x, &y, &z, &t, &r, &g, &b);
        assert (ret != EOF);
        dbmsh3d_vertex* V = pointset->_new_vertex ();
        V->get_pt().set (x, y, z);
        pointset->_add_vertex (V);
      }
    }
  }
  while (ret != EOF);

  vul_printf (std::cout, "\n  %d points loaded from %s.\n", 
               pointset->vertexmap().size(), file);

  fclose (fp);
  return true;
}

// ============================== P3D ==============================

bool dbmsh3d_load_p3d (dbmsh3d_pt_set* pointset, const char* file)
{
  FILE* fp;
  if ((fp = std::fopen (file, "r")) == NULL) {
    ///vul_printf (std::cout, "  can't open input .P3D file %s\n", file);
    return false; 
  }

  assert (pointset->vertexmap().size() == 0);

  unsigned int dim;
  std::fscanf (fp, "%u\n", &dim);
  if (dim != 3)
    return false;

  unsigned int numGenes;
  std::fscanf (fp, "%u\n", &numGenes);

  vul_printf (std::cout, "  loading %s : %d points ...\n", 
               file, numGenes);

  for (unsigned int i=0; i<numGenes; i++) {
    dbmsh3d_vertex* v = pointset->_new_vertex ();
    double x, y, z;
    std::fscanf (fp, "%lf %lf %lf\n", &x, &y, &z);
    v->get_pt().set (x, y, z);

    pointset->_add_vertex (v);
  }

  fclose (fp);
  vul_printf (std::cout, "  done.\n");
  return true;
}

bool dbmsh3d_save_p3d (dbmsh3d_pt_set* pointset, const char* file)
{
  std::FILE* fp;
  if ((fp = std::fopen (file, "w")) == NULL) {
    ///vul_printf (std::cout, "  can't open output .P3D file %s\n", file);
    return false; 
  }
  vul_printf (std::cout, "  saving %s : %d points ...\n", 
               file, pointset->vertexmap().size());

  std::fprintf (fp, "%d\n", 3);
  std::fprintf (fp, "%lu\n", pointset->vertexmap().size());
  
  std::map<int, dbmsh3d_vertex*>::iterator it = pointset->vertexmap().begin();
  for (; it != pointset->vertexmap().end(); it++) {
    dbmsh3d_vertex* v = (*it).second;
    std::fprintf (fp, "%.16f %.16f %.16f\n", v->pt().x(), v->pt().y(), v->pt().z());
  }

  fclose (fp);
  vul_printf (std::cout, "  done.\n");
  return true;
}

bool dbmsh3d_load_p3d (std::vector<vgl_point_3d<double> >& pts, const char* file)
{
  FILE* fp;
  if ((fp = std::fopen (file, "r")) == NULL) {
    ///vul_printf (std::cout, "  can't open input .P3D file %s\n", file);
    return false; 
  }

  assert (pts.size() == 0);

  unsigned int dim;
  std::fscanf (fp, "%u\n", &dim);
  assert (dim==3);
  unsigned int numGenes;
  std::fscanf (fp, "%u\n", &numGenes);
  pts.resize (numGenes);
  vul_printf (std::cout, "  loading %s : %d points into a vector storage...\n", file, numGenes);

  for (unsigned int i=0; i<numGenes; i++) {
    vgl_point_3d<double> P;
    double x, y, z;
    std::fscanf (fp, "%lf %lf %lf\n", &x, &y, &z);
    P.set (x, y, z);
    pts[i] = P;
  }

  fclose (fp);
  vul_printf (std::cout, "  done.\n");
  return true;
}

bool dbmsh3d_save_p3d (std::vector<vgl_point_3d<double> >& pts, const char* file)
{
  std::FILE* fp;
  if ((fp = std::fopen (file, "w")) == NULL) {
    ///vul_printf (std::cout, "  can't open output .P3D file %s\n", file);
    return false; 
  }
  vul_printf (std::cout, "  saving %s : %d points ...\n", file, pts.size());

  std::fprintf (fp, "%d\n", 3);
  std::fprintf (fp, "%lu\n", pts.size());
  
  std::vector<vgl_point_3d<double> >::iterator it = pts.begin();
  for (; it != pts.end(); it++) {
    vgl_point_3d<double> P = (*it);
    std::fprintf (fp, "%.16f %.16f %.16f\n", P.x(), P.y(), P.z());
  }

  fclose (fp);
  vul_printf (std::cout, "  done.\n");
  return true;
}

bool dbmsh3d_load_p3d (std::vector<std::pair<int, vgl_point_3d<double> > >& idpts,
                       const char* file)
{
  FILE* fp;
  if ((fp = std::fopen (file, "r")) == NULL) {
    ///vul_printf (std::cout, "  can't open input .P3D file %s\n", file);
    return false; 
  }

  assert (idpts.size() == 0);

  unsigned int dim;
  std::fscanf (fp, "%u\n", &dim);
  assert (dim==3);
  unsigned int numGenes;
  std::fscanf (fp, "%u\n", &numGenes);

  vul_printf (std::cout, "  loading %s : %d points into a vector storage...\n", file, numGenes);

  for (unsigned int i=0; i<numGenes; i++) {
    vgl_point_3d<double> P;
    double x, y, z;
    std::fscanf (fp, "%lf %lf %lf\n", &x, &y, &z);
    P.set (x, y, z);

    idpts.push_back (std::pair<int, vgl_point_3d<double> > (i, P));
  }

  fclose (fp);
  vul_printf (std::cout, "  done.\n");
  return true;
}

bool dbmsh3d_save_p3d (std::vector<std::pair<int, vgl_point_3d<double> > >& idpts, const char* file)
{
  std::FILE* fp;
  if ((fp = std::fopen (file, "w")) == NULL) {
    ///vul_printf (std::cout, "  can't open output .P3D file %s\n", file);
    return false; 
  }
  vul_printf (std::cout, "  saving %s : %d points ...\n", file, idpts.size());

  std::fprintf (fp, "%d\n", 3);
  std::fprintf (fp, "%lu\n", idpts.size());
  
  std::vector<std::pair<int, vgl_point_3d<double> > >::iterator it = idpts.begin();
  for (; it != idpts.end(); it++) {
    vgl_point_3d<double> P = (*it).second;
    std::fprintf (fp, "%.16f %.16f %.16f\n", P.x(), P.y(), P.z());
  }

  fclose (fp);
  vul_printf (std::cout, "  done.\n");
  return true;
}

// #################################################################################

//: Open the file and read in # of dbsk3d_bnd_pts
int read_num_genes_sphere_from_file (const char* pcFile_P3D)
{
  FILE* fp;
  if ((fp = fopen (pcFile_P3D, "r")) == NULL) {
    ///vul_printf (std::cout, "  can't open input .P3D file %s\n", pcFile_P3D);
    return false; 
  }

  int dim;
  std::fscanf (fp, "%d\n", &dim);
  assert (dim==3);
  int numGenes;
  std::fscanf (fp, "%d\n", &numGenes);
  fclose (fp);

  vul_printf (std::cout, "  reading data from %s: %d original genes.\n", pcFile_P3D, numGenes);
  return numGenes;
}

bool dbsk3d_load_con (dbmsh3d_pt_set* geneset, const char* pcFile_CON, double z)
{

#define MAX_LEN 1024

  double x, y;
  char buffer[MAX_LEN];
  int nPoints;

  //1)If file open fails, return.
  std::ifstream fp (pcFile_CON, std::ios::in);
  if (!fp){
    std::cout<<" : Unable to Open "<<pcFile_CON<<std::endl;
    return false;
  }

  //: need to release the memory!!
  geneset->vertexmap().clear();

  //2)Read in file header.
  fp.getline (buffer,MAX_LEN); //CONTOUR
  fp.getline (buffer,MAX_LEN); //OPEN/CLOSE
  fp >> nPoints;
  std::cout << "Number of Points from Contour:" << nPoints << std::endl;
  
  //3)Read in 'nPoints' of lines of data.
  for (int i=0; i<nPoints; i++) {
    fp >> x >> y;
    dbmsh3d_vertex *v = geneset->_new_vertex ();
    ///v->set_id (i);
    ///assert (x > -100000);
    v->get_pt().set (x, y, z);
  
    geneset->_add_vertex (v);
  }

  fp.close();
  vul_printf (std::cout, "  done.\n");
  return true;
}

// #################################################################
//    MESH
// #################################################################

bool save_unmeshed_p3d (dbmsh3d_mesh* M, const char* file)
{
  FILE* fp;
  if ((fp = std::fopen (file, "w")) == NULL) {
    ///vul_printf (std::cout, "  can't open output .P3D file %s\n", file);
    return false; 
  }
  unsigned int num = M->count_unmeshed_pts();
  if (num == 0)
    return false;

  vul_printf (std::cout, "  saving unmeshed %u points to %s...\n", num, file);
  std::fprintf (fp, "%d\n", 3);
  std::fprintf (fp, "%u\n", num);
  
  std::map<int, dbmsh3d_vertex*>::iterator it = M->vertexmap().begin();
  unsigned int count=0;
  for (; it != M->vertexmap().end(); it++) {
    dbmsh3d_vertex* v = (*it).second;
    if (v->b_meshed() == false) {
      std::fprintf (fp, "%f %f %f\n", v->pt().x(), v->pt().y(), v->pt().z());
      count++;
    }
  }

  assert (count == num);
  fclose (fp);
  return true;
}

bool dbmsh3d_load_ply2 (std::vector<vgl_point_3d<double> >& pts, 
                        std::vector<std::vector<int> >* faces, 
                        const char* file)
{
  assert (0);
  return true;
}

bool dbmsh3d_save_ply2 (const std::vector<vgl_point_3d<double> >& pts, 
                        const std::vector<std::vector<int> >& faces, 
                        const char* file)
{
  FILE* fp;
  if ((fp = std::fopen(file, "w")) == NULL) {
    ///vul_printf (std::cout, "  can't open PLY2 file %s to write.\n", file);
    return false; 
  }
  vul_printf (std::cout, "  saving %s : \n\t%d points, %d faces ...\n", 
              file, pts.size(), faces.size());

  std::fprintf (fp, "%ld\n", pts.size());
  std::fprintf (fp, "%lu\n", faces.size());

  for (unsigned int i=0; i<pts.size(); i++) {
    std::fprintf (fp, "%.16f ", pts[i].x());
    std::fprintf (fp, "%.16f ", pts[i].y());
    std::fprintf (fp, "%.16f ", pts[i].z());
    std::fprintf (fp, "\n");
  }

  for (unsigned int i=0; i<faces.size(); i++) {
    std::fprintf (fp, "%lu ", faces[i].size());
    assert (faces[i].size() != 0);
    for (unsigned int j=0; j<faces[i].size(); j++)
      std::fprintf (fp, "%d ", faces[i][j]);
    std::fprintf (fp, "\n");
  }

  std::fclose (fp);
  vul_printf (std::cout, "  done.\n");
  return true;
}


// ===================== GENERIC ==================================
//: Load a mesh files (only PLY and PLY2 are currently supported)
bool dbmsh3d_load (dbmsh3d_mesh* M, const char* file, const char* format)
{
  std::string file_format(format);
  file_format = "." + file_format;
  
  // if no type is given, determine it using file extension
  if (file_format == ".DEFAULT")
  {
    file_format = vul_file::extension(file);
    vul_string_upcase(file_format);
    ///file_format = vul_file::extension(file);
  }

  std::cout << "format = " << file_format << "\n";
  // call appropriate load functions to load the M
  if (file_format == ".PLY")
  {
    return dbmsh3d_load_ply(M, file);
  }

  if (file_format == ".PLY2")
  {
    return dbmsh3d_load_ply2(M, file);
  }

  std::cout << "  loading failed. Unknown file format.\n";
  return false;
}

// ============================= PLY2 ==============================

bool dbmsh3d_save_ply2 (dbmsh3d_mesh* M, const char* file)
{
  FILE* fp;
  if ((fp = std::fopen(file, "w")) == NULL) {
    ///vul_printf (std::cout, "  can't open PLY2 file %s to write.\n", file);
    return false; 
  }
  vul_printf (std::cout, "  saving %s : \n\t%d points, %d faces ...\n", 
               file, M->vertexmap().size(), M->facemap().size());

  std::fprintf (fp, "%lu\n", M->vertexmap().size());
  std::fprintf (fp, "%lu\n", M->facemap().size());

  //Use v->vid() to re-index vertices, starting with id 0.
  int vidcounter = 0;
  std::map<int, dbmsh3d_vertex*>::iterator it = M->vertexmap().begin();
  for (; it != M->vertexmap().end(); it++) {
    dbmsh3d_vertex* V = (*it).second;
    V->set_vid (vidcounter++);

    std::fprintf (fp, "%.16f ", V->pt().x());
    std::fprintf (fp, "%.16f ", V->pt().y());
    std::fprintf (fp, "%.16f ", V->pt().z());
    std::fprintf (fp, "\n");
  }

  std::map<int, dbmsh3d_face*>::iterator fit = M->facemap().begin();
  for (; fit != M->facemap().end(); fit++) {
    dbmsh3d_face* F = (*fit).second;

    if (F->vertices().empty()) //skip if the vertices_[] is non-empty.
      F->_ifs_track_ordered_vertices ();

    std::fprintf (fp, "%lu ", F->vertices().size());
    for (unsigned j=0; j<F->vertices().size(); j++) {
      dbmsh3d_vertex* V = F->vertices(j);
      std::fprintf (fp, "%d ", V->vid());
    }
    std::fprintf (fp, "\n");

  }
  std::fclose (fp);
  vul_printf (std::cout, "  done.\n");

  return true;
}

bool dbmsh3d_load_ply2 (dbmsh3d_mesh* M, const char* file)
{
  std::FILE* fp;
  assert (M->vertexmap().size() == 0);
  assert (M->facemap().size() == 0);

  if ((fp = std::fopen(file, "r")) == NULL) {
    ///vul_printf (std::cout, "  can't open input PLY2 file %s.\n", file);
    return false; 
  }

  int vertex_N, face_N;
  std::fscanf (fp, "%d\n", &vertex_N);
  std::fscanf (fp, "%d\n", &face_N);

  vul_printf (std::cout, "  loading %s : \n\t%d points, %d faces ...\n", 
               file, vertex_N, face_N);

  double p[3];
  for (int i=0; i<vertex_N; i++) {
    dbmsh3d_vertex* point = M->_new_vertex ();

    std::fscanf (fp, "%lf ", &p[0]);
    std::fscanf (fp, "%lf ", &p[1]);
    std::fscanf (fp, "%lf ", &p[2]);
    std::fscanf (fp, "\n");
    point->get_pt().set (p);

    M->_add_vertex (point);
  }

  for (int i=0; i<face_N; i++){
    dbmsh3d_face* F = M->_new_face ();

    int num_pt_per_face;
    std::fscanf (fp, "%d ", &num_pt_per_face);
    for (int j=0; j<num_pt_per_face; j++) {
      int ind;
      std::fscanf (fp, "%d ", &ind);

      dbmsh3d_vertex* V = M->vertexmap (ind);
      F->_ifs_add_bnd_V (V);
      V->set_meshed (true);
    }
    std::fscanf (fp, "\n");

    M->_add_face (F);
  }
  std::fclose (fp);
  vul_printf (std::cout, "\tdone.\n");

  return true;
}

bool dbmsh3d_load_ply2_v (dbmsh3d_mesh* M, const char* file)
{
  std::FILE* fp;
  assert (M->vertexmap().size() == 0);
  assert (M->facemap().size() == 0);

  if ((fp = std::fopen(file, "r")) == NULL) {
    ///vul_printf (std::cout, "  can't open input PLY2 file %s.\n", file);
    return false; 
  }
  int vertex_N, face_N;
  std::fscanf (fp, "%d\n", &vertex_N);
  std::fscanf (fp, "%d\n", &face_N);
  vul_printf (std::cout, "  dbmsh3d_load_ply2_v() %s: \n\t%d points, %d faces ...\n", 
              file, vertex_N, face_N);

  double p[3];
  for (int i=0; i<vertex_N; i++) {
    dbmsh3d_vertex* V = M->_new_vertex ();
    std::fscanf (fp, "%lf ", &p[0]);
    std::fscanf (fp, "%lf ", &p[1]);
    std::fscanf (fp, "%lf ", &p[2]);
    std::fscanf (fp, "\n");
    V->get_pt().set (p);
    M->_add_vertex (V);
  }

  std::fclose (fp);  
  vul_printf (std::cout, "  %d vertices loaded.\n", vertex_N);
  return true;
}

#define TINY_ERROR  1E-3

bool dbmsh3d_load_ply2_f (dbmsh3d_mesh* M, const char* file)
{
  std::FILE* fp;

  if ((fp = std::fopen(file, "r")) == NULL) {
    ///vul_printf (std::cout, "  can't open input PLY2 file %s.\n", file);
    return false; 
  }

  int vertex_N, face_N;
  std::fscanf (fp, "%d\n", &vertex_N);
  std::fscanf (fp, "%d\n", &face_N);

  vul_printf (std::cout, "  dbmsh3d_load_ply2_f (%s): \n", file);
  vul_printf (std::cout, "\tSkip %d points and load %d faces ...\n", vertex_N, face_N);

  assert (M->vertexmap().size() == vertex_N);
  double p[3];
  for (int i=0; i<vertex_N; i++) {
    dbmsh3d_vertex* V = M->vertexmap(i);
    std::fscanf (fp, "%lf ", &p[0]);
    std::fscanf (fp, "%lf ", &p[1]);
    std::fscanf (fp, "%lf ", &p[2]);
    std::fscanf (fp, "\n");
    //assert that M has exactly these vertices.
    assert (std::fabs (V->pt().x() - p[0]) < TINY_ERROR);
    assert (std::fabs (V->pt().y() - p[1]) < TINY_ERROR);
    assert (std::fabs (V->pt().z() - p[2]) < TINY_ERROR);
  }

  assert (M->facemap().size() == 0);
  for (int i=0; i<face_N; i++){
    dbmsh3d_face* F = M->_new_face ();

    int num_pt_per_face;
    std::fscanf (fp, "%d ", &num_pt_per_face);
    for (int j=0; j<num_pt_per_face; j++) {
      int ind;
      std::fscanf (fp, "%d ", &ind);

      dbmsh3d_vertex* V = M->vertexmap (ind);
      F->_ifs_add_bnd_V (V);
      V->set_meshed (true);
    }
    std::fscanf (fp, "\n");

    M->_add_face (F);
  }
  std::fclose (fp);
  vul_printf (std::cout, "  %d faces loaded.\n", face_N);
  return true;
}

void setup_IFS_M_label_Fs_vids (dbmsh3d_mesh* M, const int label, 
                                std::vector<dbmsh3d_vertex*>& vertices,
                                std::vector<dbmsh3d_face*>& faces)
{
  int vid_counter = 0;

  //Go through all vertices and set vid to -1.
  std::map<int, dbmsh3d_vertex*>::iterator vit = M->vertexmap().begin();
  for (; vit != M->vertexmap().end(); vit++) {
    dbmsh3d_vertex* V = (*vit).second;
    V->set_vid (-1);
  }

  //Go through all labelled faces and set vid for each incident vertex.
  std::map<int, dbmsh3d_face*>::iterator fit = M->facemap().begin();
  for (; fit != M->facemap().end(); fit++) {
    dbmsh3d_face* F = (*fit).second;
    if (F->is_visited (label) == false)
      continue;

    faces.push_back (F);

    std::vector<dbmsh3d_vertex*> fv;
    F->get_bnd_Vs (fv);
    for (unsigned int i=0; i<fv.size(); i++) {
      dbmsh3d_vertex* V = fv[i];
      if (V->vid() >= 0)
        continue; //skip V that already in the IFS set.
      V->set_vid (vid_counter++);
      vertices.push_back (V);
    }
  }
}

//Save the labelled mesh faces into file.
bool dbmsh3d_save_label_faces_ply2 (dbmsh3d_mesh* M, const int label, const char* file)
{
  FILE* fp;
  if ((fp = std::fopen(file, "w")) == NULL) {
    ///vul_printf (std::cout, "  can't open PLY2 file %s to write.\n", file);
    return false;  
  }

  //Go through all labelled faces & set vid for each incident vertex.
  std::vector<dbmsh3d_vertex*> vertices;
  std::vector<dbmsh3d_face*> faces;
  setup_IFS_M_label_Fs_vids (M, label, vertices, faces);

  vul_printf (std::cout, "  saving %s : \n\t%d points, %d faces ...\n", 
              file, vertices.size(), faces.size());

  std::fprintf (fp, "%lu\n", vertices.size());
  std::fprintf (fp, "%lu\n", faces.size());

  for (unsigned int i=0; i<vertices.size(); i++) {
    dbmsh3d_vertex* V = vertices[i];
    assert (V->vid() == i);

    std::fprintf (fp, "%.16f ", V->pt().x());
    std::fprintf (fp, "%.16f ", V->pt().y());
    std::fprintf (fp, "%.16f ", V->pt().z());
    std::fprintf (fp, "\n");
  }

  for (unsigned int i=0; i<faces.size(); i++) {
    dbmsh3d_face* F = faces[i];
    assert (F->is_visited (label));

    std::vector<dbmsh3d_vertex*> vertices;
    F->get_bnd_Vs (vertices);

    std::fprintf (fp, "%lu ", vertices.size());
    for (unsigned j=0; j<vertices.size(); j++) {
      dbmsh3d_vertex* V = vertices[j];
      std::fprintf (fp, "%d ", V->vid());
    }
    std::fprintf (fp, "\n");

  }
  std::fclose (fp);
  vul_printf (std::cout, "  done.\n");

  return true;
}

// ====================== Hugues Hoppe's .M Mesh File ==========================

bool dbmsh3d_load_m (dbmsh3d_mesh* M, const char* file)
{
  std::FILE* fp;
  if ((fp = std::fopen(file, "r")) == NULL) {
    ///vul_printf (std::cout, "  can't open input .M file %s.\n", file);
    return false; 
  }

  //Read in mesh vertices.
  int ret;
  do {
    int id;
    char type[128];
    double x, y, z, r, g, b;
    int tri[3];
    ret = std::fscanf (fp, "%s", type);
    if (ret == EOF)
      break;
    
    if (std::strcmp (type, "Vertex")==0) {
      std::fscanf (fp, "%d %lf %lf %lf {rgb=(%lf %lf %lf)}\n", 
                  &id, &x, &y, &z, &r, &g, &b);

      dbmsh3d_vertex* V = M->_new_vertex ();
      V->set_id (id);
      V->get_pt().set (x, y, z);
      M->_add_vertex (V);

      if (id > M->vertex_id_counter())
        M->set_vertex_id_counter (id+1);
    }
    else if (std::strcmp (type, "Face")==0) {
      std::fscanf (fp, "%d %d %d %d\n", &id, &tri[0], &tri[1], &tri[2]);
      dbmsh3d_face* F = M->_new_face ();
      for (int i=0; i<3; i++) {
        dbmsh3d_vertex* V = M->vertexmap (tri[i]);
        F->_ifs_add_bnd_V (V);
        V->set_meshed (true);
      }
      M->_add_face (F);
    }
  }
  while (ret != EOF);

  std::fclose (fp);
  vul_printf (std::cout, "  done.\n");

  return true;
}

bool dbmsh3d_save_m (dbmsh3d_mesh* M, const char* file)
{
  std::FILE* fp;
  if ((fp = std::fopen(file, "w")) == NULL) {
    ///vul_printf (std::cout, "  can't open .M file %s to write.\n", file);
    return false; 
  }
  vul_printf (std::cout, "  saving %s : \n\t%d points, %d faces ...\n", 
               file, M->vertexmap().size(), M->facemap().size());

  std::fprintf (fp, "%lu\n", M->vertexmap().size());
  std::fprintf (fp, "%lu\n", M->facemap().size());

  std::map<int, dbmsh3d_vertex*>::iterator it = M->vertexmap().begin();
  for (; it != M->vertexmap().end(); it++) {
    dbmsh3d_vertex* v = (*it).second;

    std::fprintf (fp, "%.16f ", v->pt().x());
    std::fprintf (fp, "%.16f ", v->pt().y());
    std::fprintf (fp, "%.16f ", v->pt().z());
    std::fprintf (fp, "\n");
  }

  std::map<int, dbmsh3d_face*>::iterator fit = M->facemap().begin();
  for (; fit != M->facemap().end(); fit++) {
    dbmsh3d_face* F = (*fit).second;

    std::fprintf (fp, "%lu ", F->vertices().size());
    for (unsigned j=0; j<F->vertices().size(); j++) {
      dbmsh3d_vertex* V = F->vertices(j);
      std::fprintf (fp, "%d ", V->id());
    }
    std::fprintf (fp, "\n");

  }
  std::fclose (fp);
  vul_printf (std::cout, "  done.\n");

  return true;
}


bool dbmsh3d_load_vtk (dbmsh3d_mesh* M, const char* file)
{ 
  std::FILE* fp;
  if ((fp = fopen(file, "r")) == NULL) {
    std::printf ("  Can't open VTK file %s to read.\n", file);
    return false; 
  }
  std::printf ("    dbmsh3d_load_vtk(): %s\n", file);

  std::fscanf (fp, "# vtk DataFile Version 3.0\n");
  std::fscanf (fp, "vtk output\n");
  std::fscanf (fp, "ASCII\n");
  std::fscanf (fp, "DATASET POLYDATA\n");
  int n_points;
  std::fscanf (fp, "POINTS %d float\n", &n_points);
  int n_points_div_3 = n_points / 3;
  int n_points_mod_3 = n_points % 3;

  float x1, y1, z1, x2, y2, z2, x3, y3, z3; 

  //Handle n_points_div_3.
  for (int i=0; i < n_points_div_3; i++) {
    std::fscanf (fp, "%f %f %f %f %f %f %f %f %f\n",
                &x1, &y1, &z1, &x2, &y2, &z2, &x3, &y3, &z3);

    int id = i*3;
    dbmsh3d_vertex* V = M->_new_vertex ();
    V->set_id (id);
    V->get_pt().set (x1, y1, z1);
    M->_add_vertex (V);
    if (id > M->vertex_id_counter())
      M->set_vertex_id_counter (id+1);

    id = i*3+1;
    V = M->_new_vertex ();
    V->set_id (id);
    V->get_pt().set (x2, y2, z2);
    M->_add_vertex (V);
    if (id > M->vertex_id_counter())
      M->set_vertex_id_counter (id+1);

    id = i*3+2;
    V = M->_new_vertex ();
    V->set_id (id);
    V->get_pt().set (x3, y3, z3);
    M->_add_vertex (V);
    if (id > M->vertex_id_counter())
      M->set_vertex_id_counter (id+1);
  }

  //Handle n_points_mod_3.
  if (n_points_mod_3 == 0) {
    std::fscanf (fp, "\n");
  }
  else if (n_points_mod_3 == 1) {
    std::fscanf (fp, "%f %f %f\n", &x1, &y1, &z1);
    
    int id = n_points_div_3*3;
    dbmsh3d_vertex* V = M->_new_vertex ();
    V->set_id (id);
    V->get_pt().set (x1, y1, z1);
    M->_add_vertex (V);
    if (id > M->vertex_id_counter())
      M->set_vertex_id_counter (id+1);
  }
  else if (n_points_mod_3 == 2) {
    std::fscanf (fp, "%f %f %f %f %f %f\n", &x1, &y1, &z1, &x2, &y2, &z2);

    int id = n_points_div_3*3;
    dbmsh3d_vertex* V = M->_new_vertex ();
    V->set_id (id);
    V->get_pt().set (x1, y1, z1);
    M->_add_vertex (V);
    if (id > M->vertex_id_counter())
      M->set_vertex_id_counter (id+1);

    id = n_points_div_3*3+1;
    V = M->_new_vertex ();
    V->set_id (id);
    V->get_pt().set (x2, y2, z2);
    M->_add_vertex (V);
    if (id > M->vertex_id_counter())
      M->set_vertex_id_counter (id+1);
  }

  //Handle faces.
  int n_faces, n_tri_size;
  char s_face_type[128];
  std::fscanf (fp, "%s %d %d\n", s_face_type, &n_faces, &n_tri_size);
  
  int n_fv, fv_id;
  int n_tri_size_pts = 0;

  if (std::strcmp (s_face_type, "TRIANGLE_STRIPS") == 0) {
    for (int i=0; i< n_faces; i++) {
      std::fscanf (fp, "%d", &n_fv);
      n_tri_size_pts += (n_fv+1);

      assert (n_fv > 2);

      //add the first 2 vertices to F.
      dbmsh3d_vertex *V0, *V1;
      for (int j=0; j< 2; j++) {
        std::fscanf (fp, " %d", &fv_id);
        dbmsh3d_vertex* V = M->vertexmap (fv_id);
        if (j==0)
          V0 = V;
        else if (j==1)
          V1 = V;
      }

      for (int j=2; j< n_fv; j++) {      
        //create an initial triangle F
        dbmsh3d_face* F = M->_new_face ();
        M->_add_face (F);
        
        //add V0 and V1 to F
        F->_ifs_add_bnd_V (V0);
        V0->set_meshed (true);
        F->_ifs_add_bnd_V (V1);
        V1->set_meshed (true);

        //read in new V, add to triangle F.
        std::fscanf (fp, " %d", &fv_id);
        dbmsh3d_vertex* V = M->vertexmap (fv_id);
        F->_ifs_add_bnd_V (V);
        V->set_meshed (true);

        //update V0 and V1.
        V0 = V1;
        V1 = V;
      }

      std::fscanf (fp, "\n");
    }
  }
  else if (std::strcmp (s_face_type, "POLYGONS") == 0) {
    for (int i=0; i< n_faces; i++) {
      std::fscanf (fp, "%d", &n_fv);
      n_tri_size_pts += (n_fv+1);
   
      //create an initial triangle F
      dbmsh3d_face* F = M->_new_face ();
      M->_add_face (F);

      for (int j=0; j< n_fv; j++) {   
        int fv_id;
        std::fscanf (fp, " %d", &fv_id);
        dbmsh3d_vertex* V = M->vertexmap (fv_id);
        F->_ifs_add_bnd_V (V);
        V->set_meshed (true);
      }
      std::fscanf (fp, "\n");
    }
  }
  
  assert (n_tri_size_pts == n_tri_size);
  std::fclose (fp);
  return true;
}

bool dbmsh3d_save_vtk (dbmsh3d_mesh* M, const char* file)
{  
  std::FILE* fp;
  if ((fp = fopen(file, "w")) == NULL) {
    std::printf ("  Can't open VTK file %s to write.\n", file);
    return false; 
  }

  std::fprintf (fp, "# vtk DataFile Version 3.0\n");
  std::fprintf (fp, "vtk output\n");
  std::fprintf (fp, "ASCII\n");
  std::fprintf (fp, "DATASET POLYDATA\n");
  
  int n_points = (int) M->vertexmap().size();
  std::fprintf (fp, "POINTS %d float\n", n_points);  
  int n_points_div_3 = n_points / 3;
  int n_points_mod_3 = n_points % 3;

  //Handle n_points_div_3.
  std::map<int, dbmsh3d_vertex*>::iterator it = M->vertexmap().begin();
  for (int i=0; i< n_points_div_3; i++) {
    dbmsh3d_vertex* V0 = (*it).second;
    it++;
    dbmsh3d_vertex* V1 = (*it).second;
    it++;
    dbmsh3d_vertex* V2 = (*it).second;
    it++;
    std::fprintf (fp, "%f %f %f %f %f %f %f %f %f\n",
                 V0->pt().x(), V0->pt().y(), V0->pt().z(),
                 V1->pt().x(), V1->pt().y(), V1->pt().z(),
                 V2->pt().x(), V2->pt().y(), V2->pt().z());
  }

  //Handle n_points_mod_3.
  if (n_points_mod_3 == 0) {
    std::fprintf (fp, "\n");
  }
  else if (n_points_mod_3 == 1) {
    dbmsh3d_vertex* V0 = (*it).second;
    it++;
    std::fprintf (fp, "%f %f %f\n", 
                 V0->pt().x(), V0->pt().y(), V0->pt().z());
  }
  else if (n_points_mod_3 == 2) {
    dbmsh3d_vertex* V0 = (*it).second;
    it++;
    dbmsh3d_vertex* V1 = (*it).second;
    it++;
    std::fprintf (fp, "%f %f %f %f %f %f\n", 
                 V0->pt().x(), V0->pt().y(), V0->pt().z(),
                 V1->pt().x(), V1->pt().y(), V1->pt().z());
  }

  int n_faces = M->facemap().size();
  int n_tri_size = 0;
  std::map<int, dbmsh3d_face*>::iterator fit = M->facemap().begin();
  for (; fit != M->facemap().end(); fit++) {
    dbmsh3d_face* F = (dbmsh3d_face*) (*fit).second;
    int n_fv = F->vertices().size();
    n_tri_size += (n_fv + 1);
  }

  std::fprintf (fp, "POLYGONS %d %d\n", n_faces, n_tri_size);

  fit = M->facemap().begin();
  for (; fit != M->facemap().end(); fit++) {
    dbmsh3d_face* F = (dbmsh3d_face*) (*fit).second;
    int n_fv = F->vertices().size();
    std::fprintf (fp, "%d", n_fv);

    for (int j=0; j<n_fv; j++) {
      int vid = F->vertices(j)->id();
      assert (vid < n_points);
      std::fprintf (fp, " %d", vid);
    }
    std::fprintf (fp, "\n");
  }

  std::fclose (fp);
  return true;
}

//################### LIST FILE ###############################

bool dbmsh3d_read_list_file (const char* file,
                             std::vector<std::string>& data_files, 
                             std::vector<std::string>& align_files)
{
  std::string filename (file);
  if (buld_get_suffix (filename) == "")
    filename += ".txt";
  vul_printf (std::cout, "dbmsh3d_read_list_file(): %s.\n", filename.c_str());

  std::ifstream  in;
  std::string    linestr;
  in.open (filename.c_str());
  if (in.fail()) {
    vul_printf (std::cout, "Can't open listfile %s\n", filename.c_str());
    return false; 
  }

  //Read in each line of dataset in the list file.
  int file_count = 0;
  int af_count = 0;
  while (in) {
    linestr.clear();
    std::getline (in, linestr);

    if (linestr.length() == 0 || std::strncmp (linestr.c_str(), "#", 1) == 0)
      continue; //skip empty line and comments.

    char file[256] = "", af[256] = "";
    std::sscanf (linestr.c_str(), "%s %s", file, af);

    data_files.push_back (std::string (file));
    align_files.push_back (std::string (af));
  }

  in.close();
  vul_printf (std::cout, "\t%d files (%d alignment files) read from %s.\n", 
              data_files.size(), align_files.size(), filename.c_str());
  return true;
}    

bool dbmsh3d_save_list_file (const std::string& list_file, 
                             const std::vector<std::string>& data_files, 
                             const std::vector<std::string>& align_files)
{
  vul_printf (std::cout, "  Saving list file %s.\n", list_file.c_str());
  std::FILE* fp;
  assert (data_files.size() == align_files.size());
  if ((fp = std::fopen (list_file.c_str(), "w")) == NULL) {
    vul_printf (std::cout, "Can't open output txt file %s\n", list_file.c_str());
    return false;
  }
  for (unsigned int i=0; i<data_files.size(); i++) {
    std::fprintf (fp, "%s %s\n", data_files[i].c_str(), align_files[i].c_str());
  }
  std::fclose (fp);
  return true;
}

bool dbmsh3d_save_list_view_run_file (const std::string& list_view_run, 
                                      const std::string& list_file)
{
  vul_printf (std::cout, "  Saving list view run file %s.\n", list_file.c_str());
  std::FILE* fp;
  if ((fp = std::fopen (list_view_run.c_str(), "w")) == NULL) {
    vul_printf (std::cout, "Can't open output txt file %s\n", list_file.c_str());
    return false;
  }
  std::fprintf (fp, "dbmsh3dappw -list %s\n", list_file.c_str());
  std::fclose (fp);
  return true;
}

//===================xml==============
//
bool dbmsh3d_save_xml (dbmsh3d_mesh* mesh, const char* file)
{
  FILE* fp;
  if ((fp = std::fopen(file, "w")) == NULL) {
    std::fprintf (stderr, "Can't open xml file %s to write.\n", file);
    return false; 
  }
  std::fprintf (stderr, "Saving %s : \n\t%d points, %d faces ...\n", 
               file, mesh->vertexmap().size(), mesh->facemap().size());

      std::fprintf (fp, "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>");

      std::fprintf (fp, "\n");
      std::fprintf (fp, "<CityModel xmlns=\"http://www.citygml.org/citygml/1/0/0\" xmlns:gml=\"http://www.opengis.net/gml\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.citygml.org/citygml/1/0/0 http://www.citygml.org/citygml/1/0/0/CityGML.xsd\">");

      std::fprintf (fp, "\n");
      std::fprintf (fp, "<gml:description>CityModel description</gml:description>");
      std::fprintf (fp, "<gml:name>CityModel name</gml:name>");

      std::fprintf (fp, "\n");
      std::fprintf (fp, "<cityObjectMember>");
  //  <Building gml:id="GEB_TH_IFC_Building_Variant_GEB_0">");

      std::fprintf (fp, "\n");
      std::fprintf (fp, "<Building>");

      std::fprintf (fp, "\n");
      std::fprintf (fp, "<gml:description>Building Description</gml:description>");
      std::fprintf (fp, " <gml:name>Building Name</gml:name>");
      std::fprintf (fp, "<consistsOfBuildingPart>");
 //       <BuildingPart gml:id="GEB_TH_IFC_Building_Variant_GEB_TEIL_1">
       std::fprintf (fp, "<BuildingPart>");
       std::fprintf (fp, " <gml:description>BuildingPart Description</gml:description>");
       std::fprintf (fp, "<gml:name>BuildingPart Name</gml:name>");
 
  std::map<int, dbmsh3d_face*>::iterator fit = mesh->facemap().begin();
  for (; fit != mesh->facemap().end(); fit++) {
    dbmsh3d_face* face = (*fit).second;
      std::fprintf (fp, "\n");

       std::fprintf (fp, "<boundedBy>");
       std::fprintf (fp, "<WallSurface>");
       std::fprintf (fp, "<lod4MultiSurface><gml:MultiSurface>");
      std::fprintf (fp, "\n");

    std::fprintf (fp, "<gml:surfaceMember>");
    
    std::fprintf (fp, "<TexturedSurface orientation=\"+\">");
                  
    std::fprintf (fp, "<gml:baseSurface>");
                    
    std::fprintf (fp, "<gml:Polygon>");
                      
    std::fprintf (fp, "<gml:exterior>");
                        
    std::fprintf (fp, "<gml:LinearRing>");
             std::fprintf (fp, "\n");
                   
     for (unsigned j=0; j<face->vertices().size(); j++) {

      dbmsh3d_vertex* v = face->vertices(j);  
      std::fprintf (fp, "\n");
      std::fprintf (fp, "<gml:pos srsDimension=\"3\">");
      vgl_point_3d<double > pt = v->pt();
      std::fprintf (fp, "%.16f ", v->pt().x());
      std::fprintf (fp, "%.16f ", v->pt().y());
      std::fprintf (fp, "%.16f ", v->pt().z());
      std::fprintf (fp, "</gml:pos>");
      std::fprintf (fp, "\n");
      }
//Now print the first vertex again to close the polygon
   dbmsh3d_vertex* v = face->vertices(0);  
   std::fprintf (fp, "\n");
   std::fprintf (fp, "<gml:pos srsDimension=\"3\">");
   vgl_point_3d<double > pt = v->pt();  
   std::fprintf (fp, "%.16f ", v->pt().x());
   std::fprintf (fp, "%.16f ", v->pt().y());
   std::fprintf (fp, "%.16f ", v->pt().z());
   std::fprintf (fp, "</gml:pos>");
   std::fprintf (fp, "\n");

   std::fprintf (fp, "</gml:LinearRing>");
                      
    std::fprintf (fp, "</gml:exterior>");
                    
    std::fprintf (fp, "</gml:Polygon>");
                  
    std::fprintf (fp, "</gml:baseSurface>");
                
    std::fprintf (fp, "</TexturedSurface>");
      std::fprintf (fp, "\n");

              
    std::fprintf (fp, "</gml:surfaceMember>");

   std::fprintf (fp, "</gml:MultiSurface></lod4MultiSurface>");
  std::fprintf (fp, "     </WallSurface>");
  std::fprintf (fp, "  </boundedBy>");         

    std::fprintf (fp, "\n");

  }
  std::fprintf (fp, "       </BuildingPart>");
  std::fprintf (fp, "     </consistsOfBuildingPart>");
  std::fprintf (fp, "   </Building>");
  std::fprintf (fp, "  </cityObjectMember>");       
  std::fprintf (fp, " </CityModel>");

  std::fclose (fp);
  std::fprintf (stderr, "\tdone.\n");

  return true;
}

