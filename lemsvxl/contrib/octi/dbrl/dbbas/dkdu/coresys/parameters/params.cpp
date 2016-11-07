/*****************************************************************************/
// File: params.cpp [scope = CORESYS/PARAMETERS]
// Version: Kakadu, V2.2.3
// Author: David Taubman
// Last Revised: 27 July, 2001
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
   Implements the services defined by "kdu_params.h".
******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "kdu_messaging.h"
#include "kdu_utils.h"
#include "kdu_params.h"
#include "kdu_kernels.h"
#include "kdu_compressed.h"
#include "params_local.h"

using namespace std;

/* ========================================================================= */
/*            Initialization of static const class members                   */
/* ========================================================================= */

const int kdu_params::MULTI_RECORD=1;
const int kdu_params::CAN_EXTRAPOLATE = 2;
const int kdu_params::ALL_COMPONENTS=4;


/* ========================================================================= */
/*                             Internal Functions                            */
/* ========================================================================= */

/*****************************************************************************/
/* STATIC                        step_to_eps_mu                              */
/*****************************************************************************/

static void
  step_to_eps_mu(float val, int &eps, int &mu)
  /* Finds `eps' and `mu' such that `val' ~ 2^{-`eps'}*(1+2^{-11}`mu'). */
{
  if (val <= 0.0F)
    { kdu_error e; e << "Absolute quantization step sizes must be "
      "strictly positive."; }
  for (eps=0; val < 1.0F; eps++)
    val *= 2.0F;
  mu = (int) floor(0.5F+((val-1.0F)*(1<<11)));
  if (mu >= (1<<11))
    { mu = 0; eps--; }
  if (eps > 31)
    { eps = 31; mu = 0; }
  if (eps < 0)
    { eps = 0; mu = (1<<11)-1; }
}

/*****************************************************************************/
/* STATIC                          find_lcm                                  */
/*****************************************************************************/

static int
  find_lcm(int m, int n)
  /* Returns the LCM (Least Common Multiple) of the two supplied integers.
     Both must be strictly positive. */
{
  assert((m > 0) && (n > 0));

  /* The idea is to remove all common factors of m and n from m and
     count them only once, in a `common_part'. */

  int common_part = 1;
  int divisor = 2;
  for (; (divisor <= m) && (divisor <= n); divisor++)
    {
      while (((m % divisor) == 0) && ((n % divisor) == 0))
        {
          common_part *= divisor;
          m /= divisor;
          n /= divisor;
        }
    }
  return m*n*common_part;
}

/*****************************************************************************/
/* STATIC                    parse_translator_entry                          */
/*****************************************************************************/

static char const *
  parse_translator_entry(char const *start, char separator,
                         char buf[], int len, int &value)
  /* Attempts to parse a translator entry from the supplied string. Translator
     entries consist of a text string, followed by an '=' sign and an integer
     value. Successive entries are delimited by the character identified by
     `separator', which is normally either a comma or the '|' symbol.
     The function generates an error if a valid entry cannot be found.
     Otherwise, it places the string identifier in the supplied buffer and
     the value through the `value' reference, returning a pointer to the
     delimiting character. This delimiting character is either the separator
     itself, or one of the characters, ')' or ']'. Note that identifiers whose
     length exceeds `len'-1 will cause an error to be generated. */
{
  int i;

  for (i=0; i < len; i++)
    {
      if ((start[i] == separator) || (start[i] == ')') || (start[i] == ']') ||
          (start[i] == '\0'))
        { kdu_error e;
          e << "String translators in code-stream attribute "
            "specifications must contain an '=' sign! Problem "
            "encountered at: \"" << start << "\".";
        }
      else if (start[i] == '=')
        break;
      buf[i] = start[i];
    }
  if (i == len)
    { kdu_error e;
      e << "String translators in code-stream attribute "
        "specifications may not exceed " << len-1 << " characters "
        "in length! Problem encountered at: \"" << start << "\".";
    }
  buf[i] = '\0';
  start += i+1;

  char *end_p;

  value = (int) strtol(start,&end_p,10);
  if ((end_p == start) ||
      ((*end_p != separator) && (*end_p != ')') && (*end_p != ']')))
    { kdu_error e;
      e << "String translators in code-stream attribute "
        "specifications must be identified with integers and correctly "
        "delimited! Problem encountered at: \"" << start << "\".";
    }
  return(end_p);
}

/*****************************************************************************/
/* STATIC                       display_options                              */
/*****************************************************************************/

static void
  display_options(char const *pattern, ostream &stream)
{
  char buf1[80], buf2[80], *bp, *obp, *last_bp;
  int val;
  bool multi_option;

  bp = buf1; obp = buf2; last_bp = NULL;
  multi_option = false;
  if (*pattern == '(')
    {
      stream << "Expect one of the identifiers, ";
      do {
          pattern = parse_translator_entry(pattern+1,',',bp,80,val);
          if (multi_option)
            stream << ", ";
          if (last_bp != NULL)
            { stream << "\"" << last_bp << "\""; multi_option = true; }
          last_bp = bp; bp = obp; obp = last_bp;
        } while (*pattern == ',');
      assert(*pattern == ')');
      if (multi_option)
        stream << " or ";
      stream << "\"" << last_bp << "\".";
    }
  else if (*pattern == '[')
    {
      stream << "Expect one or more of the identifiers, ";
      do {
          pattern = parse_translator_entry(pattern+1,'|',bp,80,val);
          if (multi_option)
            stream << ", ";
          if (last_bp != NULL)
            { stream << "\"" << last_bp << "\""; multi_option = true; }
          last_bp = bp; bp = obp; obp = last_bp;
        } while (*pattern == '|');
      assert(*pattern == ']');
      if (multi_option)
        stream << " or ";
      stream << "\"" << last_bp << "\", separated by `|' symbols.";
    }
  else
    assert(0);
}

/*****************************************************************************/
/* STATIC                           int2log                                  */
/*****************************************************************************/

static int
  int2log(int val)
  /* Returns the integer exponent, e, such that 2^e=val. Returns -1 if
     such an integer does not exist. */
{
  int e;

  for (e=0; ((1<<e) > 0) && ((1<<e) < val); e++);
  if (val != (1<<e))
    return 0;
  return e;
}

/*****************************************************************************/
/* STATIC                    synthesize_canvas_size                          */
/*****************************************************************************/

static bool
  synthesize_canvas_size(int components, int dims[], int origin, int &size)
  /* Determines a canvas size which is compatible with the supplied
     component component dimensions and image origin.  Specifically, we must
     have dims[n] = ceil(size/subs[n])-ceil(origin/subs[n]) for each some
     set of valid component sub-sampling factors, subs[n].  If this is not
     possible within the constraint that sub-sampling factors are no greater
     than 255, the function returns false; otherwise, the function returns
     true, with the compatible size parameter in `size'. The function
     basically conducts an intelligent (but still somewhat tedious)
     search through the parameter space. */

{
  int ref_dim, ref_sub, n;

  for (ref_dim=dims[0], n=1; n < components; n++)
    if (dims[n] > ref_dim)
      ref_dim = dims[n];
  for (ref_sub=1; ref_sub <= 255; ref_sub++)
    { // Reference component has largest dimension and smallest sub-sampling
      int max_size = (ceil_ratio(origin,ref_sub)+ref_dim)*ref_sub;
      int min_size = max_size - ref_sub + 1;
      for (n=0; n < components; n++)
        {
          int sz, last_sz, sub;
          
          // Find smallest sub-sampling factor compatible with existing
          // minimum canvas size and use this to deduce new minimum size.
          sub = (min_size - origin) / dims[n]; // Roughly smallest factor.
          if (sub <= 0)
            sub = 1;
          while ((sub > 1) &&
                 (((ceil_ratio(origin,sub)+dims[n])*sub) > min_size))
            sub--;
          while ((sz=(ceil_ratio(origin,sub)+dims[n])*sub) < min_size)
            sub++;
          if ((sz-sub+1) > min_size)
            min_size = sz-sub+1;

          if (min_size > max_size)
            break; // Current reference sub-sampling factor cannot work.

          // Find largest sub-sampling factor compatible with existing
          // maximum canvas size and use this to deduce new maximum size.

          do {
              last_sz = sz;
              if (sub == 255)
                break;
              sub++;
              sz = (ceil_ratio(origin,sub)+dims[n])*sub;
            } while ((sz-sub+1) <= max_size);
          if (last_sz < max_size)
            max_size = last_sz;
          
          if (min_size > max_size)
            break; // Current reference sub-sampling factor cannot work.
        }
      if (n == components)
        { // Succeeded.
          size = min_size;
          return true;
        }
    }
  return false; // Failed to find compatible sub-sampling factors.
}

/*****************************************************************************/
/* STATIC                    derive_absolute_steps                           */
/*****************************************************************************/

static void
  derive_absolute_steps(qcd_params *qcd, int num_dwt_levels, int kernel_id,
                        float ref_step, bool derived_from_LL)
  /* This function determines a suitable set of scalar quantization step
     sizes for the irreversible path, based on the number of DWT levels,
     the DWT kernel type (one of `Cparams_W9X7' or `Cparams_W5X3')
     and a base quantization step size.  If `derived_from_LL' is true, only
     the LL band's step size will be determined and set into the supplied
     `qcd' object.  Otherwise, separate step sizes will be determined for
     each subband and set into the `qcd' object. */

{
  if (num_dwt_levels == 0)
    {
      qcd->set(Qabs_steps,0,0,ref_step);
      return;
    }

  kdu_kernels kernels(kernel_id,false);
  int lev, band_idx;
  for (band_idx=0, lev=num_dwt_levels; lev > 0; lev--)
    {
      double low_1d = kernels.get_energy_gain(KDU_SYNTHESIS_LOW,lev);
      double high_1d = kernels.get_energy_gain(KDU_SYNTHESIS_HIGH,lev);
      if (band_idx==0)
        qcd->set(Qabs_steps,band_idx++,0,ref_step/low_1d); // LL subband.
      if (derived_from_LL)
        break;
      qcd->set(Qabs_steps,band_idx++,0,ref_step/sqrt(low_1d*high_1d));
      qcd->set(Qabs_steps,band_idx++,0,ref_step/sqrt(low_1d*high_1d));
      qcd->set(Qabs_steps,band_idx++,0,ref_step/high_1d);
    }
}


/* ========================================================================= */
/*                                 kd_attribute                              */
/* ========================================================================= */

/*****************************************************************************/
/*                         kd_attribute::kd_attribute                        */
/*****************************************************************************/

kd_attribute::kd_attribute(const char *name, const char *comment,
                           int flags, const char *pattern)
{
  char const *ch;

  this->name = name;
  this->comment = comment;
  this->flags = flags;
  this->pattern = pattern;
  for (ch=pattern, num_fields=0; *ch != '\0'; ch++, num_fields++)
    {
      if ((*ch == 'F') || (*ch == 'B') || (*ch == 'I'))
        continue;

      char term = '\0';
      if (*ch == '(')
        term = ')';
      else if (*ch == '[')
        term = ']';
      for (ch++; (*ch != term) && (*ch != '\0'); ch++);
      if (*ch == '\0')
        throw pattern;
    }
  num_records = 0;
  max_records = 1;
  this->values = new att_val[max_records*num_fields];
  for (ch=pattern, num_fields=0; *ch != '\0'; ch++, num_fields++)
    {
      values[num_fields].pattern = ch;
      if ((*ch == 'F') || (*ch == 'B') || (*ch == 'I'))
        continue;
      char term = '\0';
      if (*ch == '(')
        term = ')';
      else if (*ch == '[')
        term = ']';
      for (ch++; (*ch != term) && (*ch != '\0'); ch++);
    }
  derived = false;
  parsed = false;
  next = NULL;
}

/*****************************************************************************/
/*                       kd_attribute::augment_records                       */
/*****************************************************************************/

void
  kd_attribute::augment_records(int new_records)
{
  if (new_records <= num_records)
    return;
  if (max_records < new_records)
    { // Need to allocate more storage.
      if (!(flags & kdu_params::MULTI_RECORD))
        { kdu_error e;
          e << "Attempting to write multiple records to a code-stream "
            "attribute, \"" << name << "\", which can "
            "accept only single attributes!";
        }

      int alloc_records = max_records + new_records;
      att_val *alloc_vals = new att_val[alloc_records*num_fields];
      int rec, fld;
      att_val *new_p, *old_p;

      for (new_p=alloc_vals, old_p=values, rec=0; rec < max_records; rec++)
        for (fld=0; fld < num_fields; fld++)
          *(new_p++) = *(old_p++);
      for (; rec < alloc_records; rec++)
        for (old_p -= num_fields, fld=0; fld < num_fields; fld++)
          { *new_p = *(old_p++); new_p->is_set = false; new_p++; }
      delete[] values;
      values = alloc_vals;
      max_records = alloc_records;
    }
  num_records = new_records;
}

/*****************************************************************************/
/*                          kd_attribute::describe                           */
/*****************************************************************************/

void
  kd_attribute::describe(ostream &stream, bool allow_tiles, bool allow_comps,
                          bool include_comments)
{
  char locators[3];
  int loc_pos = 0;
  if (allow_tiles)
    locators[loc_pos++] = 'T';
  if (allow_comps && !(flags & kdu_params::ALL_COMPONENTS))
    locators[loc_pos++] = 'C';
  locators[loc_pos++] = '\0';
      
  if (loc_pos < 2)
    stream << name << "={";
  else
    stream << name << "[:<" << locators << ">]={";
  for (int fnum=0; fnum < num_fields; fnum++)
    {
      if (fnum != 0)
        stream << ",";
      char const *cp = values[fnum].pattern;
      assert(cp != NULL);
      if (*cp == 'I')
        stream << "<int>";
      else if (*cp == 'B')
        stream << "<yes/no>";
      else if (*cp == 'F')
        stream << "<float>";
      else if (*cp == '(')
        {
          char buf[80];
          int val;

          stream << "ENUM<";
          do {
              cp = parse_translator_entry(cp+1,',',buf,80,val);
              stream << buf;
              if (*cp == ',')
                stream << *cp;
            } while (*cp == ',');
          stream << ">";
        }
      else if (*cp == '[')
        {
          char buf[80];
          int val;

          stream << "FLAGS<";
          do {
              cp = parse_translator_entry(cp+1,'|',buf,80,val);
              stream << buf;
              if (*cp == '|')
                stream << *cp;
            } while (*cp == '|');
          stream << ">";
        }
    }
      
  stream << "}";
  if (flags & kdu_params::MULTI_RECORD)
    stream << ",...\n";
  else
    stream << "\n";
  if (include_comments)
    stream << "\t" << comment << "\n";
}


/* ========================================================================= */
/*                                 kdu_params                                */
/* ========================================================================= */

/*****************************************************************************/
/*                            kdu_params::kdu_params                         */
/*****************************************************************************/

kdu_params::kdu_params(const char *cluster_name, bool allow_tiles,
                     bool allow_comps, bool allow_insts)
{
  this->cluster_name = cluster_name;
  this->tile_idx = -1;
  this->comp_idx = -1;
  this->inst_idx = 0;
  this->num_comps = 0;
  this->allow_tiles = allow_tiles;
  this->allow_comps = allow_comps;
  this->allow_insts = allow_insts;
  this->attributes = NULL;
  this->empty = true;
  this->marked = false;

  first_cluster = this; next_cluster = NULL;
  first_tile = this; next_tile = NULL;
  first_comp = this; next_comp = NULL;
  first_inst = this; next_inst = NULL;
}

/*****************************************************************************/
/*                          kdu_params::~kdu_params                          */
/*****************************************************************************/

kdu_params::~kdu_params()
{
  kd_attribute *att;
  kdu_params *csp;

  while ((att=attributes) != NULL)
    {
      attributes = att->next;
      delete(att);
    }

  // Process instance list

  if (first_inst == NULL)
    return; // Function being used to delete all elements of instance list.
  if (this != first_inst)
    { // Relink instance list.
      for (csp=first_inst; this != csp->next_inst; csp=csp->next_inst);
      csp->next_inst = this->next_inst;
      return;
    }
  while ((csp=next_inst) != NULL)
    { // Delete rest of instance list.
      next_inst = csp->next_inst;
      csp->first_inst = NULL; // Prevent further instance list processing.
      delete csp;
    }

  // Process component list

  assert(this == first_inst);
  if (first_comp == NULL)
    return; // Function being used to delete all elements of a component list.
  if (this != first_comp)
    { // Relink component list.
      for (csp=first_comp; this != csp->next_comp; csp=csp->next_comp);
      csp->next_comp = this->next_comp;
      return;
    }
  while ((csp=next_comp) != NULL)
    { // Delete rest of component list.
      next_comp = csp->next_comp;
      csp->first_comp = NULL; // Prevent further component list processing.
      delete csp;
    }

  // Process tile list

  assert(this == first_comp);
  if (first_tile == NULL)
    return; // Function being used to delete all elements of a tile list.
  if (this != first_tile)
    { // Relink tile list.
      for (csp=first_tile; this != csp->next_tile; csp=csp->next_tile);
      csp->next_tile = this->next_tile;
      return;
    }
  while ((csp=next_tile) != NULL)
    { // Delete rest of tile list.
      next_tile = csp->next_tile;
      csp->first_tile = NULL; // Prevent further tile list processing.
      delete csp;
    }

  // Process cluster list

  assert(this == first_tile);
  if (first_cluster == NULL)
    return; // Function being used to delete all elements of the cluster list.
  if (this != first_cluster)
    { // Relink cluster list.
      for (csp=first_cluster; this != csp->next_cluster; csp=csp->next_cluster);
      csp->next_cluster = this->next_cluster;
      return;
    }
  while ((csp=next_cluster) != NULL)
    { // Delete entire cluster list.
      next_cluster = csp->next_cluster;
      csp->first_cluster = NULL; // Prevent further cluster list processing.
      delete csp;
    }
}

/*****************************************************************************/
/*                               kdu_params::link                            */
/*****************************************************************************/

