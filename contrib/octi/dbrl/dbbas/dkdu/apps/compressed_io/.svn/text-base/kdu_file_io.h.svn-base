/*****************************************************************************/
// File: kdu_file_io.h [scope = APPS/COMPRESSED-IO]
// Version: Kakadu, V2.2.1
// Author: David Taubman
// Last Revised: 5 July, 2001
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
   Defines classes derived from "kdu_compressed_source" and
"kdu_compressed_target" which may be used by applications to realize
simple file-oriented compressed data I/O.
******************************************************************************/

#ifndef KDU_FILE_IO_H
#define KDU_FILE_IO_H

#include <stdio.h> // Use C I/O functions for speed; can make a big difference
#include "kdu_compressed.h"

// Defined here:
class kdu_simple_file_source;
class kdu_simple_file_target;


/*****************************************************************************/
/*                          kdu_simple_file_source                           */
/*****************************************************************************/

class kdu_simple_file_source : public kdu_compressed_source {
  public: // Member functions
    kdu_simple_file_source()
      { file = NULL; }
    kdu_simple_file_source(char *fname)
      { open(fname); }
    ~kdu_simple_file_source()
      { close(); }
    bool exists()
      { return (file != NULL); }
    bool operator!()
      { return (file == NULL); }
    void open(char *fname)
      {
        file = fopen(fname,"rb");
        if (file == NULL)
          { kdu_error e;
            e << "Unable to open compressed data file, \"" << fname << "\"!"; }
      }
    int read(kdu_byte *buf, int num_bytes)
      {
        num_bytes = fread(buf,1,(size_t) num_bytes,file);
        return num_bytes;
      }
    void close()
      { // It is safe to close an object which was not opened.
        if (file != NULL)
          fclose(file);
        file = NULL;
      }
  private: // Data
    FILE *file;
  };

/*****************************************************************************/
/*                          kdu_simple_file_target                           */
/*****************************************************************************/

class kdu_simple_file_target : public kdu_compressed_target {
  public: // Member functions
    kdu_simple_file_target()
      { file = NULL; }
    kdu_simple_file_target(char *fname)
      { open(fname); }
    ~kdu_simple_file_target()
      { close(); }
    bool exists()
      { return (file != NULL); }
    bool operator!()
      { return (file == NULL); }
    void open(char *fname)
      {
        file = fopen(fname,"wb");
        if (file == NULL)
          { kdu_error e;
            e << "Unable to open compressed data file, \"" << fname << "\"!"; }
      }
    bool write(kdu_byte *buf, int num_bytes)
      {
        if (fwrite(buf,1,(size_t) num_bytes,file) < (size_t) num_bytes)
          return false;
        return true;
      }
    void close()
      { // It is safe to close an object which was not opened.
        if (file != NULL)
          fclose(file);
        file = NULL;
      }
  private: // Data
    FILE *file;
  };


#endif // KDU_FILE_IO_H
