/*****************************************************************************/
// File: simple_example_d.cpp [scope = APPS/SIMPLE_EXAMPLE]
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
   Simple example showing decompression into an intermediate buffer for the
image samples.  This is, of course, a great waste of memory and the more
sophisticated "kdu_expand" application does no such thing.  This is also far
from the recommended procedure for interactive applications, for two reasons:
1) a responsive interactive application should incrementally update the
display as it goes, processing samples in order of their relevance to the
portion of the image actually being displayed; and 2) interactive applications
usually maintain a viewing portal into a potentially much larger image, so
it is preferable to decompress only the image regions which are visible,
thereby avoiding the cost of buffering the entire decompressed image.  A
far superior approach for interactive applications is demonstrated by the
"kdu_show" utility and the "kd_region_decompressor" object used in that
utility should provide a versatile and useful interface tool for many
interactive applications.
   Although the current example is far from the recommended way of using
Kakadu in many professional applications, developers may find this type of
introductory example helpful as a first exercise in understanding how to use
Kakadu.  The vast majority of Kakadu's interesting features are not utilized by
this example, but it may be enough to satisfy the initial needs of some
developers who are working with moderately large images, already in an internal
buffer, especially when interfacing with existing image processing packages
which are buffer-oriented.
   The emphasis here is on showing a minimal number of instructions required
to open a codestream object, attach a decompression engine and recover samples
from the engine.  Decompression is necessarily a little more complex than
compression, since we have no control over the compressed code-stream which
may be sent to us.  The present application should be able to cope with any
code-stream at all, although it may decompress only the first image
component, if multiple components have differing sizes so that they are
incompatible with the simple interleaved buffer structure used here.
   By far the largest function here is that used to convert decompressed
samples to an 8-bit representation for the buffer.  Although this may sound
trivial, it is complicated by the fact that the original samples which
were compressed may have had any bit-depth at all, and they may have been
compressed by either the reversible or the irreversible processing path
offered by JPEG2000, each of which has fundamentally different implications
for the interpretation of the decompressed sample values.  There are a
variety of reasons for regarding sample representation conversions as the
task of the application, rather than the core system.  This will become
even more obvious as we add support for JP2 and perhaps other file formats
in subsequent releases of the Kakadu system.
******************************************************************************/

// System includes
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
// Kakadu core includes
#include "kdu_elementary.h"
#include "kdu_messaging.h"
#include "kdu_params.h"
#include "kdu_compressed.h"
#include "kdu_sample_processing.h"
// Application level includes
#include "kdu_file_io.h"

/*****************************************************************************/
/* STATIC                         write_image                                */
/*****************************************************************************/

static void
  write_image(char *fname, kdu_byte *buffer,
              int num_components, int height, int width)
  /* Simple PGM/PPM file writer.  The `buffer' array contains interleaved
     image samples (there are 1 or 3 components) in unsigned bytes,
     appearing in scan-line order. */
{
  std::ofstream out(fname,std::ios::out|std::ios::binary);
  if (!out)
    { kdu_error e;
      e << "Unable to open output image file, \"" << fname << "\"."; }
  if (num_components == 1)
    out << "P5\n" << width << " " << height << "\n255\n";
  else if (num_components == 3)
    out << "P6\n" << width << " " << height << "\n255\n";
  else
    assert(0); // The caller makes sure only 1 or 3 components are decompressed
  if (!out.write((char *) buffer,num_components*width*height))
    { kdu_error e;
      e << "Unable to write to output image file, \"" << fname << "\"."; }
}

/******************************************************************************/
/*                              transfer_bytes                                */
/******************************************************************************/

