/*****************************************************************************/
// File: kdu_expand.cpp [scope = APPS/DECOMPRESSOR]
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
#include "expand_local.h"

/* ========================================================================= */
/*                      Pretty Printing Version of `cout'                    */
/* ========================================================================= */



/* ========================================================================= */
/*                              kde_flow_control                             */
/* ========================================================================= */

/*****************************************************************************/
/*                     kde_flow_control::kde_flow_control                    */
/*****************************************************************************/

kde_flow_control::kde_flow_control(kde_file_binding *files, int num_channels,
                                   kdu_codestream codestream, int x_tnum,
                                   bool allow_shorts,
                                   jp2_channels channel_mapping,
                                   jp2_palette palette,vil_image_io* writer)
{
  int c;

  this->codestream = codestream;
  codestream.get_valid_tiles(this->valid_tile_indices);
  assert((x_tnum >= 0) && (x_tnum < valid_tile_indices.size.x));
  this->tile_idx = valid_tile_indices.pos;
  this->tile_idx.x += x_tnum;
  this->x_tnum = x_tnum;
  this->tile = codestream.open_tile(tile_idx);
  this->num_components = codestream.get_num_components();
  this->num_channels = num_channels;
  components = new kde_component_flow_control[num_components];
  channels = new kde_channel[num_channels];
  count_delta = 0;

  // Initialize components
  for (c=0; c < num_components; c++)
    {
      kde_component_flow_control *comp = components + c;
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
      kde_channel *chnl = channels +c;
      if (files != NULL)
        {
          assert(c >= files->first_channel_idx);
          if ((c-files->first_channel_idx) >= files->num_channels)
            {
              files = files->next;
              assert((files != NULL) && (files->first_channel_idx == c));
            }
          chnl->writer = files->writer;
        }else if(writer){
			chnl->vil_writer=writer;

	  }

      int cmp=c, plt_cmp=-1;
      if (channel_mapping.exists())
        channel_mapping.get_colour_mapping(c,cmp,plt_cmp);
      chnl->source_component = components + cmp;
      chnl->source_component->mapped_by_channel = true;
      chnl->width = chnl->source_component->width;
      chnl->allocator = &(chnl->source_component->allocator);
       if (plt_cmp < 0)
        chnl->lut = NULL;
      else
        { // Set up palette lookup table.
          int i, num_entries = palette.get_num_entries();
          assert(num_entries <= 1024);
          int palette_bits = 1;
          while ((1<<palette_bits) < num_entries)
            palette_bits++;
          chnl->source_component->palette_bits = palette_bits;
          chnl->lut = new kdu_sample16[1<<palette_bits];
          palette.get_lut(plt_cmp,chnl->lut);
          for (i=num_entries; i < (1<<palette_bits); i++)
            chnl->lut[i] = chnl->lut[num_entries-1];
        }
    }

  // Complete components and channels
  use_ycc = tile.get_ycc();
  for (c=0; c < num_components; c++)
  {
      kde_component_flow_control *comp = components + c;
      if (!comp->mapped_by_channel)
        continue;
      bool use_shorts = comp->allow_shorts;
      if ((comp->tc.get_bit_depth(true) > 16) &&
          ((comp->palette_bits == 0) || comp->reversible))
        use_shorts = false;
	  if(writer && writer->exists())
		channels[c].vil_writer->use_shorts=use_shorts;
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
}

/*****************************************************************************/
/*                    kde_flow_control::~kde_flow_control                    */
/*****************************************************************************/

kde_flow_control::~kde_flow_control()
{
  for (int n=0; n < num_components; n++)
    {
      kde_component_flow_control *comp = components + n;
      if (comp->decompressor.exists())
        comp->decompressor.destroy();
    }
  delete[] components;
  for (int c=0; c < num_channels; c++)
    {
      kde_channel *chnl = channels + c;
      if (chnl->lut != NULL)
        delete[] (chnl->lut);
    }
  delete[] channels;
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

/*****************************************************************************/
/*                    kde_flow_control::advance_components                   */
/*****************************************************************************/

bool
  kde_flow_control::advance_components()
{
  bool found_line=false;

  while (!found_line)
    {
      bool all_done = true;
      for (int n=0; n < num_components; n++)
        {
          kde_component_flow_control *comp = components + n;
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

  if ((use_ycc) && (components[0].ratio_counter < 0))
    {
      assert((num_components >= 3) &&
             (components[1].ratio_counter < 0) &&
             (components[2].ratio_counter < 0));
      kdu_convert_ycc_to_rgb(components[0].line,
                             components[1].line,
                             components[2].line);
    }

  for (int c=0; c < num_channels; c++)
    {
      kde_channel *chnl = channels + c;
      kde_component_flow_control *comp = chnl->source_component;
      if ((comp->ratio_counter < 0) && (chnl->lut != NULL))
        { // Perform LUT mapping.
          kdu_sample16 *lut = chnl->lut;
          kdu_sample16 *dp = chnl->line.get_buf16();
          int i = chnl->width;
          if (comp->line.get_buf16() != NULL)
            {
              kdu_sample16 *sp = comp->line.get_buf16();
              for (; i > 0; i--, sp++, dp++)
                *dp = lut[sp->ival];
            }
          else
            {
              kdu_sample32 *sp = comp->line.get_buf32();
              for (; i > 0; i--, sp++, dp++)
                *dp = lut[sp->ival];
            }
        }
    }

  return true;
}

/*****************************************************************************/
/*                 kde_flow_control::access_decompressed_line                */
/*****************************************************************************/

kdu_line_buf *
  kde_flow_control::access_decompressed_line(int channel_idx)
{
  assert((channel_idx >= 0) && (channel_idx < num_channels));
  kde_channel *chnl = channels + channel_idx;
  kde_component_flow_control *comp = chnl->source_component;
  if (comp->ratio_counter >= 0)
    return NULL;
  if (chnl->lut != NULL)
    return &(chnl->line);
  else
    return &(comp->line);
}

/*****************************************************************************/
/*                    kde_flow_control::process_components                   */
/*****************************************************************************/

void
  kde_flow_control::process_components()
{
  for (int c=0; c < num_channels; c++)
    {
      kde_channel *chnl = channels + c;
      kde_component_flow_control *comp = chnl->source_component;
      if ((comp->ratio_counter < 0) && chnl->writer.exists() && chnl->width)
        {
          if (chnl->lut != NULL)
            chnl->writer.put(c,chnl->line,x_tnum);
          else
            chnl->writer.put(c,comp->line,x_tnum);
        }else if ((comp->ratio_counter < 0) && chnl->vil_writer &&
			chnl->vil_writer->exists() && chnl->width)
			  chnl->vil_writer->put(c,comp->line,x_tnum);
    }

  for (int n=0; n < num_components; n++)
    {
      kde_component_flow_control *comp = components + n;
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
/*                        kde_flow_control::advance_tile                     */
/*****************************************************************************/

bool
  kde_flow_control::advance_tile()
{
  int c;

  if (!tile)
    return false;

  // Clean up existing resources
  for (c=0; c < num_components; c++)
    {
      kde_component_flow_control *comp = components + c;
      if (!comp->mapped_by_channel)
        continue;
      assert(comp->remaining_lines == 0);
      assert(comp->decompressor.exists());
      comp->decompressor.destroy();
      comp->line.destroy();
    }
  for (c=0; c < num_channels; c++)
    {
      kde_channel *chnl = channels + c;
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
      kde_component_flow_control *comp = components + c;
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


