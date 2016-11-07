#ifndef __OPTIONS_H__
#define __OPTIONS_H__
#include <vcl_string.h>
#include <vcl_map.h>

// Options class stores the option information from the ini file.
// The information is indexed , firstly by the class-name
// (i.e. plug-in name), and secondly by the attribute name.

class Options : public vcl_map<vcl_string,vcl_map<vcl_string, vcl_string> > {
 public:
  Options(){};
  ~Options(){};

  bool isDefined(const vcl_string& className, const vcl_string& attrName);
  vcl_string valueOf(const vcl_string& className, const vcl_string& attrName);
  void setValue(const vcl_string& className, const vcl_string& attrName,
    const vcl_string& value);
};

extern Options GOptions;

#endif
