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
#include "kdu_roi_processing.h"
#include "kdu_sample_processing.h"
// Application includes
#include "kdu_args.h"
#include "kdu_image.h"
#include "kdu_file_io.h"
#include "jp2.h"
#include "compress_local.h"
#include "roi_sources.h"  

/* ========================================================================= */
/*                      Pretty Printing Version of `cout'                    */
/* ========================================================================= */

static kdu_pretty_buf pretty_cout_buf(&std::cout);
static std::ostream pretty_cout(&pretty_cout_buf);
bool create_size_vil(const vil_image_view<vxl_int_16>& from,siz_params &siz);

/* ========================================================================= */
/*                             Internal Functions                            */
/* ========================================================================= */

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
  out << "-i <file 1>,...\n";
  if (comprehensive)
    out << "\tOne or more input files. If multiple files are provided, "
           "they must be separated by commas. Any spaces will be treated as "
           "part of the file name.  Currently accepted image file formats "
           "include RAW, BMP, PBM, PGM and PPM.  For raw files, the sample "
           "bits must be in the least significant bit positions of an 8, 16, "
           "24 or 32 bit word, depending on the bit-depth.  Unused MSB's in "
           "each word are entirely disregarded.  The word organization is "
           "big-endian. Also, with raw files, the dimensions must be provided "
           "separately using `Sdims' or any other appropriate combination of "
           "SIZ-type parameters.\n";
  out << "-o <compressed file -- raw code-stream unless suffix is \".jp2\">\n";
  if (comprehensive)
    out << "\tName of file to receive the compressed code-stream.  If the "
           "file name has a \".jp2\" suffix (not case sensitive), the "
           "code-stream will be wrapped up inside the JP2 file format.  In "
           "this case, the first 3 source image components will be treated "
           "as sRGB colour channels (red, green then blue) and the remainder "
           "will be identified as auxiliary undefined components in the JP2 "
           "file.  If an input file defines a colour palette (quite common "
           "with BMP files), this will be preserved through the JP2 file "
           "format and samples will be correctly de-palettized by "
           "conformant JP2 readers.  If there are fewer than 3 components "
           "available (taking any palette into account), the first component "
           "will be identified as a luminance component with the sRGB "
           "gamma and any remaining component will be identified as an "
           "auxiliary undefined channel.  Some of these default decisions "
           "may be overridden by subsequent command line arguments.\n";
  out << "-roi {<top>,<left>},{<height>,<width>} | <PGM image>,<threshold>\n";
  if (comprehensive)
    out << "\tEstablish a region of interest (foreground) to be coded more "
           "precisely and/or earlier in the progression than the rest of "
           "the image (background).  This argument has no effect unless "
           "the \"Rshift\" attribute is also set.  The \"Rlevels\" attribute "
           "may also be used to control the number of DWT levels which "
           "will be affected by the ROI information.\n"
           "\t   The single parameter supplied with this "
           "argument may take one of two forms.  The first form provides "
           "a simple rectangular region of interest, specified in terms of "
           "its upper left hand corner coordinates (comma-separated and "
           "enclosed in curly braces) and its dimensions (also comma-"
           "separated and enclosed in braces).  All coordinates and "
           "dimensions are expressed relative to the origin and dimensions "
           "of the high resolution grid (or canvas), using real numbers in "
           "the range 0 to 1. If the original image is to be rotated during "
           "compression (see `-rotate'), the coordinates supplied here are "
           "to be interpreted with respect to the orientation of the image "
           "being compressed.\n"
           "\t   The second form for the single parameter string supplied "
           "with the `-roi' argument involves a named (PGM) image file, "
           "separated by a comma from an ensuing real-valued threshold in "
           "the range 0 to 1.  In this case, the image is scaled "
           "(interpolated) to fill the region occupied by each image "
           "component.  Pixel values whose relative amplitude exceeds the "
           "threshold identify the foreground region.\n";
  out << "-rate -|<bits/pel>,<bits/pel>,...\n";
  if (comprehensive)
    out << "\tOne or more bit-rates, expressed in terms of the ratio between "
           "the total number of compressed bits (including headers) and the "
           "product of the largest horizontal and  vertical image component "
           "dimensions.  A dash, \"-\", may be used in place of the first "
           "bit-rate in the list to indicate that the final quality layer "
           "should include all compressed bits.  Specifying a very large "
           "rate target is fundamentally different to using the dash, \"-\", "
           "because the former approach may cause the incremental rate "
           "allocator to discard terminal coding passes which do not lie "
           "on the rate-distortion convex hull.  This means that reversible "
           "compression might not yield a truly lossless representation if "
           "you specify `-rate' without a dash for the first rate target, no "
           "matter how large the largest rate target is.\n"
           "\t   If \"Clayers\" is not used, the number of layers is "
           "set to the number of rates specified here. If \"Clayers\" is used "
           "to specify an actual number of quality layers, one of the "
           "following must be true: 1) the number of rates specified here is "
           "identical to the specified number of layers; or 2) one, two or no "
           "rates are specified using this argument.  When two rates are "
           "specified, the number of layers must be 2 or more and intervening "
           "layers will be assigned roughly logarithmically spaced bit-rates. "
           "When only one rate is specified, an internal heuristic determines "
           "a lower bound and logarithmically spaces the layer rates over the "
           "range.\n";
  out << "-slope <layer slope>,<layer slope>,...\n";
  if (comprehensive)
    out << "\tIf present, this argument provides rate control information "
           "directly in terms of distortion-length slope values, overriding "
           "any information which may or may not have been supplied via a "
           "`-rate' argument.  If the number of quality layers is  not "
           "specified via a `Qlayers' argument, it will be deduced from the "
           "number of slope values.  Slopes are inversely related to "
           "bit-rate, so the slopes should decrease from layer to layer.  The "
           "program automatically sorts slopes into decreasing order so you "
           "need not worry about getting the order right.  For reference "
           "we note that a slope value of 0 means that all compressed bits "
           "will be included by the end of the relevant layer, while a "
           "slope value of 65535 means that no compressed bits will be "
           "included in the  layer.\n";
  out << "-full -- forces encoding and storing of all bit-planes.\n";
  if (comprehensive)
    out << "\tBy default, the system incrementally constructs conservative "
           "estimates of the final rate allocation parameters and uses these "
           "to skip coding passes which are very likely to be discarded "
           "during rate allocation.  It also discards any compressed "
           "code-bytes which we know we will not be needing on a regular "
           "basis, so as to conserve memory.  For large images, the memory "
           "consumption can approach the final compressed file size.  You "
           "might like to use the `-full' argument if you are compressing "
           "an image with highly non-uniform statistics.\n";
  out << "-precise -- forces the use of 32-bit representations.\n";
  if (comprehensive)
    out << "\tBy default, 16-bit data representations will be employed for "
           "sample data processing operations (colour transform and DWT) "
           "whenever the image component bit-depth is sufficiently small.\n";
  out << "-no_weights -- target MSE minimization for colour images.\n";
  if (comprehensive)
    out << "\tBy default, visual weights will be automatically used for "
           "colour imagery (anything with 3 compatible components).  Turn "
           "this off if you want direct minimization of the MSE over all "
           "reconstructed colour components.\n";
  out << "-no_palette\n";
  if (comprehensive)
    out << "\tThis argument is meaningful only when reading palettized "
           "imagery and compressing to a JP2 file.  By default, the "
           "palette will be preserved in the JP2 file and only the "
           "palette indices will be compressed.  In many cases, it may "
           "be more efficient to compress the RGB data as a 24-bit "
           "continuous tone image. To make sure that this happens, select "
           "the `-no_palette' option.\n";
  out << "-jp2_space <sLUM|sRGB|sYCC|iccLUM|iccRGB>[,<parameters>]\n";
  if (comprehensive)
    out << "\tIdentifies the number of colour channels and the type of "
           "colour space. The argument is illegal except when the output "
           "file has the \".jp2\" suffix, as explained above.  The argument "
           "must be followed by a single string consisting of one of the 5 "
           "colour space names, possibly followed by a comma-separated list "
           "of parameters.\n"
           "\t   If the space is \"iccLUM\", two parameters must "
           "follow, `gamma' and `beta', which identify the tone reproduction "
           "curve.  As examples, the sRGB space has gamma=2.4 and beta=0.055, "
           "while NTSC RGB has gammma=2.2 and beta=0.099.  A pure power law "
           "has beta=0, but is not recommended due to the ill-conditioned "
           "nature of the resulting function around 0.\n"
           "\t   If the space is \"iccRGB\", 11 parameters must follow in "
           "the comma separated list.  The first two of these are the gamma "
           "and beta values, as above.  The next 9 parameters hold the X,Y,Z "
           "coordinates (nominally in the range 0 to 1) of the three colour "
           "channels, after undoing the gamma compensation to obtain a "
           "linear representation.  The first three parameters hold the X "
           "coordinates of the red, green and blue channels, respectively.  "
           "The the next three parameters hold the Y coordinates and the "
           "last three parameters hold the Z coordinates.  Note that the "
           "XYZ representation should be adjusted for the desired appearance "
           "under CIE D50 illuminant.\n";
  out << "-jp2_aspect <aspect ratio of high-res canvas grid>\n";
  if (comprehensive)
    out << "\tIdentifies the aspect ratio to be used by a conformant JP2 "
           "reader when rendering the decompressed image to a display, "
           "printer or other output device.  The aspect ratio identifies "
           "ratio formed by dividing the vertical grid spacing by the "
           "horizontal grid spacing, where the relevant grid is that of the "
           "high resolution canvas.  Sub-sampling factors determine the "
           "number of high resolution canvas grid points occupied by any "
           "given image component sample in each direction.  By "
           "default conformant JP2 readers are expected to assume a 1:1 "
           "aspect ratio on the high resolution canvas, so that the use of "
           "non-identical sub-sampling factors for an image component "
           "implies a required aspect ratio conversion after decompression.\n";
  out << "-rotate <degrees>\n";
  if (comprehensive)
    out << "\tRotate source image prior to compression. "
           "Must be multiple of 90 degrees.\n";
  siz_params siz; siz.describe_strings(out,comprehensive);
  cod_params cod; cod.describe_strings(out,comprehensive);
  qcd_params qcd; qcd.describe_strings(out,comprehensive);
  rgn_params rgn; rgn.describe_strings(out,comprehensive);
  poc_params poc; poc.describe_strings(out,comprehensive);
  crg_params crg; crg.describe_strings(out,comprehensive);
  out << "-cpu <coder-iterations>\n";
  if (comprehensive)
    out << "\tTimes end-to-end execution and, optionally, the block encoding "
           "operation, reporting throughput statistics.  If "
           "`coder-iterations' is 0, the block coder will not be timed, "
           "leading to the most accurate end-to-end system execution "
           "times.  Otherwise, `coder-iterations' must be a positive "
           "integer -- larger values will result in more accurate "
           "estimates of the block encoder processing time, but "
           "degrade the accuracy of end-to-end system execution time "
           "estimates.  Note that end-to-end times include the impact "
           "of image file reading, which can be considerable.\n";
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
    out << "\tRecord code-stream parameters in a file, using the same format "
           "which is accepted when specifying the parameters on the command "
           "line.\n";
  out << "-quiet -- suppress informative messages.\n";
  out << "-usage -- print a comprehensive usage statement.\n";
  out << "-u -- print a brief usage statement.\"\n\n";

  if (!comprehensive)
    {
      out.flush();
      exit(0);
    }

  strbuf.set_master_indent(0);
  out << "Notes:\n";
  strbuf.set_master_indent(3);
  out << "    Arguments which commence with an upper case letter (rather than "
         "a dash) are used to set up code-stream parameter attributes. "
         "These arguments have the general form:"
         "  <arg name>={fld1,fld2,...},{fld1,fld2,...},..., "
         "where curly braces enclose records and each record is composed of "
         "fields.  The type and acceptable values for the fields are "
         "identified in the usage statements, along with whether or not "
         "multiple records are allowed.  In the special case where only one "
         "field is defined per record, the curly braces may be omitted. "
         "In no event may any spaces appear inside an attribute argument.\n";
  out << "    Most of the code-stream parameter attributes take an optional "
         "tile-component modifier, consisting of a colon, followed by a "
         "tile specifier, a component specifier, or both.  The tile specifier "
         "consists of the letter `T', followed immediately be the tile index "
         "(tiles are numbered in raster order, starting from 0).  Similarly, "
         "the component specifier consists of the letter `C', followed "
         "immediately by the component index (starting from 0). These "
         "modifiers may be used to specify parameter changes in specific "
         "tiles, components, or tile-components.\n";
  out << "    If you do not remember the exact form or description of one of "
         "the code-stream attribute arguments, simply give the attribute name "
         "on the command-line and the program will exit with a detailed "
         "description of the attribute.\n";
  out << "    If SIZ parameters are to be supplied explicitly on the "
         "command line, be aware that these may be affected by simultaneous "
         "specification of geometric transformations.  If uncertain of the "
         "behaviour, use `-record' to determine the final compressed "
         "code-stream parameters which were used.\n";
  out << "    If you are compressing a 3 component image using the "
         "reversible or irreversible colour transform (this is the default), "
         "the program will automatically introduce a reasonable set of visual "
         "weighting factors, unless you use the \"Clev_weights\" or "
         "\"Cband_weights\" options yourself.  This does not happen "
         "automatically in the case of single component images, which are "
         "optimized purely for MSE by default.  To see whether weighting "
         "factors were used, you may like to use the `-record' option.\n";
  out.flush();
  exit(0);
}

