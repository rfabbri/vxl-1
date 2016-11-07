/*****************************************************************************/
// File: kdu_transcode.cpp [scope = APPS/TRANSCODE]
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
   Transcoding application, demonstrating many of the transcoding-oriented
capabilities of the Kakadu framework.  Supports most of the transcoding
operations which are appealing within the JPEG2000 framework, including
image rotation.  Rate control by the transcoder is usually superior to
that obtained by pure file truncation, even with layer-progressive
code-stream organizations.
******************************************************************************/

// System includes
#include <string.h>
#include <stdlib.h>
#include <stdio.h> // Want to use "sscanf".
#include <fstream>
#include <assert.h>
// Kakadu core includes
#include "kdu_elementary.h"
#include "kdu_messaging.h"
#include "kdu_params.h"
#include "kdu_compressed.h"
#include "kdu_block_coding.h"
// Application includes
#include "kdu_args.h"
#include "kdu_file_io.h"
#include "jp2.h"

/* ========================================================================= */
/*                      Pretty Printing Version of `cout'                    */
/* ========================================================================= */

static kdu_pretty_buf pretty_cout_buf(&std::cout);
static std::ostream pretty_cout(&pretty_cout_buf);


/* ========================================================================= */
/*                           Internal Functions                              */
/* ========================================================================= */

/*****************************************************************************/
/* STATIC                        print_about                                 */
/*****************************************************************************/

static void
  print_about(char *prog)
{
  kdu_pretty_buf strbuf(&std::cout);
  std::ostream out(&strbuf);

  out << "Brief description of \"" << prog << "\"\n";
  strbuf.set_master_indent(3);
  out << "  The standard inherently lends itself to a wide variety of "
         "transcoding operations.  This program offers the "
         "following capabilities:\n";
  strbuf.set_master_indent(7);
  out << "-- reducing the resolution (i.e., the number of DWT levels);\n"
         "-- reducing the number of image components;\n"
         "-- changing the number of quality layers;\n"
         "-- reducing the bit-rate;\n"
         "-- re-sequencing of the packet progression in any tile;\n"
         "-- introducing/discarding interleved tile-parts for any tile;\n"
         "-- changing precinct dimensions to support better spatial "
         "progressions;\n"
         "-- introducing/discarding SOP (Start of Packet) markers;\n"
         "-- introducing/discarding EPH (End of Packet Header) markers;\n"
         "-- changing any of the six block coder mode switches; and\n"
         "-- rotation in the transformed domain.\n";
  strbuf.set_master_indent(3);
  out << "In the future we plan to add compressed domain cropping to these "
         "features.\n";
  out << "\nThese capabilities are mostly realized by supplying the same "
         "type of attribute specification on the command-line, which you "
         "would use with the compressor.  Tile- and component-specific "
         "forms of the various arguments may be used.\n";
  out << "\nFor a list of arguments which may legitimately be supplied here, "
         "use the \"-u\" or \"-usage\" (long version) arguments to request "
         "a usage statement.\n";

  exit(0);
}

/*****************************************************************************/
/* STATIC                        print_usage                                 */
/*****************************************************************************/

