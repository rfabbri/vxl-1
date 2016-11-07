/*****************************************************************************/
// File: kdu_params.h [scope = CORESYS/COMMON]
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
   Defines a code-stream parameter management system.  The system manages
information which might be stored in code-stream marker segments, as well
as some information which cannot be preserved in any well-defined way
within a JPEG2000 code-stream.  The derived object names, "siz_params",
"cod_params", "qcd_params", etc., are intended to identify the association
with particular types of code-stream marker segments (SIZ, COD, QCD, etc.).
Services are provided to parse and generate such code-stream marker segments.
However, the design here is intended to provide a more uniform view of
code-stream attributes than that offered by the JPEG2000 standard's marker
segments.
   Parameters which belong together (e.g., all COD/COC type parameters) are
grouped into what we call parameter clusters.  Each cluster has a collection
of defined attributes which may be queried, textualized, translated and so
forth, in a variety of different ways.  Each cluster generally has multiple
incarnations.  The primary incarnation is known as the "tile-head", which
represents the "fall-back" attribute values to be used for all tiles, in the
event that tile- or tile-component specific information is not available.
Additional incarnations exist for every tile-component, and also as
"component-heads" (fall-back attributes for a component, when no specific
information is provided for a tile) and "tile-component" heads (fall-back
attributes for components of a specific tile, when no specific information
is provided for that tile-component).  An elaborate system is provided to
implement fall-back policies and to provide the various "lock-outs" required
to ensure that information cannot be specialized to tiles or tile-components
when that is forbidden by the standard.
   The services defined here are sensitive to the fact that code-stream
parameters may become available incrementally (say while decompressing an
existing JPEG2000 code-stream incrementally).
   Services are provided to copy a network of code-stream parameters or
transform them into suitable subsets or geometrically adjusted views, for
the benefit of transcoding applications.  These services support incremental
availability of the information.
   Services are also provided to generate human readable descriptions of
any or all of the code-stream parameter attributes and to create attributes
from descriptions in this same form (used, for example, with command-line
argument or switch-file input).  The parameters are also capable of generating
rich or comprehensive descriptions of themselves and their interpretation.
******************************************************************************/

#ifndef KDU_PARAMS_H
#define KDU_PARAMS_H

#include <assert.h>
#include <iostream>
#include "kdu_elementary.h"

// Defined here:

class kdu_params;
class siz_params;
class cod_params;
class qcd_params;
class rgn_params;
class poc_params;
class crg_params;

// Referenced here, defined elsewhere:

struct kd_attribute;

/*****************************************************************************/
/*                                kdu_output                                 */
/*****************************************************************************/

#define KDU_OBUF_SIZE 512

class kdu_output {
  /* This abstract base class must be derived to construct meaningful
     output targets.  Since there may be many low level byte-oriented
     transactions, we emphasize efficiency for such transactions. */
  public: // Member functions
    kdu_output()
      { next_buf = buffer; end_buf = buffer+KDU_OBUF_SIZE; }
    virtual ~kdu_output() { return; }
     // Derived objects should usually provide a destructor to flush the buffer
    int put(kdu_byte byte)
      { // Always returns 1.
        if (next_buf == end_buf)
          { flush_buf(); assert(next_buf < end_buf); }
        *(next_buf++) = byte;
        return 1;
      }
    int put(kdu_uint16 word)
      { // Writes the word in big-endian order and returns 2.
        put((kdu_byte)(word>>8));
        put((kdu_byte)(word>>0));
        return 2;
      }
    int put(kdu_uint32 word)
      { // Writes the word in big-endian order and returns 4.
        put((kdu_byte)(word>>24));
        put((kdu_byte)(word>>16));
        put((kdu_byte)(word>>8));
        put((kdu_byte)(word>>0));
        return 4;
      }
    void write(kdu_byte *buf, int count)
      {
        while (count > 0)
          {
            int xfer_bytes = end_buf - next_buf;
            if (xfer_bytes == 0)
              { flush_buf(); xfer_bytes = end_buf - next_buf; }
            xfer_bytes = (count < xfer_bytes)?count:xfer_bytes;
            count -= xfer_bytes;
            while (xfer_bytes--)
              *(next_buf++) = *(buf++);
          }
      }
  protected: // Data and functions used for buffer management.
    virtual void flush_buf() = 0;
      /* Flushes the buffer and returns with `next_buf' = `buffer'. */
    kdu_byte buffer[KDU_OBUF_SIZE];
    kdu_byte *next_buf; // Points to the next location to be written
    kdu_byte *end_buf; // Points immediately beyond the end of the buffer.
  };

/*****************************************************************************/
/*                                    kdu_params                             */
/*****************************************************************************/

