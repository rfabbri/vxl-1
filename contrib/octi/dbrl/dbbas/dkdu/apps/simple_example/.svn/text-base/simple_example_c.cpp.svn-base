/*****************************************************************************/
// File: simple_example_c.cpp [scope = APPS/SIMPLE_EXAMPLE]
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
   Simple example showing compression with an intermediate buffer used to
store the image samples.  This is, of course, a great waste of memory and
the more sophisticated "kdu_compress" application does no such thing.
However, developers may find this type of introductory example helpful as
a first exercise in understanding how to use Kakadu.  The vast majority of
Kakadu's interesting features are not utilized by this example, but it
may be enough to satisfy the initial needs of some developers who are
working with moderately large images, already in an internal buffer.
   The emphasis here is on showing a minimal number of instructions required
to open a codestream object, attach a compression engine, push samples into
the engine, and flush the codestream to an output file, with a given number
of automatically selected rate-distortion optimized quality layers for
a quality-progressive representation.
   The reader may find it interesting to open the compressed codestream using
the "kdu_show" utility, examine the properties (file:properties menu item),
and investigate the quality progression associated with the default set of
quality layers created here (use the "<" and ">" keys to navigate through
the quality layers interactively).
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
/* STATIC                    eat_white_and_comments                          */
/*****************************************************************************/

static void
  eat_white_and_comments(std::istream &stream)
{
  char ch;
  bool in_comment;

  in_comment = false;
  while (!(stream.get(ch)).fail())
    if (ch == '#')
      in_comment = true;
    else if (ch == '\n')
      in_comment = false;
    else if ((!in_comment) && (ch != ' ') && (ch != '\t') && (ch != '\r'))
      {
        stream.putback(ch);
        return;
      }
}

/*****************************************************************************/
/* STATIC                          read_image                                */
/*****************************************************************************/

static kdu_byte *
  read_image(char *fname, int &num_components, int &height, int &width)
  /* Simple PGM/PPM file reader.  Returns an array of interleaved image
     samples (there are 1 or 3 components) in unsigned bytes, appearing
     in scan-line order.  If the file cannot be open, the program simply
     exits for simplicity. */
{
  char magic[3];
  int max_val; // We don't actually use this.
  char ch;
  std::ifstream in(fname,std::ios::in|std::ios::binary);
  if (in.fail())
    { kdu_error e; e << "Unable to open input file, \"" << fname << "\"!"; }

  // Read PGM/PPM header
  in.get(magic,3);
  if (strcmp(magic,"P5") == 0)
    num_components = 1;
  else if (strcmp(magic,"P6") == 0)
    num_components = 3;
  else
    { kdu_error e; e << "PGM/PPM image file must start with the magic string, "
      "\"P5\" or \"P6\"!"; }
  eat_white_and_comments(in); in >> width;
  eat_white_and_comments(in); in >> height;
  eat_white_and_comments(in); in >> max_val;
  if (in.fail())
    {kdu_error e; e << "Image file \"" << fname << "\" does not appear to "
     "have a valid PGM/PPM header."; }
  while (!(in.get(ch)).fail())
    if (ch == '\n')
      break;

  // Read sample values
  int num_samples = height*width*num_components;
  kdu_byte *buffer = new kdu_byte[num_samples];
  if ((int)(in.read((char *) buffer,num_samples)).gcount() != num_samples)
    { kdu_error e; e << "PGM/PPM image file \"" << fname
      << "\" terminated prematurely!"; }
  return buffer;
}

/*****************************************************************************/
/* STATIC                         process_tile                               */
/*****************************************************************************/

