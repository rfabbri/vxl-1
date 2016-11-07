//: dbsk3d_fs_mesh_fileio.cxx
//  
//  The dbsk3d_fs_mesh (*.FS) FIle I/O
// 
//: MingChing Chang
//  Apr 30, 2004        Creation

#include <vcl_cassert.h>
#include <vcl_iostream.h>
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
      vul_printf (vcl_cout, "ERROR: Can't open file %s.\n", pcOutFile);
    return false; 
  }
  if (verbose)
    vul_printf (vcl_cout, "save_to_fs (%s)\n", pcOutFile);

  //: File Header
  vcl_fprintf (fp, "Full Medial Scaffold File v1.0\n\n");

  fs_mesh->compute_median_A122_dist ();
  vcl_fprintf (fp, "median_A122_dist: %f\n\n", fs_mesh->median_A122_dist());

  //:A1) Point Generators
  //     Can put the Mesh connectivity here!
  vcl_fprintf (fp, "# Point Generators: %u\n", fs_mesh->bnd_mesh()->vertexmap().size());
  vcl_fprintf (fp, "Id: Coord (x, y, z)\n");
  vcl_fprintf (fp, "----------\n");
  if (verbose)
    vcl_cout<< "\tSaving "<<fs_mesh->bnd_mesh()->vertexmap().size()<<" point_genes...\n";

  vcl_map<int, dbmsh3d_vertex*>::iterator it = fs_mesh->bnd_mesh()->vertexmap().begin();
  for (; it != fs_mesh->bnd_mesh()->vertexmap().end(); it++) {
    dbmsh3d_vertex* G = (*it).second;
    vcl_fprintf (fp, "g %d (%.16f %.16f %.16f)\n", 
                 G->id(), G->pt().x(), G->pt().y(), G->pt().z()); 
  }
  vcl_fprintf (fp, "\n\n");  

  //:B1) Save NodeElements
  vcl_fprintf (fp, "# Fine-Scale Vertex Elements: %u\n", fs_mesh->vertexmap().size());
  vcl_fprintf (fp, "Id: Coord (x, y, z), Shock Type {3, 4, R, A, S, D, I, ?}\n");
  vcl_fprintf (fp, "  # Assigned Generators: Ids\n");
  vcl_fprintf (fp, "-----------\n");
  if (verbose)
    vcl_cout<< "\tSaving "<< fs_mesh->vertexmap().size() <<" fs_vertices...\n";

  vcl_map<int, dbmsh3d_vertex*>::iterator nit = fs_mesh->vertexmap().begin();
  for (; nit != fs_mesh->vertexmap().end(); nit++) {
    dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) (*nit).second;
    fv_save_text_file (fp, FV);
  }
  vcl_fprintf (fp, "----------\n");
  vcl_fprintf (fp, "\n\n");

  //:C1) Save LinkElements
  vcl_fprintf (fp, "# Fine-Scale Edge Elements: %u\n", fs_mesh->edgemap().size());
  vcl_fprintf (fp, "Id: Vertices (S, E), Shock Type {R, M, A, D, I, ?}\n");
  vcl_fprintf (fp, "\t# Assigned Generators: Ids\n");
  vcl_fprintf (fp, "-----------\n");
  if (verbose)
    vcl_cout<< "\tSaving "<< fs_mesh->edgemap().size() <<" fs_edges...\n";

  vcl_map<int, dbmsh3d_edge*>::iterator lit = fs_mesh->edgemap().begin();
  for (; lit != fs_mesh->edgemap().end(); lit++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) (*lit).second;
    fe_save_text_file (fp, FE);
  }
  vcl_fprintf (fp, "\n\n");

  //:B1) Save A12PatchElements
  vcl_fprintf (fp, "# Fine-Scale Face Elements: %u\n", fs_mesh->facemap().size());
  vcl_fprintf (fp, "Id: Point Generators (0-1), Valid {V, I}, Finite {F, I}\n");
  vcl_fprintf (fp, "\t# Edges: Ids in order\n");
  vcl_fprintf (fp, "-----------\n");
  if (verbose)
    vcl_cout<< "\tSaving "<< fs_mesh->facemap().size() <<" fs_faces...\n";
  
  vcl_map<int, dbmsh3d_face*>::iterator pit = fs_mesh->facemap().begin();
  for (; pit != fs_mesh->facemap().end(); pit++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*pit).second;
    ff_save_text_file (fp, FF);
  }
  vcl_fprintf (fp, "\n\n");
  if (verbose)
    vcl_cout<< "\tdone.\n";

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
      vul_printf (vcl_cout, "ERROR(load_from_fs): Can't open file %s.\n", pcInFile);
    return false; 
  }
  if (verbose)
    vul_printf (vcl_cout, "load_from_fs (%s)\n", pcInFile);

  //: File Header
  vcl_fscanf (fp, "Full Medial Scaffold File v1.0\n\n");

  float median_A122;
  vcl_fscanf (fp, "median_A122_dist: %f\n\n", &median_A122);
  fs_mesh->set_median_A122_dist (median_A122);
  if (verbose)
    vul_printf (vcl_cout, "\tmedian_A122_dist: %f\n", median_A122);

  //:A1) Point Generators
  unsigned int iNumGenes;
  vcl_fscanf (fp, "# Point Generators: %u\n", &iNumGenes);
  vcl_fscanf (fp, "Id: Coord (x, y, z)\n");
  vcl_fscanf (fp, "----------\n");
  if (verbose)
    vcl_cout<< "\tLoading "<<iNumGenes<<" point_genes...\n";

  int id;
  double x, y, z;
  if (read_gene) {
    assert (fs_mesh->bnd_mesh()->vertexmap().size() ==0);
    for (unsigned int i=0; i<iNumGenes; i++) {
      vcl_fscanf (fp, "g %d (%lf %lf %lf)\n", &id, &x, &y, &z);
      dbmsh3d_vertex* G = new dbmsh3d_vertex (id);
      G->get_pt().set (x, y, z);
      fs_mesh->bnd_mesh()->_add_vertex (G);
    }
  }
  else {
    for (unsigned int i=0; i<iNumGenes; i++)
      vcl_fscanf (fp, "g %d (%lf %lf %lf)\n", &id, &x, &y, &z);
  }
  vcl_fscanf (fp, "\n\n");
  
  //:B1) Load fs_vertices.
  unsigned int iNumVertices;
  vcl_fscanf (fp, "# Fine-Scale Vertex Elements: %u\n", &iNumVertices);
  vcl_fscanf (fp, "Id: Coord (x, y, z), Shock Type {3, 4, R, A, S, D, I, ?}\n");
  vcl_fscanf (fp, "  # Assigned Generators: Ids\n");
  vcl_fscanf (fp, "-----------\n");
  if (verbose)
    vcl_cout<< "\tLoading "<<iNumVertices<<" fs_vertices...\n";

  for (unsigned int i=0; i<iNumVertices; i++) {
    dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) fs_mesh->_new_vertex ();
    fv_load_text_file (fp, FV, fs_mesh->bnd_mesh()->vertexmap());
    fs_mesh->_add_vertex (FV);
  }
  assert (fs_mesh->vertexmap().size() == iNumVertices);
  vcl_fscanf (fp, "----------\n");
  vcl_fscanf (fp, "\n\n");

  //:C1) Load fs_edges.
  unsigned int iNumLinks;
  vcl_fscanf (fp, "# Fine-Scale Edge Elements: %u\n", &iNumLinks);
  vcl_fscanf (fp, "Id: Vertices (S, E), Shock Type {R, M, A, D, I, ?}\n");
  vcl_fscanf (fp, "\t# Assigned Generators: Ids\n");
  vcl_fscanf (fp, "-----------\n");
  if (verbose)
    vcl_cout<< "\tLoading "<<iNumLinks<<" fs_edges...\n";
  
  for (unsigned int i=0; i<iNumLinks; i++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) fs_mesh->_new_edge (NULL, NULL);
    fe_load_text_file (fp, FE, fs_mesh->vertexmap(), fs_mesh->bnd_mesh()->vertexmap());
    fs_mesh->_add_edge (FE); //_add_edge_check (FE);
  }
  assert (fs_mesh->edgemap().size() == iNumLinks);
  vcl_fscanf (fp, "\n\n");

  //:D) Load fs_faces.
  unsigned int iNumSheets;
  vcl_fscanf (fp, "# Fine-Scale Face Elements: %u\n", &iNumSheets);
  vcl_fscanf (fp, "Id: Point Generators (0-1), Valid {V, I}, Finite {F, I}\n");
  vcl_fscanf (fp, "\t# Edges: Ids in order\n");
  vcl_fscanf (fp, "-----------\n");
  if (verbose)
    vcl_cout<< "\tLoading "<< iNumSheets <<" fs_faces...\n";
  
  for (unsigned int i=0; i<iNumSheets; i++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) fs_mesh->_new_face ();
    ff_load_text_file (fp, FF, fs_mesh->edgemap(), fs_mesh->bnd_mesh()->vertexmap());
    fs_mesh->_add_face (FF);
  }
  vcl_fscanf (fp, "\n\n");

  // #############################################################
  //:D2) Post-load Connectivity Handling & Other Computations
  
  //Sort all vertices's incident edge list.
  fs_mesh->sort_V_incident_Es ();

  fs_mesh->sort_halfedges_for_all_faces (verbose);
  if (verbose)
    vcl_cout<< "\tdone.\n";

  return true;
}