class kdu_params {
  /* This abstract base class must be derived to one of the complete
     parameter classes defined below. Each complete parameter class
     represents the parameters embodied by a single type of code-stream
     marker segment. Parameter objects may be generated in one of three ways:
     1) from text strings which describe specific attributes (these might be
     entered on a command line); 2) from a code-stream marker segment; and
     3) by explicitly setting parameter attribute values. Parameter objects
     may be serialized in one of two ways: 1) by writing text strings which
     describe specific attributes; and 2) by writing code-stream marker
     segments.
         Parameter objects are collected into clusters, which represent
     related code-stream marker segments from different tiles and different
     image components. In particular, each kdu_params object is identified
     within a cluster by 3 coordinates: its tile index; its component index;
     and its instance index. Tile indices must start from -1; component
     indices must start from -1; and instance indices must start from 0. In
     some cases multiple components, multiple tiles or multiple instances
     will be disallowed.
         The cluster is organized firstly as a list of tiles, where the
     first object in the list must have a tile index of -1, identifying it
     as a summary object for all tiles. This object is known as the
     cluster head. Each of the objects in this first list serves as a
     "tile head" for all of the component objects associated with a tile.
     These tile head objects must have component indices of -1, identifying
     them as summary objects for all components in the tile.  A list of
     tile-component objects is built from each tile head and each of these
     serves as a component head, having an instance index of 0. It heads
     a list of one or more instances for the tile-component. */
  // --------------------------------------------------------------------------
  public: // Lifecycle functions
    KDU_EXPORT
      kdu_params(const char *cluster_name, bool allow_tile_diversity,
                 bool allow_component_diversity,
                 bool allow_instance_diversity);
      /* Constructs a new individual object, having tile and component indices
         of -1 (as is required for cluster and tile heads, respectively) and
         an instance index of 0 (as is required for tile-component heads).
         To link the object into a cluster or list of clusters, the `link'
         member function must subsequently be called. */
    KDU_EXPORT
      virtual ~kdu_params();
      /* When destroying an object which is the head of any list, the entire
         list will be destroyed. Objects with instance indices of 0 are
         tile-component heads (they destroy all instances in the
         tile-component).  Tile-component heads with component indices of -1
         are tile heads (they destroy all components in the tile).  Tile heads
         with tile indices of -1 are cluster heads (they destroy all tiles in
         the cluster).  Destroying the first cluster head in the cluster list
         causes all clusters to be destroyed. */
    KDU_EXPORT kdu_params *
      link(kdu_params *existing, int tile_idx, int comp_idx);
      /* Links the object into a list of existing object clusters, of which
         `existing' must be a member. In the process, the tile index and
         component index of the new object are replaced by the supplied values.
         Since the object being linked must have been constructed using the
         above constructor, its tile and component indices will both be -1 on
         entry. If the existing list of clusters already contains an instance
         of this tile-component, a new instance is created automatically.
         The function returns a pointer to `this', for convenience. */
    virtual kdu_params *new_instance() = 0;
      /* Adds a new instance to the current object's instance list, returning
         a pointer to the instance. The current object need not be the head
         or the tail of its instance list. The function must be overridden in
         derived classes, where it will normally invoke the relevant
         constructor. */
    KDU_EXPORT void
      copy_from(kdu_params *source, int source_tile, int target_tile,
                int instance=-1, int skip_components=0,
                int discard_levels=0, bool transpose=false,
                bool vflip=false, bool hflip=false);
      /* This is a very powerful function, which may be very useful in
         implementing transcoding operations.  In the simplest case, the
         function copies the attributes of the `source' object into the
         current object, generating an error if they do not belong to exactly
         the same derived class.
            Copying will not occur unless the source and current objects'
         tile indices agree with the `source_tile' and `target_tile' arguments,
         respectively.  If both objects are the heads of their respective
         instance lists, all instances are visited until one is found which
         agrees with `instance', creating new instances for the target object
         if necessary.  If `instance' is -1, all instances are
         copied from the source.  If one or both objects is not the head of
         its instance list, at most that one object will be copied and then
         only if agreement can be reached with the value of the `instance'
         argument.
            When the source and/or target object is the head of a list of
         tile-components for some tile, all corresponding components are
         copied, skipping over the first `skip_components' components of
         the source list (but not the target list).  When the source and/or
         target object is the head of a list of tiles for some parameter
         cluster, the function automatically locates the tiles which match
         the `source_tile' and `target_tile' arguments before attempting a
         copy.  Finally, and most usefully, when either or both objects is
         the head of a list of parameter clusters, all corresponding clusters
         are visited and copied in accordance with the rules outlined above.
            Note carefully that no `kdu_params' object will be copied if the
         object has already had any record of any of its attributes set by
         any means.  In this event, the object is skipped, but an attempt is
         still made to copy any other objects in the relevant lists, following
         the rules outlined above.  The intent is to copy source objects into
         newly constructed (and hence empty) objects; however, the fact that
         individual objects will not be overwritten can come in handy when
         copying large parameter groupings, only some of which have already
         been set.
            The actual copying operation is performed by the internal function,
         "copy_with_xforms", which is overridden by each derived class.
         It is supplied the last 4 arguments, which may modify the data as it
         is being copied. */
  protected: // Specific transcoding members implemented by each derived class
    virtual void copy_with_xforms(kdu_params *source, int skip_components,
                                  int discard_levels, bool transpose,
                                  bool vflip, bool hflip) = 0;
      /* Each derived class must implement this pure virtual copy function.
         It may fail with an error message if any of the requested
         transformations cannot be implemented.
            The `skip_components' argument indicates the number of initial
         image components which are being discarded in a transcoding operation.
         This may impact some of the parameter attribute values.
            The `discard_levels' argument indicates the number of DWT
         decomposition levels which are being discarded in a transcoding
         operation to produce a reduced resolution version of the image.
            If `vflip' is true, the target representation should
         be a vertically flipped version of the source representation.
         If `hflip' is true, the target representation should be a
         horizontally flipped version of the source.  If `transpose' is true,
         the source representation is tranposed.  When `transpose' is used
         together with `vflip', the interpretation is that the top-most row
         in the target representation should be equivalent to the right-most
         column in the source representation.  That is, we think of first
         tranposing the source representation and then flipping it afterwards.
         The same principle applies for combinations of `transpose' and
         `hflip'.  That is, the left-most target column should be equivalent
         to the bottom-most source row. */
  // --------------------------------------------------------------------------
  public: // Navigation and identification member functions
    const char *identify_cluster()
      { return cluster_name; }
    KDU_EXPORT kdu_params *
      access_cluster(const char *cluster_name);
    KDU_EXPORT kdu_params *
      access_cluster(int sequence_idx);
      /* Locate a particular cluster, either by its name, or by its position
         in the underlying cluster list. The first cluster has
         `sequence_idx'=0 and may also be retrieved by supplying a NULL
         `cluster_name' argument.  Relevant `cluster_name' strings are defined
         with the derived class definitions, `siz_params', `cod_params', etc.,
         which appear toward the end of this file. */
    int get_instance()
      { return inst_idx; }
    int get_num_comps()
      { assert(first_inst->num_comps > 0); return first_inst->num_comps; }
      /* Fails if current object does not belong to a list of tile-components.
         Otherwise, returns the number of components (objects with non-negative
         component indices) in the list. */
    KDU_EXPORT kdu_params *
      access_relation(int tile_idx, int comp_idx, int inst_idx=0);
      /* Locate a particular object within the same cluster. If the object
         does not exist, the function returns NULL. */
  // --------------------------------------------------------------------------
  protected: // Assistance in constructing derived parameter classes
    KDU_EXPORT void
      define_attribute(const char *name, const char *comment,
                       const char *pattern, int flags=0);
      /* This function is called by the derived class's initializer to
         define all recognized attributes for the relevant class.
         `name' points to a string which may be used for textualizing
         the attribute. Relevant attribute names are defined with the derived
         classes toward the end of this file. `comment' provides a textual
         description of the attribute which may be used in generating usage
         statements. Valid `flags' are as follows: MULTI_RECORD means that
         multiple records are allowed; CAN_EXTRAPOLATE means that records
         can be accessed beyond those which have been actually written, with
         the missing elements extrapolated from those which are available;
         ALL_COMPONENTS means that this attribute does not have a
         component-specific form and may only be set in an object whose
         component index is -1. `pattern' identifies the structure of each
         field. The string contains a concatenation of any or all of the
         following:
            * "F" -- a floating point field
            * "I" -- an integer field
            * "B" -- a boolean field (textualized as "yes/no")
            * "(<string1>=<val>,<string2>=<val>,...)" -- an integer field,
              translated from one of the strings in the comma separated list
              of string/value pairs. Translation is case-sensitive.
            * "(<string1>=<val>|<string2>=<val>|...)" -- as above, but multiple
              strings, sparated by "|" symbols, may be translated and their
              values OR'd together to form the integer value. This is useful
              for building flag words. */
    static const int MULTI_RECORD;
    static const int CAN_EXTRAPOLATE;
    static const int ALL_COMPONENTS;
  // --------------------------------------------------------------------------
  public: // Binary attribute access functions
    KDU_EXPORT bool
      get(const char *name, int record_idx, int field_idx, int &value,
          bool allow_inherit=true, bool allow_extend=true,
          bool allow_derived=true);
    KDU_EXPORT bool
      get(const char *name, int record_idx, int field_idx, bool &value,
          bool allow_inherit=true, bool allow_extend=true,
          bool allow_derived=true); // Bools may also be retrieved as ints
    KDU_EXPORT bool
      get(const char *name, int record_idx, int field_idx, float &value,
          bool allow_inherit=true, bool allow_extend=true,
          bool allow_derived=true);
      /* The attribute `name' string is one of the names defined for the
         various derived classes, `siz_params', `cod_params', etc., appearing
         toward the end of this file.  Full string matching is not performed.
         The function generates an error if the request is incompatible with
         the record structure defined for this attribute. Returns false if the
         requested value has not yet been set and it cannot be synthesized
         from information available within the cluster. Synthesis rules are
         rather involved and may be explicitly manipulated by supplying the
         final two arguments.
            If an attribute has insufficient records, the existing records may
         be extended (extrapolated) so long as the CAN_EXTRAPOLATE flag was
         supplied when the attribute was defined and the `allow_extend'
         argument is true. If the attribute is empty, values may be
         synthesized by appealing to the same record in another object in the
         cluster, so long as the `allow_inherit' argument is true. The
         function first tries to use a summary object from the same tile
         (component index = -1); it then tries to use a summary object from
         the same component (tile index = -1); finally, it tries to use a
         summary object for all tiles and components (component and tile
         indices both -1). These rules apply only for objects whose
         instance number is 0. Other instances cannot be synthesized from
         other objects in the cluster. If none of these mechanisms succeed,
         the function returns false.
            If the `allow_derived' argument is false, the function will
         ignore any attributes which have been marked as holding derived
         quantities via the `set_derived' member function.  This is useful
         when you want to inherit attributes from objects in the same
         cluster, bypassing any intermediate objects for which values
         might have been automatically derived (usually inside the
         `finalize' function). */
    bool compare(const char *name, int record_idx, int field_idx, int value)
      { int val;
        return (get(name,record_idx,field_idx,val) && (val==value)); }
    bool compare(const char *name, int record_idx, int field_idx, bool value)
      { bool val;
        return (get(name,record_idx,field_idx,val) && (val==value)); }
    bool compare(const char *name, int record_idx, int field_idx, float value)
      { float val;
        return (get(name,record_idx,field_idx,val) && (val==value)); }
      /* Convenience functions which return false unless the indicated
         field and record of the attribute identified by `name' exists and
         has the same value as the supplied `value' argument.  These
         functions greatly simplify marker segment writing. */
    KDU_EXPORT void
      set(const char *name, int record_idx, int field_idx, int value);
    KDU_EXPORT void
      set(const char *name, int record_idx, int field_idx, bool value);
    KDU_EXPORT void
      set(const char *name, int record_idx, int field_idx, double value);
      /* Attribute `name' strings are defined with the various derived classes,
         `siz_params', `cod_params', etc., appearing toward the end of this
         file.  The function generates an error if the request is no attribute
         of this name is defined for the object, or the call is incompatible
         with the record structure defined for this attribute.  The number of
         records available for the attribute grows as required.  Note that
         boolean values may be written using either of the first two forms of
         the function. */
    KDU_EXPORT void
      set_derived(const char *name);
      /* Marks the attribute identified by `name' as holding automatically
         derived data which might not be treated in the same way as
         original data.  In particular, derived attributes might be
         skipped when the inheritance chain used by the `get' member
         function.  Also, derived attributes might be skipped during
         textualization. */
  // --------------------------------------------------------------------------
  public: // String oriented attribute access functions
    KDU_EXPORT bool
      parse_string(char *string);
      /* Parses a single attribute from the supplied string. The string may
         not contain any white space of any form at all. An error message
         will generally be produced if this requirement is violated. The
         function examines the available attributes for one whose name
         matches the initial portion of the supplied string. The attribute
         name is delimited by an '=' or a ':' character (see below). If the
         match fails, the function proceeds to test successive clusters. If
         no match is found, the function returns false.
            Once a match is found, the function checks for an optional
         tile-component locator which consists of a ':' followed by either
         a tile-specifier of the form "T<num>", a component-specifier of the
         form "C<num>", or both. Tile and component numbers start from 0.
         If a tile-component specifier is disallowed or does not correspond
         to a valid object, the function generates an error message.
            The string concludes with an '=' sign followed by one or more
         records, delimited by commas. Multiple records are disallowed unless
         the attribute was defined with the MULTI_RECORD flag. Each record
         contains one or more fields, surrounded in curly braces.  In the
         special case where there is only one field per record, the braces may
         be skipped. The fields must match the type specifications provided in
         the `pattern' string with which the attribute was defined. Any
         failure to obey these rules will cause an error message to be
         generated.
            Attribute parsing is unlike the binary or marker based attribute
         setting functions, in several respects.  Firstly, it is illegal to
         parse the same attribute twice -- an appropriate error message will
         be generated if information has already been parsed into the
         attribute.  The only exception to this rule applies to objects which
         support multiple instances, in which case a new instance of the
         object will be created automatically to accommodate multiple
         occurrences of the same attribute string.  The multiple instances rule
         does not apply to attributes which has been set by means other than
         parsing.  Quite to the contrary, any information set by any other
         means is automatically deleted before parsing new information into an
         attribute.  More than that, if the attribute string does not specify
         a particular component, the same attribute will be deleted across all
         components which do not already contain parsed information for the
         attribute.  Similarly, if the attribute string does not specify a
         particular tile, the same attribute will be deleted across all tiles
         which do not already contain parsed information.  This functionality
         is intended to reflect the most likely intention of a transcoding user
         who wishes to modify some parameter attributes of an existing
         code-stream prior to transcoding. */
    KDU_EXPORT bool
      parse_string(char *string, int tile_idx);
      /* Same as above, but parses only those strings which refer
         to the indicated tile.  As usual, a tile index of -1 refers to
         the summary object for all tiles. */
    KDU_EXPORT void
      textualize_attributes(std::ostream &stream, bool skip_derived=true);
      /* Textualizes all attributes for which information has been written,
         using the same format as described above for `parse_string' and
         writing the result out to the supplied stream.  The `skip_derived'
         argument controls whether or not attributes marked as holding
         automatically derived quantities (using the `set_derived' member
         function) should be skipped over. */
    KDU_EXPORT void
      textualize_attributes(std::ostream &stream, int min_tile, int max_tile,
                            bool skip_derived=true);
      /* Same as above, with two important differences: 1) only objects whose
         tile indices lie between `min_tile' and `max_tile' (inclusive) have
         their attributes textualized; and 2) the function textualizes all
         objects which lie within a list which is headed by the current object.
         If the object for which this function is invoked is a tile-component
         head, all instances for that tile-component have their attributes
         textualized. Similarly, if the object is a tile head, all
         tile-components for that tile have their attributes textualized and
         if the object is the cluster head, all objects in the cluster have
         their attributes textualized. Finally, if the object is the head of
         a list of clusters, all clusters will be textualized. */
    KDU_EXPORT void
      describe_strings(std::ostream &stream, bool include_comments=true);
      /* Print a text description of the attributes which are valid for
         the object.  If `include_comments' is true, descriptive comments
         will also be printed. */
    KDU_EXPORT void
      describe_string(const char *name, std::ostream &stream,
                      bool include_comments=true);
      /* Describes a single attribute. */
    KDU_EXPORT kdu_params *
      find_string(char *string, const char * &name);
      /* Finds the object which would be used by `parse_string' if parsing
         the supplied `string', returning a pointer to that object, if it
         exists.  Searches along lists following the same rules as
         `parse_string'.  Returns NULL if a match is not found.  Otherwise,
         the `name' argument is used to return the attribute name string
         (not decorated with tile- or component- specifiers). */
  protected: // Mostly for internal use by the base object.
    KDU_EXPORT void
      delete_unparsed_attribute(const char *name);
      /* Deletes the attribute with this name from the object.  Does the same
         for tile-components in any component list of which it is the head and
         also does the same for tiles in any tile list of which it is the
         head.  Generates an error if there is no attribute of this name. */
  // --------------------------------------------------------------------------
  public: // Marker oriented attribute access functions
    KDU_EXPORT bool
      translate_marker_segment(kdu_uint16 code, int num_bytes,
                               kdu_byte bytes[], int which_tile,
                               int tpart_idx);
      /* This function invokes the protected (and generally overridden)
         `read_marker_segment' member function of every object whose tile
         index agrees with `which_tile', until one is found which returns
         true. If no object can translate the marker segment, the function
         returns false. The marker code and the number of segment bytes are
         explicitly provided. The `bytes' array starts right after the segment
         length indicator and `num_bytes' indicates the length of this array,
         so `num_bytes' is always 2 bytes less than the length indicator in
         the actual code-stream marker segment.  The `tpart_idx' argument
         identifies the tile-part from which the marker segment originates.
         The first tile-part of each tile (and also the main header) has an
         index of 0. */
    KDU_EXPORT int
      generate_marker_segments(kdu_output *out, int which_tile, int tpart_idx);
      /* This function invokes the protected (and generally overridden)
         `write_marker_segment' member function of every object whose tile
         index agrees with `which_tile'. Marker segments are written starting
         from the tile head and working through each tile-component, for each
         cluster in the cluster list to which the current object belongs. The
         function returns the total number of bytes written, including the
         marker codes and segment bytes for all marker segments. If `out'
         is NULL, the process is only simulated -- nothing is written, but
         the return value represents the number of bytes which would be
         written out if `stream' had not been NULL.  The `tpart_idx'
         argument indicates the tile-part whose header is being written.  The
         first tile-part of each tile (and also the main header) have a
         tile-part index of 0. */
  protected: // Individual marker translators
    virtual int write_marker_segment(kdu_output *out, kdu_params *last_marked,
                                     int tpart_idx)
      { return 0; }
      /* This function should be overridden in any derived class which
         is capable of writing its contents into a code-stream marker segment.
         The function is called from within `generate_marker_segments', which
         also provides the `last_marked' pointer. This pointer may be safely
         cast to a pointer to the derived object. If not NULL, the function
         can determine whether or not there is a need to write a marker segment
         for this object by comparing the contents of the last marked object
         with those of the current object. If there is no difference, no marker
         segment need be written. In practice, `last_marked' is the most recent
         object in the current object's inheritance chain for which a marker
         segment was generated (return value not equal to zero). The function
         returns the total number of bytes occupied by the marker segment,
         including the marker code and the segment length field. If `stream'
         is NULL, the process is only simulated -- the length should be
         returned correctly, but nothing is actually written.  The `tpart_idx'
         field identifies the tile-part (starting from 0) into which the
         marker segment is being written. */
    virtual bool read_marker_segment(kdu_uint16 code, int num_bytes,
                                     kdu_byte bytes[], int tpart_idx)
      { return false; }
      /* This function should be overridden in any derived class which
         corresponds to a valid code-stream marker. The function is called
         from within the public `translate_marker_segment' method, which visits
         all relevant objects one by one until one of them is found whose
         `read_marker_segment' function returns true. The marker code is
         supplied, and the `bytes' array holds the data which follows the
         marker length field in the complete code-stream marker segment.
         `num_bytes' indicates the number of bytes in the `bytes' array.
         Thus, `num_bytes' is always 2 bytes less than the marker
         length field in the complete code-stream marker segment.  The function
         should always return false if the segment cannot be translated in
         the current object, even if the most appropriate object for the
         marker segment can be readily deduced.  Note that when a marker
         segment is translated by an object which supports multiple instances,
         a new instance will automatically be created to translate the
         contents of any future marker segments for the same tile-component.
         The `tpart_idx' field indicates the tile-part from whose header the
         marker segment is being read.  The first tile-part has an index
         of 0. */
  // --------------------------------------------------------------------------
  public: // Finalization functions
    virtual void finalize() {return; }
      /* This function is generally invoked through the `finalize_all'
         function described below.  Although it can usually be invoked
         quite safely when code-stream marker segments are used to recover
         attributes, it is intended primarily for use when attributes
         are derived by parsing text strings supplied as command line
         arguments.  Since the information provided in this manner is
         quite sparse, it is frequently necessary to fill in default
         values or convert summary quantities into a more complete
         set of attributes required for writing marker segments or performing
         compression. The function should be overridden in any derived
         class where such processing may be required. */
    KDU_EXPORT void
      finalize_all();
      /* This function invokes the `finalize' member function of the current
         object and every object in the list or lists headed by the current
         object.  If the object is a tile-component head, all instances
         are finalized. If the object is a tile head, all tile-components
         are finalized.  If the object is a cluster head, all objects in
         all tiles of the cluster are finalized.  If the object is the head
         of the list of clusters, all clusters are finalized.  For more
         information, see the definition of the `finalize' function itself. */
    KDU_EXPORT void
      finalize_all(int tile_idx);
      /* Same as above, but only those objects whose tile index agrees with
         the supplied argument are actually finalized.  This is useful when
         information becomes available on a tile-by-tile basis. */
  // --------------------------------------------------------------------------
  protected: // Data shared with derived classes
    const char *cluster_name; // Identitifies the derived class type.
    int tile_idx, comp_idx, inst_idx; // Object coordinates within cluster
  // --------------------------------------------------------------------------
  private: // Data
    bool allow_tiles, allow_comps, allow_insts;
    kdu_params *first_cluster, *next_cluster; /* Links clusters to one another.
                          Links are valid only for the first object in each
                          cluster (cluster heads). Otherwise, they are NULL. */
    kdu_params *first_tile, *next_tile; /* Links tiles within a cluster.
                          Links are only valid for the first object in each
                          tile (tile heads) and are otherwise NULL. */
    kdu_params *first_comp, *next_comp; /* Links components within a tile.
                          Links are only valid for the first object in each
                          component (tile-component heads) and are
                          otherwise NULL. */
    kdu_params *first_inst, *next_inst; /* Links instances of the same
                          tile-component. */
    int num_comps; /* Number of non-initial tile-components in current tile
                      list. Valid only for the head of an instance list. */
    kd_attribute *attributes; /* Points to a list of attributes. */
    friend struct kd_attribute;
    bool empty; // True until the first attempt to set any attribute records.
    bool marked; /* Becomes true when the contents of this object
                    are written to an explicit code-stream marker. */
  };