static void
  print_usage(char *prog, bool comprehensive=false)
{
  kdu_pretty_buf strbuf(&std::cout);
  std::ostream out(&strbuf);

  out << "Usage:\n  \"" << prog << " ...\n";
  strbuf.set_master_indent(3);
  out << "-i <compressed file in>\n";
  if (comprehensive)
    out << "\tCurrently accepts raw code-stream files and code-streams "
             "wrapped in any JP2 compatible file format.  The file suffix "
             "is currently used to identify the type of file which is being "
             "supplied, where a \".jp2\" or \".jpx\" suffix (case "
             "insensitive) is required if the file is to be treated as "
             "anything other than a raw code-stream.\n";
  out << "-o <compressed file out>\n";
  if (comprehensive)
    out << "\tOnly writes raw code-stream files at present, even if the "
           "input was a JP2 file.\n";
  out << "-reduce <discard levels>\n";
  if (comprehensive)
    out << "\tSet the number of highest resolution levels to be discarded.  "
           "The image resolution is effectively divided by 2 to the power of "
           "the number of discarded levels.\n";
  out << "-components <num image components>\n";
  if (comprehensive)
    out << "\tSet the maximum number of image components to be included "
           "in the output code-stream.\n";
  out << "-skip_components <num image components>\n";
  if (comprehensive)
    out << "\tSet the number of initial image components to be discarded.\n";
  out << "-rate <bits per pixel>\n";
  if (comprehensive)
    out << "\tMax output bit-rate, expressed in terms of the ratio between "
           "the total number of compressed bits (including headers) and the "
           "product of the largest horizontal and  vertical image component "
           "dimensions.  Note that all of the original layers will be "
           "included, many of which may be assigned empty packets, which "
           "still occupy at least one byte each.  Apart from this small cost, "
           "transcoding to a reduced rate usually produces superior results "
           "to simply truncating the original code-stream to that rate, even "
           "when it was layer progressive -- it also yields a fully compliant "
           "code-stream, complete with all packets and an EOC marker.\n";
  out << "-rotate <degrees>\n";
  if (comprehensive)
    out << "\tRotate by a multiple of 90 degrees.  JPEG2000 allows for "
           "image rotation by multiples of 90 degrees without the need to "
           "invert and redo the DWT (Discrete Wavelet Transform).  "
           "While this represents a considerable saving, individual "
           "code-blocks must still be transcoded.  While the distortion "
           "and size of most code-streams will not be invariant under "
           "rotation, once a compressed image has been rotated using "
           "this option, all subsequent rotations will preserve "
           "distortion and any set of rotations which sum to a multiple of "
           "360 degrees will preserve compressed length.\n"
        << "\tNote that subject to rotation, any tile-specific parameter "
           "modifications supplied here refer to tiles in the output "
           "code-stream, as opposed to the input code-stream.\n";
  cod_params cod;
  cod.describe_string(Corder,out,comprehensive);
  cod.describe_string(Clayers,out,comprehensive);
  cod.describe_string(Cprecincts,out,comprehensive);
  cod.describe_string(Cuse_precincts,out,comprehensive);
  cod.describe_string(Cuse_eph,out,comprehensive);
  cod.describe_string(Cuse_sop,out,comprehensive);
  cod.describe_string(Cmodes,out,comprehensive);
  poc_params poc;
  poc.describe_string(Porder,out,comprehensive);

  out << "-fussy\n";
  if (comprehensive)
    out << "\tEncourage fussy input code-stream parsing, in which most "
           "code-stream compliance failures will terminate execution, with "
           "an appropriate error message.\n";
  out << "-resilient\n";
  if (comprehensive)
    out << "\tEncourage error resilient processing, in which an attempt is "
           "made to recover from errors in the input code-stream with minimal "
           "degradation in reconstructed image quality.  The current "
           "implementation should avoid execution failure so long as only "
           "a single tile-part was used and no errors are found in the main "
           "or tile header.  The implementation recognizes tile-part headers "
           "only if the first 4 bytes of the marker segment are correct, "
           "which makes it extremely unlikely that a code-stream with only "
           "one tile-part will be mistaken for anything else.  Multiple "
           "tiles or tile-parts can create numerous problems for an error "
           "resilient decompressor; complete failure may occur if a "
           "multi-tile-part code-stream is corrupted.\n";
  out << "-resilient_sop\n";
  if (comprehensive)
    out << "\tSame as \"-resilient\" except that the error resilient code-"
           "stream parsing algorithm is informed that it can expect SOP "
           "markers to appear in front of every single packet, whenever "
           "the relevant flag in the Scod style byte of the COD marker is "
           "set.  The JPEG2000 standard interprets this flag as meaning "
           "that SOP markers may appear; however, this does not give the "
           "decompressor any idea where it can expect SOP markers "
           "to appear.  In most cases, SOP markers, if used, will be placed "
           "in front of every packet and knowing this a priori can "
           "improve the performance of the error resilient parser.\n";
  out << "-mem -- Report memory usage.\n";
  out << "-s <switch file>\n";
  if (comprehensive)
    out << "\tSwitch to reading arguments from a file.  In the file, argument "
           "strings are separated by whitespace characters, including spaces, "
           "tabs and new-line characters.  Comments may be included by "
           "introducing a `#' or a `%' character, either of which causes "
           "the remainder of the line to be discarded.  Any number of "
           "\"-s\" argument switch commands may be included on the command "
           "line.\n";
  out << "-record <file>\n";
  if (comprehensive)
    out << "\tRecord output code-stream parameters in a file, using the same "
           "format which is accepted when specifying the parameters to the "
           "compressor.\n"; 
  out << "-quiet -- suppress informative messages.\n";
  out << "-about -- print a brief description of this program.\n";
  out << "-usage -- print a comprehensive usage statement.\n";
  out << "-u -- print a brief usage statement.\"\n\n";
  out.flush();
  exit(0);
}

