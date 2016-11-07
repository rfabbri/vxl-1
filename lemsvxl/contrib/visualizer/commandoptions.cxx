// Implementation of the command line parsing class.
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

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_sstream.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h>    // for exit
#include <vcl_algorithm.h>
#include <vcl_cstdlib.h>

#include "commandoptions.h"

// true if val is in container con
template <typename T, typename C>
bool is_in_container(T val, C con)
{
  return vcl_find(con.begin(), con.end(), val) != con.end();
}


void CommandOptions::process_command_line(int argc, const char * const *argv)
  {
    vcl_vector<vcl_string> args;
    for (int i=0;i<argc;i++)
      {
        args.push_back(argv[i]);
       }
    process_command_line(args);
   }


void CommandOptions::process_command_line(const vcl_vector<vcl_string> &argv)
{
int argc=(int)argv.size();


argument_iterator ait = argument_table.begin();
  vcl_vector<flag_iterator> processed_flags; 
  flag_iterator fit;
  option_iterator oit;

  int i = 1;
  while (i < argc) {
    vcl_string s(argv[i]);
    
    if (s[0] == '-') {    // option encountered
      
      if (s.size() == 1)  // ensure we got some meat to work on
  throw commandoptions_error("'-' specified without option name");
      
      if (s[1] == '-') {  // long option
  
  if (s.size() == 2)  // ensure we got some meat to work on
    throw commandoptions_error("'--' specified without option name");

  vcl_string name = s.substr(2);

  // catch the built-in options
  if (name == "help")
    print_help(argv[0]);
  
  if (name == "usage")
    print_usage(argv[0]);
  
  // search flag table
  for (fit = flag_table.begin(); fit != flag_table.end(); ++fit)
    if (fit->long_name == name) {
      // make sure we don't get "-f -f" = ""
      if (!is_in_container(fit, processed_flags)) {
        *(fit->par) = !(*(fit->par)); // reverse flag
        processed_flags.push_back(fit); // remember this flag
      }
      
      goto next_token;
    }

  // then the option table
  for (oit = option_table.begin(); oit != option_table.end(); ++oit)
    if (oit->long_name == name) {
      if (++i >= argc)  // check that there's one more waiting
        throw commandoptions_error("option '--" + name + "' specified without argument");
      else if (!oit->par->update(argv[i]))
        throw commandoptions_error("invalid argument '" + vcl_string(argv[i]) +
           "' for option '--" + name + "'");
      
      goto next_token;
    }

  // if we get here, the option wasn't recognized
  throw commandoptions_error("unrecognized option '" + s + "'");
      }
      else {      // short option(s)

  bool has_had_option = false;
  
  // loop through entire vcl_string in case some flags were grouped
  unsigned int j = 1;
  while (j < s.length()) {
    char name = s[j];
  
    // catch the built-in short help option
    if (name == '?')
      print_help(argv[0]);
  
    // search flag table
    for (fit = flag_table.begin(); fit != flag_table.end(); ++fit)
      if (fit->short_name == name) {
        if (!is_in_container(fit, processed_flags)) {
    *(fit->par) = !(*(fit->par)); // reverse flag
    processed_flags.push_back(fit); // remember the flag
        }
    
        goto next_option_in_group;
      }
    
    // then the option table
    for (oit = option_table.begin(); oit != option_table.end(); ++oit)
      if (oit->short_name == name) {
        if (has_had_option)
    throw commandoptions_error("more than one option in the option group '"
             + s + "' requires an argument");
        else if (++i >= argc) // ensure that there's one more waiting
    throw commandoptions_error(vcl_string("option '-") + name + "' specified without argument");
        else if (!oit->par->update(argv[i]))
    throw commandoptions_error("invalid argument '" + vcl_string(argv[i]) +
             "' for option '-" + name + "'");
        has_had_option = true;

        goto next_option_in_group;
      }
    
    // if we get here, the option wasn't recognized
    throw commandoptions_error(vcl_string("unrecognized option '-") + name + "'");

  next_option_in_group:
    ++j;
  }
      }
    }
    else {      // just a normal argument
      if (ait == argument_table.end())
  throw commandoptions_error("too many arguments specified (last one processed is '" + s + "')");
      else if (!ait->par->update(s))
  throw commandoptions_error("invalid argument '" + s + "' passed as " + requirify(ait->name));
      else
  ++ait;      // prepare for next argument
    }
    
  next_token:
    ++i;
  }

  // check that we got enough arguments
  if (ait != argument_table.end()) {
    vcl_string missing;
    while (ait != argument_table.end()) { // build up list of missing arguments
      missing += " '" + requirify(ait->name) + "'";
      ++ait;
    }
    
    throw commandoptions_error("not enough arguments specified, missing" + missing);
  }
}




// FIXME: switch to dedicated functions for handling encountered options, the
// above function is much too long