kdu_params *
  kdu_params::link(kdu_params *existing, int tile_idx, int comp_idx)
{
  kdu_params *cluster, *prev_cluster;

  assert((this->tile_idx == -1) && (this->comp_idx == -1) &&
         (this->inst_idx == 0));
  this->tile_idx = tile_idx;
  this->comp_idx = comp_idx;
  assert((tile_idx >= -1) && (comp_idx >= -1));

  if (!allow_tiles)
    assert(tile_idx < 0);
  if (!allow_comps)
    assert(comp_idx < 0);

  cluster=existing->first_inst->first_comp->first_tile->first_cluster;
  for (prev_cluster=NULL;
       cluster != NULL;
       prev_cluster=cluster, cluster=cluster->next_cluster)
    if (strcmp(cluster->cluster_name,this->cluster_name) == 0)
      break;
  if (cluster == NULL)
    { /* Link as head of new cluster. */
      assert((tile_idx == -1) && (comp_idx == -1)); /* Cluster head must have
        component and tile indices of -1. */
      if (prev_cluster != NULL)
        {
          this->first_cluster = prev_cluster->first_cluster;
          this->next_cluster = prev_cluster->next_cluster;
          prev_cluster->next_cluster = this;
        }
      else
        {
          this->first_cluster = this;
          this->next_cluster = NULL;
        }
      return this;
    }

  kdu_params *tile, *prev_tile;

  this->first_cluster = this->next_cluster = NULL;
  for (tile=cluster, prev_tile=NULL;
       (tile != NULL) && (tile->tile_idx < this->tile_idx);
       prev_tile=tile, tile=tile->next_tile);
  if ((tile==NULL) || (tile->tile_idx > tile_idx))
    { // Link as new tile in existing cluster.
      assert(prev_tile != NULL); /* Illegal to have a new cluster head here.
                                   See comments with constructor definition. */
      assert((tile_idx >= 0) && (comp_idx == -1)); /* Tile head must have
                                    component index of -1. */
      assert(allow_tiles);
      this->first_tile = prev_tile->first_tile;
      this->next_tile = prev_tile->next_tile;
      prev_tile->next_tile = this;
      return this;
    }

  kdu_params *comp, *prev_comp;

  this->first_tile = this->next_tile = NULL;
  for (comp=tile, prev_comp=NULL;
       (comp != NULL) && (comp->comp_idx < this->comp_idx);
       prev_comp=comp, comp=comp->next_comp);
  if ((comp == NULL) || (comp->comp_idx > comp_idx))
    { // Link as new component in existing tile.
      assert(prev_comp != NULL); /* Illegal to have a new tile head here.
                                   See comments with constructor definition. */
      assert(comp_idx >= 0);
      assert(allow_comps);
      this->first_comp = prev_comp->first_comp;
      this->next_comp = prev_comp->next_comp;
      this->num_comps = prev_comp->num_comps;
      prev_comp->next_comp = this;
      for (kdu_params *scan=first_comp; scan != NULL; scan=scan->next_comp)
        scan->num_comps++;
      return this;
    }

  // Must be new instance of current tile-component.

  this->first_comp = this->next_comp = NULL;
  assert(allow_insts);
  assert((comp->tile_idx == tile_idx) && (comp->comp_idx == comp_idx));
  while (comp->next_inst != NULL)
    comp = comp->next_inst;
  this->first_inst = comp->first_inst;
  comp->next_inst = this;
  this->inst_idx = comp->inst_idx + 1;
  return this;
}

/*****************************************************************************/
/*                            kdu_params::copy_from                          */
/*****************************************************************************/

void
  kdu_params::copy_from(kdu_params *source_ref, int source_tile,
                        int target_tile, int instance,
                        int skip_components, int discard_levels,
                        bool transpose, bool vflip, bool hflip)
{
  if (source_ref->cluster_name != this->cluster_name)
    { kdu_error e; e << "Illegal attempt to copy a `kdu_params' object to "
      "one which has been derived differently."; }
  kdu_params *source = source_ref;  // Keep `source_ref' for cluster navigation
  bool skip_this_cluster = false;

  if (source == source->first_tile)
    while ((source->tile_idx != source_tile) && (source->next_tile != NULL))
      source = source->next_tile;
  if (source->tile_idx != source_tile)
    skip_this_cluster = true;

  kdu_params *target = this;
  if (target == target->first_tile)
    while ((target->tile_idx != target_tile) && (target->next_tile != NULL))
      target = target->next_tile;
  if (target->tile_idx != target_tile)
    skip_this_cluster = true;

  if (source->comp_idx != target->comp_idx)
    skip_this_cluster = true;

  bool all_comps =
    (source == source->first_comp) && (target == target->first_comp);

  while ((source != NULL) && (target != NULL) && !skip_this_cluster)
    {
      if ((source->comp_idx >= 0) && (source->comp_idx < skip_components))
        {
          source = source->next_comp;
          continue;
        }
      kdu_params *src = source;
      kdu_params *dst = target;
      bool all_insts = (src == src->first_inst) && (dst == dst->first_inst);
      do {
          if (src->inst_idx != dst->inst_idx)
            break;
          if (dst->marked)
            { kdu_error e; e << "Illegal attempt to modify a `kdu_params' "
              "object which has already been marked!"; }
          if (dst->empty)
            dst->copy_with_xforms(src,skip_components,discard_levels,
                                  transpose,vflip,hflip);
          if (!all_insts)
            break;
          src = src->next_inst;
          if (dst->next_inst == NULL)
            dst->new_instance();
          dst = dst->next_inst;
        } while ((src != NULL) && (dst != NULL));
      if (!all_comps)
        break;
      source = source->next_comp;
      target = target->next_comp;
    }

  source = source_ref;
  target = this;
  if ((source != source->first_cluster) ||
      (target != target->first_cluster))
    return;
  source = source->next_cluster;
  target = target->next_cluster;
  while ((source != NULL) && (target != NULL))
    {
      target->copy_from(source,source_tile,target_tile,instance,
                        skip_components,discard_levels,transpose,vflip,hflip);
      source = source->next_cluster;
      target = target->next_cluster;
    }
}

/*****************************************************************************/
/*                        kdu_params::access_cluster                         */
/*****************************************************************************/

kdu_params *
  kdu_params::access_cluster(const char *name)
{
  kdu_params *scan;

  scan = this->first_inst->first_comp->first_tile->first_cluster;
  if (name == NULL)
    return(scan);
  while ((scan != NULL) && (strcmp(scan->cluster_name,name) != 0))
    scan = scan->next_cluster;
  return(scan);
}

/*****************************************************************************/
/*                        kdu_params::access_cluster                         */
/*****************************************************************************/

kdu_params *
  kdu_params::access_cluster(int seq)
{
  kdu_params *scan;

  scan = this->first_inst->first_comp->first_tile->first_cluster;
  while ((scan != NULL) && (seq > 0))
    { scan = scan->next_cluster; seq--; }
  return(scan);
}

/*****************************************************************************/
/*                        kdu_params::access_relation                        */
/*****************************************************************************/

kdu_params *
  kdu_params::access_relation(int tile_idx, int comp_idx, int inst_idx)
{
  kdu_params *scan;

  scan = this->first_inst->first_comp->first_tile;
  while ((scan != NULL) && (scan->tile_idx != tile_idx))
    scan = scan->next_tile;
  while ((scan != NULL) && (scan->comp_idx != comp_idx))
    scan = scan->next_comp;
  while ((scan != NULL) && (scan->inst_idx != inst_idx))
    scan = scan->next_inst;
  return(scan);
}

/*****************************************************************************/
/*                             kdu_params::get (int)                         */
/*****************************************************************************/

bool
  kdu_params::get(const char *name, int record_idx, int field_idx, int &value,
                 bool allow_inherit, bool allow_extend, bool allow_derived)
{
  kd_attribute *ap;
  att_val *att_ptr;

  assert((record_idx >= 0) && (field_idx >= 0));
  for (ap=attributes; ap != NULL; ap=ap->next)
    if (strcmp(name,ap->name) == 0)
      break;
  if (ap == NULL)
    { kdu_error e;
      e << "Attempt to access a code-stream attribute using "
        "the invalid name, \"" << name << "\"!";
    }
  if (field_idx >= ap->num_fields)
    { kdu_error e;
      e << "Attempt to access a code-stream attribute, with "
        "an invalid field index!\n";
      e << "The attribute name is \"" << name << "\".\n";
      e << "The field index is " << field_idx << ".";
    }
  att_ptr = ap->values + field_idx;
  if (att_ptr->pattern[0] == 'F')
    { kdu_error e;
      e << "Attempting to access a floating point code-stream "
        "attribute field with the integer access method!\n";
      e << "The attribute name is \"" << name << "\".";
    }
 
  bool have_attribute = (ap->num_records > 0);
  if (ap->derived && !allow_derived)
    have_attribute = false;

  if ((!have_attribute) && allow_inherit && (inst_idx == 0))
    { // Try inheritance.
      kdu_params *summary;

      summary = access_relation(tile_idx,-1);
      if ((summary != NULL) && (this != summary) &&
          summary->get(name,record_idx,field_idx,value,false,
                       allow_extend,allow_derived))
        return true;
      summary = access_relation(-1,comp_idx);
      if ((summary != NULL) && (this != summary) &&
          summary->get(name,record_idx,field_idx,value,true,
                       allow_extend,allow_derived))
        return true;
    }
  if (!have_attribute)
    return false;

  if ((ap->num_records <= record_idx) && allow_extend &&
      (ap->flags & this->CAN_EXTRAPOLATE))
    record_idx = ap->num_records - 1;
  att_ptr += ap->num_fields*record_idx;
  if ((record_idx < 0) || (record_idx >= ap->num_records) || !att_ptr->is_set)
    return false;
  value = att_ptr->ival;
  return true;
}

/*****************************************************************************/
/*                             kdu_params::get (bool)                        */
/*****************************************************************************/

bool
  kdu_params::get(const char *name, int record_idx, int field_idx, bool &value,
                 bool allow_inherit, bool allow_extend, bool allow_derived)
{
  kd_attribute *ap;
  att_val *att_ptr;

  assert((record_idx >= 0) && (field_idx >= 0));
  for (ap=attributes; ap != NULL; ap=ap->next)
    if (strcmp(name,ap->name) == 0)
      break;
  if (ap == NULL)
    { kdu_error e;
      e << "Attempt to access a code-stream attribute using "
        "the invalid name, \"" << name << "\"!";
    }
  if (field_idx >= ap->num_fields)
    { kdu_error e;
      e << "Attempt to access a code-stream attribute, with "
        "an invalid field index!\n";
      e << "The attribute name is \"" << name << "\".\n";
      e << "The field index is " << field_idx << ".";
    }
  att_ptr = ap->values + field_idx;
  if (att_ptr->pattern[0] != 'B')
    { kdu_error e;
      e << "Attempting to access a non-boolean code-stream "
        "attribute field with the boolean access method!\n";
      e << "The attribute name is \"" << name << "\".";
    }
 
  bool have_attribute = (ap->num_records > 0);
  if (ap->derived && !allow_derived)
    have_attribute = false;

  if ((!have_attribute) && allow_inherit && (inst_idx == 0))
    { // Try inheritance.
      kdu_params *summary;

      summary = access_relation(tile_idx,-1);
      if ((summary != NULL) && (this != summary) &&
          summary->get(name,record_idx,field_idx,value,false,
                       allow_extend,allow_derived))
        return true;
      summary = access_relation(-1,comp_idx);
      if ((summary != NULL) && (this != summary) &&
          summary->get(name,record_idx,field_idx,value,true,
                       allow_extend,allow_derived))
        return true;
    }
  if (!have_attribute)
    return false;

  if ((ap->num_records <= record_idx) && allow_extend &&
      (ap->flags & this->CAN_EXTRAPOLATE))
    record_idx = ap->num_records - 1;
  att_ptr += ap->num_fields*record_idx;
  if ((record_idx < 0) || (record_idx >= ap->num_records) || !att_ptr->is_set)
    return false;
  value = (att_ptr->ival)?true:false;
  return true;
}

/*****************************************************************************/
/*                            kdu_params::get (float)                        */
/*****************************************************************************/

bool
  kdu_params::get(const char *name, int record_idx, int field_idx,
                  float &value, bool allow_inherit, bool allow_extend,
                  bool allow_derived)
{
  kd_attribute *ap;
  att_val *att_ptr;

  assert((record_idx >= 0) && (field_idx >= 0));
  for (ap=attributes; ap != NULL; ap=ap->next)
    if (strcmp(name,ap->name) == 0)
      break;
  if (ap == NULL)
    { kdu_error e;
      e << "Attempt to access a code-stream attribute using "
        "the invalid name, \"" << name << "\"!";
    }
  if (field_idx >= ap->num_fields)
    { kdu_error e;
      e << "Attempt to access a code-stream attribute, with "
        "an invalid field index!\n";
      e << "The attribute name is \"" << name << "\".\n";
      e << "The field index is " << field_idx << ".";
    }
  att_ptr = ap->values + field_idx;
  if (att_ptr->pattern[0] != 'F')
    { kdu_error e;
      e << "Attempting to access an integer code-stream parameter "
        "attribute field with the floating point access method!\n";
      e << "The attribute name is \"" << name << "\".";
    }

  bool have_attribute = (ap->num_records > 0);
  if (ap->derived && !allow_derived)
    have_attribute = false;

  if ((!have_attribute) && allow_inherit && (inst_idx == 0))
    { // Try inheritance.
      kdu_params *summary;

      summary = access_relation(tile_idx,-1);
      if ((summary != NULL) && (this != summary) &&
          summary->get(name,record_idx,field_idx,value,false,
                       allow_extend,allow_derived))
        return true;
      summary = access_relation(-1,comp_idx);
      if ((summary != NULL) && (this != summary) &&
          summary->get(name,record_idx,field_idx,value,true,
                       allow_extend,allow_derived))
        return true;
    }
  if (!have_attribute)
    return false;

  if ((ap->num_records <= record_idx) && allow_extend &&
      (ap->flags & this->CAN_EXTRAPOLATE))
    record_idx = ap->num_records - 1;
  att_ptr += ap->num_fields*record_idx;
  if ((record_idx < 0) || (record_idx >= ap->num_records) || !att_ptr->is_set)
    return false;
  value = att_ptr->fval;
  return true;
}

/*****************************************************************************/
/*                             kdu_params::set (int)                         */
/*****************************************************************************/

void
  kdu_params::set(const char *name, int record_idx, int field_idx, int value)
{
  kd_attribute *ap;
  char const *cp;

  assert((record_idx >= 0) && (field_idx >= 0));
  for (ap=attributes; ap != NULL; ap=ap->next)
    if (strcmp(name,ap->name) == 0)
      break;
  if (ap == NULL)
    { kdu_error e;
      e << "Attempt to set a code-stream attribute using "
        "the invalid name, \"" << name << "\"!";
    }
  if ((ap->flags & ALL_COMPONENTS) && (comp_idx != -1))
    { kdu_error e;
      e << "Attempt to set a non-tile-specific code-stream attribute "
        "in a specific component!\n";
      e << "The attribute name is \"" << name << "\".";
    }
  if (field_idx >= ap->num_fields)
    { kdu_error e;
      e << "Attempt to set a code-stream attribute, with an "
        "invalid field index!\n";
      e << "The attribute name is \"" << name << "\".\n";
      e << "The field index is " << field_idx << ".";
    }
  cp = ap->values[field_idx].pattern;
  if (*cp == 'F')
    { kdu_error e;
      e << "Attempting to set a floating point code-stream parameter "
        "attribute field with the integer access method!\n";
      e << "The attribute name is \"" << name << "\".";
    }
  else if (*cp == 'B')
    {
      if ((value & 1) != value)
        { kdu_error e;
          e << "Attempting to set a boolean code-stream parameter "
            "attribute field with an integer not equal to 0 or 1!\n";
          e << "The attribute name is \"" << name << "\".";
        }
    }
  else if (*cp == '(')
    {
      char buf[80];
      int val;

      do {
          cp = parse_translator_entry(cp+1,',',buf,80,val);
        } while ((*cp == ',') && (val != value));
      if (val != value)
        { kdu_error e;
          e << "Attempting to set a code-stream attribute "
            "field using an integer value which does not match any "
            "of the defined translation values for the field!\n";
          e << "The attribute name is \"" << name << "\".";
        }
    }
  else if (*cp == '[')
    {
      char buf[80];
      int val, tmp=0;

      do {
          cp = parse_translator_entry(cp+1,'|',buf,80,val);
          if ((value & val) == val)
            tmp |= val; // Word contains this flag (or this set of flags).
        } while (*cp == '|');
      if (tmp != value)
        { kdu_error e;
          e << "Attempting to set a code-stream attribute "
            "field using an integer value which is incompatible "
            "with the flags defined for the field!\n";
          e << "The attribute name is \"" << name << "\".";
        }
    }
  else
    assert(*cp == 'I');
  if (record_idx >= ap->num_records)
    ap->augment_records(record_idx+1);
  assert((record_idx >= 0) && (record_idx < ap->num_records));
  ap->values[field_idx+ap->num_fields*record_idx].is_set = true;
  ap->values[field_idx+ap->num_fields*record_idx].ival = value;
  this->empty = false;
}

/*****************************************************************************/
/*                             kdu_params::set (bool)                        */
/*****************************************************************************/

void
  kdu_params::set(const char *name, int record_idx, int field_idx, bool value)
{
  kd_attribute *ap;
  char const *cp;

  assert((record_idx >= 0) && (field_idx >= 0));
  for (ap=attributes; ap != NULL; ap=ap->next)
    if (strcmp(name,ap->name) == 0)
      break;
  if (ap == NULL)
    { kdu_error e;
      e << "Attempt to set a code-stream attribute using "
        "the invalid name, \"" << name << "\"!";
    }
  if ((ap->flags & ALL_COMPONENTS) && (comp_idx != -1))
    { kdu_error e;
      e << "Attempt to set a non-tile-specific code-stream attribute "
        "in a specific component!\n";
      e << "The attribute name is \"" << name << "\".";
    }
  if (field_idx >= ap->num_fields)
    { kdu_error e;
      e << "Attempt to set a code-stream attribute, with an "
        "invalid field index!\n";
      e << "The attribute name is \"" << name << "\".\n";
      e << "The field index is " << field_idx << ".";
    }
  cp = ap->values[field_idx].pattern;
  if (*cp != 'B')
    { kdu_error e;
      e << "Attempting to set a non-boolean code-stream parameter "
        "attribute field with the boolean access method!\n";
      e << "The attribute name is \"" << name << "\".";
    }
  if (record_idx >= ap->num_records)
    ap->augment_records(record_idx+1);
  assert((record_idx >= 0) && (record_idx < ap->num_records));
  ap->values[field_idx+ap->num_fields*record_idx].is_set = true;
  ap->values[field_idx+ap->num_fields*record_idx].ival = (value)?1:0;
  this->empty = false;
}

