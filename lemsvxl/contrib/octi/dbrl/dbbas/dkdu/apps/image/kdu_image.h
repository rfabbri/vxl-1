/*****************************************************************************/
// File: kdu_image.h [scope = APPS/IMAGE-IO]
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
   Not central to the Kakadu framework, this file defines generic image
file I/O interfaces, which can be used to isolate file-based applications from
the details of particular image file formats.
   Supports both bottom-up and top-down image file organizations with
comparable ease.
******************************************************************************/

#ifndef KDU_IMAGE
#define KDU_IMAGE

#include <assert.h>
#include <stdlib.h> // To get `abs'
#include "kdu_elementary.h"
#include "kdu_params.h"

// Defined here:

class kdu_image_dims;
struct kdu_rgb8_palette;
class kdu_image_in_base;
class kdu_image_in;
class kdu_image_out_base;
class kdu_image_out;

// Defined elsewhere.

class kdu_line_buf;
class siz_params;

/*****************************************************************************/
/*                              kdu_image_dims                               */
/*****************************************************************************/

class kdu_image_dims {
  public: // Member functions
    kdu_image_dims()
      { num_components = max_components = 0; data = NULL; }
    ~kdu_image_dims()
      { if (data != NULL) delete[] data; }
    void add_component(int height, int width, int bit_depth, bool is_signed)
      {
        assert(bit_depth > 0);
        if (num_components == max_components)
          {
            max_components += 10;
            int *tdata=new int[max_components*3];
            for (int i=0; i < 3*num_components; i++)
              tdata[i] = data[i];
            if (data != NULL) delete[] data;
            data = tdata;
          }
        data[num_components*3+0] = height;
        data[num_components*3+1] = width;
        data[num_components*3+2] = (is_signed)?(-bit_depth):bit_depth;
        num_components++;
      }
    int get_num_components()
      { return num_components; }
    int get_height(int comp_idx)
      { assert((comp_idx >= 0) && (comp_idx < num_components));
        return data[3*comp_idx+0]; }
    int get_width(int comp_idx)
      { assert((comp_idx >= 0) && (comp_idx < num_components));
        return data[3*comp_idx+1]; }
    int get_bit_depth(int comp_idx)
      { assert((comp_idx >= 0) && (comp_idx < num_components));
        return abs(data[3*comp_idx+2]); }
    bool get_signed(int comp_idx)
      { assert((comp_idx >= 0) && (comp_idx < num_components));
        return (data[3*comp_idx+2] < 0); }
  private: // Data
    int num_components;
    int max_components;
    int *data;
  };
  /* Notes:
        The `data' array holds 3 integers for each component.  The first
     integer is the height; the second is the width; the third has a
     magnitude which is equal to the bit-depth and a sign which is
     negative for signed data and positive for unsigned data. */

/*****************************************************************************/
/*                             kdu_rgb8_palette                              */
/*****************************************************************************/

struct kdu_rgb8_palette {
  public: // Member functions
    kdu_rgb8_palette()
      { input_bits = output_bits = source_component = 0; }
    bool exists()
      { return (input_bits > 0) && (output_bits > 0); }
    bool operator!()
      { return !exists(); }
    bool is_monochrome()
      { for (int n=0; n < (1<<input_bits); n++)
          if ((red[n] != green[n]) || (red[n] != blue[n]))
            return false;
        return true; }
    void rearrange(kdu_byte *map);
      /* Searches for a permutation of the palette entries which will
         optimize neighbourhood properties. Specifically, tries to minimize
         the sum of the distances between adjacent palette entries where
         distance is assessed as the sum of the absolute values of the
         colour component differences.  This is nothing other than the
         well-known travelling salesman problem, which is known to be
         NP-complete.  Consequently, the solution will generally be
         sub-optimal.  The algorithm deliberately starts with the original
         palette order and tries to incrementally improve upon it, so that
         in the event that the palette already has good neighbourhood
         properties (e.g., a previously optimized palette, a "heat" map or
         something like that), the function will execute quickly and without
         damaging the original palette's properties.  The permutation is
         written into the supplied `map' array, which is understood as a
         lookup table whose inputs are the original palette indices and whose
         outputs are the new palette indices. */
  public: // Data
    int input_bits;
    int output_bits;
    int source_component;
    kdu_int32 red[256];
    kdu_int32 green[256];
    kdu_int32 blue[256];
  };
  /* Notes:
        This simple structure may be used to convey information about an
     RGB colour palette, indexed by up to 8 bit component sample values.
     If the index has more than 8 bits, it is probably better to compress
     the de-palettized colour samples rather than the palette index.
     Accordingly, `input_bits' must be no larger than 8.  The
     `source_component' field is set to the index (starting from 0) of the
     image component which carries the palette information.  The
     `output_bits' field identifies the bit-depth of the palette colour
     values in the `red', `green' and `blue' arrays.  The palette entries
     are expected to be unsigned quantities (storing them in a signed
     format simplifies interaction with the JP2 file manager's `jp2_palette'
     object.  Accordingly, `output_bits' may not exceed 31. */

