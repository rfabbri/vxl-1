// This is brcv/rec/dbru/dbru_facedb.h
#ifndef dbru_facedb_h_
#define dbru_facedb_h_
//---------------------------------------------------------------------
//:
// \file
// \brief The object signature library class 
//        
//        facedb:         a collection of faces , for each subject there is a number of faces
//
// \author
//  O. C. Ozcanli - Aug 13, 2006
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//---------------------------------------------------------------------
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_map.h>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_array_1d.h>
#include <vcl_cassert.h>
#include <vsl/vsl_binary_io.h>
//#include <dbinfo/dbinfo_observation_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vil/vil_image_resource_sptr.h>

class dbru_facedb : public vbl_ref_count
{
  //: Constructors/destructor
 public:
  dbru_facedb();
  ~dbru_facedb();

  // if this constructer is used all the prototypes will be added as a separate object with 
  // labels only containing category
  //dbru_facedb(vcl_vector<vcl_string> const& classes,
  //         vcl_vector<vcl_vector<dbinfo_observation_sptr> > const& prototypes
  //         );

  // normally this constructer should be used and objects should be appropriately labeled
  // e.g. via dbru_labeling_tool
  dbru_facedb(vcl_vector<vcl_vector<vil_image_resource_sptr> > const& faces
           );

  //: copy constructor
  dbru_facedb(const dbru_facedb& rhs);

  void clear();

  // add a set of images from a new subject
  void add_subject(vcl_vector<vil_image_resource_sptr> subject);
  // add a set of images to an existing subject
  void add_subject(vcl_vector<vil_image_resource_sptr> subject, unsigned i);

  //: get a particular subject
  vcl_vector<vil_image_resource_sptr>& get_subject(unsigned i);

  //: get a particular image of a particular subject in facedb
  vil_image_resource_sptr get_face(unsigned i, unsigned j);

  //:total number of subjects
  unsigned n_subjects() const { return facedb_.size(); }
  
  //:total number of faces from all subjects
  //unsigned n_faces() const { return facedb_.size(); }

  void print(vcl_ostream& os = vcl_cout) const;

  //-----------------------
  //:  BINARY I/O METHODS |
  //-----------------------

  //: Serial I/O format version
  virtual unsigned version() const {return 1;}

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const {return "dbru_object_signature_library";}

  //: determine if this is the given class
  virtual bool is_class(vcl_string const& cls) const
    { return cls==is_a();}
  
  //: Binary save self to stream.
  virtual void b_write(vsl_b_ostream &os) const ;

  //: Binary load self from stream.
  virtual void b_read(vsl_b_istream &is);

 private:
  // main data structure that holds the prototypes
  vcl_vector<vcl_vector<vil_image_resource_sptr> > facedb_;
  //vcl_vector<dbru_object_sptr>* facedb_;

};

inline void vsl_b_read(vsl_b_istream &is, dbru_facedb & facedb)
{facedb.b_read(is);}


inline void vsl_b_write(vsl_b_ostream &os, const dbru_facedb & facedb)
{facedb.b_write(os);}


inline void vsl_b_write(vsl_b_ostream &os, const dbru_facedb* facedb)
{
  //Don't allow writing for null feature data
  assert(facedb);
  const dbru_facedb& cfacedb = *facedb;
  vsl_b_write(os, cfacedb);
}

//: Binary load dbinfo_feature_data from stream.
inline void vsl_b_read(vsl_b_istream &is, dbru_facedb* &facedb)
{
  if (facedb)
    delete facedb;
  facedb = new dbru_facedb();
  vsl_b_read(is, *facedb);
}

inline vcl_ostream &operator<<(vcl_ostream &os, dbru_facedb const& facedb)
{
  facedb.print(os);
  return os;
}

inline void vsl_print_summary(vcl_ostream& os, dbru_facedb const*  facedb)
{os << facedb;}


#include <ozge/face_gui/dbru_facedb_sptr.h>
#endif // dbru_facedb_h_
