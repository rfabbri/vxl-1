//---------------------------------------------------------------------
// This is brcv/rec/dbasnh/dbasnh_gradasgn.cxx
//:
// \file
// \brief Implementation of graduated assignment for hypergraphs.
//
// \author
//  Ming-Ching Chang - January 6, 2008.
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vcl_algorithm.h>
#include <vcl_ctime.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vcl_cfloat.h>
#include <vcl_iostream.h>
#include <vgl/vgl_distance.h>
#include <vnl/vnl_math.h>
#include <vul/vul_printf.h>

#include <dbasn/dbasnh_gradasgn.h>
#include <dbgl/dbgl_rigid_align.h>

//: the order of a,b,c and i,j,k are End-Mid-End
//  i.e., b and j are the Corner vertex. Corner[a][b][c], Corner[i][j][k]
//  Note: LINKS ARE UN-DIRECTED (SYMMETRIC IN THE MATRIX) HERE.
//
double dbasnh_gradasgn::C_aibjck (const int a, const int b, const int c,
                                  const int i, const int j, const int k)
{
  if (G_->links(a,b)==0) {
    assert (G_->links(b,a)==0);
    return 0;
  }
  if (G_->links(b,c)==0) {
    assert (G_->links(c,b)==0);
    return 0;
  }
  if (g_->links(i,j)==0) {
    assert (g_->links(j,i)==0);
    return 0;
  }
  if (g_->links(j,k)==0) {
    assert (g_->links(k,j)==0);
    return 0;
  }

  double angle_abc = hypg_G()->corners (a, b, c);
  double angle_ijk = hypg_g()->corners (i, j, k);

  //The number 3 is to yield an expected value of zero when the link weights are
  //randomly selected from a uniform distribution in the intervbal [0, 1].
  //Because two points chosen from a unifom distribution in the unit interval
  //will be on average 1/3 units apart.
  //The corner[][][] should be very sparse.
  double compatibility = 1 - vcl_fabs (angle_abc - angle_ijk) * 3;
  ///if (vnl_math_isnan (compatibility)) {
    ///vcl_cout<< "Error: C_aibjck NaN! ";
    ///assert (0);
  ///}

  return compatibility;
}

//: abs_max == 1: use absolute max value of max (which is max from G and g).
//                good for matching objects where the scale matters.
//: abs_max == 0: use relative max. value of each graph of G and g.
//                good for matching objects of different scale.
void dbasnh_gradasgn::normalize_costs (const bool abs_max)
{
  dbasn_gradasgn::normalize_costs (abs_max);
  double max;

  if (abs_max) {
    max = vnl_math_max (hypg_G()->corner_cost_max(), hypg_g()->corner_cost_max());
    hypg_G()->normalize_corner_cost (max, debug_out_>2);
    hypg_g()->normalize_corner_cost (max, debug_out_>2);
  }
  else {
    hypg_G()->normalize_corner_cost (hypg_G()->corner_cost_max(), debug_out_>2);
    hypg_g()->normalize_corner_cost (hypg_g()->corner_cost_max(), debug_out_>2);  
  }
}

//=====================================================================
//Very similar to the original dbasn_gradasgn, just add additional terms
bool dbasnh_gradasgn::get_assignment () 
{
  vcl_clock_t t1 = vcl_clock();

  //The main loop of graduated assignment.
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
          float Qai_node = 0.0;
          float Qai_link = 0.0;
          float Qai_sheet = 0.0;
              
          //NODE COMPARISON
          #if GA_COMPARE_NODE > 0
          Qai_node = float (C_ai (a, i));
          #endif
          
          //LINK COMPARISON (DIRECTIONAL)
          //Go through all the links of a and all the links of i.
          #if GA_COMPARE_LINK > 0
          for (int b_ind=0; b_ind<G_->nodes(a)->n_conn_nids(); b_ind++) {
            int b = G_->nodes(a)->conn_nids (b_ind);
            for (int j_ind=0; j_ind<g_->nodes(i)->n_conn_nids(); j_ind++) {
              int j = g_->nodes(i)->conn_nids (j_ind);
              double Caibj = C_aibj (a, b, i, j);
              Qai_link += float (M_[b][j] * Caibj);

              //CORNER (LINK_LINK) COMPARISON (UN_DIRECTIONAL)
              //look for all possible sheet corner assocated with G_[Na] and g_[Ni]
              //Here we have a-b match to i-j, consider c-k at the corner b-a-c to j-i-k
              double qai_sheet = 0;
              #if GA_COMPARE_CORNER > 0
              //b-a-c_ind <-> j-i-k
              for (int c_ind=0; c_ind<G_->nodes(a)->n_conn_nids(); c_ind++) {
                int c = G_->nodes(a)->conn_nids (c_ind);
                if (c ==b)
                  continue;
                for (int k_ind=0; k_ind<g_->nodes(i)->n_conn_nids(); k_ind++) {
                  int k = g_->nodes(i)->conn_nids (k_ind);
                  if (k ==j)
                    continue;
                  qai_sheet += M_[c][k] * C_aibjck (b, a, c, j, i, k);
                }
              }

              Qai_sheet += float (qai_sheet * M_[b][j]);
              #endif
            }//end for j
          }//end for b
          #endif

          // forget 0.5 here. just way to define energy and alpha

          //: Note: for the trivial matching example, 
          //  Qai_node itself is sufficient,
          //  Qai_link itself is also sufficient,
          //  Problem in Qai_sheet.
          ///Qai = Qai_node + Qai_link + Qai_sheet;
          Qai = Qai_node * params_.wN_ + Qai_link * params_.wL_ + Qai_sheet * params_.wC_; 
          M_hat_[a][i] = Qai;
        }//end i

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
    vcl_cout << "\n ==> GA assign requires " << (double)(t2-t1) / CLOCKS_PER_SEC << " seconds.\n";
  }

  return num_stable_;
}