/*****************************************************************************/
/*                              kdu_image_in                                 */
/*****************************************************************************/

class kdu_image_in_base {
  /* Pure virtual base class. Provides an interface to derived classes which
     each support reading of a specific file type. */
  public: // Single interface function.
    virtual ~kdu_image_in_base() {}
    virtual bool get(int comp_idx, kdu_line_buf &line, int x_tnum) = 0;
  };

class kdu_image_in {
  /* Supports reading one or more components from a single image file. */
  public: // Member functions
    kdu_image_in() { in = NULL; }
    kdu_image_in(char const *fname, siz_params *siz, int &next_comp_idx,
                 bool &vflip, kdu_rgb8_palette *palette=NULL);
      /* The constructor attempts to open a file with the indicated name,
         generating its own error message if unsuccessful. It automatically
         attaches the correct file reading class, derived from the
         `kdu_image_in_base' interface. The index of the first image component
         represented by the file is identified by the `next_comp_idx'
         argument. Upon return, the value of this argument is updated to
         reflect the new total number of components.  It is the job of the
         caller to write this total number of image components into the
         `siz' object's `Snum_components' attribute.
             If the dimensions of the new image components can be deduced
         from the file header, the function sets the relevant dimensions in
         the `Sdims' attribute of the supplied `siz' object.
         Similarly, if the sample bit-depth or the signed/unsigned character
         of the image samples can be deduced from a file header (or the file
         type), they are written to the `Sprecision' and
         `Ssigned' attributes  of the `siz' object.  Otherwise,
         the function expects to extract these attributes from the `siz'
         object and generates an error if they cannot be found.
             The `vflip' argument is provided in support of image file formats
         in which the image lines are organized in bottom-up fashion, as
         opposed to the more conventional top-down organization.  The
         well-known BMP file format is perhaps the most significant
         bottom-up organization.  Upon return, `vflip' is true if the
         image lines are going to be supplied in bottom-up fashion, so
         that the compressor is expected to write the code-stream in
         reverse order (this exploits the geometric transformation
         capability in JPEG2000 so as to avoid buffering up the entire
         image in memory and compressing it in top-down fashion).  Otherwise,
         the function returns with `vflip' set to false.
            The `palette' argument may be non-NULL if the compressed file
         format supports signalling of colour palette information.  BMP
         files, for example, may contain palettized data.  In this case,
         the object's entries are filled out to reflect the index of the
         image component to which the palette is applied, along with the
         palette itself.  Palettes which require more than 8-bit indices
         should be expanded directly by the file reader.  If the `palette'
         object (structure) has its `input_bits' field set to a non-zero
         value already, the palette is in use and a new palette cannot be
         created for the present image file. In this case, or if no palette
         is supplied, the image sample values are to be depalettized by
         the image reader, which will normally provide a full set of
         colour channels. */
    void destroy()
      { assert(in != NULL); delete in; in = NULL; }
      /* Note: instances of the `kdu_image_in' class are only interfaces to the
         underlying image reading object.  As such, they may be copied at
         will.  To avoid the underlying object being destroyed when an
         interface to it goes out of scope, we provide an explicit destructor
         function.  This is simpler and more obvious, if slightly more
         dangerous than reference counting.
            Generates a warning message if one or more image lines were not
         consumed from any component. */
    bool exists()
      { return (in==NULL)?false:true; }
    bool operator!()
      { return (in==NULL)?true:false; }
    bool get(int comp_idx, kdu_line_buf &line, int x_tnum)
      { return in->get(comp_idx,line,x_tnum); }
      /* Retrieves a new tile-line from the image component identified by
         `comp_idx', or returns false if the input file is exhausted.
         `comp_idx' must lie within the range of valid component indices for
         this object.
            `x_tnum' must contain the horizontal tile index (starting from 0).
         This is used to determine how much of each line has already been read
         and so control internal line buffering.  We leave line buffering to
         the image reading object itself, since it knows the smallest amount
         of memory which can actually be buffered.
            To simplify line buffering, we insist on the following rules which
         should not prove burdensome: 1) for each line, tiles should be read
         from left to right; 2) the number of tiles read from earlier lines
         should be at least as large as the number of tiles read from later
         lines; 3) when multiple components are managed by the same object,
         all components should be read for a given tile within a given line
         before moving to the next tile of that line and the components should
         be read in order.
            Sample values in the line buffer are always signed quantities.
         For normalized (floating or fixed point) sample values, the nominal
         range is from -0.5 to 0.5, while for absolute integer values, the
         range is from -2^{B-1} to 2^{B-1}, where B is the bit-depth.  The
         function takes care of renormalizing, and conversion between signed
         and unsigned data types. */
  private: // Data
    class kdu_image_in_base *in;
  };