/*****************************************************************************/
/*                                   siz_params                              */
/*****************************************************************************/

  // Cluster name
#define SIZ_params "SIZ"
  // Attributes recorded in the SIZ marker segment
#define Ssize "Ssize" // One record x "II"
#define Sorigin "Sorigin" // One record x "II"
#define Stiles "Stiles" // One record x "II"
#define Stile_origin "Stile_origin" // One record x "II"
#define Scomponents "Scomponents" // One record x "I"
#define Ssigned "Ssigned" // Multiple records x "B"
#define Sprecision "Sprecision" // Multiple records x "I"
#define Ssampling "Ssampling" // Multiple records x "II"
  // Attributes available only during content creation.
#define Sdims "Sdims" // Multiple records x "II"

class siz_params: public kdu_params {
  public: // Member functions
    KDU_EXPORT
      siz_params();
    virtual kdu_params *new_instance() { return NULL; }
    virtual void finalize();
      /* This function should be called once all available dimension
         information has been supplied to the object, either by parsing
         command line arguments, reading markers, or reading image file
         headers.  The function verifies the consistency of all available
         dimension information and derives and sets any dimensions which
         have not otherwise been set.  For example, we may start with a
         collection of individual component dimensions (set using the `Sdims'
         attribute), from which the function must deduce an appropriate
         set of canvas dimensions and sub-sampling parameters.  The function
         does its best to handle all combinations of inputs.  It generates
         an error if insufficient or conflicting information prevent it from
         determining a full set of consistent dimensions. */
  protected: // Member functions
    virtual void copy_with_xforms(kdu_params *source, int skip_components,
                   int discard_levels, bool transpose, bool vflip, bool hflip);
    virtual int write_marker_segment(kdu_output *out, kdu_params *last_marked,
                                     int tpart_idx);
    virtual bool read_marker_segment(kdu_uint16 code, int num_bytes,
                                     kdu_byte bytes[], int tpart_idx);
  };
  /* Notes:
        Although the JPEG2000 SIZ marker cannot represent negative coordinates,
     it is particularly convenient to allow this object to store and report
     negative coordinates.  These should not trouble applications which
     interface with it, since all of the algebraic properties of the coorinate
     system still hold. Negative coordinates will be converted into appropriate
     non-negative coordinates only when a valid marker segment must be written
     out -- to do this the `write_marker_segment' function must examine various
     `cod_params' attributes from every tile-component in the image to
     determine appropriate offsets to the negative coordinates which will not
     alter the interpretation of the canvas coordinate system.
        For the above reason, you should avoid writing out a SIZ marker segment
     until all code-stream parameters have been finalized for all tiles and
     tile-components. */

