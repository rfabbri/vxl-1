/*****************************************************************************/
// File: compressed.cpp [scope = CORESYS/COMPRESSED]
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
   Implements most of the compressed data management machinery which fits
logically between actual code-stream I/O (see "codestream.cpp") and individual
code-block processing (see "blocks.cpp").  Includes the machinery for
generating, tearing down and re-entering tiles, tile-components, resolutions,
subbands and precincts.
******************************************************************************/

#include <string.h>
#include <limits.h>
#include <assert.h>
#include "kdu_elementary.h"
#include "kdu_messaging.h"
#include "kdu_utils.h"
#include "kdu_kernels.h"
#include "kdu_compressed.h"
#include "compressed_local.h"

/* ========================================================================= */
/*                             Internal Functions                            */
/* ========================================================================= */

/*****************************************************************************/
/* STATIC                         get_band_dims                              */
/*****************************************************************************/

static kdu_dims
  get_band_dims(kdu_dims res_dims, kdu_coords band_idx,
                int low_extend=0, int high_extend=0)
  /* Converts a region in the containing resolution level into a region
     in an individual subband, given the suband indices (1 for high-pass,
     0 for low-pass).  The `low_extend' and `high_extend' values are
     used to extend the region in the resolution level before reducing
     it to a subband region, where `low_extend' is used for low-pass
     subbands and `high_extend' is used for high-pass subbands.  This is
     useful for taking into account the spatial support of the synthesis
     kernels when mapping regions of interest into subbands. */
{
  kdu_coords min = res_dims.pos;
  kdu_coords lim = min + res_dims.size;

  min.x -= (band_idx.x)?high_extend:low_extend;
  min.y -= (band_idx.y)?high_extend:low_extend;
  lim.x += (band_idx.x)?high_extend:low_extend;
  lim.y += (band_idx.y)?high_extend:low_extend;

  min.x = (min.x + 1 - band_idx.x) >> 1;
  lim.x = (lim.x + 1 - band_idx.x) >> 1;
  min.y = (min.y + 1 - band_idx.y) >> 1;
  lim.y = (lim.y + 1 - band_idx.y) >> 1;

  kdu_dims result;
  result.pos = min; result.size = lim-min;
  
  return result;
}

/*****************************************************************************/
/* STATIC                   get_partition_indices                            */
/*****************************************************************************/

static kdu_dims
  get_partition_indices(kdu_dims partition, kdu_dims region)
  /* Returns the range of indices for elements in the supplied partition,
     which intersect with the supplied region. The `pos' field of the
     partition identifies the coordinates of the upper left hand corner
     of the first element in the partition, having indices (0,0), while the
     `size' field indicates the dimensions of the partition elements.
     Intersecting partitions with regions is a common function in JPEG2000. */
{
  kdu_coords min = region.pos - partition.pos;
  kdu_coords lim = min + region.size;
  min.x = floor_ratio(min.x,partition.size.x);
  lim.x = ceil_ratio(lim.x,partition.size.x);
  min.y = floor_ratio(min.y,partition.size.y);
  lim.y = ceil_ratio(lim.y,partition.size.y);
  if (region.size.x == 0) lim.x = min.x;
  if (region.size.y == 0) lim.y = min.y;

  kdu_dims indices;
  indices.pos = min;
  indices.size = lim-min;

  return indices;
}

/*****************************************************************************/
/* STATIC                        is_power_2                                  */
/*****************************************************************************/

static bool
  is_power_2(int val)
{
  for (; val > 1; val >>= 1)
    if (val & 1)
      return false;
    return (val==1);
}

/*****************************************************************************/
/* STATIC                   check_coding_partition                           */
/*****************************************************************************/

static void
  check_coding_partition(kdu_dims partition)
  /* Coding partitions (namely, code-block and precinct partitions) must have
     exact power-of-2 dimensions and origins equal to 0 or 1. */
{
  if ((partition.pos.x != (partition.pos.x & 1)) ||
      (partition.pos.y != (partition.pos.y & 1)))
    { kdu_error e; e << "Coding partitions (code-blocks and precinct "
      "partitions) must have origin coordinates equal to 1 or 0 only!"; }
  if (!(is_power_2(partition.size.x) && is_power_2(partition.size.y)))
    { kdu_error e; e << "Coding partitions (namely, code-block and precinct "
      "partitions) must have exact power-of-2 dimensions!"; }
}

/*****************************************************************************/
/* INLINE                        compare_sop_num                             */
/*****************************************************************************/

static inline int
  compare_sop_num(int sop_num, int packet_num)
  /* Compares a true packet sequence number with the 16-bit sequence
     number found in an SOP marker segment.  The SOP number is the least
     significant 16 bits of the real packet sequence number, so the function
     returns 0 (equal) if and only if the least significant 16 bits of
     `packet_num' are identical to the value in `sop_num'.  Otherwise, the
     function returns the expected number of packets between the one
     identified by `packet_num' and that identified by `sop_num'.  The
     return value is positive if `sop_num' is deemed to refer to a packet
     following that identified by `packet_num', taking into account the
     fact that `sop_num' contains only the least significant 16 bits of
     the sequence number. */
{
  assert((sop_num >= 0) && (sop_num < (1<<16)));
  int diff = sop_num - packet_num;

  if ((diff & 0x0000FFFF) == 0)
    return 0;
  if ((diff > 0) || ((diff & 0x0000FFFF) <= (1<<15)))
    return diff; // `sop_num' deemed to be greater than `packet_num'.
  else
    return diff-(1<<16); // `sop_num' deemed to be less than `packet_num'.
}


/* ========================================================================= */
/*                                    kd_tile                                */
/* ========================================================================= */

/*****************************************************************************/
/*                              kd_tile::kd_tile                             */
/*****************************************************************************/

kd_tile::kd_tile(kd_codestream *codestream, int tnum)
{
  this->codestream = codestream;
  codestream->var_structure_new(sizeof(*this));
  this->tnum = tnum;
  int y_idx = tnum / codestream->num_tiles.x;
  assert((y_idx >= 0) && (y_idx < codestream->num_tiles.y));
  int x_idx = tnum - y_idx*codestream->num_tiles.x;
  dims = codestream->tile_partition;
  dims.pos.x += x_idx*dims.size.x;
  dims.pos.y += y_idx*dims.size.y;
  dims &= codestream->canvas;
  region = dims;
  initialized = false;
  exhausted = false;
  is_open = false;
  closed = false;
  packed_headers = NULL;
  sequencer = NULL;
  comps = NULL;
}

/*****************************************************************************/
/*                             kd_tile::~kd_tile                             */
/*****************************************************************************/

kd_tile::~kd_tile()
{
  codestream->var_structure_delete(sizeof(*this));
  if ((codestream->in != NULL) && initialized && !exhausted)
    finished_reading();

  if (packed_headers != NULL)
    delete packed_headers;

  if (codestream->textualize_out != NULL)
    {
      std::ostream &out = *codestream->textualize_out;
      codestream->siz->textualize_attributes(out,tnum,tnum);
      out.flush();
    }

  kdu_params *csp;
  int cluster = 1;
  while ((csp=codestream->siz->access_cluster(cluster++)) != NULL)
    if ((csp=csp->access_relation(tnum,-1)) != NULL)
      delete csp;
  if (sequencer != NULL)
    delete sequencer;
  if (comps != NULL)
    delete[] comps;
  assert(codestream->tile_refs[tnum] == this);
  codestream->tile_refs[tnum] = KD_EXPIRED_TILE;
}

/*****************************************************************************/
/*                            kd_tile::initialize                            */
/*****************************************************************************/