bool save_fs_vtk_attrib (dbsk3d_fs_mesh* M, const char* file)
{
  vcl_FILE* fp;
  if ((fp = fopen(file, "w")) == NULL) {
    vcl_printf ("  Can't open VTK file %s to write.\n", file);
    return false; 
  }

  vcl_fprintf (fp, "# vtk DataFile Version 3.0\n");
  vcl_fprintf (fp, "vtk output\n");
  vcl_fprintf (fp, "ASCII\n");
  vcl_fprintf (fp, "DATASET POLYDATA\n");
  
  int n_points = (int) M->vertexmap().size();
  vcl_fprintf (fp, "POINTS %d float\n", n_points);  
  int n_points_div_3 = n_points / 3;
  int n_points_mod_3 = n_points % 3;

  //Handle n_points_div_3.
  vcl_map<int, dbmsh3d_vertex*>::iterator it = M->vertexmap().begin();
  for (int i=0; i< n_points_div_3; i++) {
    dbmsh3d_vertex* V0 = (*it).second;
    it++;
    dbmsh3d_vertex* V1 = (*it).second;
    it++;
    dbmsh3d_vertex* V2 = (*it).second;
    it++;
    vcl_fprintf (fp, "%f %f %f %f %f %f %f %f %f\n",
                 V0->pt().x(), V0->pt().y(), V0->pt().z(),
                 V1->pt().x(), V1->pt().y(), V1->pt().z(),
                 V2->pt().x(), V2->pt().y(), V2->pt().z());
  }

  //Handle n_points_mod_3.
  if (n_points_mod_3 == 0) {
    vcl_fprintf (fp, "\n");
  }
  else if (n_points_mod_3 == 1) {
    dbmsh3d_vertex* V0 = (*it).second;
    it++;
    vcl_fprintf (fp, "%f %f %f\n", 
                 V0->pt().x(), V0->pt().y(), V0->pt().z());
  }
  else if (n_points_mod_3 == 2) {
    dbmsh3d_vertex* V0 = (*it).second;
    it++;
    dbmsh3d_vertex* V1 = (*it).second;
    it++;
    vcl_fprintf (fp, "%f %f %f %f %f %f\n", 
                 V0->pt().x(), V0->pt().y(), V0->pt().z(),
                 V1->pt().x(), V1->pt().y(), V1->pt().z());
  }

  int n_faces = M->facemap().size();
  int n_tri_size = 0;
  vcl_map<int, dbmsh3d_face*>::iterator fit = M->facemap().begin();
  for (; fit != M->facemap().end(); fit++) {
    dbmsh3d_face* F = (dbmsh3d_face*) (*fit).second;
    int n_fv = F->vertices().size();
    n_tri_size += (n_fv + 1);
  }

  vcl_fprintf (fp, "POLYGONS %d %d\n", n_faces, n_tri_size);

  fit = M->facemap().begin();
  for (; fit != M->facemap().end(); fit++) {
    dbmsh3d_face* F = (dbmsh3d_face*) (*fit).second;
    int n_fv = F->vertices().size();
    vcl_fprintf (fp, "%d", n_fv);

    for (int j=0; j<n_fv; j++) {
      int vid = F->vertices(j)->id();
      assert (vid < n_points);
      vcl_fprintf (fp, " %d", vid);
    }
    vcl_fprintf (fp, "\n");
  }

  //Vertex radius as Attribute
  vcl_fprintf (fp, "POINT_DATA %d\n", n_points);
  vcl_fprintf (fp, "SCALARS Thickness double\n");
  vcl_fprintf (fp, "LOOKUP_TABLE default\n");

  it = M->vertexmap().begin();
  for (int i=0; i< n_points; i++) {
    dbsk3d_fs_vertex* V = (dbsk3d_fs_vertex*) (*it).second;
    it++;
    double radius = V->compute_time();
    vcl_fprintf (fp, "%f", radius);
    if (i % 9 == 8)
      vcl_fprintf (fp, "\n");
    else
      vcl_fprintf (fp, " ");
  }

  vcl_fclose (fp);
  return true;
}


