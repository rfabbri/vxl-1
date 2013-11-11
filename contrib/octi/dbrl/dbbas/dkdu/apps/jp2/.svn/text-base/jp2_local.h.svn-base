/*****************************************************************************/
// File: jp2.h [scope = APPS/JP2]
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
   Defines local classes used by the internal JP2 file format processing
machinery implemented in "jp2.cpp".  The internal object classes defined here
generally parallel the externally visible interface object classes defined
in the compressed-io header file, "jp2.h".  You should respect the local
nature of this header file and not include it directly from an application
or any other part of the Kakadu system (not in the APPS/JP2 scope).
******************************************************************************/
#ifndef JP2_LOCAL_H
#define JP2_LOCAL_H

#include <stdio.h> // The C I/O functions can be a lot faster than C++ ones.
#include <assert.h>
#include "jp2.h"

// Defined here
class j2_input_box;
class j2_output_box;
class j2_dimensions;
class j2_palette;
class j2_channels;
class j2_icc_profile;
class j2_colour;
class j2_resolution;
class j2_source;
class j2_target;

/*****************************************************************************/
/*                               j2_input_box                                */
/*****************************************************************************/

class j2_input_box {
  public: // Member functions
    j2_input_box()
      { box_type = 0; file = NULL; super_box = NULL; }
    ~j2_input_box()
      { // Closes the box if necessary. */
        close();
      }
    bool operator!()
      { return (box_type == 0); }
    bool exists()
      { return (box_type != 0); }
    j2_input_box &open(FILE *file)
      {
        assert(box_type == 0); // Must have been previously closed.
        this->file = file; this->super_box = NULL;
        read_header(); return *this;
      }
    j2_input_box &open(j2_input_box *super_box)
      {
        assert(box_type == 0); // Must have been previously closed.
        this->file = NULL; this->super_box = super_box;
        read_header(); return *this;
      }
      /* Both functions open a new input box, reading its length and
         signature fields so that subsequent input, if any, will be taken from
         the body of the box.  If the box is a sub-box, it takes its input
         from the body of an existing `super_box'.  Otherwise, it takes its
         input from the indicated `file' stream. */
    bool close();
      /* Skips over any outstanding bytes which have not been read from the
         body of the box.  The function returns false if there were some
         unread bytes, which may be an error or warning condition.  Otherwise,
         it returns true.  Also returns true if the box has already been
         closed.  Subsequent calls to `exists' will return false. */
    kdu_uint32 get_box_type()
      { return box_type; }
      /* Returns the box_type, which is usually a 4 character code.  Returns
         0 if the box could not be opened.  This is also the condition which
         causes `exists' to return false. */
    kdu_uint32 get_remaining_bytes()
      /* Returns the number of body bytes which have not yet been read from
         the box.  Returns 0xFFFFFFFF if the box has a rubber length or the
         length is too large to be represented in 32 bits. */
      {
        if (box_type == 0)
          return 0;
        else
          return (box_bytes==0)?0xFFFFFFFF:remaining_bytes;
      }
    int ignore(int num_bytes);
      /* Skips over up to `num_bytes' of the box's body, returning the number
         of bytes actually skipped. */
    int read(kdu_byte *buf, int num_bytes);
      /* Attempts to read the indicated number of bytes from the body of the
         box, filling the supplied buffer with these bytes.  Returns the
         actual number of bytes which were read, which may be smaller than
         `num_bytes' if the end of the box (or surrounding file) is
         encountered. */
    bool read(kdu_uint32 &dword);
      /* Reads a big-endian 4 byte word from the box, returning false if
         the source is exhausted. */
    bool read(kdu_uint16 &word);
      /* Reads a big-endian 2 byte word from the box, returning false if
         the source is exhausted. */
    bool read(kdu_byte &byte)
      { return (read(&byte,1) == 1); }
      /* Reads a single byte from the box, returning false if the source
         is exhausted. */
  private: // Helper functions
    void read_header();
  private: // Data
    kdu_uint32 box_type;
    kdu_uint32 box_bytes; // 0 if box has rubber length or length too large.
    kdu_uint32 remaining_bytes; // Unused if `box_bytes' = 0.
    FILE *file;
    j2_input_box *super_box;
  };

/*****************************************************************************/
/*                              j2_output_box                                */
/*****************************************************************************/

