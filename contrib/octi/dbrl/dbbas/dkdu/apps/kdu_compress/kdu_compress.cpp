/*****************************************************************************/
// File: kdu_compress.cpp [scope = APPS/COMPRESSOR]
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
   File-based JPEG2000 compressor application, demonstrating many of the
compression-oriented capabilities of the Kakadu framework.
******************************************************************************/

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
/*                              kdc_flow_control                             */
/* ========================================================================= */

/*****************************************************************************/
/*                     kdc_flow_control::kdc_flow_control                    */
/*****************************************************************************/


kdc_flow_control::kdc_flow_control(kdc_file_binding *files,
                                   kdu_codestream codestream, int x_tnum,
                                   bool allow_shorts,
                                   kdu_roi_image *roi_source,vil_image_io* vil_io)
{
  int n;

  this->codestream = codestream;
  this->x_tnum = x_tnum;
  codestream.get_valid_tiles(valid_tile_indices);
  assert((x_tnum >= 0) && (x_tnum < valid_tile_indices.size.x));
  tile_idx = valid_tile_indices.pos;
  tile_idx.x += x_tnum;
  tile = codestream.open_tile(tile_idx);
  num_components = tile.get_num_components();
  assert(num_components > 0);
  roi_image = roi_source;

  // Set up the individual components.
  components = new kdc_component_flow_control[num_components];
  count_delta = 0;
  for (n=0; n < num_components; n++)
    {
      kdc_component_flow_control *comp = components + n;
      if (files != NULL)
        {
          assert(n >= files->first_comp_idx);
          if ((n-files->first_comp_idx) >= files->num_components)
            {
              files = files->next;
              assert((files != NULL) && (files->first_comp_idx == n));
            }
           comp->reader = files->reader;
        }
	  else 
		  comp->vil_reader=vil_io;

      kdu_tile_comp tc = tile.access_component(n);
      comp->reversible = tc.get_reversible();
      kdu_coords subsampling; tc.get_subsampling(subsampling);
      kdu_resolution res = tc.access_resolution();
      kdu_dims dims; res.get_dims(dims);
      comp->vert_subsampling = subsampling.y;
      if ((n == 0) || (comp->vert_subsampling < count_delta))
        count_delta = comp->vert_subsampling;
      comp->allow_shorts = allow_shorts;
      bool use_shorts = (tc.get_bit_depth(true)>16)?false:(comp->allow_shorts);
	  if((comp->vil_reader)&&(comp->vil_reader->exists()))
		  comp->vil_reader->use_shorts=use_shorts;
      comp->line.pre_create(&(comp->allocator),dims.size.x,
                            comp->reversible,use_shorts);
      kdu_roi_node *roi_node = NULL;
      if (roi_image != NULL)
        roi_node = roi_image->acquire_node(n,dims);
      if (res.which() == 0)
        comp->compressor =
          kdu_encoder(res.access_subband(LL_BAND),&(comp->allocator),
                      use_shorts,1.0F,roi_node);
      else
        comp->compressor =
          kdu_analysis(res,&(comp->allocator),use_shorts,1.0F,roi_node);
      comp->allocator.finalize();
      comp->line.create();
      comp->ratio_counter = 0;
      comp->remaining_lines = dims.size.y;
    }
  use_ycc = tile.get_ycc();
  if (use_ycc)
    assert((num_components >= 3) &&
           (components[0].reversible == components[1].reversible) &&
           (components[1].reversible == components[2].reversible));
}

/*****************************************************************************/
/*                    kdc_flow_control::~kdc_flow_control                    */
/*****************************************************************************/

kdc_flow_control::~kdc_flow_control()
{
  for (int n=0; n < num_components; n++)
    {
      kdc_component_flow_control *comp = components + n;
      if (comp->compressor.exists())
        comp->compressor.destroy();
    }
  delete[] components;
}

/*****************************************************************************/
/*                  kdc_flow_control::advance_components                     */
/*****************************************************************************/

