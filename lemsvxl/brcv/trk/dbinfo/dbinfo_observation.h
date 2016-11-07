// This is brl/bseg/dbinfo/dbinfo_observation.h
#ifndef dbinfo_observation_h_
#define dbinfo_observation_h_
//---------------------------------------------------------------------
//:
// \file
// \brief the base class for observations
//
// \author
//  J.L. Mundy - April 8, 2005
//
// \verbatim
//  Modifications
//   O. C. Ozcanli - Aug 01, 2006 added image_cropped() method
// \endverbatim
//
//-------------------------------------------------------------------------
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vbl/vbl_ref_count.h>
#include <vcl_cassert.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_pixel_format.h>
#include <dbinfo/dbinfo_feature_data_base_sptr.h>
#include <dbinfo/dbinfo_feature_base.h>
#include <dbinfo/dbinfo_region_geometry.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <brip/brip_roi.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
//forward declare the sub-classes
class dbinfo_intensity_feature;
class dbinfo_gradient_feature;

class dbinfo_observation : public vbl_ref_count
{

 public:
  //: default not sensible
  dbinfo_observation():
    margin_(0), current_frame_(0), geom_(0), image_cached_(0), image_crp_cached_(0),
    features_(vcl_vector<dbinfo_feature_base_sptr>()){};

  //: Constructors/destructor
  //: no current chached data in the features, just prior observations
  dbinfo_observation(dbinfo_region_geometry_sptr const& geom,
                     vcl_vector<dbinfo_feature_base_sptr> const& features);


  //: constructor from polygons
  //: features cache data from the current frame, represented by "image"
  dbinfo_observation(const unsigned frame,
                     vil_image_resource_sptr const& image,
                     vcl_vector<vsol_polygon_2d_sptr> const& polys,
                     vcl_vector<dbinfo_feature_base_sptr> const& features);

  //: constructor from a single polygon
  //: features cache data from the current frame, represented by "image"
  dbinfo_observation(const unsigned frame,
                     vil_image_resource_sptr const& image, 
                     vsol_polygon_2d_sptr const& poly,
                     vcl_vector<dbinfo_feature_base_sptr> const& features);

  //: constructors without features initially specified
  //: constructor from multiple polygons
  dbinfo_observation(const unsigned frame,
                     vil_image_resource_sptr const& image,
                     vcl_vector<vsol_polygon_2d_sptr> const& polys);

  //: constructor from a single polygon
  dbinfo_observation(const unsigned frame,
                     vil_image_resource_sptr const& image, 
                     vsol_polygon_2d_sptr const& poly);

  //: constructors with features automatically created according to specs
  //: constructor from multiple polygons
  dbinfo_observation(const unsigned frame,
                     vil_image_resource_sptr const& image,
                     vcl_vector<vsol_polygon_2d_sptr> const& polys,
                     bool intensity_info,
                     bool gradient_info,
                     bool color_info);


  //: constructor from a single polygon
  dbinfo_observation(const unsigned frame,
                     vil_image_resource_sptr const& image, 
                     vsol_polygon_2d_sptr const& poly,
                     bool intensity_info,
                     bool gradient_info,
                     bool color_info);



  //:default destructor
  ~dbinfo_observation(){}

  //:set the geometry for the feature
  void set_geometry(dbinfo_region_geometry_sptr const& geom)
    {geom_ = geom;}

  //:set the data for the feature
  inline void set_features(vcl_vector<dbinfo_feature_base_sptr> const& features)
    {
      features_ = features;
      this->set_margin();
      if(margin_)
        ex_roi_ = new brip_roi(*(geom_->roi()), (float)margin_);
      else ex_roi_ = geom_->roi();
    }

  //:add a polygon to the observation geometry 
  void add_poly(vsol_polygon_2d_sptr const& poly)
    {assert(geom_); geom_->add_poly(poly);} 

  //:get the geometry for the observation
  dbinfo_region_geometry_sptr geometry(){return geom_;}

  //:get the features for the observation
  vcl_vector<dbinfo_feature_base_sptr> features(){return features_;}


  //:the frame for which cached data was obtained
  unsigned frame(){return current_frame_;}

  //:the processing margin
  unsigned margin(){return margin_;}

  //scan the image data into the feature list from the current frame
  virtual bool scan(const unsigned frame, vil_image_resource_sptr const& image);

  //:clear the store of each feature
  inline void clear_data()
    {for(vcl_vector<dbinfo_feature_base_sptr>::iterator fit = features_.begin();
         fit != features_.end(); ++fit) (*fit)->clear_data();}

  //:the rating of this observation according to some context, e.g. track compatibility
  void set_score(const float score)
    {score_ = score;}

  //////: returns the clip of the observation 
  //vil_image_resource_sptr get_clip_image();

