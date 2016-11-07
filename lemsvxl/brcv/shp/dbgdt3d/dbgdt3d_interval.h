//: Aug 19, 2005 MingChing Chang
//  Definitions for the interval for the wavefront propagation algorithm
//  for computing the goedesic distance transform.

#ifndef _gdt_interval_h_
#define _gdt_interval_h_

#include <vcl_cmath.h>
#include <vcl_utility.h>
#include <vcl_algorithm.h>
#include <vgl/vgl_point_3d.h>

#include <dbmsh3d/dbmsh3d_utils.h>
#include <dbmsh3d/dbmsh3d_halfedge.h>
#include <dbgdt3d/dbgdt3d_numerical.h>
#include <dbgdt3d/dbgdt3d_vertex.h>
#include <dbgdt3d/dbgdt3d_solve_intersect.h>

class dbmsh3d_face;
class dbmsh3d_gdt_edge;
class dbmsh3d_gdt_vertex_3d;
class gdt_path;
class gdt_shock;

// ###############################################################

typedef enum {
  BOGUS_INTERVAL_TYPE = 0,
  ITYPE_PSRC          = 1,
  ITYPE_DEGE          = 2,
  ITYPE_LSRC          = 3,
  ITYPE_DUMMY         = 4,
} INTERVAL_TYPE;

//: Interval projection result.
typedef enum {
  IP_INVALID      = 0,
  IP_DEGE         = 1,
  IP_VALID        = 2,
} IP_RESULT;

// ###############################################################
//: The base class for an interval, including only the extent and type.
class gdt_ibase
{
protected:
  INTERVAL_TYPE type_;

  //: starting arc-length parameter of this interval.
  //  stau_ == 0: the first interval
  //  etau_ == length: the last interval
  double stau_;
  double etau_;

public:
  //: ====== Constructor/Destructor ======
  gdt_ibase (const INTERVAL_TYPE type, const double stau, const double etau) 
  {
    //: assert numerical errors
    assert (_eqT (stau, etau)==false);

    type_ = (INTERVAL_TYPE) type;
    stau_ = (double) stau;
    etau_ = (double) etau;
  }
  virtual ~gdt_ibase () {}

  //: ====== Data access functions ======
  INTERVAL_TYPE type () const {
    return type_;
  }

  bool is_dege () const {
    return (type_ == ITYPE_DEGE);
  }
  bool is_psrc () const {
    return (type_ == ITYPE_PSRC);
  }
  bool is_lsrc () const {
    return (type_ == ITYPE_LSRC);
  }
  bool is_dummy() const {
    return (type_ == ITYPE_DUMMY);
  }

  double stau () const {
    return stau_;
  }
  virtual void _set_stau (double s) {
    assert (s < etau_);
    stau_ = s;
  }
  double etau () const {
    return etau_;
  }
  virtual void _set_etau (double e) {
    assert (e > stau_);
    etau_ = e;
  }
};

inline bool _is_I_overlapping (const gdt_ibase* I1, const gdt_ibase* I2)
{
  const double min = vcl_max (I1->stau(), I2->stau());
  const double max = vcl_min (I1->etau(), I2->etau());

  if (min < max)
    return true;
  else
    return false;
}

// ###############################################################
//: The intermediate circular wavefront interval class
class gdt_interval : public gdt_ibase, public vispt_elm
{
protected:
  //: pointer to halfedge of the wavefront interval.
  dbmsh3d_halfedge*       he_;

  //: The previous gdt_interval of it in the propagation.
  //  if prevI_ == NULL: the init interval from a pseudo-source.
  //  For the degenerate interval, the prevI_ is used to point to 
  //  one of the parent's interval sharing the vertex psrc_. 
  //  It is used in the propagation of multiple rarefaction faces in the saddle case.
  gdt_interval*           prevI_;

  //: if flag true, this I is tied with prevI_ in the prev-next link
  bool prev_flag_;

  vcl_vector<gdt_interval*> nextIs_;

  //: point pseudo-source
  dbmsh3d_gdt_vertex_3d*  psrc_;

  double                  L_;
  double                  H_;
  
public:
  //: ====== Constructor/Destructor ======
  gdt_interval (const INTERVAL_TYPE type,
                const double stau, const double etau,
                const dbmsh3d_halfedge* he,
                const dbmsh3d_gdt_vertex_3d* psrc, 
                const double L, const double H, 
                const gdt_interval* prev);
  gdt_interval (const gdt_interval* I);

  virtual ~gdt_interval ();

