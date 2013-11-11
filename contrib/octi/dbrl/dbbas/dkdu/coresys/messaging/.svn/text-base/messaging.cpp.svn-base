/*****************************************************************************/
// File: messaging.cpp [scope = CORESYS/MESSAGING]
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
   Implements the interfaces defined by "kdu_messaging.h".
******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include "kdu_elementary.h"
#include "kdu_messaging.h"

using namespace std;

static ostream *warn_ostream = NULL;
static int warn_line_length = 79;
static kdu_messaging_callback_func warn_callback = NULL;

static ostream *err_ostream = NULL;
static int err_line_length = 79;
static kdu_messaging_callback_func err_callback = NULL;


/* ========================================================================= */
/*                             External Functions                            */
/* ========================================================================= */

/*****************************************************************************/
/* EXTERN                    kdu_customize_warnings                          */
/*****************************************************************************/

void
  kdu_customize_warnings(ostream *stream, int line_length,
                         kdu_messaging_callback_func callback)
{
  warn_ostream = stream;
  warn_line_length = line_length;
  warn_callback = callback;
}

/*****************************************************************************/
/* EXTERN                     kdu_customize_errors                           */
/*****************************************************************************/

void
  kdu_customize_errors(ostream *stream, int line_length,
                       kdu_messaging_callback_func callback)
{
  err_ostream = stream;
  err_line_length = line_length;
  err_callback = callback;
}


/* ========================================================================= */
/*                               kdu_pretty_buf                              */
/* ========================================================================= */

/*****************************************************************************/
/*                        kdu_pretty_buf::overflow                           */
/*****************************************************************************/

int kdu_pretty_buf::overflow(int nCh)
{
  if (dest == NULL)
    return 0;
  if (nCh == '\t')
    {
      if (no_output_since_newline)
        {
          int indent_change = 4;

          if ((indent+indent_change+master_indent) > max_indent)
            indent_change = max_indent - indent - master_indent;
          indent += indent_change;
          while (indent_change--)
            line_buf[num_chars++] = ' ';
          assert(num_chars < line_chars);
          return 0;
        }
      else
        nCh = ' ';
    }

  if (nCh == '\n')
    {
      indent = 0;
      no_output_since_newline = true;
      line_buf[num_chars] = '\0';
      (*dest) << line_buf << "\n";
      for (num_chars=0; num_chars < master_indent; num_chars++)
        line_buf[num_chars] = ' ';
      return 0;
    }

  line_buf[num_chars++] = (char) nCh;
  no_output_since_newline = false;
  if (num_chars > line_chars)
    {
      int blank_chars, output_chars, i;

      for (blank_chars=0; blank_chars < num_chars; blank_chars++)
        if (line_buf[blank_chars] != ' ')
          break;
      for (output_chars=num_chars-1; output_chars>blank_chars; output_chars--)
        if (line_buf[output_chars] == ' ')
          break;
      if (output_chars == blank_chars)
        output_chars = line_chars; // Have to break word across lines.
      for (i=0; i < output_chars; i++)
        dest->put(line_buf[i]);
      while ((line_buf[output_chars] == ' ') && (output_chars < num_chars))
        output_chars++;
      dest->put('\n');
      num_chars = num_chars - output_chars + indent + master_indent;
      assert(num_chars <= line_chars);
      for (i=0; i < (indent+master_indent); i++)
        line_buf[i] = ' ';
      for (; i < num_chars; i++)
        line_buf[i] = line_buf[output_chars++];
    }
  return 0;
}

/*****************************************************************************/
/*                          kdu_pretty_buf::sync                             */
/*****************************************************************************/

int kdu_pretty_buf::sync()
  /* This function will be called whenever the `ostream' object to which the
     `kdu_pretty_buf' object is attached is flushed. It will also be called
     upon termination. It flushes the contents of the current line buffer to
     the destination stream. */
{
  if (dest == NULL)
    return 0;
  if (!no_output_since_newline)
    {
      line_buf[num_chars] = '\0';
      (*dest) << line_buf << "\n";
      for (num_chars=0; num_chars < (indent+master_indent); num_chars++)
        line_buf[num_chars] = ' ';
      no_output_since_newline = true;
    }
  dest->flush();
  return 0;
}

/*****************************************************************************/
/*                    kdu_pretty_buf::set_master_indent                      */
/*****************************************************************************/

void kdu_pretty_buf::set_master_indent(int val)
{
  assert(no_output_since_newline);
  assert(val >= 0);
  if (val > max_indent)
    val = max_indent;
  while (master_indent > val)
    { master_indent--; num_chars--; }
  while (master_indent < val)
    { master_indent++; line_buf[num_chars++] = ' '; }
}

/* ========================================================================= */
/*                                kdu_warning                                */
/* ========================================================================= */

/*****************************************************************************/
/*                          kdu_warning::kdu_warning                         */
/*****************************************************************************/

kdu_warning::kdu_warning() : ostream(&warn_buf),
                             warn_buf(warn_ostream,warn_line_length)
{
  (*this) << "Kakadu Warning:\n";
  warn_buf.set_master_indent(4);
}

/*****************************************************************************/
/*                         kdu_warning::~kdu_warning                         */
/*****************************************************************************/

kdu_warning::~kdu_warning()
{
  flush();
  if (warn_callback != NULL)
    warn_callback();
}

/* ========================================================================= */
/*                                 kdu_error                                 */
/* ========================================================================= */

/*****************************************************************************/
/*                            kdu_error::kdu_error                           */
/*****************************************************************************/

kdu_error::kdu_error() : ostream(&err_buf),
                         err_buf(err_ostream,err_line_length)
{
  (*this) << "Kakadu Error:\n";
  err_buf.set_master_indent(4);
}

/*****************************************************************************/
/*                           kdu_error::~kdu_error                           */
/*****************************************************************************/

kdu_error::~kdu_error()
{
  flush();
  if (err_callback != NULL)
    err_callback();
  else
    exit(-1); // Default is to exit from process.
}