  //bool compute_clip_image(vil_image_resource_sptr const& image);
  //: returns the expanded roi of the observation 
  brip_roi_sptr ex_roi(){return ex_roi_;}

  float score() {return score_;}

  //:Documentation concerning the observation
  void set_doc(vcl_string const& doc){doc_ = doc;}

  vcl_string doc(){return doc_;}

  //:Is intensity information channel used?
  bool intensity_info();

  //:Is gradient information channel used?
  bool gradient_info();

  //:Is color information channel used?
  bool color_info();

  //:Print information about self
  virtual void print(vcl_ostream& os = vcl_cout) const;

  //:Construct an image of the observation
  vil_image_resource_sptr image(bool background_noise = true);

  //:Construct an image of the observation
  // cropped with respect to the geometry
  vil_image_resource_sptr image_cropped(bool background_noise = true);

  //:Construct an image of the bounding box of 
  // the observation
  // cropped with respect to the geometry
  vil_image_resource_sptr obs_snippet(){return observation_snippet;}

  void set_obs_snippet(vil_image_resource_sptr & os){observation_snippet=os;}

  void set_edge_points(vcl_vector<vgl_point_2d<double> > edgeps);
  void set_edge_dirs(vcl_vector<double> edgeds);
  
  vcl_vector<vgl_point_2d<double> >  get_edge_points(){return edgepoints;}
  vcl_vector<double> get_edge_dirs(){return edgedirs;}

  //-----------------------

  //:  BINARY I/O METHODS |
  //-----------------------

  //: Serial I/O format version
  virtual unsigned version() const {return 2;}

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const {return "dbinfo_observation";}

  //: determine if this is the given class
  virtual bool is_class(vcl_string const& cls) const
    { return cls==is_a();}
  
  //: Binary save self to stream.
  virtual void b_write(vsl_b_ostream &os) const ;

  //: Binary load self from stream.
  virtual void b_read(vsl_b_istream &is);


  void set_margin(unsigned m);
  //: an image roi with appropriate processing margin

  void compute_roi();

 protected:

  //===== utility functions =====

  //:points referenced to chip coordinates rather than global coordinates
  void image_to_roi(vcl_vector<vgl_point_2d<unsigned> >& points,
                    vcl_vector<bool>& valid);
  //:image processing margin
  void set_margin();

  //:set the observation id
  void set_id(const unsigned id){id_ = id;}

  //:get the observation id
  unsigned id(){return id_;}

  //====== members ======
  //: the observation unique identiy 
  unsigned id_;
  //: the observation score according to some context
  float score_;

  //: A documentation string to describe the observation
  vcl_string doc_;

  //: the margin around the extracted ROI to support processing kernels
  unsigned margin_;
  //: the expanded roi
  brip_roi_sptr ex_roi_;
  //:the current frame
  unsigned current_frame_;
  //:The region geometry 
  dbinfo_region_geometry_sptr geom_;
  //:The set of features used by the observation
  vcl_vector<dbinfo_feature_base_sptr> features_;
  
  //: cache images for fast access
  vil_image_resource_sptr image_cached_;
  vil_image_resource_sptr image_crp_cached_;
  vil_image_resource_sptr observation_snippet;

  vcl_vector<vgl_point_2d<double> > edgepoints;
  vcl_vector<double > edgedirs;
};
//: helper function
//: given the original image it crops the snippet and stores it.
bool compute_observation_snippet(dbinfo_observation * obs,vil_image_resource_sptr &im);
inline void vsl_b_read(vsl_b_istream &is, dbinfo_observation & ob)
{ob.b_read(is);}


inline void vsl_b_write(vsl_b_ostream &os, const dbinfo_observation & ob)
{ob.b_write(os);}


inline void vsl_b_write(vsl_b_ostream &os, dbinfo_observation const* ob)
{
  //Don't allow writing for null feature data
  assert(ob);
  const dbinfo_observation& cob = *ob;
  vsl_b_write(os, cob);
}

//: Binary load dbinfo_feature_data from stream.
inline void vsl_b_read(vsl_b_istream &is, dbinfo_observation* &ob)
{
  delete ob;
  ob = new dbinfo_observation();
  vsl_b_read(is, *ob);
}

inline vcl_ostream &operator<<(vcl_ostream &os, dbinfo_observation const& ob)
{
  ob.print(os);
  return os;
}
#if 0
inline vcl_ostream &operator<<(vcl_ostream &os, dbinfo_observation const* ob)
{
  if (ob)
    os << *ob;
  else
    os << "NULL Feature \n";
  return os;
}
#endif
inline void vsl_print_summary(vcl_ostream& os, dbinfo_observation const*  ob)
{os << ob;}

#include <dbinfo/dbinfo_observation_sptr.h>


#endif // dbinfo_observation_h_