/*****************************************************************************/
/*                                   cod_params                              */
/*****************************************************************************/

  // Cluster name
#define COD_params "COD"
  // Attributes recorded in COD marker segments
#define Cycc "Cycc" // One record x "B"
#define Clayers "Clayers" // One record x "I"
#define Cuse_sop "Cuse_sop" // One record x "B"
#define Cuse_eph "Cuse_eph" // One record x "B"
#define Corder "Corder" // One record x "(LRCP=0,RLCP=1,RPCL=2,PCRL=3,CPRL=4)"
#define Calign_blk_last "Calign_blk_last" // One record x "BB"
  // Attributes recorded in COD or COD marker segments
#define Clevels "Clevels" // One record x "I"
#define Creversible "Creversible" // One record x "B"
#define Ckernels "Ckernels" // One record x "(W9X7=0,W5X3=1)"
#define Cuse_precincts "Cuse_precincts" // One record x "B"
#define Cprecincts "Cprecincts" // Multiple records x "II"
#define Cblk "Cblk" // One record x "II"
#define Cmodes "Cmodes" // One record x "[BYPASS=1|RESET=2|RESTART=4|CAUSAL=8|ERTERM=16|SEGMARK=32]"
  // Attributes available only during content creation.
#define Clev_weights "Clev_weights" // Multiple records x "F"
#define Cband_weights "Cband_weights" // Multiple records x "F"
  // Values for the "Corder" attribute