/*****************************************************************************/
/*                            kdu_params::set (float)                        */
/*****************************************************************************/

void
  kdu_params::set(const char *name, int record_idx, int field_idx, double value)
{
  kd_attribute *ap;
  char const *cp;

  assert((record_idx >= 0) && (field_idx >= 0));
  for (ap=attributes; ap != NULL; ap=ap->next)
    if (strcmp(name,ap->name) == 0)
      break;
  if (ap == NULL)
    { kdu_error e;
      e << "Attempt to set a code-stream attribute using "
        "the invalid name, \"" << name << "\"!";
    }
  if ((ap->flags & ALL_COMPONENTS) && (comp_idx != -1))
    { kdu_error e;
      e << "Attempt to set a non-tile-specific code-stream attribute "
        "in a specific component!\n";
      e << "The attribute name is \"" << name << "\".";
    }
  if (field_idx >= ap->num_fields)
    { kdu_error e;
      e << "Attempt to set a code-stream attribute, with an "
        "invalid field index!\n";
      e << "The attribute name is \"" << name << "\".\n";
      e << "The field index is " << field_idx << ".";
    }
  cp = ap->values[field_idx].pattern;
  if (*cp != 'F')
    { kdu_error e;
      e << "Attempting to set an integer code-stream parameter "
        "attribute field with the floating point access method!\n";
      e << "The attribute name is \"" << name << "\".";
    }
  if (record_idx >= ap->num_records)
    ap->augment_records(record_idx+1);
  assert((record_idx >= 0) && (record_idx < ap->num_records));
  ap->values[field_idx+ap->num_fields*record_idx].is_set = true;
  ap->values[field_idx+ap->num_fields*record_idx].fval = (float) value;
  this->empty = false;
}

/*****************************************************************************/
/*                            kdu_params::set_derived                        */
/*****************************************************************************/

void
  kdu_params::set_derived(const char *name)
{
  kd_attribute *ap;

  for (ap=attributes; ap != NULL; ap=ap->next)
    if (strcmp(name,ap->name) == 0)
      break;
  if (ap == NULL)
    { kdu_error e;
      e << "Invalid attribute name, \"" << name << "\", supplied to the "
        "`kdu_params::set_derived' function.";
    }
  ap->derived = true;
}

/*****************************************************************************/
/*                          kdu_params::parse_string                         */
/*****************************************************************************/

bool
  kdu_params::parse_string(char *string)
{
  kd_attribute *ap;
  char *delim;
  size_t name_len;

  for (delim=string; *delim != '\0'; delim++)
    if ((*delim == ' ') || (*delim == '\t') || (*delim == '\n'))
      { kdu_error e;
        e << "Malformed attribute string, \"" << string << "\"!\n";
        e << "White space characters are illegal!";
      }
    else if ((*delim == ':') || (*delim == '='))
      break;
  name_len = (size_t)(delim - string);
  for (ap=attributes; ap != NULL; ap=ap->next)
    if ((strncmp(ap->name,string,name_len)==0) && (strlen(ap->name)==name_len))
      break;
  if (ap == NULL)
    {
      if (this == first_cluster)
        {
          kdu_params *scan;

          for (scan=this->next_cluster; scan != NULL; scan=scan->next_cluster)
            if (scan->parse_string(string))
              return(true);
        }
      return false;
    }
  assert(ap != NULL);
  if (*delim == '\0')
    { kdu_error e;
      e << "Attribute \"" << ap->name << "\" is missing parameters:\n\n\t";
      ap->describe(e,allow_tiles,allow_comps,true);
      e << "\nParameter values must be separated from the attribute name "
        "and optional location specifiers by an '=' sign!\n";
    }

  int target_tile = -2;
  int target_comp = -2;

  if (*delim == ':')
    {
      delim++;
      while (*delim != '=')
        if (*delim == '\0')
          break;
        else if ((*delim == 'T') && (target_tile < 0))
          target_tile = (int) strtol(delim+1,&delim,10);
        else if ((*delim == 'C') && (target_comp < 0))
          target_comp = (int) strtol(delim+1,&delim,10);
        else
          { kdu_error e;
            e << "Malformed location specifier encountered in attribute "
              "string, \"" << string << "\"!\n";
            e << "Tile specifiers following the the colon must have the "
              "form \"T<num>\", while component specifiers must have the "
              "form \"C<num>\". There may be at most one of each!";
          }
    }
  if (target_tile < -1)
    target_tile = this->tile_idx;
  if (target_comp < -1)
    target_comp = this->comp_idx;

  // Locate the object to which the attribute belongs.

  if ((this->tile_idx != target_tile) || (this->comp_idx != target_comp))
    {
      kdu_params *target;

      target = access_relation(target_tile,target_comp);
      if (target == NULL)
        { kdu_error e;
          e << "Attribute string, \"" << string << "\", refers to a non-"
            "existent tile-component!";
        }
      return target->parse_string(string);
    }

  // Perform accessibility checks

  if (marked)
    { kdu_error e; e << "Illegal attempt to modify a `kdu_params' object "
      "which has already been marked!"; }

  if ((ap->flags & ALL_COMPONENTS) && (comp_idx != -1))
    { kdu_error e;
      e << "Attempt to set a non-tile-specific code-stream attribute "
        "in a specific component!\n";
      e << "Problem occurred while parsing the attribute string, \""
        << string << "\".";
    }

  if (*delim != '=')
    { kdu_error e;
      e << "Malformed attribute string, \"" << string << "\"!\n";
      e << "Parameter values must be separated from the attribute name "
        "and optional location specifiers by an '=' sign!";
    }

  if ((ap->num_records > 0) && ap->parsed)
    { // Attribute already set by parsing.
      if ((!allow_insts) ||
          ((next_inst == NULL) && (new_instance() == NULL)))
        { kdu_error e;
          e << "The supplied attribute string, \"" << string << "\", refers "
            "to code-stream parameters which have already been parsed out "
            "of some string.  Moreover, multiple instances of this attribute "
            "are not permitted here!";
        }
      assert(next_inst != NULL);
      return next_inst->parse_string(string);
    }

  delete_unparsed_attribute(ap->name);
  ap->parsed = true;

  // Finally, we get around to parsing the parameter values.

  bool open_record;
  int fld, rec = 0;
  char *cp = delim + 1;

  while (*cp != '\0')
    { // Process a record.
      if (rec > 0)
        {
          if (*cp != ',')
            { kdu_error e;
              e << "Malformed attribute string, \"" << string << "\"!\n";
              e << "Problem encountered at \"" << cp << "\".\n";
              e << "Records must be separated by commas.";
            }
          cp++; // Skip over the inter-record comma.
        }
      if ((rec > 0) && !(ap->flags & MULTI_RECORD))
        { kdu_error e;
          e << "Malformed attribute string, \"" << string << "\"!\n";
          e << "Attribute does not support multiple parameter records!";
        }
      open_record = false;
      if (*cp == '{')
        { cp++; open_record = true; }
      else if (ap->num_fields > 1)
        { kdu_error e;
          e << "Malformed attribute string, \"" << string << "\"!\n";
          e << "Problem encountered at \"" << cp << "\".\n";
          e << "Records must be enclosed by curly braces, i.e., '{' and '}'.";
        }
      ap->augment_records(rec+1);
      for (fld=0; fld < ap->num_fields; fld++)
        {
          if (fld > 0)
            {
              if (*cp != ',')
                { kdu_error e;
                  e << "Malformed attribute string, \"" << string << "\"!\n";
                  e << "Problem encountered at \"" << cp << "\".\n";
                  e << "Fields must be separated by commas.";
                }
              cp++; // Skip over the inter-field comma.
            }
          att_val *att = ap->values + rec*ap->num_fields + fld;
          assert(!att->is_set);
          char const *pp = att->pattern;
          if (*pp == 'F')
            {
              att->fval = (float) strtod(cp,&delim);
              if (delim == cp)
                { kdu_error e;
                  e << "Malformed attribute string, \"" << string << "\"!\n";
                  e << "Problem encountered at \"" << cp << "\".\n";
                  e << "Expected a floating point field.";
                }
              cp = delim;
            }
          else if (*pp == 'I')
            {
              att->ival = (int) strtol(cp,&delim,10);
              if (delim == cp)
                { kdu_error e;
                  e << "Malformed attribute string, \"" << string << "\"!\n";
                  e << "Problem encountered at \"" << cp << "\".\n";
                  e << "Expected an integer field.";
                }
              cp = delim;
            }
          else if (*pp == 'B')
            {
              if (strncmp(cp,"yes",3) == 0)
                { att->ival = 1; cp += 3; }
              else if (strncmp(cp,"no",2) == 0)
                { att->ival = 0; cp += 2; }
              else
                { kdu_error e;
                  e << "Malformed attribute string, \"" << string << "\"!\n";
                  e << "Problem encountered at \"" << cp << "\".\n";
                  e << "Expected a boolean field identifier, i.e., one of "
                    "\"yes\" or \"no\".";
                }
            }
          else if (*pp == '(')
            {
              char buf[80], *bp, *dp;
              int val;
              bool success = false;

              do {
                  pp = parse_translator_entry(pp+1,',',buf,80,val);
                  for (bp=buf, dp=cp; *bp != '\0'; bp++, dp++)
                    if (*bp != *dp)
                      break;
                    success = (*bp == '\0') &&
                      ((*dp==',') || (*dp=='}') || (*dp=='\0'));
                } while ((*pp == ',') && !success);
              if (!success)
                { kdu_error e;
                  e << "Malformed attribute string, \"" << string << "\"!\n";
                  e << "Problem encountered at \"" << cp << "\".\n";
                  display_options(att->pattern,e);
                }
              att->ival = val;
              cp = dp;
            }
          else if (*pp == '[')
            {
              att->ival = 0;
              do {
                  char buf[80], *bp, *dp;
                  int val;
                  bool success = false;
                  
                  pp = att->pattern;
                  if (*cp == '|')
                    cp++;
                  do {
                      pp = parse_translator_entry(pp+1,'|',buf,80,val);
                      for (bp=buf, dp=cp; *bp != '\0'; bp++, dp++)
                        if (*bp != *dp)
                          break;
                        success = (*bp == '\0') &&
                          ((*dp==',')||(*dp=='}')||(*dp=='|')||(*dp=='\0'));
                    } while ((*pp == '|') && !success);
                  if ((!success) && (*cp == '0') &&
                      ((cp[1]==',') || (cp[1]=='}') ||
                       (cp[1]=='|') || (cp[1]=='\0')))
                    {
                      success = true;
                      val = 0;
                      dp = cp+1;
                    }
                  if (!success)
                    { kdu_error e;
                      e << "Malformed attribute string, \"" << string
                        << "\"!\n"; e << "Problem encountered at \"" << cp
                        << "\".\n";
                      display_options(att->pattern,e);
                    }
                  att->ival |= val;
                  cp = dp;
                } while (*cp == '|');
            }
          else
            assert(0);
          att->is_set = true;
        }
      if (*cp == '}')
        cp++;
      else if (open_record)
        { kdu_error e;
          e << "Malformed attribute string, \"" << string << "\"!\n";
          e << "Problem encountered at \"" << cp << "\".\n";
          e << "Opening brace for record is not matched by a closing brace.";
        }
      rec++;
    }

  this->empty = false;
  return true;
}

/*****************************************************************************/
/*                          kdu_params::parse_string                         */
/*****************************************************************************/

bool
  kdu_params::parse_string(char *string, int which_tile)
{
  int target_tile = -1;
  char *delim = strchr(string,':');
  if (delim != NULL)
    {
      delim++;
      while ((*delim != 'T') && (*delim != '=') && (*delim != '\0'))
        delim++;
      if (*delim == 'T')
        target_tile = (int) strtol(delim+1,NULL,10);
    }
  if (target_tile != which_tile)
    return false;
  return parse_string(string);
}

/*****************************************************************************/
/*                kdu_params::textualize_attributes  (one only)              */
/*****************************************************************************/

void
  kdu_params::textualize_attributes(ostream &stream, bool skip_derived)
{
  kd_attribute *ap;
  int rec, fld;

  for (ap=attributes; ap != NULL; ap=ap->next)
    {
      if (ap->num_records == 0)
        continue;
      if (ap->derived && skip_derived)
        continue;
      stream << ap->name;
      if ((comp_idx >= 0) || (tile_idx >= 0))
        { // Write the location specifier.
          stream << ':';
          if (tile_idx >= 0)
            stream << "T" << tile_idx;
          if (comp_idx >= 0)
            stream << "C" << comp_idx;
        }
      stream << '=';
      for (rec=0; rec < ap->num_records; rec++)
        {
          if (rec > 0)
            stream << ',';
          if (ap->num_fields > 1)
            stream << '{';
          for (fld=0; fld < ap->num_fields; fld++)
            {
              att_val *att = ap->values + rec*ap->num_fields + fld;
              char const *cp;
                
              if (fld > 0)
                stream << ',';
              if (!att->is_set)
                { kdu_error e;
                  e << "Attempting to textualize a code-stream parameter "
                    "attribute, which has only partially been set!\n";
                  e << "Error occurred in attribute \"" << ap->name << "\" "
                    " in field " << fld << " of record " << rec << ".";
                }
              cp = att->pattern;
              if (*cp == 'F')
                stream << att->fval;
              else if (*cp == 'I')
                stream << att->ival;
              else if (*cp == 'B')
                stream << ((att->ival)?"yes":"no");
              else if (*cp == '(')
                {
                  char buf[80];
                  int val;
                  
                  do {
                      cp = parse_translator_entry(cp+1,',',buf,80,val);
                      if (val == att->ival)
                        break;
                    } while (*cp == ',');
                  assert(val == att->ival);
                  stream << buf;
                }
              else if (*cp == '[')
                {
                  char buf[80];
                  int val, acc=0;
                  
                  if (att->ival == 0)
                    stream << 0;
                  do {
                      cp = parse_translator_entry(cp+1,'|',buf,80,val);
                      if (((att->ival & val) == val) && ((acc | val) > acc))
                        {
                          stream << buf;
                          acc |= val;
                          if (acc == att->ival)
                            break;
                          stream << ',';
                        }
                    } while (*cp == '|');
                  assert(acc == att->ival);
                }
              else
                assert(0);
            }
          if (ap->num_fields > 1)
            stream << '}';
        }
      stream << '\n';
    }
}

/*****************************************************************************/
/*            kdu_params::textualize_attributes  (list processing)           */
/*****************************************************************************/

void
  kdu_params::textualize_attributes(ostream &stream, int min_tile, int max_tile,
                                   bool skip_derived)
{
  kdu_params *scan;

  if ((tile_idx >= min_tile) && (tile_idx <= max_tile))
    {
      this->textualize_attributes(stream,skip_derived);
      if (this == first_inst)
        for (scan=this->next_inst; scan != NULL; scan=scan->next_inst)
          scan->textualize_attributes(stream,skip_derived);
      if (this == first_comp)
        for (scan=this->next_comp; scan != NULL; scan=scan->next_comp)
          scan->textualize_attributes(stream,min_tile,max_tile,skip_derived);
    }
  if (this == first_tile)
    for (scan=this->next_tile; scan != NULL; scan=scan->next_tile)
      scan->textualize_attributes(stream,min_tile,max_tile,skip_derived);
  if (this == first_cluster)
    for (scan=this->next_cluster; scan != NULL; scan=scan->next_cluster)
      scan->textualize_attributes(stream,min_tile,max_tile,skip_derived);
}

/*****************************************************************************/
/*                        kdu_params::describe_strings                       */
/*****************************************************************************/

void
  kdu_params::describe_strings(ostream &stream, bool include_comments)
{
  kd_attribute *ap;

  for (ap=attributes; ap != NULL; ap=ap->next)
    ap->describe(stream,allow_tiles,allow_comps,include_comments);
}

/*****************************************************************************/
/*                        kdu_params::describe_string                        */
/*****************************************************************************/

void
  kdu_params::describe_string(const char *name,
                             ostream &stream, bool include_comments)
{
  kd_attribute *ap;

  for (ap=attributes; ap != NULL; ap=ap->next)
    if (strcmp(ap->name,name) == 0)
      break;
  if (ap == NULL)
    { kdu_error e; e << "\"kdu_params::describe_string\" invoked with an "
      "invalid attribute identifier, \"" << name << "\"."; }
  ap->describe(stream,allow_tiles,allow_comps,include_comments);
}

/*****************************************************************************/
/*                          kdu_params::find_string                          */
/*****************************************************************************/

kdu_params *
  kdu_params::find_string(char *string, const char * &name)
{
  kd_attribute *ap;
  char *delim;
  size_t name_len;

  for (delim=string; *delim != '\0'; delim++)
    if ((*delim == ' ') || (*delim == '\t') || (*delim == '\n'))
      return NULL;
    else if ((*delim == ':') || (*delim == '='))
      break;
  name_len = (size_t)(delim - string);
  for (ap=attributes; ap != NULL; ap=ap->next)
    if ((strncmp(ap->name,string,name_len)==0) && (strlen(ap->name)==name_len))
      break;
  if (ap == NULL)
    {
      if (this == first_cluster)
        {
          kdu_params *scan, *result;

          for (scan=this->next_cluster; scan != NULL; scan=scan->next_cluster)
            if ((result = scan->find_string(string,name)) != NULL)
              return(result);
        }
      return NULL;
    }
  assert(ap != NULL);
  name = ap->name;
  if (*delim == '\0')
    return this;

  int target_tile = -2;
  int target_comp = -2;

  if (*delim == ':')
    {
      delim++;
      while (*delim != '=')
        if (*delim == '\0')
          break;
        else if ((*delim == 'T') && (target_tile < 0))
          target_tile = (int) strtol(delim+1,&delim,10);
        else if ((*delim == 'C') && (target_comp < 0))
          target_comp = (int) strtol(delim+1,&delim,10);
        else
          return this;
    }
  if (target_tile < -1)
    target_tile = this->tile_idx;
  if (target_comp < -1)
    target_comp = this->comp_idx;

  // Locate the object to which the attribute belongs.

  if ((this->tile_idx != target_tile) || (this->comp_idx != target_comp))
    {
      kdu_params *target;

      target = access_relation(target_tile,target_comp);
      if (target == NULL)
        return this;
      return target->find_string(string,name);
    }
  return this;
}