/*****************************************************************************/
/* STATIC                     parse_simple_args                              */
/*****************************************************************************/

static kdc_file_binding *
  parse_simple_args(kdu_args &args, char * &ofname,
                    std::ostream * &record_stream,
                    bool &transpose, bool &vflip, bool &hflip,
                    bool &allow_rate_prediction, bool &allow_shorts,
                    bool &no_weights, bool &no_palette, int &cpu_iterations,
                    bool &mem, bool &quiet)
  /* Parses most simple arguments (those involving a dash). Most parameters are
     returned via the reference arguments, with the exception of the input
     file names, which are returned via a linked list of `kdc_file_binding'
     objects.  Only the `fname' field of each `kdc_file_binding' record is
     filled out here.  The value returned via `cpu_iterations' is negative
     unless CPU times are required. */

{
  int rotate;
  kdc_file_binding *files, *last_file, *new_file;

  if ((args.get_first() == NULL) || (args.find("-u") != NULL))
    print_usage(args.get_prog_name());
  if (args.find("-usage") != NULL)
    print_usage(args.get_prog_name(),true);

  files = last_file = NULL;
  ofname = NULL;
  record_stream = NULL;
  rotate = 0;
  allow_rate_prediction = true;
  allow_shorts = true;
  no_weights = false;
  no_palette = false;
  cpu_iterations = -1;
  mem = false;
  quiet = false;

  if (args.find("-i") != NULL)
    {
      char *string, *cp;
      int len;

      if ((string = args.advance()) == NULL)
        { kdu_error e; e << "\"-i\" argument requires a file name!"; }
      while ((len=strlen(string)) > 0)
        {
          cp = strchr(string,',');
          if (cp == NULL)
            cp = string+len;
          new_file = new kdc_file_binding(string,cp-string);
          if (last_file == NULL)
            files = last_file = new_file;
          else
            last_file = last_file->next = new_file;
          if (*cp == ',') cp++;
          string = cp;
        }
      args.advance();
    }

  if (args.find("-o") != NULL)
    {
      if ((ofname = args.advance()) == NULL)
        { kdu_error e; e << "\"-o\" argument requires a file name!"; }
      args.advance();
    }

  if (args.find("-full") != NULL)
    {
      args.advance();
      allow_rate_prediction = false;
    }

  if (args.find("-precise") != NULL)
    {
      args.advance();
      allow_shorts = false;
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

  if (args.find("-no_weights") != NULL)
    {
      no_weights = true;
      args.advance();
    }

  if (args.find("-no_palette") != NULL)
    {
      no_palette = true;
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

  if (files == NULL)
    { kdu_error e; e << "Must provide one or more input files!"; }
  if (ofname == NULL)
    { kdu_error e; e << "Must provide an output file name!"; }
  while (rotate >= 4)
    rotate -= 4;
  while (rotate < 0)
    rotate += 4;
  switch (rotate) {
    case 0: transpose = false; vflip = false; hflip = false; break;
    case 1: transpose = true; vflip = true; hflip = false; break;
    case 2: transpose = false; vflip = true; hflip = true; break;
    case 3: transpose = true; vflip = false; hflip = true; break;
    }

  return(files);
}

/*****************************************************************************/
/* STATIC                   set_jp2_coding_defaults                          */
/*****************************************************************************/

static void
  set_jp2_coding_defaults(jp2_target &out, kdu_params *siz)
{
  kdu_params *cod = siz->access_cluster(COD_params);
  assert(cod != NULL);
  int num_colours = out.access_colour().get_num_colours();
  bool using_palette = (out.access_palette().get_num_components() > 0);
  bool use_ycc, reversible;
  int dwt_levels;
  if (((num_colours < 3) ||
       (out.access_colour().get_space() == JP2_sYCC_SPACE) ||
       using_palette) &&
      !cod->get(Cycc,0,0,use_ycc))
    cod->set(Cycc,0,0,use_ycc=false);
  if (using_palette && !cod->get(Creversible,0,0,reversible))
    cod->set(Creversible,0,0,reversible=true);
  if (using_palette && !cod->get(Clevels,0,0,dwt_levels))
    cod->set(Clevels,0,0,dwt_levels=0);
}

/*****************************************************************************/
/* STATIC                  set_default_colour_weights                        */
/*****************************************************************************/

static void
  set_default_colour_weights(kdu_params *siz, bool quiet)
{
  kdu_params *cod = siz->access_cluster(COD_params);
  assert(cod != NULL);

  bool can_use_ycc = true;
  bool rev0=false;
  int depth0=0, sub_x0=1, sub_y0=1;
  for (int c=0; c < 3; c++)
    {
      int depth=0; siz->get(Sprecision,c,0,depth);
      int sub_y=1; siz->get(Ssampling,c,0,sub_y);
      int sub_x=1; siz->get(Ssampling,c,1,sub_x);
      kdu_params *coc = cod->access_relation(-1,c);
      bool rev=false; coc->get(Creversible,0,0,rev);
      if (c == 0)
        { rev0=rev; depth0=depth; sub_x0=sub_x; sub_y0=sub_y; }
      else if ((rev != rev0) || (depth != depth0) ||
        (sub_x != sub_x0) || (sub_y != sub_y0))
        can_use_ycc = false;
    }
  if (!can_use_ycc)
    return;

  bool use_ycc;
  if (!cod->get(Cycc,0,0,use_ycc))
    cod->set(Cycc,0,0,use_ycc=true);
  if (!use_ycc)
    return;
  float weight;
  if (cod->get(Clev_weights,0,0,weight) ||
      cod->get(Cband_weights,0,0,weight))
    return; // Weights already specified explicitly.

  /* These example weights are adapted from numbers generated by Marcus Nadenau
     at EPFL, for a viewing distance of 15 cm and a display resolution of
     300 DPI. */
  
  cod->parse_string("Cband_weights:C0="
                    "{0.0901},{0.2758},{0.2758},"
                    "{0.7018},{0.8378},{0.8378},{1}");
  cod->parse_string("Cband_weights:C1="
                    "{0.0263},{0.0863},{0.0863},"
                    "{0.1362},{0.2564},{0.2564},"
                    "{0.3346},{0.4691},{0.4691},"
                    "{0.5444},{0.6523},{0.6523},"
                    "{0.7078},{0.7797},{0.7797},{1}");
  cod->parse_string("Cband_weights:C2="
                    "{0.0773},{0.1835},{0.1835},"
                    "{0.2598},{0.4130},{0.4130},"
                    "{0.5040},{0.6464},{0.6464},"
                    "{0.7220},{0.8254},{0.8254},"
                    "{0.8769},{0.9424},{0.9424},{1}");
  if (!quiet)
    pretty_cout << "Note:\n\tThe default rate control policy for colour "
                   "images employs visual (CSF) weighting factors.  To "
                   "minimize MSE instead, specify `-no_weights'.\n";
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
/* STATIC                      check_jp2_suffix                              */
/*****************************************************************************/

static bool
  check_jp2_suffix(char *fname)
  /* Returns true if the file-name has the suffix, ".jp2", where the
     check is case insensitive. */
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
  if (*cp != '2')
    return false;
  return true;
}

/*****************************************************************************/
/* STATIC                      set_jp2_attributes                            */
/*****************************************************************************/

static void
  set_jp2_attributes(jp2_target &jp2_out, siz_params *siz,
                     kdu_rgb8_palette &palette, int num_components,
                     kdu_args &args)
{
  // Set dimensional information (all redundant with the SIZ marker segment)
  jp2_dimensions dims = jp2_out.access_dimensions();
  dims.init(siz);

  // Set resolution information (optional)
  if (args.find("-jp2_aspect") != NULL)
    {
      float aspect_ratio;
      char *string = args.advance();
      if ((string == NULL) ||
          (sscanf(string,"%f",&aspect_ratio) != 1) ||
          (aspect_ratio <= 0.0F))
        { kdu_error e; e << "Missing or illegal aspect ratio "
          "parameter supplied with the `-jp2_aspect' argument!"; }
      args.advance();
      jp2_resolution res = jp2_out.access_resolution();
      res.init(aspect_ratio);
    }

  // Set colour space information (mandatory)
  jp2_colour colour = jp2_out.access_colour();
  int num_colours = (num_components < 3)?1:3;
  if (palette.exists())
    num_colours = (palette.is_monochrome())?1:3;
  if (args.find("-jp2_space") != NULL)
    {
      char *string = args.advance();
      if (string == NULL)
        { kdu_error e; e << "The `-jp2_space' argument requires a parameter "
          "string!"; }
      char *delim = strchr(string,',');
      if (delim != NULL)
        *delim = '\0';
      if (strcmp(string,"sRGB") == 0)
        colour.init(JP2_sRGB_SPACE);
      else if (strcmp(string,"sYCC") == 0)
        colour.init(JP2_sYCC_SPACE);
      else if (strcmp(string,"sLUM") == 0)
        colour.init(JP2_sLUM_SPACE);
      else if (strcmp(string,"iccLUM") == 0)
        {
          float gamma, beta;
          string = delim+1;
          if ((delim == NULL) || ((delim = strchr(string,',')) == NULL) ||
              ((*delim = '\0') > 0) || (sscanf(string,"%f",&gamma) != 1) ||
              (sscanf(delim+1,"%f",&beta) != 1) || (gamma < 1.0F) ||
              (beta < 0.0F) || (beta >= 1.0F))
            { kdu_error e; e << "Missing or illegal gamma/beta parameters "
              "supplied in comma-separated parameter list which must follow "
              "the \"sLUM\" JP2 colour space specification supplied via the "
              "`-jp2_space' argument.  `gamma' must be no less than 1 and "
              "`beta' must be in the range 0 to 1."; }
          colour.init(gamma,beta);
        }
      else if (strcmp(string,"iccRGB") == 0)
        {
          float val, gamma, beta, matrix3x3[9];
          for (int p=0; p < 11; p++)
            {
              string = delim+1;
              if ((delim == NULL) ||
                  ((p < 10) && ((delim = strchr(string,',')) == NULL)) ||
                  ((*delim = '\0') > 0) || (sscanf(string,"%f",&val) != 1))
                { kdu_error e; e << "The \"iccRGB\" specification must be "
                  "followed immediately by a comma-separated list of 11 "
                  "parameters, all within the single parameter string "
                  "supplied with the `-jp2_space' argument.  For more details "
                  "review the usage statement."; }
              if (p == 0)
                gamma = val;
              else if (p == 1)
                beta = val;
              else
                matrix3x3[p-2] = val;
            }
          if ((gamma < 1.0F) || (beta < 0.0F) || (beta >= 1.0F))
            { kdu_error e; e << "Illegal gamma or beta value supplied in the "
              "\"iccRGB\" specification following the `-jp2_space' argument.  "
              "`gamma' must not be less than 1 and `beta' must  be in the "
              "range 0 to 1."; }
          colour.init(matrix3x3,gamma,beta);
        }
      else
        { kdu_error e; e << "Invalid parameter string following `-jp2_space' "
          "argument.  The string must identify the colour space as one of "
          "\"sLUM\", \"sRGB\", \"sYCC\", \"iccLUM\" or \"iccRGB\"."; }
      args.advance();
      if (colour.get_num_colours() > num_colours)
        { kdu_error e; e << "Colour space specified using `-jp2_space' "
          "requires more image components or a colour palette."; }
    }
  else
    colour.init((num_colours==3)?JP2_sRGB_SPACE:JP2_sLUM_SPACE);

  // Set the colour palette and channel mapping as needed.
  if (palette.exists())
    {
      jp2_palette pclr = jp2_out.access_palette();
      if (palette.is_monochrome())
        {
          pclr.init(1,1<<palette.input_bits);
          pclr.set_lut(0,palette.red,palette.output_bits);
          assert(num_colours == 1);
          jp2_channels channels = jp2_out.access_channels();
          channels.init(1);
          channels.set_colour_mapping(0,palette.source_component,0);
        }
      else
        {
          pclr.init(3,1<<palette.input_bits);
          pclr.set_lut(0,palette.red,palette.output_bits);
          pclr.set_lut(1,palette.green,palette.output_bits);
          pclr.set_lut(2,palette.blue,palette.output_bits);
          assert(num_colours == 3);
          jp2_channels channels = jp2_out.access_channels();
          channels.init(3);
          channels.set_colour_mapping(0,palette.source_component,0);
          channels.set_colour_mapping(1,palette.source_component,1);
          channels.set_colour_mapping(2,palette.source_component,2);
        }
    }
}

/*****************************************************************************/
/* STATIC                      assign_layer_bytes                            */
/*****************************************************************************/

static int *
  assign_layer_bytes(kdu_args &args, siz_params *siz, int &num_specs)
  /* Returns a pointer to an array of `num_specs' quality layer byte
     targets.  The value of `num_specs' is determined in this function, based
     on the number of rates (or slopes) specified on the command line,
     together with any knowledge about the number of desired quality layers.
     Before calling this function, you must parse all parameter attribute
     strings into the code-stream parameter lists rooted at `siz'.  Note that
     the returned array will contain 0's whenever a quality layer's
     bit-rate is unspecified.  This allows the compressor's rate allocator to
     assign the target size for those quality layers on the fly. */
{
  char *cp;
  char *string = NULL;
  int arg_specs = 0;
  int slope_specs = 0;
  int cod_specs = 0;

  if (args.find("-slope") != NULL)
    {
      string = args.advance(false); // Need to process this arg again later.
      if (string != NULL)
        {
          while (string != NULL)
            {
              slope_specs++;
              string = strchr(string+1,',');
            }
        }
    }

  // Determine how many rates are specified on the command-line
  if (args.find("-rate") != NULL)
    {
      string = args.advance();
      if (string == NULL)
        { kdu_error e; e << "\"-rate\" argument must be followed by a "
          "string identifying one or more bit-rates, separated by commas."; }
      cp = string;
      while (cp != NULL)
        {
          arg_specs++;
          cp = strchr(cp,',');
          if (cp != NULL)
            cp++;
        }
    }

  // Find the number of layers specified by the main COD marker

  kdu_params *cod = siz->access_cluster(COD_params);
  assert(cod != NULL);
  cod->get(Clayers,0,0,cod_specs);
  if (!cod_specs)
    cod_specs = (arg_specs>slope_specs)?arg_specs:slope_specs;
  num_specs = cod_specs;
  if (num_specs == 0)
    num_specs = 1;
  if ((arg_specs != num_specs) &&
      ((arg_specs > 2) || ((arg_specs == 2) && (num_specs == 1))))
    { kdu_error e; e << "The relationship between the number of bit-rates "
      "specified by the \"-rate\" argument and the number of quality layers "
      "explicitly specified via \"Clayers\" does not conform to the rules "
      "supplied in the description of the \"-rate\" argument.  Use \"-u\" "
      "to print the usage statement."; }
  cod->set(Clayers,0,0,num_specs);
  int n;
  int *result = new int[num_specs];
  for (n=0; n < num_specs; n++)
    result[n] = 0;

  int total_pels = get_bpp_dims(siz);
  bool have_dash = false;
  for (n=0; n < arg_specs; n++)
    {
      cp = strchr(string,',');
      if (cp != NULL)
        *cp = '\0'; // Temporarily terminate string.
      if (strcmp(string,"-") == 0)
        { have_dash = true; result[n] = INT_MAX; }
      else
        {
          double bpp;
          if ((!sscanf(string,"%lf",&bpp)) || (bpp <= 0.0))
            { kdu_error e; e << "Illegal sub-string encoutered in parameter "
              "string supplied to the \"-rate\" argument.  Rate parameters "
              "must be strictly positive real numbers, with multiple "
              "parameters separated by commas only.  Problem encountered at "
              "sub-string: \"" << string << "\"."; }
          result[n] = (int) floor(bpp * 0.125 * total_pels);
        }
      if (cp != NULL)
        { *cp = ','; string = cp+1; }
    }

  if (arg_specs)
    { // Bubble sort the supplied specs.
      bool done = false;
      while (!done)
        { // Use trivial bubble sort.
          done = true;
          for (int n=1; n < arg_specs; n++)
            if (result[n-1] > result[n])
              { // Swap misordered pair.
                int tmp=result[n];
                result[n]=result[n-1];
                result[n-1]=tmp;
                done = false;
              }
        }
    }

  if (arg_specs && (arg_specs != num_specs))
    { // Arrange for specified rates to identify max and/or min layer rates
      assert((arg_specs < num_specs) && (arg_specs <= 2));
      result[num_specs-1] = result[arg_specs-1];
      result[arg_specs-1] = 0;
    }

  if (have_dash)
    { // Convert final rate target of INT_MAX into 0 (forces rate allocator
      // to assign all remaining compressed bits to that layer.)
      assert(result[num_specs-1] == INT_MAX);
      result[num_specs-1] = 0;
    }

  if (string != NULL)
    args.advance();
  return result;
}

/*****************************************************************************/
/* STATIC                      assign_layer_thresholds                            */
/*****************************************************************************/

static kdu_uint16 *
  assign_layer_thresholds(kdu_args &args, int num_specs)
  /* Returns a pointer to an array of `num_specs' slope threshold values,
     all of which are set to 0 unless the command-line arguments contain
     an explicit request for particular distortion-length slope thresholds. */
{
  int n;
  kdu_uint16 *result = new kdu_uint16[num_specs];

  for (n=0; n < num_specs; n++)
    result[n] = 0;
  if (args.find("-slope") == NULL)
    return result;
  char *string = args.advance();
  if (string == NULL)
    { kdu_error  e; e << "The `-slope' argument must be followed by a "
      "comma-separated list of slope values."; }
  for (n=0; (n < num_specs) && (string != NULL); n++)
    {
      char *delim = strchr(string,',');
      if (delim != NULL)
        { *delim = '\0'; delim++; }
      int val;
      if ((sscanf(string,"%d",&val) != 1) || (val < 0) || (val > 65535))
        { kdu_error e; e << "The `-slope' argument must be followed by a "
          "comma-separated  list of integer distortion-length slope values, "
          "each of which must be in the range 0 to 65535, inclusive."; }
      result[n] = (kdu_uint16) val;
      string = delim;
    }

  // Now sort the entries into decreasing order.
  int k;
  if (n > 1)
    {
      bool done = false;
      while (!done)
        { // Use trivial bubble sort.
          done = true;
          for (k=1; k < n; k++)
            if (result[k-1] < result[k])
              { // Swap misordered pair.
                kdu_uint16 tmp=result[k];
                result[k]=result[k-1];
                result[k-1]=tmp;
                done = false;
              }
        }
    }
  
  // Fill in any remaining missing values.
  for (k=n; k < num_specs; k++)
    result[k] = result[n-1];
  args.advance();
  return result;
}

/*****************************************************************************/
/* STATIC                      create_roi_source                             */
/*****************************************************************************/

static kdu_roi_image *
  create_roi_source(kdu_codestream codestream, kdu_args &args)
{
  if (args.find("-roi") == NULL)
    return NULL;
  char *string = args.advance();
  kdu_roi_image *result = NULL;
  if ((string != NULL) && (*string == '{'))
    {
      double top, left, height, width;

      if ((sscanf(string,"{%lf,%lf},{%lf,%lf}",
                  &top,&left,&height,&width) != 4) ||
          ((top < 0.0) || (left < 0.0) || (height < 0.0) || (width < 0.0)))
        { kdu_error e; e << "The `-roi' argument requires a set of "
          "coordinates of the form, \"{<top>,<left>},{<height>,<width>}\", "
          "where all quantities must be real numbers in the range 0 to 1."; }
      kdu_dims region; codestream.get_dims(-1,region);
      region.pos.y += (int) floor(region.size.y * top);
      region.pos.x += (int) floor(region.size.x * left);
      region.size.y = (int) ceil(region.size.y * height);
      region.size.x = (int) ceil(region.size.x * width);
      result = new kdu_roi_rect(codestream,region);
    }
  else
    { // Must be file-name/threshold form.
      char *fname = string;
      float threshold;

      if ((fname == NULL) || ((string = strchr(fname,',')) == NULL) ||
          (sscanf(string+1,"%f",&threshold) == 0) ||
          (threshold < 0.0F) || (threshold >= 1.0F))
        { kdu_error e; e << "The `-roi' argument requires a single parameter "
          "string, which should either identify the four coordinates of a "
          "rectangular foreground region or else an image file and threshold "
          "value, separated by a comma.  The threshold may be no less than 0 "
          "and must be strictly less than 1."; }
      *string = '\0';
      result = new kdu_roi_graphics(codestream,fname,threshold);
    }
  args.advance();
  return result;
}

int main(int argc, char *argv[])
{
  kdu_customize_warnings(&std::cout);
  kdu_customize_errors(&std::cerr);
  kdu_args args(argc,argv,"-s");

  // Collect simple arguments.

  bool transpose, vflip, hflip;
  bool allow_rate_prediction, allow_shorts, mem, quiet, no_weights, no_palette;
  int cpu_iterations;
  char *ofname;
  std::ostream *record_stream;
  kdc_file_binding *inputs =
    parse_simple_args(args,ofname,record_stream,transpose,vflip,hflip,
                      allow_rate_prediction,allow_shorts,no_weights,
                      no_palette,cpu_iterations,mem,quiet);

  // Create appropriate output file.

  kdu_compressed_target *output = NULL;
  kdu_simple_file_target file_out;
  jp2_target jp2_out;
  if (check_jp2_suffix(ofname))
    {
      output = &jp2_out;
      jp2_out.open(ofname);
    }
  else
    {
      output = &file_out;
      file_out.open(ofname);
    }

  // Collects any parameters required for raw input.
  siz_params siz;
  char *string;
  for (string=args.get_first(); string != NULL; )
    string = args.advance(siz.parse_string(string));

  // Set up input image files.
  kdu_rgb8_palette palette; // To support palettized imagery.
  kdc_file_binding *iscan;
  int num_components;
  bool extra_flip=false;

  num_components = 0;
  for (iscan=inputs; iscan != NULL; iscan=iscan->next)
    {
      bool flip;

      iscan->first_comp_idx = num_components;
      iscan->reader = kdu_image_in(iscan->fname,&siz,num_components,flip,
                                   ((no_palette || !jp2_out)?NULL:(&palette)));
      iscan->num_components = num_components - iscan->first_comp_idx;
      if (iscan == inputs)
        extra_flip = flip;
      if (extra_flip != flip)
        { kdu_error e; e << "Cannot mix input file types which have different "
          "vertical ordering conventions (i.e., top-to-bottom and "
          "bottom-to-top)."; }
    }
  if (extra_flip)
    vflip = !vflip;
  siz.set(Scomponents,0,0,num_components);

  // Complete `siz' object.

  siz_params *siz_p = &siz; siz_p->finalize(); // Access func via v-table
  siz_params transformed_siz; // Use this one to construct code-stream.
  transformed_siz.copy_from(&siz,-1,-1,-1,0,0,transpose,false,false);
  if (jp2_out.exists())
    set_jp2_attributes(jp2_out,&transformed_siz,palette,num_components,args);

  // Construct the `kdu_codestream' object and parse all remaining arguments.

  kdu_codestream codestream;
  codestream.create(&transformed_siz,output);
 
  for (string=args.get_first(); string != NULL; )
    string = args.advance(codestream.access_siz()->parse_string(string));
  if (jp2_out.exists())
    set_jp2_coding_defaults(jp2_out,codestream.access_siz());
  int num_layer_specs = 0;
  int *layer_bytes =
    assign_layer_bytes(args,codestream.access_siz(),num_layer_specs);
  kdu_uint16 *layer_thresholds =
    assign_layer_thresholds(args,num_layer_specs);
  if ((num_layer_specs > 0) && allow_rate_prediction &&
      (layer_bytes[num_layer_specs-1] > 0))
    codestream.set_max_bytes(layer_bytes[num_layer_specs-1]);
  if ((num_layer_specs > 0) && allow_rate_prediction &&
      (layer_thresholds[num_layer_specs-1] > 0))
    codestream.set_min_slope_threshold(layer_thresholds[num_layer_specs-1]);
  if ((num_layer_specs < 2) && !quiet)
    pretty_cout << "Note:\n\tIf you want quality scalability, you should "
                   "generate multiple layers with `-rate' or by using "
                   "the \"Clayers\" option.\n";
  if ((num_components == 3) && (!no_weights))
    set_default_colour_weights(codestream.access_siz(),quiet);

  codestream.access_siz()->finalize_all();
  codestream.set_textualization(record_stream);
  if (cpu_iterations >= 0)
    codestream.collect_timing_stats(cpu_iterations);
  codestream.change_appearance(transpose,vflip,hflip);
  kdu_roi_image *roi_source = create_roi_source(codestream,args);
  if (args.show_unrecognized(pretty_cout) != 0)
    { kdu_error e; e << "There were unrecognized command line arguments!"; }

  // Now we are ready for sample data processing.

  int x_tnum;
  kdu_dims tile_indices; codestream.get_valid_tiles(tile_indices);
  kdc_flow_control **tile_flows = new kdc_flow_control *[tile_indices.size.x];
  for (x_tnum=0; x_tnum < tile_indices.size.x; x_tnum++)
    tile_flows[x_tnum] =
      new kdc_flow_control(inputs,codestream,x_tnum,allow_shorts,roi_source);
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

  // Produce the compressed output.

  codestream.flush(layer_bytes,num_layer_specs,layer_thresholds);

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
          pretty_cout << "Block encoding CPU time (estimated) ";
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
      double bpp_dims = get_bpp_dims(codestream.access_siz());
      pretty_cout << "\nGenerated " << codestream.get_num_tparts()
                  << " tile-part(s) for a total of "
                  << tile_indices.area() << " tile(s).\n";
      pretty_cout << "Code-stream bytes (excluding any file "
                     "format) = " << codestream.get_total_bytes()
                  << " = "
                  << 8.0*codestream.get_total_bytes() / bpp_dims
                  << " bits/pel.\n";

      int layer_idx;
      pretty_cout << "Layer bit-rates (possibly inexact if tiles are "
        "divided across tile-parts):\n\t\t";
      for (layer_idx=0; layer_idx < num_layer_specs; layer_idx++)
        {
          if (layer_idx > 0)
            pretty_cout << ", ";
          pretty_cout << 8.0*layer_bytes[layer_idx]/bpp_dims;
        }
      pretty_cout << "\n";
      pretty_cout << "Layer thresholds:\n\t\t";
      for (layer_idx=0; layer_idx < num_layer_specs; layer_idx++)
        {
          if (layer_idx > 0)
            pretty_cout << ", ";
          pretty_cout << (int)(layer_thresholds[layer_idx]);
        }
      pretty_cout << "\n";
    }
  delete[] layer_bytes;
  delete[] layer_thresholds;
  codestream.destroy();
  output->close();
  if (roi_source != NULL)
    delete roi_source;
  if (record_stream != NULL)
    delete record_stream;
  delete inputs;
  return 0;
}