//: Compute the final similarity (as energy defined in the GA).
double dbasnh_gradasgn::get_similarity (const int verbose)
{
  assert (0);
  return 0;
}

//###################################################################

double dbasnh_gradasgn_aug::get_norm_row_dist (vcl_vector<vcl_vector<double> >& table,
                                               const int r, const int c)
{
  //Get the specified row from the table.
  vcl_vector<double> row;
  row.insert (row.begin(), table[r].begin(), table[r].end());

  //Compute the max of row to normalize the value of table[r][c].
  double min = DBL_MAX;
  double max = 0;
  for (unsigned int i=0; i<row.size(); i++) {
    //Skip the huge value of diff. curve type.
    if (row[i] == DP_DIST_HUGE)
      continue;
    if (min > row[i])
      min = row[i];
    if (max < row[i])
      max = row[i];
  }

  //Normalization (w.r.t row-max and row-min).
  if (max < 1E-7) {    
    return 0; //set distance = 0
  }
  else {
    return row[c] / max;
  }

    ///return (row[c] - min) / (max - min); <-- no sq dist <--wrong: forcing min_cost to 0 cost!!
    ///return vcl_sqrt ((row[c] - min) / (max - min)); <--wrong: forcing min_cost to 0 cost!!

  ///Method 2: Normalization (w.r.t row-max only).
  ///if (max == min)
    ///return row[c] - min;
  ///else
    ///return (row[c]) / (max);
}

//: Check if the two nodes (a,i)'s are the same type. 
bool dbasnh_gradasgn_aug::same_N_type (const int a, const int i)
{  
  dbasn_node_aug* Na = (dbasn_node_aug*) G_->nodes (a);
  dbasn_node_aug* Ni = (dbasn_node_aug*) g_->nodes (i);
  return is_same_n_type (Na->type(), Ni->type());
}

double dbasnh_gradasgn_aug::get_N_type_diff (const int a, const int i)
{
  dbasn_node_aug* Na = (dbasn_node_aug*) G_->nodes (a);
  dbasn_node_aug* Ni = (dbasn_node_aug*) g_->nodes (i);

  //For the most cases of (Na, Ni) of type A1A3 or A14, return the result for better performance.
  if (Na->type() == AN_TYPE_RIB_END) { //Na A1A3
    if (Ni->type() == AN_TYPE_RIB_END)
      return 0;
    else if (Ni->type() == AN_TYPE_AXIAL_END)
      return 0.75f;
  }
  else if (Na->type() == AN_TYPE_AXIAL_END) { //Na A14
    if (Ni->type() == AN_TYPE_RIB_END)
      return 0.75f;
    else if (Ni->type() == AN_TYPE_AXIAL_END)
      return 0;
  }

  int Na_m, Na_n, Na_f;
  bool result_a = dbash_get_canonical_type (Na->n_rib(), Na->n_axial(), Na->n_dege(), Na_m, Na_n, Na_f);
  int Ni_m, Ni_n, Ni_f;
  bool result_b = dbash_get_canonical_type (Ni->n_rib(), Ni->n_axial(), Ni->n_dege(), Ni_m, Ni_n, Ni_f);

  if (result_a == false || result_b == false) {
    //Both nodes are non-canonical type (very degenerate). Return 0 distance, since they are likely to match well!
    return 0;
  }
  else {
    double diff = dbash_comp_node_type_diff (Na_m, Na_n, Na_f, Ni_m, Ni_n, Ni_f);
    //Use sqrare root distance.
    return vcl_sqrt (diff);
  }
}

//: compute node similarity (compatibility).
//  duplicating from dbasn_gradasgn_aug::C_ai ().
double dbasnh_gradasgn_aug::C_ai (const int a, const int i)
{
  ///if (C_ai_array_[a][i] != UNDEFINED_NODE_AI)
    ///return C_ai_array_[a][i]; //Use precomputed cached value.

  dbasn_node_aug* Na = (dbasn_node_aug*) G_->nodes (a);
  dbasn_node_aug* Ni = (dbasn_node_aug*) g_->nodes (i);

  double compatibility;  
  if (use_R_T_) {
    //Transform N(a) using the current (R, T) to match N(i)
    vnl_vector_fixed<double,3> NaP (Na->pt().x(), Na->pt().y(), Na->pt().z());
    NaP = R_ * NaP + T_;
    vgl_point_3d<double> NaPt (NaP[0], NaP[1], NaP[2]);
    double dist = vgl_distance (NaPt, Ni->pt());
    compatibility = 1 - abs_dist (Na->cost(), Ni->cost()) * 0.5 - dist / max_len_btwn_nodes_ * 0.5;
  }
  else {
    ///compatibility = 1 - sqr_dist (Na->cost(), Ni->cost()); //sqr_dist_wei_max
    compatibility = 1 - 0.5 * sqr_dist (Na->cost(), Ni->cost())
                      - 0.125 * sqr_dist (Na->grad_r_max (), Ni->grad_r_max())
                      - 0.125 * sqr_dist (Na->grad_r_min (), Ni->grad_r_min())
                      - 0.125 * sqr_dist (Na->corner_a_max (), Ni->corner_a_max())
                      - 0.125 * sqr_dist (Na->corner_a_min (), Ni->corner_a_min()); //sqr_dist_wei_max
  }


  //Final compatibility = str_comp * param_comp.
  double str = C_ai_str (a, i); //Structural similarity
  compatibility *= str;

  ///C_ai_array_[a][i] = compatibility;
  return compatibility; ///C_ai_array_[a][i];
}

