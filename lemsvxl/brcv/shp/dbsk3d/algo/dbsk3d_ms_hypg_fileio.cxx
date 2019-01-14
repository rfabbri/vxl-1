//: MingChing Chang
//  Oct 26, 2004  Creation

#include <cassert>
#include <string>
#include <map>
#include <iostream>
#include <vul/vul_printf.h>

#include <dbsk3d/dbsk3d_fs_mesh.h>
#include <dbsk3d/dbsk3d_ms_hypg.h>
#include <dbsk3d/algo/dbsk3d_ms_algos.h>

// ##############################################################################
//   *.CMS FILE I/O
// ##############################################################################

void save_to_cms (dbsk3d_ms_hypg* ms_hypg, const char *cms_file, const int verbose)
{
  if (verbose)
    vul_printf (std::cout, "\nsave_to_cms (%s)\n", cms_file);
  
  FILE  *fp;
  if ((fp = fopen(cms_file, "w")) == NULL) {
    vul_printf (std::cout, "ERROR: Can't open output file %s.\n", cms_file);
    return; 
  }

  std::fprintf (fp, "Medial Scaffold Hyper-graph File v1.0\n");

  //################# Part I Save the HyperGraph Vertices, Links, and Sheets #################

  //:V) Save coarse-scale shock scaffold vertices
  int nA1A3MN = 0;
  int nA14MN = 0;
  int nDegeA1A3MN = 0;
  int nDegeA14MN = 0;
  int nLoopMN = 0;
  count_ms_vertices (ms_hypg, nA1A3MN, nA14MN, nDegeA1A3MN, nDegeA14MN, nLoopMN);
  if (verbose)
    vul_printf (std::cout, "\tSaving %d ms_nodes (%d A1A3, %d A14, %d DegeA1A3, %d DegeA14, %d LoopEnd).\n",
                ms_hypg->vertexmap().size(), nA1A3MN, nA14MN, nDegeA1A3MN, nDegeA14MN, nLoopMN);
  std::fprintf (fp, "\n\n# Medial Scaffold Nodes: %d\n", ms_hypg->vertexmap().size());
  std::fprintf (fp, "# A1A3_ms_nodes: %d\n", nA1A3MN);
  std::fprintf (fp, "# A14_ms_nodes: %d\n", nA14MN);
  std::fprintf (fp, "# Dege_A1A3_ms_nodes: %d\n", nDegeA1A3MN);
  std::fprintf (fp, "# Dege_A14_ms_nodes: %d\n", nDegeA14MN);
  std::fprintf (fp, "# Loop_ms_nodes: %d\n", nLoopMN);
  std::fprintf (fp, "Id: vertex_elm_id, time\n");
  std::fprintf (fp, "\t# incident_ms_curves: [type: R(rib) A(axial) D(degenerate) L(loopend)] ids\n");
  std::fprintf (fp, "-----------\n");

  std::map<int, dbmsh3d_vertex*>::iterator MN_it = ms_hypg->vertexmap().begin();
  for (; MN_it != ms_hypg->vertexmap().end(); MN_it++) {
    dbsk3d_ms_node* MN = (dbsk3d_ms_node*) (*MN_it).second;
    mn_save_text_file (fp, MN);
  }

  //:C) Save coarse-scale shock scaffold curves
  int nA3MC = 0;
  int nA13MC = 0;
  int nDegeMC = 0;
  int nLoopMC = 0;
  int nVirtual = 0;
  count_ms_curves (ms_hypg, nA3MC, nA13MC, nDegeMC, nLoopMC, nVirtual);  
  if (verbose)
    vul_printf (std::cout, "\tSaving %d ms_curves (%d A13, %d A13, %d Dege, %d Loop).\n",
                ms_hypg->edgemap().size(), nA3MC, nA13MC, nDegeMC, nLoopMC);
  std::fprintf (fp, "\n\n# Medial Scaffold Curves: %d\n", ms_hypg->edgemap().size());
  std::fprintf (fp, "# A3_ms_curves: %d\n", nA3MC);
  std::fprintf (fp, "# A13_ms_curves: %d\n", nA13MC);
  std::fprintf (fp, "# Dege_ms_curves: %d\n", nDegeMC);
  std::fprintf (fp, "# Loop_ms_curves: %d\n", nLoopMC);
  ///std::fprintf (fp, "# Virtual_ms_curves: %d\n", nVirtual);
  std::fprintf (fp, "Id: ms_nodes (S, E)\n");
  std::fprintf (fp, "-----------\n");

  std::map<int, dbmsh3d_edge*>::iterator MC_it = ms_hypg->edgemap().begin();
  for (; MC_it != ms_hypg->edgemap().end(); MC_it++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*MC_it).second;
    mc_save_text_file (fp, MC);
  }

  //:S) coarse-scale shock scaffold sheets
  if (verbose)
    vul_printf (std::cout, "\tSaving %u ms_sheets.\n", ms_hypg->sheetmap().size());
  std::fprintf (fp, "\n\n# Medial Scaffold Sheets: %u\n", ms_hypg->sheetmap().size());
  std::fprintf (fp, "Id:\n");
  std::fprintf (fp, "\t# bnd_curves: ids in order\n");
  std::fprintf (fp, "\t# i_curves: ids\n");
  std::fprintf (fp, "\t# face_elms: ids\n");
  std::fprintf (fp, "-----------\n");

  std::map<int, dbmsh3d_sheet*>::iterator SS_it = ms_hypg->sheetmap().begin();
  for (; SS_it != ms_hypg->sheetmap().end(); SS_it++) {
    dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) (*SS_it).second;
    ms_save_text_file (fp, MS);
  }

  //: Save the counter information
  std::fprintf (fp, "\n\nnode_id_counter: %d\n", ms_hypg->vertex_id_counter());
  std::fprintf (fp, "curve_id_counter: %d\n", ms_hypg->edge_id_counter());
  std::fprintf (fp, "sheet_id_counter: %d\n", ms_hypg->sheet_id_counter());
  std::fprintf (fp, "-----------\n");
  
  fclose (fp);
}

