/*****************************************************************************/
// File: kdu_args.h [scope = APPS/ARGS]
// Version: Kakadu, V2.2
// Author: David Taubman
// Last Revised: 20 June, 2001
/*****************************************************************************/
// Copyright 2001, David Taubman, The University of New South Wales (UNSW)
// The copyright owner is Unisearch Ltd, Australia (commercial arm of UNSW)
// Neither this copyright statement, nor the licensing details below
// may be removed from this file or dissociated from its contents.
/*****************************************************************************/
// Licensee: Book Owner
// License number: 99999
// The Licensee has been granted a NON-COMMERCIAL license to the contents of
// this source file, said Licensee being the owner of a copy of the book,
// "JPEG2000: Image Compression Fundamentals, Standards and Practice," by
// Taubman and Marcellin (Kluwer Academic Publishers, 2001).  A brief summary
// of the license appears below.  This summary is not to be relied upon in
// preference to the full text of the license agreement, which was accepted
// upon breaking the seal of the compact disc accompanying the above-mentioned
// book.
// 1. The Licensee has the right to Non-Commercial Use of the Kakadu software,
//    Version 2.2, including distribution of one or more Applications built
//    using the software, provided such distribution is not for financial
//    return.
// 2. The Licensee has the right to personal use of the Kakadu software,
//    Version 2.2.
// 3. The Licensee has the right to distribute Reusable Code (including
//    source code and dynamically or statically linked libraries) to a Third
//    Party, provided the Third Party possesses a license to use the Kakadu
//    software, Version 2.2, and provided such distribution is not for
//    financial return.
/******************************************************************************
Description:
   Defines handy services for command-line argument processing.
******************************************************************************/

#ifndef KDU_ARGS_H
#define KDU_ARGS_H

#include <stdlib.h>
#include <iostream>
#include <assert.h>
#include "kdu_elementary.h"

/*****************************************************************************/
/*                                 kdu_args                                  */
/*****************************************************************************/

class kdu_args {
  public: // Member functions
    kdu_args(int argc, char *argv[], char *switch_pattern = NULL);
      /* Transfers command-line arguments into the internal representation.
         The `switch_pattern' argument may be used to provide an argument
         string (usually "-s", for "switch") which will be recognized as
         a request recover arguments from a file.  If this pattern is found,
         the next argument will be interpreted as the file name and each
         token in the file becomes a new argument, where tokens are delimited
         by white space, i.e., spaces, new-lines, tabs and
         carriage returns.  The very first element of the `argv' array is
         expected to hold the program name. */
    ~kdu_args();
    char *get_prog_name()
      { return prog_name; }
    char *get_first();
      /* Returns NULL if there are no arguments left.  Otherwise returns
         the first argument which has not yet been removed, where "first"
         refers to the order of appearance of the arguments in the original
         list. */
    char *find(const char *pattern);
      /* Returns NULL unless an argument matching the supplied `pattern'
         string can be found, in which it returns a pointer to the relevant
         argument.  Currently, only direct string matching on the `pattern'
         string is supported. */
    char *advance(bool remove_last=true);
      /* Advances to the next argument. If `remove_last' is true, the most
         recent argument returned via any of `get_first', `find' or `advance'
         is first removed. The function returns NULL if we try to advance
         past the last argument, or the most recent return value from
         `get_first', `find' or `advance' was NULL. */
    int show_unrecognized(std::ostream &out);
      /* Warns the user of any arguments which have not been removed,
         presumably because they were not recognized.  The warning messages
         are sent to the supplied stream.  The function returns a count of
         the number of unrecognized arguments. */
  private: // Convenience functions
    void new_arg(char *string);
  private: // Data
    char *prog_name;
    struct kd_arg_list *first, *current, *prev, *removed;
  };

#endif // KDU_ARGS_H
