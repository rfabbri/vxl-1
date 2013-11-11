//: This is 3DShock_Scaffold_Graph_SA_FileIO.cxx
//  MingChing Chang
//  Feb 29, 2004  Creation

#include <vcl_cassert.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
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
    vcl_fprintf(fp, "A3");
  break;
  case C_TYPE_AXIAL:
    vcl_fprintf(fp, "A13");
  break;
  case C_TYPE_DEGE_AXIAL:
    vcl_fprintf(fp, "Dege");
  break;
  default:
    assert (0);
  break;
  }

  MC->compute_length ();
  vcl_fprintf (fp, " %d: (%d, %d), %lf\n", MC->id(), 
               MC->sV()->id(), MC->eV()->id(), MC->length());

  // NodeElements
  vcl_fprintf (fp, "\tNodeElm %d:", (unsigned int) MC->V_vec().size());
  for (unsigned int j=0; j<MC->V_vec().size(); j++) {
    const dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) MC->V_vec(j);
    assert (sg_sa->FV_map (FV->id()) != NULL);
    vcl_fprintf (fp, " %d", FV->id());
  }
  vcl_fprintf (fp, "\n");
}

void sc_load_text_file_sg (dbsk3d_sg_sa* sg_sa,
                           dbsk3d_ms_curve* MC, FILE* fp)
{  
  char c_type[128];
  vcl_fscanf(fp, "%s", c_type);
  vcl_string type (c_type);
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
  vcl_fscanf (fp, " %d: (%d, %d), %lf\n", &id, &svid, &evid, &length);
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
  vcl_fscanf (fp, "\tNodeElm %d:", &nN);
  for (unsigned int j=0; j<(unsigned int)nN; j++) {
    int id;
    vcl_fscanf (fp, " %d", &id);
    const dbsk3d_fs_vertex* FV = sg_sa->FV_map (id);
    assert (FV);
    MC->add_V_to_back (FV);
  }  
  vcl_fscanf (fp, "\n");

  MC->set_data_type (C_DATA_TYPE_VERTEX);
}

//##################################################################

void save_to_sg (dbsk3d_sg_sa* sg_sa, const char *pc_file_sg)
{
  vul_printf (vcl_cout, "\ndbsk3d_ms_graph_sa::save_sg (%s)\n", pc_file_sg);
  
  FILE  *fp;
  if ((fp = fopen(pc_file_sg, "w")) == NULL) {
    vul_printf (vcl_cout, "ERROR: Can't open output file %s.\n", pc_file_sg);
    return; 
  }

  vcl_fprintf (fp, "Scaffold Graph File v1.0\n");

  //:FV) Save A14NodeElements
  vcl_fprintf (fp, "\n# fs_vertex_elms: %u\n", (unsigned int) sg_sa->FV_map().size());
  vcl_fprintf (fp, "Id: Coord (x, y, z), shockType {3, 4, R, A, S, D, I, ?}\n");
  vcl_fprintf (fp, "-----------\n");
  vcl_cout<< "\tSaving "<<sg_sa->FV_map().size()<<" fs_vertices.\n";
  
  vcl_map<int, dbsk3d_fs_vertex*>::iterator it = sg_sa->FV_map().begin();
  for (; it != sg_sa->FV_map().end(); it++) {
    dbsk3d_fs_vertex* FV = (*it).second;
    fv_save_text_file (fp, FV);
  }
  vcl_fprintf (fp, "----------\n");

  //:V) Save ScaffoldVertices
  int nA1A3MN = 0;
  int nA14MN = 0;
  int nDegeA1A3MN = 0;
  int nDegeA14MN = 0;
  int nLoopMN = 0;
  count_ms_vertices (sg_sa, nA1A3MN, nA14MN, nDegeA1A3MN, nDegeA14MN, nLoopMN);
  vul_printf (vcl_cout, "\tSaving %d ms_nodes (%d A1A3, %d A14, %d DegeA1A3, %d DegeA14, %d LoopEnd).\n",
              sg_sa->vertexmap().size(), nA1A3MN, nA14MN, nDegeA1A3MN, nDegeA14MN, nLoopMN);
  vcl_fprintf (fp, "\n\n# Medial Scaffold Nodes: %d\n", sg_sa->vertexmap().size());
  vcl_fprintf (fp, "# A1A3_ms_nodes: %d\n", nA1A3MN);
  vcl_fprintf (fp, "# A14_ms_nodes: %d\n", nA14MN);
  vcl_fprintf (fp, "# Dege_A1A3_ms_nodes: %d\n", nDegeA1A3MN);
  vcl_fprintf (fp, "# Dege_A14_ms_nodes: %d\n", nDegeA14MN);
  vcl_fprintf (fp, "# Loop_ms_nodes: %d\n", nLoopMN);
  vcl_fprintf (fp, "Id: vertex_elm_id, time\n");
  vcl_fprintf (fp, "\t# incident_ms_curves: [type: R(rib) A(axial) D(degenerate) L(loopend)] ids\n");
  vcl_fprintf (fp, "-----------\n");

  vcl_map<int, dbmsh3d_vertex*>::iterator SV_it = sg_sa->vertexmap().begin();
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
  vul_printf (vcl_cout, "\tSaving %d ms_curves (%d A13, %d A13, %d Dege, %d Loop).\n",
              sg_sa->edgemap().size(), nA3MC, nA13MC, nDegeMC, nLoopMC);
  vcl_fprintf (fp, "\n\n# Medial Scaffold Curves: %d\n", sg_sa->edgemap().size());
  vcl_fprintf (fp, "# A3_ms_curves: %d\n", nA3MC);
  vcl_fprintf (fp, "# A13_ms_curves: %d\n", nA13MC);
  vcl_fprintf (fp, "# Dege_ms_curves: %d\n", nDegeMC);
  vcl_fprintf (fp, "# Loop_ms_curves: %d\n", nLoopMC);
  vcl_fprintf (fp, "Id: ms_nodes (S, E)\n");
  vcl_fprintf (fp, "-----------\n");

  vcl_map<int, dbmsh3d_edge*>::iterator SC_it = sg_sa->edgemap().begin();
  for (; SC_it != sg_sa->edgemap().end(); SC_it++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*SC_it).second;
    sc_save_text_file_sg (sg_sa, MC, fp);
  }

  //: Counters
  vcl_fprintf (fp, "\n\nnode_id_counter: %d\n", sg_sa->vertex_id_counter());
  vcl_fprintf (fp, "curves_id_counter: %d\n", sg_sa->edge_id_counter());
  vcl_fprintf (fp, "FV_id_counter: %d\n", sg_sa->FV_id_counter());  
  vcl_fprintf (fp, "-----------\n");

  fclose (fp);
}