  //: ====== Data access functions ======
  dbmsh3d_halfedge* he() const {
    return he_;
  }

  dbmsh3d_gdt_edge* edge() const {
    return (dbmsh3d_gdt_edge*) he_->edge();
  }
  dbmsh3d_gdt_vertex_3d* sV() const;
  dbmsh3d_gdt_vertex_3d* eV() const;

  bool _psrc_closer_to_Sv () const {    
    return vcl_fabs (L_-stau_) < vcl_fabs (L_-etau_);
  }

  //: edge's starting vertex S and ending vertex E
  vgl_point_3d<double> Spt () const;
  vgl_point_3d<double> Ept () const;

  dbmsh3d_face* _face() const;
  
  dbmsh3d_gdt_vertex_3d* psrc() const {
    return psrc_;
  }
  double mu() const {
    assert (psrc_->dist() != INVALID_DIST);
    return psrc_->dist();
  }

  double L() const {
    return L_;
  }
  double H() const {
    return H_;
  }

  gdt_interval* prevI() const {
    return prevI_;
  }
  void _set_prevI (const gdt_interval* prev) {
    prevI_ = (gdt_interval*) prev;
  }

  //: return true if I is in this interval's prev tree.
  bool in_prevI_tree (const gdt_interval* I) const {
    gdt_interval* cur = prevI_;
    while (cur != NULL) {
      if (cur == I)
        return true;

      cur = cur->prevI();
    }
    return false;
  } 
  
  bool prev_flag() const {
    return prev_flag_;
  }
  void set_prev_flag (const bool b) {
    prev_flag_ = b;
  }

  unsigned int num_nextI() const {
    return nextIs_.size();
  }

  vcl_vector<gdt_interval*>& nextIs() {
    return nextIs_;
  }
  gdt_interval* nextI (const unsigned int i) const {
    assert (i==0 || i==1);
    if (i < nextIs_.size())
      return nextIs_[i];
    else
      return NULL;
  }
  void _add_nextI (const gdt_interval* nI) {
    nextIs_.push_back ((gdt_interval*) nI);
  }
  void _remove_nextI (const gdt_interval* nI) {
    vcl_vector<gdt_interval*>::iterator it = nextIs_.begin();
    for (; it != nextIs_.end(); it++) {
      gdt_interval* I = (*it);
      if (I == nI) {
        nextIs_.erase (it);
        return;
      }
    }
    assert (0);
  }
  bool is_nextI (const gdt_interval* nI) {
    vcl_vector<gdt_interval*>::iterator it = nextIs_.begin();
    for (; it != nextIs_.end(); it++) {
      gdt_interval* I = (*it);
      if (I == nI)
        return true;
    }
    return false;
  }

  //: Return true if this I has a nextI on given e
  bool _has_nextI_on_edge (const dbmsh3d_gdt_edge* e) {
    vcl_vector<gdt_interval*>::iterator it = nextIs_.begin();
    for (; it != nextIs_.end(); it++) {
      gdt_interval* I = (*it);
      if (I->edge() == e)
        return true;
    }
    return false;
  }

  bool _is_psource_L () const {
    return L_ < stau_;
  }
  bool _is_psource_R () const {
    return L_ > etau_;
  }
  bool _is_psource_CL () const {
    return L_-stau_ < etau_-L_;
  }

  virtual void _set_stau (double s) {
    assert (s < etau_);
    stau_ = s;
  }
  virtual void _set_etau (double e) {
    assert (e > stau_);
    etau_ = e;
  }

  //: ====== Query functions ======
  bool _is_tau_in (const double& tau) const {
    return (stau_ < tau) && (tau < etau_);
  }
  bool _is_tau_eq_in (const double& tau) const {
    return (stau_ <= tau) && (tau <= etau_);
  }
  bool _is_tau_eqT_in (const double& tau) const {
    return _eqT (tau, stau_) || _eqT (tau, etau_) || _is_tau_in (tau);
  }
  double _fit_tau_fuzzy (const double& input_tau) const {
    if (_eqT (input_tau, stau_))
      return stau_;
    else if ( _eqT (input_tau, etau_))
      return etau_;
    else
      return input_tau;
  }
  dbmsh3d_face* m2_next_face () const;

  
  //: given a theta, return the arc-length parameter, tau
  double tau_from_theta (const double input_theta) const {
    return _tau_from_theta (L_, H_, input_theta);
  }

  dbmsh3d_gdt_vertex_3d* get_VO (dbmsh3d_face* F) const;

  //: return true if this interval is on the input_face.
  bool is_on_face (const dbmsh3d_face* input_face) const;