/*****************************************************************************/
/*                  kdu_params::delete_unparsed_attribute                    */
/*****************************************************************************/

void
  kdu_params::delete_unparsed_attribute(const char *name)
{
  kd_attribute *ap;

  for (ap=attributes; ap != NULL; ap=ap->next)
    if (strcmp(ap->name,name) == 0)
      break;
  if (ap == NULL)
    { kdu_error e; e << "Attempting to delete a non-existent attribute with "
      "\"kdu_params::delete_unparsed_attribute\"."; }
  if (!ap->parsed)
    {
      int num_fields = ap->num_records * ap->num_fields;
      for (int n=0; n < num_fields; n++)
        ap->values[n].is_set = false;
      ap->num_records = 0;
    }

  kdu_params *scan;
  if (this == first_inst)
    for (scan=next_inst; scan != NULL; scan=scan->next_inst)
      scan->delete_unparsed_attribute(name);
  if (this == first_comp)
    for (scan=next_comp; scan != NULL; scan=scan->next_comp)
      scan->delete_unparsed_attribute(name);
  if (this == first_tile)
    for (scan=next_tile; scan != NULL; scan=scan->next_tile)
      scan->delete_unparsed_attribute(name);
}

/*****************************************************************************/
/*                    kdu_params::translate_marker_segment                   */
/*****************************************************************************/

bool
  kdu_params::translate_marker_segment(kdu_uint16 code, int num_bytes,
                                      kdu_byte bytes[], int which_tile,
                                      int tpart_idx)
{
  kdu_params *cluster, *tile, *comp, *inst;

  cluster = this->first_inst->first_comp->first_tile->first_cluster;
  for (; cluster != NULL; cluster = cluster->next_cluster)
    {
      for (tile=cluster; tile != NULL; tile=tile->next_tile)
        if (tile->tile_idx == which_tile)
          break;
      if (tile == NULL)
        continue;
      for (comp=tile; comp != NULL; comp=comp->next_comp)
        {
          for (inst=comp; inst != NULL; inst=inst->next_inst)
            if (!inst->marked)
              break;
          if (inst == NULL)
            continue;
          if (inst->read_marker_segment(code,num_bytes,bytes,tpart_idx))
            {
              inst->marked = true;
              if (inst->allow_insts)
                inst->new_instance();
              inst->empty = false;
              return true;
            }
        }
    }
  return false;
}

/*****************************************************************************/
/*                    kdu_params::generate_marker_segments                   */
/*****************************************************************************/

int
  kdu_params::generate_marker_segments(kdu_output *out, int which_tile,
                                      int tpart_idx)
{
  kdu_params *cluster, *tile, *comp, *inst, *last_marked;
  int total_bytes, new_bytes;

  total_bytes = 0;
  cluster = this->first_inst->first_comp->first_tile->first_cluster;
  for (; cluster != NULL; cluster=cluster->next_cluster)
    {
      for (tile=cluster; tile != NULL; tile=tile->next_tile)
        if (tile->tile_idx == which_tile)
          break;
      if (tile == NULL)
        continue;
      for (comp=tile; comp != NULL; comp=comp->next_comp)
        {
          if ((comp != tile) && (tile->marked))
            last_marked = tile;
          else if (tile != cluster)
            {
              for (last_marked=cluster; last_marked != NULL;
                   last_marked=last_marked->next_comp)
                if (last_marked->comp_idx == comp->comp_idx)
                  break;
              if ((last_marked == NULL) || !last_marked->marked)
                last_marked = (cluster->marked)?cluster:NULL;
            }
          else
            last_marked = NULL;
          for (inst=comp; inst != NULL; inst=inst->next_inst)
            {
              new_bytes =
                inst->write_marker_segment(out,last_marked,tpart_idx);
              if (new_bytes > 0)
                {
                  assert(new_bytes >= 4);
                  total_bytes += new_bytes;
                  inst->marked = true;
                }
              last_marked = (inst->marked)?inst:NULL;
            }
        }
    }
  return(total_bytes);
}

/*****************************************************************************/
/*                          kdu_params::finalize_all                         */
/*****************************************************************************/

void
  kdu_params::finalize_all()
{
  kdu_params *scan;

  finalize();
  if (this == first_inst)
    for (scan=next_inst; scan != NULL; scan=scan->next_inst)
      scan->finalize_all();
  if (this == first_comp)
    for (scan=next_comp; scan != NULL; scan=scan->next_comp)
      scan->finalize_all();
  if (this == first_tile)
    for (scan=next_tile; scan != NULL; scan=scan->next_tile)
      scan->finalize_all();
  if (this == first_cluster)
    for (scan=next_cluster; scan != NULL; scan=scan->next_cluster)
      scan->finalize_all();
}

/*****************************************************************************/
/*                          kdu_params::finalize_all                         */
/*****************************************************************************/

void
  kdu_params::finalize_all(int which_tile)
{
  kdu_params *scan;

  if (tile_idx == which_tile)
    {
      finalize();
      if (this == first_inst)
        for (scan=next_inst; scan != NULL; scan=scan->next_inst)
          scan->finalize_all();
      if (this == first_comp)
        for (scan=next_comp; scan != NULL; scan=scan->next_comp)
          scan->finalize_all();
    }
  if ((this == first_tile) && (which_tile >= 0))
    for (scan=next_tile; scan != NULL; scan=scan->next_tile)
      if (scan->tile_idx == which_tile)
        scan->finalize_all();
  if (this == first_cluster)
    for (scan=next_cluster; scan != NULL; scan=scan->next_cluster)
      scan->finalize_all(which_tile);
}

/*****************************************************************************/
/*                        kdu_params::define_attribute                       */
/*****************************************************************************/

void
  kdu_params::define_attribute(const char *name, const char *comment,
                              const char *pattern, int flags)
  /* Makes sure the attributes appear in the list in the same order in
     which they are added. */
{
  kd_attribute *att, *scan;

  att = new kd_attribute(name,comment,flags,pattern);
  if ((scan = attributes) != NULL)
    {
      while (scan->next != NULL)
        scan = scan->next;
      scan->next = att;
    }
  else
    attributes = att;
}

/* ========================================================================= */
/*                               siz_params                                  */
/* ========================================================================= */

/*****************************************************************************/
/*                       siz_params::siz_params (no args)                    */
/*****************************************************************************/

siz_params::siz_params()
  : kdu_params(SIZ_params,false,false,false)
{
  define_attribute(Ssize,
                   "Canvas dimensions: vertical dimension first.\n"
                   "\t\t[For compressors, this will "
                   "normally be derived from the dimensions of the individual "
                   "image components. Explicitly supplying the canvas "
                   "dimensions may be desirable if the source image files "
                   "do not indicate their dimensions, or if custom "
                   "sub-sampling factors are desired.]",
                   "II");
  define_attribute(Sorigin,
                   "Image origin on canvas: vertical coordinate first.\n"
                   "\t\t[Defaults to {0,0}, or the tile origin if one "
                   "is given]",
                   "II");
  define_attribute(Stiles,
                   "Tile partition size: vertical dimension first.\n"
                   "\t\t[Defaults to {0,0}]",
                   "II");
  define_attribute(Stile_origin,
                   "Tile origin on the canvas: vertical coordinate first.\n"
                   "\t\t[Defaults to {0,0}]",
                   "II");
  define_attribute(Scomponents,
                   "Number of image components.\n"
                   "\t\t[For compressors, this will "
                   "normally be deduced from the number and type of image "
                   "files supplied to the compressor.]",
                   "I");
  define_attribute(Ssigned,
                   "Indicates whether each image component contains signed "
                   "or unsigned sample values.\n"
                   "\t\t[For compressors, this will normally be deduced "
                   "from the image files supplied to the compressor, but "
                   "may be explicitly set if raw input files are to be used. "
                   "The last supplied identifier is repeated indefinitely for "
                   "all remaining components.]",
                   "B",MULTI_RECORD | CAN_EXTRAPOLATE);
  define_attribute(Sprecision,
                   "Indicates the bit-depth of each image component.\n"
                   "\t\t[For compressors, this "
                   "will normally be deduced from the image files supplied to "
                   "the compressor, but may be explicitly set if raw input "
                   "files are to be used. The last supplied value is "
                   "repeated indefinitely for all remaining components.]",
                   "I",MULTI_RECORD | CAN_EXTRAPOLATE);
  define_attribute(Ssampling,
                   "Indicates the sub-sampling factors for each image "
                   "component. In each record, the vertical factor appears "
                   "first, followed by the horizontal sub-sampling factor. "
                   "The last supplied record is repeated indefinitely for "
                   "all remaining components.\n"
                   "\t\t[For compressors, a suitable set of sub-sampling "
                   "factors will normally be deduced from the individual "
                   "image component dimensions.]",
                   "II",MULTI_RECORD | CAN_EXTRAPOLATE);
  define_attribute(Sdims,
                   "Indicates the dimensions (vertical, then horizontal) of "
                   "each individual image component. The last supplied "
                   "record is repeated indefinitely for all remaining "
                   "components.\n"
                   "\t\t[For compressors, the image component dimensions will "
                   "normally be deduced from the image files supplied to the "
                   "compressor, but may be explicitly set if raw input "
                   "files are to be used.]",
                   "II",MULTI_RECORD | CAN_EXTRAPOLATE);
}

/*****************************************************************************/
/*                       siz_params::copy_with_xforms                        */
/*****************************************************************************/

void
  siz_params::copy_with_xforms(kdu_params *source,
                               int skip_components, int discard_levels,
                               bool transpose, bool vflip, bool hflip)
  /* Note:
        For a thorough understanding of the effect of geometric manipulations
     on canvas coordinates (including an expanation as to why these JPEG2000
     standard allows such geometric maninpulations to be conducted in the
     transformed domain) refer to Section 11.4.2 of the book by Taubman and
     Marcellin. */

{
  int canvas_x, canvas_y, origin_x, origin_y;
  int tiles_y, tiles_x, tile_origin_y, tile_origin_x;

  if (!(source->get(Ssize,0,(transpose)?1:0,canvas_y) &&
        source->get(Ssize,0,(transpose)?0:1,canvas_x) &&
        source->get(Sorigin,0,(transpose)?1:0,origin_y) &&
        source->get(Sorigin,0,(transpose)?0:1,origin_x) &&
        source->get(Stiles,0,(transpose)?1:0,tiles_y) &&
        source->get(Stiles,0,(transpose)?0:1,tiles_x) &&
        source->get(Stile_origin,0,(transpose)?1:0,tile_origin_y) &&
        source->get(Stile_origin,0,(transpose)?0:1,tile_origin_x)))
    { kdu_error e; e << "Unable to copy SIZ parameters, unless all canvas "
      "coordinates are available.  Try using `siz_params::finalize' before "
      "attempting the copy."; }
  int tmp;
  if (hflip)
    {
      if ((tile_origin_x+tiles_x) >= canvas_x)
        { /* Only one tile -- make it a power of 2 to simplify equivalence
             transformations performed during marker writing later on. */
          for (int n=0; n < 31; n++)
            if ((1<<n) >= tiles_x)
              { tiles_x = (1<<n); break; }
        }
      tmp = 1-canvas_x;  canvas_x = 1-origin_x;  origin_x = tmp;
      tile_origin_x = 1-tile_origin_x;
      while (tile_origin_x > origin_x)
        tile_origin_x -= tiles_x;
    }
  if (vflip)
    {
      if ((tile_origin_y+tiles_y) >= canvas_y)
        { /* Only one tile -- make it a power of 2 to simplify equivalence
             transformations performed during marker writing later on. */
          for (int n=0; n < 31; n++)
            if ((1<<n) >= tiles_y)
              { tiles_y = (1<<n); break; }
        }
      tmp = 1-canvas_y;  canvas_y = 1-origin_y;  origin_y = tmp;
      tile_origin_y = 1-tile_origin_y;
      while (tile_origin_y > origin_y)
        tile_origin_y -= tiles_y;
    }
  set(Ssize,0,0,canvas_y);             set(Ssize,0,1,canvas_x);
  set(Sorigin,0,0,origin_y);           set(Sorigin,0,1,origin_x);
  set(Stiles,0,0,tiles_y);             set(Stiles,0,1,tiles_x);
  set(Stile_origin,0,0,tile_origin_y); set(Stile_origin,0,1,tile_origin_x);

  int num_components=0;
  if (source->get(Scomponents,0,0,num_components))
    {
      if (num_components <= skip_components)
        { kdu_error e; e << "Attempting to discard all of the components "
          "from an existing code-stream!"; }
      set(Scomponents,0,0,num_components-skip_components);
    }
  for (int n=skip_components; n < num_components; n++)
    { /* Note that we deliberately do not copy the image dimensions since these
         should be automatically recomputed based on any discarded resolution
         levels. */
      int precision, x, y;
      bool is_signed;

      if (source->get(Sprecision,n,0,precision))
        set(Sprecision,n-skip_components,0,precision);
      if (source->get(Ssigned,n,0,is_signed))
        set(Ssigned,n-skip_components,0,is_signed);
      if (source->get(Ssampling,n,(transpose)?1:0,y) &&
          source->get(Ssampling,n,(transpose)?0:1,x))
        {
          x <<= discard_levels; y <<= discard_levels;
          if ((x > 255) || (y > 255))
            { kdu_error e; e << "Current algorithm for discarding "
              "resolution levels from an existing code-stream for transcoding "
              "operates by multiplying the sub-sampling factors in the SIZ "
              "marker.  For the current code-stream, this results in "
              "sub-sampling factors which exceed the legal range of 1 to "
              "255."; }
          set(Ssampling,n-skip_components,0,y);
          set(Ssampling,n-skip_components,1,x);
        }
    }
}

/*****************************************************************************/
/*                       siz_params::write_marker_segment                    */
/*****************************************************************************/

int
  siz_params::write_marker_segment(kdu_output *out, kdu_params *last_marked,
                                   int tpart_idx)
{
  siz_params *ref = (siz_params *) last_marked;
  int x, y, xo, yo, xt, yt, xto, yto, num_components;
  int length;

  assert(ref == NULL);
  if (tpart_idx != 0)
    return 0;
  if (!(get(Ssize,0,0,y) && get(Ssize,0,1,x) &&
        get(Sorigin,0,0,yo) && get(Sorigin,0,1,xo) &&
        get(Stiles,0,0,yt) && get(Stiles,0,1,xt) &&
        get(Stile_origin,0,0,yto) && get(Stile_origin,0,1,xto) &&
        get(Scomponents,0,0,num_components)))
    { kdu_error e; e << "Unable to write SIZ marker segment yet!";}

  if ((num_components < 1) || (num_components > 16384))
    { kdu_error e; e << "Illegal number of image components! Must be in the "
      "range 1 to 16384."; }

  length = 4 + 2 + 4*8 + 2 + 3*num_components;
  if (out == NULL)
    return length;

  // Deal with negative canvas coordinates.
  if ((xto < 0) || (yto < 0)) // These are the most negative coordinates
    { /* For a better understanding of canvas coordinate equivalence
         transformations, consult Section 11.4.2 of the book by Taubman
         and Marcellin. */
      int x_lcm = xt; // Coordinate shifts must at least be multiples
      int y_lcm = yt; // of the relevant tile dimensions.
      kdu_params *cod = this->access_cluster(COD_params);
      int num_tiles = ceil_ratio(x-xo,xt) * ceil_ratio(y-yo,yt);
      for (int t=-1; t < num_tiles; t++)
        for (int c=0; c < num_components; c++)
          {
            int xr=1, yr=1;
            get(Ssampling,c,0,yr); get(Ssampling,c,1,xr);
            int levels;
            bool use_precincts;
            if ((cod == NULL) ||
                ((cod = cod->access_relation(t,c)) == NULL) ||
                !(cod->get(Clevels,0,0,levels) &&
                  cod->get(Cuse_precincts,0,0,use_precincts)))
              { kdu_error e; e << "Attempting to write geometrically "
                "transformed SIZ marker information without attaching and "
                "finalizing all tile-component COD marker information.  This "
                "is essential to establish canvas coordinate equivalence "
                "relationships."; }
            int r=levels, res_exp=0;
            for (; r >= 0; r--, res_exp++)
              {
                int xp=15, yp=15;
                if (use_precincts)
                  { cod->get(Cprecincts,(levels-r),0,yp);
                    cod->get(Cprecincts,(levels-r),1,xp); }
                kdu_int32 x_precinct = xr<<(xp+r); // Precinct dimensions on
                kdu_int32 y_precinct = yr<<(yp+r); // full resolution canvas
                if ((x_precinct <= 0) || (y_precinct <= 0))
                  { kdu_error e; e << "Error attempting to convert "
                    "geometrically transformed canvas coordinates to legal "
                    "marker ranges.  Try using a smaller precinct size (you "
                    "can use the transcoding utility to achieve this "
                    "at the same time as geometric transformations.)"; }
                x_lcm = find_lcm(x_lcm,x_precinct);
                y_lcm = find_lcm(y_lcm,y_precinct);
              }
          }
        // Finally, we can determine canvas translates which will lead to
        // non-negative coordinates which are equivalent to the originals.
        int x_off = x_lcm * ceil_ratio(-xto,x_lcm);
        int y_off = y_lcm * ceil_ratio(-yto,y_lcm);
        x += x_off; xo += x_off; xto += x_off;
        y += y_off; yo += y_off; yto += y_off;
        assert((xto >= 0) && (yto >= 0));
    }

  // Check for legal sequencing of the tile and image origins.
  if ((xo < xto) || (yo < yto) || (xo >= (xto+xt)) || (yo >= (yto+yt)) ||
      (x <= xo) || (y <= yo))
    { kdu_error e; e << "Cannot write SIZ marker with illegal canvas "
      "coordinates.  The first tile is required to have a non-empty "
      "intersection with the image region on the canvas."; }
  
  int acc_length = 0; // Accumulate actual length for consistency checking.
  acc_length += out->put(KDU_SIZ);
  acc_length += out->put((kdu_uint16)(length-2));
  acc_length += out->put((kdu_uint16) 0);
  acc_length += out->put((kdu_uint32) x);
  acc_length += out->put((kdu_uint32) y);
  acc_length += out->put((kdu_uint32) xo);
  acc_length += out->put((kdu_uint32) yo);
  acc_length += out->put((kdu_uint32) xt);
  acc_length += out->put((kdu_uint32) yt);
  acc_length += out->put((kdu_uint32) xto);
  acc_length += out->put((kdu_uint32) yto);
  acc_length += out->put((kdu_uint16) num_components);
  for (int c=0; c < num_components; c++)
    {
      int is_signed, precision, xr, yr;

      if (!(get(Ssigned,c,0,is_signed) && get(Sprecision,c,0,precision) &&
            get(Ssampling,c,0,yr) && get(Ssampling,c,1,xr)))
        { kdu_error e; e << "Unable to write SIZ marker segment! Precision "
          "or sub-sampling information missing for at least one component.";}
      if ((precision < 1) || (precision > 38))
        { kdu_error e; e << "Illegal image sample bit-depth, " << precision <<
          ". Legal range is from 1 to 38 bits per sample."; }
      if ((xr < 1) || (xr > 255) || (yr < 1) || (yr > 255))
        { kdu_error e; e << "Illegal component sub-sampling factors, {"
          << yr << "," << xr << "}. Legal range is from 1 to 255."; }
      acc_length += out->put((kdu_byte)(precision-1 + (is_signed<<7)));
      acc_length += out->put((kdu_byte) xr);
      acc_length += out->put((kdu_byte) yr);
    }
  assert(acc_length == length);
  return length;
}

