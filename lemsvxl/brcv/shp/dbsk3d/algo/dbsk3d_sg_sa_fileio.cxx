//: This is 3DShock_Scaffold_Graph_SA_FileIO.cxx
//  MingChing Chang
//  Feb 29, 2004  Creation

#include <cassert>
#include <string>
#include <iostream>
#include <vul/vul_printf.h>

#include <dbsk3d/dbsk3d_ms_node.h>
#include <dbsk3d/dbsk3d_ms_curve.h>
#include <dbsk3d/dbsk3d_fs_mesh.h>
#include <dbsk3d/dbsk3d_sg_sa.h>
#include <dbsk3d/algo/dbsk3d_ms_algos.h>

void sc_save_text_file_sg (dbsk3d_sg_sa* sg_sa, 
                           dbsk3d_ms_curve* MC, FILE* fp)
{
  //: For each dbsk3d_ms_curve
  switch (MC->c_type()) {
  case C_TYPE_RIB:
    std::fprintf(fp, "A3");
  break;
  case C_TYPE_AXIAL:
    std::fprintf(fp, "A13");
  break;
  case C_TYPE_DEGE_AXIAL:
    std::fprintf(fp, "Dege");
  break;
  default:
    assert (0);
  break;
  }

  MC->compute_length ();
  std::fprintf (fp, " %d: (%d, %d), %lf\n", MC->id(), 
               MC->sV()->id(), MC->eV()->id(), MC->length());

  // NodeElements
  std::fprintf (fp, "\tNodeElm %d:", (unsigned int) MC->V_vec().size());
  for (unsigned int j=0; j<MC->V_vec().size(); j++) {
    const dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) MC->V_vec(j);
    assert (sg_sa->FV_map (FV->id()) != NULL);
    std::fprintf (fp, " %d", FV->id());
  }
  std::fprintf (fp, "\n");
}

void sc_load_text_file_sg (dbsk3d_sg_sa* sg_sa,
                           dbsk3d_ms_curve* MC, FILE* fp)
{  
  char c_type[128];
  std::fscanf(fp, "%s", c_type);
  std::string type (c_type);
  if (type == "A3")
    MC->set_c_type (C_TYPE_RIB);
  else if (type == "A13")
    MC->set_c_type (C_TYPE_AXIAL);
  else if (type == "Dege")
    MC->set_c_type (C_TYPE_DEGE_AXIAL);
  else
    assert (0);

  //: For each dbsk3d_ms_curve
  int id;
  int svid, evid;
  double length;
  std::fscanf (fp, " %d: (%d, %d), %lf\n", &id, &svid, &evid, &length);
  assert (svid >= 0);
  assert (evid >= 0);
  
  //: Use the id from the file
  MC->set_id (id);

  // Recover the connectivity.
  if (MC->c_type() == C_TYPE_RIB || MC->c_type() == C_TYPE_AXIAL || MC->c_type() == C_TYPE_DEGE_AXIAL) {
    dbsk3d_ms_node* connMN = (dbsk3d_ms_node*) sg_sa->vertexmap(svid);
    assert (connMN->id() == svid);
    MC->_set_vertex (0, connMN);

    connMN = (dbsk3d_ms_node*) sg_sa->vertexmap(evid);
    assert (connMN->id() == evid);
    MC->_set_vertex (1, connMN);
  }
  else
    assert (0);

  // NodeElements
  int nN;
  std::fscanf (fp, "\tNodeElm %d:", &nN);
  for (unsigned int j=0; j<(unsigned int)nN; j++) {
    int id;
    std::fscanf (fp, " %d", &id);
    const dbsk3d_fs_vertex* FV = sg_sa->FV_map (id);
    assert (FV);
    MC->add_V_to_back (FV);
  }  
  std::fscanf (fp, "\n");

  MC->set_data_type (C_DATA_TYPE_VERTEX);
}

//##################################################################

