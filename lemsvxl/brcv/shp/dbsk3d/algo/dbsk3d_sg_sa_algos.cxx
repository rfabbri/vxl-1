//: This is dbsk3d/algo/dbsk3d_sg_sa_algos.cxx
//  MingChing Chang 
//  Sep 4, 2007

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_ctime.h>
#include <vcl_iostream.h>
#include <vgl/vgl_point_3d.h>
#include <vnl/vnl_random.h>
#include <vul/vul_printf.h>

#include <dbsk3d/dbsk3d_ms_curve.h>
#include <dbsk3d/dbsk3d_ms_hypg.h>
#include <dbsk3d/dbsk3d_sg_sa.h>
#include <dbsk3d/algo/dbsk3d_sg_sa_algos.h>


//: Build a stand along shock scaffold graph.
dbsk3d_sg_sa* build_sg_sa (dbsk3d_ms_hypg* ms_hypg)
{
  vul_printf (vcl_cout, "build_sg_sa(): %d vertices, %d curves.\n", 
              ms_hypg->vertexmap().size(), ms_hypg->edgemap().size());
  dbsk3d_sg_sa* SG = new dbsk3d_sg_sa;

  //Build the coarse-scale shock vertices
  vcl_map<int, dbmsh3d_vertex*>::iterator SV_it = ms_hypg->vertexmap().begin();
  for (; SV_it != ms_hypg->vertexmap().end(); SV_it++) {
    dbsk3d_ms_node* MN = (dbsk3d_ms_node*) (*SV_it).second;

    dbsk3d_ms_node* newMN = (dbsk3d_ms_node*) MN->clone ();
    SG->_add_vertex (newMN);

    dbsk3d_fs_vertex* newFV = (dbsk3d_fs_vertex*) MN->FV()->clone ();
    newMN->set_V (newFV);
    SG->_add_FV (newFV);
  }

  //Build the coarse-scale shock curves.
  vcl_map<int, dbmsh3d_edge*>::iterator SC_it = ms_hypg->edgemap().begin();
  for (; SC_it != ms_hypg->edgemap().end(); SC_it++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*SC_it).second;

    //Skip all loop-type shock curves.
    if (MC->is_self_loop())
      continue;

    dbsk3d_ms_curve* newSC = new dbsk3d_ms_curve (NULL, NULL, MC->id());
    newSC->set_c_type (MC->c_type());
    newSC->set_data_type (C_DATA_TYPE_VERTEX);

    //Assign the correct MC-MN connectivity
    for (int j=0; j<2; j++) {
      int cid = (j==0) ? MC->sV()->id() : MC->eV()->id();
      dbsk3d_ms_node* connMN = (dbsk3d_ms_node*) SG->vertexmap (cid);
      assert (connMN);
      newSC->connect_V (j, connMN);
      newSC->add_V_to_back (connMN->V());
    }
    SG->_add_edge (newSC);

    //For each fs_vertices of the MC, clone a new one and insert it to SG,
    //except the starting and ending ones. Also, put the new fs_vertices into newSC.
    //Be careful to avoid duplication at junctions.
    vcl_vector<dbmsh3d_vertex*> V_vec;
    MC->get_V_vec (V_vec);
    assert (V_vec.size() > 1);
    for (unsigned int j=1; j<V_vec.size()-1; j++) {
      const dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) V_vec[j];
      //clone the fs_vertex
      dbsk3d_fs_vertex* newFV = (dbsk3d_fs_vertex*) FV->clone ();
      SG->_add_FV (newFV);      
      newSC->add_V_to_pos (j, newFV);
    }
  }

  //Setup the idcounters
  SG->set_vertex_id_counter (ms_hypg->vertex_id_counter());
  SG->set_edge_id_counter (ms_hypg->edge_id_counter());
  SG->set_FV_id_counter (ms_hypg->fs_mesh()->vertex_id_counter());

  return SG;
}

bool dbsk3d_apply_xform (dbsk3d_sg_sa* SG, const vgl_h_matrix_3d<double>& H)
{
  vul_printf (vcl_cout, "  dbmsh3d_apply_xform on SG: %u points", SG->FV_map().size());

  //Iterate thru all the points in M and compute its image after transformation
  vcl_map<int, dbsk3d_fs_vertex*>::iterator it = SG->FV_map().begin();
  for (; it != SG->FV_map().end(); it++) {
    dbsk3d_fs_vertex* FV = (*it).second;

    vgl_homg_point_3d<double > Ph (FV->pt());
    vgl_homg_point_3d<double > xPh = H (Ph);
    double vx, vy, vz;
    if (!xPh.get_nonhomogeneous(vx, vy, vz)) {
      vcl_cerr << "Error in " << __FILE__ << " : Pt at infinity\n";
      return false;
    }
    FV->get_pt().set(vx, vy, vz);
  }
  vcl_cout << "  done.\n";
  return true;
}