/*****************************************************************************/
/*                        siz_params::read_marker_segment                    */
/*****************************************************************************/

bool
  siz_params::read_marker_segment(kdu_uint16 code, int num_bytes,
                                  kdu_byte bytes[], int tpart_idx)
{
  kdu_byte *bp, *end;
  int num_components, c;

  if ((tpart_idx != 0) || (code != KDU_SIZ))
    return false;
  bp = bytes;
  end = bp + num_bytes;
  try {
    if (kdu_read(bp,end,2) != 0)
      { kdu_error e; e << "Non-Part1 SIZ marker segment encountered!"; }
    set(Ssize,0,1,kdu_read(bp,end,4));
    set(Ssize,0,0,kdu_read(bp,end,4));
    set(Sorigin,0,1,kdu_read(bp,end,4));
    set(Sorigin,0,0,kdu_read(bp,end,4));
    set(Stiles,0,1,kdu_read(bp,end,4));
    set(Stiles,0,0,kdu_read(bp,end,4));
    set(Stile_origin,0,1,kdu_read(bp,end,4));
    set(Stile_origin,0,0,kdu_read(bp,end,4));
    set(Scomponents,0,0,num_components = kdu_read(bp,end,2));
    for (c=0; c < num_components; c++)
      {
        int precision;

        precision = kdu_read(bp,end,1);
        set(Ssigned,c,0,(precision>>7)&1);
        set(Sprecision,c,0,(precision&0x7F)+1);
        set(Ssampling,c,1,kdu_read(bp,end,1));
        set(Ssampling,c,0,kdu_read(bp,end,1));
      }
    if (bp != end)
      { kdu_error e;
        e << "Malformed SIZ marker segment encountered. The final "
          << (int)(end-bp) << " bytes were not consumed!";
      }
  } // End of try block.
  catch(kdu_byte *) { kdu_error e;
    e << "Malformed SIZ marker segment encountered. "
         "Marker segment is too small.";
  }
  return true;
}

/*****************************************************************************/
/*                             siz_params::finalize                          */
/*****************************************************************************/

void
  siz_params::finalize()
{
  int components, val, n;
  bool have_sampling, have_dims, have_components, have_size;
  int size_x, size_y, origin_x, origin_y;

  // First collect some basic facts.

  have_components = get(Scomponents,0,0,components);
  have_sampling = get(Ssampling,0,0,val);
  have_dims = get(Sdims,0,0,val);
  have_size = get(Ssize,0,0,size_y) && get(Ssize,0,1,size_x);
  if (!(get(Sorigin,0,0,origin_y) && get(Sorigin,0,1,origin_x)))
    { set(Sorigin,0,0,origin_y=0); set(Sorigin,0,1,origin_x=0); }
  if (have_sampling && !have_components)
    {
      for (components=1;
           get(Ssampling,components,0,val,false,false);
           components++);
      have_components = true;
    }
  if (have_dims && !have_components)
    {
      for (components=1;
           get(Sdims,components,0,val,false,false);
           components++);
      have_components = true;
    }
  if (!have_components)
    { kdu_error e; e << "Problem trying to finalize SIZ information. ";
      e << "Insufficient data supplied to determine the number of "
        "image components! Available information is as follows:\n\n";
      this->textualize_attributes(e); e << "\n";
    }

  int *dim_x = new int[components]; int *dim_y = new int[components];
  int *sub_x = new int[components]; int *sub_y = new int[components];
  if (have_dims)
    for (n=0; n < components; n++)
      if (!(get(Sdims,n,0,dim_y[n]) && get(Sdims,n,1,dim_x[n])))
        { kdu_error e; e << "Problem trying to finalize SIZ information. ";
          e << "Component dimensions are only partially available!";
        }
  if (have_sampling)
    for (n=0; n < components; n++)
      if (!(get(Ssampling,n,0,sub_y[n]) && get(Ssampling,n,1,sub_x[n])))
        { kdu_error e; e << "Problem trying to finalize SIZ information ";
          e << "Component sub-sampling factors are only "
            "partially available!";
        }
      else if ((sub_x[n] < 1) || (sub_y[n] < 1))
        { kdu_error e; e << "Image component sub-sampling factors must be "
          "strictly positive!"; }

  // Now for the information synthesis problem.

  if ((!have_sampling) && (!have_dims))
    { kdu_error e; e << "Problem trying to finalize SIZ information. ";
      e << "Must have either the individual component dimensions or the "
        "component sub-sampling factors.";
    }
  if (!have_dims)
    { // Have sub-sampling factors and need to synthesize dimensions.
      if (!have_size)
        { kdu_error e; e << "Problem trying to finalize SIZ information. ";
          e << "Must have either the image component dimensions or the canvas "
            "size! Available information is as follows:\n\n";
          this->textualize_attributes(e); e << "\n";
        }
      for (n=0; n < components; n++)
        {
          dim_x[n] = ceil_ratio(size_x,sub_x[n])-ceil_ratio(origin_x,sub_x[n]);
          dim_y[n] = ceil_ratio(size_y,sub_y[n])-ceil_ratio(origin_y,sub_y[n]);
          set(Sdims,n,0,dim_y[n]); set(Sdims,n,1,dim_x[n]);
        }
      have_dims = true;
    }

  if ((!have_size) && (!have_sampling))
    { // Synthesize a compatible canvas size.
      assert(have_dims);
      if (!(synthesize_canvas_size(components,dim_x,origin_x,size_x) &&
            synthesize_canvas_size(components,dim_y,origin_y,size_y)))
        { kdu_error e; e << "Problem trying to finalize SIZ information ";
          e << "Component dimensions are not consistent with a "
            "legal set of sub-sampling factors. Available information is "
            "as follows:\n\n";
          this->textualize_attributes(e); e << "\n";
        }
      set(Ssize,0,0,size_y); set(Ssize,0,1,size_x);
      have_size = true;
    }

  if (!have_sampling)
    { // Synthesize sub-sampling factors from canvas size and image dimensions.
      assert(have_dims && have_size);
      for (n=0; n < components; n++)
        {
          val = (size_x - origin_x) / dim_x[n]; // Rough estimate.
          if (val <= 0)
            val = 1;
          while ((val > 1) &&
                 ((ceil_ratio(size_x,val)-ceil_ratio(origin_x,val))<dim_x[n]))
            val--;
          while ((ceil_ratio(size_x,val)-ceil_ratio(origin_x,val)) > dim_x[n])
            val++;
          if ((ceil_ratio(size_x,val)-ceil_ratio(origin_x,val)) != dim_x[n])
            { kdu_error e; e << "Problem trying to finalize SIZ information. ";
              e << "Horizontal canvas and image component dimensions are not "
                "compatible.  Available information is as follows:\n\n";
              this->textualize_attributes(e); e << "\n";
            }
          sub_x[n] = val;
          val = (size_y - origin_y) / dim_y[n]; // Rough estimate.
          if (val <= 0)
            val = 1;
          while ((val > 1) &&
                 ((ceil_ratio(size_y,val)-ceil_ratio(origin_y,val))<dim_y[n]))
            val--;
          while ((ceil_ratio(size_y,val)-ceil_ratio(origin_y,val)) > dim_y[n])
            val++;
          if ((ceil_ratio(size_y,val)-ceil_ratio(origin_y,val)) != dim_y[n])
            { kdu_error e; e << "Problem trying to finalize SIZ information. ";
              e << "Vertical canvas and image component dimensions are not "
                "compatible.  Available information is as follows:\n\n";
              this->textualize_attributes(e); e << "\n";
            }
          sub_y[n] = val;
          set(Ssampling,n,0,sub_y[n]); set(Ssampling,n,1,sub_x[n]);
        }
      have_sampling = true;
    }

  if (!have_size)
    { // Synthesize canvas size from sub-sampling factors and image dimensions.
      assert(have_dims && have_sampling);
      int min_size=0, max_size=0, max, min;
      for (n=0; n < components; n++)
        {
          max = (ceil_ratio(origin_x,sub_x[n])+dim_x[n])*sub_x[n];
          min = max - sub_x[n] + 1;
          if ((n==0) || (max < max_size)) max_size = max;
          if ((n==0) || (min > min_size)) min_size = min;
        }
      if (min_size > max_size)
        { kdu_error e; e << "Problem trying to finalize SIZ information. ";
          e << "Horizontal component dimensions and sub-sampling factors are "
            "incompatible. Available information is as follows:\n\n";
          this->textualize_attributes(e); e << "\n";
        }
      size_x = min_size;
      for (n=0; n < components; n++)
        {
          max = (ceil_ratio(origin_y,sub_y[n])+dim_y[n])*sub_y[n];
          min = max - sub_y[n] + 1;
          if ((n==0) || (max < max_size)) max_size = max;
          if ((n==0) || (min > min_size)) min_size = min;
        }
      if (min_size > max_size)
        { kdu_error e; e << "Problem trying to finalize SIZ information. ";
          e << "Vertical component dimensions and sub-sampling factors are "
            "incompatible. Available information is as follows:\n\n";
          this->textualize_attributes(e); e << "\n";
        }
      size_y = min_size;
      set(Ssize,0,0,size_y); set(Ssize,0,1,size_x);
      have_size = true;
    }
  
  // Now for the final verification that all quantities are consistent.
  // Here we set the tiling parameters if necessary.

  assert(have_sampling && have_dims && have_size);
  for (n=0; n < components; n++)
    {
      if (((ceil_ratio(size_x,sub_x[n]) -
            ceil_ratio(origin_x,sub_x[n])) != dim_x[n]) ||
          ((ceil_ratio(size_y,sub_y[n]) -
            ceil_ratio(origin_y,sub_y[n])) != dim_y[n]))
        { kdu_error e; e << "Problem trying to finalize SIZ information. ";
          e << "Dimensions are inconsistent.  Available information is as "
            "follows:\n\n";
          this->textualize_attributes(e); e << "\n";
        }
    }
  
  int tile_x, tile_y, tile_ox, tile_oy;

  if (!(get(Stile_origin,0,0,tile_oy) && get(Stile_origin,0,1,tile_ox)))
    { set(Stile_origin,0,0,tile_oy=origin_y);
      set(Stile_origin,0,1,tile_ox=origin_x); }
  if (!(get(Stiles,0,0,tile_y) && get(Stiles,0,1,tile_x)))
    {
      tile_x = size_x - tile_ox;
      tile_y = size_y - tile_oy;
      set(Stiles,0,0,tile_y); set(Stiles,0,1,tile_x);
    }
  if ((tile_ox > origin_x) || ((tile_ox+tile_x) <= origin_x) ||
      (tile_oy > origin_y) || ((tile_oy+tile_y) <= origin_y))
    { kdu_error e; e << "Problems trying to finalize SIZ information. ";
      e << "Illegal tile origin coordinates.  The first tile must have "
        "a non-empty intersection with the image region on the canvas. "
        "Available information is as follows:\n\n";
      this->textualize_attributes(e); e << "\n";
    }

  delete[] dim_x; delete[] dim_y; delete[] sub_x; delete[] sub_y;
}


/* ========================================================================= */
/*                                cod_params                                 */
/* ========================================================================= */

/*****************************************************************************/
/*                       cod_params::cod_params (no args)                    */
/*****************************************************************************/

cod_params::cod_params()
  : kdu_params(COD_params,true,true,false)
{
  define_attribute(Cycc,
                   "RGB to Luminance-Chrominance conversion?\n"
                   "\t\t[Default is to convert 3 component images]",
                   "B",ALL_COMPONENTS);
  define_attribute(Clayers,
                   "Number of quality layers.\n"
                   "\t\t[Default is 1]",
                   "I",ALL_COMPONENTS);
  define_attribute(Cuse_sop,
                   "Include SOP markers (i.e., resync markers)?\n"
                   "\t\t[Default is no SOP markers]",
                   "B",ALL_COMPONENTS);
  define_attribute(Cuse_eph,
                   "Include EPH markers (marker end of each packet header)?\n"
                   "\t\t[Default is no EPH markers]",
                   "B",ALL_COMPONENTS);
  define_attribute(Corder,
                   "Default progression order (may be overridden by "
                   "POCorder).  The four character identifiers have the "
                   "following interpretation: L=layer; R=resolution; "
                   "C=component; P=position. The first character in the "
                   "identifier refers to the index which progresses most "
                   "slowly, while the last refers to the index which "
                   "progresses most quickly.\n"
                   "\t\t[Default is LRCP]",
                   "(LRCP=0,RLCP=1,RPCL=2,PCRL=3,CPRL=4)",ALL_COMPONENTS);
  define_attribute(Calign_blk_last,
                   "If \"yes\", the code-block partition is aligned so that "
                   "the last sample in each nominal block (ignoring the "
                   "effect of boundaries) is aligned at a multiple of "
                   "the block dimension (a power of 2).  Equivalently, the "
                   "first sample in each nominal block lies at a location "
                   "which is a multiple of the block dimension, plus 1. By "
                   "default (i.e., \"no\"), the first sample of each block is "
                   "aligned at a multiple of the block dimension. The "
                   "alignment is specified separately for both dimensions, "
                   "with the vertical dimension specified first.",
                   "BB",ALL_COMPONENTS);
  define_attribute(Clevels,
                   "Number of wavelet decomposition levels, or stages.\n"
                   "\t\t[Default is 5]",
                   "I");
  define_attribute(Creversible,
                   "Reversible compression?\n"
                   "\t\t[Default is irreversible]",
                   "B");
  define_attribute(Ckernels,
                   "Wavelet kernels to use.\n"
                   "\t\t[Default is W5X3 for reversible "
                   "compression and W9X7 for irreversible compression]",
                   "(W9X7=0,W5X3=1)");
  define_attribute(Cuse_precincts,
                   "Explicitly specify whether or not precinct dimensions "
                   "are supplied.\n"
                   "\t\t[Default is \"no\" unless `Cprecincts' is "
                   "used]",
                   "B");
  define_attribute(Cprecincts,
                   "Precinct dimensions (must be powers of 2). Multiple "
                   "records may be supplied, in which case the first record "
                   "refers to the highest resolution level and subsequent "
                   "records to lower resolution levels. The last specified "
                   "record is used for any remaining lower resolution levels."
                   "Inside each record, vertical coordinates appear first.",
                   "II",MULTI_RECORD | CAN_EXTRAPOLATE);
  define_attribute(Cblk,
                   "Nominal code-block dimensions (must be powers of 2 and "
                   "no less than 4). Actual dimensions are subject to "
                   "precinct, tile and image dimensions. Vertical coordinates "
                   "appear first.\n"
                   "\t\t[Default block dimensions are {64,64}]",
                   "II");
  define_attribute(Cmodes,
                 "Block coder mode switches. Any combination is legal.\n"
                 "\t\t[By default, all mode switches are turned off]",
                 "[BYPASS=1|RESET=2|RESTART=4|CAUSAL=8|ERTERM=16|SEGMARK=32]");
  define_attribute(Clev_weights,
                   "Weighting factors for each successive resolution "
                   "level, starting from the highest resolution and working "
                   "down to the lowest (but not including the LL band!!). The "
                   "last supplied weight is repeated as necessary.  Weight "
                   "values are squared to obtain energy weights for weighted "
                   "MSE calculations.  The LL subband always has a weight of "
                   "1.0, regardless of the number of resolution levels.",
                   "F",MULTI_RECORD | CAN_EXTRAPOLATE);
  define_attribute(Cband_weights,
                   "Weighting factors for each successive subband, "
                   "starting from the highest frequency subbands and working "
                   "down (i.e., HH1, LH1, HL1, HH2, ...). The last supplied "
                   "weight is repeated as necessary for all remaining "
                   "subbands (except the LL band). If `Clev_weights' is also "
                   "supplied, both sets of weighting factors are combined "
                   "(multiplied).  Weight values are squared to obtain energy "
                   "weights for weighted MSE calculations.  The LL subband "
                   "always has a weight of 1.0; although this might not be "
                   "the optimal policy for colour images, it avoids "
                   "problems which may occur when tiles are inter-mixed "
                   "with different numbers of resolution levels.",
                   "F",MULTI_RECORD | CAN_EXTRAPOLATE);
}

/*****************************************************************************/
/*                       cod_params::copy_with_xforms                        */
/*****************************************************************************/

