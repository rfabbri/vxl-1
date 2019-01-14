#ifndef __OPTIONS_H__
#define __OPTIONS_H__
#include <string>
#include <map>

// Options class stores the option information from the ini file.
// The information is indexed , firstly by the class-name
// (i.e. plug-in name), and secondly by the attribute name.

class Options : public std::map<std::string,std::map<std::string, std::string> > {
 public:
  Options(){};
  ~Options(){};

  bool isDefined(const std::string& className, const std::string& attrName);
  std::string valueOf(const std::string& className, const std::string& attrName);
  void setValue(const std::string& className, const std::string& attrName,
    const std::string& value);
};

extern Options GOptions;

#endif