/*****************************************************************************/
/* STATIC                    check_parameter_args                            */
/*****************************************************************************/

static void
  check_parameter_args(kdu_args &args)
{
  char *string;
  for (string=args.get_first(); string != NULL; string=args.advance(false))
    {
      char *delim = string;
      while ((*delim != '\0') && (*delim != ':') && (*delim != '=') &&
             (*delim != ' '))
        delim++;
      if ((*delim != ':') && (*delim != '='))
        continue;
      char save = *delim; *delim = '\0';
      if (strcmp(string,Corder) &&
          strcmp(string,Clayers) &&
          strcmp(string,Cprecincts) &&
          strcmp(string,Cuse_precincts) &&
          strcmp(string,Cuse_eph) &&
          strcmp(string,Cuse_sop) &&
          strcmp(string,Cmodes) &&
          strcmp(string,Porder))
        {  *delim = save;
          kdu_error e; e << "Attempting to modify a code-stream parameter "
          "for which transcoding is not defined.  Offending argument is \""
          << string << "\".  For more information see the usage statement."; }
      *delim = save;
    }
}

/*****************************************************************************/
/* STATIC                     parse_simple_args                              */
/*****************************************************************************/

static void
  parse_simple_args(kdu_args &args, char * &ifname, char * &ofname,
                    std::ostream * &record_stream, int &discard_levels,
                    int &skip_components, int &max_components,
                    float &max_bpp, bool &transpose,
                    bool &vflip, bool &hflip, bool &mem, bool &quiet)
  /* Parses most simple arguments (those involving a dash).
     Note that `max_bpp' is returned as negative if the
     bit-rate is not explicitly set. */

