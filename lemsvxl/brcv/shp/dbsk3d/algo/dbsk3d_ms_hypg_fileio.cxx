//: MingChing Chang
//  Oct 26, 2004  Creation

#include <vcl_cassert.h>
#include <vcl_string.h>
#include <vcl_map.h>
#include <vcl_iostream.h>
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
    vul_printf (vcl_cout, "\nsave_to_cms (%s)\n", cms_file);
  
  FILE  *fp;
  if ((fp = fopen(cms_file, "w")) == NULL) {
    vul_printf (vcl_cout, "ERROR: Can't open output file %s.\n", cms_file);
    return; 
  }

  vcl_fprintf (fp, "Medial Scaffold Hyper-graph File v1.0\n");

  //################# Part I Save the HyperGraph Vertices, Links, and Sheets #################

  //:V) Save coarse-scale shock scaffold vertices
  int nA1A3MN = 0;
  int nA14MN = 0;
  int nDegeA1A3MN = 0;
  int nDegeA14MN = 0;
  int nLoopMN = 0;
  count_ms_vertices (ms_hypg, nA1A3MN, nA14MN, nDegeA1A3MN, nDegeA14MN, nLoopMN);
  if (verbose)
    vul_printf (vcl_cout, "\tSaving %d ms_nodes (%d A1A3, %d A14, %d DegeA1A3, %d DegeA14, %d LoopEnd).\n",
                ms_hypg->vertexmap().size(), nA1A3MN, nA14MN, nDegeA1A3MN, nDegeA14MN, nLoopMN);
  vcl_fprintf (fp, "\n\n# Medial Scaffold Nodes: %d\n", ms_hypg->vertexmap().size());
  vcl_fprintf (fp, "# A1A3_ms_nodes: %d\n", nA1A3MN);
  vcl_fprintf (fp, "# A14_ms_nodes: %d\n", nA14MN);
  vcl_fprintf (fp, "# Dege_A1A3_ms_nodes: %d\n", nDegeA1A3MN);
  vcl_fprintf (fp, "# Dege_A14_ms_nodes: %d\n", nDegeA14MN);
  vcl_fprintf (fp, "# Loop_ms_nodes: %d\n", nLoopMN);
  vcl_fprintf (fp, "Id: vertex_elm_id, time\n");
  vcl_fprintf (fp, "\t# incident_ms_curves: [type: R(rib) A(axial) D(degenerate) L(loopend)] ids\n");
  vcl_fprintf (fp, "-----------\n");

  vcl_map<int, dbmsh3d_vertex*>::iterator MN_it = ms_hypg->vertexmap().begin();
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
    vul_printf (vcl_cout, "\tSaving %d ms_curves (%d A13, %d A13, %d Dege, %d Loop).\n",
                ms_hypg->edgemap().size(), nA3MC, nA13MC, nDegeMC, nLoopMC);
  vcl_fprintf (fp, "\n\n# Medial Scaffold Curves: %d\n", ms_hypg->edgemap().size());
  vcl_fprintf (fp, "# A3_ms_curves: %d\n", nA3MC);
  vcl_fprintf (fp, "# A13_ms_curves: %d\n", nA13MC);
  vcl_fprintf (fp, "# Dege_ms_curves: %d\n", nDegeMC);
  vcl_fprintf (fp, "# Loop_ms_curves: %d\n", nLoopMC);
  ///vcl_fprintf (fp, "# Virtual_ms_curves: %d\n", nVirtual);
  vcl_fprintf (fp, "Id: ms_nodes (S, E)\n");
  vcl_fprintf (fp, "-----------\n");

  vcl_map<int, dbmsh3d_edge*>::iterator MC_it = ms_hypg->edgemap().begin();
  for (; MC_it != ms_hypg->edgemap().end(); MC_it++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*MC_it).second;
    mc_save_text_file (fp, MC);
  }

  //:S) coarse-scale shock scaffold sheets
  if (verbose)
    vul_printf (vcl_cout, "\tSaving %u ms_sheets.\n", ms_hypg->sheetmap().size());
  vcl_fprintf (fp, "\n\n# Medial Scaffold Sheets: %u\n", ms_hypg->sheetmap().size());
  vcl_fprintf (fp, "Id:\n");
  vcl_fprintf (fp, "\t# bnd_curves: ids in order\n");
  vcl_fprintf (fp, "\t# i_curves: ids\n");
  vcl_fprintf (fp, "\t# face_elms: ids\n");
  vcl_fprintf (fp, "-----------\n");

  vcl_map<int, dbmsh3d_sheet*>::iterator SS_it = ms_hypg->sheetmap().begin();
  for (; SS_it != ms_hypg->sheetmap().end(); SS_it++) {
    dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) (*SS_it).second;
    ms_save_text_file (fp, MS);
  }

  //: Save the counter information
  vcl_fprintf (fp, "\n\nnode_id_counter: %d\n", ms_hypg->vertex_id_counter());
  vcl_fprintf (fp, "curve_id_counter: %d\n", ms_hypg->edge_id_counter());
  vcl_fprintf (fp, "sheet_id_counter: %d\n", ms_hypg->sheet_id_counter());
  vcl_fprintf (fp, "-----------\n");
  
  fclose (fp);
}

