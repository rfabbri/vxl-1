/*****************************************************************************/
// File: image_local.h [scope = APPS/IMAGE-IO]
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
   Local class definitions used by the implementation in "image_in.cpp" and
"image_out.cpp".  These should not be included from any other scope.
******************************************************************************/

#ifndef IMAGE_LOCAL_H
#define IMAGE_LOCAL_H

#include <stdio.h> // C I/O functions can be quite a bit faster than C++ ones
#include "kdu_elementary.h"
#include "kdu_image.h"

// Defined here.

struct image_line_buf;
class pgm_in;
class pgm_out;
class ppm_in;
class ppm_out;
class raw_in;
class raw_out;
struct bmp_header;
class bmp_in;
class bmp_out;


/*****************************************************************************/
/*                            image_line_buf                                 */
/*****************************************************************************/

struct image_line_buf {
  public: // Member functions
    image_line_buf(int width, int sample_bytes)
      {
        this->width = width;
        this->sample_bytes = sample_bytes;
        this->buf = new kdu_byte[width*sample_bytes];
        next = NULL;
        accessed_samples = 0;
        next_x_tnum = 0;
      }
    ~image_line_buf()
      { delete[] buf; }
  public:  // Data
    kdu_byte *buf;
    int sample_bytes;
    int width;
    int accessed_samples;
    int next_x_tnum;
    image_line_buf *next;
  };
  /* Notes:
     This structure provides a convenient mechanism for buffering
     tile-component lines as they are generated or consumed.  The `width'
     field indicates the number of samples in the line, while `sample_bytes'
     indicates the number of bytes used to represent each sample (the most
     significant byte always comes first).  The `accessed_samples' field
     indicates the number of samples which have already been read from or
     written into the line.  The `next_x_tnum' field holds the value of the
     `x_tnum' argument which should be expected in the next `get' or `put'
     call relevant to this line.  The `next' field may be used to build a
     linked list of buffered image lines. */

/*****************************************************************************/
/*                             class pbm_in                                  */
/*****************************************************************************/

class pbm_in : public kdu_image_in_base {
  public: // Member functions
    pbm_in(char const *fname, siz_params *siz, int &next_comp_idx,
           kdu_rgb8_palette *palette);
    ~pbm_in();
    bool get(int comp_idx, kdu_line_buf &line, int x_tnum);
  private: // Data
    int comp_idx;
    int rows, cols;
    image_line_buf *incomplete_lines; // Points to first incomplete line.
    image_line_buf *free_lines; // List of line buffers not currently in use.
    int num_unread_rows;
    FILE *in;
  };

/*****************************************************************************/
/*                             class pgm_in                                  */
/*****************************************************************************/

class pgm_in : public kdu_image_in_base {
  public: // Member functions
    pgm_in(char const *fname, siz_params *siz, int &next_comp_idx);
    ~pgm_in();
    bool get(int comp_idx, kdu_line_buf &line, int x_tnum);
  private: // Data
    int comp_idx;
    int rows, cols;
    image_line_buf *incomplete_lines; // Points to first incomplete line.
    image_line_buf *free_lines; // List of line buffers not currently in use.
    int num_unread_rows;
    FILE *in;
  };

/*****************************************************************************/
/*                             class pgm_out                                 */
/*****************************************************************************/

class pgm_out : public kdu_image_out_base {
  public: // Member functions
    pgm_out(char const *fname, kdu_image_dims &dims, int &next_comp_idx);
    ~pgm_out();
    void put(int comp_idx, kdu_line_buf &line, int x_tnum);
  private: // Data
    int comp_idx;
    int rows, cols;
    int precision;
    image_line_buf *incomplete_lines; // Points to first incomplete line.
    image_line_buf *free_lines; // List of line buffers not currently in use.
    int num_unwritten_rows;
    FILE *out;
  };

/*****************************************************************************/
/*                             class ppm_in                                  */
/*****************************************************************************/

class ppm_in : public kdu_image_in_base {
  public: // Member functions
    ppm_in(char const *fname, siz_params *siz, int &next_comp_idx);
    ~ppm_in();
    bool get(int comp_idx, kdu_line_buf &line, int x_tnum);
  private: // Data
    int first_comp_idx;
    int rows, cols;
    image_line_buf *incomplete_lines; // Each "sample" consists of 3 bytes.
    image_line_buf *free_lines;
    int num_unread_rows;
    FILE *in;
  };

/*****************************************************************************/
/*                             class ppm_out                                 */
/*****************************************************************************/

class ppm_out : public kdu_image_out_base {
  public: // Member functions
    ppm_out(char const *fname, kdu_image_dims &dims, int &next_comp_idx);
    ~ppm_out();
    void put(int comp_idx, kdu_line_buf &line, int x_tnum);
  private: // Data
    int first_comp_idx;
    int rows, cols;
    int precision[3];
    image_line_buf *incomplete_lines; // Each "sample" consists of 3 bytes.
    image_line_buf *free_lines;
    int num_unwritten_rows;
    FILE *out;
  };

/*****************************************************************************/
/*                             class raw_in                                  */
/*****************************************************************************/