#define Corder_LRCP    ((int) 0)
#define Corder_RLCP    ((int) 1)
#define Corder_RPCL    ((int) 2)
#define Corder_PCRL    ((int) 3)
#define Corder_CPRL    ((int) 4)
  // Values for the "Ckernels" attribute
#define Ckernels_W9X7  ((int) 0)
#define Ckernels_W5X3  ((int) 1)
  // Values for the "Cmodes" attribute
#define Cmodes_BYPASS  ((int) 1)
#define Cmodes_RESET   ((int) 2)
#define Cmodes_RESTART ((int) 4)
#define Cmodes_CAUSAL  ((int) 8)
#define Cmodes_ERTERM  ((int) 16)
#define Cmodes_SEGMARK ((int) 32)

class cod_params: public kdu_params {
  public: // Member Functions
    KDU_EXPORT
      cod_params();
    virtual kdu_params *new_instance() { return NULL; }
    virtual void finalize();
  protected: // Member functions
    virtual void copy_with_xforms(kdu_params *source, int skip_components,
                   int discard_levels, bool transpose, bool vflip, bool hflip);
    virtual int write_marker_segment(kdu_output *out, kdu_params *last_marked,
                                     int tpart_idx);
    virtual bool read_marker_segment(kdu_uint16 code, int num_bytes,
                                     kdu_byte bytes[], int tpart_idx);
  };

