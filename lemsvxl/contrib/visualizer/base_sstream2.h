#ifndef __SSTREAM2_H__
#define __SSTREAM2_H__

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>

// SStream2 is an extended version istreamstream.
// It does the same thing, except that it understands
// user-defined special strings (ex. "=", ":", "->", etc) as delimiters.

class SStream2 : public std::istringstream {
 private:
  std::string _origStr;
  std::string _resStr;
  std::vector<std::string> _specialStrings;
  void _convStr();
  
 public:
  SStream2(const std::string& str_, const std::string& specialChars_=std::string());
  SStream2(const std::string& str_, const std::vector<std::string>& specialStrings_);
  ~SStream2();
  char StdDelim;
  char StdQuote;
  char StdEsc;
  
  void setSpecialChars(const std::string& specialChars_);
  void setSpecialStrings(const std::vector<std::string>& specialStrings_);
};

// This is a simple manipulator class to skip the delimiter.
class Check {
 public: // modified by MC, check the Check() function.
  std::string _skipStr;
 public:
  Check(const std::string& skipStr);
  ~Check();
  friend std::istream &operator >> (std::istream &strm, const Check &obj);
};
std::istream &operator >> (std::istream &strm, const Check &obj);

#endif
