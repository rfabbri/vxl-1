//---------------------------------------------------------------------
// This is brcv/rec/dbasn/dbasnh_hypg.h
//:
// \file
// \brief Definition of a hypergraph structure
//
// \author
//  Ming-Ching Chang - January 6, 2008
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------
#ifndef dbasnh_hypg_h_
#define dbasnh_hypg_h_

#include <vcl_iostream.h>
#include <vnl/vnl_math.h>
#include <vul/vul_printf.h>
#include <dbasn/dbasn_graph.h>

//: Graduated Assignment hypggraph structure
class dbasnh_hypg : public dbasn_graph
{
protected:
  //: the corners_[a][b][c] table for storing the 3rd-order corner (link-link) cost.
  //  the sizes in all dimensions are nN = nodes_.size().
  //  Similar to the links_[][] table for link cost.
  float***  corners_;

  //: number of corners (non-zero entries in the upper matrix of corners_[][][].
  int       n_corners_;

  double    corner_cost_max_;

public:  
  //####### Constructor/Destructor #######
  dbasnh_hypg () {
    corners_ = NULL;
    n_corners_ = 0;
    corner_cost_max_ = 0;
  }
  virtual ~dbasnh_hypg () {
    const unsigned int nN = nodes_.size();
    if (corners_ != NULL) {
      for (unsigned int i=0; i<nN; i++) {
        for (unsigned int j=0; j<nN; j++)
          delete []corners_[i][j];
        delete []corners_[i];
      }
      delete []corners_;
    }
  }

  //####### Data access functions ####### 
  float*** corners() {
    return corners_;
  }
  float corners (const unsigned int i, const unsigned int j, const unsigned int k) {
    assert (i < nodes_.size());
    assert (j < nodes_.size());
    assert (k < nodes_.size());
    assert (corners_[i][j][k] == corners_[k][j][i]);
    return corners_[i][j][k];
  }
  int n_corners() {
    return n_corners_;
  }  
  const double corner_cost_max () const {
    return corner_cost_max_;
  }

  //####### Graph connectivity query functions #######
  int count_max_corners ();
  void print_corner_cost ();

  //####### Graph connectivity modification functions #######
  void alloc_corners_angle ();

  void add_corner_angle (const unsigned int i, const unsigned int j, 
                         const unsigned int k, const float cost) {
    assert (cost != 0);
    assert (i < nodes_.size());
    assert (j < nodes_.size());
    assert (k < nodes_.size());    
    assert (links_[i][j] != 0);
    assert (links_[j][i] != 0);
    assert (links_[j][k] != 0);
    assert (links_[k][j] != 0);
    assert (vnl_math_isnan(cost) == false);

    //After adding virtual link, this duplication becomes possible!!
    ///assert (corners_[i][j][k] == 0);
    ///assert (corners_[k][j][i] == 0);
    if (corners_[i][j][k] == 0) {      
      assert (corners_[k][j][i] == 0);
      corners_[i][j][k] = cost;
      corners_[k][j][i] = cost;
      n_corners_++;
    }
    else {
      assert (corners_[k][j][i] != 0);
      vul_printf (vcl_cout, "  add corner[%d][%d][%d] = %f skipped, existing value %f!\n", i, j, k, cost, corners_[i][j][k]);
    }

    if (cost > corner_cost_max_)
      corner_cost_max_ = cost;
  }

  void normalize_corner_cost (const double max, const int verbose = 1);
};

class dbasnh_hypg_aug : public dbasnh_hypg
{
protected:
  AL_TYPE** links_type_;

  int** link_idx_;

  float***  corner_nf_;
  double    corner_nf_max_;

  float***  corner_grad_r_;
  double    corner_grad_r_max_;

public:
  //###### Constructor/Destructor ######
  dbasnh_hypg_aug () : dbasnh_hypg () {
    links_type_ = NULL;
    link_idx_ = NULL;
    corner_nf_ = NULL;
    corner_nf_max_ = 0;
    corner_grad_r_ = NULL;
    corner_grad_r_max_ = 0;
  }
  virtual ~dbasnh_hypg_aug () {
    if (links_type_ != NULL) {
      assert (nodes_.size() != 0);
      for (unsigned int i=0; i<nodes_.size(); i++)
        delete []links_type_[i];
      delete []links_type_;
    }
    
    if (link_idx_ != NULL) {
      assert (nodes_.size() != 0);
      for (unsigned int i=0; i<nodes_.size(); i++)
        delete []link_idx_[i];
      delete []link_idx_;
    }
    
    const unsigned int nN = nodes_.size();
    if (corner_nf_ != NULL) {
      for (unsigned int i=0; i<nN; i++) {
        for (unsigned int j=0; j<nN; j++)
          delete []corner_nf_[i][j];
        delete []corner_nf_[i];
      }
      delete []corner_nf_;
    }
    if (corner_grad_r_ != NULL) {
      for (unsigned int i=0; i<nN; i++) {
        for (unsigned int j=0; j<nN; j++)
          delete []corner_grad_r_[i][j];
        delete []corner_grad_r_[i];
      }
      delete []corner_grad_r_;
    }
  }

  //###### Data access functions ######

  dbasn_node_aug* N_aug (const int i) {
    return (dbasn_node_aug*) nodes_[i];
  }

