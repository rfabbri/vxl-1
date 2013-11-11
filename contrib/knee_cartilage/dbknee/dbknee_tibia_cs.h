// This is dbknee/dbknee_tibia_cs.h
#ifndef dbknee_tibia_cs_h_
#define dbknee_tibia_cs_h_

//:
// \file
// \brief A class for a coordinate system on a tibia cartilage
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date June 7, 2007
//
// \verbatim
//  Modifications
// \endverbatim


#include <vbl/vbl_ref_count.h>
//#include <vgl/vgl_box_3d.h>
//#include <vgl/vgl_cylinder.h>
//#include <vcsl/vcsl_spatial_sptr.h>
#include <dbmsh3d/dbmsh3d_mesh.h>


// ============================================================================
// dbknee_tibia_cs
// ============================================================================

//: The knee coordinate system that use the moment of inertia axis
class dbknee_tibia_cs : public vbl_ref_count
{
public:
  // CONSTRUCTORS/DESTRUCTORS -------------------------------------------------

  //: constructor
  dbknee_tibia_cs(){};

  //: destructor
  virtual ~dbknee_tibia_cs(){};

  // DATA ACCESS --------------------------------------------------------------
  
  //: Get and set the point set
  dbmsh3d_mesh* mesh() const {return this->mesh_; }
  void set_mesh(dbmsh3d_mesh* mesh) {this->mesh_ = mesh; }

  //: Get and Set the origin
  vgl_point_3d<double > origin() const {return this->origin_; }
  void set_origin(const vgl_point_3d<double >& pt) 
  { this->origin_ = pt; }
  
  //: Get the x_axis
  vgl_vector_3d<double > x_axis() const {return this->x_axis_; }

  //: Get the y_axis
  vgl_vector_3d<double > y_axis() const {return this->y_axis_; }

  //: Get the z_axis
  vgl_vector_3d<double > z_axis() const {return this->z_axis_; }



  // UTILITIES ----------------------------------------------------------------

  ////: default coordinate system
  //void init_default();

  

  //: Build the coordinate system from input data
  void build();

  //: write info of the coordinate system to an output stream
  virtual void print(vcl_ostream & os);

  bool load_from_cs_file(const vcl_string& cs_file);

  //: convert from local coordinate to world coordinate
  vgl_point_3d<double > local_to_wcs(const vgl_point_3d<double >& pt) const;

  //: convert from world coordinate to local coordinate
  vgl_point_3d<double > wc_to_local(const vgl_point_3d<double >& pt) const;

  //: convert in-place on a mesh from world coordinate to local coordinate
  void wc_to_local(dbmsh3d_mesh* mesh) const;

  


  // --------------------------------------------------------------------------
  // SUPPORT FUNCTIONS
  // --------------------------------------------------------------------------

  //: Set the x-axis
  void set_x_axis(const vgl_vector_3d<double >& x_axis)
  { this->x_axis_ = normalized(x_axis); }

  
  //: Set the y-axis
  void set_y_axis(const vgl_vector_3d<double >& y_axis)
  { this->y_axis_ = normalized(y_axis); }

  
  //: Set the z-axis
  void set_z_axis(const vgl_vector_3d<double >& z_axis)
  { this->z_axis_ = normalized(z_axis); }

  // --------------------------------------------------------------------------
  // MEMBER VARIABLES
  // --------------------------------------------------------------------------
protected:
  dbmsh3d_mesh* mesh_;

  vgl_point_3d<double > origin_;
  vgl_vector_3d<double > x_axis_;
  vgl_vector_3d<double > y_axis_;
  vgl_vector_3d<double > z_axis_;
};


//// ============================================================================
//// dbknee_tibia_cs_params
//// ============================================================================
//
//class dbknee_tibia_cs_params
//{
//public:
//  dbknee_tibia_cs_params(const vgl_box_3d<double >& bounding_box,
//    const vcl_map<vcl_string, double >& param_list);
//  ~dbknee_cylinder_based_coord_params(){};
//
//  // variables:
//  vgl_box_3d<double > cropping_box;
//  double dx;
//  double dy;
//  double dz;
//  vgl_point_3d<double > notch_point;
//  
//};
//






#endif // dbknee/dbknee_tibia_cs.h