{
  if ((args.get_first() == NULL) || (args.find("-about") != NULL))
    print_about(args.get_prog_name());
  if (args.find("-u") != NULL)
    print_usage(args.get_prog_name());
  if (args.find("-usage") != NULL)
    print_usage(args.get_prog_name(),true);

  ifname = NULL;
  ofname = NULL;
  record_stream = NULL;
  discard_levels = 0;
  skip_components = 0;
  max_components = 0;
  max_bpp = -1.0;
  int rotate = 0;
  mem = false;
  quiet = false;

  if (args.find("-i") != NULL)
    {
      if ((ifname = args.advance()) == NULL)
        { kdu_error e; e << "\"-i\" argument requires a file name!"; }
      args.advance();
    }

  if (args.find("-o") != NULL)
    {
      if ((ofname = args.advance()) == NULL)
        { kdu_error e; e << "\"-o\" argument requires a file name!"; }
      args.advance();
    }

  if (args.find("-reduce") != NULL)
    {
      char *string = args.advance();
      if ((string == NULL) || (sscanf(string,"%d",&discard_levels) != 1) ||
          (discard_levels < 0))
        { kdu_error e; e << "\"-reduce\" argument requires a non-negative "
          "integer parameter!"; }
      args.advance();
    }

  if (args.find("-skip_components") != NULL)
    {
      char *string = args.advance();
      if ((string == NULL) || (sscanf(string,"%d",&skip_components) != 1) ||
          (skip_components < 0))
        { kdu_error e; e << "\"-skip_components\" argument requires a "
          "non-negative integer parameter!"; }
      args.advance();
    }

  if (args.find("-components") != NULL)
    {
      char *string = args.advance();
      if ((string == NULL) || (sscanf(string,"%d",&max_components) != 1) ||
          (max_components <= 0))
        { kdu_error e; e << "\"-components\" argument requires a positive "
          "integer parameter!"; }
      args.advance();
    }

  if (args.find("-rate") != NULL)
    {
      char *string = args.advance();
      if ((string == NULL) || (sscanf(string,"%f",&max_bpp) != 1) ||
          (max_bpp <= 0.0F))
        { kdu_error e; e << "\"-rate\" argument requires a positive "
          "numeric parameter!"; }
      args.advance();
    }

  if (args.find("-rotate") != NULL)
    {
      char *string = args.advance();
      if ((string == NULL) || (sscanf(string,"%d",&rotate) != 1) ||
          ((rotate % 90) != 0))
        { kdu_error e; e << "\"-rotate\" argument requires an integer "
          "multiple of 90 degrees!"; }
      args.advance();
      rotate /= 90;
    }

  if (args.find("-mem") != NULL)
    {
      mem = true;
      args.advance();
    }

  if (args.find("-quiet") != NULL)
    {
      quiet = true;
      args.advance();
    }

  if (args.find("-record") != NULL)
    {
      char *fname = args.advance();
      if (fname == NULL)
        { kdu_error e; e << "\"-record\" argument requires a file name!"; }
      record_stream = new std::ofstream(fname);
      if (record_stream->fail())
        { kdu_error e; e << "Unable to open record file, \"" << fname << "\"."; }
      args.advance();
    }

  if ((ifname == NULL) || (ofname == NULL))
    { kdu_error e; e << "Must at least supply the names of an input and an "
      "output file."; }

  while (rotate >= 4)
    rotate -= 4;
  while (rotate < 0)
    rotate += 4;
  switch (rotate) { /* Note: we will be applying the geometric transformations
                       to the input code-stream. */
    case 0: transpose = false; vflip = false; hflip = false; break;
    case 1: transpose = true;  vflip = false; hflip = true;  break;
    case 2: transpose = false; vflip = true;  hflip = true;  break;
    case 3: transpose = true;  vflip = true;  hflip = false; break;
    }
}

/*****************************************************************************/
/* STATIC                check_jp2_compatible_suffix                         */
/*****************************************************************************/

static bool
  check_jp2_compatible_suffix(char *fname)
  /* Returns true if the file-name has the suffix, ".jp2" or ".jpx", where
     the check is case insensitive. */
{
  char *cp = strrchr(fname,'.');
  if (cp == NULL)
    return false;
  cp++;
  if ((*cp != 'j') && (*cp != 'J'))
    return false;
  cp++;
  if ((*cp != 'p') && (*cp != 'P'))
    return false;
  cp++;
  if ((*cp != '2') && (*cp != 'x') && (*cp != 'X'))
    return false;
  return true;
}

/*****************************************************************************/
/* STATIC                    set_error_behaviour                             */
/*****************************************************************************/