void
  kd_tile::initialize()
{
  bool read_failure = false;

  num_components = codestream->num_components;
  next_tpart = num_tparts = 0;
  if (codestream->in != NULL)
    read_failure = !read_tile_part_header();

  kdu_params *cod = codestream->siz->access_cluster(COD_params);
  assert(cod != NULL); cod = cod->access_relation(tnum,-1);
  kdu_params *qcd = codestream->siz->access_cluster(QCD_params);
  assert(qcd != NULL); qcd = qcd->access_relation(tnum,-1);
  kdu_params *rgn = codestream->siz->access_cluster(RGN_params);
  assert(rgn != NULL); rgn = rgn->access_relation(tnum,-1);

  // Get tile-wide COD parameters.

  if (!(cod->get(Cuse_sop,0,0,use_sop) &&
        cod->get(Cuse_eph,0,0,use_eph) &&
        cod->get(Cycc,0,0,use_ycc) &&
        cod->get(Calign_blk_last,0,0,coding_origin.y) &&
        cod->get(Calign_blk_last,0,1,coding_origin.x) &&
        cod->get(Clayers,0,0,num_layers)))
    assert(0);
  if (num_layers > codestream->max_tile_layers)
    codestream->max_tile_layers = num_layers;

  // Initialize appearance parameters

  num_apparent_components = num_components;
  first_apparent_component = 0;
  num_apparent_layers = num_layers;

  // Build tile-components.

  int c;
  kd_tile_comp *tc =
    comps = new kd_tile_comp[num_components];
  codestream->var_structure_new(sizeof(kd_tile_comp)*num_components);
  this->total_precincts = 0;
  for (c=0; c < num_components; c++, tc++)
    {
      kdu_coords subs, min, lim;

      tc->codestream = codestream;
      tc->tile = this;
      tc->cnum = c;
      tc->sub_sampling = subs = codestream->sub_sampling[c];
      min = dims.pos; lim = min + dims.size;
      min.x = ceil_ratio(min.x,subs.x); lim.x = ceil_ratio(lim.x,subs.x);
      min.y = ceil_ratio(min.y,subs.y); lim.y = ceil_ratio(lim.y,subs.y);
      tc->dims.pos = min; tc->dims.size = lim - min;
      tc->region = tc->dims;
      
      kdu_params *coc = cod->access_relation(tnum,c);
      kdu_params *qcc = qcd->access_relation(tnum,c);
      kdu_params *rgc = rgn->access_relation(tnum,c);
      assert((coc != NULL) && (qcc != NULL) && (rgc != NULL));

      bool use_precincts;
      bool derived_quant;
      float base_delta = 0.0F;
      if (!(coc->get(Clevels,0,0,tc->dwt_levels) &&
            coc->get(Creversible,0,0,tc->reversible) &&
            coc->get(Ckernels,0,0,tc->kernel_id) &&
            coc->get(Cuse_precincts,0,0,use_precincts) &&
            coc->get(Cblk,0,0,tc->blk.y) &&
            coc->get(Cblk,0,1,tc->blk.x) &&
            coc->get(Cmodes,0,0,tc->modes)))
        assert(0);
      if ((!tc->reversible) &&
          !(qcc->get(Qderived,0,0,derived_quant) &&
            ((!derived_quant) || qcc->get(Qabs_steps,0,0,base_delta))))
        assert(0);
      int roi_levels;
      if ((codestream->out == NULL) || !rgc->get(Rlevels,0,0,roi_levels))
        roi_levels = 0;
      tc->apparent_dwt_levels = tc->dwt_levels;
      if (tc->reversible)
        tc->recommended_extra_bits = 4 + ((use_ycc)?1:0);
      else
        tc->recommended_extra_bits = 7;

      // Create a DWT kernels object to recover energy weights.

      kdu_kernels kernels(tc->kernel_id,tc->reversible);

      // Now build the resolution level structure.

      int r;
      tc->resolutions = new kd_resolution[tc->dwt_levels+1];
      codestream->var_structure_new(sizeof(kd_resolution)*(tc->dwt_levels+1));
      kdu_dims res_dims = tc->dims;
      for (r=tc->dwt_levels; r >= 0; r--)
        {
          kd_resolution *res = tc->resolutions + r;

          res->codestream = codestream;
          res->tile_comp = tc;
          res->res_level = r;
          res->dwt_level = tc->dwt_levels - ((r==0)?0:(r-1));
          res->propagate_roi = (res->dwt_level <= roi_levels);
          res->dims = res->region = res_dims;

          // Set up precincts.

          res->precinct_partition.pos = coding_origin;
          if (!use_precincts)
            {
              res->precinct_partition.size.x = 1<<15;
              res->precinct_partition.size.y = 1<<15;
            }
          else if (!(coc->get(Cprecincts,tc->dwt_levels-r,0,
                              res->precinct_partition.size.y) &&
                     coc->get(Cprecincts,tc->dwt_levels-r,1,
                              res->precinct_partition.size.x)))
              assert(0);
          check_coding_partition(res->precinct_partition);
          res->precinct_indices =
            get_partition_indices(res->precinct_partition,res_dims);
          int num_precincts = res->precinct_indices.area();
          res->precinct_refs = new kd_precinct *[num_precincts];
          codestream->var_structure_new(sizeof(kd_precinct *)*num_precincts);
          for (int n=0; n < num_precincts; n++)
            res->precinct_refs[n] = NULL;
          this->total_precincts += num_precincts;

          // Get energy weighting parameters.

          float level_weight;
          if (!coc->get(Clev_weights,tc->dwt_levels-r,0,level_weight))
            level_weight = 1.0;
          double G_lo =
            kernels.get_energy_gain(KDU_SYNTHESIS_LOW,res->dwt_level);
          double G_hi =
            kernels.get_energy_gain(KDU_SYNTHESIS_HIGH,res->dwt_level);

          // Now build the subband structure.

          if (r==0)
            { res->min_band = res->max_band = LL_BAND; }
          else
            { res->min_band = 1; res->max_band = 3; }
          int b;
          kd_subband *band;
          res->bands = new kd_subband[res->max_band+1];
          codestream->var_structure_new(sizeof(kd_subband)*(res->max_band+1));
          for (b=res->min_band; b <= res->max_band; b++)
            {
              band = res->bands + b;
              band->codestream = codestream;
              band->resolution = res;
              band->which_band = b;
              band->band_idx.y = ((b==LH_BAND)||(b==HH_BAND))?1:0;
              band->band_idx.x = ((b==HL_BAND)||(b==HH_BAND))?1:0;
              if (r > 0)
                band->dims = band->region =
                  get_band_dims(res_dims,band->band_idx);
              else
                band->dims = band->region = res_dims;

              // Determine quantization-related parameters for the subband.

              int max_band_idx = 3*tc->dwt_levels;
              int abs_band_idx = 3*(tc->dwt_levels-res->dwt_level) + b;
              assert((abs_band_idx >= 0) && (abs_band_idx <= max_band_idx));

              if (tc->reversible)
                {
                  if (!qcc->get(Qabs_ranges,abs_band_idx,0,band->epsilon))
                    assert(0);
                  band->delta = 1.0F / ((float)(1<<codestream->precision[c]));
                }
              else
                {
                  float delta;
                  if (derived_quant)
                    delta = base_delta * (float)
                      (1<<(tc->dwt_levels-res->dwt_level));
                  else if (!qcc->get(Qabs_steps,abs_band_idx,0,delta))
                    assert(0);
                  assert(delta > 0.0F);
                  band->delta = delta;
                  for (band->epsilon=0; delta < 1.0F; delta*=2.0F)
                    band->epsilon++;
                  assert(delta < 2.0F);
                }
              if (!qcc->get(Qguard,0,0,band->K_max))
                assert(0);
              if (!rgc->get(Rweight,0,0,band->roi_weight))
                band->roi_weight = -1.0F; // Indicates no ROI weights.
              band->K_max += band->epsilon - 1;
              if (!rgc->get(Rshift,0,0,band->K_max_prime))
                band->K_max_prime = 0;
              band->K_max_prime += band->K_max;
              if (!coc->get(Cband_weights,max_band_idx-abs_band_idx,0,
                            band->W_b))
                band->W_b = 1.0F;
              band->W_b *= level_weight;
              if (b == LL_BAND)
                band->W_b = 1.0F; // Don't tamper with DC band.
              band->G_b = (float)
               (((band->band_idx.x)?G_hi:G_lo)*((band->band_idx.y)?G_hi:G_lo));
              if (use_ycc)
                { // Need to include colour transform synthesis gains.
                  if (tc->reversible)
                    {
                      if (c == 0)
                        band->G_b *=(float)( 1.0*1.0  +  1.0*1.0  +  1.0*1.0);
                      else if ((c == 1) || (c == 2))
                        band->G_b *=(float)(0.75*0.75 + 0.25*0.25 + 0.25*0.25);
                    }
                  else
                    {
                      double alpha_R=0.299, alpha_G=0.587, alpha_B=0.114;
                      if (c == 0)
                        band->G_b *= (float)( 1.0*1.0  +  1.0*1.0  +  1.0*1.0);
                      else if (c == 1)
                        { double f1 = 2.0*(1-alpha_B);
                          double f2 = 2.0*alpha_B*(1-alpha_B)/alpha_G;
                          band->G_b *= (float)(f1*f1 + f2*f2); }
                      else if (c == 2)
                        { double f1 = 2.0*(1-alpha_R);
                          double f2 = 2.0*alpha_R*(1-alpha_R)/alpha_G;
                          band->G_b *= (float)(f1*f1 + f2*f2); }
                    }
                }

              // Now determine code-block partition parameters for the subband.

              band->block_partition.pos = res->precinct_partition.pos;
              band->block_partition.size = tc->blk;
              if (b != LL_BAND)
                {
                  band->block_partition.size.x <<= 1;
                  band->block_partition.size.y <<= 1;
                }
              band->block_partition &= res->precinct_partition; // Intersect.
              band->blocks_per_precinct.x =
                res->precinct_partition.size.x / band->block_partition.size.x;
              band->blocks_per_precinct.y =
                res->precinct_partition.size.y / band->block_partition.size.y;
              if (b != LL_BAND)
                {
                  band->block_partition.pos.x &= ~(band->band_idx.x);
                  band->block_partition.pos.y &= ~(band->band_idx.y);
                  band->block_partition.size.x >>= 1;
                  band->block_partition.size.y >>= 1;
                  if (!band->block_partition)
                    { kdu_error e; e << "Precinct partition dimensions must "
                      "be >= 2 in all but the lowest resolution level!"; }
                }
              check_coding_partition(band->block_partition);
              band->block_indices = band->region_indices =
                get_partition_indices(band->block_partition,band->dims);
              band->log2_blocks_per_precinct = kdu_coords(0,0);
              while ((1<<band->log2_blocks_per_precinct.x) <
                     band->blocks_per_precinct.x)
                band->log2_blocks_per_precinct.x++;
              while ((1<<band->log2_blocks_per_precinct.y) <
                     band->blocks_per_precinct.y)
                band->log2_blocks_per_precinct.y++;
            } // End of subband loop.
          res_dims = get_band_dims(res_dims,kdu_coords(0,0));
        } // End of resolution loop.
    } // End of tile-component loop.

  // Perform any parameter consistency checks.

  if (use_ycc)
    {
     if ((num_components < 3) ||
         (comps[0].reversible != comps[1].reversible) ||
         (comps[1].reversible != comps[2].reversible) ||
         (comps[0].sub_sampling != comps[1].sub_sampling) ||
         (comps[1].sub_sampling != comps[2].sub_sampling))
       { kdu_error e; e << "Illegal colour transform specified when "
         "image has insufficient or incompatible colour components."; }
    }


  // Now set up the packet sequencing machinery. Note that packet
  // sequencing is performed incrementally, rather than up front.

  total_packets = total_precincts * num_layers;
  initialized = true;
  num_transferred_packets = 0;
  skipping_to_sop = false;
  sequencer = new kd_packet_sequencer(this);
  if (read_failure)
    finished_reading();
}