void
  transfer_bytes(kdu_byte *dest, kdu_line_buf &src, int gap, int precision)
  /* Transfers source samples from the supplied line buffer into the output
     byte buffer, spacing successive output samples apart by `gap' bytes
     (to allow for interleaving of colour components).  The function performs
     all necessary level shifting, type conversion, rounding and truncation. */
{
  int width = src.get_width();
  if (src.get_buf32() != NULL)
    { // Decompressed samples have a 32-bit representation (integer or float)
      assert(precision >= 8); // Else would have used 16 bit representation
      kdu_sample32 *sp = src.get_buf32();
      if (!src.is_absolute())
        { // Transferring normalized floating point data.
          float scale16 = (float)(1<<16);
          kdu_int32 val;

          for (; width > 0; width--, sp++, dest+=gap)
            {
              val = (kdu_int32)(sp->fval*scale16);
              val = (val+128)>>8; // May be faster than true rounding
              val += 128;
              if (val & ((-1)<<8))
                val = (val<0)?0:255;
              *dest = (kdu_byte) val;
            }
        }
      else
        { // Transferring 32-bit absolute integers.
          kdu_int32 val;
          kdu_int32 downshift = precision-8;
          kdu_int32 offset = (1<<downshift)>>1;
              
          for (; width > 0; width--, sp++, dest+=gap)
            {
              val = sp->ival;
              val = (val+offset)>>downshift;
              val += 128;
              if (val & ((-1)<<8))
                val = (val<0)?0:255;
              *dest = (kdu_byte) val;
            }
        }
    }
  else
    { // Source data is 16 bits.
      kdu_sample16 *sp = src.get_buf16();
      if (!src.is_absolute())
        { // Transferring 16-bit fixed point quantities
          kdu_int16 val;

          if (precision >= 8)
            { // Can essentially ignore the bit-depth.
              for (; width > 0; width--, sp++, dest+=gap)
                {
                  val = sp->ival;
                  val += (1<<(KDU_FIX_POINT-8))>>1;
                  val >>= (KDU_FIX_POINT-8);
                  val += 128;
                  if (val & ((-1)<<8))
                    val = (val<0)?0:255;
                  *dest = (kdu_byte) val;
                }
            }
          else
            { // Need to force zeros into one or more least significant bits.
              kdu_int16 downshift = KDU_FIX_POINT-precision;
              kdu_int16 upshift = 8-precision;
              kdu_int16 offset = 1<<(downshift-1);

              for (; width > 0; width--, sp++, dest+=gap)
                {
                  val = sp->ival;
                  val = (val+offset)>>downshift;
                  val <<= upshift;
                  val += 128;
                  if (val & ((-1)<<8))
                    val = (val<0)?0:(256-(1<<upshift));
                  *dest = (kdu_byte) val;
                }
            }
        }
      else
        { // Transferring 16-bit absolute integers.
          kdu_int16 val;

          if (precision >= 8)
            {
              kdu_int16 downshift = precision-8;
              kdu_int16 offset = (1<<downshift)>>1;
              
              for (; width > 0; width--, sp++, dest+=gap)
                {
                  val = sp->ival;
                  val = (val+offset)>>downshift;
                  val += 128;
                  if (val & ((-1)<<8))
                    val = (val<0)?0:255;
                  *dest = (kdu_byte) val;
                }
            }
          else
            {
              kdu_int16 upshift = 8-precision;

              for (; width > 0; width--, sp++, dest+=gap)
                {
                  val = sp->ival;
                  val <<= upshift;
                  val += 128;
                  if (val & ((-1)<<8))
                    val = (val<0)?0:(256-(1<<upshift));
                  *dest = (kdu_byte) val;
                }
            }
        }
    }
}

/*****************************************************************************/
/* STATIC                        process_tile                                */
/*****************************************************************************/

static void
  process_tile(kdu_tile tile, kdu_byte *buf, int row_gap)
  /* Decompresses a tile, writing the data into the supplied byte buffer.
     The buffer contains interleaved image components, if there are any.
     Although you may think of the buffer as belonging entirely to this tile,
     the `buf' pointer may actually point into a larger buffer representing
     multiple tiles.  For this reason, `row_gap' is needed to identify the
     separation between consecutive rows in the real buffer. */
{
  int c, num_components = tile.get_num_components(); assert(num_components<=3);
  bool use_ycc = tile.get_ycc();

  // Open tile-components and create processing engines and resources
  kdu_dims dims;
  kdu_sample_allocator allocator;
  kdu_tile_comp comps[3];
  kdu_line_buf lines[3];
  kdu_pull_ifc engines[3];
  bool reversible[3]; // Some components may be reversible and others not.
  int bit_depths[3]; // Original bit-depth may be quite different from 8.
  for (c=0; c < num_components; c++)
    {
      comps[c] = tile.access_component(c);
      reversible[c] = comps[c].get_reversible();
      bit_depths[c] = comps[c].get_bit_depth();
      kdu_resolution res = comps[c].access_resolution(); // Get top resolution
      kdu_dims comp_dims; res.get_dims(comp_dims);
      if (c == 0)
        dims = comp_dims;
      else
        assert(dims == comp_dims); // Safety check; the caller has ensured this
      bool use_shorts = (comps[c].get_bit_depth(true) <= 16);
      lines[c].pre_create(&allocator,dims.size.x,reversible[c],use_shorts);
      if (res.which() == 0) // No DWT levels used
        engines[c] =
          kdu_decoder(res.access_subband(LL_BAND),&allocator,use_shorts);
      else
        engines[c] = kdu_synthesis(res,&allocator,use_shorts);
    }
  allocator.finalize(); // Actually creates buffering resources
  for (c=0; c < num_components; c++)
    lines[c].create(); // Grabs resources from the allocator.

  // Now walk through the lines of the buffer, recovering them from the
  // relevant tile-component processing engines.

  while (dims.size.y--)
    {
      for (c=0; c < num_components; c++)
        engines[c].pull(lines[c],true);
      if ((num_components == 3) && use_ycc)
        kdu_convert_ycc_to_rgb(lines[0],lines[1],lines[2]);
      for (c=0; c < num_components; c++)
        transfer_bytes(buf+c,lines[c],num_components,bit_depths[c]);
      buf += row_gap;
    }

  // Cleanup
  for (c=0; c < num_components; c++)
    engines[c].destroy(); // engines are interfaces; no default destructors
}