class j2_output_box {
  public: // Member functions
    j2_output_box()
      { box_type = 0; rubber_length = output_failed = false;
        super_box = NULL; file = NULL;
        buffer_size = buffered_bytes = 0; buffer = NULL; }
    ~j2_output_box()
      { close(); }
    j2_output_box &
      open(FILE *file, kdu_uint32 box_type, bool rubber_length=false)
      {
        assert((this->box_type == 0) && (buffer == NULL));
        this->box_type = box_type; this->rubber_length = rubber_length;
        this->super_box = NULL; this->file = file;
        buffer_size = buffered_bytes = 0; output_failed = false;
        if (rubber_length) write_header();
        return *this;
      }
    j2_output_box &
      open(j2_output_box *super_box, kdu_uint32 box_type,
           bool rubber_length=false)
      {
        assert((this->box_type == 0) && (buffer == NULL));
        this->box_type = box_type; this->rubber_length = rubber_length;
        this->super_box = super_box; this->file = NULL;
        buffer_size = buffered_bytes = 0; output_failed = false;
        if (rubber_length) write_header();
        return *this;
      }
      /* Both functions open a new output box with the indicated
         box_type (usually a 4 character code).  The second function is
         for opening sub-boxes, whose output will be directed to the body
         of the containing super-box.  If the `rubber_length' argument is
         false, the contents of the box will be buffered so that the
         final length of the box may be determined and written as the first
         field of the box.  Otherwise, the box will pass its contents
         directly through to the output as they appear.  Note that a box
         may be given a rubber length after it is opened (see below). */
    void set_rubber_length();
      /* Informs the box that it can flush any buffered data and pass all
         future body bytes directly to the output from now on.  This call is
         legal only when the box is already open.  The box will
         have a zero-valued length field.  This function is generally
         invoked whenever a sub-box is created with rubber length or is
         assigned rubber length by one of its own sub-boxes. */
    bool close();
      /* Flushes the contents of the box to the output.  If the box had
         rubber length, there will be nothing to flush and this function
         will return false, meaning that no further boxes may be opened.  The
         function may also return false if the box contents cannot be
         completely flushed to the output device for some reason (e.g., disk
         full).  If the box has already been closed (or never opened), this
         function returns true. */
    bool write(kdu_byte *buf, int num_bytes);
      /* Writes the indicated number of bytes to the body of the box, from
         the supplied buffer.  Returns false if the output device is unable
         to receive all of the supplied data (e.g., disk full). */
    bool write(kdu_uint32 dword);
      /* Writes a big-endian 32-bit unsigned integer to the body of the box,
         returning false if the output device is unable to receive all of
         the supplied data for some reason. */
    bool write(kdu_uint16 word);
      /* Writes a big-endian 16-bit unsigned integer to the body of the box,
         returning false if the output device is unable to receive all of
         the supplied data for some reason. */
    bool write(kdu_byte byte)
      { return write(&byte,1); }
      /* Writes a single byte to the body of the box, returning false if the
         output device is unable to receive all of the supplied data for some
         reason. */
  private: // Helper functions
    void write_header();
  private: // Data
    kdu_uint32 box_type; // 0 if box does not exist or has been closed.
    bool rubber_length;
    FILE *file;
    j2_output_box *super_box;
    int buffer_size, buffered_bytes;
    kdu_byte *buffer;
    bool output_failed; // True if the output device failed to absorb all data
  };

/*****************************************************************************/
/*                               j2_dimensions                               */
/*****************************************************************************/

class j2_dimensions {
  public: // Member functions
    j2_dimensions()
      { num_components = 0; bit_depths = NULL; }
    ~j2_dimensions()
      { if (bit_depths != NULL) delete[] bit_depths; }
    void init(j2_input_box *ihrd_box);
      /* Initializes the object from the information recorded in an image
         header box.  Note that the construction might not be complete
         (`finalize' may generate an error) if components have different
         bit-depths or signed/unsigned characteristics (see below).  Note
         also that the function closes the `ihdr_box' when done. */
    void init(kdu_coords size, int num_components, bool unknown_space);
      /* Does the work of the parallel `jp2_dimensions::init' function. */
    void process_bpcc_box(j2_input_box *bpcc_box);
      /* This function is called if a bits per component box is encountered
         while parsing a JP2 file.  The function closes the box before
         returning. */
    void finalize();
      /* Checks that the object has been completely initialized.  Generates
         an appropriate error otherwise. */
    void save_boxes(j2_output_box *super_box);
      /* Creates an image header box and, if necessary, a bits per component
         box, saving them as sub-boxes of the supplied `super_box'. */
  private: // Data
    friend class jp2_dimensions;
    kdu_coords size;
    int num_components;
    bool colour_space_unknown, ipr_box_available;
    int *bit_depths;
  };
  /* Notes:
        The `bit_depths' array holds one element for each image component
     with the magnitude identifying the actual number of bits used to
     represent the samples.  Negative values mean that the relevant
     component has a signed representation. */