/*****************************************************************************/
/*                              kd_tile::open                                */
/*****************************************************************************/

void
  kd_tile::open()
{
  if (is_open)
    { kdu_error e; e << "You must close a tile before you can re-open it."; }

  // Inherit appearance parameters from parent object

  first_apparent_component = codestream->first_apparent_component;
  num_apparent_components = codestream->num_apparent_components;
  num_apparent_layers = codestream->max_apparent_layers;
  if (num_apparent_layers > num_layers)
    num_apparent_layers = num_layers;
  region = dims & codestream->region;

  // Walk through the components

  for (int c=0; c < num_components; c++)
    {
      kd_tile_comp *tc = comps + c;
      kdu_coords subs = tc->sub_sampling;
      kdu_coords min, lim;

      min = region.pos; lim = min + region.size;
      min.x = ceil_ratio(min.x,subs.x); lim.x = ceil_ratio(lim.x,subs.x);
      min.y = ceil_ratio(min.y,subs.y); lim.y = ceil_ratio(lim.y,subs.y);
      tc->region.pos = min; tc->region.size = lim - min;

      tc->apparent_dwt_levels = tc->dwt_levels - codestream->discard_levels;
      if (tc->apparent_dwt_levels < 0)
        { kdu_error e; e << "Attempting to discard more DWT levels than are "
          "available in one or more tile components!"; }

      // Find the DWT kernel lengths for use in mapping regions of interest.
      kdu_kernels kernels(tc->kernel_id,tc->reversible);
      int synth_hlen_low, synth_hlen_high;
      kernels.get_impulse_response(KDU_SYNTHESIS_LOW,synth_hlen_low);
      kernels.get_impulse_response(KDU_SYNTHESIS_HIGH,synth_hlen_high);

      // Now work through the resolution levels.
      kdu_dims next_region = tc->region;
      for (int r=tc->dwt_levels; r >= 0; r--)
        {
          kd_resolution *res = tc->resolutions + r;

          res->region = next_region & res->dims;
          for (int b=res->min_band; b <= res->max_band; b++)
            {
              kd_subband *band = res->bands + b;
              if (r > 0)
                {
                  band->region = get_band_dims(res->region,band->band_idx,
                                               synth_hlen_low,synth_hlen_high);
                  band->region &= band->dims;
                }
              else
                band->region = res->region;
              band->region_indices =
                get_partition_indices(band->block_partition,band->region);
            }
          if (r > tc->apparent_dwt_levels)
            next_region = get_band_dims(res->region,kdu_coords(0,0));
          else
            next_region = get_band_dims(res->region,kdu_coords(0,0),
                                        synth_hlen_low,synth_hlen_high);
        }
    }
  is_open = true;
  codestream->num_open_tiles++;
}

/*****************************************************************************/
/*                      kd_tile::read_tile_part_header                       */
/*****************************************************************************/

bool
  kd_tile::read_tile_part_header()
{
  assert(codestream->in != NULL);
  if (exhausted || ((num_tparts > 0) && (next_tpart >= num_tparts)))
    return false;
  do {
      kd_tile *active = codestream->active_tile;
      if (active != NULL)
        { // Read data into tile until we come to an SOT marker.
          kd_precinct *precinct;
          while ((precinct=active->sequencer->next_in_sequence()) != NULL)
            if (!precinct->read_packet())
              break;
            if (precinct == NULL)
              { // May not have read the next marker yet.
                codestream->active_tile = NULL;
                if (codestream->marker->get_code() != KDU_SOT)
                  codestream->marker->read();
              }
            assert(codestream->active_tile == NULL);
        }

      if (codestream->in->failed())
        return false;
      if (codestream->marker->get_code() != KDU_SOT)
        { kdu_error e; e << "Invalid marker code found in code-stream!\n";
          e << "\tExpected SOT marker and got ";
          codestream->marker->print_current_code(e); e << "."; }

      // Now process the SOT marker.

      int seg_length = codestream->marker->get_length();
      assert(seg_length == 8); // Should already have been checked in `read'.
      kdu_byte *bp = codestream->marker->get_bytes();
      kdu_byte *end = bp+seg_length;
      int sot_tnum = kdu_read(bp,end,2);
      int sot_tpart_length = kdu_read(bp,end,4);
      int sot_tpart = kdu_read(bp,end,1);
      int sot_num_tparts = kdu_read(bp,end,1);

      active = codestream->tile_refs[sot_tnum];
      if ((active != this) && (sot_tpart_length == 0))
        { // At the last tile-part and it belongs to a different tile.
          finished_reading();
          return false;
        }
      if (active == NULL)
        active = codestream->create_tile(sot_tnum); // May call here again
      else if (active != KD_EXPIRED_TILE)
        { // Read a new tile-part header for the currently active tile.
          assert(active->tnum == sot_tnum);
          if (active->next_tpart != sot_tpart)
            { kdu_error e; e << "Missing or out-of-sequence tile-parts for "
              "tile number " << sot_tnum << " in code-stream!"; }
          if (sot_num_tparts != 0)
            {
              if (active->num_tparts == 0)
                active->num_tparts = sot_num_tparts;
              else if (active->num_tparts != sot_num_tparts)
                { kdu_error e; e << "The number of tile-parts for tile number "
                  << sot_tnum << " is identified by different non-zero values "
                  "in different SOT markers for the tile!"; }
            }

          // Translate tile-part header markers.

          kdu_params *root = codestream->siz;
          kd_pp_markers *ppt_markers = NULL;
          do {
              if (!codestream->marker->read())
                {
                  if (!codestream->in->failed())
                    { kdu_error e; e << "Found non-marker code while looking "
                      "for SOD marker to terminate a tile-part header.  "
                      "Chances are that a marker segment length field is "
                      "incorrect!"; }
                  return false;
                }
              if (codestream->marker->get_code() == KDU_PPT)
                {
                  if (ppt_markers == NULL)
                    ppt_markers = new kd_pp_markers;
                  ppt_markers->add_marker(*(codestream->marker));
                }
              else
                root->translate_marker_segment(codestream->marker->get_code(),
                                              codestream->marker->get_length(),
                                              codestream->marker->get_bytes(),
                                              sot_tnum,sot_tpart);
            } while (codestream->marker->get_code() != KDU_SOD);

          // Transfer packed packet header data.

          if (ppt_markers != NULL)
            {
              if (codestream->ppm_markers != NULL)
                { kdu_error e; e << "Use of both PPM and PPT marker segments "
                  "is illegal!"; }
              if (active->packed_headers == NULL)
                active->packed_headers =
                  new kd_pph_input(codestream->buf_server);
              ppt_markers->transfer_tpart(active->packed_headers);
              delete ppt_markers;
            }
          else if (codestream->ppm_markers != NULL)
            {
              if (active->packed_headers == NULL)
                active->packed_headers =
                  new kd_pph_input(codestream->buf_server);
              codestream->ppm_markers->transfer_tpart(active->packed_headers);
            }

          // Make `active' the active tile.

          active->next_tpart++;
          codestream->active_tile = active;
          codestream->num_tparts_used++;
        }
      if (active == KD_EXPIRED_TILE)
        { /* Tile has been discarded.  Skip to next SOT marker, discarding any
             PPM info which may be available for this tile-part. */
          assert(sot_tpart_length != 0);
          int skip_bytes =
            sot_tpart_length - (codestream->marker->get_length()+4);

          if (codestream->ppm_markers != NULL)
            codestream->ppm_markers->ignore_tpart();
          codestream->in->ignore(skip_bytes);
          codestream->marker->read();
          if (codestream->marker->get_code() != KDU_SOT)
            {
              if (!codestream->in->failed())
                { kdu_error e; e << "Corrupt SOT length field used to "
                  "seek over a discarded tile-part."; }
              return false;
            }
        }
    } while (this != codestream->active_tile);

  return true;
}

/*****************************************************************************/
/*                        kd_tile::finished_reading                          */
/*****************************************************************************/