  AL_TYPE** links_type () const {
    return links_type_;
  }
  AL_TYPE link_type (const unsigned int i, const unsigned int j) const {
    assert (i < nodes_.size());
    assert (j < nodes_.size());
    return links_type_[i][j];
  }
  void add_link_type (const unsigned int i, const unsigned int j, const AL_TYPE type) {
    //The virtual link type is NOT allowed here!
    assert (type == AL_TYPE_A3_RIB || type == AL_TYPE_A13_AXIAL || 
            type == AL_TYPE_DEGE_AXIAL);
    assert (i < nodes_.size());
    assert (j < nodes_.size());
    links_type_[i][j] = type;
    links_type_[j][i] = type;
  }
  //: allocate the links_type_[][] array according to the size of nodes_[].
  void alloc_links_type () {
    const unsigned int nN = nodes_.size();
    links_type_ = new AL_TYPE*[nN];
    for (unsigned int i = 0; i<nN; i++)
      links_type_[i] = new AL_TYPE[nN];

    for (unsigned int i = 0; i<nN; i++)
      for (unsigned int j = 0; j<nN; j++)
        links_type_[i][j] = AL_TYPE_BOGUS;
  }

  int** link_idx () const {
    return link_idx_;
  }
  int get_link_idx (const unsigned int i, const unsigned int j,
                    bool& flip) const {
    assert (i < nodes_.size());
    assert (j < nodes_.size());
    //Use the sign to indicate flipping of curve.
    flip = link_idx_[i][j] < 0;
    int idx_abs = (link_idx_[i][j]>0) ? link_idx_[i][j] : -link_idx_[i][j];
    return idx_abs - 1;
  }
  void add_link_idx (const unsigned int i, const unsigned int j, const int id) {
    assert (i < nodes_.size());
    assert (j < nodes_.size());
    link_idx_[i][j] = id + 1;
    link_idx_[j][i] = -id - 1;
  }
  void alloc_link_idx () {
    const unsigned int nN = nodes_.size();
    link_idx_ = new int*[nN];
    for (unsigned int i = 0; i<nN; i++)
      link_idx_[i] = new int[nN];

    for (unsigned int i = 0; i<nN; i++)
      for (unsigned int j = 0; j<nN; j++)
        link_idx_[i][j] = 0; //no link.
  }
  
  float*** corner_nf () {
    return corner_nf_;
  }
  float corner_nf (const unsigned int i, const unsigned int j, const unsigned int k) {
    assert (i < nodes_.size());
    assert (j < nodes_.size());
    assert (k < nodes_.size());
    assert (corner_nf_[i][j][k] == corner_nf_[k][j][i]);
    return corner_nf_[i][j][k];
  }
  const double corner_nf_max () const {
    return corner_nf_max_;
  }
  
  float*** corner_grad_r () {
    return corner_grad_r_;
  }
  float corner_grad_r (const unsigned int i, const unsigned int j, const unsigned int k) {
    assert (i < nodes_.size());
    assert (j < nodes_.size());
    assert (k < nodes_.size());
    assert (corner_grad_r_[i][j][k] == corner_grad_r_[k][j][i]);
    return corner_grad_r_[i][j][k];
  }
  const double corner_grad_r_max () const {
    return corner_grad_r_max_;
  }

  //####### Graph connectivity modification functions #######
  void alloc_corner_nf_grad_r ();
  
  void add_corner_nf (const unsigned int i, const unsigned int j, 
                      const unsigned int k, const float cost) {
    assert (i < nodes_.size());
    assert (j < nodes_.size());
    assert (k < nodes_.size());
    ///assert (links_[i][j] != 0);
    ///assert (links_[j][i] != 0);
    ///assert (links_[j][k] != 0);
    ///assert (links_[k][j] != 0);
    assert (vnl_math_isnan(cost) == false);
    corner_nf_[i][j][k] = cost;
    corner_nf_[k][j][i] = cost;

    if (cost > corner_nf_max_)
        corner_nf_max_ = cost;
  }
  void add_corner_grad_r (const unsigned int i, const unsigned int j, 
                          const unsigned int k, const float cost) {
    assert (i < nodes_.size());
    assert (j < nodes_.size());
    assert (k < nodes_.size());
    ///assert (links_[i][j] != 0);
    ///assert (links_[j][i] != 0);
    ///assert (links_[j][k] != 0);
    ///assert (links_[k][j] != 0);
    assert (vnl_math_isnan(cost) == false);
    corner_grad_r_[i][j][k] = cost;
    corner_grad_r_[k][j][i] = cost;

    if (cost > corner_grad_r_max_)
        corner_grad_r_max_ = cost;
  }

  float max_of_grad_r_max ();
  float max_of_grad_r_min ();
  float max_of_corner_a_max ();
  float max_of_corner_a_min ();
  void normalize_node_grad_r_max (const float max, const int verbose = 1);
  void normalize_node_grad_r_min (const float max, const int verbose = 1);
  void normalize_node_corner_a_max (const float max, const int verbose = 1);
  void normalize_node_corner_a_min (const float max, const int verbose = 1);

  void normalize_corner_nf_cost (const float max, const int verbose = 1);
  void normalize_corner_grad_r_cost (const float max, const int verbose = 1);

  double bound_box_radius_of_nodes ();

  void get_cen_N_aug (vgl_point_3d<double>& C);
};

#endif
