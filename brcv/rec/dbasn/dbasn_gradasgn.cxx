//---------------------------------------------------------------------
// This is brcv/rec/dbasn/dbasn_gradasgn.cxx
//:
// \file
// \brief class to implement graduated assignment between two graphs
//
// \author
//  O.C. Ozcanli - January 22, 2004
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vcl_algorithm.h>
#include <vcl_cfloat.h>
#include <vcl_cmath.h>
#include <vcl_ctime.h>
#include <vcl_cassert.h>
#include <vcl_sstream.h>
#include <vcl_cstdlib.h>
#include <vcl_iostream.h>
#include <vul/vul_printf.h>
#include <vnl/vnl_math.h>

#include <dbasn/dbasn_gradasgn.h>

void dbasn_gradasgn::_allocate_mem ()
{
  int i, j;

  //no need to match the slack.
  labelGg_ = new int[M_row_-1]; //M_row_-1 

  //1)The array is to cache the node similarity cost.
  /*C_ai_array_ = new double*[M_row_-1];
  for (i = 0; i<M_row_-1; i++)
    C_ai_array_[i] = new double[M_col_-1];
  for (i = 0; i<M_row_-1; i++)
    for (j = 0; j<M_col_-1; j++)
      C_ai_array_[i][j] = UNDEFINED_NODE_AI; //-1*/

  //2)Allocate the 3 M_ array of size [row][column] for the main graduated assignment loop.
  //M_ is the M_ in loop_B
  //M_hat_ is the M_hat_ in loop_C
  //M_hat0_ is the M_ we save before enter loop_C, to compare if converge or not.

  M_ = new double*[M_row_];
  M_hat_ = new double*[M_row_];
  M_hat0_ = new double*[M_row_];
  for (i = 0; i<M_row_; i++) {
    M_[i] = new double[M_col_];
    M_hat_[i] = new double[M_col_];
    M_hat0_[i] = new double[M_col_];
  }

  // initialize M_: put random numbers.
  vcl_srand ((unsigned int) vcl_time(NULL));
  int s = (M_row_>M_col_) ? M_row_ : M_col_;
  for (i = 0; i<M_row_; i++)
    for (j = 0; j<M_col_; j++) {
      //M_hat_[i][j] = 1.0 + params_.init_epsilon_;
      //M_[i][j] = 1.0 + params_.init_epsilon_;
      M_[i][j] = 1.0/s + 0.01* ( (double)vcl_rand() / (double)(RAND_MAX) );
    }
}

void dbasn_gradasgn::_free_mem ()
{
  if (labelGg_ != NULL) {
    delete labelGg_;
    labelGg_ = NULL;
  }

  //Clean up the temporary tables.
  /*if (C_ai_array_) {
    for (int i=0; i<M_row_-1; i++)
      delete []C_ai_array_[i];
    delete []C_ai_array_;
    C_ai_array_ = NULL;
  }*/

  if (M_) {
    for (int i=0; i<M_row_; i++) {
      delete []M_[i];
      delete []M_hat_[i];
      delete []M_hat0_[i];
    }
    delete []M_;
    delete []M_hat_;
    delete []M_hat0_;
    M_ = NULL;
    M_hat_ = NULL;
    M_hat0_ = NULL;
  }
  else {
    assert (M_hat_ == NULL);
    assert (M_hat0_ == NULL);
  }
}

dbasn_gradasgn::~dbasn_gradasgn ()
{
  _free_mem ();

  if (G_) {
    delete G_;
    G_ = NULL;
  }
  if (g_) {
    delete g_;
    g_ = NULL;
  }
}

//: abs_max == 1: use absolute max value of max (which is max from G and g).
//                good for matching objects where the scale matters.
//: abs_max == 0: use relative max. value of each graph of G and g.
//                good for matching objects of different scale.
void dbasn_gradasgn::normalize_costs (const bool abs_max)
{
  //Normalize the node cost to be within [0 ~ 1] by the max value.
  float G_n_max, G_n_min;
  G_->get_node_cost_max_min (G_n_max, G_n_min);
  float g_n_max, g_n_min;
  g_->get_node_cost_max_min (g_n_max, g_n_min);
  float G_l_max, G_l_min;
  G_->get_link_cost_max_min (G_l_max, G_l_min);
  float g_l_max, g_l_min;
  g_->get_link_cost_max_min (g_l_max, g_l_min);

  if (abs_max) {
    float n_max = vcl_max (G_n_max, g_n_max);
    float l_max = vcl_max (G_l_max, g_l_max);
    float n_min = vcl_max (G_n_min, g_n_min);
    float l_min = vcl_max (G_l_min, g_l_min);
    G_->normalize_node_cost (n_max, n_min, debug_out_);
    g_->normalize_node_cost (n_max, n_min, debug_out_);
    G_->normalize_link_cost (l_max, l_min, debug_out_);
    g_->normalize_link_cost (l_max, l_min, debug_out_);
  }
  else {
    G_->normalize_node_cost (G_n_max, G_n_min, debug_out_);
    g_->normalize_node_cost (g_n_max, g_n_min, debug_out_);
    G_->normalize_link_cost (G_l_max, G_l_min, debug_out_);
    g_->normalize_link_cost (g_l_max, g_l_min, debug_out_);
  }
}