static void
  set_error_behaviour(kdu_args &args, kdu_codestream codestream)
{
  bool fussy = false;
  bool resilient = false;
  bool ubiquitous_sops = false;
  if (args.find("-fussy") != NULL)
    { args.advance(); fussy = true; }
  if (args.find("-resilient") != NULL)
    { args.advance(); resilient = true; }
  if (args.find("-resilient_sop") != NULL)
    { args.advance(); resilient = true; ubiquitous_sops = true; }
  if (resilient)
    codestream.set_resilient(ubiquitous_sops);
  else if (fussy)
    codestream.set_fussy();
  else
    codestream.set_fast();
}

/*****************************************************************************/
/* STATIC                        get_bpp_dims                                */
/*****************************************************************************/

static int
  get_bpp_dims(siz_params *siz)
{
  int comps, max_width, max_height, n;

  siz->get(Scomponents,0,0,comps);
  max_width = max_height = 0;
  for (n=0; n < comps; n++)
    {
      int width, height;
      siz->get(Sdims,n,0,height);
      siz->get(Sdims,n,1,width);
      if (width > max_width)
        max_width = width;
      if (height > max_height)
        max_height = height;
    }
  return max_height * max_width;
}

/*****************************************************************************/
/* STATIC                        copy_block                                  */
/*****************************************************************************/
                    
static void
  copy_block(kdu_block *in, kdu_block *out)
{
  if (in->K_max_prime != out->K_max_prime)
    { kdu_error e; e << "Cannot copy blocks belonging to subbands with "
      "different quantization parameters."; }
  assert(!(out->transpose || out->vflip || out->hflip));
  kdu_coords size = in->size;
  if (in->transpose) size.transpose();
  if ((size.x != out->size.x) || (size.y != out->size.y))  
    { kdu_error e; e << "Cannot copy code-blocks with different dimensions."; }
  out->missing_msbs = in->missing_msbs;
  if (out->max_passes < (in->num_passes+2))      // Gives us enough to round up
    out->set_max_passes(in->num_passes+2,false); // to the next whole bit-plane
  out->num_passes = in->num_passes;
  int num_bytes = 0;
  for (int z=0; z < in->num_passes; z++)
    {
      num_bytes += (out->pass_lengths[z] = in->pass_lengths[z]);
      out->pass_slopes[z] = in->pass_slopes[z];
    }

  if ((in->modes != out->modes) || (in->orientation != out->orientation) ||
      in->transpose || in->vflip || in->hflip)
    { // Need to transcode the individual code-blocks.
      kdu_block_decoder decoder;
      decoder.decode(in);
      out->num_passes = in->num_passes; // Just in case we couldn't decode all
      if (out->num_passes == 0)
        return;
      int num_samples = in->size.x*in->size.y;
      if (num_samples > out->max_samples)
        out->set_max_samples((num_samples>4096)?num_samples:4096);
      if (in->transpose || in->vflip || in->hflip)
        { // Need geometric transformation and termination at whole bit-plane
          int row_gap_in = in->size.x;
          int row_gap_out = out->size.x;
          kdu_int32 *sp, *spp = in->sample_buffer;
          kdu_int32 *dp, *dpp = out->sample_buffer;
          int out_cinc = 1;
          if (in->vflip)
            { dpp += (size.y-1)*row_gap_out; row_gap_out = -row_gap_out; }
          if (in->hflip)
            { dpp += size.x-1; out_cinc = -out_cinc; }
          int r, c;
          if (!in->transpose)
            { // Non-transposed copy
              for (r=size.y; r > 0; r--, spp+=row_gap_in, dpp+=row_gap_out )
                { // Unroll loop a little for speed.
                  for (sp=spp, dp=dpp, c=size.x; c > 3; c-=4)
                    {
                      *dp = *(sp++); dp += out_cinc;
                      *dp = *(sp++); dp += out_cinc;
                      *dp = *(sp++); dp += out_cinc;
                      *dp = *(sp++); dp += out_cinc;
                    }
                  while (c--)
                    { *dp = *(sp++); dp += out_cinc; }
                }
            }
          else
            { // Transposed copy
              for (r=size.y; r > 0; r--, spp++, dpp+=row_gap_out)
                { // Unroll loop a little for speed.
                  for (sp=spp, dp=dpp, c=size.x; c > 3; c-=4)
                    {
                      *dp = *sp; sp += row_gap_in; dp += out_cinc;
                      *dp = *sp; sp += row_gap_in; dp += out_cinc;
                      *dp = *sp; sp += row_gap_in; dp += out_cinc;
                      *dp = *sp; sp += row_gap_in; dp += out_cinc;
                    }
                  while (c--)
                    { *dp = *sp; sp += row_gap_in; dp += out_cinc; }
                }
            }
          if ((out->num_passes % 3) != 1)
            { /* Round up to the next whole bit-plane to avoid any
                 degradation from further geometric transformations. */
              for (; (out->num_passes % 3) != 1; out->num_passes++)
                {
                  out->pass_slopes[out->num_passes] =
                    out->pass_slopes[out->num_passes-1];
                  out->pass_slopes[out->num_passes-1] = 0;
                }
              dp = out->sample_buffer; // Prepare to reset rounding bit
              int p = 30 - out->missing_msbs - (out->num_passes / 3);
              kdu_int32 mask = (((kdu_int32)(-1))<<p);
              for (c=num_samples; c > 3; c-=4, dp+=4)
                {
                  if ((dp[0] & mask) == 0) dp[0] &= (mask+mask);
                  if ((dp[1] & mask) == 0) dp[1] &= (mask+mask);
                  if ((dp[2] & mask) == 0) dp[2] &= (mask+mask);
                  if ((dp[3] & mask) == 0) dp[3] &= (mask+mask);
                }
              for (; c > 0; c--, dp++)
                if ((dp[0] & mask) == 0) dp[0] &= (mask+mask);
            }
        }
      else
        { // No change in the sample data itself.
          memcpy(out->sample_buffer,in->sample_buffer,
                 (size_t) num_samples * sizeof(kdu_int32));
        }
      kdu_block_encoder encoder;
      encoder.encode(out);
    }
  else
    { // Just copy compressed code-bytes.  No need for block transcoding.
      if (out->max_bytes < num_bytes)
        out->set_max_bytes(num_bytes,false);
      memcpy(out->byte_buffer,in->byte_buffer,(size_t) num_bytes);
    }
}