void
  kd_tile::finished_reading()
{
  if (!initialized)
    return; // Prevent calls here while trying to initialize a tile.
  if (this == codestream->active_tile)
    { // Need to discard unread data up to the next SOT marker.
      assert(!exhausted);
      assert(codestream->in != NULL);
      assert(codestream->marker->get_code() != KDU_SOT);
      while (codestream->marker->read(true,true))
        if (codestream->marker->get_code() == KDU_SOT)
          break;
      codestream->active_tile = NULL;
    }
  if (exhausted)
    return;
  exhausted = true;
  if (closed)
    { // Should never happen if the codestream object is persistent.
      delete this; // Self efacing.
      return;
    }
  for (int c=0; c < num_components; c++)
    {
      kd_tile_comp *tc = comps + c;
      for (int r=0; r <= tc->dwt_levels; r++)
        {
          kd_resolution *res = tc->resolutions + r;
          int num_precincts = res->precinct_indices.area();
          for (int p=0; p < num_precincts; p++)
            {
              kd_precinct *precinct = res->precinct_refs[p];
              if ((precinct == NULL) || (precinct == KD_EXPIRED_PRECINCT))
                continue;
              if (precinct->num_outstanding_blocks == 0)
                delete precinct;
            }
        }
    }
}

/*****************************************************************************/
/*                         kd_tile::simulate_output                          */
/*****************************************************************************/

int
  kd_tile::simulate_output(int &header_bytes, int layer_idx,
                            kdu_uint16 slope_threshold, bool finalize_layer,
                            bool last_layer, int max_bytes, int *sloppy_bytes)
{
  header_bytes = 0;
  if (layer_idx == 0)
    { // Include the cost of all tile-part headers.
      int new_bytes = 0;
      int try_tpart = 0;
      do {
          new_bytes =
            codestream->siz->generate_marker_segments(NULL,tnum,try_tpart);
          if ((new_bytes > 0) || (try_tpart == 0))
            {
              new_bytes += 12; // Cost of SOT marker segment
              new_bytes += 2; // Cost of SOD marker
            }
          header_bytes += new_bytes;
          try_tpart++;
        } while (new_bytes != 0);
    }

  if (last_layer && (num_layers > (layer_idx+1)))
    { // Add in cost of empty packets
      header_bytes +=
        (num_layers-1-layer_idx)*total_precincts*((use_eph)?3:1);
    }

  int total_bytes = header_bytes;
  if (layer_idx >= num_layers)
    return total_bytes;

  for (int c=0; c < num_components; c++)
    {
      kd_tile_comp *tc = comps + c;
      for (int r=0; r <= tc->dwt_levels; r++)
        {
          kd_resolution *res = tc->resolutions + r;
          int p=0;
          for (int py=0; py < res->precinct_indices.size.y; py++)
            for (int px=0; px < res->precinct_indices.size.x; px++, p++)
              {
                kd_precinct *precinct = res->precinct_refs[p];
                assert(precinct != KD_EXPIRED_PRECINCT);
                if (precinct == NULL)
                  { // Could be a precinct without any code-blocks.
                    precinct = new kd_precinct(res,kdu_coords(px,py));
                    assert(precinct == res->precinct_refs[p]);
                    if (precinct->num_outstanding_blocks != 0)
                      { kdu_error e; e << "You may not currently flush "
                        "compressed code-stream data without completing the "
                        "compression of all tiles."; }
                  }
                int packet_header_bytes = 0, packet_bytes = 0;
                if (sloppy_bytes != NULL)
                  {
                    assert(finalize_layer && last_layer &&
                           (*sloppy_bytes >= 0));
                    assert(slope_threshold < 0xFFFF);
                    packet_bytes =
                      precinct->simulate_packet(packet_header_bytes,layer_idx,
                                                slope_threshold+1,false,true);
                    int packet_max_bytes = packet_bytes + *sloppy_bytes;
                    assert((packet_max_bytes+total_bytes) <= max_bytes);
                      // Otherwise, you are not driving this function correctly
                    int packet_bytes =
                      precinct->simulate_packet(packet_header_bytes,layer_idx,
                                                slope_threshold,true,true,
                                                packet_max_bytes,true);
                    assert(packet_bytes <= packet_max_bytes);
                    *sloppy_bytes = packet_max_bytes - packet_bytes;
                  }
                else
                  packet_bytes =
                    precinct->simulate_packet(packet_header_bytes,layer_idx,
                                             slope_threshold,finalize_layer,
                                             last_layer,max_bytes-total_bytes);
                total_bytes += packet_bytes;
                header_bytes += packet_header_bytes;
                if (total_bytes > max_bytes)
                  {
                    assert(!finalize_layer);
                      // Otherwise, you are not driving this function correctly
                    return total_bytes;
                  }
              }
        }
    }
  return total_bytes;
}

/*****************************************************************************/
/*                       kd_tile::generate_tile_part                         */
/*****************************************************************************/

int
  kd_tile::generate_tile_part(int max_layers, kdu_uint16 slope_thresholds[])
{
  if (num_transferred_packets == total_packets)
    return 0;
  if (next_tpart >= 255)
    {kdu_error e; e << "Too many tile-parts for tile " << this->tnum << ".  "
     "No tile may have more than 255 parts."; }
  next_tpart++; // Makes sure packet sequencer does the right thing.
  int tpart_bytes = 12 + 2 +
    codestream->siz->generate_marker_segments(NULL,tnum,next_tpart-1);

  // Simulate packet sequencing to determine the tile-part length
  kd_precinct *precinct;
  sequencer->save_state();
  while ((precinct = sequencer->next_in_sequence()) != NULL)
    {
      int layer_idx = precinct->next_layer_idx;
      assert (layer_idx < precinct->resolution->tile_comp->tile->num_layers);
      if ((precinct->packet_bytes == NULL) ||
          (precinct->packet_bytes[layer_idx] == 0))
        { kdu_error e; e << "Attempting to generate tile-part data without "
          "first determining packet lengths.  This may be a consequence of "
          "incomplete simulation of the packet construction process."; }
      tpart_bytes += precinct->packet_bytes[layer_idx];
      precinct->next_layer_idx++; // This will be restored later.
      num_transferred_packets++;
    }

  // Now generate the tile-part header
  kd_compressed_output *out = codestream->out;
  int start_bytes = out->get_bytes_written();
  
  out->put(KDU_SOT);
  out->put((kdu_uint16) 10);
  out->put((kdu_uint16) tnum);
  out->put((kdu_uint32) tpart_bytes);
  out->put((kdu_byte)(next_tpart-1));
  if (num_transferred_packets == total_packets)
    out->put((kdu_byte) next_tpart); // This is the last tile-part
  else
    out->put((kdu_byte) 0); // We don't know how many tile-parts might follow
  codestream->siz->generate_marker_segments(out,tnum,next_tpart-1);
  out->put(KDU_SOD);

  // Finally, output the packet data
  sequencer->restore_state();
  while ((precinct = sequencer->next_in_sequence()) != NULL)
    {
      int layer_idx = precinct->next_layer_idx;
      if (layer_idx < max_layers)
        precinct->write_packet(slope_thresholds[layer_idx]);
      else
        precinct->write_packet(0,true); // Write an empty packet.
    }

  // Finish up
  int check_tpart_bytes = out->get_bytes_written() - start_bytes;
  assert(tpart_bytes == check_tpart_bytes);
  codestream->num_tparts_used++;
  return tpart_bytes;
}


/* ========================================================================= */
/*                                  kdu_tile                                 */
/* ========================================================================= */

/*****************************************************************************/
/*                              kdu_tile::close                              */
/*****************************************************************************/

void
  kdu_tile::close()
{
  if (state->is_open)
    state->codestream->num_open_tiles--;
  else
    assert(0);
  state->is_open = false;
  assert(!state->closed);
  if (!state->codestream->persistent)
    {
      state->closed = true;
      if (state->exhausted)
        delete state;
    }
  state = NULL; // Renders the interface impotent.
}

/*****************************************************************************/
/*                             kdu_tile::get_tnum                            */
/*****************************************************************************/

int
  kdu_tile::get_tnum()
{
  return state->tnum;
}

/*****************************************************************************/
/*                             kdu_tile::get_ycc                             */
/*****************************************************************************/

bool
  kdu_tile::get_ycc()
{
  if ((state->first_apparent_component == 0) &&
      (state->num_apparent_components >= 3))
    return state->use_ycc;
  else
    return false;
}

/*****************************************************************************/
/*                        kdu_tile::get_num_components                       */
/*****************************************************************************/

int
  kdu_tile::get_num_components()
{
  return state->num_apparent_components;
}

/*****************************************************************************/
/*                         kdu_tile::access_component                        */
/*****************************************************************************/

kdu_tile_comp
  kdu_tile::access_component(int comp_idx)
{
  assert((comp_idx >= 0) && (comp_idx < state->num_apparent_components));
  comp_idx += state->first_apparent_component;
  return kdu_tile_comp(state->comps+comp_idx);
}


/* ========================================================================= */
/*                                 kd_tile_comp                              */
/* ========================================================================= */

/*****************************************************************************/
/*                       kd_tile_comp::~kd_tile_comp                         */
/*****************************************************************************/

kd_tile_comp::~kd_tile_comp()
{
  codestream->var_structure_delete(sizeof(*this));
  if (resolutions != NULL)
    delete[] resolutions;
}

/* ========================================================================= */
/*                                kdu_tile_comp                              */
/* ========================================================================= */

