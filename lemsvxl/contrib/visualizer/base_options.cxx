#include <base_options.h>

Options GOptions;

bool Options::isDefined(const std::string& className, const std::string& attrName)
{
  return ((*this)[className].count(attrName)>0);
}

std::string Options::valueOf(const std::string& className, const std::string& attrName)
{
  if(isDefined(className,attrName)){
    return (*this)[className][attrName];
  }else{
    return std::string();
  }
}

void Options::setValue
(const std::string& className, const std::string& attrName, const std::string& value)
{
  (*this)[className][attrName] = value;
}
