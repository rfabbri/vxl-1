/******************************************************************************/
// File: region_decompressor.h [scope = APPS/SHOW]
// Version: Kakadu, V2.2
// Author: David Taubman
// Last Revised: 20 June, 2001
/******************************************************************************/
// Copyright 2001, David Taubman, The University of New South Wales (UNSW)
// The copyright owner is Unisearch Ltd, Australia (commercial arm of UNSW)
// Neither this copyright statement, nor the licensing details below
// may be removed from this file or dissociated from its contents.
/******************************************************************************/
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
/*******************************************************************************
Description:
   Defines incremental, robust, region-based decompression services through
the "kd_region_decompressor" object.  These services should prove useful
to many interactive applications which require JPEG2000 rendering capabilities.
*******************************************************************************/

#ifndef REGION_DECOMPRESSOR_H
#define REGION_DECOMPRESSOR_H

#include "kdu_compressed.h"
#include "kdu_sample_processing.h"
#include "jp2.h"

// Declared here
struct kdr_channel_mapping;
struct kdr_component;
struct kdr_channel;
class kdr_region_decompressor;

/******************************************************************************/
/*                             kdr_channel_mapping                            */
/******************************************************************************/

struct kdr_channel_mapping {
  public: // Member functions
    kdr_channel_mapping()
      {
        palette[0] = palette[1] = palette[2] = NULL;
        clear();
      }
    ~kdr_channel_mapping()
      { clear(); }
    void clear()
      {
        num_channels = 0; palette_bits = 0;
        for (int c=0; c < 3; c++)
          {
            source_components[c] = 0;
            if (palette[c] != NULL)
              delete[] palette[c];
            palette[c] = NULL;
          }
      }
  public: // Data
    int num_channels;
    int source_components[3];
    int palette_bits;
    kdu_sample16 *palette[3];
    jp2_colour colour;
  };
  /* Notes:
        This structure describes the procedures required to map source image
     components to colour channels.  The number of channels is either 1 or 3.
     The `source_components' array holds the indices of the image components
     which are used to generate each of the three colour channels (the first
     one if there is only one channel). Similarly, the `palette' array holds
     the palette lookup table to be applied to each source component to obtain
     the colour channel values.  The number of index bits for the palette
     lookup tables is given by `palette_bits' and each table is guaranteed to
     have 2^{palette_bits} entries.  If `palette_bits' is 0, there is no
     palette.  If any individual entry in the `palette' array is NULL, that
     colour channel is rendered directly from  its source component.  The
     entries in each palette lookup table are 16-bit fixed point values,
     having KDU_FIX_POINT fraction bits and representing normalized quantities
     having the nominal range of -0.5 to +0.5.  The `colour' object, if it
     exists (use the `exists' member function to check), identifies the
     colour space of the data and provides colour conversion capabilities
     which may be required. */

/******************************************************************************/
/*                                kdr_component                               */
/******************************************************************************/

struct kdr_component {
    int comp_idx; // This will be < 0 if component is not used.
    int bit_depth;
    bool is_signed;
    int palette_bits; // If != 0, samples will be converted to palette indices
    bool reversible;
    kdu_line_buf line;
    kdu_pull_ifc engine; // Decompression engine
    kdu_dims dims; // See notes below.
    kdu_coords sampling; // See notes below.
    kdu_coords interp; // See notes below.
    bool line_buf_valid; // False when `line' contains valid decompressed data
  };
  /* Notes:
        The `sampling' and `dims' members together identify the size, location
     and interpolation properties of the current tile region within this
     image component.
        The `dims' member holds the location and dimensions of
     the component region on the high resolution code-stream canvas, after
     possible after geometric transformation.  This is the information returned
     by the `kdu_tile_comp::get_dims' function.  Note that `dims.size.y' is
     decremented and `dims.pos.y' is incremented whenever a new line is
     decompressed.
        The `sampling' member identifies the spacing between image component
     samples, measured in rendering grid points.  This is essentially the
     expansion (or interpolation) factor required for this component.
        The `interp' member holds placement state information which is used
     for interpolating the component samples up to the rendering grid.
     Specifically, `interp.x' holds the number of rendering grid points
     which are considered to be closest to the left-most component sample.
     Our nearest neighbour interpolation rule sets all of these grid points
     equal to the first component sample and then moves on to the next
     component sample, copying it into the next `sampling.x' grid points and
     so forth.  `interp.y' initially has the same interpretation for the
     vertical direction.  Its value is decremented whenever a new channel line
     is produced; when it reaches 0, a new line must be decompressed and
     `interp.y' is incremented by `sampling.y'. */