  //: return true if this interval is from input_edge
  bool is_from_edge (const dbmsh3d_gdt_edge* input_edge) const;

  //: given a theta, return the extrinsic coordinate
  vgl_point_3d<double> _point_from_tau (const double tau) const;

  //: given a tau, return the local geodesic distance of it
  double _get_d_at_tau (const double input_tau) const;

  //: given a tau, return the geodesic shortest distance of it
  double get_dist_at_tau (const double input_tau) const;

  double get_dist_at_tau_fuzzy (const double& input_tau) const;

  
  //: if possible, should use the regular version which is faster.
  double _get_local_dist (const double input_tau) const {
    return vnl_math::hypot (input_tau-L_, H_); 
  }

  double _get_min_dist () const;

  void get_min_max_dist (double& min_dist, double& max_dist) const;

  //: given the query_dist, there exists at almost two points with the given geodesic distance
  //  also check for validality, if not valid, put value -1
  void query_taus_from_dist (const double gdt_dist, double& tau1, double& tau2) const;

  //: return the # of iterations needed for the query input_tau and a given query_dist.
  //  local_s is the residue arc length.
  int I_iter_to_source (const double input_tau, const double query_dist, double& local_s) const;

  //: given a tau, return the geodesic path till the previous pseudo-source 
  //  (some intermediate vertex of the gdt_path to the closest source)
  void I_get_prev_path_till_psource (const double input_tau, gdt_path* gdt_path) const;
  
  //: Find the next face to propagate for the degenerate interval.
  //  It can not be the cur_from_he, if known.
  dbmsh3d_halfedge* dege_get_he_to_propagate (const dbmsh3d_halfedge* cur_from_he = NULL);

  dbmsh3d_gdt_edge* get_nextF_eL () const;
  dbmsh3d_gdt_edge* get_nextF_eR () const;

  //: ====== Projection functions ======
  
  //: decide the prev propagation is a left or a right one.
  bool is_prev_left () const;

  //: given a tau, return the angle parameter, theta
  double theta_from_tau (const double input_tau) const {
    return _theta_from_tau (L_, H_, input_tau);
  }

  bool detect_project_to_L_dege (const double& alpha_cl) const;
  bool detect_project_to_R_dege (const double& alpha_cr) const;

  bool do_L_projections (const double& alpha_l, const dbmsh3d_gdt_edge* eL,
                         double& left_nL, double& left_nH, 
                         double& proj_stau, double& proj_etau) const;

  IP_RESULT L_proj_tauS_tauE (const double& alpha_l, 
                              const dbmsh3d_gdt_edge* eL,
                              double& L_nL, double& L_nH, 
                              double& tau_min, double& tau_max) const;

  bool L_proj_tau (const double& tau, const double& alphaCL, 
                   const dbmsh3d_gdt_edge* eL, double& prj_tau) const;

    void _get_left_nL_nH (const double& alpha_l, double& left_nL, double& left_nH) const;

    //: return the left_nL without orientation fix
    double get_left_nL_nH (const double alpha_l, const dbmsh3d_gdt_edge* eL,
                           double& left_nL, double& left_nH) const;

    bool _project_tau_to_L (const double& input_tau, const double& alpha_l, 
                            const dbmsh3d_gdt_edge* eL, 
                            const double& left_nL_nofix, const double& left_nH, 
                            double& proj_tau) const;

  bool do_R_projections (const double& alpha_r, const dbmsh3d_gdt_edge* eR,
                         double& right_nL, double& right_nH, 
                         double& proj_stau, double& proj_etau) const;

  IP_RESULT R_proj_tauS_tauE (const double& alpha_r, 
                              const dbmsh3d_gdt_edge* eR,
                              double& R_nL, double& R_nH,  
                              double& tau_min, double& tau_max) const;
  bool R_proj_tau (const double& tau, const double& alphaCR, 
                   const dbmsh3d_gdt_edge* eR, double& prj_tau) const;

    void _get_right_nL_nH (const double alpha_r, const dbmsh3d_gdt_edge* eR,
                           double& right_nL_nofix, double& right_nH) const;

    //: return the right_nL (without orientation fix)
    double get_right_nL_nH (const double alpha_r, const dbmsh3d_gdt_edge* eR,
                            double& right_nL, double& right_nH) const;

    bool _project_tau_to_R (const double& input_tau, const double& alpha_r, 
                            const dbmsh3d_gdt_edge* eR,
                            const double& right_nL, const double& right_nH, 
                            double& proj_tau) const;
    
