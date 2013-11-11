//---------------------------------------------------------------------
// This is brcv/rec/dbasnh/dbasnh_gradasgn.h
//:
// \file
// \brief class for graduated assignment between two hypergraphs
//
// \author
//  Ming-Ching Chang - January 6, 2008
//
// \verbatim
//  Modifications
// \endverbatim
//
//-------------------------------------------------------------------------

#ifndef dbasnh_gradasgn_h_
#define dbasnh_gradasgn_h_

#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector_fixed.h>

#include <dbasn/dbasn_gradasgn.h>
#include <dbasn/dbasnh_hypg.h>

#define DP_DIST_HUGE  1E20

class dbasnh_gradasgn : public dbasn_gradasgn
{
protected:
public:
  //###### Constructor & Destructor ######
  dbasnh_gradasgn () : dbasn_gradasgn () {
  }
  virtual ~dbasnh_gradasgn () {
  }

  //###### Data Access Functions ######
  dbasnh_hypg* hypg_G() {
    return (dbasnh_hypg*) G_;
  }
  void set_hypg_G (dbasnh_hypg* G) {
    G_ = G;
  }
  dbasnh_hypg* hypg_g() {
    return (dbasnh_hypg*) g_;
  }
  void set_hypg_g (dbasnh_hypg* g) {
    g_ = g;
  }

  //###### Compatibility definition functions ######
  //: compute corner similarity (compatibility).
  virtual double C_aibjck (const int a, const int b, const int c, 
                           const int i, const int j, const int k);

  virtual void normalize_costs (const bool abs_max);

  //: An extended version of get_assignment() to add the new C_abcijk term.
  //  Return false if numerical problem occurs.
  virtual bool get_assignment ();

  //: Compute the final similarity (as energy defined in the GA).
  virtual double get_similarity (const int verbose = 1);
};

class dbasnh_hypg_aug;

class dbasnh_gradasgn_aug : public dbasnh_gradasgn
{
protected:
  bool use_R_T_;
  bool use_DP_cost_;

  //: table to store edit distance: match G <-> g curves
  vcl_vector<vcl_vector<double> > Gg_ed_dist_;

  //: table to store Euclidean distance: match G <-> g curves
  vcl_vector<vcl_vector<double> > Gg_Eu_dist_;

  //: table to store edit distance: match G <-> g curves flip
  vcl_vector<vcl_vector<double> > Gg_ed_dist_f_;

  //: table to store Euclidean distance: match G <-> g curves flip
  vcl_vector<vcl_vector<double> > Gg_Eu_dist_f_;

  vnl_matrix_fixed<double,3,3>    R_; 
  vnl_vector_fixed<double,3>      T_; 

  double max_len_btwn_nodes_;

  double similarity_;
  double norm_similarity_;
  double norm_sim_node_;
  double norm_sim_curve_;
  double norm_sim_corner_;

public:
  //###### Constructor & Destructor ######
  dbasnh_gradasgn_aug () : dbasnh_gradasgn () {
    use_R_T_ = false;
    use_DP_cost_ = false;
    R_.set_identity ();
    max_len_btwn_nodes_ = 0;
    similarity_ = 0;
    norm_similarity_ = 0;
    norm_sim_node_ = 0;
    norm_sim_curve_ = 0;
    norm_sim_corner_ = 0;
  }
  void clear () {
    Gg_ed_dist_.clear ();
    Gg_Eu_dist_.clear ();
    Gg_ed_dist_f_.clear ();
    Gg_Eu_dist_f_.clear ();
  }
  virtual ~dbasnh_gradasgn_aug () {
    clear ();
  }

  //####### Data access functions ####### 
  dbasnh_hypg_aug* hypg_G_aug() {
    return (dbasnh_hypg_aug*) G_;
  }
  dbasnh_hypg_aug* hypg_g_aug() {
    return (dbasnh_hypg_aug*) g_;
  }

  const bool use_DP_cost () const {
    return use_DP_cost_;
  }
  void set_use_DP_cost (const bool b) {
    use_DP_cost_ = b;
  }

  const bool use_R_T () const {
    return use_R_T_;
  }
  void set_use_R_T (const bool b) {
    use_R_T_ = b;
  }

  //: table to store edit distance: match G <-> g curves
  vcl_vector<vcl_vector<double> >& Gg_ed_dist () {
    return Gg_ed_dist_;
  }
  vcl_vector<double>& Gg_ed_dist (const unsigned int i) {
    return Gg_ed_dist_[i];
  }
  const double Gg_ed_dist (const unsigned int i, const unsigned int j) const {
    return Gg_ed_dist_[i][j];
  }