static void
  process_tile(kdu_tile tile, kdu_byte *buffer)
{
  int c, num_components = tile.get_num_components(); assert(num_components<=3);
  bool use_ycc = tile.get_ycc(); // Should be true here if `num_components'=3

  // Open tile-components and create processing engines and resources
  kdu_dims dims;
  kdu_sample_allocator allocator;
  kdu_tile_comp comps[3];
  kdu_line_buf lines[3];
  kdu_push_ifc engines[3];
  for (c=0; c < num_components; c++)
    {
      comps[c] = tile.access_component(c);
      assert(comps[c].get_reversible());
      kdu_resolution res = comps[c].access_resolution(); // Get top resolution
      kdu_dims comp_dims; res.get_dims(comp_dims);
      if (c == 0)
        dims = comp_dims;
      else
        assert(dims == comp_dims); // Must be true in this simple example.
      lines[c].pre_create(&allocator,dims.size.x,true,true);
      if (res.which() == 0) // No DWT levels (should not occur in this example)
        engines[c] =
          kdu_encoder(res.access_subband(LL_BAND),&allocator,true);
      else
        engines[c] = kdu_analysis(res,&allocator,true);
         // Note: if you wish to specify a region of interest for special
         // attention (higher fidelity) at encode time, an appropriately
         // derived "kdu_roi_node" object may be supplied to any of the
         // constructors above.  This object will normally be obtained from
         // "kdu_roi_image::acquire_node", where the 'kdu_roi_image' object
         // itself must be derived in a suitable manner to support the region
         // specification techniques which are most appropriate to the
         // application.  One very simple and one very sophisticated example of
         // this are provided by "roi_sources.h" and "roi_sources.cpp" found
         // in the "kdu_compress" directory -- their use is amply
         // demonstrated by the "kdu_compress" application.  You should be
         // aware that, in addition to supplying ROI mask information, it is
         // necessary to tell the system what to do with it. This is controlled
         // by the "Rshift", "Rlevels" and "Rweight" attributes offered by the
         // "roi_params" coding parameter object.  These may be set in the
         // same way as any of the coding parameter attributes (e.g., by
         // calling "codestream.access_siz()->parse_string", as demonstrated
         // below in "main()").
    }
  allocator.finalize(); // Actually creates buffering resources
  for (c=0; c < num_components; c++)
    lines[c].create(); // Grabs resources from the allocator.

  // Now walk through the lines of the buffer, pushing them into the
  // relevant tile-component processing engines.

  while (dims.size.y--)
    {
      for (c=0; c < num_components; c++)
        {
          kdu_sample16 *dest = lines[c].get_buf16();
          kdu_byte *sp = buffer+c;
          for (int n=dims.size.x; n > 0; n--, dest++, sp+=num_components)
            dest->ival = ((kdu_int16)(*sp)) - 128; // Convert to signed quantity.
        }
      buffer += dims.size.x * num_components; // Move buffer to next row.
      if ((num_components == 3) && use_ycc)
        kdu_convert_rgb_to_ycc(lines[0],lines[1],lines[2]);
      for (c=0; c < num_components; c++)
        engines[c].push(lines[c],true);
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

  out << "Usage: \"" << prog << " <PNM input file> <J2C output file>\"\n";
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

  // Load image into a buffer
  int num_components, height, width;
  kdu_byte *buffer = read_image(argv[1],num_components,height,width);

  // Construct code-stream object
  siz_params siz;
  siz.set(Scomponents,0,0,num_components);
  siz.set(Sdims,0,0,height);  // Height of first image component
  siz.set(Sdims,0,1,width);   // Width of first image component
  siz.set(Sprecision,0,0,8);  // Image samples have original bit-depth of 8
  siz.set(Ssigned,0,0,false); // Image samples are originally unsigned
  siz_params *siz_ref = &siz; siz_ref->finalize();
     // Finalizing the siz parameter object will fill in the myriad SIZ
     // parameters we have not explicitly specified in this simple example.
     // The capabilities of the finalization process are documented in
     // "kdu_params.h".  Note that we execute the virtual member function
     // through a pointer, since the address of the function is not explicitly
     // exported by the core DLL (minimizes the export table).
  kdu_simple_file_target output; output.open(argv[2]);
     // As an alternative to raw code-stream output, you may wish to wrap
     // the code-stream in a JP2 file, which then allows you to add a
     // additional information concerning the colour and geometric properties
     // of the image, all of which should be respected by conforming readers.
     // To do this, include "jp2.h" and declare "output" to be of class
     // "jp2_target" instead of "kdu_simple_file_target".  At a minimum, you
     // must execute the following additional configuration steps for the
     // JP2 file:
     //     jp2_dimensions dims = output.access_dimensions(); dims.init(&siz);
     //     jp2_colour colr = output.access_colour();
     //     colr.init((num_components==3)?:JP2_sRGB_SPACE:JP2_sLUM_SPACE);
     // Of course, there is a lot more you can do with JP2.  Read the
     // interface descriptions in "jp2.h" and/or take a look at the more
     // sophisticated demonstration in "kdu_compress.cpp".
  kdu_codestream codestream; codestream.create(&siz,&output);

  // Set up any specific coding parameters and finalize them.
  codestream.access_siz()->parse_string("Clayers=12");
  codestream.access_siz()->parse_string("Creversible=yes");
  codestream.access_siz()->finalize_all(); // Set up coding defaults

  // Open the one and only tile in use here and push in all the samples.
  kdu_tile tile = codestream.open_tile(kdu_coords(0,0));
  process_tile(tile,buffer);
  tile.close();
  delete[] buffer;

  // Produce the final compressed output.
  int layer_bytes[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
        // Here we are not requesting specific sizes for any of the 12
        // quality layers.  As explained in the description of
        // "kdu_codestream::flush" (see "kdu_compressed.h"), the rate allocator
        // will then assign the layers in such a way as to achieve roughly
        // two quality layers per octave change in bit-rate, with the final
        // layer reaching true lossless quality.
  codestream.flush(layer_bytes,12); // You can see how many bytes were assigned
      // to each quality layer by looking at the entries of `layer_bytes' here.
      // The flush function can do a lot of interesting things which you may
      // want to spend some time looking into. In addition to targeting
      // specific bit-rates for each quality layer, it can be configured to
      // use rate-distortion slope thresholds of your choosing and to return
      // the thresholds which it finds to be best for any particular set of
      // target layer sizes.  This opens the door to feedback-oriented rate
      // control for video.  You should also look into the
      // "kdu_codestream::set_max_bytes" and
      // "kdu_codestream::set_min_slope_threshold" functions which can be
      // used to significantly speed up compression.
  codestream.destroy(); // All done: simple as that.
  output.close(); // Not really necessary here.
  return 0;
}