void
  cod_params::copy_with_xforms(kdu_params *source,
                               int skip_components, int discard_levels,
                               bool transpose, bool vflip, bool hflip)
{
  if (comp_idx < 0)
    { // Start with the attributes which are common to all components
      bool use_ycc;
      int num_layers;
      bool use_sop, use_eph;
      int order;
      bool x_last, y_last;

      if (source->get(Cycc,0,0,use_ycc,false))
        {
          if (skip_components)
            use_ycc = false;
          set(Cycc,0,0,use_ycc);
        }
      if (source->get(Clayers,0,0,num_layers,false))
        set(Clayers,0,0,num_layers);
      if (source->get(Cuse_sop,0,0,use_sop,false))
        set(Cuse_sop,0,0,use_sop);
      if (source->get(Cuse_eph,0,0,use_eph,false))
        set(Cuse_eph,0,0,use_eph);
      if (source->get(Corder,0,0,order,false))
        set(Corder,0,0,order);
      if (source->get(Calign_blk_last,0,(transpose)?1:0,y_last,false) &&
          source->get(Calign_blk_last,0,(transpose)?0:1,x_last,false))
        {
          if (hflip) x_last = !x_last;
          if (vflip) y_last = !y_last;
          set(Calign_blk_last,0,0,y_last); set(Calign_blk_last,0,1,x_last);
        }
    }

  // Now for the tile-component specific attributes
  int num_levels;
  bool reversible;
  int kernels;
  bool use_precincts;
  int blk_x, blk_y;
  int modes;

  if (source->get(Clevels,0,0,num_levels,false))
    {
      num_levels -= discard_levels;
      if (num_levels < 0)
        { kdu_error e; e << "Attempting to discard too many resolution "
          "levels!  Cannot discard more resolution levels than there are "
          "DWT levels."; }
      set(Clevels,0,0,num_levels);
    }
  if (source->get(Creversible,0,0,reversible,false))
    set(Creversible,0,0,reversible);
  if (source->get(Ckernels,0,0,kernels,false))
    set(Ckernels,0,0,kernels);
  if (source->get(Cuse_precincts,0,0,use_precincts,false))
    set(Cuse_precincts,0,0,use_precincts);
  if (source->get(Cblk,0,(transpose)?1:0,blk_y,false) &&
      source->get(Cblk,0,(transpose)?0:1,blk_x,false))
    { set(Cblk,0,0,blk_y); set(Cblk,0,1,blk_x); }
  if (source->get(Cmodes,0,0,modes,false))
    set(Cmodes,0,0,modes);

  int x, y;
  if (source->get(Cprecincts,discard_levels,(transpose)?1:0,y,false) &&
      source->get(Cprecincts,discard_levels,(transpose)?0:1,x,false))
    { // Copy precinct dimensions
      set(Cprecincts,0,0,y); set(Cprecincts,0,1,x);
      int n = 1;
      while (source->get(Cprecincts,discard_levels+n,(transpose)?1:0,y,
                         false,false) &&
             source->get(Cprecincts,discard_levels+n,(transpose)?0:1,x,
                         false,false))
        {
          set(Cprecincts,n,0,y); set(Cprecincts,n,1,x);
          n++;
        }
    }
}

/*****************************************************************************/
/*                       cod_params::write_marker_segment                    */
/*****************************************************************************/

int
  cod_params::write_marker_segment(kdu_output *out, kdu_params *last_marked,
                                   int tpart_idx)
{
  cod_params *ref = (cod_params *) last_marked;
  int length, n;
  bool use_ycc, use_sop, use_eph, align_last_x, align_last_y;
  int layers, order;
  bool reversible, use_precincts;
  int levels, kernels, blk_y, blk_x, modes;
  int ppx, ppy;

  if (tpart_idx != 0)
    return 0;

  // Collect most of the parameters.

  if (!(get(Cycc,0,0,use_ycc) && get(Clayers,0,0,layers) &&
        get(Cuse_sop,0,0,use_sop) && get(Cuse_eph,0,0,use_eph) &&
        get(Corder,0,0,order) &&
        get(Calign_blk_last,0,0,align_last_y) &&
        get(Calign_blk_last,0,1,align_last_x) &&
        get(Clevels,0,0,levels) && get(Creversible,0,0,reversible) &&
        get(Ckernels,0,0,kernels) && get(Cblk,0,0,blk_y) &&
        get(Cblk,0,1,blk_x) && get(Cmodes,0,0,modes) &&
        get(Cuse_precincts,0,0,use_precincts)))
    { kdu_error e; e << "Unable to write COD marker segment yet! "
      "Some info missing."; }

  if (ref != NULL)
    { // See if we can skip marker segment generation.
      if (ref->compare(Cycc,0,0,use_ycc) && ref->compare(Clayers,0,0,layers) &&
          ref->compare(Cuse_sop,0,0,use_sop) &&
          ref->compare(Cuse_eph,0,0,use_eph) &&
          ref->compare(Corder,0,0,order) &&
          ref->compare(Calign_blk_last,0,0,align_last_y) &&
          ref->compare(Calign_blk_last,0,1,align_last_x) &&
          ref->compare(Clevels,0,0,levels) &&
          ref->compare(Creversible,0,0,reversible) &&
          ref->compare(Ckernels,0,0,kernels) &&
          ref->compare(Cblk,0,0,blk_y) && ref->compare(Cblk,0,1,blk_x) &&
          ref->compare(Cmodes,0,0,modes) &&
          ref->compare(Cuse_precincts,0,0,use_precincts))
        { // So far, so good. Just need to check on precincts now.
          if (!use_precincts)
            return 0; // Object identical to reference.
          for (n=0; n <= levels; n++)
            if (!(get(Cprecincts,n,0,ppy) && get(Cprecincts,n,1,ppx) &&
                  ref->compare(Cprecincts,n,0,ppy) &&
                  ref->compare(Cprecincts,n,1,ppx)))
              break;
          if (n > levels)
            return 0; // Object identical to reference.
        }
    }
  
  if ((layers < 0) || (layers >= (1<<16)))
    { kdu_error e; e << "Illegal number of quality layers, " << layers << "."; }
  if ((levels < 0) || (levels > 32))
    { kdu_error e; e << "Illegal number of DWT levels, " << levels << ". "
      "Legal range is 0 to 32!"; }

  int component_bytes = (get_num_comps() <= 256)?1:2;

  if (comp_idx < 0)
    { // Generating a COD marker.
      length = 4 + 1 + (1+2+1);
      if (use_ycc)
        { // Check compatibility
          kdu_params *siz = access_cluster(SIZ_params);
          assert(siz != NULL);
          kdu_params *coc[3];
          coc[0] = access_relation(tile_idx,0);
          coc[1] = access_relation(tile_idx,1);
          coc[2] = access_relation(tile_idx,2);
          bool rev_test; coc[0]->get(Creversible,0,0,rev_test);
          int prec_test;
          siz->get(Sprecision,0,0,prec_test);
          for (n=1; (n < 3) && (coc[n] != NULL); n++)
            {
              if (!(coc[n]->compare(Creversible,0,0,rev_test) &&
                    siz->compare(Sprecision,n,0,prec_test)))
                { kdu_error e; e << "You cannot use a colour transform "
                  "unless the first 3 image components have identical "
                  "bit-depths and are either all reversible or all "
                  "irreversible."; }
            }
        }
    }
  else
    { // Generating a COC marker.
      length = 4 + component_bytes + 1;
    }
  length += 5 + use_precincts*(levels+1);
  if (out == NULL)
    return length;

  int acc_length = 0; // Accumulate actual length for verification purposes.

  if (comp_idx < 0)
    {
      if (align_last_x || align_last_y)
        { kdu_warning w; w << "The \"Calign_blk_last\" options are the "
          "subject of a proposed ammendment (PDAM) to JPEG2000, Part 1. "
          "Even if the PDAM is unsuccessful, Part 2 of the standard will "
          "support this useful feature."; }
      int style = (((int) use_precincts) << 0) + (((int) use_sop) << 1) +
                  (((int) use_eph) << 2) + (((int) align_last_x) << 3) +
                  (((int) align_last_y) << 4);
      acc_length += out->put(KDU_COD);
      acc_length += out->put((kdu_uint16)(length-2));
      acc_length += out->put((kdu_byte) style);
      acc_length += out->put((kdu_byte) order);
      acc_length += out->put((kdu_uint16) layers);
      acc_length += out->put((kdu_byte)((use_ycc)?1:0));
    }
  else
    {
      int style = (((int) use_precincts) << 0);
      acc_length += out->put(KDU_COC);
      acc_length += out->put((kdu_uint16)(length-2));
      if (component_bytes == 1)
        acc_length += out->put((kdu_byte) comp_idx);
      else
        acc_length += out->put((kdu_uint16) comp_idx);
      acc_length += out->put((kdu_byte) style);
    }

  int xcb, ycb;

  if (((xcb = int2log(blk_x)) < 0) || ((ycb = int2log(blk_y)) < 0))
    { kdu_error e; e << "Maximum code-block dimensions must be powers of 2!"; }
  if ((xcb < 2) || (ycb < 2) || ((xcb+ycb) > 12))
    { kdu_error e; e << "Maximum code-block dimensions must be no less than 4 "
      "and the maximum code-block area must not exceed 4096 samples!"; }

  acc_length += out->put((kdu_byte) levels);
  acc_length += out->put((kdu_byte)(xcb-2));
  acc_length += out->put((kdu_byte)(ycb-2));
  acc_length += out->put((kdu_byte) modes);
  if ((kernels == Ckernels_W5X3) && (!reversible))
    { kdu_warning w; w << "The irreversible W5X3 DWT kernels were dropped "
      "from FPDAM-1 at the Singapore meeting in March, 2001.  You can still "
      "use them with Kakadu for experimentation purposes."; }
  if (kernels == Ckernels_W5X3)
    acc_length += out->put((kdu_byte)((reversible)?1:2));
  else
    {
      acc_length += out->put((kdu_byte) 0);
      if (reversible || (kernels != Ckernels_W9X7))
        { kdu_error e; e << "You must use the W5X3 kernels for reversible "
          "compression and either the W5X3 or the W9X7 kernels for "
          "irreversible compression."; }
    }
  if (use_precincts)
    for (n=0; n <= levels; n++)
      {
        int ppx, ppy;

        if (!(get(Cprecincts,levels-n,0,ppy) &&
              get(Cprecincts,levels-n,1,ppx)))
          { kdu_error e; e << "No precinct dimensions supplied for COD/COC!"; }
        if (((ppx = int2log(ppx)) < 0) || ((ppy = int2log(ppy)) < 0))
          { kdu_error e;
            e << "Precinct dimensions must be exact powers of 2!"; }
        if ((ppx > 15) || (ppy > 15))
          { kdu_error e; e << "Precinct dimensions may not exceed 2^15!"; }
        acc_length += out->put((kdu_byte)(ppx+(ppy<<4)));
      }

  assert(length == acc_length);
  return length;
}

/*****************************************************************************/
/*                        cod_params::read_marker_segment                    */
/*****************************************************************************/

bool
  cod_params::read_marker_segment(kdu_uint16 code, int num_bytes,
                                  kdu_byte bytes[], int tpart_idx)
{
  kdu_byte *bp, *end;
  bool use_precincts;

  if (tpart_idx != 0)
    return false;
  bp = bytes;
  end = bp + num_bytes;
  if (comp_idx < 0)
    { // Need COD
      if (code != KDU_COD)
        return false;
      try {
        int style = kdu_read(bp,end,1);
        if (style != (style & 31))
          { kdu_error e;
            e << "Malformed COD marker segment encountered. "
            "Invalid \"Scod\" value!";
          }
        set(Cuse_precincts,0,0,use_precincts = (style&1)?true:false);
        set(Cuse_sop,0,0,(style&2)?true:false);
        set(Cuse_eph,0,0,(style&4)?true:false);
        set(Calign_blk_last,0,1,(style&8)?true:false);
        set(Calign_blk_last,0,0,(style&16)?true:false);
        set(Corder,0,0,kdu_read(bp,end,1));
        set(Clayers,0,0,kdu_read(bp,end,2));
        set(Cycc,0,0,kdu_read(bp,end,1));
        } // End of try block.
      catch (kdu_byte *) { kdu_error e;
        e << "Malformed COD marker segment encountered. "
          "Marker segment is too small.";
        }
    }
  else
    { // Need COC for success
      int which_comp;

      if (code != KDU_COC)
        return false;
      if (get_num_comps() <= 256)
        which_comp = *(bp++);
      else
        { which_comp = *(bp++); which_comp = (which_comp<<8) + *(bp++); }
      if (which_comp != comp_idx)
        return false;
      try {
        int style = kdu_read(bp,end,1);
        if (style != (style & 1))
          { kdu_error e; e << "Malformed COC marker segment. "
            "Invalid \"Scoc\" value!"; }
        set(Cuse_precincts,0,0,use_precincts = (style&1)?true:false);
        } // End of try block.
      catch (kdu_byte *) { kdu_error e;
        e << "Malformed COC marker segment encountered. "
          "Marker segment is too small.";
        }
    }

  try {
    bool reversible;
    int levels, n;

    set(Clevels,0,0,levels=kdu_read(bp,end,1));
    set(Cblk,0,1,1<<(2+kdu_read(bp,end,1)));
    set(Cblk,0,0,1<<(2+kdu_read(bp,end,1)));
    set(Cmodes,0,0,kdu_read(bp,end,1));
    int xforms = kdu_read(bp,end,1);
    set(Creversible,0,0,reversible=(xforms==1));
    if (reversible)
      set(Ckernels,0,0,Ckernels_W5X3);
    else if (xforms == 0)
      set(Ckernels,0,0,Ckernels_W9X7);
    else if (xforms == 2)
      set(Ckernels,0,0,Ckernels_W5X3);
    else
      { kdu_error e; e << "Illegal DWT kernels specification found in "
        "COD/COC marker segment."; }
    if (use_precincts)
      for (n=0; n <= levels; n++)
        {
          int ppx, ppy;

          ppx = kdu_read(bp,end,1);
          ppy = ppx >> 4;
          ppx &= 0x0F;
          set(Cprecincts,levels-n,0,1<<ppy);
          set(Cprecincts,levels-n,1,1<<ppx);
        }
    if (bp != end)
      { kdu_error e;
        e << "Malformed COD/COC marker segment encountered. The final "
          << (int)(end-bp) << " bytes were not consumed!";
      }
    } // End of try block.
  catch (kdu_byte *) { kdu_error e;
    e << "Malformed COD/COC marker segment encountered. "
      "Marker segment is too small.";
    }

  return true;
}

/*****************************************************************************/
/*                            cod_params::finalize                           */
/*****************************************************************************/

void
  cod_params::finalize()
{
  int val;
  bool bval;

  if (!get(Clayers,0,0,val))
    set(Clayers,0,0,1);
  if (!get(Cuse_sop,0,0,val))
    set(Cuse_sop,0,0,0);
  if (!get(Cuse_eph,0,0,val))
    set(Cuse_eph,0,0,0);
  if (!get(Corder,0,0,val))
    set(Corder,0,0,Corder_LRCP);
  if (!get(Calign_blk_last,0,0,val))
    {
      assert(!get(Calign_blk_last,0,1,val));
      set(Calign_blk_last,0,0,0);
      set(Calign_blk_last,0,1,0);
    }
  if (!get(Clevels,0,0,val))
    set(Clevels,0,0,5);

  bool reversible;
  if (get(Creversible,0,0,reversible,false) &&
      !get(Ckernels,0,0,val,false))
    set(Ckernels,0,0,(reversible)?Ckernels_W5X3:Ckernels_W9X7);
  if (!get(Creversible,0,0,reversible))
    set(Creversible,0,0,reversible=false); // Default is always irreversible.
  if (!get(Ckernels,0,0,val))
    set(Ckernels,0,0,(reversible)?Ckernels_W5X3:Ckernels_W9X7);

  if (!get(Cblk,0,0,val))
    {
      assert(!get(Cblk,0,1,val));
      set(Cblk,0,0,64);
      set(Cblk,0,1,64);
    }
  if (!get(Cmodes,0,0,val))
    set(Cmodes,0,0,0);

  bool use_ycc;
  if (!get(Cycc,0,0,use_ycc))
    { // Determine the default, based on component compatibility
      assert(comp_idx < 0);
      int components = get_num_comps();
      use_ycc = false; // No transform unless the following conditions are met
      if (components == 3)
        {
          kdu_params *siz = access_cluster(SIZ_params);
          int precision, last_precision;
          bool rev, last_rev;
          int sub_x, last_sub_x, sub_y, last_sub_y, c;
          assert(siz != NULL);
          for (c=0; c < 3; c++, last_rev=rev, last_precision=precision,
               last_sub_x=sub_x, last_sub_y=sub_y)
            {
              kdu_params *coc = access_relation(tile_idx,c);
              if (!(coc->get(Creversible,0,0,rev) &&
                siz->get(Sprecision,c,0,precision) &&
                siz->get(Ssampling,c,0,sub_y) &&
                siz->get(Ssampling,c,1,sub_x)))
                assert(0);
              if (c == 0)
                {
                  last_rev = rev; last_precision = precision;
                  last_sub_x = sub_x; last_sub_y = sub_y;
                }
              else if ((rev != last_rev) || (precision != last_precision) ||
                       (sub_x != last_sub_x) || (sub_y != last_sub_y))
                break;
            }
          if (c == 3)
            use_ycc = true; // All conditions satisfied for component transform
        }
      set(Cycc,0,0,use_ycc);
    }
  if (use_ycc && (this->get_num_comps() < 3))
    set(Cycc,0,0,false);

  // We have now only to consider precincts.

  if (get(Cprecincts,0,0,val,false,false))
    { // Precinct dimensions explicitly set for this tile-component.
      set(Cuse_precincts,0,0,true);
    }
  if (!get(Cuse_precincts,0,0,bval))
    set(Cuse_precincts,0,0,false); // Default is not to use precincts.
}


/* ========================================================================= */
/*                                qcd_params                                 */
/* ========================================================================= */

/*****************************************************************************/
/*                       qcd_params::qcd_params (no args)                    */
/*****************************************************************************/

