// This is dbknee/dbknee_coord.h
#ifndef dbknee_coord_h_
#define dbknee_coord_h_

//:
// \file
// \brief A class for a coordinate system on a knee joint
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date Oct 2, 2006
//
// \verbatim
//  Modifications
// \endverbatim


#include <vbl/vbl_ref_count.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_cylinder.h>
#include <vcsl/vcsl_spatial_sptr.h>
#include <dbmsh3d/dbmsh3d_mesh.h>


// ============================================================================
// dbknee_coord_base
// ============================================================================

//: Base class for knee coordinate system
class dbknee_coord_base : public vbl_ref_count
{
public:
  // --------------------------------------------------------------------------
  // CONSTRUCTORS/DESTRUCTORS/INITIALIZATION
  // --------------------------------------------------------------------------
  
  //: constructor
  dbknee_coord_base(){};

  //: destructor
  virtual ~dbknee_coord_base(){};

  //: write info of the dbskbranch to an output stream
  virtual void print(vcl_ostream & os){};

protected:
  
};


// ============================================================================
// dbknee_cylinder_based_coord
// ============================================================================

//: The knee coordinate system that uses a cylinder to fit to the anterior
// portion of the cartilage
class dbknee_cylinder_based_coord : public vbl_ref_count
{
public:
  // --------------------------------------------------------------------------
  // CONSTRUCTORS/DESTRUCTORS
  // --------------------------------------------------------------------------

  //: constructor
  dbknee_cylinder_based_coord(){ this->init_default(); };

  //: destructor
  virtual ~dbknee_cylinder_based_coord(){};

  // --------------------------------------------------------------------------
  // DATA ACCESS
  // --------------------------------------------------------------------------
  
  //: Get and set the point set
  dbmsh3d_mesh* point_set() const {return this->point_set_; }
  void set_point_set(dbmsh3d_mesh* pt_set) {this->point_set_ = pt_set; }

  dbmsh3d_mesh& cropped_mesh() { return this->cropped_mesh_; }

  //: Get and set the box used to crop the original mesh
  vgl_box_3d<double > cropping_box() const {return this->cropping_box_; }
  void set_cropping_box(const vgl_box_3d<double >& box )
  { this->cropping_box_ = box; }

  //: Get and Set the cylinder
  vgl_cylinder<double > cylinder() const {return this->cylinder_; }
  void set_cylinder(const vgl_cylinder<double >& cylinder)
  { this->cylinder_ = cylinder; }

  //: Get and Set the notch point
  vgl_point_3d<double > notch_point() const {return this->notch_point_; }
  void set_notch_point(const vgl_point_3d<double >& notch_point)
  { this->notch_point_ = notch_point; }

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




  //: Get and Set the coordinate system
  vcsl_spatial_sptr coord_system() const {return this->coord_system_; }
  void set_coord_system(const vcsl_spatial_sptr& cs)
  { this->coord_system_ = cs; }

  // --------------------------------------------------------------------------
  // UTILITIES
  // --------------------------------------------------------------------------


  //: default coordinate system
  void init_default();

  

  //: Build the coordinate system from input data
  void build();

  //: Compute (local) z-coodinates of band centroids
  void compute_z_of_band_centroids();

  //: write info of the coordinate system to an output stream
  virtual void print(vcl_ostream & os);

  //: read from a cylinder-based coordinate system file
  // Example cs file:
  // origin_x 91.6919
  // origin_y -74.2875
  // origin_z -69.6523
  // x_axis_x -0.831151
  // x_axis_y -0.55549
  // x_axis_z -0.0248692
  // y_axis_x -0.55577
  // y_axis_y 0.831317
  // y_axis_z 0.00567378
  // z_axis_x 0.0175225
  // z_axis_y 0.0185373
  // z_axis_z -0.999675
  // cylinder_radius 17.0202
  // cylinder_length 66.2678
  // fitting_error 2.21707
  bool load_from_cs_file(const vcl_string& cs_file);

  //: convert from local coordinate to world coordinate
  vgl_point_3d<double > local_to_wcs(const vgl_point_3d<double >& pt) const;

  //: convert local cylinderical coordinate to world coordinate
  vgl_point_3d<double > local_cyl_to_wcs(double radius, double theta, double z);

  //: convert from world coordinate to local coordinate
  vgl_point_3d<double > wc_to_local(const vgl_point_3d<double >& pt) const;

  //: convert from world coordinate to local cylindrical coordinate
  void wc_to_local_cyl(const vgl_point_3d<double >& pt, 
    double& radius, double& theta, double& z) const;

  //: convert in-place on a mesh from world coordinate to local coordinate
  void wc_to_local(dbmsh3d_mesh* mesh) const;