void make_sub_graph (dbsk3d_sg_sa* SG, const float R, const bool keep_isolated_vertex)
{
  vnl_random get_rand_sub;
  get_rand_sub.reseed ((unsigned int) vcl_time(NULL));

  //Randomly remove nER edges from SG.
  const int nER = (int) vcl_ceil (R * SG->edgemap().size());
  int nE_removed = 0;
  while (SG->edgemap().size() > 0 && nE_removed < nER) {
    //Remove a random edge.
    vcl_map<int, dbmsh3d_edge*>::iterator it = SG->edgemap().begin();
    int n = (int) get_rand_sub.drand32 (0.0, SG->edgemap().size());
    for (int i=0; i<n; i++)
      it++;
    dbmsh3d_edge* E = (*it).second;
    dbmsh3d_vertex* sV = E->vertices(0);
    dbmsh3d_vertex* eV = E->vertices(1);
    SG->remove_edge (E);

    if (keep_isolated_vertex == false) {
      //Remove isolated vertex (with no incident edge).
      if (sV->n_incident_Es() == 0)
        SG->remove_vertex (sV->id());
      if (eV->n_incident_Es() == 0)
        SG->remove_vertex (eV->id());
    }
    nE_removed++;
  }

  vul_printf (vcl_cout, "make_sub_graph(): %d edges removed, %d remain.\n",
              nE_removed, SG->edgemap().size());
}

float get_avg_V_radius (dbsk3d_sg_sa* SG)
{
  double avgR = 0;
  vcl_map<int, dbmsh3d_vertex*>::iterator it = SG->vertexmap().begin();
  for (; it != SG->vertexmap().end(); it++) {
    dbsk3d_ms_node* MN = (dbsk3d_ms_node*) (*it).second;
    double r = MN->radius();
    avgR += r;
  }
  avgR /= SG->vertexmap().size();
  vul_printf (vcl_cout, "get_avg_V_radius(): %u vertices, avg radius: %f.\n", 
              SG->vertexmap().size(), avgR);
  return (float) avgR;
}

float get_avg_C_len (dbsk3d_sg_sa* SG)
{
  double avgL = 0;  
  vcl_map<int, dbmsh3d_edge*>::iterator it = SG->edgemap().begin();
  for (; it != SG->edgemap().end(); it++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*it).second;
    double l = MC->get_length();
    avgL += l;
  }
  avgL /= SG->edgemap().size();
  vul_printf (vcl_cout, "get_avg_C_len(): %u edges, avg radius: %f.\n", 
              SG->edgemap().size(), avgL);
  return (float) avgL;
}

void perturb_graph_vertex_radius (dbsk3d_sg_sa* SG, const float nrv)
{
  vul_printf (vcl_cout, "perturb_graph_vertex_radius(): nrv %f.\n", nrv);
  float avgR = get_avg_V_radius (SG);
  vnl_random get_rand;
  get_rand.reseed ((unsigned int) vcl_time(NULL));

  vcl_map<int, dbmsh3d_vertex*>::iterator it = SG->vertexmap().begin();
  for (; it != SG->vertexmap().end(); it++) {
    dbsk3d_ms_node* MN = (dbsk3d_ms_node*) (*it).second;
    double maxr = avgR * nrv;
    double r = get_rand.drand32 (0.0, maxr);
    MN->set_radius (float(MN->radius() + r));
  }
}

void perturb_graph_edge_len (dbsk3d_sg_sa* SG, const float nrc)
{
  vul_printf (vcl_cout, "perturb_graph_edge_len(): nrc %f.\n", nrc);
  float avgL = get_avg_C_len (SG);
  vnl_random get_rand;
  get_rand.reseed ((unsigned int) vcl_time(NULL));
  
  vcl_map<int, dbmsh3d_edge*>::iterator it = SG->edgemap().begin();
  for (; it != SG->edgemap().end(); it++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*it).second;
    double maxl = avgL * nrc;
    double r = get_rand.drand32 (0.0, maxl);
    MC->set_length (float(MC->get_length() + r));
  }
}

int remove_A3_shock_curves (dbsk3d_sg_sa* SG)
{
  int n_removed = 0;
  vcl_map<int, dbmsh3d_edge*>::iterator it = SG->edgemap().begin();
  while (it != SG->edgemap().end()) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*it).second;
    if (MC->c_type() == C_TYPE_RIB) {
      //Delete this MC.
      it++;
      SG->remove_edge (MC);
      n_removed++;
    }
    else {
      assert (MC->c_type() == C_TYPE_AXIAL || MC->c_type() == C_TYPE_DEGE_AXIAL);
      it++;
    }
  }
  return n_removed;
}