/*****************************************************************************/
/* STATIC                         print_usage                                */
/*****************************************************************************/

static void
  print_usage(char *prog)
{
  kdu_pretty_buf strbuf(&std::cout);
  std::ostream out(&strbuf); // Creates a pretty-printing output stream.
    // Member functions of the `strbuf' object can control global properties.

  out << "Usage: \"" << prog << " <J2C input file> <PNM output file>\"\n";
  exit(0);
}

/*****************************************************************************/
/*                                    main                                   */
/*****************************************************************************/

int
  main(int argc, char *argv[])
{
  if (argc != 3)
    print_usage(argv[0]);

  // Custom messaging services
  kdu_customize_warnings(&std::cout);
  kdu_customize_errors(&std::cerr);

  // Construct code-stream object
  kdu_simple_file_source input(argv[1]);
    // As an alternative to raw code-stream input, you may wish to open a
    // JP2-compatible file which embeds a JPEG2000 code-stream.  Full support
    // for the optional JP2 file format is available by declaring "input"
    // to be of class "jp2_source" instead of "kdu_simple_file_source".  You
    // will need to include "jp2.h" to enable this functionality.  When
    // processing JP2 files, you should also respect the palette mapping,
    // channel binding and colour transformation attributes conveyed through
    // the auxiliary boxes in the file.  All relevant information is exposed
    // through the interfaces which can be accessed from the "jp2_source"
    // object.  These objects not only provide you with a uniform interface
    // to the sometimes tangled box definitions used by JP2, but they also
    // provide tools for performing most common transformations of interest.
    // You may wish to consult the more extensive demonstrations in the
    // "kdu_expand" and "kdu_show" applications.
  kdu_codestream codestream; codestream.create(&input);
  codestream.set_fussy(); // Set the parsing error tolerance.

      //    If you want to flip or rotate the image for some reason, change
      // the resolution, or identify a restricted region of interest, this is
      // the place to do it.  You may use "kdu_codestream::change_appearance"
      // and "kdu_codestream::apply_input_restrictions" for this purpose.
      //    If you wish to truncate the code-stream prior to decompression, you
      // may use "kdu_codestream::set_max_bytes".
      //    If you wish to retain all compressed data so that the material
      // can be decompressed multiple times, possibly with different appearance
      // parameters, you should call "kdu_codestream::set_persistent" here.
      //    There are a variety of other features which must be enabled at
      // this point if you want to take advantage of them.  See the
      // descriptions appearing with the "kdu_codestream" interface functions
      // in "kdu_compressed.h" for an itemized account of these capabilities.
  
  // Determine number of components to decompress -- simple app only writes PNM
  kdu_dims dims; codestream.get_dims(0,dims);
  int num_components = codestream.get_num_components();
  if (num_components == 2)
    num_components = 1;
  else if (num_components >= 3)
    { // Check that components have consistent dimensions (for PPM file)
      num_components = 3;
      kdu_dims dims1; codestream.get_dims(1,dims1);
      kdu_dims dims2; codestream.get_dims(2,dims2);
      if ((dims1 != dims) || (dims2 != dims))
        num_components = 1;
    }
  codestream.apply_input_restrictions(0,num_components,0,0,NULL);

  // Now we are ready to walk through the tiles processing them one-by-one.
  kdu_byte *buffer = new kdu_byte[dims.area()*num_components];
  kdu_dims tile_indices; codestream.get_valid_tiles(tile_indices);
  kdu_coords tpos;
  for (tpos.y=0; tpos.y < tile_indices.size.y; tpos.y++)
    for (tpos.x=0; tpos.x < tile_indices.size.x; tpos.x++)
      {
        kdu_tile tile = codestream.open_tile(tpos+tile_indices.pos);

        // Find the region of the buffer occupied by this tile.  Note that
        // we have no control over sub-sampling factors which might have been
        // used during compression and so it can happen that tiles (at the
        // image component level) actually have different dimensions.  For this
        // reason, we cannot figure out the buffer region occupied by a tile
        // directly from the tile indices.  Instead, we query the highest
        // resolution of the first tile-component concerning its location and
        // size on the canvas -- the `dims' object already holds the location
        // and size of the entire image component on the same canvas coordinate
        // system.  Comparing the two tells us where the current tile is in
        // the buffer.
        kdu_resolution res = tile.access_component(0).access_resolution();
        kdu_dims tile_dims; res.get_dims(tile_dims);
        kdu_coords offset = tile_dims.pos - dims.pos;
        int row_gap = num_components*dims.size.x; // inter-row separation
        kdu_byte *buf = buffer + offset.y*row_gap + offset.x*num_components;

        // Do the actual processing
        process_tile(tile,buf,row_gap);
        tile.close();
      }

  // Write image buffer to file and clean up
  codestream.destroy();
  input.close(); // Not really necessary here.
  write_image(argv[2],buffer,num_components,dims.size.y,dims.size.x);
  delete[] buffer;
  return 0;
}
