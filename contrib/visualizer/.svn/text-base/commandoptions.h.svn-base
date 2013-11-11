// A -*- C++ -*- class for parsing command line options.
//
// Copyright (C) 2001  Ole Laursen <olau@hardworking.dk>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the 
// Free Software Foundation, Inc., 59 Temple Place - Suite 330, 
// Boston, MA  02111-1307  USA.


#ifndef COMMANDOPTIONS_HEADER
#define COMMANDOPTIONS_HEADER

#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_exception.h>
#include <vcl_sstream.h>

class CommandOptions
{
public:
  // register an option with pointer to variable, long and short argument names, a
  // description of the option and the name of the argument to the option
  template <typename T>
  void register_option(T &par, vcl_string long_name, char short_name,
           vcl_string des, vcl_string arg_name)
  {
    option_table.push_back(option(new updater<T>(par), long_name, short_name, des, arg_name));
  }

  void register_flag(bool &par, vcl_string long_name, char short_name, vcl_string des)
  {
    flag_table.push_back(flag(par, long_name, short_name, des));
  }
  
  template <typename T>
  void register_argument(T &par, vcl_string name, vcl_string des)
  {
    argument_table.push_back(argument(new updater<T>(par), name, des));
  }

  void process_command_line(int argc, const char * const *argv);
  void process_command_line(const vcl_vector<vcl_string> &);

  ~CommandOptions()
  {
    // make sure dynamically allocated stuff is cleaned up
    for (vcl_vector<updaterbase *>::iterator it = clean_up_table.begin();
   it != clean_up_table.end(); ++it)
      delete (*it);
  }
  
  
private:
  // member helpers
    
  vcl_string requirify(vcl_string s) 
  {
    return '<' + s + '>';
  }
  vcl_string optionalify(vcl_string s) 
  {
    return '[' + s + ']';
  }

  vcl_string strip_path(vcl_string s);

  void print_usage(vcl_string executable);
  void print_help(vcl_string executable);
  
  // private data structures
  // first a few classes for updating a variable from a stream
public:
  class updaterbase 
  {
  public:
    virtual bool update(vcl_string s) = 0;
  };
private:
  template <typename T>
  class updater: public updaterbase
  {
  public:
    updater(T &par):
      var(par)
    {}

    // update the contained variable through reference - return false upon failure
    virtual bool update(vcl_string s)
    {
      std::istringstream is(s);
      is >> var;
      if (!is)
  return false;
      else
  return true;
    }
    
  private:
    T &var;
  };

  // a structure for keeping a command line option
  struct option 
  {
    updaterbase *par;
    vcl_string long_name, des, arg_name;
    char short_name;

    option(updaterbase *p, vcl_string ln, char sn, vcl_string d, vcl_string an):
      par(p), long_name(ln), des(d), arg_name(an), short_name(sn)
    {}
    option(const option &o):
      par(o.par), long_name(o.long_name), des(o.des), arg_name(o.arg_name), short_name(o.short_name)
    {}
  };

  // one for keeping a flag
  struct flag
  {
    bool *par;
    vcl_string long_name, des;
    char short_name;
    
    flag(bool &p, vcl_string ln, char sn, vcl_string d):
      par(&p), long_name(ln), des(d), short_name(sn)
    {}
  };
  
  // and one for keeping an argument
  struct argument
  {
    updaterbase *par;
    vcl_string name, des;
    
    argument(updaterbase *p, vcl_string n, vcl_string d):
      par(p), name(n), des(d)
    {}
    argument(const argument &a):
      par(a.par), name(a.name), des(a.des)
    {}
  };

  vcl_vector<option> option_table;
  vcl_vector<flag> flag_table;
  vcl_vector<argument> argument_table;
  typedef vcl_vector<option>::iterator option_iterator;
  typedef vcl_vector<flag>::iterator flag_iterator;
  typedef vcl_vector<argument>::iterator argument_iterator;

  vcl_vector<updaterbase *> clean_up_table;
};

template <> class CommandOptions::updater<vcl_string>: public CommandOptions::updaterbase
{
public:
  updater(vcl_string &par):
    var(par)
  {}

  // update the contained variable through reference - return false upon failure
  virtual bool update(vcl_string s)
  {
    var = s;
    return true;
  }
    
private:
  vcl_string &var;
};
  

// an exception class
class commandoptions_error: std::exception
{
public:
  commandoptions_error(vcl_string s) throw(): msg(s)
  {}
  ~commandoptions_error() throw()
  {}
    
  virtual const char *what() throw()
  {
    return msg.c_str();
  }
  
private:
  vcl_string msg;
};


#endif // COMMANDOPTIONS_HEADER
