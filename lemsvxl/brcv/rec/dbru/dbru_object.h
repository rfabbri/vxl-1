// This is brcv/rec/dbru/dbru_object.h
#ifndef dbru_object_h_
#define dbru_object_h_
//:
// \file
// \brief Class that holds attributes of vehicle objects
//
// \author Ozge C Ozcanli, (ozge@lems.brown.edu)
// \date 7/03/05
//      
// \verbatim
//  Modifications
//   Ozge C Ozcanli - Nov 22, 06 - Version 3
//                                 Added multiple instance as a member to the class 
//                                 so that OSL can be created for edgel based recognition as well
//  
// \endverbatim

#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_cassert.h>
#include <vbl/vbl_ref_count.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <dbru/dbru_label_sptr.h>
#include <vsl/vsl_binary_io.h>
#include <dbinfo/dbinfo_observation_sptr.h>

#include <dbru/dbru_multiple_instance_object_sptr.h>

//: class to hold attributes of vehicle objects in video
class dbru_object : public vbl_ref_count 
{

 public:

  dbru_object(const int video_id = -1,
              const int start_frame = -1,
              const int end_frame = -1);

  dbru_object(const dbru_object& other);
 ~dbru_object() {}

 //: set number of polygons per frame
 void set_polygon_per_frame(int poly_per_frame);

 //: set start frame
 void set_start_frame(int start_frame) { start_frame_ = start_frame; }

 //: set end frame
 void set_end_frame(int end_frame) { end_frame_ = end_frame; }

 int start_frame(void) { return start_frame_; }
 int end_frame(void) { return end_frame_; }

 //: add a polygon (polygons are added without regard to frames, user should keep such records 
 bool add_polygon(vsol_polygon_2d_sptr poly, dbru_label_sptr label); 

 //: remove the last added non-empty polygon and all the empty polygons before it
 int remove_last_polygon(void);

 //: null the prototype and observation pointers if they exist
 bool null_observation(unsigned i);

 //: replace an existing polygon with the current one (label stays the same)
 void set_polygon(unsigned int i, vsol_polygon_2d_sptr poly);

 //: replace an existing polygon and its label with the given ones
 bool set_polygon(unsigned int i, vsol_polygon_2d_sptr poly, dbru_label_sptr label);

 //: replace an existing polygon label with the given ones
 bool set_polygon_label(unsigned int i, dbru_label_sptr label);

 //: get the ith polygon in the list
 vsol_polygon_2d_sptr get_polygon(unsigned int i); 

 //: get the kth non-null polygon
 vsol_polygon_2d_sptr get_non_null_polygon(unsigned int k, unsigned int&i);

 //: get ith polygon's label
 dbru_label_sptr get_label(unsigned int i); 

 void write_xml(vcl_ostream& os);
 
 //: my simple reader for a very strict and limited output format for object instances
 //  as written out by write_xml method
 bool read_xml(vcl_istream& os);

 //: add an observation
 bool add_observation(dbinfo_observation_sptr obs); 

 //: get the ith observation in the list
 dbinfo_observation_sptr get_observation(unsigned int i); 

 //: replace an existing observation
 bool set_observation(unsigned int i, dbinfo_observation_sptr obs);

 //: get the observation with given documentation
 dbinfo_observation_sptr get_observation(vcl_string const& obs_doc); 

 //: replace an existing observation with given documentation
 bool set_observation(vcl_string const& obs_doc, dbinfo_observation_sptr obs);

 //: add an instance
 bool add_instance(dbru_multiple_instance_object_sptr ins); 

 //: replace an existing instance
 bool set_instances(unsigned int i, dbru_multiple_instance_object_sptr ins);

 //: get the ith instance in the list
 dbru_multiple_instance_object_sptr get_instance(unsigned int i); 

 //: remove the observation and corresponding polygon and label based on polygon_per_frame count
 void remove_observation(unsigned int i);

 //: clear the current observations only, do not touch polygons and labels
 void clear_only_observations() { observations_.clear(); }

 //: clear the current instances only, do not touch polygons and labels
 void clear_only_instances() { instances_.clear(); }

