// This is shp/dbsksp/dbsksp_xshock_fragment.h
#ifndef dbsksp_xshock_fragment_h_
#define dbsksp_xshock_fragment_h_

//:
// \file
// \brief A class for shock fragment
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu )
// \date Sep 22, 2008
//
// \verbatim
//  Modifications
// \endverbatim


#include <vbl/vbl_ref_count.h>
#include <vgl/vgl_box_2d.h>
#include <dbsksp/dbsksp_xshock_node_descriptor.h>
#include <dbgl/algo/dbgl_biarc.h>
#include <vgl/vgl_vector_2d.h>
#include <vnl/vnl_vector.h>
#include <dbsksp/dbsksp_xshock_fragment_sptr.h>


// =============================================================================
// dbsksp_xshock_fragment
// =============================================================================

//: A class to represent a shock branch - both geometry and dynamics
class dbsksp_xshock_fragment : public vbl_ref_count
{
public:
  enum bnd_side
  {
    LEFT = 0,
    RIGHT = 1,
  };

  // CONSTRUCTORS/DESTRUCTORS/INITIALIZATION ===================================
  
  //: constructor
  dbsksp_xshock_fragment(){};

  //: constructor
  dbsksp_xshock_fragment(const dbsksp_xshock_node_descriptor& start, 
    const dbsksp_xshock_node_descriptor& end): start_(start), end_(end){}

  //: destructor
  virtual ~dbsksp_xshock_fragment(){};
  
  // DATA ACCESS ===============================================================
  
  //: Starting xshock descriptor
  dbsksp_xshock_node_descriptor start() const {return this->start_; }
  dbsksp_xshock_node_descriptor& start() {return this->start_; }

  //: Set the starting xshock node descriptor
  void set_start(const dbsksp_xshock_node_descriptor& start)
  { this->start_ = start; }

  //: Ending xshock descriptor
  dbsksp_xshock_node_descriptor end() const { return this->end_; }
  dbsksp_xshock_node_descriptor& end() { return this->end_; }

  //: Set the ending xshock node descriptor
  void set_end(const dbsksp_xshock_node_descriptor& end)
  { this->end_ = end; }


  //: start shock point
  vgl_point_2d<double > start_shock_pt() const
  {
    return this->start_.pt();
  }


  //: end shock point
  vgl_point_2d<double > end_shock_pt() const
  {
    return this->end_.pt();
  }


  //: Interpolate the left boundary as a biarc
  dbgl_biarc bnd_left_as_biarc() const;

  //: Interpolate the right boundary as a biarc
  dbgl_biarc bnd_right_as_biarc() const;

  //: Interpolate a boundary contour using a biarc
  dbgl_biarc bnd_as_biarc(bnd_side side) const
  {
    return (side == LEFT) ? this->bnd_left_as_biarc() :
      this->bnd_right_as_biarc();
  }


  // UTILITY FUNCTIONS =========================================================
  
  //: Compute area of the shape fragment
  double area() const;

  //: Return true if this fragment is legal, i.e. so self-intersection
  bool is_legal() const;

  bool is_legal_new() const;

  bool is_legal_singular_arc_frag(vgl_point_2d< double > start_shock_pt,
								  vgl_point_2d< double > start_pt_L, double start_angle_L, double L_L, vgl_point_2d< double > center_L, double r_L,
								  vgl_point_2d< double > start_pt_R, double start_angle_R, double L_R, vgl_point_2d< double > center_R, double r_R,
								  vgl_point_2d< double > &end_pt_L, double &end_angle_L, double &actual_L_L,
								  vgl_point_2d< double > &end_pt_R, double &end_angle_R, double &actual_L_R, vgl_point_2d< double > &end_shock_pt) const;
  //: Return true if this fragment has swallowtail on a boundary side
  bool bnd_has_swallowtail(bnd_side side) const
  {
    return (side == LEFT) ? 
      this->bnd_has_swallowtail_left() : bnd_has_swallowtail_right();
  }

  //: Return true if the left boundary has swallowtail
  bool bnd_has_swallowtail_left() const;

  //: Return true if the left boundary has swallowtail
  bool bnd_has_swallowtail_right() const;

  //: Compute bounding box of the fragment
  vgl_box_2d<double > compute_bounding_box() const;

  //: Translate the fragment
  void translate(const vgl_vector_2d<double >& v);

  //: Rotate the fragment
  void rotate(const vgl_point_2d<double >& center, double angle_in_radian);

  //: Rotate, scale, then translate the fragment
  dbsksp_xshock_fragment_sptr rot_scale_trans(const vgl_point_2d<double >& rot_center, 
    double rot_angle_in_radian = 0, 
    double scale_up_factor = 1, 
    const vgl_vector_2d<double >& translate_vec = vgl_vector_2d<double >(0, 0));



  // I/O =======================================================================
  
  //: write info of the dbskbranch to an output stream
  virtual void print(vcl_ostream & os){};

protected:
  // starting and ending extrinsic shock descriptor
  dbsksp_xshock_node_descriptor start_;
  dbsksp_xshock_node_descriptor end_;

  // support functions

  //: compute fragment area by approximating it with a polygon
  double area_approx_w_polygon(int num_pts) const;
};



#endif // shp/dbsksp/dbsksp_xshock_fragment.h


