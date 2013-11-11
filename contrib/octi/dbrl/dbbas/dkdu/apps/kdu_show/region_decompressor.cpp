/******************************************************************************/
// File: region_decompressor.cpp [scope = APPS/SHOW]
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
   Implements the incremental, region-based decompression services of the
"kd_region_decompressor" object.  These services should prove useful
to many interactive applications which require JPEG2000 rendering capabilities.
*******************************************************************************/

#include <assert.h>
#include "kdu_utils.h"
#include "kdu_compressed.h"
#include "kdu_sample_processing.h"
#include "region_decompressor.h"


/* ========================================================================== */
/*                            Internal Functions                              */
/* ========================================================================== */

/******************************************************************************/
/* STATIC                          reset_line                                 */
/******************************************************************************/

void                    
  reset_line_buf(kdu_line_buf &buf)
{
  int num_samples = buf.get_width();
  if (buf.get_buf32() != NULL)
    {
      assert(buf.is_absolute());
      kdu_sample32 *sp = buf.get_buf32();
      while (num_samples--)
        (sp++)->ival = 0;
    }
  else
    {
      kdu_sample16 *sp = buf.get_buf16();
      while (num_samples--)
        (sp++)->ival = 0;
    }
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

/******************************************************************************/
/* STATIC                     interpolate_and_map                             */
/******************************************************************************/

static void
  interpolate_and_map(kdu_line_buf &src, int interp_first, int interp_factor,
                      kdu_sample16 *lut, kdu_line_buf &dst,
                      int skip_cols, int num_cols)
  /* This function maps the elements of the `src' buffer through the supplied
     lookup table (the `src' samples are indices to the `lut' array) to
     produce 16-bit fixed point output samples, stored in the `dst' buffer.
     At the same time, the function interpolates the sources samples, using
     a simple zero-order hold (nearest neighbour) policy.  The interpolation
     factor is given by `interp_factor'.  In this way, each input sample
     nominally produces `interp_factor' output samples.  The first input
     sample produces only `interp_first' output samples.  This allows for
     the fact that the interpolation might start from any point.  It can happen
     that `interp_first' is larger than `interp_facto', in which case some
     extrapolation is involved at the left boundary.  Similarly, some
     extrapolation may be required at the right boundary if there are
     insufficient source samples.  If there are no source samples at all,
     they are extrapolated as 0, although this should not happen in a well
     constructed code-stream (it may happen if a tiled image is cropped
     down -- another reason to avoid tiling).
        Notionally, the line is interpolated as described above and then
     the first `skip_cols' samples are discarded before writing the result
     into the `dst' buffer.  The number of output samples to be written to
     `dst' is given by `num_cols'. */
{
  kdu_sample16 val, *dp = dst.get_buf16();
  assert((dp != NULL) && !dst.is_absolute());
  int in_cols = src.get_width();
  if (in_cols == 0)
    { // No data available.  Synthesize 0's.
      for (; num_cols > 0; num_cols--)
        *(dp++) = lut[0];
      return;
    }

  // Determine the source offset and the number of outputs from the first input
  interp_first -= skip_cols;
  int src_offset = 0;
  for (; interp_first <= 0; interp_first += interp_factor)
    { src_offset++; in_cols--; }
  if (in_cols <= 0)
    { src_offset += (in_cols-1); in_cols = 1; }
  if (interp_first > num_cols)
    interp_first = num_cols;
  num_cols -= interp_first; // Number of samples left to be interpolated

  // Determine the number of input samples which expand fully to exactly
  // `interp_factor' output samples.
  int fast_count = num_cols / interp_factor;
  if (fast_count > (in_cols-2))
    fast_count = in_cols-2;

  // Convert `num_cols' to mean the number of remaining output samples which
  // should all be set equal to the last input sample.
  if (fast_count > 0)
    num_cols -= fast_count * interp_factor;

  // Now for the processing
  int k;
  if (src.get_buf32() != NULL)
    { // Indices are 32-bit integers
      kdu_sample32 *sp = src.get_buf32() + src_offset;
      for (val=lut[(sp++)->ival]; interp_first > 0; interp_first--)
        *(dp++) = val;
      if (interp_factor == 1)
        for (; fast_count > 0; fast_count--)
          *(dp++) = val = lut[(sp++)->ival];
      else
        for (; fast_count > 0; fast_count--)
          for (val=lut[(sp++)->ival], k=interp_factor; k > 0; k--)
            *(dp++) = val;
      if (fast_count == 0)
        val = lut[sp->ival]; // Otherwise, we had only one input sample.
      for (; num_cols > 0; num_cols--)
        *(dp++) = val;
    }
  else
    { // Indices are 16-bit integers
      kdu_sample16 *sp = src.get_buf16() + src_offset;
      for (val=lut[(sp++)->ival]; interp_first > 0; interp_first--)
        *(dp++) = val;
      if (interp_factor == 1)
        for (; fast_count > 0; fast_count--)
          *(dp++) = val = lut[(sp++)->ival];
      else
        for (; fast_count > 0; fast_count--)
          for (val=lut[(sp++)->ival], k=interp_factor; k > 0; k--)
            *(dp++) = val;
      if (fast_count == 0)
        val = lut[sp->ival]; // Otherwise, we had only one input sample.
      for (; num_cols > 0; num_cols--)
        *(dp++) = val;
    }
}

/******************************************************************************/
/* STATIC                   interpolate_and_convert                           */
/******************************************************************************/

static void
  interpolate_and_convert(kdu_line_buf &src, int interp_first,
                          int interp_factor, int bit_depth, kdu_line_buf &dst,
                          int skip_cols, int num_cols)
  /* Same as `interpolate_and_map' except that there is no lookup table and
     the source values may need to be converted to the 16-bit fixed point
     output data type. */
{
  kdu_sample16 *dp = dst.get_buf16();
  assert((dp != NULL) && !dst.is_absolute());
  int in_cols = src.get_width();
  if (in_cols == 0)
    { // No data available.  Synthesize 0's.
      for (; num_cols > 0; num_cols--)
        (dp++)->ival = 0;
      return;
    }

  // Determine the source offset and the number of outputs from the first input
  interp_first -= skip_cols;
  int src_offset = 0;
  for (; interp_first <= 0; interp_first += interp_factor)
    { src_offset++; in_cols--; }
  if (in_cols <= 0)
    { src_offset += (in_cols-1); in_cols = 1; }
  if (interp_first > num_cols)
    interp_first = num_cols;
  num_cols -= interp_first; // Number of samples left to be interpolated

  // Determine the number of input samples which expand fully to exactly
  // `interp_factor' output samples.
  int fast_count = num_cols / interp_factor;
  if (fast_count > (in_cols-2))
    fast_count = in_cols-2;

  // Convert `num_cols' to mean the number of remaining output samples which
  // should all be set equal to the last input sample.
  if (fast_count > 0)
    num_cols -= fast_count * interp_factor;

  // Now for the processing
  int k;
  if (!src.is_absolute())
    { // Source already contains 16-bit fixed point numbers
      kdu_sample16 val, *sp = src.get_buf16(); assert(sp != NULL);
      sp += src_offset;
      for (val=*(sp++); interp_first > 0; interp_first--)
        *(dp++) = val;
      if (interp_factor == 1)
        for (; fast_count > 0; fast_count--)
          *(dp++) = val = *(sp++);
      else
        for (; fast_count > 0; fast_count--)
          for (val=*(sp++), k=interp_factor; k > 0; k--)
            *(dp++) = val;
      if (fast_count == 0)
        val = *sp; // Otherwise, we had only one input sample.
      for (; num_cols > 0; num_cols--)
        *(dp++) = val;
    }
  else
    { // Source data is absolute integers.  Needs to be shifted.
      int upshift = KDU_FIX_POINT - bit_depth;
      int downshift = -upshift;
      if (src.get_buf32() != NULL)
        { // Source data is 32-bit integers
          kdu_sample32 *sp = src.get_buf32() + src_offset;
          kdu_int32 val;
          val = (sp++)->ival; val = (upshift>0)?(val<<upshift):(val>>downshift);
          for (; interp_first > 0; interp_first--)
            (dp++)->ival = (kdu_int16) val;
          if (upshift > 0)
            {
              if (interp_factor == 1)
                for (; fast_count > 0; fast_count--)
                  (dp++)->ival = (kdu_int16)(val = ((sp++)->ival << upshift));
              else
                for (; fast_count > 0; fast_count--)
                  for (val=(sp++)->ival<<upshift, k=interp_factor; k > 0; k--)
                    (dp++)->ival = (kdu_int16) val;
            }
          else
            {
              if (interp_factor == 1)
                for (; fast_count > 0; fast_count--)
                  (dp++)->ival = (kdu_int16)(val = ((sp++)->ival >> downshift));
              else
                for (; fast_count > 0; fast_count--)
                  for (val=(sp++)->ival>>downshift, k=interp_factor; k > 0; k--)
                    (dp++)->ival = (kdu_int16) val;
            }
          if (fast_count == 0)
            { // Otherwise, we had only one input sample.
              val = sp->ival;
              val = (upshift>0)?(val<<upshift):(val>>downshift);
            }
          for (; num_cols > 0; num_cols--)
            (dp++)->ival = (kdu_int16) val;
        }
      else
        { // Source data is 16-bit absolute integers
          kdu_sample16 *sp = src.get_buf16() + src_offset;
          kdu_int16 val;
          val = (sp++)->ival; val = (upshift>0)?(val<<upshift):(val>>downshift);
          for (; interp_first > 0; interp_first--)
            (dp++)->ival = val;
          if (upshift > 0)
            {
              if (interp_factor == 1)
                for (; fast_count > 0; fast_count--)
                  (dp++)->ival = val = ((sp++)->ival << upshift);
              else
                for (; fast_count > 0; fast_count--)
                  for (val=(sp++)->ival<<upshift, k=interp_factor; k > 0; k--)
                    (dp++)->ival = val;
            }
          else
            {
              if (interp_factor == 1)
                for (; fast_count > 0; fast_count--)
                  (dp++)->ival = val = ((sp++)->ival >> downshift);
              else
                for (; fast_count > 0; fast_count--)
                  for (val=(sp++)->ival>>downshift, k=interp_factor; k > 0; k--)
                    (dp++)->ival = val;
            }
          if (fast_count == 0)
            { // Otherwise, we had only one input sample.
              val = sp->ival;
              val = (upshift>0)?(val<<upshift):(val>>downshift);
            }
          for (; num_cols > 0; num_cols--)
            (dp++)->ival = val;
        }
    }
}

/******************************************************************************/
/* STATIC                     transfer_fixed_point                            */
/******************************************************************************/

static void
  transfer_fixed_point(kdu_line_buf &src, kdu_byte *dest, int num_samples)
  /* Transfers source samples from the supplied line buffer into the output
     byte buffer, spacing successive output samples apart by 3 bytes (to allow
     for interleaving of colour components).  Only `num_samples' samples are
     transferred and the source data is guaranteed to have a 16-bit fixed
     point representation. */
{
  assert(num_samples <= src.get_width());
  kdu_sample16 *sp = src.get_buf16();
  assert((sp != NULL) && !src.is_absolute());
  kdu_int16 val;

  for (; num_samples > 0; num_samples--, sp++, dest+=3)
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


/* ========================================================================== */
/*                    Implementation of Member Functions                      */
/* ========================================================================== */

/******************************************************************************/
/*                      kdr_region_decompressor::start                        */
/******************************************************************************/

void
  kdr_region_decompressor::start(kdu_codestream codestream,
                                 kdr_channel_mapping *mapping,
                                 int single_component, int discard_levels,
                                 int max_layers, kdu_dims region,
                                 kdu_coords sampling)
{
  int c, num_components = codestream.get_num_components();
  this->codestream = codestream;
  codestream_failure = false;
  tile_open = false; // Just in case.

  // Set up components and channels.
  for (c=0; c < 6; c++)
    {
      components[c].comp_idx = -1;
      components[c].palette_bits = 0;
    }
  if (mapping != NULL)
    {
      num_channels = mapping->num_channels;
      assert((num_channels == 1) || (num_channels == 3));
      for (c=0; c < num_channels; c++)
        {
          int n;
          for (n=0; n < 5; n++)
            if ((components[n].comp_idx < 0) ||
                (components[n].comp_idx == mapping->source_components[c]))
              break;
          components[n].comp_idx = mapping->source_components[c];
          channels[c].source = components + n;
          channels[c].lut = mapping->palette[c];
          if (channels[c].lut != NULL)
            components[n].palette_bits = mapping->palette_bits;
        }
      colour = mapping->colour;
      space = (num_channels==3)?JP2_sRGB_SPACE:JP2_sLUM_SPACE;
      if (colour.exists())
        space = colour.get_space();
    }
  else
    {
      num_channels = 1;
      components[0].comp_idx = single_component;
      channels[0].source = components;
      channels[0].lut = NULL;
      colour = jp2_colour(NULL);
      space = JP2_sLUM_SPACE;
    }

  // Configure component sampling and data representation information.
  int first_idx = channels[0].source->comp_idx;
  kdu_coords first_subs;
  codestream.get_subsampling(first_idx,first_subs);
  kdu_coords max_sampling = sampling;
  for (c=0; (c < 6) && (components[c].comp_idx >= 0); c++)
    {
      kdr_component *comp = components + c;
      comp->bit_depth = codestream.get_bit_depth(comp->comp_idx);
      comp->is_signed = codestream.get_signed(comp->comp_idx);
      kdu_coords this_subs;
      codestream.get_subsampling(comp->comp_idx,this_subs);
      comp->sampling.x = (this_subs.x * sampling.x) / first_subs.x;
      comp->sampling.y = (this_subs.y * sampling.y) / first_subs.y;
      if (comp->sampling.x > max_sampling.x)
        max_sampling.x = comp->sampling.x;
      if (comp->sampling.y > max_sampling.y)
        max_sampling.y = comp->sampling.y;
    }

  // Find an appropriate region (a little large) on the code-stream canvas
  kdu_coords min = region.pos;
  kdu_coords lim = min + region.size;
  min.x = ceil_ratio(min.x-2*max_sampling.x,sampling.x);
  min.y = ceil_ratio(min.y-2*max_sampling.y,sampling.y);
  lim.x = ceil_ratio(lim.x+2*max_sampling.x,sampling.x);
  lim.y = ceil_ratio(lim.y+2*max_sampling.y,sampling.y);
  kdu_dims first_region; first_region.pos = min; first_region.size = lim-min;
  
  codestream.apply_input_restrictions(0,0,discard_levels,max_layers,NULL);
  kdu_dims canvas_region;
  codestream.map_region(first_idx,first_region,canvas_region);
  codestream.apply_input_restrictions(0,0,discard_levels,max_layers,
                                      &canvas_region);

  // Prepare for processing tiles within the region.
  codestream.get_valid_tiles(valid_tiles);
  next_tile_idx = valid_tiles.pos;
  tile_open = false;
}

/******************************************************************************/
/*                     kdr_region_decompressor::open_tile                     */
/******************************************************************************/

void
  kdr_region_decompressor::open_tile()
{
  int c;

  assert(!tile_open);
  assert((next_tile_idx.y - valid_tiles.pos.y) < valid_tiles.size.y);
  current_tile = codestream.open_tile(next_tile_idx);
  tile_open = true;
  next_tile_idx.x++;
  if ((next_tile_idx.x-valid_tiles.pos.x) >= valid_tiles.size.x)
    {
      next_tile_idx.x = valid_tiles.pos.x;
      next_tile_idx.y++;
    }

  // Fill in tile-specific component fields.
  use_ycc = current_tile.get_ycc();
  for (c=0; (c < 6) && (components[c].comp_idx >= 0); c++)
    {
      kdr_component *comp = components + c;
      if (use_ycc && (comp->comp_idx < 3) && comp->palette_bits)
        { kdu_error e; e << "It is illegal for a JP2 file to identify a "
          "code-stream component as index to a Palette lookup table, if the "
          "component is also part of a code-stream colour transform (RCT "
          "or ICT)."; }
      comp->line_buf_valid = false;
      kdu_tile_comp tc = current_tile.access_component(comp->comp_idx);
      comp->reversible = tc.get_reversible();
      kdu_resolution res = tc.access_resolution();
      res.get_dims(comp->dims);
      bool use_shorts = true;
      if (comp->reversible && (tc.get_bit_depth(true) > 16))
        use_shorts = false;
      if (res.which() == 0)
        comp->engine =
          kdu_decoder(res.access_subband(LL_BAND),&allocator,use_shorts);
      else
        comp->engine = kdu_synthesis(res,&allocator,use_shorts);
      comp->line.pre_create(&allocator,comp->dims.size.x,
                            comp->reversible,use_shorts);
    }
  if (num_channels == 1)
    use_ycc = false;

  // Establish rendering dimensions for the current tile based on the first
  // channel.  The mapping between rendering dimensions and actual
  // code-stream dimensions is invariably based upon the first channel as
  // the reference component.  This channel's sample locations are understood
  // as occupying locations at integer multiples of the relevant sampling
  // factors on the rendering canvas.
  kdr_component *ref_comp = channels[0].source;
  kdu_coords min = ref_comp->dims.pos;
  kdu_coords lim = min + ref_comp->dims.size;
  min.x = min.x * ref_comp->sampling.x - ((ref_comp->sampling.x-1) >> 1);
  min.y = min.y * ref_comp->sampling.y - ((ref_comp->sampling.y-1) >> 1);
  lim.x = lim.x * ref_comp->sampling.x - ((ref_comp->sampling.x-1) >> 1);
  lim.y = lim.y * ref_comp->sampling.y - ((ref_comp->sampling.y-1) >> 1);
  render_dims.pos = min;
  render_dims.size = lim - min;
  kdu_coords tmp_sampling = ref_comp->sampling;
  tmp_sampling.x<<=1; tmp_sampling.y<<=1; // Measure offsets in half pels
  kdu_coords ref_offset;
  codestream.get_registration(ref_comp->comp_idx,tmp_sampling,ref_offset);
  ref_offset.x -= 1-(ref_comp->sampling.x&1); // Even sub-sampling factors yield
  ref_offset.y -= 1-(ref_comp->sampling.y&1); // sample positions effectively
                               // displaced to the right (or down) by half a pel
  for (c=0; (c < 6) && (components[c].comp_idx >= 0); c++)
    {
      kdr_component *comp = components + c;
      tmp_sampling = comp->sampling;
      tmp_sampling.x<<=1; tmp_sampling.y<<=1; // Measure offsets in half pels
      kdu_coords offset;
      codestream.get_registration(comp->comp_idx,tmp_sampling,offset);
      offset -= ref_offset; // Measure offsets relative to reference component
      comp->interp.x = comp->dims.pos.x * comp->sampling.x - render_dims.pos.x
        + 1 + ((comp->sampling.x+offset.x)>>1);
      comp->interp.y = comp->dims.pos.y * comp->sampling.y - render_dims.pos.y
        + 1 + ((comp->sampling.y+offset.y)>>1);
      if ((c < 3) && use_ycc &&
          ((comp->interp != components[0].interp) ||
           (comp->sampling != components[0].sampling)))
        { kdu_error e; e << "The code-stream incorrectly identifies the use "
          "of a colour transform when the components have different "
          "sub-sampling or alignment properties."; }
    }

  // Pre-create channel line buffers.
  for (c=0; c < num_channels; c++)
    channels[c].line.pre_create(&allocator,render_dims.size.x,false,true);

  // Perform final resource allocation
  allocator.finalize();
  for (c=0; (c < 6) && (components[c].comp_idx >= 0); c++)
    components[c].line.create();
  for (c=0; c < num_channels; c++)
    channels[c].line.create();
}

/******************************************************************************/
/*                    kdr_region_decompressor::close_tile                     */
/******************************************************************************/

void
  kdr_region_decompressor::close_tile()
{
  int c;

  assert(tile_open);
  tile_open = false;
  current_tile.close();
  for (c=0; c < 6; c++)
    {
      kdr_component *comp = components + c;
      if (comp->engine.exists())
        comp->engine.destroy();
      comp->line.destroy(); // Works even if it was never pre-created
    }
  for (c=0; c < num_channels; c++)
    channels[c].line.destroy();
  allocator.restart(); // Get ready to use the allocator again in the next tile
}

/******************************************************************************/
/*                       kdr_region_decompressor::finish                      */
/******************************************************************************/

bool
  kdr_region_decompressor::finish()
{
  bool success;
  int c;

  success = !codestream_failure;
  if (success && tile_open)
    {
      try {
        current_tile.close();
        }
      catch (int) // `kdu_error' can throw this exception from deep inside core
        {
          success = false;
        }
    }
  tile_open = false;
  codestream_failure = false;

  // Cleanup any left over resources.
  for (c=0; c < 6; c++)
    {
      kdr_component *comp = components + c;
      if (comp->engine.exists())
        comp->engine.destroy();
      comp->line.destroy();
    }
  for (c=0; c < num_channels; c++)
    {
      channels[c].line.destroy();
      channels[c].lut = NULL;
    }
  codestream = kdu_codestream(); // Invalidate the internal pointer for safety
  allocator.restart(); // Get ready to use the allocation object again.
  return success;
}

/******************************************************************************/
/*                       kdr_region_decompressor::process                     */
/******************************************************************************/

bool
  kdr_region_decompressor::process(kdu_byte *buffer, int buffer_row_gap,
                                   kdu_dims buffer_region,
                                   kdu_dims &incomplete_region,
                                   int suggested_increment,
                                   kdu_dims &new_region)
{
  new_region.size = kdu_coords(0,0); // In case we decompress nothing
  if (codestream_failure || !incomplete_region)
    return false;
  assert(incomplete_region == (incomplete_region & buffer_region));
  try { // Protect, in case a fatal error is generated by the decompressor
      if (!tile_open)
        open_tile();
      kdu_dims incomplete_tile_region = render_dims & incomplete_region;
      if (!incomplete_tile_region)
        { // No intersection between tile and incomplete region.
          if (next_tile_idx.x == valid_tiles.pos.x)
            { // This is the last tile on the current row of tiles.
              int y = render_dims.pos.y + render_dims.size.y;
              if (y > incomplete_region.pos.y)
                { // Incomplete region must have shrunk in the meantime.
                  y -= incomplete_region.pos.y;
                  if (y > incomplete_region.size.y)
                    y = incomplete_region.size.y;
                  incomplete_region.pos.y += y;
                  incomplete_region.size.y -= y;
                }
            }
          close_tile();
          return true; // Let the caller get back to us for more tile processing
        }

      bool last_tile_in_row = // Advance incomplete region only when this true
        ((incomplete_tile_region.pos.x+incomplete_tile_region.size.x) ==
         (incomplete_region.pos.x+incomplete_region.size.x));

      // Determine an appropriate number of lines to process before returning.
      // Note that some or all of these lines might not intersect with the
      // incomplete region.
      assert(suggested_increment > 0);
      suggested_increment *= channels[0].source->sampling.x;
      suggested_increment *= channels[0].source->sampling.y;
      int new_lines = ceil_ratio(suggested_increment,render_dims.size.x);
      int y_lim = incomplete_tile_region.pos.y + incomplete_tile_region.size.y;
      if ((new_lines+render_dims.pos.y) > y_lim)
        new_lines = y_lim - render_dims.pos.y;
      assert(new_lines > 0);

      // Determine and process new region.
      new_region = incomplete_tile_region;
      new_region.size.y = 0;
      buffer += 
        ((incomplete_tile_region.pos.y-buffer_region.pos.y)*buffer_row_gap +
         (incomplete_tile_region.pos.x-buffer_region.pos.x))*3;
      for (; new_lines > 0; new_lines--,
           render_dims.pos.y++, render_dims.size.y--)
        {
          int c;

          // Decompress new image component lines as necessary.
          for (c=0; (c < 6) && (components[c].comp_idx >= 0); c++)
            {
              kdr_component *comp = components + c;
              if (comp->dims.size.y <= 0)
                { // No more lines available.
                  if (!comp->line_buf_valid)    // Tile has no lines of this
                    reset_line_buf(comp->line); // component at all.  Set to 0's
                  continue;
                }
              do {
                  if (comp->interp.y <= 0)
                    {
                      comp->interp.y += comp->sampling.y;
                      comp->line_buf_valid = false;
                    }
                  if (!comp->line_buf_valid)
                    {
                      comp->engine.pull(comp->line,true);
                      comp->dims.size.y--;
                      comp->dims.pos.y++;
                    }
                } while ((comp->interp.y <= 0) && (comp->dims.size.y > 0));
            }

          // Finish processing of component lines which have been invalidated
          if (use_ycc && !components[0].line_buf_valid)
            {
              assert((!components[1].line_buf_valid) &&
                     (!components[2].line_buf_valid));
              kdu_convert_ycc_to_rgb(components[0].line,components[1].line,
                                     components[2].line);
            }
          for (c=0; (c < 6) && (components[c].comp_idx >= 0); c++)
            {
              kdr_component *comp = components + c;
              if (!comp->line_buf_valid)
                {
                  comp->line_buf_valid = true;
                  if (comp->palette_bits > 0)
                    convert_samples_to_palette_indices(comp->line,
                         comp->bit_depth,comp->is_signed,comp->palette_bits);
                }
              comp->interp.y--; // Decrement interpolation counter.
            }

          // Now process a new set of channel lines, as required

          if (render_dims.pos.y == incomplete_tile_region.pos.y)
            { // This line has non-empty intersection with the incomplete region
              int skip_cols = incomplete_tile_region.pos.x-render_dims.pos.x;
              int num_cols = incomplete_tile_region.size.x;
              for (c=0; c < num_channels; c++)
                {
                  kdr_channel *channel = channels + c;
                  kdr_component *comp = channel->source;
                  if (channel->lut != NULL)
                    interpolate_and_map(comp->line,comp->interp.x,
                                    comp->sampling.x,channel->lut,
                                    channel->line,skip_cols,num_cols);
                  else
                    interpolate_and_convert(comp->line,comp->interp.x,
                                    comp->sampling.x,comp->bit_depth,
                                    channel->line,skip_cols,num_cols);
                }
              if (space == JP2_sYCC_SPACE)
                kdu_convert_ycc_to_rgb(channels[0].line,channels[1].line,
                                       channels[2].line,num_cols);
              else if (space == JP2_iccLUM_SPACE)
                colour.convert_icc_to_slum(channels[0].line,num_cols);
              else if (space == JP2_iccRGB_SPACE)
                colour.convert_icc_to_srgb(channels[0].line,channels[1].line,
                                           channels[2].line,num_cols);
              for (c=0; c < num_channels; c++)
                transfer_fixed_point(channels[c].line,buffer+2-c,num_cols);
              if (num_channels == 1)
                { // Above code only transferred the red channel.
                  kdu_byte *bp = buffer;
                  for (c=num_cols; c > 0; c--, bp+=3)
                    bp[0] = bp[1] = bp[2];
                }
              buffer += buffer_row_gap*3;
              incomplete_tile_region.pos.y++;
              incomplete_tile_region.size.y--;
              new_region.size.y++; // Transferred data region grows by one row.
              if (last_tile_in_row)
                {
                  assert(render_dims.pos.y == incomplete_region.pos.y);
                  incomplete_region.pos.y++;
                  incomplete_region.size.y--;
                }
            }
        }
      if (!incomplete_tile_region)
        { // Done all the processing we need for this tile.
          close_tile();
          return true;
        }
    }
  catch (int)
    {
      codestream_failure = true;
      return false;
    }
  return true;
}