 //: get the observations size
 unsigned int n_observations() { return observations_.size(); }

 //: get the instances size
 unsigned int n_instances() { return instances_.size(); }

 //: get the polygons size
 unsigned int n_polygons() { return polygons_.size(); }

 //: get the labels size
 //  CAUTION: Number of labels should always be equal to number of multiple instances
 //           or number of observations 
 unsigned int n_labels() { return labels_.size(); }

 //:Documentation concerning the observation
  void set_doc(vcl_string const& doc){doc_ = doc;}

  vcl_string doc() {return doc_;}

  //: category the observation
  void set_category(vcl_string const& cat){category_ = cat;}

  vcl_string category() {return category_;}

  //:Print information about self
  virtual void print(vcl_ostream& os = vcl_cout) const;

  dbru_object& operator=(const dbru_object& right);


 //-----------------------
 //:  BINARY I/O METHODS |
 //-----------------------

 //: Serial I/O format version
 virtual unsigned version() const {return 3;}

 //: Return a platform independent string identifying the class
 virtual vcl_string is_a() const {return "dbru_object";}

 //: determine if this is the given class
 virtual bool is_class(vcl_string const& cls) const
   { return cls==is_a();}
  
 //: Binary save self to stream.
 virtual void b_write(vsl_b_ostream &os) const ;

 //: Binary load self from stream.
 virtual void b_read(vsl_b_istream &is);

 public:
  //
  // Parameter blocks and parameters
  //
  
  int video_id_;                               //!< id of the video this object is extracted from
  int start_frame_;                            //!< the first frame object appears 
  int end_frame_;                              //!< the last frame object appears
  vcl_string category_;                        //!< category name, this name should be repeated for all labels
  unsigned int polygon_cnt_;                   //!< number of polygonal contours associated to this object
  int polygon_per_frame_;                      //!< each frame might contain more than one polygon (e.g. vehicle and shadow contours)
  vcl_vector < dbru_label_sptr > labels_;         //!< labels of each polygonal contour (motion orientation, etc.)
  vcl_vector < vsol_polygon_2d_sptr > polygons_;  //!< vector of polygons, without regard to frames, user should keep track of frames

  //: A documentation string to describe the object (e.g. pickup1, car2)
  // this is another way of accessing objects and observations (via names determined by users)
  vcl_string doc_;
                              
  //: keep inner region features extracted from image for each polygon
  vcl_vector< dbinfo_observation_sptr > observations_;

  //: After tracking, additional info from multiple frames are kept in these multiple_instance_object
  //  They contain edgels fused into the middle frame from adjacent frames and
  //  an array of intensity images transformed onto middle frame (created via algos by Vishal Jain)
  vcl_vector< dbru_multiple_instance_object_sptr > instances_;

};


inline void vsl_b_read(vsl_b_istream &is, dbru_object & obj)
{obj.b_read(is);}


inline void vsl_b_write(vsl_b_ostream &os, const dbru_object & obj)
{obj.b_write(os);}

inline void vsl_b_write(vsl_b_ostream &os, dbru_object const* obj)
{
  //Don't allow writing for null feature data
  assert(obj);
  const dbru_object& cobj = *obj;
  vsl_b_write(os, cobj);
}

//: Binary load dbinfo_feature_data from stream.
inline void vsl_b_read(vsl_b_istream &is, dbru_object* &obj)
{
  delete obj;
  obj = new dbru_object();
  vsl_b_read(is, *obj);
}

#include <dbru/dbru_object_sptr.h>

inline vcl_ostream &operator<<(vcl_ostream &os, dbru_object const& fb)
{
  fb.print(os);
  return os;
}

inline void vsl_print_summary(vcl_ostream & os, const dbru_object* fbp)
{os << *fbp;}

bool read_objects_from_file(const char *filename, vcl_vector< dbru_object_sptr>& objects);
int read_poly_file(vcl_string const& poly_file, vcl_vector<vcl_vector< vsol_polygon_2d_sptr > >& frame_polys);
void read_ins_file(const char *filename, vcl_vector< vcl_vector<dbru_multiple_instance_object_sptr> >& frames);

#endif // dbru_object_h_
