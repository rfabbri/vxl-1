/*****************************************************************************/
// File: params_local.h [scope = CORESYS/PARAMETERS]
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
   Local definitions for use by "params.cpp".  These should not be included
from any other scope.
******************************************************************************/

#ifndef PARAMS_LOCAL_H
#define PARAMS_LOCAL_H

// Defined here:

struct att_val;
struct kd_attribute;

/*****************************************************************************/
/*                                    att_val                                */
/*****************************************************************************/

struct att_val {
  /* Stores a single attribute value.  If `pattern' points to a string
     whose first character is 'F', the value is a floating point quantity.
     Otherwise, the value is an integer. */
    att_val()
      { is_set = false; pattern=NULL; }
    union {
      int ival;
      float fval;
      };
    char const *pattern;
    bool is_set;
  };

/*****************************************************************************/
/*                                  kd_attribute                             */
/*****************************************************************************/

struct kd_attribute {
  /* Objects of this class are used to build a linked list of attributes,
     which are managed by the kdu_params class. An attribute may contain
     one or more parameter records, each of which may contain one or more
     fields. Each field may have a different data type and  interpretation. */
  public: // Member functions
    kd_attribute(const char *name, const char *comment,
                 int flags, const char *pattern);
      /* See the definition of `kdu_params::define_attribute'. */
    ~kd_attribute()
      { delete[](values); }
    void augment_records(int new_records);
    void describe(std::ostream &stream, bool allow_tiles, bool allow_comps,
                  bool include_comments);
  public: // Data
    const char *name; // See constructor.
    const char *comment; // See constructor.
    int flags; // See constructor.
    const char *pattern; // See constructor.
    int num_fields; // Number of fields in each record (i.e., in `pattern').
    int num_records; // Number of records which have actually been written.
    att_val *values; // Array of `max_records'*`num_fields' values.
    bool derived; // Set using the `kdu_params::set_derived' function.
    bool parsed; // Set if the information was obtained by string parsing.
    kd_attribute *next; // Used to build linked list within kdu_params class
  private:
    int max_records; // Maximum storage available in `values' array.
  };

#endif // PARAMS_LOCAL_H
