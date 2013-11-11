			/*****************************************************************************/
// File: kdu_expand.cpp [scope = APPS/DECOMPRESSOR]
// Version: Kakadu, V2.2
// Author: David Taubman
// Last Revised: 20 June, 2001
/*****************************************************************************/
// Copyright 2001, David Taubman, The University of New South Wales (UNSW)
// The copyright owner is Unisearch Ltd, Australia (commercial arm of UNSW)
// Neither this copyright statement, nor the licensing details below
// may be removed to this file or dissociated to its contents.
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
   File-based JPEG2000 decompressor application, demonstrating quite a few of
the decompression-oriented capabilities of the Kakadu framework.  For further
demonstration of these capabilities, refer to the "kdu_show" application.
******************************************************************************/

// System includes
#include <string.h>
#include <stdio.h> // so we can use `sscanf' for arg parsing.
#include <math.h>
#include <assert.h>
#include <fstream>
// Kakadu core includes
#include "kdu_elementary.h"
#include "kdu_messaging.h"
#include "kdu_params.h"
#include "kdu_compressed.h"
#include "kdu_sample_processing.h"
// Application includes
#include "kdu_args.h"
#include "kdu_image.h"
#include "kdu_file_io.h"
#include "jp2.h"
#include "kdu_expand_bg.h"


/* ========================================================================= */
/*                      Pretty Printing Version of `cout'                    */
/* ========================================================================= */

static kdu_pretty_buf pretty_cout_buf(&std::cout);
static std::ostream pretty_cout(&pretty_cout_buf);

 static void print_usage(char *prog, bool comprehensive=false)
{
  kdu_pretty_buf strbuf(&std::cout);
  std::ostream out(&strbuf);

  out << "Usage:\n  \"" << prog << " ...\n";
  strbuf.set_master_indent(3);
  out << "-i <compressed file>\n";
  if (comprehensive)
      out << "\tCurrently accepts raw code-stream files and code-streams "
             "wrapped in any JP2 compatible file format.  The file suffix "
             "is currently used to identify the type of file which is being "
             "supplied, where a \".jp2\" or \".jpx\" suffix (case "
             "insensitive) is required if the file is to be treated as "
             "anything other than a raw code-stream.\n";
  out << "-o <file 1>,...\n";
  if (comprehensive)
      out << "\tOne or more output files. If multiple files are provided, "
             "they must be separated by commas. Any spaces will be treated as "
             "part of the file name.  This argument is not mandatory; if "
             "no output files are given, the decompressor will run completely "
             "but produce no image output.  This can be useful for timing "
             "purposes.  Currently accepted image file formats "
             "include RAW, BMP, PGM and PPM.  There need not be sufficient "
             "image files to represent all image components in the "
             "code-stream.  Raw files are written with the sample bits in the "
             "least significant bit positions of an 8, 16, 24 or 32 bit word, "
             "depending on the bit-depth.  For signed data, the word is sign "
             "extended. The word organization is big-endian.\n";
  out << "-raw_components\n";
  if (comprehensive)
    out << "\tBy default, when a JP2 compatible file is decompressed, only "
           "the colour components are decompressed (one for luminance, three "
           "for RGB), applying any required palette mapping.  In some cases, "
           "however, it may be desirable to decompress all of the raw "
           "image components available to the code-stream.  Use this "
           "switch to accomplish this.  There will then be no palette "
           "mapping.\n";
  out << "-rotate <degrees>\n";
  if (comprehensive)
    out << "\tRotate source image prior to compression. "
           "Must be multiple of 90 degrees.\n";
  out << "-rate <bits per pixel>\n";
  if (comprehensive)
    out << "\tMaximum bit-rate, expressed in terms of the ratio between the "
           "total number of compressed bits (including headers) and the "
           "product of the largest horizontal and  vertical image component "
           "dimensions. Note that we use the original dimensions of the "
           "compressed image, regardless or resolution scaling and regions "
           "of interest.  Also, the file is simply truncated to this limit.\n";
  out << "-skip_components <num initial image components to skip>\n";
  if (comprehensive)
    out << "\tSkips over one or more initial image components, reconstructing "
           "as many remaining image components as can be stored in the "
           "output image file(s) specified with \"-o\" (or all remaining "
           "components, if no \"-o\" argument is supplied).  This argument "
           "is not meaningful if the input is a JP2 compatible file, unless "
           "the `-raw_components' switch is also selected.\n";
  out << "-reduce <discard levels>\n";
  if (comprehensive)
    out << "\tSet the number of highest resolution levels to be discarded.  "
           "The image resolution is effectively divided by 2 to the power of "
           "the number of discarded levels.\n";
  out << "-region {<top>,<left>},{<height>,<width>}\n";
  if (comprehensive)
    out << "\tEstablish a region of interest within the original compressed "
           "image.  Only the region of interest will be decompressed and the "
           "output image dimensions will be modified accordingly.  The "
           "coordinates of the top-left corner of the region are given first, "
           "separated by a comma and enclosed in curly braces, after which "
           "the dimensions of the region are given in similar fashion.  The "
           "two coordinate pairs must be separated by a comma, with no "
           "intervening spaces.  All coordinates and dimensions are expressed "
           "relative to the origin and dimensions of the high resolution "
           "grid, using real numbers in the range 0 to 1.\n";
  out << "-precise -- forces the use of 32-bit representations.\n";
  if (comprehensive)
    out << "\tBy default, 16-bit data representations will be employed for "
           "sample data processing operations (colour transform and DWT) "
           "whenever the image component bit-depth is sufficiently small.\n";
  out << "-fussy\n";
  if (comprehensive)
    out << "\tEncourage fussy code-stream parsing, in which most code-stream "
           "compliance failures will terminate execution, with an appropriate "
           "error message.\n";
  out << "-resilient\n";
  if (comprehensive)
    out << "\tEncourage error resilient processing, in which an attempt is "
           "made to recover to errors in the code-stream with minimal "
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
  out << "-cpu <coder-iterations>\n";
  if (comprehensive)
    out << "\tTimes end-to-end execution and, optionally, the block decoding "
           "operation, reporting throughput statistics.  If "
           "`coder-iterations' is 0, the block decoder will not be timed, "
           "leading to the most accurate end-to-end system execution "
           "times.  Otherwise, `coder-iterations' must be a positive "
           "integer -- larger values will result in more accurate "
           "estimates of the block decoder processing time, but "
           "degrade the accuracy of end-to-end execution times.  "
           "Note that end-to-end times include image file writing, which "
           "can have a dominant impact.  To avoid this, you may specify "
           "no output files at all.\n";
  out << "-mem -- Report memory usage\n";
  out << "-s <switch file>\n";
  if (comprehensive)
    out << "\tSwitch to reading arguments to a file.  In the file, argument "
           "strings are separated by whitespace characters, including spaces, "
           "tabs and new-line characters.  Comments may be included by "
           "introducing a `#' or a `%' character, either of which causes "
           "the remainder of the line to be discarded.  Any number of "
           "\"-s\" argument switch commands may be included on the command "
           "line.\n";
  out << "-record <file>\n";
  if (comprehensive)
    out << "\tRecord code-stream parameters in a file, using the same format "
           "which is accepted when specifying the parameters to the "
           "compressor. Parameters specific to tiles which do not intersect "
           "with the region of interest will not generally be recorded.\n";
  out << "-quiet -- suppress informative messages.\n";
  out << "-usage -- print a comprehensive usage statement.\n";
  out << "-u -- print a brief usage statement.\"\n\n";
  out.flush();
  exit(0);
}