bool load_from_sg (dbsk3d_sg_sa* sg_sa, const char *pc_file_sg)
{
  vul_printf (vcl_cout, "\ndbsk3d_ms_graph_sa::load_sg (%s)\n", pc_file_sg);

  FILE    *fp;
  if ((fp = fopen(pc_file_sg, "r")) == NULL) {
    vul_printf (vcl_cout, "ERROR: Can't open input file %s.\n", pc_file_sg);
    return false; 
  }

  vcl_map<int, ms_node_file_read*> MN_fileread_;

  vcl_fscanf (fp, "Scaffold Graph File v1.0\n");
  
  //:FV) Load A14NodeElements
  int nN;
  vcl_fscanf (fp, "\n# fs_vertex_elms: %u\n", &nN);
  vcl_fscanf (fp, "Id: Coord (x, y, z), shockType {3, 4, R, A, S, D, I, ?}\n");
  vcl_fscanf (fp, "-----------\n");
  vcl_cout<< "\tLoading "<<nN<<" fs_vertices.\n";

  for (int i=0; i<nN; i++) {
    dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) sg_sa->_new_FV();
    fv_load_text_file_sg (fp, FV);
    sg_sa->_add_FV (FV);
  }
  vcl_fscanf (fp, "----------\n");

  //:V) Load ScaffoldVertices
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

  vul_printf (vcl_cout, "\tLoading # ms_nodes: %u.\n", nMN);
  for (int i=0; i<nMN; i++) {
    // \todo: work on a generic graph structure to solve this issue!
    dbsk3d_ms_node* MN = (dbsk3d_ms_node*) sg_sa->_new_vertex();
    ms_node_file_read* mn_fileread = new ms_node_file_read;
    mn_load_text_file_sg (fp, MN, mn_fileread, sg_sa); ///sv_load_text_file_sg_old
    sg_sa->_add_vertex (MN);
    MN_fileread_.insert (vcl_pair<int, ms_node_file_read*> (mn_fileread->id_, mn_fileread));
  }
  assert (int (sg_sa->vertexmap().size()) == nMN);

  //:C1) Load ScaffoldCurves
  int nMC;
  int nA3MC;
  int nA13MC;
  int nDegeMC;
  int nLoopMC;
  vcl_fscanf (fp, "\n\n# Medial Scaffold Curves: %d\n", &nMC);
  vcl_fscanf (fp, "# A3_ms_curves: %d\n", &nA3MC);
  vcl_fscanf (fp, "# A13_ms_curves: %d\n", &nA13MC);
  vcl_fscanf (fp, "# Dege_ms_curves: %d\n", &nDegeMC);
  vcl_fscanf (fp, "# Loop_ms_curves: %d\n", &nLoopMC);
  vcl_fscanf (fp, "Id: ms_nodes (S, E)\n");
  vcl_fscanf (fp, "-----------\n");

  vul_printf (vcl_cout, "\tLoading # ms_curves: %u.\n", nMC);
  for (int i=0; i<nMC; i++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) sg_sa->_new_edge (NULL, NULL);
    sc_load_text_file_sg (sg_sa, MC, fp);
    sg_sa->_add_edge (MC);
  }

  //: Counters
  int counter;
  vcl_fscanf (fp, "\n\nnode_id_counter: %d\n", &counter);
  sg_sa->set_vertex_id_counter (counter);
  vcl_fscanf (fp, "curve_id_counter: %d\n", &counter);
  sg_sa->set_edge_id_counter (counter);
  vcl_fscanf (fp, "FV_id_counter: %d\n", &counter);
  if (counter > sg_sa->FV_id_counter())
    sg_sa->set_FV_id_counter (counter);
  vcl_fscanf (fp, "-----------\n");

  fclose (fp);

  //: Now process the Id information and make the connectivity between real objects
  //1) For each dbsk3d_ms_node, recover MC/SSheet connectivity
  vcl_map<int, dbmsh3d_vertex*>::iterator MN_it = sg_sa->vertexmap().begin();
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


