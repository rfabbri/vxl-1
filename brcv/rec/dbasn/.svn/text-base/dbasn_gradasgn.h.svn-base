//---------------------------------------------------------------------
// This is brcv/rec/dbasn/dbasn_gradasgn.h
//:
// \file
// \brief class to find graduated assignment between two graphs
//
// \author
//  O.C. Ozcanli - January 22, 2004
//
// \verbatim
//  Modifications
//   Ming Ching Chang :  modified to be used in shoch graph matching
// \endverbatim
//
//-------------------------------------------------------------------------

#ifndef dbasn_gradasgn_h_
#define dbasn_gradasgn_h_

#include <vcl_vector.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>
#include <dbasn/dbasn_graph.h>
#include <dbasn/dbasn_params.h>
#include <dbasn/dbasn_softasgn.h>

#define UNDEFINED_NODE_AI  (-1)

class dbasn_gradasgn
{
protected:
  dbasn_graph     *G_, *g_;
  dbasn_params    params_;

  int             M_row_;       // number of nodes in the first graph + slack
  int             M_col_;       // number of nodes in the second graph + slack    

  //: the gradAssign output.
  //  labelGg_[M_row_-1] G->nN() = M_row_-1
  //  Note that labelGg_[i] must have only one match.
  int*    labelGg_;

  //: The array is to save the node similarity cost. 
  //  No need to compute them again and again.
  ///double**  C_ai_array_; //array to cache node similarity.
  double**  M_;           //init_M
  double**  M_hat_;       //current_M (benefit matrix)
  double**  M_hat0_;

  bool  num_stable_;

  //mode of verbose debugging message.
  // 0: no output (silent).
  // 1: only the match matrix (0-100) after assignment.
  // 2: basic results, the match matrix, assignment matrix, time.
  // 3: more details: each iteration. (default)
  // 4: full necessary details.
  // 5: verbose debug messages.
  int             debug_out_;

public:
  //####### Constructor/Destructor #######
  dbasn_gradasgn () {
    M_row_        = 0;
    M_col_        = 0;
    G_            = NULL;
    g_            = NULL;

    labelGg_      = NULL;

    M_            = NULL;
    M_hat_        = NULL;
    M_hat0_       = NULL;

    num_stable_ = true;

    debug_out_    = 3;
  }
  
  virtual ~dbasn_gradasgn ();

  //####### Data access functions #######   
  const int M_col() const { 
    return M_col_; 
  }
  const int M_row() const { 
    return M_row_; 
  }

  //: Given the i-th node in G, return the matching node in g.
  const int labelGg (const unsigned int i) const {
    assert (int(i)<M_row_-1);
    return labelGg_[i];
  }

  dbasn_graph* G() {
    return G_;
  }
  dbasn_graph* g() {
    return g_;
  }
  void set_G (const dbasn_graph* G) {
    assert (G_ == NULL);
    G_ = (dbasn_graph*) G;
    G_->set_1st_graph (true);
  }
  void set_g (const dbasn_graph* g) {
    assert (g_ == NULL);
    g_ = (dbasn_graph*) g;
    g_->set_1st_graph (false);
  } 
  void set_params (const dbasn_params& params) {
    params_ = params;
  }

  void _allocate_mem ();

  void _free_mem ();

  void _reset_mem () {
    _free_mem ();

    assert (G_->n_nodes() != 0);
    M_row_ = G_->n_nodes()+1;    // with slack row
    assert (g_->n_nodes() != 0);
    M_col_ = g_->n_nodes()+1;    // with slack col

    _allocate_mem ();
  }

  const bool num_stable() const {
    return num_stable_;
  }

  void set_debug_out (const int d) {
    debug_out_ = d;
  }
  
  //####### Grad Assignment matching functions #######

  //option abs_max: true: use absolute max of G and g.
  //                false: normalize w.r.t. the max of each graph.
  virtual void normalize_costs (const bool abs_max = false);

  //: Rangarajan's graduated assignment.
  //  Return false if numerical problem occurs.
  virtual bool get_assignment ();

  //: function to test convergence.
  bool test_converge_M (double **M1, double **M2, const double& epsilon) ;
  
  double current_energy (double **M, const dbasn_graph* G, const dbasn_graph* g);

  //: function to make assignment matrix and setup labelGg_[] as the result.
  void make_assignment_matrix (double **M, double **M_clean);

  //: compute node similarity (compatibility).
  virtual double C_ai (const int a, const int i);

  //: compute link similarity (compatibility).
  virtual double C_aibj (const int a, const int b, const int i, const int j);

  //####### Query functions #######

  //: return number of wrong matching nodes.
  int compare_to_gtruth (const int* labelgG, const int n_g_nodes) const;

  void print_match_result ();

  //: Print the C_ai array to debug.
  ///void print_C_ai_array (const bool print_full = false);
  
  //: Compute the final similarity (as energy defined in the GA).
  virtual double get_similarity (const int verbose = 1);
};

//###################################################################

double inline abs_dist_wei_max (const float v1, const float v2)
{
  if (v1 == v2)
    return 0;
  else
    return vcl_fabs (v1 - v2) / vcl_max (v1, v2);
}

double inline sqr_dist_wei_max (const float v1, const float v2)
{
  if (v1 == v2)
    return 0;
  else 
    return vcl_sqrt (vcl_fabs (v1 - v2) / vcl_max (v1, v2));
}

double inline abs_dist (const float norm_v1, const float norm_v2)
{
  //norm_v1 and norm_v2 should be already normalized to beween 0 and 1.
  return vcl_fabs (norm_v1 - norm_v2);
}

double inline sqr_dist (const float norm_v1, const float norm_v2)
{
  //norm_v1 and norm_v2 should be already normalized to beween 0 and 1.
  return vcl_sqrt (vcl_fabs (norm_v1 - norm_v2));
}

double inline abs_dist_w3 (const float norm_v1, const float norm_v2)
{
  //norm_v1 and norm_v2 should be already normalized to beween 0 and 1.
  
  //The number 3 is to yield an expected value of zero when the link weights are
  //randomly selected from a uniform distribution in the intervbal [0, 1].
  //Because two points chosen from a unifom distribution in the unit interval
  //will be on average 1/3 units apart.
  return 3* vcl_fabs (norm_v1 - norm_v2);
}

#define SQRT_3 1.7320508075688772935274463415059

double inline sqr_dist_w3 (const float norm_v1, const float norm_v2)
{
  //norm_v1 and norm_v2 should be already normalized to beween 0 and 1.
  return SQRT_3 * vcl_sqrt (vcl_fabs (norm_v1 - norm_v2));
}

//###################################################################

class dbasn_gradasgn_aug : public dbasn_gradasgn
{
public:
  //###### Constructor & Destructor ######
  dbasn_gradasgn_aug () : dbasn_gradasgn () {
  }
  virtual ~dbasn_gradasgn_aug () {
  }

  //####### Data access functions ####### 
  dbasn_graph_aug* G_aug() {
    return (dbasn_graph_aug*) G_;
  }
  dbasn_graph_aug* g_aug() {
    return (dbasn_graph_aug*) g_;
  }

  //####### Grad Assignment matching functions #######
  //: compute node similarity (compatibility).
  virtual double C_ai (const int a, const int i);

  //: compute link similarity (compatibility).
  virtual double C_aibj (const int a, const int b, const int i, const int j);
};

#endif
