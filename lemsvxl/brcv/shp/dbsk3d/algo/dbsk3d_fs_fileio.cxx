//: dbsk3d_fs_mesh_fileio.cxx
//  
//  The dbsk3d_fs_mesh (*.FS) FIle I/O
// 
//: MingChing Chang
//  Apr 30, 2004        Creation

#include <cassert>
#include <iostream>
#include <vul/vul_printf.h>

#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <dbsk3d/dbsk3d_fs_mesh.h>

//: --- Save Full Shock Data --- 
bool save_to_fs (dbsk3d_fs_mesh* fs_mesh, const char *pcOutFile, const int verbose)
{
  //Sort all vertices's incident edge list.
  fs_mesh->sort_V_incident_Es ();

  //: Open the *.fs file to write.
  FILE*   fp = fopen (pcOutFile, "w");
  if (fp == NULL) {
    if (verbose)
      vul_printf (std::cout, "ERROR: Can't open file %s.\n", pcOutFile);
    return false; 
  }
  if (verbose)
    vul_printf (std::cout, "save_to_fs (%s)\n", pcOutFile);

  //: File Header
  std::fprintf (fp, "Full Medial Scaffold File v1.0\n\n");

  fs_mesh->compute_median_A122_dist ();
  std::fprintf (fp, "median_A122_dist: %f\n\n", fs_mesh->median_A122_dist());

  //:A1) Point Generators
  //     Can put the Mesh connectivity here!
  std::fprintf (fp, "# Point Generators: %u\n", fs_mesh->bnd_mesh()->vertexmap().size());
  std::fprintf (fp, "Id: Coord (x, y, z)\n");
  std::fprintf (fp, "----------\n");
  if (verbose)
    std::cout<< "\tSaving "<<fs_mesh->bnd_mesh()->vertexmap().size()<<" point_genes...\n";

  std::map<int, dbmsh3d_vertex*>::iterator it = fs_mesh->bnd_mesh()->vertexmap().begin();
  for (; it != fs_mesh->bnd_mesh()->vertexmap().end(); it++) {
    dbmsh3d_vertex* G = (*it).second;
    std::fprintf (fp, "g %d (%.16f %.16f %.16f)\n", 
                 G->id(), G->pt().x(), G->pt().y(), G->pt().z()); 
  }
  std::fprintf (fp, "\n\n");  

  //:B1) Save NodeElements
  std::fprintf (fp, "# Fine-Scale Vertex Elements: %u\n", fs_mesh->vertexmap().size());
  std::fprintf (fp, "Id: Coord (x, y, z), Shock Type {3, 4, R, A, S, D, I, ?}\n");
  std::fprintf (fp, "  # Assigned Generators: Ids\n");
  std::fprintf (fp, "-----------\n");
  if (verbose)
    std::cout<< "\tSaving "<< fs_mesh->vertexmap().size() <<" fs_vertices...\n";

  std::map<int, dbmsh3d_vertex*>::iterator nit = fs_mesh->vertexmap().begin();
  for (; nit != fs_mesh->vertexmap().end(); nit++) {
    dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) (*nit).second;
    fv_save_text_file (fp, FV);
  }
  std::fprintf (fp, "----------\n");
  std::fprintf (fp, "\n\n");

  //:C1) Save LinkElements
  std::fprintf (fp, "# Fine-Scale Edge Elements: %u\n", fs_mesh->edgemap().size());
  std::fprintf (fp, "Id: Vertices (S, E), Shock Type {R, M, A, D, I, ?}\n");
  std::fprintf (fp, "\t# Assigned Generators: Ids\n");
  std::fprintf (fp, "-----------\n");
  if (verbose)
    std::cout<< "\tSaving "<< fs_mesh->edgemap().size() <<" fs_edges...\n";

  std::map<int, dbmsh3d_edge*>::iterator lit = fs_mesh->edgemap().begin();
  for (; lit != fs_mesh->edgemap().end(); lit++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) (*lit).second;
    fe_save_text_file (fp, FE);
  }
  std::fprintf (fp, "\n\n");

  //:B1) Save A12PatchElements
  std::fprintf (fp, "# Fine-Scale Face Elements: %u\n", fs_mesh->facemap().size());
  std::fprintf (fp, "Id: Point Generators (0-1), Valid {V, I}, Finite {F, I}\n");
  std::fprintf (fp, "\t# Edges: Ids in order\n");
  std::fprintf (fp, "-----------\n");
  if (verbose)
    std::cout<< "\tSaving "<< fs_mesh->facemap().size() <<" fs_faces...\n";
  
  std::map<int, dbmsh3d_face*>::iterator pit = fs_mesh->facemap().begin();
  for (; pit != fs_mesh->facemap().end(); pit++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*pit).second;
    ff_save_text_file (fp, FF);
  }
  std::fprintf (fp, "\n\n");
  if (verbose)
    std::cout<< "\tdone.\n";

  fclose (fp);
  return true;
}