double dbasnh_gradasgn_aug::C_ai_str (const int a, const int i)
{
  //node structural similarity.
  double N_type_diff = get_N_type_diff (a, i);
  return 1- N_type_diff;
}

//: Check if the two links (ab, ij)'s are the same type. 
bool dbasnh_gradasgn_aug::same_L_type (const int a, const int b, const int i, const int j)
{
  AL_TYPE Lab_type = hypg_G_aug()->link_type (a, b);
  AL_TYPE Lij_type = hypg_g_aug()->link_type (i, j);

  return is_same_c_type (Lab_type, Lij_type);
}

//: compute link similarity considering the type compatibility.
//  duplicating from dbasn_gradasgn_aug::C_aibj ().
double dbasnh_gradasgn_aug::C_aibj (const int a, const int b, const int i, const int j)
{
  //curve structural similarity
  if (C_aibj_str_valid (a, b, i, j) == false)
    return 0;
  
  double compatibility;
  double cost_ab = G_->links (a, b);
  double cost_ij = g_->links (i, j);

  if (use_DP_cost_ == false) {
    //curve length as compatibility.
    compatibility = 1 - sqr_dist (cost_ab, cost_ij); //sqr_dist_wei_max
    
    //Weighted by the orientation as radius (Na to Ni), (Nb to Nj).
    //Not much difference!
    //compatibility = 0.8 * compatibility + 0.1 * ( C_ai (a, i) + C_ai (b, j) );
  }
  else {
    //The elastic compatibility measurement:
    bool Gflip, gflip;
    int m = hypg_G_aug()->get_link_idx (a, b, Gflip);
    int n = hypg_g_aug()->get_link_idx (i, j, gflip);

    //Compute the normalized oriented row-distance from Gg_ed_dist_[m][n].
    double ed, Eu;
    if ((!Gflip && !gflip) || (Gflip && gflip)) {
      ed = get_norm_row_dist (Gg_ed_dist_, m, n);
      Eu = get_norm_row_dist (Gg_Eu_dist_, m, n);
    }
    else {
      ed = get_norm_row_dist (Gg_ed_dist_f_, m, n);  
      Eu = get_norm_row_dist (Gg_Eu_dist_f_, m, n);
    }

    double ed_sq = vcl_sqrt (ed);
    double Eu_sq = vcl_sqrt (Eu);
    
    //!!Important Parameters!!
    //Here we should emphasize the Eu & ed distance and abandon the curve lenth similarity!!
    ///compatibility = 1 - 0.33 * ed - 0.33 * Eu - 0.34 * sqr_dist (cost_ab, cost_ij); //sqr_dist_wei_max
    compatibility = 1 - 0.3 * ed_sq - 0.7 * Eu_sq;
  }

  //Final compatibility = str_comp * param_comp.
  double str = C_aibj_str (a, b, i, j); //Structural similarity
  compatibility *= str;

  return compatibility;
}

//Curve structural similarity.
bool dbasnh_gradasgn_aug::C_aibj_str_valid (const int a, const int b, const int i, const int j)
{
  //if no such link exists, just cost=0;
  if (G_->links (a, b)==0)
    return false;
  if (g_->links (i, j)==0)
    return false;

  //If the two link types are different, cost = 0.
  if (same_L_type (a, b, i, j) == false)
    return false;

  return true;
}

double dbasnh_gradasgn_aug::C_aibj_str (const int a, const int b, const int i, const int j)
{
  //!!Important Parameter!!
  //Penalize if ending node type is different:
  //Reduce the strurcutal compatibility by checking the n type compatibility.    
  double Nai_type_diff = get_N_type_diff (a, i);
  double Nbj_type_diff = get_N_type_diff (b, j);
  return 1 - 0.1 * Nai_type_diff - 0.1 * Nbj_type_diff;
}