vcl_string CommandOptions::strip_path(vcl_string s)
{
#ifdef HAVE_DOS
  return s.substr(s.rfind('\\') + 1);
#else
  return s.substr(s.rfind('/') + 1);
#endif
}

void CommandOptions::print_usage(vcl_string executable)
{
  flag_iterator fi;
  option_iterator oi;
  argument_iterator ai;

  // print usage and program name
  vcl_cerr << "Usage: " << strip_path(executable);

  // gather and print flags
  vcl_string shorts = "-";  // we sort the flags into short and long ones
  vcl_string longs;

  for (fi = flag_table.begin(); fi != flag_table.end(); ++fi) {
    if (fi->short_name != '\0')
      shorts += fi->short_name;
    else
      longs += ' ' + optionalify("--" + fi->long_name);
  }

  // insert -? (for help)
  shorts += '?';
    
  vcl_cerr << ' ' << optionalify(shorts) << longs; 

  
  // print options
  for (oi = option_table.begin(); oi != option_table.end(); ++oi)
    if (oi->short_name != '\0')
      vcl_cerr << ' ' << optionalify(vcl_string("-") + oi->short_name  + " " + oi->arg_name);
    else
      vcl_cerr << ' ' << optionalify("--" + oi->long_name + " " + oi->arg_name);

  // then print --usage so it doesn't clutter up the more interesting options
  vcl_cerr << ' ' + optionalify("--usage");
  
  
  // and finally print arguments
  for (ai = argument_table.begin(); ai != argument_table.end(); ++ai)
    vcl_cerr << ' ' << requirify(ai->name);

  
  vcl_cerr << vcl_endl;
  
  exit(1);
}


// helper for securing right indentation
void indent_for_description(int current_indent)
{
  const int description_indentation = 24;

  int extra_space = description_indentation - current_indent;
    
  if (extra_space > 0)
    vcl_cerr << vcl_string(extra_space, ' ');
  else
    vcl_cerr << ' ';    // we always need at least one space
}


void CommandOptions::print_help(vcl_string executable)
{
  // print "Usage: programname [OPTIONS...]"
  vcl_cerr << "Usage: " << strip_path(executable) << ' ' << optionalify("OPTIONS...");

  // concatenate argument names
  for (argument_iterator i = argument_table.begin(); i != argument_table.end(); ++i)
    vcl_cerr << ' ' << requirify(i->name);

  vcl_cerr << vcl_endl;

  if (flag_table.size() > 0 || option_table.size() > 0)
    vcl_cerr << "\nOptions:\n";
  
  // then list the flags and options
  for (flag_iterator fi = flag_table.begin(); fi != flag_table.end(); ++fi) {
    vcl_cerr << "  ";    // indentation

    int x = 2;      // we need to keep track of how far we've reached
    
    if (fi->short_name != '\0') {
      vcl_cerr << '-' << fi->short_name;
      x += 2;
    }

    if (fi->long_name.length() > 0) {
      if (fi->short_name != '\0') { 
  vcl_cerr << ", ";  // add separator
  x += 2;
      }
      
      vcl_cerr << "--" << fi->long_name;
      
      x += 2 + fi->long_name.length();
    }
    
    indent_for_description(x);

    vcl_cerr << fi->des << vcl_endl;
  }

  for (option_iterator oi = option_table.begin(); oi != option_table.end(); ++oi) {
    vcl_cerr << "  ";    

    int x = 2;      
    
    if (oi->short_name != '\0') {
      vcl_cerr << '-' << oi->short_name;
      x += 2;
    }

    if (oi->long_name.length() > 0) {
      if (oi->short_name != '\0') { // output separator
  vcl_cerr << ", ";  
  x += 2;
      }
      
      vcl_cerr << "--" << oi->long_name;
      
      x += 2 + oi->long_name.length();
    }

    // output the name of the option argument
    vcl_cerr << ' ' << oi->arg_name;

    x += 1 + oi->arg_name.length();

    indent_for_description(x);
      
    vcl_cerr << oi->des << vcl_endl;
  }


  bool first_argument = true;

  // output descriptions of arguments, if any
  for (argument_iterator ai = argument_table.begin(); ai != argument_table.end(); ++ai) {
    if (ai->des.length() > 0) {
      if (first_argument)  // prefix the first argument
  vcl_cerr << "\nArguments:\n";
      first_argument = false;

      vcl_cerr << "  " << ai->name;

      indent_for_description(2 + ai->name.length());
      vcl_cerr << ai->des << vcl_endl;
    }
  }

  vcl_cerr << "\nHelp options:\n";
  
  // and finally output help about help options
  vcl_string s = "  -h, --help";
  vcl_cerr << s;
  indent_for_description(s.length());
  vcl_cerr << "Show this help message\n";

  s = "  --usage";
  vcl_cerr << s;
  indent_for_description(s.length());
  vcl_cerr << "Display brief usage message\n";
  
  exit(1);
} 
