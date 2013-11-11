/*****************************************************************************/
// File: kdu_messaging.h [scope = CORESYS/COMMON]
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
   Defines formatted error and warning message services, which alleviate
other parts of the implementation from concerns regarding formatting of text
messages, appropriate termination methods (e.g. process exit() or exception
throwing) in the event of a fatal error, graphical user interface
considerations and so forth.  These reduce or eliminate the effort required
to port the system to different application environments.
******************************************************************************/

#ifndef KDU_MESSAGING_H
#define KDU_MESSAGING_H

#include <iostream>
#include <assert.h>
#include "kdu_elementary.h"

/* ========================================================================= */
/*                       Messaging Setup Functions                           */
/* ========================================================================= */

typedef void (*kdu_messaging_callback_func)(void);
   /* Prototype for call-back function invoked when a warning or error
      message is completed. */

extern KDU_EXPORT void
  kdu_customize_warnings(std::ostream *stream, int line_length=79,
                         kdu_messaging_callback_func callback=NULL);
extern KDU_EXPORT void
  kdu_customize_errors(std::ostream *stream, int line_length=79,
                       kdu_messaging_callback_func callback=NULL);
  /* These functions can and usually should be used to customize the behaviour
     of warning and error messages produced using the "kdu_warning" and
     "kdu_error" objects.  By default, messages will simply be discarded
     and the completion of an error message will terminate the process.
        To prevent the messages from being discarded, supply a non-NULL
     pointer for the `stream' argument.  After formatting in accordance with
     the rules defined for "kdu_pretty_buf", the message text will be delivered
     to the object referenced by `stream'.
        A reasonable default line length for message text formatting is 79
     characters.  A different value may be specified via the `line_length'
     argument; however, there is no guarantee that the specified value will
     not be reduced to accommodate internal resources.
        The `callback' argument may be used to specify a function to be called
     once the warning or error message is complete.  At this point, a popup
     window might be brought to the foreground.  In the case of error messages,
     this function may be used to override the default behaviour of exiting
     the process -- throwing an exception is a useful alternative. */

/* ========================================================================= */
/*                           Messaging Classes                               */
/* ========================================================================= */

/*****************************************************************************/
/*                              kdu_pretty_buf                               */
/*****************************************************************************/

class kdu_pretty_buf : public std::streambuf {
  /* Supplying a pointer to an object of this class in the constructor for
     `ostream' will yield a stream object which delivers its output to
     the identified `output' stream after first formatting it for a given
     maximum line length. The maximum line length is currently 79 characters.
     Every attempt is made to avoid breaking words (space-delimited tokens)
     across lines. Tab characters are converted to a single space, unless
     they appear immmediately after the last new-line (or immediately after
     the object is created). In the latter case, each tab increments
     indentation for the paragraph (currently by 4 spaces). Indentation
     state is cancelled at the next new-line character. */
  public:
    kdu_pretty_buf(std::ostream *output, int max_line=79)
      { /* If `output' is NULL, no output will be generated at all. */
        if (max_line > 80) max_line = 80;
        dest = output; line_chars = max_line; num_chars = 0;
        max_indent = 40; indent = 0; master_indent = 0;
        no_output_since_newline = true;
      }
    ~kdu_pretty_buf() { sync(); }
    KDU_EXPORT void
      set_master_indent(int val);
      /* You may only call this function at the beginning of a paragraph, i.e.
         before outputting any characters since the last new-line. */
  protected:
    int underflow() { return EOF; } // Input operations meaningless.
    KDU_EXPORT int
      overflow(int nCh = EOF);
    KDU_EXPORT int
      sync();
  private:
    char line_buf[81];
    int line_chars; // Maximum number of characters per line. Must be <= 80.
    int num_chars; // Number of characters written to current line.
    int max_indent;
    int indent; // Indent to be applied until next new-line character.
    int master_indent; // Indent to be applied to all lines henceforth.
    bool no_output_since_newline;
    std::ostream *dest;
  };

/*****************************************************************************/
/*                                kdu_error                                  */
/*****************************************************************************/

  /* Objects of the `kdu_error' class may be created, written to and then
     destroyed as a compact and powerful mechanism for generating error
     messages and jumping out of the immediate execution context.  The default
     behaviour of the destructor is to exit the process; however, this may be
     overridden by use of the "kdu_customize_errors" function defined above. */
class kdu_error : public std::ostream {
  public:
    KDU_EXPORT
      kdu_error();
    KDU_EXPORT
      ~kdu_error();
  private:
    kdu_pretty_buf err_buf;
  };

/*****************************************************************************/
/*                               kdu_warning                                 */
/*****************************************************************************/

  /* Objects of the `kdu_warning' class are used in a similar manner to
     `kdu_error' objects, except that the default behaviour of the destructor
      is not to cause any interruption of the current execution context. */

class kdu_warning : public std::ostream {
  public:
    KDU_EXPORT
      kdu_warning();
    KDU_EXPORT
      ~kdu_warning();
  private:
    kdu_pretty_buf warn_buf;
  };

#endif // KDU_UTILS_H