//: duplicating from dbasnh_gradasgn::C_aibjck ().
double dbasnh_gradasgn_aug::C_aibjck (const int a, const int b, const int c, 
                                      const int i, const int j, const int k)
{ 
  //corner structural similarity
  if (C_aibjck_str_valid (a, b, c, i, j, k) == false)
    return 0;

  double angle_abc = hypg_G()->corners (a, b, c);
  double angle_ijk = hypg_g()->corners (i, j, k);
  double nf_abc = hypg_G_aug()->corner_nf (a, b, c);
  double nf_ijk = hypg_g_aug()->corner_nf (i, j, k);
  double grad_r_abc = hypg_G_aug()->corner_grad_r (a, b, c);
  double grad_r_ijk = hypg_g_aug()->corner_grad_r (i, j, k);

  //radius at the node.  
  dbasn_node_aug* Na = (dbasn_node_aug*) G_->nodes (a);
  dbasn_node_aug* Ni = (dbasn_node_aug*) g_->nodes (i);

  // !! Important parameter here!!
  ///double compatibility = 1 - 0.2 * sqr_dist (nf_abc, nf_ijk)
                           ///- 0.3 * sqr_dist (angle_abc, angle_ijk)
                           ///- 0.3 * sqr_dist (Na->cost(), Ni->cost())
                           ///- 0.2 * sqr_dist (grad_r_abc, grad_r_ijk); 
  double compatibility = 1 - 0.6 * sqr_dist (nf_abc, nf_ijk)
                           - 0.2 * sqr_dist (angle_abc, angle_ijk)
                           - 0.1 * sqr_dist (Na->cost(), Ni->cost())
                           - 0.1 * sqr_dist (grad_r_abc, grad_r_ijk); 

  //sqr_dist_wei_max
  ///double compatibility = 1 - vcl_fabs (nf_abc - nf_ijk) - vcl_fabs (angle_abc - angle_ijk) * 2;
  //Orientation as radius (Na to Ni), (Nb to Nj).
  //No much difference!
  //compatibility = 0.7 * compatibility + 0.1 * ( C_ai (a, i) + C_ai (b, j) + C_ai (c, k));

  //Final compatibility = str_comp * param_comp.
  double str = C_aibjck_str (a, b, c, i, j, k); //Structural similarity
  compatibility *= str;
    
  return compatibility;
}

//: corner structural similarity.
bool dbasnh_gradasgn_aug::C_aibjck_str_valid (const int a, const int b, const int c, 
                                              const int i, const int j, const int k)
{
  if (G_->links(a,b)==0) {
    assert (G_->links(b,a)==0);
    return false;
  }
  if (G_->links(b,c)==0) {
    assert (G_->links(c,b)==0);
    return false;
  }
  if (g_->links(i,j)==0) {
    assert (g_->links(j,i)==0);
    return false;
  }
  if (g_->links(j,k)==0) {
    assert (g_->links(k,j)==0);
    return false;
  }

  //If the two links (ab, ij)'s types are different, cost = 0.
  if (same_L_type (a, b, i, j) == false)
    return false;
  //If the two links (bc, jk)'s types are different, cost = 0.
  if (same_L_type (b, c, j, k) == false)
    return false;
  
  return true;
}

//: corner structural similarity.
double dbasnh_gradasgn_aug::C_aibjck_str (const int a, const int b, const int c, 
                                          const int i, const int j, const int k)
{  
  //Penalize if ending node type is different:
  //Reduce the strurcutal compatibility by checking the n type compatibility.    
  double Nai_type_diff = get_N_type_diff (a, i);
  double Nbj_type_diff = get_N_type_diff (b, j);
  double Nck_type_diff = get_N_type_diff (c, k);

  return 1 - 0.8 * Nbj_type_diff - 0.1 * Nai_type_diff - 0.1 * Nck_type_diff;
}

//: abs_max == 1: use absolute max value of max (which is max from G and g).
//                good for matching objects where the scale matters.
//: abs_max == 0: use relative max. value of each graph of G and g.
//                good for matching objects of different scale.
void dbasnh_gradasgn_aug::normalize_costs (const bool abs_max)
{
  dbasnh_gradasgn::normalize_costs (abs_max);
  double max;

  if (abs_max) {
    max = vnl_math_max (hypg_G_aug()->max_of_grad_r_max(), hypg_g_aug()->max_of_grad_r_max());
    hypg_G_aug()->normalize_node_grad_r_max (max, debug_out_);
    hypg_g_aug()->normalize_node_grad_r_max (max, debug_out_);

    max = vnl_math_max (hypg_G_aug()->max_of_grad_r_min(), hypg_g_aug()->max_of_grad_r_min());
    hypg_G_aug()->normalize_node_grad_r_min (max, debug_out_);
    hypg_g_aug()->normalize_node_grad_r_min (max, debug_out_);

    max = vnl_math_max (hypg_G_aug()->max_of_corner_a_max(), hypg_g_aug()->max_of_corner_a_max());
    hypg_G_aug()->normalize_node_corner_a_max (max, debug_out_);
    hypg_g_aug()->normalize_node_corner_a_max (max, debug_out_);

    max = vnl_math_max (hypg_G_aug()->max_of_corner_a_min(), hypg_g_aug()->max_of_corner_a_min());
    hypg_G_aug()->normalize_node_corner_a_min (max, debug_out_);
    hypg_g_aug()->normalize_node_corner_a_min (max, debug_out_);

    max = vnl_math_max (hypg_G_aug()->corner_nf_max(), hypg_g_aug()->corner_nf_max());
    hypg_G_aug()->normalize_corner_nf_cost (max, debug_out_);
    hypg_g_aug()->normalize_corner_nf_cost (max, debug_out_);

    max = vnl_math_max (hypg_G_aug()->corner_grad_r_max(), hypg_g_aug()->corner_grad_r_max());
    hypg_G_aug()->normalize_corner_grad_r_cost (max, debug_out_);
    hypg_g_aug()->normalize_corner_grad_r_cost (max, debug_out_);
  }
  else {
    hypg_G_aug()->normalize_node_grad_r_max (hypg_G_aug()->max_of_grad_r_max(), debug_out_);
    hypg_g_aug()->normalize_node_grad_r_max (hypg_g_aug()->max_of_grad_r_max(), debug_out_);

    hypg_G_aug()->normalize_node_grad_r_min (hypg_G_aug()->max_of_grad_r_min(), debug_out_);
    hypg_g_aug()->normalize_node_grad_r_min (hypg_g_aug()->max_of_grad_r_min(), debug_out_);

    hypg_G_aug()->normalize_node_corner_a_max (hypg_G_aug()->max_of_corner_a_max(), debug_out_);
    hypg_g_aug()->normalize_node_corner_a_max (hypg_g_aug()->max_of_corner_a_max(), debug_out_);

    hypg_G_aug()->normalize_node_corner_a_min (hypg_G_aug()->max_of_corner_a_min(), debug_out_);
    hypg_g_aug()->normalize_node_corner_a_min (hypg_g_aug()->max_of_corner_a_min(), debug_out_);

    hypg_G_aug()->normalize_corner_nf_cost (hypg_G_aug()->corner_nf_max(), debug_out_);
    hypg_g_aug()->normalize_corner_nf_cost (hypg_g_aug()->corner_nf_max(), debug_out_); 

    hypg_G_aug()->normalize_corner_grad_r_cost (hypg_G_aug()->corner_grad_r_max(), debug_out_);
    hypg_g_aug()->normalize_corner_grad_r_cost (hypg_g_aug()->corner_grad_r_max(), debug_out_);    
  }
  ///vcl_cout << vcl_endl;
}