bool dbasn_gradasgn::get_assignment ()
{
  vcl_clock_t t1 = vcl_clock();

  float T = params_.T0_;
  int iter_a = 0;
  while (T >= params_.Tf_) { // loop A 
    if (debug_out_>2)
      vul_printf (vcl_cout, "\n%2d T: %1.3lf ", iter_a, T);
    int iter_b = 0;

    while (iter_b <= params_.I0_) { // loop B: iter_b <= I0_  
      if (debug_out_>2)
        vul_printf (vcl_cout, "  <b%d>", iter_b);
      for (int a = 0; a< M_row_-1; a++)
        for (int i = 0; i<M_col_-1; i++) {
          float Qai = 0.0;
          float Qai_link = 0.0;
          float Qai_node = 0.0;
          
          //NODE COMPARISON
          #if GA_COMPARE_NODE > 0
          Qai_node = float (C_ai (a, i));
          #endif

          //LINK COMPARISON
          //Go through all the links of a and all the links of i.
          #if GA_COMPARE_LINK > 0
          int b_size = G_->nodes(a)->n_conn_nids ();
          int j_size = g_->nodes(i)->n_conn_nids ();
        
          for (int b = 0; b<b_size; b++) {
            int b_ind = G_->nodes(a)->conn_nids (b);
            for (int j = 0; j<j_size; j++) {
              int j_ind = g_->nodes(i)->conn_nids (j);
              double Caibj = C_aibj (a, b_ind, i, j_ind);
              Qai_link += float (M_[b_ind][j_ind] * Caibj);
            }
          }
          #endif
          
          Qai = Qai_node * params_.wN_ + Qai_link * params_.wL_;
          M_hat_[a][i] = Qai;         
        }

      //Initialize M_hat_'s slack variables to 0
      for (int a = 0; a<M_row_; a++)
        M_hat_[a][M_col_-1] = 0;
      for (int i = 0; i<M_col_; i++)
        M_hat_[M_row_-1][i] = 0;
      //debug: print_M (M_hat_, M_row_, M_col_);
      
      //Run softassign.
      dbasn_softasgn sa (M_row_, M_col_);
      num_stable_ = sa.run_assign (M_hat_, M_hat_, T, params_.Is_, params_.eS_); 
      if (num_stable_ == false) { //exp. explosion.
        if (debug_out_)          
          vcl_cout << "\n\n\n\t\t  EXP. EXPLOSION in softasgn!\n\n\n";
        goto GRAD_ASGN_FINISH; 
      }

      //debug: vcl_cout << "\t returned assignment matrix at temperature " << T << " is:\n";
      //debug: print_M (M_hat_, M_row_, M_col_);      
      //debug: print energy for the current assignment matrix 
      ///vcl_cout << "\t\t" << current_energy(M_, G_, g_) << "\n";

      if (test_converge_M (M_hat_, M_, params_.eB_)) { 
        copy_M (M_, M_hat_, M_row_, M_col_);
        break;
      }

      copy_M (M_, M_hat_, M_row_, M_col_);      
      iter_b++;
    }//end loop B
    //debug: print_M (M_, M_row_, M_col_);
    T = params_.Tr_*T;
    iter_a++;
  }//end loop A

GRAD_ASGN_FINISH:
  if (debug_out_>1)
    vcl_cout << "\nTotal iteration A: " << iter_a << vcl_endl;
  //M_ is the result, put clean one in M_hat_
  make_assignment_matrix (M_, M_hat_);
  
  vcl_clock_t t2 = vcl_clock();

  if (debug_out_>0)
    print_M (M_, M_row_, M_col_);  
  if (debug_out_>1) {
    print_M_bin (M_hat_, M_row_, M_col_);
    print_match_result ();
    vcl_cout << "\n ==> GAassign requires " << (double)(t2-t1) / CLOCKS_PER_SEC << " seconds.\n";
  }
  return num_stable_;
}

