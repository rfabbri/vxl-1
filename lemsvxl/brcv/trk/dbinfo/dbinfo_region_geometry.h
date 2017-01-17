// This is brl/bseg/dbinfo/dbinfo_region_geometry.h
#ifndef dbinfo_region_geometry_h_
#define dbinfo_region_geometry_h_
//---------------------------------------------------------------------
//:
// \file
// \brief the base class for tracking feature data
//
// \author
//  J.L. Mundy - March 20, 2005
//
// \verbatim
//  Modifications
//   O. C. Ozcanli - January 13, 2006 Added centroid() method
//   O. C. Ozcanli - January 23, 2006 added one line to constructor:
//                                    dbinfo_region_geometry(const dbinfo_region_geometry & rhs,
//                                    vgl_h_matrix_2d<float> const& trans,
//                                    const float thresh = 0.9)
//                                    so that it updates sufficient_points_ 
//   O. C. Ozcanli - January 23, 2006 changed line 113 in .cxx from
//                                    if(x>=0||x<image_cols_||y>0||y<image_rows_)
//                                    to
//                                    if(x>=0 && x<image_cols_ && y>0 && y<image_rows_)
//                        
//   
// \endverbatim
//
//-------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vbl/vbl_ref_count.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <brip/brip_roi.h>
#include <vcl_cassert.h>

class dbinfo_region_geometry : public vbl_ref_count
{

 public:
  dbinfo_region_geometry() : points_valid_(false), sufficient_points_(false),
    image_cols_(0), image_rows_(0){} // not a sensible constructor

  //: the fraction of points inside the image for valid operation
  float fraction_inside_thresh_;

  //: Constructors/destructor
  dbinfo_region_geometry(const unsigned image_cols, const unsigned image_rows,
                         vcl_vector<vsol_polygon_2d_sptr> const& track_boundary,
                         const float thresh = 0.9);

  //: From a single polygon
  dbinfo_region_geometry(const unsigned image_cols, const unsigned image_rows,
                         vsol_polygon_2d_sptr const& track_boundary,
                         const float thresh = 0.9);


  //:Transform the region, producing a new region
  dbinfo_region_geometry(const dbinfo_region_geometry & rhs,
                         vgl_h_matrix_2d<float> const& trans,
                         const float thresh = 0.9);

  //:Copy constructor
  dbinfo_region_geometry(const dbinfo_region_geometry & rhs);

  ~dbinfo_region_geometry(){}

  //:Number of image columns
  unsigned cols() const {return image_cols_;}

  //:Number of image rows
  unsigned rows() const {return image_rows_;}

  //:Number of polygons
  unsigned n_polys() const{return boundaries_.size();}

  //:get a poly 
  inline vsol_polygon_2d_sptr poly(const unsigned index) const
    {assert(index<boundaries_.size());return boundaries_[index];} 

  //:Get the total number of points in the region
  inline unsigned size() 
    {if(!points_valid_) this->compute_points(); return points_.size();}

  //:Sufficient number of points inside the region
  bool sufficient_points() const {return sufficient_points_;}

  inline vgl_point_2d<float> point(unsigned index)
    {assert(index<size());  return points_[index];}

  inline bool point_mask(unsigned index)
    {assert(index<size());  return point_masks_[index];}

  //: Get the full sample point set and valid mask
  inline vcl_vector<vgl_point_2d<float> > points(){return points_;}
  inline vcl_vector<bool> masks(){return point_masks_;}

  //:The centroid of the region
  vsol_point_2d_sptr cog() ;

  //: get the overall centroid weighted by area
  vsol_point_2d_sptr centroid();

  //:The diameter of the region
  double diameter();

  //:A bounding region of interest for image processing.
  brip_roi_sptr roi() const;

  //: Add a polygon to the boundary
  void add_poly(vsol_polygon_2d_sptr const& poly){boundaries_.push_back(poly);
  points_valid_ = false;}

  //:Transform in place
  bool transform(vgl_h_matrix_2d<float> const& trans);
  
  void print(vcl_ostream &strm=vcl_cout);
  
  friend vcl_ostream &operator<<(vcl_ostream &, dbinfo_region_geometry const&);
  friend vcl_ostream &operator<<(vcl_ostream &, dbinfo_region_geometry const*);

  // Binary I/O------------------------------------------------------------------

  //: Return a platform independent string identifying the class
  vcl_string is_a() const {return vcl_string("dbinfo_region_geometry");}
  //: Return true if the argument matches the string identifying the class or any parent class
  bool is_class(vcl_string const& cls) const
    { return cls==is_a();}

  //: Return IO version number;
  short version() const {return 1;}

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  friend void vsl_b_read(vsl_b_istream &is, dbinfo_region_geometry* &rg);

 protected:
  //:internal methods
  void adjust_image_bounds();
  void compute_points() ;
  void compute_sufficient();
  bool points_valid_; //!< flag indicating points have been computed 
  bool sufficient_points_; //!< sufficient number of points inside the image
  unsigned image_cols_;//!< number of columns in image
  unsigned image_rows_;//!< number of rows in image
  vcl_vector<bool> point_masks_;//!< false if point is outside image
  vcl_vector<vgl_point_2d<float> > points_;//!< sample pixel locations 
  vcl_vector<vsol_polygon_2d_sptr> boundaries_;//!< polygonal boundaries
  };

// inline functions
inline void vsl_b_read(vsl_b_istream &is, dbinfo_region_geometry & rg)
{rg.b_read(is);}

inline void vsl_b_write(vsl_b_ostream &os, const dbinfo_region_geometry & rg)
{rg.b_write(os);}


//: Binary save dbinfo_region_geometry* to stream.
inline void vsl_b_write(vsl_b_ostream &os, const dbinfo_region_geometry* rg)
{
  //Don't allow null pointer
  assert(rg);
  rg->b_write(os);
}

//: Binary load dbinfo_region_geometry* from stream.
inline void vsl_b_read(vsl_b_istream &is, dbinfo_region_geometry* &rg)
{
  delete rg;
  rg = new dbinfo_region_geometry();
  rg->b_read(is);
}

inline vcl_ostream &operator<<(vcl_ostream &strm, dbinfo_region_geometry const& rg)
{
  dbinfo_region_geometry& non_const = const_cast<dbinfo_region_geometry&>(rg);
  non_const.print(strm);
  return strm;
}

inline vcl_ostream &operator<<(vcl_ostream &strm, dbinfo_region_geometry const* rg)
{
  if (rg)
    strm << *rg;
  else
    strm << "NULL dbinfo_region_geometry object\n";
  return strm;
}

//: Stream output operator for class pointer
inline void vsl_print_summary(vcl_ostream& os, dbinfo_region_geometry const* rg)
{
  os << rg;
}

#include <dbinfo/dbinfo_region_geometry_sptr.h>

#endif // dbinfo_region_geometry_h_