  //: table to store Euclidean distance: match G <-> g curves
  vcl_vector<vcl_vector<double> >& Gg_Eu_dist() {
    return Gg_Eu_dist_;
  }
  vcl_vector<double>& Gg_Eu_dist (const unsigned int i) {
    return Gg_Eu_dist_[i];
  }
  const double Gg_Eu_dist (const unsigned int i, const unsigned int j) const {
    return Gg_Eu_dist_[i][j];
  }

  //: table to store edit distance: match G <-> g curves flip
  vcl_vector<vcl_vector<double> >& Gg_ed_dist_f() {
    return Gg_ed_dist_f_;
  }
  vcl_vector<double>& Gg_ed_dist_f (const unsigned int i) {
    return Gg_ed_dist_f_[i];
  }
  const double Gg_ed_dist_f (const unsigned int i, const unsigned int j) const {
    return Gg_ed_dist_f_[i][j];
  }

  //: table to store Euclidean distance: match G <-> g curves flip
  vcl_vector<vcl_vector<double> >& Gg_Eu_dist_f() {
    return Gg_Eu_dist_f_;
  }
  vcl_vector<double>& Gg_Eu_dist_f (const unsigned int i) {
    return Gg_Eu_dist_f_[i];
  }
  const double Gg_Eu_dist_f (const unsigned int i, const unsigned int j) const {
    return Gg_Eu_dist_f_[i][j];
  }
  
  vnl_matrix_fixed<double,3,3>& R() {
    return R_; 
  }
  vnl_vector_fixed<double,3>& T() {
    return T_; 
  }

  double similarity () {
    return similarity_;
  }
  double norm_similarity () {
    return norm_similarity_;
  }
  double norm_sim_node () {
    return norm_sim_node_;
  }
  double norm_sim_curve () {
    return norm_sim_curve_;
  }
  double norm_sim_corner () {
    return norm_sim_corner_;
  }

  //####### Data query functions ####### 
  double get_norm_row_dist (vcl_vector<vcl_vector<double> >& table,
                            const int r, const int c);

  //####### Data modification functions ####### 
  void init_table (const int Gne, const int gne) {
    vcl_vector<double> dummy;
    
    for (int i=0; i<Gne; i++) {
      //Table to store edit distance: match G <-> g curves
      Gg_ed_dist_.push_back (dummy);

      //Table to store Euclidean distance: match G <-> g curves
      Gg_Eu_dist_.push_back (dummy);

      //Table to store edit distance: match G <-> g curves flip
      Gg_ed_dist_f_.push_back (dummy);

      //Table to store Euclidean distance: match G <-> g curves flip
      Gg_Eu_dist_f_.push_back (dummy);
    }
  }
  
  //###### Compatibility definition functions ######

  //: Check if the two nodes (a,i)'s are the same type. 
  bool same_N_type (const int a, const int i);

  double get_N_type_diff (const int a, const int i);

  //: compute node similarity (compatibility).
  virtual double C_ai (const int a, const int i);

  //: node structural similarity.
  virtual double C_ai_str (const int a, const int i);

  //: Check if the two links (ab, ij)'s are the same type. 
  bool same_L_type (const int a, const int b, const int i, const int j);

  //: compute link similarity (compatibility).
  virtual double C_aibj (const int a, const int b, const int i, const int j);
  
  //: curve structural similarity.
  virtual bool C_aibj_str_valid (const int a, const int b, const int i, const int j);

  virtual double C_aibj_str (const int a, const int b, const int i, const int j);

  //: compute corner similarity (compatibility).
  virtual double C_aibjck (const int a, const int b, const int c, 
                           const int i, const int j, const int k);

  //: corner structural similarity.
  virtual bool C_aibjck_str_valid (const int a, const int b, const int c, 
                                   const int i, const int j, const int k);

  virtual double C_aibjck_str (const int a, const int b, const int c, 
                               const int i, const int j, const int k);

  virtual void normalize_costs (const bool abs_max);

  void init_R_T ();

  void get_max_len_between_nodes ();
  
  //: An extended version of get_assignment() to updating (R, T) in the iterative process.
  //  Return false if numerical problem occurs.
  virtual bool get_assignment ();

  void update_new_R_T ();

  //: Compute the final similarity (as energy defined in the GA).
  virtual double get_similarity (const int verbose = 1);
};

bool is_same_n_type (AN_TYPE Na_type, AN_TYPE Ni_type);


bool dbash_get_canonical_type (const int nA3, const int nA13, const int nDege, 
                               int& m, int& n, int& f);

double dbash_comp_node_type_diff (const int m1, const int n1, const int f1, 
                                  const int m2, const int n2, const int f2);

bool is_same_c_type (AL_TYPE Lab_type, AL_TYPE Lij_type);

#endif