void save_to_sg (dbsk3d_sg_sa* sg_sa, const char *pc_file_sg)
{
  vul_printf (std::cout, "\ndbsk3d_ms_graph_sa::save_sg (%s)\n", pc_file_sg);
  
  FILE  *fp;
  if ((fp = fopen(pc_file_sg, "w")) == NULL) {
    vul_printf (std::cout, "ERROR: Can't open output file %s.\n", pc_file_sg);
    return; 
  }

  std::fprintf (fp, "Scaffold Graph File v1.0\n");

  //:FV) Save A14NodeElements
  std::fprintf (fp, "\n# fs_vertex_elms: %u\n", (unsigned int) sg_sa->FV_map().size());
  std::fprintf (fp, "Id: Coord (x, y, z), shockType {3, 4, R, A, S, D, I, ?}\n");
  std::fprintf (fp, "-----------\n");
  std::cout<< "\tSaving "<<sg_sa->FV_map().size()<<" fs_vertices.\n";
  
  std::map<int, dbsk3d_fs_vertex*>::iterator it = sg_sa->FV_map().begin();
  for (; it != sg_sa->FV_map().end(); it++) {
    dbsk3d_fs_vertex* FV = (*it).second;
    fv_save_text_file (fp, FV);
  }
  std::fprintf (fp, "----------\n");

  //:V) Save ScaffoldVertices
  int nA1A3MN = 0;
  int nA14MN = 0;
  int nDegeA1A3MN = 0;
  int nDegeA14MN = 0;
  int nLoopMN = 0;
  count_ms_vertices (sg_sa, nA1A3MN, nA14MN, nDegeA1A3MN, nDegeA14MN, nLoopMN);
  vul_printf (std::cout, "\tSaving %d ms_nodes (%d A1A3, %d A14, %d DegeA1A3, %d DegeA14, %d LoopEnd).\n",
              sg_sa->vertexmap().size(), nA1A3MN, nA14MN, nDegeA1A3MN, nDegeA14MN, nLoopMN);
  std::fprintf (fp, "\n\n# Medial Scaffold Nodes: %d\n", sg_sa->vertexmap().size());
  std::fprintf (fp, "# A1A3_ms_nodes: %d\n", nA1A3MN);
  std::fprintf (fp, "# A14_ms_nodes: %d\n", nA14MN);
  std::fprintf (fp, "# Dege_A1A3_ms_nodes: %d\n", nDegeA1A3MN);
  std::fprintf (fp, "# Dege_A14_ms_nodes: %d\n", nDegeA14MN);
  std::fprintf (fp, "# Loop_ms_nodes: %d\n", nLoopMN);
  std::fprintf (fp, "Id: vertex_elm_id, time\n");
  std::fprintf (fp, "\t# incident_ms_curves: [type: R(rib) A(axial) D(degenerate) L(loopend)] ids\n");
  std::fprintf (fp, "-----------\n");

  std::map<int, dbmsh3d_vertex*>::iterator SV_it = sg_sa->vertexmap().begin();
  for (; SV_it != sg_sa->vertexmap().end(); SV_it++) {
    dbsk3d_ms_node* MN = (dbsk3d_ms_node*) (*SV_it).second;
    mn_save_text_file_sg (fp, MN);
  }

  //:C) Save ScaffoldCurves
  int nA3MC = 0;
  int nA13MC = 0;
  int nDegeMC = 0;
  int nLoopMC = 0;
  int nVirtual = 0;
  count_ms_curves (sg_sa, nA3MC, nA13MC, nDegeMC, nLoopMC, nVirtual);
  vul_printf (std::cout, "\tSaving %d ms_curves (%d A13, %d A13, %d Dege, %d Loop).\n",
              sg_sa->edgemap().size(), nA3MC, nA13MC, nDegeMC, nLoopMC);
  std::fprintf (fp, "\n\n# Medial Scaffold Curves: %d\n", sg_sa->edgemap().size());
  std::fprintf (fp, "# A3_ms_curves: %d\n", nA3MC);
  std::fprintf (fp, "# A13_ms_curves: %d\n", nA13MC);
  std::fprintf (fp, "# Dege_ms_curves: %d\n", nDegeMC);
  std::fprintf (fp, "# Loop_ms_curves: %d\n", nLoopMC);
  std::fprintf (fp, "Id: ms_nodes (S, E)\n");
  std::fprintf (fp, "-----------\n");

  std::map<int, dbmsh3d_edge*>::iterator SC_it = sg_sa->edgemap().begin();
  for (; SC_it != sg_sa->edgemap().end(); SC_it++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*SC_it).second;
    sc_save_text_file_sg (sg_sa, MC, fp);
  }

  //: Counters
  std::fprintf (fp, "\n\nnode_id_counter: %d\n", sg_sa->vertex_id_counter());
  std::fprintf (fp, "curves_id_counter: %d\n", sg_sa->edge_id_counter());
  std::fprintf (fp, "FV_id_counter: %d\n", sg_sa->FV_id_counter());  
  std::fprintf (fp, "-----------\n");

  fclose (fp);
}