void dbasnh_gradasgn_aug::init_R_T ()
{
  //Initialize R: sample the R space and compute the best R.
  
  //Initialize T as the centroid (g_i) - centroid (G_a)
  vgl_point_3d<double> CG, Cg;
  hypg_G_aug()->get_cen_N_aug (CG);
  hypg_g_aug()->get_cen_N_aug (Cg);

  T_ = vnl_vector_fixed<double, 3> (Cg.x(), Cg.y(), Cg.z()) - 
       vnl_vector_fixed<double, 3> (CG.x(), CG.y(), CG.z());
}

void dbasnh_gradasgn_aug::get_max_len_between_nodes ()
{
  //compute the max dist between all pairs of (G[a], G[b]) and (g[i], g[j]).
  double G_max_r = hypg_G_aug()->bound_box_radius_of_nodes ();
  double g_max_r = hypg_g_aug()->bound_box_radius_of_nodes ();
  max_len_btwn_nodes_ = vnl_math_max (G_max_r, g_max_r);
  vul_printf (vcl_cout, "  get_max_len_between_nodes: %f.\n", max_len_btwn_nodes_);
}

//: An extended version of get_assignment() to updating (R, T) in the iterative process.
bool dbasnh_gradasgn_aug::get_assignment ()
{
  vcl_clock_t t1 = vcl_clock();

  //The main loop of graduated assignment.
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
          float Qai_node = 0.0;
          float Qai_link = 0.0;
          float Qai_sheet = 0.0;
              
          //NODE COMPARISON
          #if GA_COMPARE_NODE > 0
          Qai_node = float (C_ai (a, i));
          #endif
          
          //LINK COMPARISON (DIRECTIONAL)
          //Go through all the links of a and all the links of i.
          #if GA_COMPARE_LINK > 0
          for (int b_ind=0; b_ind<G_->nodes(a)->n_conn_nids(); b_ind++) {
            int b = G_->nodes(a)->conn_nids (b_ind);
            for (int j_ind=0; j_ind<g_->nodes(i)->n_conn_nids(); j_ind++) {
              int j = g_->nodes(i)->conn_nids (j_ind);
              double Caibj = C_aibj (a, b, i, j);
              Qai_link += float (M_[b][j] * Caibj);

              //CORNER (LINK_LINK) COMPARISON (UN_DIRECTIONAL)
              //look for all possible sheet corner assocated with G_[Na] and g_[Ni]
              //Here we have a-b match to i-j, consider c-k at the corner b-a-c to j-i-k
              double qai_sheet = 0;
              #if GA_COMPARE_CORNER > 0
              //b-a-c <-> j-i-k
              for (int c_ind=0; c_ind<G_->nodes(a)->n_conn_nids(); c_ind++) {
                int c = G_->nodes(a)->conn_nids (c_ind);
                if (c ==b)
                  continue;
                for (int k_ind=0; k_ind<g_->nodes(i)->n_conn_nids(); k_ind++) {
                  int k = g_->nodes(i)->conn_nids (k_ind);
                  if (k ==j)
                    continue;
                  qai_sheet += M_[c][k] * C_aibjck (b, a, c, j, i, k);
                }
              }

              Qai_sheet += float (qai_sheet * M_[b][j]);
              #endif
            }//end for j
          }//end for b
          #endif

          // forget 0.5 here. just way to define energy and alpha

          //: Note: for the trivial matching example, 
          //  Qai_node itself is sufficient,
          //  Qai_link itself is also sufficient,
          //  Problem in Qai_sheet.
          ///Qai = Qai_node + Qai_link + Qai_sheet;
          Qai = Qai_node * params_.wN_ + Qai_link * params_.wL_ + Qai_sheet * params_.wC_; 
          M_hat_[a][i] = Qai;
        }//end i

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

    if (debug_out_>3)
      print_M (M_, M_row_, M_col_); //debug: 

    T = params_.Tr_*T;
    iter_a++;

    //Update the (R, T) using the current assignment M.      
    if (use_R_T_)
      update_new_R_T ();

  }//end loop A