bool load_from_cms (dbsk3d_ms_hypg* ms_hypg, const char *filename, const int verbose)
{
  if (verbose)
    vul_printf (std::cout, "\nload_from_cms (%s)\n", filename);

  // Make sure that the underlying fine-scale shock mesh is there.
  assert (ms_hypg->fs_mesh()->vertexmap().size() != 0);

  FILE *fp;
  if ((fp = fopen(filename, "r")) == NULL) {
    vul_printf (std::cout, "ERROR: Can't open input file %s.\n", filename);
    return false; 
  }

  std::map<int, ms_node_file_read*> MN_fileread_map;
  std::fscanf (fp, "Medial Scaffold Hyper-graph File v1.0\n");

  //################# Part I Load the HyperGraph Vertices, Links, and Sheets #################

  //Load coarse-scale shock scaffold vertices
  int nMN;
  int nA1A3MN;
  int nA14MN;
  int nDegeA1A3MN;
  int nDegeA14MN;
  int nLoopMN;
  std::fscanf (fp, "\n\n# Medial Scaffold Nodes: %d\n", &nMN);
  std::fscanf (fp, "# A1A3_ms_nodes: %d\n", &nA1A3MN);
  std::fscanf (fp, "# A14_ms_nodes: %d\n", &nA14MN);
  std::fscanf (fp, "# Dege_A1A3_ms_nodes: %d\n", &nDegeA1A3MN);
  std::fscanf (fp, "# Dege_A14_ms_nodes: %d\n", &nDegeA14MN);
  std::fscanf (fp, "# Loop_ms_nodes: %d\n", &nLoopMN);
  std::fscanf (fp, "Id: vertex_elm_id, time\n");
  std::fscanf (fp, "\t# incident_ms_curves: [type: R(rib) A(axial) D(degenerate) L(loopend)] ids\n");
  std::fscanf (fp, "-----------\n");

  if (verbose)
    vul_printf (std::cout, "\tLoading # ms_nodes: %u.\n", nMN);
  for (int i=0; i<nMN; i++) {
    dbsk3d_ms_node* MN = (dbsk3d_ms_node*) ms_hypg->_new_vertex ();
    ms_node_file_read* MN_fileread = new ms_node_file_read;
    mn_load_text_file (fp, MN, MN_fileread, ms_hypg->fs_mesh());
    ms_hypg->_add_vertex (MN);
    MN_fileread_map.insert (std::pair<int, ms_node_file_read*> (MN_fileread->id_, MN_fileread));
  }
  assert (int(ms_hypg->vertexmap().size()) == nMN);  

  //Load coarse-scale shock scaffold curves
  int nMC;
  int nA3MC;
  int nA13MC;
  int nDegeMC;
  int nLoopMC;
  ///int nVirtual;
  std::fscanf (fp, "\n\n# Medial Scaffold Curves: %d\n", &nMC);
  std::fscanf (fp, "# A3_ms_curves: %d\n", &nA3MC);
  std::fscanf (fp, "# A13_ms_curves: %d\n", &nA13MC);
  std::fscanf (fp, "# Dege_ms_curves: %d\n", &nDegeMC);
  std::fscanf (fp, "# Loop_ms_curves: %d\n", &nLoopMC);
  ///std::fscanf (fp, "# Virtual_ms_curves: %d\n", &nVirtual);
  std::fscanf (fp, "Id: ms_nodes (S, E)\n");
  std::fscanf (fp, "-----------\n");

  if (verbose)
    vul_printf (std::cout, "\tLoading # ms_curves: %u.\n", nMC);
  for (int i=0; i<nMC; i++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) ms_hypg->_new_edge (NULL, NULL);
    mc_load_text_file (fp, MC, ms_hypg->fs_mesh(), ms_hypg);
    ms_hypg->_add_edge (MC);
  }
  assert (int(ms_hypg->edgemap().size()) == nMC);

  //Load coarse-scale shock scaffold sheets
  unsigned int nMS;
  std::fscanf (fp, "\n\n# Medial Scaffold Sheets: %u\n", &nMS);
  std::fscanf (fp, "Id:\n");
  std::fscanf (fp, "\t# bnd_curves: ids in order\n");
  std::fscanf (fp, "\t# i_curves: ids\n");
  std::fscanf (fp, "\t# face_elms: ids\n");
  std::fscanf (fp, "-----------\n");

  if (verbose)
    vul_printf (std::cout, "\tLoading # ms_sheets: %u.\n", nMS);
  for (unsigned int i=0; i<nMS; i++) {
    dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) ms_hypg->_new_sheet ();
    ms_load_text_file (fp, MS, ms_hypg->fs_mesh(), ms_hypg);
    ms_hypg->_add_sheet (MS);
  }
  assert (int(ms_hypg->sheetmap().size()) == nMS);

  //Load the counter information
  int vertex_id_counter;
  int edge_id_counter;
  int sheet_id_counter;
  std::fscanf (fp, "\n\nnode_id_counter: %u\n", &vertex_id_counter);
  std::fscanf (fp, "curve_id_counter: %u\n", &edge_id_counter);
  std::fscanf (fp, "sheet_id_counter: %u\n", &sheet_id_counter);
  ms_hypg->set_vertex_id_counter (vertex_id_counter);
  ms_hypg->set_edge_id_counter (edge_id_counter);
  ms_hypg->set_sheet_id_counter (sheet_id_counter);

  std::fscanf (fp, "-----------\n");
  fclose (fp);

  //Now process the Id information and make the connectivity between real objects
  //For each dbsk3d_ms_node, recover MC/MS connectivity
  std::map<int, dbmsh3d_vertex*>::iterator MN_it = ms_hypg->vertexmap().begin();
  for (; MN_it != ms_hypg->vertexmap().end(); MN_it++) {
    dbsk3d_ms_node* MN = (dbsk3d_ms_node*) (*MN_it).second;
    int id = MN->id();
    ms_node_file_read* MN_fileread = MN_fileread_map[id];
    mn_recover_pointers (MN, MN_fileread, ms_hypg);
    MN_fileread_map.erase (id);
  }
  assert (MN_fileread_map.size() == 0);

  if (verbose)
    vul_printf (std::cout, "\tdone.\n", nMS);
  return true;
}
