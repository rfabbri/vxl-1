// This is brcv/rec/dbru/dbru_label.h
#ifndef dbru_label_h_
#define dbru_label_h_
//:
// \file
// \brief Class to hold labeling information of vehicle objects for recognition
//
// \author Ozge C Ozcanli Ozbay, (ozge@lems.brown.edu)
// \date 6/28/05
//      
// \endverbatim

#include <vcl_string.h>
#include <vbl/vbl_ref_count.h>
#include <vcl_iostream.h>
#include <vsl/vsl_binary_io.h>

//: class to hold label information of vehicle objects in video
class dbru_label : public vbl_ref_count 
{

 public:

  dbru_label(const vcl_string name = "null",
            const int motion_orientation_bin = -1,
            const int view_angle_bin = -1,
            const int shadow_angle_bin = -1,
            const int shadow_length = -1);

  dbru_label(const dbru_label& old_label);
 ~dbru_label(){}

  vcl_string category() { return category_name_; }

  //:Print information about self
  virtual void print(vcl_ostream& os = vcl_cout) const;
  
  dbru_label& operator=(const dbru_label& right);

  //-----------------------
 //:  BINARY I/O METHODS |
 //-----------------------

 //: Serial I/O format version
 virtual unsigned version() const {return 1;}

 //: Return a platform independent string identifying the class
 virtual vcl_string is_a() const {return "dbru_label";}

 //: determine if this is the given class
 virtual bool is_class(vcl_string const& cls) const
   { return cls==is_a();}
  
 //: Binary save self to stream.
 virtual void b_write(vsl_b_ostream &os) const ;

 //: Binary load self from stream.
 virtual void b_read(vsl_b_istream &is);

 protected:
  void Init(vcl_string name,
            int motion_orientation_bin,
            int view_angle_bin,
            int shadow_angle_bin,
            int shadow_length);
 public:
  //
  // Parameter blocks and parameters
  //
  vcl_string category_name_;  //!< 
  int motion_orientation_bin_;    //!< 
  int view_angle_bin_;                   //!< 
  int shadow_angle_bin_;   //!< 
  int shadow_length_;               //!< 
  //bool training_sample_;   //!< true if this object is selected to be inserted into database otherwise its gonna be used for testing
                                      
};

inline vcl_ostream &operator<<(vcl_ostream &os, dbru_label const& fb)
{
  fb.print(os);
  return os;
}

#endif // dbru_label_h_