//: function to test convergence.
bool dbasn_gradasgn::test_converge_M (double **M1, double **M2, const double& epsilon) 
{
  int i, j;

  double sum = 0.0; 
  for (i = 0; i<M_row_-1; i++)
    for (j = 0; j<M_col_-1; j++) {
      if (M1[i][j] > M2[i][j]) 
        sum += M1[i][j] - M2[i][j];
      else 
        sum += M2[i][j] - M1[i][j];
    }

  if (sum < epsilon) 
    return true;
  else 
    return false;
}

double dbasn_gradasgn::current_energy (double **M, const dbasn_graph* G, const dbasn_graph* g)
{
  int i, a;

  double energy = 0.0;
  //: first find link energy
  for (a = 0; a< M_row_-1; a++)
    for (i = 0; i<M_col_-1; i++) {
      int b_size = G->nodes(a)->n_conn_nids();
      int j_size = g->nodes(i)->n_conn_nids();
          
      for (int b_idx = 0; b_idx<b_size; b_idx++) { 
        int b = G->nodes(a)->conn_nids(b_idx);
        for (int j_idx = 0; j_idx<j_size; j_idx++) {
          int j = g->nodes(i)->conn_nids(j_idx);
          energy += M_[a][i] * M_[b][j] * C_aibj (a, b, i, j);
        }
      }
    }

  energy = -0.5*energy;
  //: find node energy
  for (a = 0; a< M_row_-1; a++)
    for (i = 0; i<M_col_-1; i++) 
        energy -= params_.wN_ * M_[a][i] * C_ai (a, i);

  return energy;
}

//: function to make assignment matrix and setup labelGg_[] as the result.
void dbasn_gradasgn::make_assignment_matrix (double **M, double **M_clean) 
{
  int i, j, m_ind;
  double max;

  //: initialize the clean matrix[][] to be 0
  for (i=0; i<M_row_; i++)
    for (j=0; j<M_col_; j++)
      M_clean[i][j] = 0;

  //: initialize the matching array labelGg_[M_row_-1] to be -1
  //  This implies that the unmatched node is represented by index -1.
  for (i=0; i<M_row_-1; i++) {
    labelGg_[i] = -1;
  }

  //: turn strictly row_dominant into binary
  //  assume M_row_ > M_col_ 
  //100
  //000
  //001
  //000
  //010
  ///assert (M_row_ >= M_col_);

  for (i=0; i<M_row_-1; i++) {
    max = M[i][0];
    m_ind = 0;
    for (j = 0; j<M_col_; j++)
      if (M[i][j] > max) {
        max = M[i][j];
        m_ind = j;
      }
    //: now m_ind is the max one in the i-th row.
    M_clean[i][m_ind] = 1;

    //: make the slack matching to be -1, not the size()
    if (m_ind == M_col_-1)
      m_ind = -1;

    //labelGg_: match the i-th node in G to the one in g.
    labelGg_[i] = m_ind;
  }
}

//=====================================================================
//The definition of compatibility between nodes.

//: return value = 1 if G[a] and g[i] are perfectly matched
//                 0 if G[a] and g[i] are totally different.
// Assume G_a between 0 and 1
// Assume g_i between 0 and 1
double dbasn_gradasgn::C_ai (const int a, const int i)
{
  ///if (C_ai_array_[a][i] != UNDEFINED_NODE_AI)
    ///return C_ai_array_[a][i]; //Use precomputed cached value.

  //The number 3 is to yield an expected value of zero when the link weights are
  //randomly selected from a uniform distribution in the intervbal [0, 1].
  //Because two points chosen from a unifom distribution in the unit interval
  //will be on average 1/3 units apart.
  return 1 - vcl_fabs (G_->nodes(a)->cost() - g_->nodes(i)->cost()) * 3;
}

//=====================================================================
//The definition of compatibility between links.

// 1: pefect match
// 0: worst match
double dbasn_gradasgn::C_aibj (const int a, const int b, const int i, const int j)
{
  //if no such link exists, just cost=0;
  double cost_ab = G_->links (a, b);
  if (cost_ab==0)
    return 0;
  double cost_ij = g_->links (i, j);
  if (cost_ij==0)
    return 0;

  //The number 3 is to yield an expected value of zero when the link weights are
  //randomly selected from a uniform distribution in the intervbal [0, 1].
  //Because two points chosen from a unifom distribution in the unit interval
  //will be on average 1/3 units apart.
  double compatibility = 1 - vcl_fabs (cost_ab - cost_ij) * 3;

  if (vnl_math::isnan (compatibility)) {
    vcl_cout<< "Error: C_aibj NaN! ";
    assert (0);
  }

  return compatibility;
}