  //: crop a mesh using cylindrical coordinates
  // always rotate CCW to go from theta_min to theta_max, regardless of value
  // criteria (not exact but should work most of the time):
  // if all vertices of a face is inside the cropping region then
  // the face is inside the region
  bool crop_mesh_cylindrical(dbmsh3d_mesh& source_mesh, 
    dbmsh3d_mesh& new_mesh,
    double z_min, double z_max,
    double theta_min, double theta_max,
    double r_min = 0, double r_max = 1e100);



  // --------------------------------------------------------------------------
  // SUPPORT FUNCTIONS
  // --------------------------------------------------------------------------

  //: build the cylinder
  bool build_cylinder();

  //: build cartesian coordinate system from the cylinder and notch point
  bool build_coord_system();

  //: Set the x-axis
  void set_x_axis(const vgl_vector_3d<double >& x_axis)
  { this->x_axis_ = normalized(x_axis); }

  
  //: Set the y-axis
  void set_y_axis(const vgl_vector_3d<double >& y_axis)
  { this->y_axis_ = normalized(y_axis); }

  
  //: Set the z-axis
  void set_z_axis(const vgl_vector_3d<double >& z_axis)
  { this->z_axis_ = normalized(z_axis); }

  //: Get and set the fitting error
  double fitting_error() const {return this->fitting_error_; }
  void set_fitting_error(double error)
  { this->fitting_error_ = error; }

  //: Get and set the z-coordinates of the centroids of the two bands
  double z_top_band_centroid() const {return this->z_band_centroid_[0]; }
  void set_z_top_band_centroid(double z)
  { this->z_band_centroid_[0] = z; }

  double z_bot_band_centroid() const {return this->z_band_centroid_[1]; }
  void set_z_bot_band_centroid(double z)
  { this->z_band_centroid_[1] = z; }


  // --------------------------------------------------------------------------
  // MEMBER VARIABLES
  // --------------------------------------------------------------------------
protected:
  dbmsh3d_mesh* point_set_;
  dbmsh3d_mesh cropped_mesh_;
  vgl_box_3d<double > cropping_box_;
  vgl_point_3d<double > notch_point_;
  vgl_cylinder<double > cylinder_;
  vcsl_spatial_sptr coord_system_;

  vgl_point_3d<double > origin_;
  vgl_vector_3d<double > x_axis_;
  vgl_vector_3d<double > y_axis_;
  vgl_vector_3d<double > z_axis_;

  // z-coordinates of the centroids of the two anterior bands
  // TOP : 0
  // BOTTOM : 1
  double z_band_centroid_[2];

  double fitting_error_;
};


// ============================================================================
// dbknee_cylinder_based_coord_params
// ============================================================================

class dbknee_cylinder_based_coord_params
{
public:
  dbknee_cylinder_based_coord_params(const vgl_box_3d<double >& bounding_box,
    const vcl_map<vcl_string, double >& param_list);
  ~dbknee_cylinder_based_coord_params(){};

  // variables:
  vgl_box_3d<double > cropping_box;
  double dx;
  double dy;
  double dz;
  vgl_point_3d<double > notch_point;
  
};

//: Read a cropping parameter from a parameter file
void dbknee_read_coord_param_file(const vcl_string& param_file,
                                  vcl_map<vcl_string, double >& param_list
                                  );

//: Read a coordinate system file and overwrite parameters in a coordinate system
void dbknee_read_cs_file(const vcl_string& cs_file, dbknee_cylinder_based_coord& cs);

//: Write the local coordinates of a point set to file
void dbknee_compute_write_local_coords_to_file(const dbknee_cylinder_based_coord& cs,
                                               dbmsh3d_mesh* pt_set,
                                               const vcl_string& outfile);

//: Extract the regions of interest on the knee cartilage
void dbknee_compute_cartilage_regions(const dbknee_cylinder_based_coord& cs,
           vcl_vector<vcl_vector<vgl_point_3d<double > > >& top_regions,
           vcl_vector<vcl_vector<vgl_point_3d<double > > >& bot_regions);

//: Separate the cartilage mesh into two separate top and bottom surfaces.
// given the axis of a cylinder
// A face (f) is consider in the inner surface if a ray starting from and orthogonal to
// the cylinder axis pointing toward (f) hits another face after hitting (f)
void dbknee_separate_inner_outer_surfaces(dbmsh3d_mesh& mesh,
                                          const vgl_point_3d<double >& axis_origin,
                                          const vgl_vector_3d<double >& axis_dir,
                                          dbmsh3d_mesh& inner_mesh,
                                          dbmsh3d_mesh& outer_mesh);


#endif // dbknee/dbknee_coord.h