GRAD_ASGN_FINISH:
  if (debug_out_>1)
    vcl_cout << "\nTotal iteration A: " << iter_a << vcl_endl;
  //M_ is the result, put clean one in M_hat_
  make_assignment_matrix (M_, M_hat_);
  
  vcl_clock_t t2 = vcl_clock();

  if (debug_out_)
    print_M (M_, M_row_, M_col_);
  if (debug_out_>1) {
    print_M_bin (M_hat_, M_row_, M_col_);
    print_match_result ();
    vcl_cout << "\n ==> GA assign requires " << (double)(t2-t1) / CLOCKS_PER_SEC << " seconds.\n";
  }  

  return num_stable_;
}

void dbasnh_gradasgn_aug::update_new_R_T ()
{
  vcl_vector<vgl_point_3d<double> > Gnode, gnodeM;
  vnl_matrix_fixed<double,3,3>    R;
  vnl_vector_fixed<double,3>      Cf, Cm;

  //Need to normalize M without slacks (store in M_hat_).
  get_norm_row_M_no_slack (M_, M_hat_, M_row_, M_col_);

  //Put all nodes in hypg_G into the point set Gnode.
  //compute gnodeM = M[][] * gnode[];
  for (unsigned int i=0; i<hypg_G_aug()->nodes().size(); i++) {
    Gnode.push_back (hypg_G_aug()->N_aug(i)->pt());

    double x=0, y=0, z=0;
    for (unsigned int j=0; j<hypg_g_aug()->nodes().size(); j++) {
      x += M_hat_[i][j] * hypg_g_aug()->N_aug(j)->pt().x();
      y += M_hat_[i][j] * hypg_g_aug()->N_aug(j)->pt().y();
      z += M_hat_[i][j] * hypg_g_aug()->N_aug(j)->pt().z();
    }
    
    gnodeM.push_back (vgl_point_3d<double> (x, y, z));
  }
  
  //Solve for the best (R, T)
  dbgl_get_rigid_alignment (Gnode, gnodeM, R, Cf, Cm);

  R_ = R;
  T_ = Cm - Cf;

  //Debug:
  ///print_M (M_hat_, M_row_, M_col_);
  ///vul_printf (vcl_cout, "\n  R:\n");
  ///R_.print (vcl_cerr);
  ///vul_printf (vcl_cout, "  T:\n");
  ///T_.print (vcl_cerr);
}