class raw_in : public kdu_image_in_base {
  public: // Member functions
    raw_in(char const *fname, siz_params *siz, int &next_comp_idx);
    ~raw_in();
    bool get(int comp_idx, kdu_line_buf &line, int x_tnum);
  private: // Data
    int comp_idx;
    int rows, cols;
    int precision;
    int sample_bytes;
    bool is_signed;
    image_line_buf *incomplete_lines;
    image_line_buf *free_lines;
    int num_unread_rows;
    FILE *in;
  };

/*****************************************************************************/
/*                             class raw_out                                 */
/*****************************************************************************/

class raw_out : public kdu_image_out_base {
  public: // Member functions
    raw_out(char const *fname, kdu_image_dims &dims, int &next_comp_idx);
    ~raw_out();
    void put(int comp_idx, kdu_line_buf &line, int x_tnum);
  private: // Data
    int comp_idx;
    int rows, cols;
    int precision;
    int sample_bytes;
    bool is_signed;
    image_line_buf *incomplete_lines;
    image_line_buf *free_lines;
    int num_unwritten_rows;
    FILE *out;
  };

/*****************************************************************************/
/*                              bmp_header                                   */
/*****************************************************************************/

struct bmp_header {
    kdu_uint32 size; // Size of this structure: must be 40
    kdu_int32 width; // Image width
    kdu_int32 height; // Image height; -ve means top to bottom.
    kdu_uint16 planes; // Must be 1
    kdu_uint16 bit_count; // Must be 1, 4, 8 or 24
    kdu_uint32 compression; // Only accept 0 here (uncompressed RGB data)
    kdu_uint32 image_size; // Can be 0
    kdu_int32 xpels_per_metre; // We ignore these
    kdu_int32 ypels_per_metre; // We ignore these
    kdu_uint32 num_colours_used; // Entries in colour table (0 means use default)
    kdu_uint32 num_colours_important; // 0 means all colours are important.
  };
  /* Notes:
        This header structure must be preceded by a 14 byte field, whose
     first 2 bytes contain the string, "BM", and whose next 4 bytes contain
     the length of the entire file.  The next 4 bytes must be 0. The final
     4 bytes provides an offset from the start of the file to the first byte
     of image sample data.
        If the bit_count is 1, 4 or 8, the structure must be followed by
     a colour lookup table, with 4 bytes per entry, the first 3 of which
     identify the blue, green and red intensities, respectively. */

/*****************************************************************************/
/*                             class bmp_in                                  */
/*****************************************************************************/

class bmp_in : public kdu_image_in_base {
  public: // Member functions
    bmp_in(char const *fname, siz_params *siz, int &next_comp_idx,
           bool &vflip, kdu_rgb8_palette *palette);
    ~bmp_in();
    bool get(int comp_idx, kdu_line_buf &line, int x_tnum);
  private:
    void map_palette_index_bytes(kdu_byte *buf, bool absolute);
    void map_palette_index_nibbles(kdu_byte *buf, bool absolute);
    void map_palette_index_bits(kdu_byte *buf, bool absolute);
  private: // Data
    int first_comp_idx;
    int num_components;
    bool bytes, nibbles, bits; // Only when reading palette data
    bool expand_palette; // True if palette is to be applied while reading.
    kdu_byte map[1024];
    int rows, cols;
    int line_bytes; // Number of bytes in a single line of the BMP file.
    image_line_buf *incomplete_lines; // Each "sample" represents a full pixel
    image_line_buf *free_lines;
    int num_unread_rows;
    FILE *in;
  };
  /* Notes:
        `num_components' is either 1 or 3.  It holds the number of image
     components produced by the reader.  If  `bytes', `nibbles' or `bits'
     is true, the file contains palette indices, which must be unpacked
     (except when the indices are bytes) and mapped.
        If `expand_palette' is true, the palette indices must be mapped
     directly to RGB values and the `map' array holds interleaved palette
     information.  Each group of four bytes corresponds to a single palette
     index: the first byte is the blue colour value, the second is green and
     the third is red.  In this case, `num_components' may be 3 or 1.  If 1,
     the palette is monochrome and only one component will be expanded.
        If `expand_palette' is false, the number of components must be 1.  In
     this case, the palette indices themselves are to be compressed as a
     single image component; however, they must first be subjected to a
     permutation mapping, which rearranges the palette in a manner more
     amenable to compression.  In this case, the palette indices are applied
     directly to the `map' lookup table, which returns the mapped index in
     its low order 1, 4 or 8 bits, as appropriate. */

/*****************************************************************************/
/*                             class bmp_out                                 */
/*****************************************************************************/

class bmp_out : public kdu_image_out_base {
  public: // Member functions
    bmp_out(char const *fname, kdu_image_dims &dims, int &next_comp_idx);
    ~bmp_out();
    void put(int comp_idx, kdu_line_buf &line, int x_tnum);
  private: // Data
    int first_comp_idx;
    int num_components;
    int rows, cols;
    int alignment_bytes; // Number of 0's at end of each line.
    int precision[3];
    image_line_buf *incomplete_lines; // Each "sample" represents a full pixel
    image_line_buf *free_lines;
    int num_unwritten_rows;
    FILE *out;
  };

#endif // IMAGE_LOCAL_H