bool load_from_cms (dbsk3d_ms_hypg* ms_hypg, const char *filename, const int verbose)
{
  if (verbose)
    vul_printf (vcl_cout, "\nload_from_cms (%s)\n", filename);

  // Make sure that the underlying fine-scale shock mesh is there.
  assert (ms_hypg->fs_mesh()->vertexmap().size() != 0);

  FILE *fp;
  if ((fp = fopen(filename, "r")) == NULL) {
    vul_printf (vcl_cout, "ERROR: Can't open input file %s.\n", filename);
    return false; 
  }

  vcl_map<int, ms_node_file_read*> MN_fileread_map;
  vcl_fscanf (fp, "Medial Scaffold Hyper-graph File v1.0\n");

  //################# Part I Load the HyperGraph Vertices, Links, and Sheets #################

  //Load coarse-scale shock scaffold vertices
  int nMN;
  int nA1A3MN;
  int nA14MN;
  int nDegeA1A3MN;
  int nDegeA14MN;
  int nLoopMN;
  vcl_fscanf (fp, "\n\n# Medial Scaffold Nodes: %d\n", &nMN);
  vcl_fscanf (fp, "# A1A3_ms_nodes: %d\n", &nA1A3MN);
  vcl_fscanf (fp, "# A14_ms_nodes: %d\n", &nA14MN);
  vcl_fscanf (fp, "# Dege_A1A3_ms_nodes: %d\n", &nDegeA1A3MN);
  vcl_fscanf (fp, "# Dege_A14_ms_nodes: %d\n", &nDegeA14MN);
  vcl_fscanf (fp, "# Loop_ms_nodes: %d\n", &nLoopMN);
  vcl_fscanf (fp, "Id: vertex_elm_id, time\n");
  vcl_fscanf (fp, "\t# incident_ms_curves: [type: R(rib) A(axial) D(degenerate) L(loopend)] ids\n");
  vcl_fscanf (fp, "-----------\n");

  if (verbose)
    vul_printf (vcl_cout, "\tLoading # ms_nodes: %u.\n", nMN);
  for (int i=0; i<nMN; i++) {
    dbsk3d_ms_node* MN = (dbsk3d_ms_node*) ms_hypg->_new_vertex ();
    ms_node_file_read* MN_fileread = new ms_node_file_read;
    mn_load_text_file (fp, MN, MN_fileread, ms_hypg->fs_mesh());
    ms_hypg->_add_vertex (MN);
    MN_fileread_map.insert (vcl_pair<int, ms_node_file_read*> (MN_fileread->id_, MN_fileread));
  }
  assert (int(ms_hypg->vertexmap().size()) == nMN);  

  //Load coarse-scale shock scaffold curves
  int nMC;
  int nA3MC;
  int nA13MC;
  int nDegeMC;
  int nLoopMC;
  ///int nVirtual;
  vcl_fscanf (fp, "\n\n# Medial Scaffold Curves: %d\n", &nMC);
  vcl_fscanf (fp, "# A3_ms_curves: %d\n", &nA3MC);
  vcl_fscanf (fp, "# A13_ms_curves: %d\n", &nA13MC);
  vcl_fscanf (fp, "# Dege_ms_curves: %d\n", &nDegeMC);
  vcl_fscanf (fp, "# Loop_ms_curves: %d\n", &nLoopMC);
  ///vcl_fscanf (fp, "# Virtual_ms_curves: %d\n", &nVirtual);
  vcl_fscanf (fp, "Id: ms_nodes (S, E)\n");
  vcl_fscanf (fp, "-----------\n");

  if (verbose)
    vul_printf (vcl_cout, "\tLoading # ms_curves: %u.\n", nMC);
  for (int i=0; i<nMC; i++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) ms_hypg->_new_edge (NULL, NULL);
    mc_load_text_file (fp, MC, ms_hypg->fs_mesh(), ms_hypg);
    ms_hypg->_add_edge (MC);
  }
  assert (int(ms_hypg->edgemap().size()) == nMC);

  //Load coarse-scale shock scaffold sheets
  unsigned int nMS;
  vcl_fscanf (fp, "\n\n# Medial Scaffold Sheets: %u\n", &nMS);
  vcl_fscanf (fp, "Id:\n");
  vcl_fscanf (fp, "\t# bnd_curves: ids in order\n");
  vcl_fscanf (fp, "\t# i_curves: ids\n");
  vcl_fscanf (fp, "\t# face_elms: ids\n");
  vcl_fscanf (fp, "-----------\n");

  if (verbose)
    vul_printf (vcl_cout, "\tLoading # ms_sheets: %u.\n", nMS);
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
  vcl_fscanf (fp, "\n\nnode_id_counter: %u\n", &vertex_id_counter);
  vcl_fscanf (fp, "curve_id_counter: %u\n", &edge_id_counter);
  vcl_fscanf (fp, "sheet_id_counter: %u\n", &sheet_id_counter);
  ms_hypg->set_vertex_id_counter (vertex_id_counter);
  ms_hypg->set_edge_id_counter (edge_id_counter);
  ms_hypg->set_sheet_id_counter (sheet_id_counter);

  vcl_fscanf (fp, "-----------\n");
  fclose (fp);

  //Now process the Id information and make the connectivity between real objects
  //For each dbsk3d_ms_node, recover MC/MS connectivity
  vcl_map<int, dbmsh3d_vertex*>::iterator MN_it = ms_hypg->vertexmap().begin();
  for (; MN_it != ms_hypg->vertexmap().end(); MN_it++) {
    dbsk3d_ms_node* MN = (dbsk3d_ms_node*) (*MN_it).second;
    int id = MN->id();
    ms_node_file_read* MN_fileread = MN_fileread_map[id];
    mn_recover_pointers (MN, MN_fileread, ms_hypg);
    MN_fileread_map.erase (id);
  }
  assert (MN_fileread_map.size() == 0);

  if (verbose)
    vul_printf (vcl_cout, "\tdone.\n", nMS);
  return true;
}