bool load_from_sg (dbsk3d_sg_sa* sg_sa, const char *pc_file_sg)
{
  vul_printf (std::cout, "\ndbsk3d_ms_graph_sa::load_sg (%s)\n", pc_file_sg);

  FILE    *fp;
  if ((fp = fopen(pc_file_sg, "r")) == NULL) {
    vul_printf (std::cout, "ERROR: Can't open input file %s.\n", pc_file_sg);
    return false; 
  }

  std::map<int, ms_node_file_read*> MN_fileread_;

  std::fscanf (fp, "Scaffold Graph File v1.0\n");
  
  //:FV) Load A14NodeElements
  int nN;
  std::fscanf (fp, "\n# fs_vertex_elms: %u\n", &nN);
  std::fscanf (fp, "Id: Coord (x, y, z), shockType {3, 4, R, A, S, D, I, ?}\n");
  std::fscanf (fp, "-----------\n");
  std::cout<< "\tLoading "<<nN<<" fs_vertices.\n";

  for (int i=0; i<nN; i++) {
    dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) sg_sa->_new_FV();
    fv_load_text_file_sg (fp, FV);
    sg_sa->_add_FV (FV);
  }
  std::fscanf (fp, "----------\n");

  //:V) Load ScaffoldVertices
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

  vul_printf (std::cout, "\tLoading # ms_nodes: %u.\n", nMN);
  for (int i=0; i<nMN; i++) {
    // \todo: work on a generic graph structure to solve this issue!
    dbsk3d_ms_node* MN = (dbsk3d_ms_node*) sg_sa->_new_vertex();
    ms_node_file_read* mn_fileread = new ms_node_file_read;
    mn_load_text_file_sg (fp, MN, mn_fileread, sg_sa); ///sv_load_text_file_sg_old
    sg_sa->_add_vertex (MN);
    MN_fileread_.insert (std::pair<int, ms_node_file_read*> (mn_fileread->id_, mn_fileread));
  }
  assert (int (sg_sa->vertexmap().size()) == nMN);

  //:C1) Load ScaffoldCurves
  int nMC;
  int nA3MC;
  int nA13MC;
  int nDegeMC;
  int nLoopMC;
  std::fscanf (fp, "\n\n# Medial Scaffold Curves: %d\n", &nMC);
  std::fscanf (fp, "# A3_ms_curves: %d\n", &nA3MC);
  std::fscanf (fp, "# A13_ms_curves: %d\n", &nA13MC);
  std::fscanf (fp, "# Dege_ms_curves: %d\n", &nDegeMC);
  std::fscanf (fp, "# Loop_ms_curves: %d\n", &nLoopMC);
  std::fscanf (fp, "Id: ms_nodes (S, E)\n");
  std::fscanf (fp, "-----------\n");

  vul_printf (std::cout, "\tLoading # ms_curves: %u.\n", nMC);
  for (int i=0; i<nMC; i++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) sg_sa->_new_edge (NULL, NULL);
    sc_load_text_file_sg (sg_sa, MC, fp);
    sg_sa->_add_edge (MC);
  }

  //: Counters
  int counter;
  std::fscanf (fp, "\n\nnode_id_counter: %d\n", &counter);
  sg_sa->set_vertex_id_counter (counter);
  std::fscanf (fp, "curve_id_counter: %d\n", &counter);
  sg_sa->set_edge_id_counter (counter);
  std::fscanf (fp, "FV_id_counter: %d\n", &counter);
  if (counter > sg_sa->FV_id_counter())
    sg_sa->set_FV_id_counter (counter);
  std::fscanf (fp, "-----------\n");

  fclose (fp);

  //: Now process the Id information and make the connectivity between real objects
  //1) For each dbsk3d_ms_node, recover MC/SSheet connectivity
  std::map<int, dbmsh3d_vertex*>::iterator MN_it = sg_sa->vertexmap().begin();
  for (; MN_it != sg_sa->vertexmap().end(); MN_it++) {
    dbsk3d_ms_node* MN = (dbsk3d_ms_node*) (*MN_it).second;
    int id = MN->id();
    ms_node_file_read* mn_fileread = MN_fileread_[id];
    mn_recover_pointers_sg (MN, mn_fileread, sg_sa);
    MN_fileread_.erase (id);
  }

  assert (MN_fileread_.size() == 0);

  return true;
}