/*****************************************************************************/
/*                             kdu_image_out                                 */
/*****************************************************************************/

class kdu_image_out_base {
  /* Pure virtual base class. Provides an interface to derived classes which
     each support writing of a specific file type. */
  public: // Single interface function.
    virtual ~kdu_image_out_base() {}
    virtual void put(int comp_idx, kdu_line_buf &line, int x_tnum) = 0;
  };

class kdu_image_out {
  public: // Member functions
    kdu_image_out() { out = NULL; }
    kdu_image_out(char const *fname, kdu_image_dims &dims, int &next_comp_idx,
                  bool &vflip);
      /* The constructor attempts to open a file with the indicated name,
         generating its own error message if unsuccessful. It automatically
         attaches the correct file writing class, derived from the
         `kdu_image_out_base' interface. The index of the first image component
         (or channel) represented by the file is identified by the
         `next_comp_idx' argument.  Upon return, the value of this argument
         is updated to reflect the new total number of components (or
         channels).
             The `vflip' argument has the same interpretation and role as
         its namesake in the `kdu_image_in' object's constructor.  See the
         comments there for more information.
             The dimensions of the image components, along with their
         bit-depths and signed/unsigned characteristics are all deduced from
         the supplied `kdu_image_dims' object.  Note the difference between
         the methods used to pass dimensions to/from the `kdu_image_in'
         object and the `kdu_image_out' object.  The former object needs to
         take advantage of the rich capabilities offered by the `siz_params'
         class to extrapolated limited information which may be present
         concerning dimensions (for raw files), while the present object
         is not constructed until all dimensional information is already
         available from the JPEG2000 code-stream.  Also, when compressing
         image data, all original image components must be available from
         files, while decompressors may choose to decompress only a limited
         set of image components, or may need to map components through
         expansive palettes, introducing fundamental differences between
         the information represented by the code-stream SIZ marker segment
         and the dimensions of generated image files.  If the `dims'
         object reports insufficient components, a terminal error is
         generated. */
    void destroy()
      { assert(out != NULL); delete out; out = NULL; }
      /* Note: instances of the `kdu_image_out' class are only interfaces to
         the underlying image writing object.  As such, they may be copied at
         will.  To avoid the underlying object being destroyed when an
         interface to it goes out of scope, we provide an explicit destructor
         function.  This is simpler and more obvious, if slightly more
         dangerous than reference counting.
            Generates a warning message if one or more image lines were not
         written to any component. */
    bool exists()
      { return (out==NULL)?false:true; }
    bool operator!()
      { return (out==NULL)?true:false; }
    void put(int comp_idx, kdu_line_buf &line, int x_tnum)
      { out->put(comp_idx,line,x_tnum); }
      /* Writes a new tile-line to the image component identified by
         `comp_idx'. `comp_idx' must lie within the range of valid component
         indices for this object.
            `x_tnum' must contain the horizontal tile index.
         This is used to determine how much of each line has already been
         written and so control internal line buffering.  We leave line
         buffering to the image writing object itself, since it knows the
         smallest amount of memory which can actually be buffered.
            To simplify line buffering, we insist on the following rules which
         should not prove burdensome: 1) for each line, tiles should be written
         from left to right; 2) the number of tiles written to earlier lines
         should be at least as large as the number of tiles written to later
         lines; 3) when multiple components are managed by the same object,
         all components should be written for a given tile within a given line
         before moving to the next tile of that line and the components should
         be written in order.
            Sample values in the line buffer are always signed quantities.
         For normalized (floating or fixed point) sample values, the nominal
         range is from -0.5 to 0.5, while for absolute integers, the range is
         from -2^{B-1} to 2^{B-1}, where B is the bit-depth.  The function
         takes care of renormalizing, truncating, rounding and conversion
         between signed and unsigned data types. */
  private: // Data
    class kdu_image_out_base *out;
  };

#endif // KDU_IMAGE