/*****************************************************************************/
/*                                 j2_palette                                */
/*****************************************************************************/

class j2_palette {
  public: // Member functions
    j2_palette()
      { num_components = 0; num_entries = 0; bit_depths = NULL; luts = NULL; }
    ~j2_palette()
      {
        if (bit_depths != NULL) delete[] bit_depths;
        if (luts != NULL)
          {
            for (int c=0; c < num_components; c++)
              delete[] luts[c];
            delete[] luts;
          }
      }
    void init(j2_input_box *pclr_box);
      /* Initializes the object from the information recorded in a palette
         box.  Note that the function closes the `pclr_box' when done. */
    void init(int num_components, int num_entries);
      /* Does the work of the parallel `jp2_palette::init' function. */
    void finalize();
      /* Checks that the object has been completely initialized.  Generates
         an appropriate error otherwise. */
    void save_box(j2_output_box *super_box);
      /* Creates a palette box and saves it as a sub-box of the supplied
         `super-box'.  Does nothing if no palette information has been set. */
  private: // Data
    friend class jp2_palette;
    int num_components;
    int num_entries;
    int *bit_depths; // Magnitude identifies bit-depth; -ve values mean signed
    kdu_int32 **luts; // One LUT array for each component.
  };
  /* Notes:
        The values stored in the LUT's all have a signed representation and
     the binary values are all located in the most significant bit positions
     of the 32-bit signed words.
        The `bit_depths' entries should never specify bit-depths in excess of
     32 bits.  If the original values have a larger bit-depth, some of the
     least significant bits will be discarded to make them fit into the 32-bit
     word size. */

/*****************************************************************************/
/*                                j2_channels                                */
/*****************************************************************************/

class j2_channels {
  public: // Member functions
    j2_channels(); // Installs a default mapping.
    ~j2_channels()
      { if (cmap_channels != NULL) delete[] cmap_channels; }
    void process_cdef_box(j2_input_box *cdef_box);
    void process_cmap_box(j2_input_box *cmap_box);
      /* These functions may be called in any order, as the relevant boxes
         arrive.  You will need to call `finalize' once the available boxes
         have been processed. */
    void finalize(int num_colours, int num_components,
                  int num_palette_components);
      /* Call after all relevant JP2 header boxes have been read from an
         input file, or before saving boxes to an output file. */
    void save_boxes(j2_output_box *super_box);
      /* Generates either, none or both of the component mapping and
         channel definition boxes, as required by the standard. */
  private: // Structure definitions
      struct j2_channel {
          bool operator==(j2_channel &rhs)
            { return (source_component == rhs.source_component) &&
                     (palette_component == rhs.palette_component); }
          int source_component; // Index of image component or -1 if none
          int palette_component; // Index of palette LUT or -1 if none
        };
  private: // Data
    friend class jp2_channels;
    int num_colours; // Might not be known until `finalize' called; 0 till then
    int num_components; // Not known until `finalize' is called; 0 till then
    int num_palette_components; // Not known until `finalize'; 0 till then
    j2_channel channel_functions[9];

    int num_cmap_channels; // 0 if not processing component mapping (cmap) box
    j2_channel *cmap_channels; // NULL except when processing cmap box
  };
  /* Notes:
        The structure of this object is complicated by the most unfortunate
     approach to representing channel mapping relationships in JP2 files.
     The sum total of the information which can be represented by the
     component mapping (cmap) and channel definition (cdef) boxes in a JP2
     file is embodied by the 9 elements managed by the `channel_functions'
     array.  This array identifies the index of the code-stream image
     component used to create each colour channel function, as well as the
     index of any palette LUT to which the relevant image component samples
     must first be applied.  There are 9 possible functional roles played
     by channels in a JP2 file; any image component which does not contribute
     to any of these functional roles is undefined within the scope of JP2,
     although for some reason the standard suggests that these undefined
     components must be explicitly identified as such within cmap and/or
     cdef boxes.
        The functional roles are colour, opacity and pre-multiplied opacity.
     Accordingly, the first 3 entries in the `channel_functions' array
     identify the source component and palette mapping LUT for the red,
     green and blue colour values.  The next 3 entries identify the source
     component and palette mapping LUT for the red, green and blue opacity
     values; and the last 3 entries identify the source component and palette
     mapping LUT for the red, green and blue pre-multiplied opacity values.
     If there is only one colour (monochrome imagery), only the first entry
     in each of these 3 groups is actually used.
        In most cases, one or more of the functional roles might not be
     used.  The unused functional roles are identified by negative
     `source_component' indices.  Similarly, when a palette mapping is not
     used, the `palette_component' index shall be negative.  When palette
     mappings are used, the `palette_component' value must be in the range
     0 through `num_palette_components'-1, where `num_palette_components' is
     the number of components defined by a palette (pclr) box.
        When recovering the information in the `channel_functions' array
     from JP2 boxes, the relevant details are initially distributed between
     `channel_functions' and `cmap_channels' arrays until the point when
     the information can be merged together by the `finalize' function.  At
     that point, the `cmap_channels' array will be deleted and the
     relevant cmap quantities nulled out.  The reason for splitting information
     is that the order in which these boxes might appear is unpredictable
     (the whole thing is really quite crazy).  When a `cmap' box appears, the
     number of channels is determined and written into `num_cmap_channels'
     and the source component and palette LUT associated with each channel
     are written into the `cmap_channels' array.  When a `cdef' box appears,
     its contents are written into the `channel_functions' array.  During
     finalization, if a `cmap_channels' array exists, the `source_component'
     entries in the `channel_functions' array are treated as indices into
     the `cmap_channels' array, which holds the actual source component
     and palette LUT information for the relevant channel function.  Once
     the information has been transferred to the `channel_functions' array
     the `cmap_channels' array is destroyed. */

