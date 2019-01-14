// This is brcv/rec/dbru/dbru_osl.h
#ifndef dbru_osl_h_
#define dbru_osl_h_
//---------------------------------------------------------------------
//:
// \file
// \brief The object signature library class 
//        Added functionalities from J.L. Mundy's dbinfo_osl class which was different from this one
//        essentially because it did not have a notion of an object
//        In this context:
//        prototype:   an instance of a vehicle seen in any frame (related data structure is dbinfo_observation)
//                     it has a geometry (e.g. a polygon of its sillhouette boundary) and an intensity profile of its pixels
//        object:      a series of prototypes of the same vehicle in the video collected over many frames
//        OSL:         a collection of labeled objects (each prototype of each object has a label)
//
// \author
//  O. C. Ozcanli - July 28, 2006
//
// \verbatim
//  Modifications
//  O. C. Ozcanli - Dec 04, 2006 - Include a list of all categories in the OSL
//                                 So that in the recognition phase they can be used to report results
//                                 This information is never written into binary files
//                                 (because this list can be populated during binary reading)
//                                 so version number is not increased to be consistent with existing files
// \endverbatim
//
//---------------------------------------------------------------------
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_array_1d.h>
#include <cassert>
#include <vsl/vsl_binary_io.h>
#include <dbinfo/dbinfo_observation_sptr.h>
#include <dbru/dbru_object_sptr.h>
#include <dbru/dbru_label_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>

// This class will keep a map to access objects and prototypes based on class info
class dbru_osl : public vbl_ref_count
{
  //: Constructors/destructor
 public:
  dbru_osl();
  ~dbru_osl();

  // if this constructer is used all the prototypes will be added as a separate object with 
  // labels only containing category
  //dbru_osl(std::vector<std::string> const& classes,
  //         std::vector<std::vector<dbinfo_observation_sptr> > const& prototypes
  //         );

  // normally this constructer should be used and objects should be appropriately labeled
  // e.g. via dbru_labeling_tool
  dbru_osl(std::vector<dbru_object_sptr> const& objects
           );

  //: copy constructor
  dbru_osl(const dbru_osl& rhs);

  void clear();

  //:mutators
  // this method adds the new prototype as a new object with a category label
  //void add_prototype(std::string const& cls,
  //                   dbinfo_observation_sptr const& proto);
  //void add_prototype(dbru_label_sptr lbl,
  //                   dbinfo_observation_sptr const& proto);

  //void add_prototypes(std::vector<std::string> const& classes,
  //                    std::vector<std::vector<dbinfo_observation_sptr> > const& prototypes);

  //: fill_category_id_map() method should be called after all objects are added
  //  to make that information available
  void add_object(dbru_object_sptr const& obj);

  void add_objects(std::vector<dbru_object_sptr> const& objects);

  //: remove a prototype observation. Return false if not in osl
  //bool remove_prototype(std::string const& cls, std::string const& obs_doc);
  //: remove all the prototypes for a class and delete the class from the osl
  bool remove_class(std::string const& cls);

  //: remove a particular object
  bool remove_object(unsigned i);
  
  //: make the observation and polygon pointers null
  bool null_prototype(unsigned i, unsigned j);

  //:accessors
  //bool find_prototype_vector(std::string const& cls, 
  //                           std::vector<dbinfo_observation_sptr>*& prototypes );
  bool prototypes(std::string const& cls, 
                  std::vector<dbinfo_observation_sptr>& prototypes);
  //dbinfo_observation_sptr prototype(std::string const& cls, const unsigned index);

  //: get a particular object
  dbru_object_sptr get_object(unsigned i);

  //: get a particular polygon of a particular object in osl
  vsol_polygon_2d_sptr get_polygon(unsigned i, unsigned j);
  //: get a particular prototype of a particular object in osl
  dbinfo_observation_sptr get_prototype(unsigned i, unsigned j);
  //: get a particular label of a particular object in osl
  dbru_label_sptr get_label(unsigned i, unsigned j);

  //: get all prototypes of an object
//  bool prototypes(unsigned i, 
//                  std::vector<dbinfo_observation_sptr>& prototypes);
  //: get all prototypes of an object based on doc
//  bool prototypes(std::string const& object_doc, 
//                  std::vector<dbinfo_observation_sptr>& prototypes);

  std::map<std::string, int>& get_category_id_map(void) { return category_id_map_; }
  bool fill_category_id_map(void);

  std::vector<std::string> classes() const;

  //:total number of classes
  unsigned n_classes() const { return osl_class_map_.size(); }
  
  //: the number of prototypes per class
  unsigned n_protos_in_class(std::string const& cls) const;

  //:total number of objects
  unsigned n_objects() const { return osl_.size(); }
  unsigned size() const { return osl_.size(); }

  //: the vector of number of class prototypes by enumerated by class
  std::vector<unsigned> n_prototypes() const;

  void print(std::ostream& os = std::cout) const;

  //-----------------------
  //:  BINARY I/O METHODS |
  //-----------------------

  //: Serial I/O format version
  virtual unsigned version() const {return 1;}

  //: Return a platform independent string identifying the class
  virtual std::string is_a() const {return "dbru_object_signature_library";}

  //: determine if this is the given class
  virtual bool is_class(std::string const& cls) const
    { return cls==is_a();}
  
  //: Binary save self to stream.
  virtual void b_write(vsl_b_ostream &os) const ;

  //: Binary load self from stream.
  virtual void b_read(vsl_b_istream &is);

  dbru_object_sptr operator[] (unsigned int i); 

 private:
  //: keep record of class structure 
  //                                     object id  prototype id
  std::map<std::string, std::vector<std::pair<unsigned, unsigned> >* > osl_class_map_;

  // main data structure that holds the prototypes
  std::vector<dbru_object_sptr> osl_;
  
  //: category map that assigns ids to categories, 
  //  this is useful during recognition phase
  std::map<std::string, int> category_id_map_;

};

inline void vsl_b_read(vsl_b_istream &is, dbru_osl & osl)
{osl.b_read(is);}


inline void vsl_b_write(vsl_b_ostream &os, const dbru_osl & osl)
{osl.b_write(os);}


inline void vsl_b_write(vsl_b_ostream &os, const dbru_osl* osl)
{
  //Don't allow writing for null feature data
  assert(osl);
  const dbru_osl& cosl = *osl;
  vsl_b_write(os, cosl);
}

//: Binary load dbinfo_feature_data from stream.
inline void vsl_b_read(vsl_b_istream &is, dbru_osl* &osl)
{
  if (osl)
    delete osl;
  osl = new dbru_osl();
  vsl_b_read(is, *osl);
}

inline std::ostream &operator<<(std::ostream &os, dbru_osl const& osl)
{
  osl.print(os);
  return os;
}

inline void vsl_print_summary(std::ostream& os, dbru_osl const*  osl)
{os << osl;}


#include <dbru/dbru_osl_sptr.h>
#endif // dbru_osl_h_