//: --- Load Full Shock Data --- 
bool load_from_fs (dbsk3d_fs_mesh* fs_mesh, const char *pcInFile, 
                   const bool read_gene, const int verbose)
{
  //: Open the *.FS file.
  FILE*    fp = fopen (pcInFile, "r");
  if (fp == NULL) {
    if (verbose)
      vul_printf (std::cout, "ERROR(load_from_fs): Can't open file %s.\n", pcInFile);
    return false; 
  }
  if (verbose)
    vul_printf (std::cout, "load_from_fs (%s)\n", pcInFile);

  //: File Header
  std::fscanf (fp, "Full Medial Scaffold File v1.0\n\n");

  float median_A122;
  std::fscanf (fp, "median_A122_dist: %f\n\n", &median_A122);
  fs_mesh->set_median_A122_dist (median_A122);
  if (verbose)
    vul_printf (std::cout, "\tmedian_A122_dist: %f\n", median_A122);

  //:A1) Point Generators
  unsigned int iNumGenes;
  std::fscanf (fp, "# Point Generators: %u\n", &iNumGenes);
  std::fscanf (fp, "Id: Coord (x, y, z)\n");
  std::fscanf (fp, "----------\n");
  if (verbose)
    std::cout<< "\tLoading "<<iNumGenes<<" point_genes...\n";

  int id;
  double x, y, z;
  if (read_gene) {
    assert (fs_mesh->bnd_mesh()->vertexmap().size() ==0);
    for (unsigned int i=0; i<iNumGenes; i++) {
      std::fscanf (fp, "g %d (%lf %lf %lf)\n", &id, &x, &y, &z);
      dbmsh3d_vertex* G = new dbmsh3d_vertex (id);
      G->get_pt().set (x, y, z);
      fs_mesh->bnd_mesh()->_add_vertex (G);
    }
  }
  else {
    for (unsigned int i=0; i<iNumGenes; i++)
      std::fscanf (fp, "g %d (%lf %lf %lf)\n", &id, &x, &y, &z);
  }
  std::fscanf (fp, "\n\n");
  
  //:B1) Load fs_vertices.
  unsigned int iNumVertices;
  std::fscanf (fp, "# Fine-Scale Vertex Elements: %u\n", &iNumVertices);
  std::fscanf (fp, "Id: Coord (x, y, z), Shock Type {3, 4, R, A, S, D, I, ?}\n");
  std::fscanf (fp, "  # Assigned Generators: Ids\n");
  std::fscanf (fp, "-----------\n");
  if (verbose)
    std::cout<< "\tLoading "<<iNumVertices<<" fs_vertices...\n";

  for (unsigned int i=0; i<iNumVertices; i++) {
    dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) fs_mesh->_new_vertex ();
    fv_load_text_file (fp, FV, fs_mesh->bnd_mesh()->vertexmap());
    fs_mesh->_add_vertex (FV);
  }
  assert (fs_mesh->vertexmap().size() == iNumVertices);
  std::fscanf (fp, "----------\n");
  std::fscanf (fp, "\n\n");

  //:C1) Load fs_edges.
  unsigned int iNumLinks;
  std::fscanf (fp, "# Fine-Scale Edge Elements: %u\n", &iNumLinks);
  std::fscanf (fp, "Id: Vertices (S, E), Shock Type {R, M, A, D, I, ?}\n");
  std::fscanf (fp, "\t# Assigned Generators: Ids\n");
  std::fscanf (fp, "-----------\n");
  if (verbose)
    std::cout<< "\tLoading "<<iNumLinks<<" fs_edges...\n";
  
  for (unsigned int i=0; i<iNumLinks; i++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) fs_mesh->_new_edge (NULL, NULL);
    fe_load_text_file (fp, FE, fs_mesh->vertexmap(), fs_mesh->bnd_mesh()->vertexmap());
    fs_mesh->_add_edge (FE); //_add_edge_check (FE);
  }
  assert (fs_mesh->edgemap().size() == iNumLinks);
  std::fscanf (fp, "\n\n");

  //:D) Load fs_faces.
  unsigned int iNumSheets;
  std::fscanf (fp, "# Fine-Scale Face Elements: %u\n", &iNumSheets);
  std::fscanf (fp, "Id: Point Generators (0-1), Valid {V, I}, Finite {F, I}\n");
  std::fscanf (fp, "\t# Edges: Ids in order\n");
  std::fscanf (fp, "-----------\n");
  if (verbose)
    std::cout<< "\tLoading "<< iNumSheets <<" fs_faces...\n";
  
  for (unsigned int i=0; i<iNumSheets; i++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) fs_mesh->_new_face ();
    ff_load_text_file (fp, FF, fs_mesh->edgemap(), fs_mesh->bnd_mesh()->vertexmap());
    fs_mesh->_add_face (FF);
  }
  std::fscanf (fp, "\n\n");

  // #############################################################
  //:D2) Post-load Connectivity Handling & Other Computations
  
  //Sort all vertices's incident edge list.
  fs_mesh->sort_V_incident_Es ();

  fs_mesh->sort_halfedges_for_all_faces (verbose);
  if (verbose)
    std::cout<< "\tdone.\n";

  return true;
}

bool save_fs_vtk_attrib (dbsk3d_fs_mesh* M, const char* file)
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

  //Vertex radius as Attribute
  std::fprintf (fp, "POINT_DATA %d\n", n_points);
  std::fprintf (fp, "SCALARS Thickness double\n");
  std::fprintf (fp, "LOOKUP_TABLE default\n");

  it = M->vertexmap().begin();
  for (int i=0; i< n_points; i++) {
    dbsk3d_fs_vertex* V = (dbsk3d_fs_vertex*) (*it).second;
    it++;
    double radius = V->compute_time();
    std::fprintf (fp, "%f", radius);
    if (i % 9 == 8)
      std::fprintf (fp, "\n");
    else
      std::fprintf (fp, " ");
  }

  std::fclose (fp);
  return true;
}