/*****************************************************************************/
/*                              j2_icc_profile                               */
/*****************************************************************************/

class j2_icc_profile {
  public: // Member functions
    j2_icc_profile()
      { buffer = NULL; num_buffer_bytes = 0; num_colours = 0; num_tags = 0; }
    ~j2_icc_profile()
      { if (buffer != NULL) delete[] buffer; }
    void init(kdu_byte *profile_buf, bool donate_buffer=false);
        // If `donate_buffer' is true, no internal copy of the buffer is made
    int get_num_colours()
      { return num_colours; }
    kdu_byte *get_profile_buf(int *num_bytes=NULL)
      { if (num_bytes != NULL)
          *num_bytes = num_buffer_bytes;
        return buffer; }
    void get_lut(int channel_idx, float lut[], int index_bits);
    void get_matrix(float matrix3x3[]);
      /* These various functions implement the services offered by the
         obvious `jp2_colour' member functions defined in "jp2.h". */
  private: /* Read functions: offset in bytes relative to start of buffer;
              returns false if requested address is beyond end of buffer. */
    bool read(kdu_byte &val, int offset)
      { if (offset >= num_buffer_bytes) return false;
        val = buffer[offset];
        return true; }
    bool read(kdu_uint16 &val, int offset)
      { if (offset >= (num_buffer_bytes-1)) return false;
        val = buffer[offset]; val = (val<<8) + buffer[offset+1];
        return true; }
    bool read(kdu_uint32 &val, int offset)
      { if (offset >= (num_buffer_bytes-3)) return false;
        val = buffer[offset]; val = (val<<8) + buffer[offset+1];
        val = (val<<8) + buffer[offset+2]; val = (val<<8) + buffer[offset+3];
        return true; }
  private: // Other helper functions
    int get_curve_data_offset(int tag_offset, int tag_length);
      /* Checks the tag signature and length information, generating
         an error if anything is wrong.  Then returns the offset of
         the data portion of the curve (starts with the number of
         points identifer). */
    int get_xyz_data_offset(int tag_offset, int tag_length);
      /* Checks the tag signature and length information, generating
         an error if anything is wrong.  Then returns the offset of
         the first XYZ number in the tag. */
  private: // Data
    kdu_byte *buffer;
    int num_buffer_bytes;
    int num_colours; // must be either 1 or 3.
    int num_tags;
    int trc_offsets[3]; // Offset to data portion of each trc curve.
    int colorant_offsets[3]; // Offset to the XYZ data.
  };

/*****************************************************************************/
/*                                 j2_colour                                 */
/*****************************************************************************/