/*****************************************************************************/
/*                         kdu_tile_comp::get_reversible                     */
/*****************************************************************************/

bool
  kdu_tile_comp::get_reversible()
{
  return state->reversible;
}

/*****************************************************************************/
/*                        kdu_tile_comp::get_subsampling                     */
/*****************************************************************************/

void
  kdu_tile_comp::get_subsampling(kdu_coords &sub_sampling)
{
  sub_sampling = state->sub_sampling;
  int shift = state->dwt_levels - state->apparent_dwt_levels;
  sub_sampling.x <<= shift;
  sub_sampling.y <<= shift;
  if (state->codestream->transpose)
    sub_sampling.transpose();
}

/*****************************************************************************/
/*                          kdu_tile_comp::get_bit_depth                     */
/*****************************************************************************/

int
  kdu_tile_comp::get_bit_depth(bool internal)
{
  int bit_depth = state->codestream->precision[state->cnum];
  if (internal)
    bit_depth += state->recommended_extra_bits;
  return bit_depth;
}

/*****************************************************************************/
/*                           kdu_tile_comp::get_signed                       */
/*****************************************************************************/

bool
  kdu_tile_comp::get_signed()
{
  return state->codestream->is_signed[state->cnum];
}

/*****************************************************************************/
/*                      kdu_tile_comp::get_num_resolutions                   */
/*****************************************************************************/

int
  kdu_tile_comp::get_num_resolutions()
{
  return state->apparent_dwt_levels+1;
}

/*****************************************************************************/
/*                      kdu_tile_comp::access_resolution                     */
/*****************************************************************************/

kdu_resolution
  kdu_tile_comp::access_resolution(int res_level)
{
  assert((res_level >= 0) && (res_level <= state->apparent_dwt_levels));
  return kdu_resolution(state->resolutions+res_level);
}

/*****************************************************************************/
/*                 kdu_tile_comp::access_resolution (no args)                */
/*****************************************************************************/

kdu_resolution
  kdu_tile_comp::access_resolution()
{
  return kdu_resolution(state->resolutions+state->apparent_dwt_levels);
}


/* ========================================================================= */
/*                                kd_resolution                              */
/* ========================================================================= */

/*****************************************************************************/
/*                      kd_resolution::~kd_resolution                        */
/*****************************************************************************/

kd_resolution::~kd_resolution()
{
  codestream->var_structure_delete(sizeof(*this));
  if (precinct_refs != NULL)
    {
      for (int n=0; n < precinct_indices.size.x*precinct_indices.size.y; n++)
        if ((precinct_refs[n] != NULL) &&
            (precinct_refs[n] != KD_EXPIRED_PRECINCT))
          delete precinct_refs[n];
      delete[] precinct_refs;
      codestream->var_structure_delete(sizeof(kd_precinct *) *
                                       precinct_indices.area());
    }
  if (bands != NULL)
    {
      delete[] bands;
      codestream->var_structure_delete(sizeof(kd_subband)*(max_band+1));
    }
}

/* ========================================================================= */
/*                             kdu_resolution                                */
/* ========================================================================= */

/*****************************************************************************/
/*                       kdu_resolution::access_next                         */
/*****************************************************************************/

kdu_resolution
  kdu_resolution::access_next()
{
  assert(state != NULL);
  return kdu_resolution((state->res_level==0)?NULL:(state-1));
}

/*****************************************************************************/
/*                         kdu_resolution::which                             */
/*****************************************************************************/

int
  kdu_resolution::which()
{
  assert(state != NULL);
  return state->res_level;
}

/*****************************************************************************/
/*                       kdu_resolution::get_dwt_level                       */
/*****************************************************************************/

int
  kdu_resolution::get_dwt_level()
{
  return state->dwt_level;
}

/*****************************************************************************/
/*                         kdu_resolution::get_dims                          */
/*****************************************************************************/

void
  kdu_resolution::get_dims(kdu_dims &result)
{
  assert(state != NULL);
  result = state->region;
  state->codestream->to_apparent(result);
}

/*****************************************************************************/
/*                      kdu_resolution::access_subband                       */
/*****************************************************************************/

kdu_subband
  kdu_resolution::access_subband(int band_idx)
{
  assert((band_idx >= state->min_band) && (band_idx <= state->max_band));
  if (state->codestream->transpose)
    {
      if (band_idx == HL_BAND)
        band_idx = LH_BAND;
      else if (band_idx == LH_BAND)
        band_idx = HL_BAND;
    }
  return kdu_subband(state->bands+band_idx);
}

/*****************************************************************************/
/*                      kdu_resolution::get_reversible                       */
/*****************************************************************************/

bool
  kdu_resolution::get_reversible()
{
  return state->tile_comp->reversible;
}

/*****************************************************************************/
/*                      kdu_resolution::get_kernel_id                        */
/*****************************************************************************/

int
  kdu_resolution::get_kernel_id()
{
  return state->tile_comp->kernel_id;
}

/*****************************************************************************/
/*                      kdu_resolution::propagate_roi                        */
/*****************************************************************************/

bool
  kdu_resolution::propagate_roi()
{
  return state->propagate_roi;
}


/* ========================================================================= */
/*                               kdu_subband                                 */
/* ========================================================================= */

/*****************************************************************************/
/*                            kdu_subband::which                             */
/*****************************************************************************/

int
  kdu_subband::which()
{
  int band_idx = state->which_band;
  if (state->codestream->transpose)
    {
      if (band_idx == HL_BAND)
        band_idx = LH_BAND;
      else if (band_idx == LH_BAND)
        band_idx = HL_BAND;
    }
  return band_idx;
}

/*****************************************************************************/
/*                          kdu_subband::get_K_max                           */
/*****************************************************************************/

int
  kdu_subband::get_K_max()
{
  return state->K_max;
}

/*****************************************************************************/
/*                       kdu_subband::get_K_max_prime                        */
/*****************************************************************************/

int
  kdu_subband::get_K_max_prime()
{
  return state->K_max_prime;
}

/*****************************************************************************/
/*                        kdu_subband::get_reversible                        */
/*****************************************************************************/

bool
  kdu_subband::get_reversible()
{
  return state->resolution->tile_comp->reversible;
}

/*****************************************************************************/
/*                           kdu_subband::get_delta                          */
/*****************************************************************************/

float
  kdu_subband::get_delta()
{
  return (state->resolution->tile_comp->reversible)?0.0F:state->delta;
}

/*****************************************************************************/
/*                          kdu_subband::get_msb_wmse                        */
/*****************************************************************************/

float
  kdu_subband::get_msb_wmse()
{
  double result = state->delta;
  result *= (1<<(state->K_max_prime-1));
  result *= result;
  result *= state->G_b;
  result *= state->W_b;
  result *= state->W_b; // Squares the W_b weight.
  return (float) result;
}

/*****************************************************************************/
/*                         kdu_subband::get_roi_weight                       */
/*****************************************************************************/

bool
  kdu_subband::get_roi_weight(float &energy_weight)
{
  if (state->roi_weight < 0.0F)
    return false;
  energy_weight = state->roi_weight * state->roi_weight;
  return true;
}

/*****************************************************************************/
/*                           kdu_subband::get_dims                           */
/*****************************************************************************/

void
  kdu_subband::get_dims(kdu_dims &result)
  /* This is the only place where we need to be particularly careful
     about mapping apparent dimensions through geometric transformations.
     The reason is that high-pass subband coordinates are notionally based
     on an origin +1/2 a subband sample spacing beyond the origin of the
     real coordinate system.  To flip these subbands, it is not sufficient to
     negate the coordinates of each constituent sample; we must also
     subtract 1. */
{
  kdu_coords band_idx = state->band_idx;
  result = state->region;
  state->codestream->to_apparent(result);
  if (state->codestream->transpose)
    band_idx.transpose();
  if (state->codestream->hflip && band_idx.x)
    result.pos.x--;
  if (state->codestream->vflip && band_idx.y)
    result.pos.y--;
}

/*****************************************************************************/
/*                       kdu_subband::get_valid_blocks                       */
/*****************************************************************************/

void
  kdu_subband::get_valid_blocks(kdu_dims &indices)
{
  indices = state->region_indices;
  state->codestream->to_apparent(indices);
}

/*****************************************************************************/
/*                        kdu_subband::get_block_size                        */
/*****************************************************************************/

void
  kdu_subband::get_block_size(kdu_coords &nominal_size, kdu_coords &first_size)
{
  kdu_dims indices;
  kdu_coords first_idx;
  kdu_dims first_dims;
  
  nominal_size = state->block_partition.size;
  get_valid_blocks(indices);
  first_idx = indices.pos;
  state->codestream->from_apparent(first_idx);
  first_dims = state->block_partition;
  first_dims.pos.x += first_idx.x*first_dims.size.x;
  first_dims.pos.y += first_idx.y*first_dims.size.y;
  first_dims &= state->region;
  assert((!first_dims) || (first_dims.area() > 0));
  first_size = first_dims.size;
  if (state->codestream->transpose)
    {
      nominal_size.transpose();
      first_size.transpose();
    }
}

/*****************************************************************************/
/*                          kdu_subband::open_block                          */
/*****************************************************************************/