//: Compute the final similarity (as energy defined in the GA).
double dbasnh_gradasgn_aug::get_similarity (const int verbose)
{
  double node_sim = 0;
  double node_sim_max = 0;
  double curve_sim = 0;
  double curve_sim_max = 0;
  double corner_sim = 0;
  double corner_sim_max = 0;

  for (int a = 0; a< M_row_-1; a++) {
    for (int i = 0; i<M_col_-1; i++) {      
      if (M_hat_[a][i] == 0)
        continue; //skip non-matching entries.

      double m1 = M_[a][i] * params_.wN_;
      node_sim_max += m1;
      double Cai = C_ai (a, i);
      node_sim += m1 * Cai;

      for (int b_ind=0; b_ind<G_->nodes(a)->n_conn_nids(); b_ind++) {
        int b = G_->nodes(a)->conn_nids (b_ind);
        for (int j_ind=0; j_ind<g_->nodes(i)->n_conn_nids(); j_ind++) {
          int j = g_->nodes(i)->conn_nids (j_ind);

          if (M_hat_[b][j] == 0)
            continue; //skip non-matching entries.

          double m2 = M_[a][i] * M_[b][j] * params_.wL_;       
          curve_sim_max += m2;
          double Caibj = C_aibj (a, b, i, j);
          curve_sim += m2 * Caibj;

          for (int c_ind=0; c_ind<G_->nodes(a)->n_conn_nids(); c_ind++) {
            int c = G_->nodes(a)->conn_nids (c_ind);
            if (c==b)
              continue;
            for (int k_ind=0; k_ind<g_->nodes(i)->n_conn_nids(); k_ind++) {
              int k = g_->nodes(i)->conn_nids (k_ind);
              if (k==j)
                continue;

              if (M_hat_[c][k] == 0)
                continue; //skip non-matching entries.

              double m3 = M_[a][i] * M_[b][j] * M_[c][k] * params_.wC_;
              corner_sim_max += m3;
              double Caibjck = C_aibjck (b, a, c, j, i, k);
              corner_sim += m3 * Caibjck;
            }
          }
        }
      }
    }
  }

  //We should NOT consider the matching of slack as part of similarity!
  //This deviates the perfect similarity of identical shapes!!
  //Add the slack rows and columns (except the last entry) to the total max similarity.
  //Slack row at the bottom
  /*for (int i = 0; i<M_col_-1; i++) {
    node_sim_max += M_[M_row_-1][i];
    curve_sim_max += M_[M_row_-1][i];
    corner_sim_max += M_[M_row_-1][i];
  }  
  //Slack column at the right
  for (int a = 0; a<M_row_-1; a++) {
    node_sim_max += M_[a][M_col_-1];
    curve_sim_max += M_[a][M_col_-1];
    corner_sim_max += M_[a][M_col_-1];
  }*/

  //Since we pick subset of MS curves for matching, it is possible there is no corner matches.
  norm_sim_node_ = (node_sim_max != 0) ? node_sim / node_sim_max : 1;
  norm_sim_curve_ = (curve_sim_max != 0) ? curve_sim / curve_sim_max : 1;
  norm_sim_corner_ = (corner_sim_max != 0) ? corner_sim / corner_sim_max : 1;  

  //It is hard to justify how to normalize the similarity properly.
  //GA graph matching is good for matching nodes.
  //Curve matching is not always guaranteed, so if we add curve similarity after normalization,
  //it is too rush, and even good matching case get very low similarity score.

  //Here we simply use normalized node similarity as final similarit measure!
  //We might want to augment it with some curve and corner similarity (but how to ensure normalization??)
  //In the case curve matches, both end nodes also matches, so it is already considered in the node similarity measure!
  ///norm_similarity_ = node_sim_n;
  //The above is not good, not using the discriminative ability of D.P. shock curve matching!

  if (corner_sim_max != 0) {
    similarity_ = (params_.wN_ * node_sim + params_.wL_ * curve_sim  + params_.wC_ * corner_sim) / 
                  (params_.wN_ + params_.wL_ + params_.wC_);
    norm_similarity_ = (params_.wN_ * norm_sim_node_ + params_.wL_ * norm_sim_curve_ + params_.wC_ * norm_sim_corner_) / 
                       (params_.wN_ + params_.wL_ + params_.wC_);
  }
  else {
    if (curve_sim_max != 0) {
      similarity_ = (params_.wN_ * node_sim + params_.wL_ * curve_sim) / 
                    (params_.wN_ + params_.wL_);
      norm_similarity_ = (params_.wN_ * norm_sim_node_ + params_.wL_ * norm_sim_curve_) / 
                         (params_.wN_ + params_.wL_);
    }
    else {
      if (node_sim_max != 0) {
        similarity_ = node_sim;
        norm_similarity_ = norm_sim_node_;
      }
      else {
        similarity_ = 1;
        norm_similarity_ = 1;
      }
    }
  }

  if (verbose) {
    vul_printf (vcl_cout, "\n  node sim = %.3f, max = %.3f, normalized = %.3f, wN = %.3f.\n", node_sim, node_sim_max, norm_sim_node_, params_.wN_);
    vul_printf (vcl_cout, "  curve sim = %.3f, max = %.3f, normalized = %.3f, wL = %.3f.\n", curve_sim, curve_sim_max, norm_sim_curve_, params_.wL_);
    vul_printf (vcl_cout, "  corner sim = %.3f, max = %.3f, normalized = %.3f, wC = %.3f.\n", corner_sim, corner_sim_max, norm_sim_corner_, params_.wC_);
    vul_printf (vcl_cout, "  Similarity = %.3f, Norm Similarity = %f.\n", similarity_, norm_similarity_);
  }

  return norm_similarity_;
}

//###################################################################

bool is_same_n_type (AN_TYPE Na_type, AN_TYPE Ni_type)
{
  if (Na_type == AN_TYPE_RIB_END || Na_type == AN_TYPE_DEGE_RIB_END) {
    if (Ni_type != AN_TYPE_RIB_END && Ni_type != AN_TYPE_DEGE_RIB_END)
      return false;
  }
  else if (Na_type == AN_TYPE_AXIAL_END || Na_type == AN_TYPE_DEGE_AXIAL_END) {
    if (Ni_type != AN_TYPE_AXIAL_END && Ni_type != AN_TYPE_DEGE_AXIAL_END) 
      return false;
  }
  else if (Na_type == AN_TYPE_LOOP_END) {
    if (Ni_type != AN_TYPE_LOOP_END)
      return false;
  }
  else {
    assert (0);
  }

  return true;
}

bool dbash_get_canonical_type (const int nA3, const int nA13, const int nDege, 
                                 int& m, int& n, int& f)
{
  //assume all dege. curves are A14 curves.
  //so equivalent to 2*nDege A13 curves.
  int a = nA13 + 2*nDege;
  m = 0;
  n = 0;
  f = 0;

  if (nA3 == 0) { //1) A1n
    assert (a % 2 == 0);
    n = a / 2 + 2;
    return true;
  }
  else if (nA3 == 1) { //2) A1mA3
    assert (a % 2);
    m = (a+1) / 2;
    return true;
  }
  else {
    if (nA3 == nA13) { //3) multiple loop end of A1A3-f.
      f = nA3;
      m = 1;
      return true;
    }
    else if (nA13 == 1) { //4) The corner type dege. A5 (two A3)
      //treated as A1A3-f for now.
      f = nA3;
      m = 1;
      return true;
    }
  }

  ///assert (0); //Node is with non-canoncial type.
  return false;
}