bool
  kdc_flow_control::advance_components()
{
  bool found_line=false;

  while (!found_line)
    {
      bool all_done = true;
      for (int n=0; n < num_components; n++)
        {
          kdc_component_flow_control *comp = components + n;
          assert(comp->ratio_counter >= 0);
          if (comp->remaining_lines > 0)
            {
              all_done = false;
              comp->ratio_counter -= count_delta;
              if (comp->ratio_counter < 0)
                {
                  found_line = true;
                  if (comp->reader.exists() && comp->line.get_width())
				      comp->reader.get(n,comp->line,x_tnum);
				  else if(comp->vil_reader && comp->vil_reader->exists()
					  && comp->line.get_width())
					  comp->vil_reader->get(n,comp->line,x_tnum);
                }
            }
        }
      if (all_done)
        return false;
    }
  return true;
}

/*****************************************************************************/
/*                  kdc_flow_control::access_compressor_line                 */
/*****************************************************************************/

kdu_line_buf *
  kdc_flow_control::access_compressor_line(int comp_idx)
{
  assert((comp_idx >= 0) && (comp_idx < num_components));
  kdc_component_flow_control *comp = components + comp_idx;
  return (comp->ratio_counter < 0)?(&(comp->line)):NULL;
}

/*****************************************************************************/
/*                    kdc_flow_control::process_components                   */
/*****************************************************************************/

void
  kdc_flow_control::process_components()
{

  if ((use_ycc) && (components[0].ratio_counter < 0))
    {
      assert((num_components >= 3) &&
             (components[1].ratio_counter < 0) &&
             (components[2].ratio_counter < 0));
      kdu_convert_rgb_to_ycc(components[0].line,
                             components[1].line,
                             components[2].line);
    }
  for (int n=0; n < num_components; n++)
    {
      kdc_component_flow_control *comp = components + n;
      if (comp->ratio_counter < 0)
        {
          comp->ratio_counter += comp->vert_subsampling;
          assert(comp->ratio_counter >= 0);
          assert(comp->remaining_lines > 0);
          comp->remaining_lines--;
          comp->compressor.push(comp->line,true);
        }
    }
}

/*****************************************************************************/
/*                        kdc_flow_control::advance_tile                     */
/*****************************************************************************/

bool
  kdc_flow_control::advance_tile()
{
  int n;

  if (!tile)
    return false;
  for (n=0; n < num_components; n++)
    {
      kdc_component_flow_control *comp = components + n;
      assert(comp->remaining_lines == 0);
      assert(comp->compressor.exists());
      comp->compressor.destroy();
      comp->line.destroy();
    }
  tile.close();
  tile = kdu_tile(NULL);

  tile_idx.y++;
  if ((tile_idx.y-valid_tile_indices.pos.y) == valid_tile_indices.size.y)
    return false;

  // Prepare for processing the next vertical tile.

  tile = codestream.open_tile(tile_idx);
  for (n=0; n < num_components; n++)
    {
      kdc_component_flow_control *comp = components + n;
      kdu_tile_comp tc = tile.access_component(n);
      comp->reversible = tc.get_reversible();
      kdu_resolution res = tc.access_resolution();
      kdu_dims dims; res.get_dims(dims);
      comp->allocator.restart();
      bool use_shorts = (tc.get_bit_depth(true)>16)?false:(comp->allow_shorts);
      comp->line.pre_create(&(comp->allocator),dims.size.x,
                            comp->reversible,use_shorts);
      kdu_roi_node *roi_node = NULL;
      if (roi_image != NULL)
        roi_node = roi_image->acquire_node(n,dims);
      if (res.which() == 0)
        comp->compressor =
          kdu_encoder(res.access_subband(LL_BAND),&(comp->allocator),
                      use_shorts,1.0F,roi_node);
      else
        comp->compressor =
          kdu_analysis(res,&(comp->allocator),use_shorts,1.0F,roi_node);
      comp->allocator.finalize();
      comp->line.create();
      comp->ratio_counter = 0;
      comp->remaining_lines = dims.size.y;
    }
  use_ycc = tile.get_ycc();
  if (use_ycc)
    assert((num_components >= 3) &&
           (components[0].reversible == components[1].reversible) &&
           (components[1].reversible == components[2].reversible));

  return true;
}

/* ========================================================================= */
/*                             External Functions                            */
/* ========================================================================= */

/*****************************************************************************/
/*                                   main                                    */
/*****************************************************************************/