kdu_block *
  kdu_subband::open_block(kdu_coords block_idx, int *return_tpart)
{
  state->codestream->from_apparent(block_idx);
  block_idx -= state->region_indices.pos;
  assert((block_idx.x >= 0) && (block_idx.y >= 0) &&
         (block_idx.x < state->region_indices.size.x) &&
         (block_idx.y < state->region_indices.size.y));
  block_idx += state->region_indices.pos; // Back to absolute indices.

  // First find the precinct to which this block belongs and the block index
  // within that precinct.

  kdu_coords precinct_idx = block_idx;
  precinct_idx.x >>= state->log2_blocks_per_precinct.x;
  precinct_idx.y >>= state->log2_blocks_per_precinct.y;

  // Create the precinct if necessary.

  kd_resolution *res = state->resolution;
  precinct_idx -= res->precinct_indices.pos;
  assert((precinct_idx.x >= 0) && (precinct_idx.y >= 0) &&
         (precinct_idx.x < res->precinct_indices.size.x) &&
         (precinct_idx.y < res->precinct_indices.size.y));
  int precinct_num =
    precinct_idx.x + precinct_idx.y*res->precinct_indices.size.x;
  kd_precinct *precinct = res->precinct_refs[precinct_num];
  assert(precinct != KD_EXPIRED_PRECINCT);
  if (precinct == NULL)
    {
      precinct = new kd_precinct(res,precinct_idx);
      assert(precinct == res->precinct_refs[precinct_num]);
    }

  // Load the precinct if necessary.

  kd_codestream *codestream = res->codestream;
  kd_tile *tile = res->tile_comp->tile;
  if (codestream->in != NULL)
    {
      while ((!tile->exhausted) &&
             (precinct->next_layer_idx < precinct->num_layers))
        {
          if ((tile != codestream->active_tile) &&
              !tile->read_tile_part_header())
            {
              assert(!tile->closed); // Otherwise, we could delete ourself.
              tile->finished_reading();
              break; // Can't read any more information for this tile.
            }
          kd_precinct *seq = tile->sequencer->next_in_sequence();
          if ((seq == NULL) || !seq->read_packet())
            tile->read_tile_part_header();
        }
    }

  // Initialize the block structure.

  kdu_dims band_dims = state->block_partition;
  band_dims.pos.x += block_idx.x*band_dims.size.x;
  band_dims.pos.y += block_idx.y*band_dims.size.y;
  band_dims &= state->dims;
  assert(band_dims.area() > 0);

  kd_precinct_band *pband = precinct->bands + state->which_band;
  block_idx -= pband->block_indices.pos;
  assert((block_idx.x >= 0) && (block_idx.y >= 0) &&
         (block_idx.x < pband->block_indices.size.x) &&
         (block_idx.y < pband->block_indices.size.y));

  kdu_block *result = codestream->shared_block;
  codestream->shared_block = NULL;
  assert(result != NULL);
  result->precinct = precinct;
  result->which_block = block_idx.x + block_idx.y*pband->block_indices.size.x;
  kd_block *block = pband->blocks + result->which_block;

  // Set up the common fields (common to input and output codestream objects).

  result->size = band_dims.size;
  result->region = band_dims & state->region;
  result->region.pos -= band_dims.pos;
  result->modes = state->resolution->tile_comp->modes;
  result->orientation = state->which_band;
  result->K_max_prime = state->K_max_prime;

  // Retrieve compressed data, if necessary.

  if (codestream->in != NULL)
    block->retrieve_data(result,tile->num_apparent_layers);
  else if (!block->empty())
    { kdu_error e; e << "Attempting to open the same code-block more than "
      "once for writing!"; }

  if (return_tpart != NULL)
    *return_tpart = precinct->resolution->tile_comp->tile->next_tpart-1;

  return result;
}

/*****************************************************************************/
/*                          kdu_subband::close_block                         */
/*****************************************************************************/

void
  kdu_subband::close_block(kdu_block *result)
{
  kd_precinct *precinct = result->precinct;
  assert(precinct != NULL);
  assert(state->codestream->shared_block == NULL);
  state->codestream->shared_block = result;
  result->precinct = NULL;

  kd_precinct_band *pband = precinct->bands + state->which_band;
  kd_block *block = pband->blocks + result->which_block;
  assert(precinct->num_outstanding_blocks > 0);
  if (state->codestream->in != NULL)
    { // Release the block storage.
      if (!state->codestream->persistent)
        {
          block->cleanup(pband);
          precinct->num_outstanding_blocks--;
          if (precinct->num_outstanding_blocks == 0)
            delete precinct;
        }
      return;
    }

  // If we get here, we have an output block.

  bool trim_storage = false;
  if (state->codestream->stats != NULL)
    trim_storage = state->codestream->stats->update(result);
  assert(block->empty());
  block->store_data(result,state->codestream->buf_server);
  precinct->num_outstanding_blocks--;
  if (trim_storage)
    state->codestream->trim_compressed_data();
}

/*****************************************************************************/
/*               kdu_subband::get_conservative_slope_threshold               */
/*****************************************************************************/

kdu_uint16
  kdu_subband::get_conservative_slope_threshold()
{
  kd_codestream *codestream = state->codestream;
  kdu_uint16 result = 1;
  if (codestream->stats != NULL)
    result = codestream->stats->get_conservative_slope_threshold();
  if (codestream->min_slope_threshold > result)
    result = codestream->min_slope_threshold;
  return result;
}


/* ========================================================================= */
/*                                kd_precinct                                */
/* ========================================================================= */

/*****************************************************************************/
/*                        kd_precinct::kd_precinct                           */
/*****************************************************************************/

kd_precinct::kd_precinct(kd_resolution *resolution, kdu_coords pos_idx)
{
  kd_tile_comp *comp = resolution->tile_comp;
  kd_tile *tile = comp->tile;
  kdu_dims dims, region;

  resolution->codestream->var_structure_new(sizeof(*this));
  this->resolution = resolution;
  this->pos_idx = pos_idx;
  num_layers = tile->num_layers;
  next_layer_idx = 0;
  corrupted = false;
  assert((pos_idx.x < resolution->precinct_indices.size.x) &&
         (pos_idx.y < resolution->precinct_indices.size.y));
  int p = pos_idx.x + resolution->precinct_indices.size.x*pos_idx.y;
  resolution->precinct_refs[p] = this;
  pos_idx += resolution->precinct_indices.pos;
  dims = resolution->precinct_partition;
  dims.pos.x += pos_idx.x*dims.size.x;
  dims.pos.y += pos_idx.y*dims.size.y;
  dims &= resolution->dims;
  region = dims & resolution->region;

  bool discard = false;
  bool persistent = resolution->codestream->persistent;
  if ((!persistent) &&
      ((!region) || (resolution->res_level > comp->apparent_dwt_levels) ||
       (comp->cnum < tile->first_apparent_component) ||
       (comp->cnum >=
        (tile->first_apparent_component+tile->num_apparent_components))))
    {
      assert((dims.size.x>0) && (dims.size.y>0));
      discard = true; // Mark all code-blocks in precinct for discarding.
    }
  
  // Initialize the precinct-bands.

  num_outstanding_blocks = 0;
  total_block_nodes = 0;
  int num_block_nodes;
  for (int b=resolution->min_band; b <= resolution->max_band; b++)
    {
      kd_precinct_band *pb = bands+b;
      kd_subband *subband = resolution->bands+b;
      pb->subband = subband;
      kdu_dims pband_dims =
        (b==LL_BAND)?dims:get_band_dims(dims,subband->band_idx);
      pb->block_indices =
        get_partition_indices(subband->block_partition,pband_dims);
      num_outstanding_blocks += pb->block_indices.area();
      pb->blocks = kd_block::build_tree(pb->block_indices.size,
                                        &num_block_nodes);
      total_block_nodes += num_block_nodes;

      /* Finally, scan through the leaf nodes of the `blocks' array setting
         up the `modes' field and setting `num_passes' to indicate whether
         or not each block's contents can be discarded. */

      // Get location and size of first code-block in the partition which
      // intersects with current precinct-band.
      kdu_dims block_dims = subband->block_partition;
      block_dims.pos.x += block_dims.size.x * pb->block_indices.pos.x;
      block_dims.pos.y += block_dims.size.y * pb->block_indices.pos.y;
      kdu_coords min = block_dims.pos; // So we can keep coming back here.
      kd_block *block = pb->blocks;
      int x, y;
      for (y=0, block_dims.pos.y=min.y;
           y < pb->block_indices.size.y;
           y++, block_dims.pos.y += block_dims.size.y)
        for (x=0, block_dims.pos.x=min.x;
             x < pb->block_indices.size.x;
             x++, block_dims.pos.x += block_dims.size.x, block++)
          {
            block->set_modes(resolution->tile_comp->modes);
            if ((!persistent) && (discard || !(block_dims & subband->region)))
              block->set_discard();
          }
    }
  resolution->codestream->var_structure_new(sizeof(kd_block)*total_block_nodes);
  packet_bytes = NULL; // Create this array if and when we need it.
}

/*****************************************************************************/
/*                        kd_precinct::~kd_precinct                          */
/*****************************************************************************/