/******************************************************************************/
/*                                 kdr_channel                                */
/******************************************************************************/

struct kdr_channel {
    kdr_component *source; // Source component for this channel.
    kdu_line_buf line; // Suitably interpolated and mapped image line.
    kdu_sample16 *lut; // Palette mapping LUT.  NULL if no palette.
  };
  /* Notes:
        The `line' buffer always holds 16-bit fixed point quantities.  When
     a palette is involved, these are obtained through the palette mapping.
     Otherwise, the values are obtained from the code-stream image component
     samples by performing appropriate conversion operations. */

/******************************************************************************/
/*                            kdr_region_decompressor                         */
/******************************************************************************/

class kdr_region_decompressor {
  public: // Member functions
    ~kdr_region_decompressor()
      { /* Deallocate resources, which might be left behind if a `finish'
           was still pending when the object was destroyed. */
        codestream_failure = true;
        finish();
      }
    void start(kdu_codestream codestream, kdr_channel_mapping *mapping,
               int single_component, int discard_levels, int max_layers,
               kdu_dims region, kdu_coords sampling);
      /* Prepares for decompression of a new region of interest.  If
         `mapping' is NULL, a single image component is to be decompressed,
         whose index is identified by `single_component'.  Otherwise, one
         or more components must be decompressed and subjected to potentially
         quite complex mapping rules to generate colour channels for display;
         the relevant components and mapping rules are identified by the
         `mapping' object.
            The `region' field identifies the image region which is to be
         decompressed.  This region is defined with respect to a rendering
         canvas on which each of the image components must have integer
         sub-sampling factors (this constraint could be dropped without too
         much difficulty, although it would make the implementation
         significantly less comprehensible.  The sub-sampling factors for the
         first colour channel (or the single image component) are given by
         the `sampling' argument. Note that these sub-sampling factors are
         not generally the same as those which describe the image components
         relative to the high resolution code-stream canvas coordinate
         system.  The sub-sampling factors for other colour channels and
         components are automatically derived from the code-stream, but they
         must also be found to have an integer relationship to the rendering
         grid.  That is, S_0*s_0 must be divisible by S_k, where the S_k
         are the code-stream sub-sampling factors, S_0 is the code-stream
         sub-sampling factor for the first channel and s_0 is the rendering
         sampling factor supplied via the `sampling' argument.  Of course,
         these relationships apply separately to each of the horizontal and
         vertical dimensions.
            The individual image components may have additional registration
         offsets, specified in the code-stream.  To clarify the relationship
         between these registration offsets and the rendering canvas, it is
         expected that all regions on the rendering canvas are derived with
         reference to the first channel (or the single image component) and
         its sub-sampling factors.  Specifically, the upper-left point on
         the rendering canvas should have coordinates
            [sampling.y*y0+floor((sampling.y-1)/2),
             sampling.x*x0+floor((sampling.x-1)/2)]
         where [y0,x0] are the coordinates of the upper-left point of the
         first channel or single image comonent.  Also, the image dimensions
         on the rendering canvas should be sampling.y*Y by sampling.x*X
         where the first channel or single image component has dimensions
         Y by X. */
    bool process(kdu_byte *buffer, int buffer_row_gap,
                 kdu_dims buffer_region, kdu_dims &incomplete_region,
                 int suggested_increment, kdu_dims &new_region);
      /* This powerful function is the workhorse of the "kdu_show" application.
         It is used to incrementally decompress an active region into the
         supplied buffer.  The function decompresses some whole number of
         lines of a single tile, where the total number of samples in those
         lines is approximately equal to the value of the `suggested_increment'
         argument, unless the value of that argument is smaller than a single
         line of the current tile, or larger than the number of samples
         remaining in the tile.  In this way, the newly decompressed lines
         are guaranteed to belong to a rectangular region -- the function
         returns this region via the `new_region' argument.  This and all
         other regions manipulated by the function are expressed in the
         rendering coordinate system (the coordinate system associated with
         the `region' argument supplied to the `start' member function).
            Decompressed samples are automatically colour transformed,
         clipped, level adjusted, interpolated and colour appearance
         transformed, as necessary.  They are then written into the supplied
         byte buffer.  The byte buffer invariably consists of interleaved
         BGR samples, so that a monochrome source's sample values will be
         replicated into each of the B, G and R planes.  The `buffer_row_gap'
         argument identifies the number of BGR pixels separating successive
         rows in the buffer.  The `buffer_region' argument identifies the
         region associated with the buffer.  Decompressed sample values which
         lie outside this region will be discarded.  This may happen for any
         number of reasons; for example, the user may scroll the window so
         that the originally requested region no longer lies entirely within
         the user's viewport.
            On entry, the `incomplete_region' structure identifies the subset
         of the original region supplied to `start', which has not yet been
         decompressed and is still relevant to the application.  It is always
         a subset of the `buffer_region'.  The function uses this information
         to avoid unnecessary processing of tiles which are no longer relevant.
            On exit, the upper boundary of the `incomplete_region' is updated
         to reflect any completely decompressed rows.  Once the region becomes
         empty, all processing is complete and future calls will return false.
            The function may return true, with `new_region' empty.  This can
         happen, for example, when skipping over a now-defunct tile.  The
         intent is to avoid forcing the caller to wait for new tiles to be
         loaded (possibly from disk).
            If the underlying code-stream is found to be sufficiently corrupt
         that the decompression process must be aborted, the current function
         will return false prematurely.  This condition will be evident from
         the fact that `incomplete_region' is non-empty.  You should still
         call `finish' and watch the return value from that function. */
    bool finish();
      /* Every call to `start' must be matched by a call to `finish'; however,
         you may call `finish' prematurely.  This allows processing to be
         terminated on a region whose intersection with a display window has
         become too small to justify the effort.
            If the function returns false, a fatal error has occurred in the
         underlying code-stream and you must destroy the codestream object
         (use `codestream.destroy').  This should clean up all the resources
         correctly, in preparation for subsequently opening a new code-stream
         for further decompression and rendering work. */
  private: // Implementation helpers
    void open_tile();
    void close_tile();
  private: // Data
    kdu_sample_allocator allocator;
    kdu_codestream codestream;
    bool codestream_failure; // True if an exception generated in `process'.
    kdu_dims valid_tiles;
    kdu_coords next_tile_idx; // Index to be used for next open tile call.
    kdu_tile current_tile;
    bool tile_open; // True if the current tile is open.
    bool use_ycc;
    kdu_dims render_dims; // Dimensions of current tile on rendering canvas
    kdr_component components[6]; // Can't need more than this number components
    int num_channels; // Number of channels to render (1 or 3).
    kdr_channel channels[3]; // Rendering output is channels, not components
    jp2_colour_space space; // For use in determining channel colour properties
    jp2_colour colour; // For use in implementing channel colour transforms
  };
  /* Notes:
        The `render_dims' member identifies the dimensions and location of the
     region which is being decompressed (a region of some tile) on the
     rendering canvas coordinate system (not the code-stream canvas coordinate
     system). Whenever a new line of channel data is produced, the
     `render_dims.pos.y' field is incremented and `render_dims.size.y' is
     decremented. */

#endif REGION_DECOMPRESSOR_H
