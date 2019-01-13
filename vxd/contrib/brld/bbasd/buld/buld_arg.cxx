#include "buld_arg.h"
#include <iostream>


template <> void settype(vul_arg<std::vector<std::string> > &arg) { arg.type_ = "string list"; }

template <> void print_value(std::ostream &s, vul_arg<std::vector<std::string> > const &argmt)
{
  for (unsigned int i=0; i<argmt().size(); ++i) {
    s << argmt()[i];
    if (argmt()[i].length() == 0)
      s << "<empty>";
    if (i + 1 != argmt().size())
      s << ", " ;
  }
}

template <> int parse(vul_arg<std::vector<std::string> >* argmt, char ** argv)
{
  const char separator=' ';

  if ( !argv ||  !argv[0] ) {
    // no input
    std::cerr << "vul_arg_parse: Expected string, none is provided.\n";
    return -1;
  }

  if (argv[0]) {
    argmt->value_.clear();

    std::string str(argv[0]);
    //    std::cout << "Argv[0]: " << str << std::endl;

    std::string substr;
    for (unsigned i=0; i < str.length(); ++i) {
      if (str[i] == separator) {
        if (substr.length()) {
          argmt->value_.push_back(substr);
          substr.clear();
        }
      } else
        substr.push_back(str[i]);
    }

    if (substr.length())
      argmt->value_.push_back(substr);

    return 1;
  }
  else {
    std::cerr << __FILE__ ": no argument to string list option\n";
    return 0;
  }

  return 1;
}

