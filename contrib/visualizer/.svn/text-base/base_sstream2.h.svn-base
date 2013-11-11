#ifndef __SSTREAM2_H__
#define __SSTREAM2_H__

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_sstream.h>
#include <vcl_vector.h>
#include <vcl_map.h>

// SStream2 is an extended version istreamstream.
// It does the same thing, except that it understands
// user-defined special strings (ex. "=", ":", "->", etc) as delimiters.

class SStream2 : public vcl_istringstream {
 private:
  vcl_string _origStr;
  vcl_string _resStr;
  vcl_vector<vcl_string> _specialStrings;
  void _convStr();
  
 public:
  SStream2(const vcl_string& str_, const vcl_string& specialChars_=vcl_string());
  SStream2(const vcl_string& str_, const vcl_vector<vcl_string>& specialStrings_);
  ~SStream2();
  char StdDelim;
  char StdQuote;
  char StdEsc;
  
  void setSpecialChars(const vcl_string& specialChars_);
  void setSpecialStrings(const vcl_vector<vcl_string>& specialStrings_);
};

// This is a simple manipulator class to skip the delimiter.
class Check {
 public: // modified by MC, check the Check() function.
  vcl_string _skipStr;
 public:
  Check(const vcl_string& skipStr);
  ~Check();
  friend vcl_istream &operator >> (vcl_istream &strm, const Check &obj);
};
vcl_istream &operator >> (vcl_istream &strm, const Check &obj);

#endif