/*****************************************************************************/
/* STATIC                        copy_tile                                   */
/*****************************************************************************/

static void
  copy_tile(kdu_tile tile_in, kdu_tile tile_out, int tnum_in, int tnum_out,
            kdu_params *siz_in, kdu_params *siz_out, int skip_components,
            int &num_blocks)
  /* Although there could be more efficient ways of doing this (in terms of
     saving memory), we currently just walk through all code-blocks in the
     most obvious order, copying them from the input to the output tile.
     Note that the main tile-header coding parameters should have been
     copied already, but this function will copy POC parameters for
     non-initial tile-parts, wherever the information has not already
     been substituted for the purpose of rearranging the packet sequence
     during transcoding. */
{
  int num_components = tile_out.get_num_components();
  int new_tpart=0, next_tpart = 1;

  for (int c=0; c < num_components; c++)
    {
      kdu_tile_comp comp_in;  comp_in = tile_in.access_component(c);
      kdu_tile_comp comp_out; comp_out = tile_out.access_component(c);
      int num_resolutions = comp_out.get_num_resolutions();
      for (int r=0; r < num_resolutions; r++)
        {
          kdu_resolution res_in;  res_in = comp_in.access_resolution(r);
          kdu_resolution res_out; res_out = comp_out.access_resolution(r);
          int min_band = (r==0)?0:1;
          int max_band = (r==0)?0:3;
          for (int b=min_band; b <= max_band; b++)
            {
              kdu_subband band_in;  band_in = res_in.access_subband(b);
              kdu_subband band_out; band_out = res_out.access_subband(b);
              kdu_dims blocks_in;  band_in.get_valid_blocks(blocks_in);
              kdu_dims blocks_out; band_out.get_valid_blocks(blocks_out);
              if ((blocks_in.size.x != blocks_out.size.x) ||
                  (blocks_in.size.y != blocks_out.size.y))
                { kdu_error e; e << "Transcoding operation cannot proceed: "
                  "Code-block partitions for the input and output "
                  "code-streams do not agree."; }
              kdu_coords idx;
              for (idx.y=0; idx.y < blocks_out.size.y; idx.y++)
                for (idx.x=0; idx.x < blocks_out.size.x; idx.x++)
                  {
                    kdu_block *in =
                      band_in.open_block(idx+blocks_in.pos,&new_tpart);
                    for (; next_tpart <= new_tpart; next_tpart++)
                      siz_out->copy_from(siz_in,tnum_in,tnum_out,next_tpart,
                                         skip_components);
                    kdu_block *out = band_out.open_block(idx+blocks_out.pos);
                    copy_block(in,out);
                    band_in.close_block(in);
                    band_out.close_block(out);
                    num_blocks++;
                  }
            }
        }
    }
}