qcd_params::qcd_params()
  : kdu_params(QCD_params,true,true,false)
{
  define_attribute(Qguard,
                   "Number of guard bits to prevent overflow in the magnitude "
                   "bit-plane representation. Typical values are 1 or 2.\n"
                   "\t\t[Default is 1]",
                   "I");
  define_attribute(Qderived,
                   "Quantization steps derived from LL band parameters? "
                   "If \"yes\", all quantization step sizes will be related "
                   "to the LL subband's step sizes through appropriate powers "
                   "of 2 and only the LL band step size will be written in "
                   "code-stream markers. Otherwise, a separate step size will "
                   "be recorded for every subband. You cannot use this option "
                   "with reversible compression.\n"
                   "\t\t[Default is not derived]",
                   "B");
  define_attribute(Qstep,
                   "Base step size to be used in deriving irreversible "
                   "quantization step sizes for every subband. The base "
                   "step parameter should be in the range 0 to 2.\n"
                   "\t\t[Default is 1/256]",
                   "F");
  define_attribute(Qabs_steps,
                   "Absolute quantization step sizes for each subband, "
                   "expressed as a fraction of the nominal dynamic range "
                   "for that subband. The nominal range is equal to 2^B "
                   "(B is the image sample bit-depth) multiplied by "
                   "the DC gain of each low-pass subband analysis filter and "
                   "the AC gain of each high-pass subband analysis filter, "
                   "involved in the construction of the relevant subband. "
                   "The bands are described one by one, in the following "
                   "sequence: LL_D, HL_D, LH_D, ..., HL_1, LH_1, HH_1. Here, "
                   "D denotes the number of DWT levels. A single "
                   "step size must be supplied for every subband (there is "
                   "no extrapolation), except in the event that `Qderived' is "
                   "set to \"yes\" -- then, only one parameter is allowed, "
                   "corresponding to the LL_D subband.\n"
                   "\t\t[For compressors, the absolute "
                   "step sizes are ignored if `Qstep' has been used.]",
                   "F",MULTI_RECORD); // No extrapolation.
  define_attribute(Qabs_ranges,
                   "Number of range bits used to code each subband during "
                   "reversible compression.  Subbands appear in the sequence, "
                   "LL_D, HL_D, LH_D, ..., HL_1, LH_1, HH_1, where D denotes "
                   "the number of DWT levels.  The number of range bits for a "
                   "reversibly compressed subband, plus the number of guard "
                   "bits (see `Qguard'), is equal to 1 plus the number of "
                   "magnitude bit-planes which are used for coding its "
                   "samples.\n"
                   "\t\t[For compressors, most users will accept the "
                   "default policy, which sets the number of range bits to "
                   "the smallest value which is guaranteed to avoid overflow "
                   "or underflow in the bit-plane representation, "
                   "assuming that the RCT (colour transform) is used.  If "
                   "explicit values are supplied, they must be given for "
                   "each and every subband.]",
                   "I",MULTI_RECORD); // No extrapolation.
}

/*****************************************************************************/
/*                       qcd_params::copy_with_xforms                        */
/*****************************************************************************/

void
  qcd_params::copy_with_xforms(kdu_params *source,
                               int skip_components, int discard_levels,
                               bool transpose, bool vflip, bool hflip)
{
  int guard;
  bool derived;
  if (source->get(Qguard,0,0,guard,false))
    set(Qguard,0,0,guard);
  if (source->get(Qderived,0,0,derived,false))
    set(Qderived,0,0,derived);

  float step;
  if (source->get(Qabs_steps,0,0,step,false))
    {
      set(Qabs_steps,0,0,step);
      for (int n=1; source->get(Qabs_steps,n,0,step,false,false); n++)
        {
          int n_out = n;
          if (transpose)
            {
              if ((n % 3) == 1)
                n_out++; // HL band becomes LH band
              else if ((n % 3) == 2)
                n_out--; // LH band becomes HL band
            }
          set(Qabs_steps,n_out,0,step);
        }
    }

  int range;
  if (source->get(Qabs_ranges,0,0,range,false))
    {
      set(Qabs_ranges,0,0,range);
      for (int n=1; source->get(Qabs_ranges,n,0,range,false,false); n++)
        {
          int n_out = n;
          if (transpose)
            {
              if ((n % 3) == 1)
                n_out++; // HL band becomes LH band
              else if ((n % 3) == 2)
                n_out--; // LH band becomes HL band
            }
          set(Qabs_ranges,n_out,0,range);
        }
    }
}

/*****************************************************************************/
/*                       qcd_params::write_marker_segment                    */
/*****************************************************************************/

int
  qcd_params::write_marker_segment(kdu_output *out, kdu_params *last_marked,
                                   int tpart_idx)
{
  qcd_params *ref = (qcd_params *) last_marked;
  int length, n;
  int guard, levels;
  bool reversible, derived;

  if (tpart_idx != 0)
    return 0;
  kdu_params *cod = access_cluster(COD_params);
  assert(cod != NULL);
  cod = cod->access_relation(tile_idx,comp_idx);
  if (!((cod != NULL) &&
        cod->get(Clevels,0,0,levels) &&
        cod->get(Creversible,0,0,reversible)))
    { kdu_error e; e << "Cannot write QCD/QCC marker segment without first "
      "completing relevant COD/COC information!"; }
  if (!get(Qguard,0,0,guard))
    { kdu_error e; e << "Cannot write QCD/QCC marker segment yet! "
      "No info on guard bits."; }
  if (reversible)
    derived = false;
  else if (!get(Qderived,0,0,derived))
    { kdu_error e; e << "Cannot write QCD/QCC marker segment yet!  Not "
      "clear whether quant steps are derived from the LL band step size."; }
  
  int num_bands = (derived)?1:(1+3*levels);

  if (ref != NULL)
    { // See if we can avoid writing this marker.
      int ref_levels;
      bool ref_reversible;
      kdu_params *ref_cod = cod->access_relation(ref->tile_idx,ref->comp_idx);
      assert(ref_cod != NULL);
      if (!((ref_cod != NULL) &&
            ref_cod->get(Clevels,0,0,ref_levels) &&
            ref_cod->get(Creversible,0,0,ref_reversible)))
        { kdu_error e; e << "Cannot write QCD/QCC marker segment without "
          "first completing all relevant COD/COC information!"; }
      if ((ref_reversible == reversible) && (ref_levels == levels) &&
          ref->compare(Qguard,0,0,guard) &&
          (reversible || ref->compare(Qderived,0,0,derived)))
        { // Just need to check the actual quantization parameters now.
          for (n=0; n < num_bands; n++)
            if (reversible)
              {
                int range;
                if (!get(Qabs_ranges,n,0,range))
                  { kdu_error e;
                    e << "Cannot write QCD/QCC marker segment yet!  "
                    "Absolute reversible ranging information not available."; }
                if (!ref->compare(Qabs_ranges,n,0,range))
                  break;
              }
            else
              {
                float step;
                if (!get(Qabs_steps,n,0,step))
                  { kdu_error e;
                    e << "Cannot write QCD/QCC marker segment yet!  "
                    "Absolute step size information not available."; }
                if (!ref->compare(Qabs_steps,n,0,step))
                  break;
              }
          if (n == num_bands)
            return 0; // No need to explicitly write a marker here.
        }
    }

  // We are now committed to writing (or simulating) a marker segment.

  if ((guard > 7) || (guard < 0))
    { kdu_error e; e << "Illegal number of guard bits, " << guard
      << ". Legal range is from 0 to 7."; }

  int component_bytes = (get_num_comps() <= 256)?1:2;
  
  if (comp_idx < 0)
    length = 4 + 1;
  else
    length = 4 + component_bytes + 1;
  length += num_bands * (2-reversible);
  if (out == NULL)
    return length;

  // Now for actually writing out the marker.

  int acc_length = 0; // Count actual written bytes for consistency checking.
  int style = (guard << 5) + ((reversible)?0:(2-derived));
  if (comp_idx < 0)
    {
      acc_length += out->put(KDU_QCD);
      acc_length += out->put((kdu_uint16)(length-2));
      acc_length += out->put((kdu_byte) style);
    }
  else
    {
      acc_length += out->put(KDU_QCC);
      acc_length += out->put((kdu_uint16)(length-2));
      if (component_bytes == 1)
        acc_length += out->put((kdu_byte) comp_idx);
      else
        acc_length += out->put((kdu_uint16) comp_idx);
      acc_length += out->put((kdu_byte) style);
    }
  for (n=0; n < num_bands; n++)
    if (reversible)
      {
        int val;

        if (!get(Qabs_ranges,n,0,val))
          { kdu_error e; e << "Insufficient absolute ranging parameters "
            "available for writing QCD/QCC marker segment"; }
        if ((val < 0) || (val > 31))
          { kdu_error e; e << "Absolute ranging parameters for reversibly "
            "compressed subbands must be non-negative, no larger than 31!";
          }
        acc_length += out->put((kdu_byte)(val<<3));
      }
    else
      {
        float val;
        int eps, mu;

        if (!get(Qabs_steps,n,0,val))
          { kdu_error e; e << "Insufficient absolute quantization step size "
            "parameters available for writing QCD/QCC marker segment."; }
        step_to_eps_mu(val,eps,mu);
        acc_length += out->put((kdu_uint16)((eps<<11)+mu));
      }
  assert(length == acc_length);
  return length;
}

/*****************************************************************************/
/*                        qcd_params::read_marker_segment                    */
/*****************************************************************************/

bool
  qcd_params::read_marker_segment(kdu_uint16 code, int num_bytes,
                                  kdu_byte bytes[], int tpart_idx)
{
  kdu_byte *bp, *end;

  if (tpart_idx != 0)
    return false;
  bp = bytes;
  end = bp + num_bytes;
  if (comp_idx < 0)
    { // Need QCD for success
      if (code != KDU_QCD)
        return false;
    }
  else
    { // Need QCC for success
      int which_comp;

      if (code != KDU_QCC)
        return false;
      if (get_num_comps() <= 256)
        which_comp = *(bp++);
      else
        { which_comp = *(bp++); which_comp = (which_comp<<8) + *(bp++); }
      if (which_comp != comp_idx)
        return false;
    }

  try {
    int style = kdu_read(bp,end,1);
    bool reversible, derived;
    int n;

    set(Qguard,0,0,(style>>5));
    style &= 31;
    if (style == 0)
      { reversible = true; derived = false; }
    else if (style == 1)
      { reversible = false; derived = true; }
    else if (style == 2)
      { reversible = false; derived = false; }
    else
      { kdu_error e; e << "Undefined style byte found in QCD/QCC marker "
        "segment!"; }
    if (!reversible)
      set(Qderived,0,0,derived);
    if (reversible)
      for (n=0; bp < end; n++)
        set(Qabs_ranges,n,0,kdu_read(bp,end,1)>>3);
    else
      for (n=0; bp < (end-1); n++)
        {
          int val = kdu_read(bp,end,2);
          int mu, eps;
          float step;
          
          mu = val & ((1<<11)-1);
          eps = val >> 11;
          step = ((float) mu) / ((float)(1<<11)) + 1.0F;
          step = step / ((float)(1<<eps));
          set(Qabs_steps,n,0,step);
        }
    if (n < 1)
      throw bp;
    if (bp != end)
      { kdu_error e;
        e << "Malformed QCD/QCC marker segment encountered. The final "
          << (int)(end-bp) << " bytes were not consumed!";
      }
    } // End of try block.
  catch (kdu_byte *) { kdu_error e;
    e << "Malformed QCD/QCC marker segment encountered. "
      "Marker segment is too small.";
    }

  return true;
}

/*****************************************************************************/
/*                               qcd_params::finalize                        */
/*****************************************************************************/

void
  qcd_params::finalize()
{
  int val, guard_bits;
  float fval;

  if (!get(Qguard,0,0,guard_bits))
    set(Qguard,0,0,guard_bits=1); // Default is 1 guard bit.

  kdu_params *cod = access_cluster(COD_params);
  assert(cod != NULL);
  cod = cod->access_relation(tile_idx,comp_idx);
  assert(cod != NULL);
  int reversible, num_levels, num_bands, kernels;
  if (!(cod->get(Creversible,0,0,reversible) &&
        cod->get(Clevels,0,0,num_levels) &&
        cod->get(Ckernels,0,0,kernels)))
    assert(0);
  num_bands = 1+3*num_levels;
  int abs_steps=0, abs_ranges=0;
  while (get(Qabs_steps,abs_steps,0,fval,true,true,false))
    abs_steps++;
  while (get(Qabs_ranges,abs_ranges,0,val,true,true,false))
    abs_ranges++;

  if (reversible)
    {
      if (!get(Qderived,0,0,val))
        set(Qderived,0,0,false);
      if (abs_ranges >= num_bands)
        return; // Have all the info we need.

      // Implement default policy for absolute ranges.
      int precision;
      kdu_params *siz = access_cluster(SIZ_params);
      if (!siz->get(Sprecision,
                    ((comp_idx<0)?0:comp_idx),0,precision))
        { kdu_error e; e << "Trying to finalize quantization parameter "
          "attributes without first providing any information about the "
          "image component bit-depths (i.e. \"Sprecision\")."; }

      /* The following ranging parameters are based on the folowing:
         1) To avoid overflow/underflow we need `epsilon'+G >= `precision'+B
            where `epsilon' is the ranging parameter, G is the number of
            guard bits and B is the worst case (BIBO) bit-depth expansion for
            the reversible DWT.  Calculated values for B are given in
            Table 17.4 of the book by Taubman and Marcellin.  In particular,
            we find that B never exceeds 2 for the LL band, 3 for the HL and
            LH bands and 4 for the HH band.
         2) We need to allow an extra bit in the chrominance channels if
            the RCT is used.  Since a single set of ranging parameters may
            have to work for some tile-components which use the RCT and
            others which do not, it is safest to always allow this extra bit.
         3) The BIBO gain values are accurate only for moderate to high
            bit-depth (precision) imagery.  At lower precisions, it is safer
            to increase the ranging parameters. */
      if (precision < 5)
        precision++;
      set(Qabs_ranges,0,0,precision+2-guard_bits+1); // LL band
      for (int n=0; n < num_levels; n++)
        {
          set(Qabs_ranges,1+3*n,0,precision+3-guard_bits+1); // HL band
          set(Qabs_ranges,2+3*n,0,precision+3-guard_bits+1); // LH band
          set(Qabs_ranges,3+3*n,0,precision+4-guard_bits+1); // HH band
        }
      set_derived(Qabs_ranges);
      return;
    }

  // Processing for irreversible step sizes is more complex.

  float ref_step;
  bool derived_from_LL=false;

  if (get(Qstep,0,0,ref_step))
    { // Ignore any existing step sizes in this case.
      if (get(Qabs_steps,0,0,fval,false,false,false))
        { kdu_warning w;
          w << "Some absolute step sizes which you have supplied will be "
          "ignored, since `Qstep' has been used or a default value for "
          "`Qstep' has been forced.  If you want to specify explicit absolute "
          "step sizes, you must not use `Qstep' anywhere in the inheritance "
          "path of the relevant tile-component.";
        }
      if (!get(Qderived,0,0,derived_from_LL))
        set(Qderived,0,0,derived_from_LL=false);
      derive_absolute_steps(this,num_levels,kernels,ref_step,derived_from_LL);
      set_derived(Qabs_steps);
    }
  else if (abs_steps >= num_bands)
    { // Use the available step sizes.
      if (!get(Qderived,0,0,derived_from_LL))
        set(Qderived,0,0,derived_from_LL=false);
    }
  else if (abs_steps == 1)
    {
      if (!get(Qderived,0,0,derived_from_LL))
        set(Qderived,0,0,derived_from_LL=true);
      if (!derived_from_LL)
        { // Can't use existing step sizes.
          if (get(Qabs_steps,0,0,fval,false,false,false))
            { kdu_warning w;
              w << "A single absolute step size (value = "
                << fval << "), which you have provided will be "
                "ignored, since the relevant tile, component or "
                "tile-component also has `Qderived' equal to \"no\".";
            }
          set(Qstep,0,0,ref_step=1.0F/256.0F);
          derive_absolute_steps(this,num_levels,kernels,ref_step,false);
          set_derived(Qabs_steps);
        }
    }
  else
    { // Must derive step sizes, using default reference step.
      if (!get(Qderived,0,0,derived_from_LL))
        set(Qderived,0,0,derived_from_LL=false);
      set(Qstep,0,0,ref_step=1.0F/256.0F);
      derive_absolute_steps(this,num_levels,kernels,ref_step,derived_from_LL);
      set_derived(Qabs_steps);
    }

  // At this point, the object has access to a set of valid step sizes.

  if (derived_from_LL)
    num_bands = 1;
  if (get(Qabs_steps,0,0,fval,false,false,true))
    { /* The attribute is not inherited.  Just make minor corrections to
         ensure that the values conform exactly to the epsilon-mu
         parametrization supported by the code-stream. */
      int n, eps, mu;

      for (n=0; n < num_bands; n++)
        {
          if (!get(Qabs_steps,n,0,fval))
            assert(0);
          step_to_eps_mu(fval,eps,mu);
          set(Qabs_steps,n,0,(1.0F+mu/((float)(1<<11)))/((float)(1<<eps)));
        }
    }
  else
    { /* The step sizes which we have are inherited.  Make a local copy
         to ensure that nothing goes wrong. */
      int n, eps, mu;
      float *values = new float[num_bands];

      for (n=0; n < num_bands; n++)
        if (!get(Qabs_steps,n,0,values[n],true,true,false))
          assert(0);
      for (n=0; n < num_bands; n++)
        {
          step_to_eps_mu(values[n],eps,mu);
          set(Qabs_steps,n,0,(1.0F+mu/((float)(1<<11)))/((float)(1<<eps)));
        }
      set_derived(Qabs_steps);
      delete[] values;
    }
}


/* ========================================================================= */
/*                              rgn_params members                           */
/* ========================================================================= */

/*****************************************************************************/
/*                       rgn_params::rgn_params (no args)                    */
/*****************************************************************************/

rgn_params::rgn_params()
  : kdu_params(RGN_params,true,true,false)
{
  define_attribute(Rshift,
                   "Region of interest up-shift value.  All subband samples "
                   "which are involved in the synthesis of any image sample "
                   "which belongs to the foreground region of an ROI mask "
                   "will be effectively shifted up (scaled by two the power "
                   "of this shift value) prior to quantization.  The "
                   "region geometry is specified independently and is not "
                   "explicitly signalled through the code-stream; instead, "
                   "this shift must be sufficiently large to enable the "
                   "decoder to separate the foreground and background "
                   "on the basis of the shifted sample amplitudes alone.  "
                   "You will receive an appropriate error message if the "
                   "shift value is too small.\n"
                   "\t\t[Default is 0]",
                   "I");
  define_attribute(Rlevels,
                   "Number of initial (highest frequency) DWT levels through "
                   "which to propagate geometric information concerning the "
                   "foreground region for ROI processing.  Additional "
                   "levels (i.e., lower frequency subbands) will be treated "
                   "as belonging entirely to the foreground region.\n"
                   "\t\t[Default is 4]",
                   "I");
  define_attribute(Rweight,
                   "Region of interest significance weight.  Although this "
                   "attribute may be used together with `Rshift', it is "
                   "common to use only one or the other.  All code-blocks "
                   "whose samples contribute in any way to the reconstruction "
                   "of the foreground region of an ROI mask will have their "
                   "distortion metrics scaled by the square of the supplied "
                   "weighting factor, for the purpose of rate allocation.  "
                   "This renders such blocks more important and assigns to "
                   "them relatively more bits, in a manner which "
                   "is closely related to the effect of the `Clevel_weights' "
                   "and `Cband_weights' attributes on the importance of "
                   "whole subbands.  Note that this region weighting "
                   "strategy is most effective when working with large "
                   "images and relatively small code-blocks (or precincts).\n"
                   "\t\t[Default is 1, i.e., no extra weighting]",
                   "F");
}