//: return number of wrong matching nodes.
int dbasn_gradasgn::compare_to_gtruth (const int* labelgG, const int n_g_nodes) const
{
  assert (n_g_nodes == M_col_-1);
  int bad = 0;
  for (int i = 0; i<n_g_nodes; i++) {
    if (i != labelGg(labelgG[i])) 
      bad++;
  }
  return bad;
}

void dbasn_gradasgn::print_match_result ()
{
  vcl_cout<< "GA matching result (G <-> g): \n";
  //The final output, labelGg_p[M_col_-1]
  for (int i=0; i<M_row_-1; i++) {
    vcl_cout << i <<"<->"<< labelGg_[i] << vcl_endl;
  }
}

/*void dbasn_gradasgn::print_C_ai_array (const bool print_full)
{
  vcl_cout<<vcl_endl << "N_ai_array: "<< vcl_endl;
  for (int a = 0; a<M_row_-1; a++)
    for (int i = 0; i<M_col_-1; i++)
      if (print_full || C_ai_array_[a][i] != 0)
        vcl_cout<< a <<" "<< i <<" "<< C_ai_array_[a][i] <<vcl_endl;
}*/

//: Compute the final similarity (as energy defined in the GA).
double dbasn_gradasgn::get_similarity (const int verbose)
{
  assert (0);
  return 0;
}

//###################################################################

//: compute node similarity (compatibility).
double dbasn_gradasgn_aug::C_ai (const int a, const int i)
{
  ///if (C_ai_array_[a][i] != UNDEFINED_NODE_AI)
    ///return C_ai_array_[a][i]; //Use precomputed cached value.

  double compatibility = dbasn_gradasgn::C_ai (a, i);

  dbasn_node_aug* Na = (dbasn_node_aug*) G_->nodes (a);
  dbasn_node_aug* Ni = (dbasn_node_aug*) g_->nodes (i);

  AN_TYPE Na_type = Na->type();
  AN_TYPE Ni_type = Ni->type();

  if (Na->type() == AN_TYPE_RIB_END || Na->type() == AN_TYPE_DEGE_RIB_END) {
    if (Ni->type() != AN_TYPE_RIB_END && Ni->type() != AN_TYPE_DEGE_RIB_END) {
      ///C_ai_array_[a][i] = 0;
      return 0;
    }
  }
  else if (Na->type() == AN_TYPE_AXIAL_END || Na->type() == AN_TYPE_DEGE_AXIAL_END) {
    if (Ni->type() != AN_TYPE_AXIAL_END && Ni->type() != AN_TYPE_DEGE_AXIAL_END) {
      ///C_ai_array_[a][i] = 0;
      return 0;
    }
  }
  else if (Na->type() == AN_TYPE_LOOP_END) {
    if (Ni->type() != AN_TYPE_LOOP_END) {
      ///C_ai_array_[a][i] = 0;
      return 0;
    }
  }
  else {
    assert (0);
  }

  //Count the total number of diff. #A_3, #A13, and #A1n in Na and Ni.
  double D = vcl_abs (Na->n_rib() - Ni->n_rib()) +
             vcl_abs (Na->n_axial() - Ni->n_axial()) +
             vcl_abs (Na->n_dege() - Ni->n_dege());

  compatibility -= 0.1 * D;
  return compatibility;
}

//: compute link similarity considering the type compatibility.
double dbasn_gradasgn_aug::C_aibj (const int a, const int b, const int i, const int j)
{
  //get the cost without considering the augmented type info.
  double compatibility = dbasn_gradasgn::C_aibj (a, b, i, j);

  //If the two types are different, cost = 0.
  AL_TYPE Lab_type = G_aug()->link_type (a, b);
  AL_TYPE Lij_type = g_aug()->link_type (i, j);

  if (Lab_type == AL_TYPE_A3_RIB) {
    if (Lij_type != AL_TYPE_A3_RIB)
      return 0;
  }
  else {
    assert (Lab_type == AL_TYPE_A13_AXIAL || Lab_type == AL_TYPE_DEGE_AXIAL);
    if (Lij_type != AL_TYPE_A13_AXIAL && Lab_type != AL_TYPE_DEGE_AXIAL)
      return 0;
  }

  //If the two nodes' types are different, cost = 0.
  //Should reduce the cost by, say half.

  return compatibility;
}