/*****************************************************************************/
/* EXTERN                           main                                     */
/*****************************************************************************/

int
  main(int argc, char *argv[])
{
  kdu_customize_warnings(&std::cout);
  kdu_customize_errors(&std::cerr);
  kdu_args args(argc,argv,"-s");

  // Collect simple arguments.

  char *ifname, *ofname;
  std::ostream *record_stream;
  int discard_levels;
  int skip_components, max_components;
  float max_bpp;
  bool transpose, vflip, hflip;
  bool mem, quiet;
  parse_simple_args(args,ifname,ofname,record_stream,
                    discard_levels,skip_components,max_components,max_bpp,
                    transpose,vflip,hflip,mem,quiet);

  // Create the input codestream object.

  kdu_compressed_source *input = NULL;
  kdu_simple_file_source file_in;
  jp2_source jp2_in;
  if (check_jp2_compatible_suffix(ifname))
    {
      input = &jp2_in;
      jp2_in.open(ifname);
    }
  else
    {
      input = &file_in;
      file_in.open(ifname);
    }
  kdu_codestream codestream_in; codestream_in.create(input);
  set_error_behaviour(args,codestream_in);
  codestream_in.apply_input_restrictions(skip_components,max_components,
                                         discard_levels,0,NULL);
  codestream_in.change_appearance(transpose,vflip,hflip);
  siz_params *siz_in = codestream_in.access_siz();

  // Create the output codestream object.

  if (check_jp2_compatible_suffix(ofname))
    { kdu_error e; e << "Transcoder can reads JP2 files, but currently only "
      "writes raw code-stream files.  You should be able to easily add such "
      "capability yourself if you really need it."; }
  kdu_simple_file_target output; output.open(ofname);
  siz_params siz;
  siz.copy_from(siz_in,-1,-1,-1,skip_components,discard_levels,
                transpose,vflip,hflip);
  siz.set(Scomponents,0,0,codestream_in.get_num_components());
  kdu_codestream codestream_out; codestream_out.create(&siz,&output);
  codestream_out.share_buffering(codestream_in);
  siz_params *siz_out = codestream_out.access_siz();
  siz_out->copy_from(siz_in,-1,-1,-1,skip_components,discard_levels,
                     transpose,vflip,hflip);
  char *string;
  for (string=args.get_first(); string != NULL; )
    string = args.advance(siz_out->parse_string(string,-1));
  codestream_out.access_siz()->finalize_all(-1);
  codestream_out.set_textualization(record_stream);

  check_parameter_args(args);

  // Now ready to perform the transfer of compressed data between streams

  kdu_dims tile_indices_in;  codestream_in.get_valid_tiles(tile_indices_in);
  kdu_dims tile_indices_out; codestream_out.get_valid_tiles(tile_indices_out);
  assert((tile_indices_in.size.x == tile_indices_out.size.x) &&
         (tile_indices_in.size.y == tile_indices_out.size.y));
  int num_blocks=0;

  kdu_coords idx;
  for (idx.y=0; idx.y < tile_indices_out.size.y; idx.y++)
    for (idx.x=0; idx.x < tile_indices_out.size.x; idx.x++)
      {
        kdu_tile tile_in = codestream_in.open_tile(idx+tile_indices_in.pos);
        int tnum_in = tile_in.get_tnum();
        int tnum_out = idx.x + idx.y*tile_indices_out.size.x;
        siz_out->copy_from(siz_in,tnum_in,tnum_out,0,skip_components,
                           discard_levels,transpose,vflip,hflip);
        for (string=args.get_first(); string != NULL; )
          string = args.advance(siz_out->parse_string(string,tnum_out));
        siz_out->finalize_all(tnum_out);
           /* Note carefully: we must not open the output tile without
              first copying any tile-specific code-stream parameters, as
              above.  It is tempting to do this. */
        kdu_tile tile_out = codestream_out.open_tile(idx+tile_indices_out.pos);
        assert(tnum_out == tile_out.get_tnum());
        copy_tile(tile_in,tile_out,tnum_in,tnum_out,siz_in,siz_out,
                  skip_components,num_blocks);
      }

  /* Leave argument warnings to this point, because we need to be able to
     parse arguments incrementally as tiles become available. */

  if (args.show_unrecognized(pretty_cout) != 0)
    { kdu_error e; e << "There were unrecognized command line arguments!"; }

  // Generate the output code-stream

  int max_bytes = INT_MAX;
  if (max_bpp > 0.0F)
    max_bytes = (int)(0.125*max_bpp*get_bpp_dims(siz_out));
  int non_empty_layers = codestream_out.trans_out(max_bytes);
  kdu_params *cod = siz_out->access_cluster(COD_params);
  int total_layers;  cod->get(Clayers,0,0,total_layers);
  if (non_empty_layers > total_layers)
    non_empty_layers = total_layers; // Can happen if a tile has more layers

  // Cleanup

  if (mem)
    {
      pretty_cout << "Total compressed data memory = "
                  << codestream_out.get_compressed_data_memory()
                  << " bytes.\n";
      pretty_cout << "Total state memory associated with compressed data = "
                  << codestream_in.get_compressed_state_memory() +
                  codestream_out.get_compressed_state_memory() << " bytes.\n";
    }

  if (!quiet)
    {
      pretty_cout << "\nOutput contains "
                  << total_layers
                  << " quality layers";
      if (non_empty_layers < total_layers)
        pretty_cout << " (" << total_layers-non_empty_layers
                    << " of these were assigned empty packets)\n";
      else
        pretty_cout << "\n";

      pretty_cout << "Transferred " << num_blocks << " code-blocks.\n";

      pretty_cout << "\nRead " << codestream_in.get_num_tparts()
                  << " tile-part(s) from a total of "
                  << tile_indices_in.area() << " tile(s).\n";
      pretty_cout << "Total bytes read = " << codestream_in.get_total_bytes()
                  << " = "
                  << 8.0*codestream_in.get_total_bytes() /
                     get_bpp_dims(codestream_in.access_siz())
                  << " bits/pel.\n";

      pretty_cout << "\nWrote " << codestream_out.get_num_tparts()
                  << " tile-part(s) in a total of "
                  << tile_indices_out.area() << " tile(s).\n";
      pretty_cout << "Total bytes written = "
                  << codestream_out.get_total_bytes()
                  << " = "
                  << 8.0*codestream_out.get_total_bytes() /
                     get_bpp_dims(codestream_out.access_siz())
                  << " bits/pel.\n";
    }

  codestream_out.destroy();
  codestream_in.destroy();
  input->close();
  if (record_stream != NULL)
    delete record_stream;

  return 0;
}
