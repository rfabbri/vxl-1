// This is brl/bseg/dbinfo/dbinfo_osl.h
#ifndef dbinfo_osl_h_
#define dbinfo_osl_h_
//---------------------------------------------------------------------
//:
// \file
// \brief The object signature library
//
// \author
//  J.L. Mundy - May 08, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//---------------------------------------------------------------------
// A simple object signature library that stores a set of observations of
// a given class. 
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <vbl/vbl_ref_count.h>
#include <cassert>
#include <vsl/vsl_binary_io.h>
#include <dbinfo/dbinfo_observation_sptr.h>

class dbinfo_osl : public vbl_ref_count
{
  //: Constructors/destructor
 public:
  dbinfo_osl();
  ~dbinfo_osl();

  dbinfo_osl(std::vector<std::string> const& classes,
             std::vector<std::vector<dbinfo_observation_sptr> > const& prototypes
             );

  //: copy constructor
  dbinfo_osl(const dbinfo_osl& rhs);

  //:mutators
  void add_prototype(std::string const& cls,
                     dbinfo_observation_sptr const& proto);

  void add_prototypes(std::vector<std::string> const& classes,
                      std::vector<std::vector<dbinfo_observation_sptr> > const& prototypes);
  //: remove a prototype observation. Return false if not in osl
  bool remove_prototype(std::string const& cls, std::string const& obs_doc);
  //: remove all the prototypes for a class and delete the class from the osl
  bool remove_class(std::string const& cls);
    
  //:accessors
  bool find_prototype_vector(std::string const& cls, 
                             std::vector<dbinfo_observation_sptr>*& prototypes );
  bool prototypes(std::string const& cls, 
                  std::vector<dbinfo_observation_sptr>& prototypes);

  dbinfo_observation_sptr prototype(std::string const& cls, const unsigned index);

  std::vector<std::string> classes() const;

  //:total number of classes
  unsigned size(){return osl_.size();}
  
  //: the number of prototypes per class
  unsigned n_protos_in_class(std::string const& cls);

  //: the vector of number of class prototypes by enumerated by class
  std::vector<unsigned> n_prototypes();

  void print(std::ostream& os = std::cout) const;

  //-----------------------
  //:  BINARY I/O METHODS |
  //-----------------------

  //: Serial I/O format version
  virtual unsigned version() const {return 1;}

  //: Return a platform independent string identifying the class
  virtual std::string is_a() const {return "dbinfo_object_signature_library";}

  //: determine if this is the given class
  virtual bool is_class(std::string const& cls) const
    { return cls==is_a();}
  
  //: Binary save self to stream.
  virtual void b_write(vsl_b_ostream &os) const ;

  //: Binary load self from stream.
  virtual void b_read(vsl_b_istream &is);

 private:
  std::map<std::string, std::vector<dbinfo_observation_sptr>* > osl_;

};

inline void vsl_b_read(vsl_b_istream &is, dbinfo_osl & osl)
{osl.b_read(is);}


inline void vsl_b_write(vsl_b_ostream &os, const dbinfo_osl & osl)
{osl.b_write(os);}


inline void vsl_b_write(vsl_b_ostream &os, const dbinfo_osl* osl)
{
  //Don't allow writing for null feature data
  assert(osl);
  const dbinfo_osl& cosl = *osl;
  vsl_b_write(os, cosl);
}

//: Binary load dbinfo_feature_data from stream.
inline void vsl_b_read(vsl_b_istream &is, dbinfo_osl* &osl)
{
  delete osl;
  osl = new dbinfo_osl();
  vsl_b_read(is, *osl);
}

inline std::ostream &operator<<(std::ostream &os, dbinfo_osl const& osl)
{
  osl.print(os);
  return os;
}

inline void vsl_print_summary(std::ostream& os, dbinfo_osl const*  osl)
{os << osl;}


#include <dbinfo/dbinfo_osl_sptr.h>
#endif // dbinfo_osl_h_