/*****************************************************************************/
/*                                   qcd_params                              */
/*****************************************************************************/

  // Cluster name
#define QCD_params "QCD"
  // Attributes recorded in QCD/QCC marker segments
#define Qguard "Qguard" // One record x "I"
#define Qderived "Qderived" // One record x "B"
#define Qabs_steps "Qabs_steps" // Multiple records x "F"; No resizing
#define Qabs_ranges "Qabs_ranges" // Multiple records x "I"; No resizing
  // Attributes available only during content creation.
#define Qstep "Qstep" // One record x "F"

class qcd_params: public kdu_params {
  public: // Member Functions
    KDU_EXPORT
      qcd_params();
    virtual kdu_params *new_instance() { return NULL; }
    virtual void finalize();
  protected: // Member functions
    virtual void copy_with_xforms(kdu_params *source, int skip_components,
                   int discard_levels, bool transpose, bool vflip, bool hflip);
    virtual int write_marker_segment(kdu_output *out, kdu_params *last_marked,
                                     int tpart_idx);
    virtual bool read_marker_segment(kdu_uint16 code, int num_bytes,
                                     kdu_byte bytes[], int tpart_idx);
  };

/*****************************************************************************/
/*                                   rgn_params                              */
/*****************************************************************************/

  // Cluster name
