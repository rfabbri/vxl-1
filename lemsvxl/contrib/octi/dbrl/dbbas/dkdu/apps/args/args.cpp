/*****************************************************************************/
// File: args.cpp [scope = APPS/ARGS]
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
   Implements the interfaces defined by "kdu_args.h".
******************************************************************************/

#include <string.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include "kdu_elementary.h"
#include "kdu_messaging.h"
#include "kdu_args.h"

/* ========================================================================= */
/*                                kdu_args                                   */
/* ========================================================================= */

struct kd_arg_list {
  public: // Functions
    ~kd_arg_list() { delete[] string; }
  public: // Data
    kd_arg_list *next;
    char *string;
  };

/*****************************************************************************/
/*                            kdu_args::new_arg                              */
/*****************************************************************************/

void
  kdu_args::new_arg(char *string)
{
  if (current == NULL)
    current = first;
  while ((current != NULL) && (current->next != NULL))
    current = current->next;
  prev = current;
  current = new kd_arg_list;
  current->string = new char[strlen(string)+1];
  strcpy(current->string,string);
  current->next = NULL;
  if (prev == NULL)
    first = current;
  else
    prev->next = current;
  prev = NULL;
}

/*****************************************************************************/
/*                            kdu_args::kdu_args                             */
/*****************************************************************************/

kdu_args::kdu_args(int argc, char *argv[], char *switch_pattern)
{
  first = current = prev = removed = NULL;
  assert(argc > 0);
  prog_name = argv[0];
  for (argc--, argv++; argc > 0; argc--, argv++)
    {
      if ((switch_pattern != NULL) && (strcmp(*argv,switch_pattern) == 0))
        {
          argc--; argv++;
          if (argc == 0)
            { kdu_error e; e << "The \"" << switch_pattern << "\" argument "
              "must be followed by a file name from which to read "
              "arguments."; }
          char *fname = *argv;
          std::ifstream stream(fname,std::ios::in);
          if (!stream)
            { kdu_error e; e << "Unable to open the argument switch file, \"";
              e << fname << "\"!"; }
          char line[2048];
          while (!(stream.getline(line,2048).fail()))
            {
              char *start=line;
              char *end;
              do {
                while ((*start == ' ') || (*start == '\t') || (*start == '\r'))
                  start++;
                if ((*start == '#') || (*start == '%'))
                  break;
                end = start;
                while ((*end != '\0') && (*end != ' ') &&
                       (*end != '\t') && (*end != '\r'))
                  end++;
                if (end == start)
                  break;
                char tmp = *end; *end = '\0'; new_arg(start); *end = tmp;
                start = end;
              } while (*start != '\0');
            }
        }
      else
        new_arg(*argv);
    }
  current = prev = NULL;
}

/*****************************************************************************/
/*                           kdu_args::~kdu_args                             */
/*****************************************************************************/

kdu_args::~kdu_args()
{
  while ((current=first) != NULL)
    {
      first = current->next;
      delete current;
    }
  while ((current=removed) != NULL)
    {
      removed = current->next;
      delete current;
    }
}

/*****************************************************************************/
/*                          kdu_args::get_first                              */
/*****************************************************************************/

char *
  kdu_args::get_first()
{
  current = first;
  prev = NULL;
  return (current==NULL)?NULL:current->string;
}

/*****************************************************************************/
/*                             kdu_args::find                                */
/*****************************************************************************/

char *
  kdu_args::find(const char *pattern)
{
  prev = NULL;
  for (current=first; current != NULL; prev=current, current=current->next)
    if (strcmp(current->string,pattern) == 0)
      break;
  return (current==NULL)?NULL:current->string;
}

/*****************************************************************************/
/*                            kdu_args::advance                              */
/*****************************************************************************/

char *
  kdu_args::advance(bool remove_last)
{
  if (current == NULL)
    { prev = NULL; return NULL; }
  if (remove_last)
    {
      if (prev == NULL)
        {
          assert(current == first);
          first = current->next;
          current->next = removed;
          removed = current;
          current = first;
        }
      else
        {
          prev->next = current->next;
          current->next = removed;
          removed = current;
          current = prev->next;
        }
    }
  else
    {
      prev = current;
      current = current->next;
    }
  return (current==NULL)?NULL:current->string;
}

/*****************************************************************************/
/*                       kdu_args::show_unrecognized                         */
/*****************************************************************************/

int
  kdu_args::show_unrecognized(std::ostream &out)
{
  int count = 0;
  for (kd_arg_list *scan=first; scan != NULL; scan=scan->next)
    {
      out << "Unused argument: \"" << scan->string << "\"\n";
      count++;
    }
  out.flush();
  return count;
}