double dbash_comp_node_type_diff (const int m1, const int n1, const int f1, 
                                  const int m2, const int n2, const int f2)
{
  //Implement Table 7.3 and Table 7.4 in thesis.
  if (f1 == 0 && f2 == 0) {
    //Implement Table 7.3 in thesis.
    if (m1 == 0) {
      assert (n1 != 0);
      if (m2 == 0) {
        assert (n2 != 0);
        //A_1^n1 to A_1^n2.
        return double(vnl_math_abs (n1-n2)) / vnl_math_max (n1, n2);
      }
      else { //m2 !=0
        assert (n2 == 0);
        //A_1^n1 to A_1^m2A_3.
        return vnl_math_max (double(vnl_math_abs (n1-m2)), 3.0) / vnl_math_max (n1, m2+3);
      }
    }
    else { //m1 != 0
      assert (n1 == 0);
      if (m2 == 0) {
        assert (n2 != 0);
        //A_1^m1A_3 to A_1^n2.
        return vnl_math_max (double(vnl_math_abs (n2-m1)), 3.0) / vnl_math_max (n2, m1+3);
      }
      else { //m2 !=0
        assert (n2 == 0);
        //A_1^m1A_3 to A_1^m2A_3.
        return double(vnl_math_abs (m1-m2)) / (vnl_math_max (m1, m2) + 3);
      }
    }
  }
  else {
    //Implement Table 7.4 in thesis.
    if (f1 != 0) {
      if (m1==1 && f1==3) { //Corner
        if (f2==0) {
          //Corner to A1mA3 or A1n.
          if (n2==0) {
            assert (m2 != 0);
            //Corner to A1mA3. 
            return double(m2) / vnl_math_max (m2+3, 5);
          }
          else {
            assert (m2 == 0);
            //Corner to A1n.
            return 1;
          }
        }
        else if (m2==1 && f2==3) {
          //Corner to corner.
          return 0;
        }
        else if (f2==2) {
          //Corner to (A1A3)_2
          return 0.25;
        }
        else if (f2==3) {
          //Corner to (A1A3)_3
          return 0.5;
        }
        else {
          assert (f2!=1);
          //Not classified, assume no compatibility.
          return 1;
        }
      }
      else if (f1==2) {
        if (f2==0) {
          //(A1A3)_2 to A1mA3 or A1n.
          if (n2==0) {
            assert (m2 != 0);
            //(A1A3)_2 to A1mA3.
            return double (3 + vnl_math_abs(m2-2)) / vnl_math_max (m2+3, 8);
          }
          else {
            assert (m2 == 0);
            //(A1A3)_2 to A1n.
            return 1;
          }
        }
        else if (m2==1 && f2==3) {
          //(A1A3)_2 to Corner.
          return 0.25;
        }
        else if (f2==2) {
          //(A1A3)_2 to (A1A3)_2.
          return 0;
        }
        else if (f2==3) {
          //(A1A3)_2 to (A1A3)_3.
          return 1/3;
        }
        else {
          assert (f2!=1);
          //Not classified, assume no compatibility.
          return 1;
        }
      }
      else if (f1==3) {
        if (f2==0) {
          //(A1A3)_3 to A1mA3 or A1n.
          if (n2==0) {
            assert (m2 != 0);
            //(A1A3)_3 to A1mA3.
            return double (6 + vnl_math_abs(m2-3)) / vnl_math_max (m2+3, 12);
          }
          else {
            assert (m2 == 0);
            //(A1A3)_3 to A1n.
            return 1;
          }
        }
        else if (m2==1 && f2==3) {
          //(A1A3)_3 to Corner.
          return 0.5;
        }
        else if (f2==2) {
          //(A1A3)_3 to (A1A3)_2.
          return 1/3;
        }
        else if (f2==3) {
          //(A1A3)_3 to (A1A3)_3.
          return 0;
        }
        else {
          assert (f2!=1);
          //Not classified, assume no compatibility.
          return 1;
        }
      }
      else {
        assert (f1 != 1);
        //Not classified, assume no compatibility.
        return 1;
      }
    }
    else {
      assert (f2 != 0);
      if (m2==1 && f2==3) { //Corner
        //A1mA3 or A1n to Corner.
        if (n1==0) {
          assert (m1 != 0);
          //A1mA3 to Corner. 
          return double(m1) / vnl_math_max (m1+3, 5);
        }
        else {
          assert (m1 == 0);
          //A1n to corner.
          return 1;
        }
      }
      else if (f2==2) {
        //A1mA3 or A1n to to (A1A3)_2
        if (n1==0) {
          assert (m1 != 0);
          //A1mA3 to (A1A3)_2
          return double (3 + vnl_math_abs(m1-2)) / vnl_math_max (m1+3, 8);
        }
        else {
          assert (m1 == 0);
          //A1n to to (A1A3)_2
          return 1;
        }
      }
      else if (f2==3) {
        //A1mA3 or A1n to to (A1A3)_3
        if (n1==0) {
          assert (m1 != 0);
          //A1mA3 to (A1A3)_3
          return double (6 + vnl_math_abs(m1-3)) / vnl_math_max (m1+3, 12);
        }
        else {
          assert (m1 == 0);
          //A1n to to (A1A3)_3
          return 1;
        }
      }
      else {
        assert (f2 != 1);
        //Not classified, assume no compatibility.
        return 1;
      }
    }
  }
}

bool is_same_c_type (AL_TYPE Lab_type, AL_TYPE Lij_type)
{
  if (Lab_type == AL_TYPE_A3_RIB) {
    if (Lij_type != AL_TYPE_A3_RIB)
      return false;
  }
  else {
    assert (Lab_type == AL_TYPE_A13_AXIAL || Lab_type == AL_TYPE_DEGE_AXIAL);
    if (Lij_type != AL_TYPE_A13_AXIAL && Lab_type != AL_TYPE_DEGE_AXIAL)
      return false;
  }

  return true;
}

