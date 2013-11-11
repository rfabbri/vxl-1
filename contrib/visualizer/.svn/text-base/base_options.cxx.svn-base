#include <base_options.h>

Options GOptions;

bool Options::isDefined(const vcl_string& className, const vcl_string& attrName)
{
  return ((*this)[className].count(attrName)>0);
}

vcl_string Options::valueOf(const vcl_string& className, const vcl_string& attrName)
{
  if(isDefined(className,attrName)){
    return (*this)[className][attrName];
  }else{
    return vcl_string();
  }
}

void Options::setValue
(const vcl_string& className, const vcl_string& attrName, const vcl_string& value)
{
  (*this)[className][attrName] = value;
}