/*****************************************************************************/
/*                       rgn_params::copy_with_xforms                        */
/*****************************************************************************/

void
  rgn_params::copy_with_xforms(kdu_params *source,
                               int skip_components, int discard_levels,
                               bool transpose, bool vflip, bool hflip)
{
  int shift;
  if (source->get(Rshift,0,0,shift,false))
    set(Rshift,0,0,shift);
}

/*****************************************************************************/
/*                       rgn_params::write_marker_segment                    */
/*****************************************************************************/

int
  rgn_params::write_marker_segment(kdu_output *out, kdu_params *last_marked,
                                   int tpart_idx)
{
  rgn_params *ref = (rgn_params *) last_marked;
  int length;
  int shift;

  if ((tpart_idx != 0) || (comp_idx < 0))
    return 0;

  if (!get(Rshift,0,0,shift))
    shift = 0;
  if (ref != NULL)
    {
      int ref_shift;

      if (!get(Rshift,0,0,ref_shift))
        ref_shift = 0;
      if (shift == ref_shift)
        return 0;
    }
  else if (shift == 0)
    return 0;

  if ((shift > 255) || (shift < 0))
    { kdu_error e; e << "Illegal ROI up-shift, " << shift << ". Legal "
      "range is from 0 to 255!"; }

  int component_bytes = (get_num_comps() <= 256)?1:2;
  
  length = 6+component_bytes;
  if (out == NULL)
    return length;

  int acc_length = 0;

  acc_length += out->put(KDU_RGN);
  acc_length += out->put((kdu_uint16)(length-2));
  if (component_bytes == 1)
    acc_length += out->put((kdu_byte) comp_idx);
  else
    acc_length += out->put((kdu_uint16) comp_idx);
  acc_length += out->put((kdu_byte) 0);
  acc_length += out->put((kdu_byte) shift);

  assert(length == acc_length);
  return length;
}

/*****************************************************************************/
/*                        rgn_params::read_marker_segment                    */
/*****************************************************************************/

bool
  rgn_params::read_marker_segment(kdu_uint16 code, int num_bytes,
                                  kdu_byte bytes[], int tpart_idx)
{
  kdu_byte *bp, *end;

  if ((tpart_idx != 0) || (code != KDU_RGN) || (comp_idx < 0))
    return false;
  bp = bytes;
  end = bp + num_bytes;

  int component_bytes = (get_num_comps() <= 256)?1:2;

  try {
    int which_comp = kdu_read(bp,end,component_bytes);
    if (which_comp != comp_idx)
      return false;
    
    int style = kdu_read(bp,end,1);
    if (style != 0)
      { kdu_error e; e << "Encountered non-Part1 RGN marker segment!"; }
    set(Rshift,0,0,kdu_read(bp,end,1));
    if (bp != end)
      { kdu_error e;
        e << "Malformed RGN marker segment encountered. The final "
          << (int)(end-bp) << " bytes were not consumed!";
      }
    } // End of try block.
  catch (kdu_byte *) { kdu_error e;
    e << "Malformed RGN marker segment encountered. "
      "Marker segment is too small.";
    }

  return true;
}

/*****************************************************************************/
/*                               rgn_params::finalize                        */
/*****************************************************************************/

void
  rgn_params::finalize()
{
  int val;

  if (!get(Rlevels,0,0,val))
    set(Rlevels,0,0,4); // Default is 4 DWT levels of ROI mask propagation.
  if (get(Rshift,0,0,val) && (val > 37))
    { kdu_warning w; w << "Up-shift values in the RGN marker segment should "
      "not need to exceed 37 under any circumstances.  The use of a larger "
      "value, " << val << " in this case, may cause problems."; }
}


/* ========================================================================= */
/*                                 poc_params                                */
/* ========================================================================= */

/*****************************************************************************/
/*                       poc_params::poc_params (no args)                    */
/*****************************************************************************/

poc_params::poc_params()
  : kdu_params(POC_params,true,false,true)
{
  define_attribute(Porder,
                   "Progression order change information. There may be "
                   "multiple instances of this attribute, in which case "
                   "each instance corresponds to a tile-part boundary. "
                   "Each instance may contain one or more progression "
                   "records, each of which defines the order for "
                   "a collection of packets. Each record contains 6 fields. "
                   "The first two fields identify inclusive lower bounds for "
                   "the resolution level and image component indices, "
                   "respectively. The next three fields identify exclusive "
                   "upper bounds for the quality layer, resolution level and "
                   "image component indices, respectively. All indices are "
                   "zero-based, with resolution level 0 corresponding to the "
                   "LL_D subband. The final field in each record identifies "
                   "the progression order to be applied within the indicated "
                   "bounds. This order is applied only to those packets which "
                   "have not already been sequenced by previous records or "
                   "instances.",
                   "IIIII(LRCP=0,RLCP=1,RPCL=2,PCRL=3,CPRL=4)",MULTI_RECORD);
}

/*****************************************************************************/
/*                       poc_params::copy_with_xforms                        */
/*****************************************************************************/

void
  poc_params::copy_with_xforms(kdu_params *source,
                               int skip_components, int discard_levels,
                               bool transpose, bool vflip, bool hflip)
{
  int r_min, c_min, layer_lim, r_lim, c_lim, order;
  if (source->get(Porder,0,0,r_min,false))
    {
      int n=0;
      while (source->get(Porder,n,0,r_min,false,false) &&
             source->get(Porder,n,1,c_min,false,false) &&
             source->get(Porder,n,2,layer_lim,false,false) &&
             source->get(Porder,n,3,r_lim,false,false) &&
             source->get(Porder,n,4,c_lim,false,false) &&
             source->get(Porder,n,5,order,false,false))
        {
          c_min -= skip_components;
          if (c_min < 0)
            c_min = 0;
          c_lim -= skip_components;
          if (c_lim < 1)
            { c_lim = 1; layer_lim = 0; }
          set(Porder,n,0,r_min);
          set(Porder,n,1,c_min);
          set(Porder,n,2,layer_lim);
          set(Porder,n,3,r_lim);
          set(Porder,n,4,c_lim);
          set(Porder,n,5,order);
          n++;
        }
    }
}

/*****************************************************************************/
/*                       poc_params::write_marker_segment                    */
/*****************************************************************************/

int
  poc_params::write_marker_segment(kdu_output *out, kdu_params *last_marked,
                                   int tpart_idx)
{
  poc_params *ref = (poc_params *) last_marked;
  int length, num_records, n;
  int res_min, comp_min, layer_above, res_above, comp_above, order;

  if (tpart_idx != inst_idx)
    return 0;
  for (n=0; get(Porder,n,0,res_min,false); n++);
  num_records = n;
  if (num_records == 0)
    return 0; // No order information available.
  if ((ref != NULL) && (ref->tile_idx != this->tile_idx))
    { // See if we can skip the marker altogether.
      assert((ref->tile_idx < 0) && (inst_idx == 0));
      kdu_params *next_instance = access_relation(tile_idx,comp_idx,1);
      if ((next_instance == NULL) || !next_instance->get(Porder,0,0,res_min))
        { /* This is the only instance; otherwise, we would definitely need
             explicit markers for this tile. */
          for (n=0; n < num_records; n++)
            {
              if (!(get(Porder,n,0,res_min) && get(Porder,n,1,comp_min) &&
                    get(Porder,n,2,layer_above) && get(Porder,n,3,res_above) &&
                    get(Porder,n,4,comp_above) && get(Porder,n,5,order)))
                { kdu_error e; e << "Information required to write POC marker "
                  "segment is not currently complete!"; }
              if (!(ref->compare(Porder,n,0,res_min) &&
                    ref->compare(Porder,n,1,comp_min) &&
                    ref->compare(Porder,n,2,layer_above) &&
                    ref->compare(Porder,n,3,res_above) &&
                    ref->compare(Porder,n,4,comp_above) &&
                    ref->compare(Porder,n,5,order)))
                break;
            }
          if (n < num_records)
            return 0;
        }
    }

  // We are now committed to writing the marker, or at least simulating it.

  int num_components, component_bytes, max_comp_above;
  kdu_params *siz = access_cluster(SIZ_params);
  if ((siz == NULL) || !siz->get(Scomponents,0,0,num_components))
    assert(0);
  if (num_components <= 256)
    { component_bytes = 1; max_comp_above = 256; }
  else
    { component_bytes = 2; max_comp_above = 16384; }

  length = 4 + num_records*(1 + component_bytes + 2 + 1 + component_bytes + 1);
  if (out == NULL)
    return length;

  int acc_length = 0;

  acc_length += out->put(KDU_POC);
  acc_length += out->put((kdu_uint16)(length-2));
  for (n=0; n < num_records; n++)
    {
      if (!(get(Porder,n,0,res_min) && get(Porder,n,1,comp_min) &&
            get(Porder,n,2,layer_above) && get(Porder,n,3,res_above) &&
            get(Porder,n,4,comp_above) && get(Porder,n,5,order)))
        { kdu_error e; e << "Information required to write POC marker "
          "segment is not currently complete!"; }
      if ((res_min < 0) || (res_min >= 33))
        { kdu_error e; e << "Illegal lower bound, " << res_min <<
          ", for resolution level indices in progression order change "
          "attribute.  Legal range is from 0 to 32."; }
      if ((res_above <= res_min) || (res_above > 33))
        { kdu_error e; e << "Illegal upper bound (exclusive), " << res_above <<
          ", for resolution level indices in progression order change "
          "attribute.  Legal range is from the lower bound + 1 to 33."; }
      if ((comp_min < 0) || (comp_min >= max_comp_above))
        { kdu_error e; e << "Illegal lower bound, " << comp_min <<
          ", for component indices in progression order change attribute. "
          "Legal range is from 0 to " << max_comp_above-1 << "."; }
      if ((comp_above <= comp_min) || (comp_above > max_comp_above))
        { kdu_error e; e << "Illegal upper bound (exclusive), "
          << comp_above <<
          ", for component indices in progression order change attribute. "
          "Legal range is from the lower bound + 1 to " << max_comp_above << "."; }
      if ((layer_above < 0) || (layer_above >= (1<<16)))
        { kdu_error e; e << "Illegal upper bound (exclusive), "
          << layer_above <<
          ", for layer indices in progression order change attribute. "
          "Legal range is from 0 to " << (1<<16)-1 << "."; }

      if ((comp_above >= max_comp_above) && (component_bytes == 1))
        comp_above = 0; // Interpreted as 256

      acc_length += out->put((kdu_byte) res_min);
      if (component_bytes == 1)
        acc_length += out->put((kdu_byte) comp_min);
      else
        acc_length += out->put((kdu_uint16) comp_min);
      acc_length += out->put((kdu_uint16) layer_above);
      acc_length += out->put((kdu_byte) res_above);
      if (component_bytes == 1)
        acc_length += out->put((kdu_byte) comp_above);
      else
        acc_length += out->put((kdu_uint16) comp_above);
      acc_length += out->put((kdu_byte) order);
    }

  assert(length == acc_length);
  return length;
}

/*****************************************************************************/
/*                        poc_params::read_marker_segment                    */
/*****************************************************************************/

bool
  poc_params::read_marker_segment(kdu_uint16 code, int num_bytes,
                                  kdu_byte bytes[], int tpart_idx)
{
  kdu_byte *bp, *end;
  int num_records, n;

  if (code != KDU_POC)
    return false;
  bp = bytes;
  end = bp + num_bytes;

  int num_components, component_bytes;
  kdu_params *siz = access_cluster(SIZ_params);
  if ((siz == NULL) || !siz->get(Scomponents,0,0,num_components))
    assert(0);
  component_bytes = (num_components <= 256)?1:2;
  
  try {
    num_records = num_bytes / (1+component_bytes+2+1+component_bytes+1);
    if (num_records < 1)
      throw bp;
    for (n=0; n < num_records; n++)
      {
        set(Porder,n,0,kdu_read(bp,end,1));
        set(Porder,n,1,kdu_read(bp,end,component_bytes));
        set(Porder,n,2,kdu_read(bp,end,2));
        set(Porder,n,3,kdu_read(bp,end,1));
        int comp_above = kdu_read(bp,end,component_bytes);
        if ((comp_above == 0) && (component_bytes == 1))
          comp_above = 256;
        set(Porder,n,4,comp_above);
        set(Porder,n,5,kdu_read(bp,end,1));
      }
    if (bp != end)
      { kdu_error e;
        e << "Malformed POC marker segment encountered. The final "
          << (int)(end-bp) << " bytes were not consumed!";
      }
    } // End of try block.
  catch (kdu_byte *) { kdu_error e;
    e << "Malformed POC marker segment encountered. "
      "Marker segment is too small.";
    }

  return true;
}


/* ========================================================================= */
/*                                crg_params                                 */
/* ========================================================================= */

/*****************************************************************************/
/*                       crg_params::crg_params (no args)                    */
/*****************************************************************************/

crg_params::crg_params()
  : kdu_params(CRG_params,false,false,false)
{
  define_attribute(CRGoffset,
                   "Provides additional component registration offsets. "
                   "The offsets add to those implied by the canvas "
                   "coordinate system and should only be used when "
                   "canvas coordinates (notably `Ssize', `Soffset' and "
                   "`Ssampling') cannot be found, which adequately reflect "
                   "the relative displacement of the components. Each record "
                   "specifies offsets for one component, with the vertical "
                   "offset appearing first. Offsets must be in the range "
                   "0 (inclusive) to 1 (exclusive) and represent a fraction "
                   "of the relevant component sub-sampling factor (see "
                   "`Ssampling'). The last supplied record is repeated "
                   "as needed to recover offsets for all components. ",
                   "FF",MULTI_RECORD | CAN_EXTRAPOLATE);
}

/*****************************************************************************/
/*                        crg_params::copy_with_xforms                       */
/*****************************************************************************/

void
  crg_params::copy_with_xforms(kdu_params *source,
                               int skip_components, int discard_levels,
                               bool transpose, bool vflip, bool hflip)
{
  if (vflip || hflip)
    { // Can't deal with flipping.  Best to destroy any existing information.
      delete_unparsed_attribute(CRGoffset);
    }
  float y, x;
  int n = 0;
  while (source->get(CRGoffset,n,(transpose)?1:0,y,false,false) &&
         source->get(CRGoffset,n,(transpose)?0:1,x,false,false))
    {
      if (n >= skip_components)
        {
          set(CRGoffset,n-skip_components,0,y);
          set(CRGoffset,n-skip_components,1,x);
        }
      n++;
    }
  if ((n > 0) && (n <= skip_components))
    { // Copy last actual offsets into the first target component.
      set(CRGoffset,0,0,y);
      set(CRGoffset,0,1,x);
    }
}

/*****************************************************************************/
/*                       crg_params::write_marker_segment                    */
/*****************************************************************************/

int
  crg_params::write_marker_segment(kdu_output *out, kdu_params *last_marked,
                                   int tpart_idx)
{
  crg_params *ref = (crg_params *) last_marked;
  int length;
  float xoff, yoff;

  if ((tpart_idx != 0) || (comp_idx >= 0))
    return 0;
  if (!get(CRGoffset,0,0,yoff))
    return 0;
  assert(ref == NULL);

  int num_components, c;
  kdu_params *siz = access_cluster(SIZ_params);
  if ((siz == NULL) || !siz->get(Scomponents,0,0,num_components))
    assert(0);

  length = 4 + 4*num_components;
  if (out == NULL)
    return length;

  int acc_length = 0;

  acc_length += out->put(KDU_CRG);
  acc_length += out->put((kdu_uint16)(length-2));
  for (c=0; c < num_components; c++)
    {
      int x, y;

      if (!(get(CRGoffset,c,0,yoff) && get(CRGoffset,c,1,xoff)))
        { kdu_error e; e << "Component registration information incomplete!"; }
      if ((xoff < 0.0F) || (xoff >= 1.0F) || (yoff < 0.0F) || (yoff >= 1.0F))
        { kdu_error e; e << "Illegal component registration offsets, {"
          << y << "," << x << "}.  Legal range is from 0.0 to 1.0 "
          "(exclusive).";}
      x = (int) floor(0.5F + xoff * (float)(1<<16));
      if (x >= (1<<16))
        x = (1<<16) - 1;
      y = (int) floor(0.5F + yoff * (float)(1<<16));
      if (y >= (1<<16))
        y = (1<<16) - 1;
      acc_length += out->put((kdu_uint16) x);
      acc_length += out->put((kdu_uint16) y);
    }

  assert(length == acc_length);
  return length;
}

/*****************************************************************************/
/*                        crg_params::read_marker_segment                    */
/******************************************************************************/

bool
  crg_params::read_marker_segment(kdu_uint16 code, int num_bytes,
                                  kdu_byte bytes[], int tpart_idx)
{
  kdu_byte *bp, *end;

  if ((tpart_idx != 0) || (code != KDU_CRG) || (comp_idx >= 0))
    return false;
  bp = bytes;
  end = bp + num_bytes;

  int num_components, c;
  kdu_params *siz = access_cluster(SIZ_params);
  if ((siz == NULL) || !siz->get(Scomponents,0,0,num_components))
    assert(0);

  try {
    for (c=0; c < num_components; c++)
      {
        set(CRGoffset,c,1,kdu_read(bp,end,2)/((float)(1<<16)));
        set(CRGoffset,c,0,kdu_read(bp,end,2)/((float)(1<<16)));
      }
    if (bp != end)
      { kdu_error e;
        e << "Malformed CRG marker segment encountered. The final "
          << (int)(end-bp) << " bytes were not consumed!";
      }
    } // End of try block.
  catch (kdu_byte *) { kdu_error e;
    e << "Malformed CRG marker segment encountered. "
      "Marker segment is too small.";
    }

  return true;
}