class j2_colour {
  public: // Member functions
    j2_colour()
      { num_colours = 0; icc_profile = NULL; srgb_curve = NULL;
        icc_curves[0] = icc_curves[1] = icc_curves[2] = NULL; }
    ~j2_colour()
      {
        if (icc_profile != NULL) delete icc_profile;
        for (int c=0; c < 3; c++)
          if (icc_curves[c] != NULL) delete[] icc_curves[c];
        if (srgb_curve != NULL) delete[] srgb_curve;
      }
    void init(j2_input_box *colr_box);
      /* Initializes the object from the information recorded in a colour
         description box.  Note that the constructor closes the
         `colr_box' when done.  If a colour box has already been encountered,
         the current box is closed immediately and its contents ignored. */
    void init(jp2_colour_space space);
      /* Does the work of the parallel `jp2_colour::init' function. */
    void init(j2_icc_profile *profile);
      /* Constructs the object to represent colour through an ICC profile.
         The supplied `profile' object has been dynamically allocated by
         the caller and is to be deleted from within the current object's
         destructor. */
    void finalize();
      /* Checks that the object has been correctly initialized, generating
         an error if not. */
    void save_box(j2_output_box *super_box);
      /* Creates a colour description box and saves it as a sub-box of the
         supplied `super-box'. */
  private: // Data
    friend class jp2_colour;
    int num_colours;
    jp2_colour_space space;
    j2_icc_profile *icc_profile;
    kdu_int16 *icc_curves[3]; // 3 LUTs to convert channel data to linear
    kdu_int32 icc_matrix[9];
    kdu_sample16 *srgb_curve; // One LUT to convert linear data to sRGB
  };
  /* Notes:
        The `icc_curves' array contains either NULL pointers or else pointers
     to 3 lookup tables, one for each colour channel, indicating the tone
     reproduction curve which is to be applied (actually inverted) to obtain
     linear data for colour conversion purposes.  These LUT's are used only
     if RGB data described by an ICC profile is to be converted to sRGB.  In
     this case, the indices to the lookup tables have 10 bits and the
     outputs of the lookup tables are integers in the range 0 to 2^{15}-1,
     where the maximum value has a numerical interpretation of 1.  These
     outputs are subjected to colour matrixing.  That is, the 3 16-bit
     integers output by the three icc curves are treated as a column vector
     which is multiplied on the left by the 3x3 matrix in `icc_matrix'.  The
     resulting 3 32-bit integers are divided by 2^{16} to obtain integers
     with a nominal range of 0 to 2^{12}-1.  The values are clipped to this
     range to form the 12-bit index to a 4096 element lookup table,
     `srgb_curve'.  Its outputs are signed fixed point quantities in the
     range -0.5 to +0.5, having KDU_FIX_POINT fraction bits (i.e., the table
     incoporates the offset to create signed values.
        If an iccLUM space is to be converted to an sLUM space, only one
     lookup table is required.  In this case, the `icc_curves' entries are
     to be NULL, the `icc_matrix' is ignored and the `srgb_curve has 1024
     entries, embodying the end to end mapping into and out of the linear
     space. */

/*****************************************************************************/
/*                               j2_resolution                               */
/*****************************************************************************/

class j2_resolution {
  public: // Member functions
    j2_resolution()
      { display_ratio = capture_ratio = 0.0F;
        display_res = capture_res = 0.0F; }
      /* Does the work of the parallel `jp2_resolution::init' function. */
    void init(float aspect_ratio = 1.0F);
    void init(j2_input_box *res_box);
      /* Initializes the object from the information recorded in a resolution
         box.  Note that the function closes the `res_box' when done. */
    void finalize();
      /* Checks that the object has been completely initialized.  Generates
         an appropriate error otherwise. */
    void save_box(j2_output_box *super_box);
      /* Creates a resolution box and appropriate sub-boxes inside the
         supplied `super-box'. */
  private: // Helper functions
    void parse_sub_box(j2_input_box *box);
      /* Parses resolution values from either type of resolution sub-box.
         Closes the box for us. */
    void save_sub_box(j2_output_box *super_box, kdu_uint32 box_type,
                      double v_res, double h_res);
      /* Creates a capture or display resolution box and writes the supplied
         vertical and horizontal resolution values into that box. */
  private: // Data
    friend class jp2_resolution;
    float display_ratio;
    float capture_ratio;
    float display_res; // > 0 if and only if display info available
    float capture_res; // > 0 if and only if capture info available
  };

/*****************************************************************************/
/*                                 j2_source                                 */
/*****************************************************************************/

class j2_source {
  public: // Member functions
    void init(char *fname);
  private: // Data
    friend class jp2_source;
    j2_dimensions dimensions;
    j2_colour colour;
    j2_palette palette;
    j2_channels channels;
    j2_resolution resolution;
    j2_input_box box;
    FILE *file;
  };

/*****************************************************************************/
/*                                 j2_target                                 */
/*****************************************************************************/

class j2_target {
  public: // Member functions
    j2_target()
      { initialized = false; }
    void init();
  private: // Data
    friend class jp2_target;
    j2_dimensions dimensions;
    j2_colour colour;
    j2_palette palette;
    j2_channels channels;
    j2_resolution resolution;
    j2_output_box box;
    FILE *file;
    bool initialized;
  };

#endif // JP2_LOCAL_H