  // Return true if the update makes this interval FULLY_COVERED.
  ///virtual bool update_coverage (const WAVEFRONT_COVERAGE c);
  bool _Spt_closer_than_Ept ();
  bool _Ept_closer_than_Spt ();

  //: ====== Geodesic Shortest Distance Query functions ======
  //: given a theta, return the extrinsic coordinate
  vgl_point_3d<double> point_from_theta (const double theta) const;

  //: return prev angle alpha
  double _prev_alpha () const;

  //: given a theta, return the parameter theta on the previous front theta_p of it
  //  you should check prevI_ != NULL before using this function.
  double _get_prev_theta (const double theta) const;

  double get_prev_tau (const double input_tau) const;

  void get_min_max_dist_I_face (double& min_dist, double& max_dist) const;

  //: given a query_tau, return the whole geodesic path to the closest source.
  void I_get_gdt_path (const double input_tau, gdt_path* gdt_path) const;

  //: ====== split into two ======  
  void transfer_nextI (gdt_interval** IL, gdt_interval** IR, 
                       const double& tauVO, const double& split_tau);
    void _transfer_nextI_eLIL_eRIR (gdt_interval** IL, gdt_interval** IR);
    void _transfer_nextI_eLIL (gdt_interval** IL);
    void _transfer_nextI_eRIR (gdt_interval** IR);

      bool _transfer_1_nextI_on_eL_to_IL (gdt_interval** IL);
      bool _transfer_1_nextI_on_eR_to_IR (gdt_interval** IR);


  //: ====== getInfo ======
  virtual void getInfo (vcl_ostringstream& ostrm);
  
#if GDT_ALGO_F //============================================
protected:
  //: The children gdt_interval
  //  Needed for deleting sub-tree of propagation as a new updating comes
  //  in to a propagated tree of intervals.
  ///vcl_vector<gdt_interval*> nexts_;

  //: true: this I is associated with its reference edge, he->edge(), 
  //        i.e., need to maintain the both-way pointers
  bool b_attach_to_edge_;

public:
  bool b_attach_to_edge () const {
    return b_attach_to_edge_;
  }
  void _set_attach_to_edge (bool b) {
    b_attach_to_edge_ = b;
  }

#endif  //============================================

#if GDT_ALGO_I //============================================
protected:
  //: a flag: this interval is invalid but currently on the front queue.
  //          It should not be deleted in a local process and 
  //          should be deleted immediately when poped from the priority queue.
  //  ==0: not on the front
  //  ==1: on the front
  //  ==2: on the front, and to be deleted.
  int                 i_invalid_on_front_;

public:
  virtual int i_invalid_on_front () const {
    return i_invalid_on_front_;
  }
  virtual void set_invalid_on_front (int i) {
    i_invalid_on_front_ = i;
  }
#endif //============================================

};

// ###############################################################

class gdt_lsrc_interval : public gdt_interval
{
protected:
  dbmsh3d_gdt_vertex_3d* srcline_e_;
  double he_;
  double le_;

public:
  //: ====== Constructor/Destructor ======
  gdt_lsrc_interval (const dbmsh3d_halfedge* edge,
                     const dbmsh3d_gdt_vertex_3d* srcline_s, 
                     const dbmsh3d_gdt_vertex_3d* srcline_e, 
                     const double stau, const double etau,
                     const gdt_interval* prev,
                     const double Hs, const double Ls,
                     const double He, const double Le);
};

// ###############################################################

void tie_prevI_nextI (gdt_interval* pI, gdt_interval* nI);
void untie_prevI_nextI (gdt_interval* pI, gdt_interval* nI);

void tie_psrcV_nextI (dbmsh3d_gdt_vertex_3d* v, gdt_interval* nI);
void untie_psrcV_nextI (dbmsh3d_gdt_vertex_3d* v, gdt_interval* nI);

//: This is needed only for the 'hack' of interval-based propagation!
//: if the i_invalid_on_front==0, delete it
//  else, set the flag to 2, and it will be deleted when poped.
inline bool _delete_interval_handle_front (gdt_interval* I) 
{
#if GDT_ALGO_I
  if (I->i_invalid_on_front()==0) {
    delete I;
    return true;
  }
  else {
    I->set_invalid_on_front (2);
    return false;
  }
#else
  delete I;
  return true;
#endif

}

vgl_point_2d<double> Ib_coord_to_Ia (const gdt_interval* Ia, const gdt_interval* Ib,
                                     const double& thetav, const vgl_point_2d<double>& Pb);

#endif