/*****************************************************************************/
/* STATIC                     parse_simple_args                              */
/*****************************************************************************/

static kde_file_binding *
  parse_simple_args(kdu_args &args, char * &ifname,
                    std::ostream * &record_stream,
                    float &max_bpp, bool &transpose, bool &vflip, bool &hflip,
                    bool &allow_shorts, int &skip_components,
                    bool &raw_components, int &discard_levels,
                    int &cpu_iterations, bool &mem, bool &quiet)
  /* Parses most simple arguments (those involving a dash). Most parameters are
     returned via the reference arguments, with the exception of the input
     file names, which are returned via a linked list of `kde_file_binding'
     objects.  Only the `fname' field of each `kde_file_binding' record is
     filled out here.  Note that `max_bpp' is returned as negative if the
     bit-rate is not explicitly set.  Note also that the function may return
     NULL if no output files are specified; in this case, the decompressor
     is expected to run completely, but not output anything. The value returned
     via `cpu_iterations' is negative unless CPU times are required. */

{
  int rotate;
  kde_file_binding *files, *last_file, *new_file;

  if ((args.get_first() == NULL) || (args.find("-u") != NULL))
    print_usage(args.get_prog_name());
  if (args.find("-usage") != NULL)
    print_usage(args.get_prog_name(),true);

  files = last_file = NULL;
  ifname = NULL;
  record_stream = NULL;
  rotate = 0;
  max_bpp = -1.0F;
  allow_shorts = true;
  skip_components = 0;
  raw_components = false;
  discard_levels = 0;
  cpu_iterations = -1;
  mem = false;
  quiet = false;

  if (args.find("-o") != NULL)
    {
      char *string, *cp;
      int len;

      if ((string = args.advance()) == NULL)
        { kdu_error e; e << "\"-o\" argument requires a file name!"; }
      while ((len=strlen(string)) > 0)
        {
          cp = strchr(string,',');
          if (cp == NULL)
            cp = string+len;
          new_file = new kde_file_binding(string,cp-string);
          if (last_file == NULL)
            files = last_file = new_file;
          else
            last_file = last_file->next = new_file;
          if (*cp == ',') cp++;
          string = cp;
        }
      args.advance();
    }

  if (args.find("-i") != NULL)
    {
      if ((ifname = args.advance()) == NULL)
        { kdu_error e; e << "\"-i\" argument requires a file name!"; }
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

  if (args.find("-skip_components") != NULL)
    {
      char *string = args.advance();
      if ((string == NULL) || (sscanf(string,"%d",&skip_components) != 1) ||
          (skip_components < 0))
        { kdu_error e; e << "\"-skip_components\" argument requires a "
          "non-negative integer parameter!"; }
      args.advance();
    }

  if (args.find("-raw_components") != NULL)
    {
      raw_components = true;
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

  if (args.find("-precise") != NULL)
    {
      args.advance();
      allow_shorts = false;
    }

  if (args.find("-cpu") != NULL)
    {
      char *string = args.advance();
      if ((string == NULL) || (sscanf(string,"%d",&cpu_iterations) != 1) ||
          (cpu_iterations < 0))
        { kdu_error e; e << "\"-cpu\" argument requires a non-negative "
          "integer, specifying the number of times to execute the block "
          "coder within a timing loop."; }
      args.advance();
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

  if (ifname == NULL)
    { kdu_error e; e << "Must provide an input file name!"; }
  while (rotate >= 4)
    rotate -= 4;
  while (rotate < 0)
    rotate += 4;
  switch (rotate) {
    case 0: transpose = false; vflip = false; hflip = false; break;
    case 1: transpose = true; vflip = false; hflip = true; break;
    case 2: transpose = false; vflip = true; hflip = true; break;
    case 3: transpose = true; vflip = true; hflip = false; break;
    }

  return(files);
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
/* STATIC                     set_region_of_interest                         */
/*****************************************************************************/

static void
  set_region_of_interest(kdu_args &args, kdu_dims &region, siz_params *siz)
{
  if (!(siz->get(Sorigin,0,0,region.pos.y) &&
        siz->get(Sorigin,0,1,region.pos.x) &&
        siz->get(Ssize,0,0,region.size.y) &&
        siz->get(Ssize,0,1,region.size.x)))
    assert(0);
  region.size.y -= region.pos.y;
  region.size.x -= region.pos.x;
  if (args.find("-region") == NULL)
    return;
  char *string = args.advance();
  if (string != NULL)
    {
      double top, left, height, width;

      if (sscanf(string,"{%lf,%lf},{%lf,%lf}",&top,&left,&height,&width) != 4)
        string = NULL;
      else if ((top < 0.0) || (left < 0.0) || (height < 0.0) || (width < 0.0))
        string = NULL;
      else
        {
          region.pos.y += (int) floor(region.size.y * top);
          region.pos.x += (int) floor(region.size.x * left);
          region.size.y = (int) ceil(region.size.y * height);
          region.size.x = (int) ceil(region.size.x * width);
        }
    }
  if (string == NULL)
    { kdu_error e; e << "The `-region' argument requires a set of coordinates "
      "of the form, \"{<top>,<left>},{<height>,<width>}\". All quantities "
      "must be real numbers in the range 0 to 1."; }
  args.advance();
}

/*****************************************************************************/
/* STATIC             convert_samples_to_palette_indices                     */
/*****************************************************************************/

static void
  convert_samples_to_palette_indices(kdu_line_buf &line, int bit_depth,
                                     bool is_signed, int palette_bits)
{
  int i=line.get_width();
  if (line.get_buf32() != NULL)
    {
      assert(line.is_absolute());
      kdu_sample32 *sp = line.get_buf32();
      kdu_int32 offset = (is_signed)?0:((1<<bit_depth)>>1);
      kdu_int32 mask = ((kdu_int32)(-1))<<palette_bits;
      kdu_int32 val;
      for (; i > 0; i--, sp++)
        {
          val = sp->ival + offset;
          if (val & mask)
            val = (val<0)?0:(~mask);
          sp->ival = val;
        }
    }
  else if (line.is_absolute())
    {
      kdu_sample16 *sp = line.get_buf16();
      kdu_int16 offset = (kdu_int16)((is_signed)?0:((1<<bit_depth)>>1));
      kdu_int16 mask = ((kdu_int16)(-1))<<palette_bits;
      kdu_int16 val;
      for (; i > 0; i--, sp++)
        {
          val = sp->ival + offset;
          if (val & mask)
            val = (val<0)?0:(~mask);
          sp->ival = val;
        }
    }
  else
    {
      kdu_sample16 *sp = line.get_buf16();
      kdu_int16 offset = (kdu_int16)((is_signed)?0:((1<<KDU_FIX_POINT)>>1));
      int downshift = KDU_FIX_POINT-palette_bits; assert(downshift > 0);
      offset += (kdu_int16)((1<<downshift)>>1);
      kdu_int32 mask = ((kdu_int16)(-1))<<palette_bits;
      kdu_int16 val;
      for (; i > 0; i--, sp++)
        {
          val = (sp->ival + offset) >> downshift;
          if (val & mask)
            val = (val<0)?0:(~mask);
          sp->ival = val;
        }
    }
}


kde_flow_control_bg::kde_flow_control_bg(kde_file_binding *files, int num_channels,
                     kdu_codestream codestream,kde_flow_control* param_flow_, int x_tnum, bool allow_shorts,
					 bbgm_io& bgm_reader_):bgm_interface(bgm_reader_)
{
  int c;

  this->codestream = codestream;
  this->param_flow=param_flow_;
  //this->bgm_interface=bgm_reader_;
  codestream.get_valid_tiles(this->valid_tile_indices);
  assert((x_tnum >= 0) && (x_tnum < valid_tile_indices.size.x));
  this->tile_idx = valid_tile_indices.pos;
  this->tile_idx.x += x_tnum;
  this->x_tnum = x_tnum;
  this->tile =this->param_flow->tile;
  this->num_components = codestream.get_num_components();
  this->num_channels = num_channels;
  components = new kde_component_flow_control_bg[num_components];
  channels = new kde_channel_bg[num_components];
  count_delta = 0;
  
  // Initialize components
  for (c=0; c < num_components; c++)
    {
      kde_component_flow_control_bg *comp = components + c;
      comp->tc = tile.access_component(c);
      comp->reversible = comp->tc.get_reversible();
      comp->is_signed = comp->tc.get_signed();
      comp->bit_depth = comp->tc.get_bit_depth();
      comp->mapped_by_channel = false;
      comp->palette_bits = 0;
      kdu_coords subsampling; comp->tc.get_subsampling(subsampling);
      comp->res = comp->tc.access_resolution();
      kdu_dims dims; comp->res.get_dims(dims);
      comp->width = dims.size.x;
      comp->vert_subsampling = subsampling.y;
      if ((c == 0) || (comp->vert_subsampling < count_delta))
        count_delta = comp->vert_subsampling; // Delta is min sampling factor
      comp->ratio_counter = 0;
      comp->remaining_lines = dims.size.y;
      comp->allow_shorts = allow_shorts;
    }
  
  // Initialize channels
   for (c=0; c < num_channels; c++)
    {
      kde_channel_bg *chnl = channels +c;
       chnl->writer = &bgm_interface;
      int cmp=c, plt_cmp=-1;
      chnl->source_component = components + cmp;
      chnl->source_component->mapped_by_channel = true;
      chnl->width = chnl->source_component->width;
      chnl->allocator = &(chnl->source_component->allocator);
      if (plt_cmp < 0)
        chnl->lut = NULL;
     
    }

  // Complete components and channels
  use_ycc = false;
  for (c=0; c < num_components; c++)
    {
      kde_component_flow_control_bg *comp = components + c;
      if (!comp->mapped_by_channel)
        continue;
      bool use_shorts = comp->allow_shorts;
      if ((comp->tc.get_bit_depth(true) > 16) &&
          ((comp->palette_bits == 0) || comp->reversible))
        use_shorts = false;
      comp->line.pre_create(&(comp->allocator),comp->width,
                            comp->reversible,use_shorts,true);
	  kdu_pull_ifc* param_synth_network=new kdu_pull_ifc[param_flow->num_components];
	  for (int i=0;i<param_flow->num_components;i++)
		  param_synth_network[i]=(this->param_flow->components[i].decompressor);

      if (comp->res.which() == 0)
        comp->decompressor = bgmu_LL_synthesis(comp->res.access_subband(LL_BAND),
                                         &(comp->allocator),bgm_interface,param_flow,param_synth_network);
      else
        comp->decompressor = kdu_bg_synthesis(comp->res,
                                           &(comp->allocator),use_shorts,bgm_interface,1.0F
										   ,param_flow,param_synth_network);
    }
  use_ycc = false;  

  // Finalize resources
  for (c=0; c < num_components; c++)
    if (components[c].mapped_by_channel)
      {
        components[c].allocator.finalize();
        components[c].line.create();
      }
  for (c=0; c < num_channels; c++)
    channels[c].line.create(); // Does no harm if not pre-created.
}

/*****************************************************************************/
/*                    kde_flow_control_bg::~kde_flow_control_bg                    */
/*****************************************************************************/

kde_flow_control_bg::~kde_flow_control_bg()
{
  for (int n=0; n < num_components; n++)
    {
      kde_component_flow_control_bg *comp = components + n;
      if (comp->decompressor.exists())
        comp->decompressor.destroy();
    }
  delete[] components;
  for (int c=0; c < num_channels; c++)
    {
      kde_channel_bg *chnl = channels + c;
      if (chnl->lut != NULL)
        delete[] (chnl->lut);
    }
  delete[] channels;
}

/*****************************************************************************/
/*                    kde_flow_control_bg::advance_components                   */
/*****************************************************************************/

bool				
  kde_flow_control_bg::advance_components()
{
  bool found_line=false;

  while (!found_line)
    {
      bool all_done = true;
      for (int n=0; n < num_components; n++)
        {
          kde_component_flow_control_bg *comp = components + n;
          assert(comp->ratio_counter >= 0);
          if (comp->mapped_by_channel && (comp->remaining_lines > 0))
            {
              all_done = false;
              comp->ratio_counter -= count_delta;
              if (comp->ratio_counter < 0)
                {
                  found_line = true;
                  comp->decompressor.pull(comp->line,true);
                  if (comp->palette_bits > 0)
                    convert_samples_to_palette_indices(comp->line,
                         comp->bit_depth,comp->is_signed,comp->palette_bits);
                }
            }
        }
      if (all_done)
        return false;
    }

   

  return true;
}

/*****************************************************************************/
/*                 kde_flow_control_bg::access_decompressed_line                */
/*****************************************************************************/

kdu_line_buf *
  kde_flow_control_bg::access_decompressed_line(int channel_idx)
{
  assert((channel_idx >= 0) && (channel_idx < num_channels));
  kde_channel_bg *chnl = channels + channel_idx;
  kde_component_flow_control_bg *comp = chnl->source_component;
  if (comp->ratio_counter >= 0)
    return NULL;
  if (chnl->lut != NULL)
    return &(chnl->line);
  else
    return &(comp->line);
}

/*****************************************************************************/
/*                    kde_flow_control_bg::process_components                   */
/*****************************************************************************/

void
  kde_flow_control_bg::process_components()
{
  for (int c=0; c < num_channels; c++)
    {
      kde_channel_bg *chnl = channels + c;
      kde_component_flow_control_bg *comp = chnl->source_component;
      if ((comp->ratio_counter < 0) && chnl->writer->exists() && chnl->width)
        {
          chnl->writer->put(comp->line,x_tnum);
        }
    }

  for (int n=0; n < num_components; n++)
    {
      kde_component_flow_control_bg *comp = components + n;
      if (comp->ratio_counter < 0)
        {
          comp->ratio_counter += comp->vert_subsampling;
          assert(comp->ratio_counter >= 0);
          assert(comp->remaining_lines > 0);
          comp->remaining_lines--;
        }
    }
}

/*****************************************************************************/
/*                        kde_flow_control_bg::advance_tile                     */
/*****************************************************************************/

bool
  kde_flow_control_bg::advance_tile()
{
  int c;

  if (!tile)
    return false;

  // Clean up existing resources
  for (c=0; c < num_components; c++)
    {
      kde_component_flow_control_bg *comp = components + c;
      if (!comp->mapped_by_channel)
        continue;
      assert(comp->remaining_lines == 0);
      assert(comp->decompressor.exists());
      comp->decompressor.destroy();
      comp->line.destroy();
    }
  for (c=0; c < num_channels; c++)
    {
      kde_channel_bg *chnl = channels + c;
      chnl->line.destroy();
    }

  // Advance to next vertical tile.
  tile.close();
  tile = kdu_tile(NULL);
  tile_idx.y++;
  if ((tile_idx.y-valid_tile_indices.pos.y) == valid_tile_indices.size.y)
    return false;
  tile = codestream.open_tile(tile_idx);

  // Prepare for processing the new tile.
  use_ycc = tile.get_ycc();
  for (c=0; c < num_components; c++)
    {
      kde_component_flow_control_bg *comp = components + c;
      comp->tc = tile.access_component(c);
      comp->res = comp->tc.access_resolution();
      comp->reversible = comp->tc.get_reversible();
      if (!comp->mapped_by_channel)
        continue;
      kdu_dims dims; comp->res.get_dims(dims);
      comp->ratio_counter = 0;
      comp->remaining_lines = dims.size.y;
      comp->allocator.restart();
      bool use_shorts = comp->allow_shorts;
      if ((comp->tc.get_bit_depth(true) > 16) &&
          ((comp->palette_bits == 0) || comp->reversible))
        use_shorts = false;
      comp->line.pre_create(&(comp->allocator),comp->width,
                            comp->reversible,use_shorts);
      if (comp->res.which() == 0)
        comp->decompressor = kdu_decoder(comp->res.access_subband(LL_BAND),
                                         &(comp->allocator),use_shorts);
      else
        comp->decompressor = kdu_synthesis(comp->res,
                                           &(comp->allocator),use_shorts);
    }
  for (c=0; c < num_channels; c++)
    if (channels[c].lut != NULL)
      {
        if (use_ycc && ((channels[c].source_component - components) < 3))
          { kdu_error e; e << "It is illegal for a JP2 file to identify a "
            "code-stream component as index to a Palette lookup table, if the "
            "component is also part of a code-stream colour transform (RCT "
            "or ICT)."; }
        channels[c].line.pre_create(channels[c].allocator,
                                    channels[c].width,false,true);
      }

  // Turn off colour transforms if we only want a subset of the components
  if (!(components[0].mapped_by_channel &&
        components[1].mapped_by_channel &&
        components[2].mapped_by_channel))
    use_ycc = false;

  // Finalize resources
  for (c=0; c < num_components; c++)
    if (components[c].mapped_by_channel)
      {
        components[c].allocator.finalize();
        components[c].line.create();
      }
  for (c=0; c < num_channels; c++)
    channels[c].line.create(); // Does no harm if not pre-created.
  return true;
}


/* ========================================================================= */
/*                             External Functions                            */
/* ========================================================================= */

/*****************************************************************************/
/*                                   main                                    */
/*****************************************************************************/

bool kde_expand_bg(const bbgm_image_of<bgm_mix>* from,bbgm_image_of<bgm_mix>* to,
	int argc, char* argv[],kdu_dims* region_dims)
{
  kdu_customize_warnings(&std::cout);
  kdu_customize_errors(&std::cerr);
  char** rv_argv=new char*[argc];
  for (int i=0;i<argc;i++)
   {  rv_argv[i]=new char[400];
	  strcpy(rv_argv[i],argv[i]);
   }
  kdu_customize_errors(&std::cerr);
  kdu_args args(argc,argv,"-s");
  kdu_args rv_args(argc,rv_argv,"-s");
  

  // Collect simple arguments.

  bool transpose, vflip, hflip, allow_shorts, mem, quiet;
  char *ifname;
  std::ostream *record_stream;
  float max_bpp;
  bool raw_components;
  int skip_components, discard_levels, cpu_iterations;
  kde_file_binding *outputs =
    parse_simple_args(args,ifname,record_stream,max_bpp,
                      transpose,vflip,hflip,allow_shorts,
                      skip_components,raw_components,discard_levels,
                      cpu_iterations,mem,quiet);

  // Create appropriate input file.

  kdu_compressed_source *input = NULL;
  kdu_simple_file_source file_in;
  jp2_source jp2_in;
  jp2_channels channels;
  jp2_palette palette;
  if (check_jp2_compatible_suffix(ifname))
    {
      if (skip_components & !raw_components)
        { kdu_error e; e << "The `-skip_components' argument may be "
          "used only with raw code-stream sources or with the "
          "`-raw_components' switch."; }
      input = &jp2_in;
      jp2_in.open(ifname);
      if (!raw_components)
        {
          channels = jp2_in.access_channels();
          palette = jp2_in.access_palette();
        }
    }
  else
    {
      input = &file_in;
      file_in.open(ifname);
      raw_components = true;
    }

  // Create the codestream object.
  siz_params siz_rv;
  int nLevels,nLayers;
  kdu_codestream rv_codestream,param_codestream;
  param_codestream.create(input);
 
  
  //transformed_siz_rv.access_cluster(COD_params)->get("Clevels",0,0,nLevels);
  set_error_behaviour(args,param_codestream);
  if (cpu_iterations >= 0)
    param_codestream.collect_timing_stats(cpu_iterations);
  if (max_bpp > 0.0F)
    param_codestream.set_max_bytes((int)(0.125*max_bpp*
                                   get_bpp_dims(param_codestream.access_siz())));
  param_codestream.set_textualization(record_stream);
  kdu_dims region;
  set_region_of_interest(args,region,param_codestream.access_siz());
  
  param_codestream.apply_input_restrictions(skip_components,0,discard_levels,0,
                                      &region);	
   
  param_codestream.access_siz()->access_cluster(COD_params)->get("Clevels",0,0,nLevels);
  param_codestream.access_siz()->access_cluster(COD_params)->get("Clayers",0,0,nLayers);
  siz_params rv_siz;
  copy_sz_from_input(&rv_siz,param_codestream.access_siz());
  rv_siz.finalize_all();
  rv_codestream.create(&rv_siz,0);
  rv_codestream.access_siz()->access_cluster(COD_params)->set("Clevels",0,0,nLevels);
  rv_codestream.access_siz()->access_cluster(COD_params)->set("Clayers",0,0,nLayers);  
  param_codestream.change_appearance(transpose,vflip,hflip);
  
  if (args.show_unrecognized(pretty_cout) != 0)
    { kdu_error e; e << "There were unrecognized command line arguments!"; }

  // Get the component (or mapped colour channel) dimensional properties
  kdu_image_dims idims;
  int num_channels, num_components=param_codestream.get_num_components();
  if (channels.exists())
    num_channels = channels.get_num_colours();
  else
    num_channels = num_components;
 

  // Now we are ready to open the output files.
   bool extra_flip = false;
  int output_channels=0;
  rv_codestream.access_siz()->finalize_all();
  if (output_channels == 0)
    output_channels = num_channels; // There may be no output files specified.
  // The following call saves us the cost of buffering up unused image
  // components, but not when working with the JP2 file format, for simplicity.
  param_codestream.apply_input_restrictions(skip_components,
             ((jp2_in.exists())?0:output_channels),discard_levels,0,&region);

  kdu_dims out_dims	;
  param_codestream.get_dims(0,out_dims);
  // Now we are ready for sample data processing.

  int x_tnum;
  to->set_size(out_dims.size.x,out_dims.size.y);
  if (region_dims)
	  *region_dims=out_dims;

  
  bbgm_io bg_reader(from,to,nLevels);
  kdu_dims tile_indices; rv_codestream.get_valid_tiles(tile_indices);
  kde_flow_control_bg **rv_tile_flows = new kde_flow_control_bg *[tile_indices.size.x];
  kde_flow_control **param_tile_flows = new kde_flow_control*[tile_indices.size.x];
  for (x_tnum=0; x_tnum < tile_indices.size.x; x_tnum++)
  {
	  param_tile_flows[x_tnum] =
      new kde_flow_control(0,output_channels,param_codestream,
                           x_tnum,allow_shorts,channels,palette);
	  rv_tile_flows[x_tnum] =
      new kde_flow_control_bg(0,1,rv_codestream,param_tile_flows[x_tnum],
                           x_tnum,allow_shorts,bg_reader);
  }

  bool done = false;

  while (!done)
    {
      while (!done)
        { // Process a row of tiles line by line.
          done = true;
          for (x_tnum=0; x_tnum < tile_indices.size.x; x_tnum++)
            {
              if (rv_tile_flows[x_tnum]->advance_components())
                {
                  done = false;
                  rv_tile_flows[x_tnum]->process_components();
                }
            }
        }
      for (x_tnum=0; x_tnum < tile_indices.size.x; x_tnum++)
        if (rv_tile_flows[x_tnum]->advance_tile())
          done = false;
    }
  int sample_bytes = 0;
  for (x_tnum=0; x_tnum < tile_indices.size.x; x_tnum++)
    {
      sample_bytes += param_tile_flows[x_tnum]->get_buffer_memory();
      delete rv_tile_flows[x_tnum];
	  delete param_tile_flows[x_tnum];
    }
  delete [] param_tile_flows;
  delete [] rv_tile_flows;

  // Cleanup

  if (cpu_iterations >= 0)
    {
      int num_samples;
      double seconds = param_codestream.get_timing_stats(&num_samples);
      pretty_cout << "End-to-end CPU time ";
      if (cpu_iterations > 0)
        pretty_cout << "(estimated) ";
      pretty_cout << "= " << seconds << " seconds ("
                  << 1.0E6*seconds/num_samples << " us/sample)\n";
    }
  if (cpu_iterations > 0)
    {
      int num_samples;
      double seconds = param_codestream.get_timing_stats(&num_samples,true);
      if (seconds > 0.0)
        {
          pretty_cout << "Block decoding CPU time (estimated) ";
          pretty_cout << "= " << seconds << " seconds ("
                      << 1.0E6*seconds/num_samples << " us/sample)\n";
        }
    }
  if (mem)
    {
      pretty_cout << "\nSample processing/buffering memory = "
                  << sample_bytes << " bytes.\n";
      pretty_cout << "Compressed data memory = "
                  << param_codestream.get_compressed_data_memory() << " bytes.\n";
      pretty_cout << "State memory associated with compressed data = "
                  << param_codestream.get_compressed_state_memory() << " bytes.\n";
    }
  if (!quiet)
    {
      pretty_cout << "\nConsumed " << param_codestream.get_num_tparts()
                  << " tile-part(s) to a total of "
                  << tile_indices.area() << " tile(s).\n";
      pretty_cout << "Code-stream bytes (excluding any file format) = "
                  << param_codestream.get_total_bytes() << " = "
                  << 8.0*param_codestream.get_total_bytes() /
                     get_bpp_dims(param_codestream.access_siz())
                  << " bits/pel.\n";
    }
  param_codestream.destroy();
  rv_codestream.destroy();
  input->close();
  if (record_stream != NULL)
    delete record_stream;
  delete outputs;

   for (int i=0;i<argc;i++)
	{
		delete rv_argv[i];
	}
  delete [] rv_argv;

  return true;
}

bool kde_decompress_vil_image(vil_image_view<vxl_byte>& to,int argc, char *argv[],
	kdu_dims* region_dims)
{
  kdu_customize_warnings(&std::cout);
  kdu_customize_errors(&std::cerr);
  kdu_args args(argc,argv,"-s");

  // Collect simple arguments.

  bool transpose, vflip, hflip, allow_shorts, mem, quiet;
  char *ifname;
  std::ostream *record_stream;
  float max_bpp;
  bool raw_components;
  int skip_components, discard_levels, cpu_iterations;
  kde_file_binding *outputs =
    parse_simple_args(args,ifname,record_stream,max_bpp,
                      transpose,vflip,hflip,allow_shorts,
                      skip_components,raw_components,discard_levels,
                      cpu_iterations,mem,quiet);

  // Create appropriate input file.

  kdu_compressed_source *input = NULL;
  kdu_simple_file_source file_in;
  jp2_source jp2_in;
  jp2_channels channels;
  jp2_palette palette;
  if (check_jp2_compatible_suffix(ifname))
    {
      if (skip_components & !raw_components)
        { kdu_error e; e << "The `-skip_components' argument may be "
          "used only with raw code-stream sources or with the "
          "`-raw_components' switch."; }
      input = &jp2_in;
      jp2_in.open(ifname);
      if (!raw_components)
        {
          channels = jp2_in.access_channels();
          palette = jp2_in.access_palette();
        }
    }
  else
    {
      input = &file_in;
      file_in.open(ifname);
      raw_components = true;
    }

  // Create the codestream object.

  kdu_codestream codestream;
  codestream.create(input);
  set_error_behaviour(args,codestream);
  if (cpu_iterations >= 0)
    codestream.collect_timing_stats(cpu_iterations);
  if (max_bpp > 0.0F)
    codestream.set_max_bytes((int)(0.125*max_bpp*
                                   get_bpp_dims(codestream.access_siz())));
  codestream.set_textualization(record_stream);
  kdu_dims region;
  set_region_of_interest(args,region,codestream.access_siz());
  codestream.apply_input_restrictions(skip_components,0,discard_levels,0,
                                      &region);
  codestream.change_appearance(transpose,vflip,hflip);
  if (args.show_unrecognized(pretty_cout) != 0)
    { kdu_error e; e << "There were unrecognized command line arguments!"; }

  // Get the component (or mapped colour channel) dimensional properties
  kdu_image_dims idims;
  int n, num_channels, num_components = codestream.get_num_components();
  if (channels.exists())
    num_channels = channels.get_num_colours();
  else
    num_channels = num_components;
  

  // Now we are ready to open the output files.
  kde_file_binding *oscan;
  bool extra_flip = false;
  int output_channels=0;
  if (extra_flip)
    {
      vflip = !vflip;
      codestream.change_appearance(transpose,vflip,hflip);
    }
  if (output_channels == 0)
    output_channels = num_channels; // There may be no output files specified.
  // The following call saves us the cost of buffering up unused image
  // components, but not when working with the JP2 file format, for simplicity.
  codestream.apply_input_restrictions(skip_components,
             ((jp2_in.exists())?0:output_channels),discard_levels,0,&region);

  // Now we are ready for sample data processing.

  int x_tnum;
  kdu_dims out_dims	;
  codestream.get_dims(0,out_dims);
  to.set_size(out_dims.size.x,out_dims.size.y,codestream.get_num_components());
  int nLevels,precision;
  bool reversible;
  codestream.access_siz()->access_cluster(COD_params)->get("Clevels",0,0,nLevels);
  codestream.access_siz()->access_cluster(SIZ_params)->get("Sprecision",0,0,precision); 
  codestream.access_siz()->access_cluster(COD_params)->get("Creversible",0,0,reversible);
  vil_image_io vil_writer(vil_image_view<vxl_byte>(),to,nLevels,(precision<=16),reversible,true);
  kdu_dims tile_indices; codestream.get_valid_tiles(tile_indices);
  kde_flow_control **tile_flows = new kde_flow_control *[tile_indices.size.x];
  for (x_tnum=0; x_tnum < tile_indices.size.x; x_tnum++)
    tile_flows[x_tnum] =
      new kde_flow_control(outputs,output_channels,codestream,
                           x_tnum,allow_shorts,channels,palette,&vil_writer);
  bool done = false;

  while (!done)
    {
      while (!done)
        { // Process a row of tiles line by line.
          done = true;
          for (x_tnum=0; x_tnum < tile_indices.size.x; x_tnum++)
            {
              if (tile_flows[x_tnum]->advance_components())
                {
                  done = false;
                  tile_flows[x_tnum]->process_components();
                }
            }
        }
      for (x_tnum=0; x_tnum < tile_indices.size.x; x_tnum++)
        if (tile_flows[x_tnum]->advance_tile())
          done = false;
    }
  int sample_bytes = 0;
  for (x_tnum=0; x_tnum < tile_indices.size.x; x_tnum++)
    {
      sample_bytes += tile_flows[x_tnum]->get_buffer_memory();
      delete tile_flows[x_tnum];
    }
  delete tile_flows;

  // Cleanup

  if (cpu_iterations >= 0)
    {
      int num_samples;
      double seconds = codestream.get_timing_stats(&num_samples);
      pretty_cout << "End-to-end CPU time ";
      if (cpu_iterations > 0)
        pretty_cout << "(estimated) ";
      pretty_cout << "= " << seconds << " seconds ("
                  << 1.0E6*seconds/num_samples << " us/sample)\n";
    }
  if (cpu_iterations > 0)
    {
      int num_samples;
      double seconds = codestream.get_timing_stats(&num_samples,true);
      if (seconds > 0.0)
        {
          pretty_cout << "Block decoding CPU time (estimated) ";
          pretty_cout << "= " << seconds << " seconds ("
                      << 1.0E6*seconds/num_samples << " us/sample)\n";
        }
    }
  if (mem)
    {
      pretty_cout << "\nSample processing/buffering memory = "
                  << sample_bytes << " bytes.\n";
      pretty_cout << "Compressed data memory = "
                  << codestream.get_compressed_data_memory() << " bytes.\n";
      pretty_cout << "State memory associated with compressed data = "
                  << codestream.get_compressed_state_memory() << " bytes.\n";
    }
  if (!quiet)
    {
      pretty_cout << "\nConsumed " << codestream.get_num_tparts()
                  << " tile-part(s) from a total of "
                  << tile_indices.area() << " tile(s).\n";
      pretty_cout << "Code-stream bytes (excluding any file format) = "
                  << codestream.get_total_bytes() << " = "
                  << 8.0*codestream.get_total_bytes() /
                     get_bpp_dims(codestream.access_siz())
                  << " bits/pel.\n";
    }
  codestream.destroy();
  input->close();
  if (record_stream != NULL)
    delete record_stream;
  delete outputs;
  return true;

}


bool copy_sz_from_input(siz_params* rv_sz,siz_params* param_sz)
{
	  int width,height,levels;
	  param_sz->access_cluster(COD_params)->get("Clevels",0,0,levels);
	  param_sz->get(Sdims,0,0,height);
	  param_sz->get(Sdims,0,1,width);	
	  rv_sz->set(Sdims,0,0,(int)height);
      rv_sz->set(Sdims,0,1,(int)width);
      rv_sz->set(Ssigned,0,0,true);
      rv_sz->set(Sprecision,0,0,32);
	  rv_sz->set(Scomponents,0,0,1);
	  return true;


}