kd_precinct::~kd_precinct()
{
  resolution->codestream->var_structure_delete(sizeof(*this));
  for (int b=resolution->min_band; b <= resolution->max_band; b++)
    {
      kd_precinct_band *pb = bands+b;
      if (pb->blocks != NULL)
        {
          int num_blocks = pb->block_indices.area();
          for (int n=0; n < num_blocks; n++)
            pb->blocks[n].cleanup(pb);
          delete[] pb->blocks;
        }
    }
  resolution->codestream->var_structure_delete(sizeof(kd_block) *
                                               total_block_nodes);

  if (packet_bytes != NULL)
    delete[] packet_bytes;

  int precinct_num = pos_idx.x + pos_idx.y*resolution->precinct_indices.size.x;
  assert(resolution->precinct_refs[precinct_num] == this);
  resolution->precinct_refs[precinct_num] = KD_EXPIRED_PRECINCT;
}

/*****************************************************************************/
/*                        kd_precinct::read_packet                           */
/*****************************************************************************/

bool
  kd_precinct::read_packet()
  /* For the purpose of error resilience, we can focus on just this one
     function, since an error resilient code-stream should probably not
     contain any more than one tile-part and one tile.  Otherwise, we
     would have to deal with the problem of specially protecting marker
     codes which may appear in tile-part headers. */
{
  kd_codestream *codestream = resolution->codestream;
  kd_tile *tile = resolution->tile_comp->tile;
  assert(next_layer_idx < num_layers);
  assert(tile == codestream->active_tile);
  assert(next_layer_idx < num_layers);

  bool use_sop = resolution->tile_comp->tile->use_sop;
  bool use_eph = resolution->tile_comp->tile->use_eph;
  bool tpart_ends = false;
  bool seek_marker =
    use_sop && codestream->expect_ubiquitous_sops && codestream->resilient;

  if (corrupted ||
      (tile->skipping_to_sop &&
       (compare_sop_num(tile->next_sop_sequence_num,
                        tile->num_transferred_packets) != 0)))
    return handle_corrupt_packet();
  if (tile->skipping_to_sop)
    { // We have already read the SOP marker for this packet.
      assert(compare_sop_num(tile->next_sop_sequence_num,
                             tile->num_transferred_packets) == 0);
      tile->skipping_to_sop = false;
    }
  else
    while (codestream->marker->read(true,seek_marker))
      {
        if (codestream->marker->get_code() == KDU_SOT)
          { tpart_ends = true; break; }
        else if (use_sop &&
                 (codestream->marker->get_code() == KDU_SOP))
          { // Check the validity of the SOP marker.
            int sequence_num = codestream->marker->get_bytes()[0];
            sequence_num <<= 8;
            sequence_num += codestream->marker->get_bytes()[1];
            if (compare_sop_num(sequence_num,tile->num_transferred_packets)!=0)
              {
                if (!codestream->resilient)
                  { kdu_error e;
                    e << "Out-of-sequence SOP marker found while "
                         "attempting to read a packet from the code-stream!\n";
                    e << "\tFound sequence number "
                      << sequence_num << ", but expected "
                      << (tile->num_transferred_packets & 0x0000FFFF) << ".\n";
                    e << "Use the resilient option if you would like to try "
                         "to recover from this error.";
                  }
                else
                  {
                    tile->skipping_to_sop = true;
                    tile->next_sop_sequence_num = sequence_num;
                    return handle_corrupt_packet();
                  }
              }
            break;
          }
        else if (!seek_marker) // If `expect_marker' is true we will loop back
          { 
            if (!codestream->resilient)
              { kdu_error e;
                e << "Illegal marker code found while attempting to "
                     "read a packet from the code-stream!\n";
                e << "\tIllegal marker code is ";
                codestream->marker->print_current_code(e); e << ".\n";
                e << "Use the resilient option if you would like to try to "
                     "recover from this error.";
              }
            return handle_corrupt_packet();
          }
      } // End of marker seeking loop.

  if (codestream->in->failed())
    {
      tile->finished_reading();
      return false;
    }

  if (tpart_ends)
    {
      codestream->active_tile = NULL;
      return false;
    }

  // Now read the packet header.

  kd_input *header_source = tile->packed_headers;
  if (header_source == NULL)
    header_source = codestream->in;
  kd_header_in header(header_source);
  int body_bytes = 0;
  if (codestream->resilient)
    header_source->enable_marker_throwing();
  else if (codestream->fussy)
    header_source->enable_marker_throwing(true);
  try {
      if (header.get_bit() != 0)
        for (int b=resolution->min_band; b <= resolution->max_band; b++)
          {
            kd_precinct_band *pband = &(bands[b]);
            kd_block *block = pband->blocks;
            for (int m=pband->block_indices.size.y; m > 0; m--)
              for (int n=pband->block_indices.size.x; n > 0; n--, block++)
                body_bytes +=
                  block->parse_packet_header(header,codestream->buf_server,
                                             next_layer_idx);
          }
      header.finish(); // Clears any terminal FF
    }
  catch (kd_header_in *)
    { // Insufficient header bytes in source.
      if (header_source == tile->packed_headers)
        {
          assert(tile->packed_headers->failed());
          kdu_error e; e << "Exhausted PPM/PPT marker segment data while "
            "attempting to parse a packet header!";
        }
      assert(codestream->in->failed());
      tile->finished_reading();
      return false;
    }
  catch (kdu_uint16 code)
    {
      if (codestream->resilient && (header_source == codestream->in))
        { // Assume that all exceptions arise as a result of corruption.
          return handle_corrupt_packet();
        }
      else if (code == KDU_EXCEPTION_PRECISION)
        { kdu_error e;
          e << "Packet header contains a representation which is "
          "not strictly illegal, but unreasonably large so that it exceeds "
          "the dynamic range available for our internal representation!  The "
          "problem is most likely due to a corrupt or incorrectly constructed "
          "code-stream.";
        }
      else if (code == KDU_EXCEPTION_ILLEGAL_LAYER)
        { kdu_error e;
          e << "Illegal inclusion tag tree encountered while decoding "
          "a packet header.  This problem can arise if empty packets are "
          "used (i.e., packets whose first header bit is 0) and the "
          "value coded by the inclusion tag tree in a subsequent packet "
          "is not exactly equal to the index of the quality layer in which "
          "each code-block makes its first contribution.  Such an "
          "error may arise from a mis-interpretation of the standard.  "
          "The problem may also occur as a result of a corrupted code-stream.";
        }
      else
        { kdu_error e;
          e << "Illegal marker code, ";
          print_marker_code(code,e);
          e << ", found while reading packet header.";
        }
    }
  if (!header_source->disable_marker_throwing())
    {
      assert(0); // If this happens, `header.finish()' did not execute properly
    }

  // Next, read any required EPH marker.

  if (use_eph)
    {
      kdu_byte byte;
      kdu_uint16 code;

      code = 0;
      if (header_source->get(byte))
        code = byte;
      if (header_source->get(byte))
        code = (code<<8) + byte;
      if (header_source->failed())
        {
          if (header_source == tile->packed_headers)
            {
              assert(tile->packed_headers->failed());
              kdu_error e; e << "Exhausted PPM/PPT marker segment data while "
                "attempting to parse a packet header!";
            }
          assert(codestream->in->failed());
          tile->finished_reading();
          return false;
        }
      if (code != KDU_EPH)
        {
          if (codestream->resilient && (header_source == codestream->in))
            { // Missing EPH is a clear indicator of corruption.
              return handle_corrupt_packet();
            }
          else
            { kdu_error e;
              e << "Expected to find EPH marker following packet header.  "
                << "Found ";
              print_marker_code(code,e);
              e << " instead.";
            }
        }
    }

  // Finally, read the body bytes.

  if (body_bytes > 0)
    {
      if (codestream->resilient)
        codestream->in->enable_marker_throwing();
      else if (codestream->fussy)
        codestream->in->enable_marker_throwing(true);
      try {
          for (int b=resolution->min_band; b <= resolution->max_band; b++)
            {
              kd_precinct_band *pband = &(bands[b]);
              kd_block *block = pband->blocks;
              for (int m=pband->block_indices.size.y; m > 0; m--)
                for (int n=pband->block_indices.size.x; n > 0; n--, block++)
                  block->read_body_bytes(codestream->in,
                                         codestream->buf_server);
            }
        }
      catch (kdu_uint16 code)
        {
          if (codestream->resilient)
            { // We have run into the next SOP or SOT marker.
              return handle_corrupt_packet();
            }
          else
            { kdu_error e;
              e << "Illegal marker code, ";
              print_marker_code(code,e);
              e << ", found while reading packet body.";
            }
        }
    }
  if (!codestream->in->disable_marker_throwing())
    {
      if (codestream->resilient)
        { // We probably just ran into the 1'st byte of the next SOP/SOT marker
          codestream->in->putback((kdu_byte) 0xFF);
          return handle_corrupt_packet();
        }
      else
        { kdu_error e;
          e << "Packet body terminated with an FF!";
        }
    }
  
  next_layer_idx++;
  tile->num_transferred_packets++;
  if (tile->num_transferred_packets == tile->total_packets)
    tile->finished_reading();
  return true;
}

/*****************************************************************************/
/*                   kd_precinct::handle_corrupt_packet                      */
/*****************************************************************************/

