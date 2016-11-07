//: This is dbsk3d_sg_sa_algos.h
//  Medial Scaffold Graph Transitions
//  MingChing Chang 061117 

#ifndef dbsk3d_sg_sa_algos_h_
#define dbsk3d_sg_sa_algos_h_

#include <vgl/algo/vgl_h_matrix_3d.h>
#include <dbsk3d/dbsk3d_sg_sa.h>

//: Build stand alone shock scaffold graph 
dbsk3d_sg_sa* build_sg_sa (dbsk3d_ms_hypg* ms_hypg);

bool dbsk3d_apply_xform (dbsk3d_sg_sa* SG, const vgl_h_matrix_3d<double>& H);

//: Smoothing the medial scaffold curves
void gaussian_sm_sg_sa (dbsk3d_sg_sa* SG, const unsigned int nTimes = 5);

void gaussian_sm_SC (dbsk3d_ms_curve* MC, int start, int end, int nTimes);

///void gaussian_sm_sg_sa_0 (dbsk3d_sg_sa* sg_sa, const unsigned int nTimes = 5);

void make_sub_graph (dbsk3d_sg_sa* SG, const float R, const bool keep_isolated_vertex);

float get_avg_V_radius (dbsk3d_sg_sa* SG);
float get_avg_C_len (dbsk3d_sg_sa* SG);

void perturb_graph_vertex_radius (dbsk3d_sg_sa* SG, const float nrv);

void perturb_graph_edge_len (dbsk3d_sg_sa* SG, const float nrc);

int remove_A3_shock_curves (dbsk3d_sg_sa* SG);

//: Class to handle (stand alone) shock scaffold graph transitions.
class dbsk3d_sg_sa_trans
{
protected:
  dbsk3d_sg_sa*   sg_sa_;

  vcl_vector<dbsk3d_ms_curve*>   A15_Transition_List_;

public:
  //###### Constructor/Destructor ######
  dbsk3d_sg_sa_trans (dbsk3d_sg_sa* sg_sa) {
    sg_sa_ = sg_sa;
  }
  virtual ~dbsk3d_sg_sa_trans () {
  }

  //###### Data access functions ######
  dbsk3d_sg_sa* sg_sa () {
    return sg_sa_;
  }
  void set_sg_sa (dbsk3d_sg_sa* sg_sa) {
    sg_sa_ = sg_sa;
  }
  void set_ms_hypg (dbsk3d_sg_sa* sg_sa) {
    sg_sa_ = sg_sa;
  }

  //###### Transition Handling Functions ######
  bool graph_trans_A5 (float A5_th);
  bool graph_trans_A15 (float A15_th);
    dbsk3d_ms_curve* _detect_next_A15 (float A15_th);
    void _run_trans_A15 (dbsk3d_ms_curve* A13);

  bool graph_trans_remove_dummy_nodes ();

};

#endif