#define RGN_params "RGN"
  // Attributes recorded in RGN marker segments
#define Rshift "Rshift" // One record x "I"
  // Attributes available only during content creation.
#define Rlevels "Rlevels" // One record x "I"
#define Rweight "Rweight" // One record x "F"

class rgn_params: public kdu_params {
  public: // Member Functions.
    KDU_EXPORT
      rgn_params();
    virtual kdu_params *new_instance() { return NULL; }
    virtual void finalize();
  protected: // Member functions
    virtual void copy_with_xforms(kdu_params *source, int skip_components,
                   int discard_levels, bool transpose, bool vflip, bool hflip);
    virtual int write_marker_segment(kdu_output *out, kdu_params *last_marked,
                                     int tpart_idx);
    virtual bool read_marker_segment(kdu_uint16 code, int num_bytes,
                                     kdu_byte bytes[], int tpart_idx);
  };

/*****************************************************************************/
/*                                   poc_params                              */
/*****************************************************************************/

  // Cluster name
#define POC_params "POC"
  // Attributes recorded in POC marker segments
#define Porder "Porder" // Multiple records x "IIIII(LRCP=0,RLCP=1,RPCL=2,PCRL=3,CPRL=4)"

class poc_params: public kdu_params {
  public: // Member Functions
    KDU_EXPORT
      poc_params();
    kdu_params *new_instance()
      { if (tile_idx < 0) return NULL;
        kdu_params *result = new poc_params();
        return result->link(this,tile_idx,comp_idx); }
  protected: // Member functions
    virtual void copy_with_xforms(kdu_params *source, int skip_components,
                   int discard_levels, bool transpose, bool vflip, bool hflip);
    virtual int write_marker_segment(kdu_output *out, kdu_params *last_marked,
                                     int tpart_idx);
    virtual bool read_marker_segment(kdu_uint16 code, int num_bytes,
                                     kdu_byte bytes[], int tpart_idx);
  };

/*****************************************************************************/
/*                                   crg_params                              */
/*****************************************************************************/

  // Cluster name
#define CRG_params "CRG"
  // Attributes recorded in the CRG marker segment
#define CRGoffset "CRGoffset" // Multiple records x "FF"

class crg_params: public kdu_params {
  public: // Member Functions
    KDU_EXPORT
      crg_params();
    kdu_params *new_instance() { return NULL; }
  protected: // Member functions
    virtual void copy_with_xforms(kdu_params *source, int skip_components,
                   int discard_levels, bool transpose, bool vflip, bool hflip);
    virtual int write_marker_segment(kdu_output *out, kdu_params *last_marked,
                                     int tpart_idx);
    virtual bool read_marker_segment(kdu_uint16 code, int num_bytes,
                                     kdu_byte bytes[], int tpart_idx);
  };

#endif // KDU_PARAMS_H