bool
  kd_precinct::handle_corrupt_packet()
{
  kd_tile *tile = resolution->tile_comp->tile;
  kd_codestream *codestream = tile->codestream;
  bool expect_large_gap = !codestream->expect_ubiquitous_sops;
      // It is reasonable to expect large (unbounded) gaps between valid
      // SOP marker sequence numbers if SOP markers are not known to be
      // in front of every packet.
  bool confirm_large_gap = expect_large_gap;

  corrupted = true;
  do { // Read up to next valid SOP marker, if this has not already been done.
      if (!tile->skipping_to_sop)
        { // Need to read next SOP marker.
          do {
              if (!codestream->marker->read(true,true))
                { // Must have exhausted the code-stream.
                  assert(codestream->in->failed());
                  tile->finished_reading();
                  return false;
                }
              if (codestream->marker->get_code() == KDU_SOT)
                { // We have encountered the end of the current tile-part.
                  codestream->active_tile = NULL;
                  return false;
                }
            } while (codestream->marker->get_code() != KDU_SOP);
          tile->next_sop_sequence_num = codestream->marker->get_bytes()[0];
          tile->next_sop_sequence_num<<=8;
          tile->next_sop_sequence_num += codestream->marker->get_bytes()[1];
          tile->skipping_to_sop = true;
        }
      assert(tile->skipping_to_sop);
      if (compare_sop_num(tile->next_sop_sequence_num,
                          tile->num_transferred_packets) <= 0)
        { // If equal, the SOP marker must be for a previously corrupt precinct
          tile->skipping_to_sop = false; // Force hunt for another SOP marker.
          confirm_large_gap = expect_large_gap;
        }
      else if ((compare_sop_num(tile->next_sop_sequence_num,
                               tile->num_transferred_packets) > 3) &&
               !confirm_large_gap)
        { // Unwilling to skip so many packets without confirmation that
          // the SOP sequence number itself has not been corrupted.
          tile->skipping_to_sop = false; // Skip another one to make sure.
          confirm_large_gap = true;
        }
      else if (compare_sop_num(tile->next_sop_sequence_num,
                               tile->total_packets) >= 0)
        { // Assume that the SOP marker itself has a corrupt sequence number
          tile->skipping_to_sop = false; // Force hunt for another SOP marker.
          confirm_large_gap = expect_large_gap;
        }
    } while (!tile->skipping_to_sop);

  // Update state to indicate a transferred packet.
  assert(next_layer_idx < num_layers);
  next_layer_idx++;
  tile->num_transferred_packets++;
  return true;
}

/*****************************************************************************/
/*                       kd_precinct::simulate_packet                        */
/*****************************************************************************/

int
  kd_precinct::simulate_packet(int &header_bytes, int layer_idx,
                               kdu_uint16 slope_threshold, bool finalize_layer,
                               bool last_layer, int max_bytes,
                               bool trim_to_limit)
{
  kd_tile *tile = resolution->tile_comp->tile;
  kd_buf_server *buf_server = resolution->codestream->buf_server;
  assert(layer_idx < num_layers);
  if (this->num_outstanding_blocks != 0)
    { kdu_error e; e << "You may not currently flush compressed "
      "code-stream data without completing the compression "
      "of all code-blocks in all precincts of all tiles."; }
  if (packet_bytes == NULL)
    {
      assert(next_layer_idx == 0);
      packet_bytes = new int[num_layers];
    }
  if (layer_idx == 0)
    { // Assign each packet the smallest legal empty packet size by default.
      for (int n=0; n < num_layers; n++)
        packet_bytes[n] = (tile->use_eph)?3:1;
    }
  packet_bytes[layer_idx] = 0; // Mark packet so we can catch any failure later

  if (trim_to_limit)
    { /* To make the code below work efficiently, we need to first trim away
         all coding passes which have slopes <= `slope_threshold'.  This
         only leaves coding passes which have slopes equal to `slope_threshold'
         to consider for further discarding. */
      assert(last_layer && finalize_layer);
      for (int b=resolution->min_band; b <= resolution->max_band; b++)
        {
          kd_precinct_band *pband = bands + b;
          int num_blocks = pband->block_indices.area();
          for (int n=0; n < num_blocks; n++)
            pband->blocks[n].trim_data(slope_threshold,buf_server);
        }
    }

  int last_trimmed_subband = resolution->max_band+1;
  int last_trimmed_block = 0;
  int body_bytes;

  do { // Iterates only if we are trimming blocks to meet a `max_bytes' target
      body_bytes = 0;
      header_bytes = 1; // Minimum possible header size.
      if (tile->use_sop)
        header_bytes += 6;
      if (tile->use_eph)
        header_bytes += 2;
      // Run the packet start functions for this packet
      int b;
      for (b=resolution->min_band; b <= resolution->max_band; b++)
        {
          kd_precinct_band *pband = bands + b;
          if (layer_idx == 0)
            kd_block::reset_output_tree(pband->blocks,
                                        pband->block_indices.size);
          else
            kd_block::restore_output_tree(pband->blocks,
                                          pband->block_indices.size);
          int num_blocks = pband->block_indices.area();
          for (int n=0; n < num_blocks; n++)
            body_bytes +=
              pband->blocks[n].start_packet(layer_idx,slope_threshold);
          if ((body_bytes+header_bytes) > max_bytes)
            {
              if (!finalize_layer)
                return body_bytes+header_bytes;
              else
                assert(trim_to_limit);
            }
        }

      // Simulate header construction for this packet
      kd_header_out head(NULL);
      head.put_bit(1); // Packet not empty.
      for (b=resolution->min_band; b <= resolution->max_band; b++)
        {
          kd_precinct_band *pband = bands + b;
          int num_blocks = pband->block_indices.area();
          for (int n=0; n < num_blocks; n++)
            pband->blocks[n].write_packet_header(head,layer_idx,true);
        }
      header_bytes += head.finish() - 1; // Already counted the 1'st byte above

      if ((body_bytes+header_bytes) > max_bytes)
        {
          if (!finalize_layer)
            return body_bytes+header_bytes;
          assert(trim_to_limit);

          // If we get here, we need to trim away some code-block contributions

          bool something_discarded = false;
          while (!something_discarded)
            {
              kd_precinct_band *pband = bands + last_trimmed_subband;
              if (last_trimmed_block == 0)
                {
                  last_trimmed_subband--; pband--;
                  assert(last_trimmed_subband >= resolution->min_band);
                    // Otherwise, the function is being used incorrectly.
                  last_trimmed_block = pband->block_indices.area();
                  continue; // In case precinct-band contains no code-blocks
                }
              last_trimmed_block--;
              kd_block *block = pband->blocks + last_trimmed_block;
              something_discarded =
                block->trim_data(slope_threshold+1,buf_server);
            }
        }
      else if (finalize_layer)
        { // Save the state information in preparation for output.
          for (b=resolution->min_band; b <= resolution->max_band; b++)
            {
              kd_precinct_band *pband = bands + b;
              kd_block::save_output_tree(pband->blocks,
                                         pband->block_indices.size);
            }
        }
    } while ((body_bytes+header_bytes) > max_bytes);

  // Record packet length information.
  packet_bytes[layer_idx] = body_bytes + header_bytes;
  return body_bytes + header_bytes;
}

/*****************************************************************************/
/*                         kd_precinct::write_packet                         */
/*****************************************************************************/

int
  kd_precinct::write_packet(kdu_uint16 threshold, bool empty_packet)
{
  int b, n, num_blocks;
  int check_bytes = 0;
  kd_tile *tile = resolution->tile_comp->tile;
  kdu_output *out = resolution->codestream->out;

  if (!empty_packet)
    { // Start the packets.
      for (b=resolution->min_band; b <= resolution->max_band; b++)
        {
          kd_precinct_band *pband = bands + b;
          if (next_layer_idx == 0)
            kd_block::reset_output_tree(pband->blocks,
                                        pband->block_indices.size);
          num_blocks = pband->block_indices.area();
          for (n=0; n < num_blocks; n++)
            check_bytes +=
            pband->blocks[n].start_packet(next_layer_idx,threshold);
        }
      if (resolution->tile_comp->tile->use_sop)
        { // Generate an SOP marker.
          check_bytes += out->put(KDU_SOP);
          check_bytes += out->put((kdu_uint16) 4);
          check_bytes += out->put((kdu_uint16) tile->num_transferred_packets);
        }
    }

  kd_header_out head(out);
  if (empty_packet)
    head.put_bit(0); // Empty packet bit.
  else
    {
      head.put_bit(1);
      for (b=resolution->min_band; b <= resolution->max_band; b++)
        {
          kd_precinct_band *pband = bands + b;
          num_blocks = pband->block_indices.area();
          for (n=0; n < num_blocks; n++)
            pband->blocks[n].write_packet_header(head,next_layer_idx,false);
        }
    }
  check_bytes += head.finish();
  if (resolution->tile_comp->tile->use_eph)
    check_bytes += out->put(KDU_EPH);
  if (!empty_packet)
    for (b=resolution->min_band; b <= resolution->max_band; b++)
      {
        kd_precinct_band *pband = bands + b;
        num_blocks = pband->block_indices.area();
        for (n=0; n < num_blocks; n++)
          pband->blocks[n].write_body_bytes(out);
      }

  assert(check_bytes == packet_bytes[next_layer_idx]); // Simulation correct?
  next_layer_idx++;
  tile->num_transferred_packets++;
  return check_bytes;
}
