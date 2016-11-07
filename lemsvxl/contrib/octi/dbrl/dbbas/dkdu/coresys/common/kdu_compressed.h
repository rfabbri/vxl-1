/*****************************************************************************/
// File: kdu_compressed.h [scope = CORESYS/COMMON]
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
   This file provides the key interfaces between the Kakadu framework
and an application which uses its services.  In particular, it defines the
services required to create a codestream interface object and to access the
various sub-ordinate elements: tiles, tile-components, resolutions, subbands
and code-blocks.  It provides high level interfaces which isolate the
application from the actual machinery used to incrementally read, write and
transform JPEG2000 code-streams.  The key data processing objects attach to
one or more of these code-stream interfaces.
   You are strongly encouraged to use the services provided here as-is and
not to meddle with the internal machinery.  Most services which you should
require for compression, decompression, transcoding and interactive
applications are provided.  One notable exception is that there is currently
no direct access to precincts (and hence the packet data which they embody).
Direct access to precincts may be of interest to client-server applications.
The corresponding objects exist in the internal machinery and will be exposed
through appropriate interfaces at a later date.
******************************************************************************/

#ifndef KDU_COMPRESSED_H
#define KDU_COMPRESSED_H

#include <time.h>
#include <iostream>
#include "kdu_params.h"

// Defined here:

struct kdu_coords;
struct kdu_dims;

class kdu_compressed_source;
class kdu_compressed_target;

class kdu_codestream;
class kdu_tile;
class kdu_tile_comp;
class kdu_resolution;
class kdu_subband;

struct kdu_block;

// Referenced here, defined inside the private implementation

struct kd_codestream;
struct kd_tile;
struct kd_tile_comp;
struct kd_resolution;
struct kd_subband;
struct kd_precinct;

/* ========================================================================= */
/*                             Marker Codes                                  */
/* ========================================================================= */

#define KDU_SOC ((kdu_uint16) 0xFF4F)
                // Delimiting marker         -- processed in "codestream.cpp"
#define KDU_SOT ((kdu_uint16) 0xFF90)
                // Delimiting marker segment -- processed in "compressed.cpp"
#define KDU_SOD ((kdu_uint16) 0xFF93)
                // Delimiting marker         -- processed in "compressed.cpp"
#define KDU_SOP ((kdu_uint16) 0xFF91)
                // In-pack-stream marker     -- processed in "compressed.cpp"
#define KDU_EPH ((kdu_uint16) 0xFF92)
                // In-pack-stream marker     -- processed in "compressed.cpp"
#define KDU_EOC ((kdu_uint16) 0xFFD9)
                // Delimiting marker         -- processed in "codestream.cpp"

#define KDU_SIZ ((kdu_uint16) 0xFF51)
                // Parameter marker segment  -- processed in "params.cpp"
#define KDU_COD ((kdu_uint16) 0xFF52)
                // Parameter marker segment  -- processed in "params.cpp"
#define KDU_COC ((kdu_uint16) 0xFF53)
                // Parameter marker segment  -- processed in "params.cpp"
#define KDU_QCD ((kdu_uint16) 0xFF5C)
                // Parameter marker segment  -- processed in "params.cpp"
#define KDU_QCC ((kdu_uint16) 0xFF5D)
                // Parameter marker segment  -- processed in "params.cpp"
#define KDU_RGN ((kdu_uint16) 0xFF5E)
                // Parameter marker segment  -- processed in "params.cpp"
#define KDU_POC ((kdu_uint16) 0xFF5F)
                // Parameter marker segment  -- processed in "params.cpp"
#define KDU_CRG ((kdu_uint16) 0xFF63)
                // Parameter marker segment  -- processed in "params.cpp"

#define KDU_COM ((kdu_uint16) 0xFF64)
                // Packet headers in advance -- processed in "compressed.cpp"
#define KDU_TLM ((kdu_uint16) 0xFF55)
                // Packet headers in advance -- processed in "compressed.cpp"

#define KDU_PLM ((kdu_uint16) 0xFF57)
                // Comment marker -- can safely ignore these
#define KDU_PLT ((kdu_uint16) 0xFF58)
                // Comment marker -- can safely ignore these
#define KDU_PPM ((kdu_uint16) 0xFF60)
                // Comment marker -- can safely ignore these
#define KDU_PPT ((kdu_uint16) 0xFF61)
                // Comment marker -- can safely ignore these


/* ========================================================================= */
/*                     Class and Structure Definitions                       */
/* ========================================================================= */

/*****************************************************************************/
/*                               kdu_coords                                  */
/*****************************************************************************/

struct kdu_coords {
  public: // Data
    int y, x;
  public: // Convenience functions
    kdu_coords() {}
    kdu_coords(int x, int y) {this->x=x; this->y=y; }
    void transpose() {int tmp=y; y=x; x=tmp; }
    kdu_coords operator+(kdu_coords &rhs)
      { kdu_coords result; result.x=x+rhs.x; result.y=y+rhs.y; return result; }
    kdu_coords operator-(kdu_coords &rhs)
      { kdu_coords result; result.x=x-rhs.x; result.y=y-rhs.y; return result; }
    kdu_coords operator+=(kdu_coords &rhs)
      { x+=rhs.x; y+=rhs.y; return *this; }
    kdu_coords operator-=(kdu_coords &rhs)
      { x-=rhs.x; y-=rhs.y; return *this; }
    bool operator==(kdu_coords &rhs)
      { return (x==rhs.x) && (y==rhs.y); }
    bool operator!=(kdu_coords &rhs)
      { return (x!=rhs.x) || (y!=rhs.y); }
  };


/*****************************************************************************/
/*                                kdu_dims                                   */
/*****************************************************************************/

struct kdu_dims {
  public: // Data
    kdu_coords pos;
    kdu_coords size;
  public: // Convenience functions
    int area() { return size.x*size.y; }
    void transpose()
      { size.transpose(); pos.transpose(); }
    kdu_dims operator&(kdu_dims &rhs) // Intersects region with RHS.
      { kdu_dims result = *this; result &= rhs; return result; }
    kdu_dims operator&=(kdu_dims &rhs) // Returns intersection of operands
      {
        kdu_coords lim = pos+size;
        kdu_coords rhs_lim = rhs.pos + rhs.size;
        if (lim.x > rhs_lim.x) lim.x = rhs_lim.x;
        if (lim.y > rhs_lim.y) lim.y  = rhs_lim.y;
        if (pos.x < rhs.pos.x) pos.x = rhs.pos.x;
        if (pos.y < rhs.pos.y) pos.y = rhs.pos.y;
        size = lim-pos;
        if (size.x < 0) size.x = 0;
        if (size.y < 0) size.y = 0;
        return *this;
      }
    bool operator!()
      { return ((size.x>0)&&(size.y>0))?false:true; }
    bool operator==(kdu_dims &rhs)
      { return (pos==rhs.pos) && (size==rhs.size); }
    bool operator!=(kdu_dims &rhs)
      { return (pos!=rhs.pos) || (size!=rhs.size); }
  };
  /* Generic structure for holding location and size information for various
     partitions on the canvas.  The `size' coordinates identify the dimensions
     of the specific tile, tile-component, precinct, code-block, etc., while
     the `pos' coordinates identify its position.  When used to describe
     generic partitions, the nominal partition size is maintained by `size',
     while `pos' holds the anchor point. */

/*****************************************************************************/
/*                             kdu_compressed_source                         */
/*****************************************************************************/

#define KDU_SOURCE_CAP_SEQUENTIAL   0x0001
#define KDU_SOURCE_CAP_SEEKABLE     0x0002
#define KDU_SOURCE_CAP_TILES        0x0004
#define KDU_SOURCE_CAP_PRECINCTS    0x0008

class kdu_compressed_source {
  /* Abstract base class must be derived to create a real compressed data
     source with which to construct an input `kdu_codestream' object.  Supports
     everything from simple file reading sources to caching sources for use
     in sophisticated client-server applications.
        Currently, none of the read scoping functions are implemented by any
     of our compressed data sources, nor can they be exploited by the
     `kdu_codestream' object.  In the future, however, we expect to rely
     heavily upon these functions for supporting rich client-server interaction
     in remote image browsing applications. */
  public: // Member functions
    virtual ~kdu_compressed_source()
      { // Allows destruction of source objects from the abstract reference.
        return;
      }
    virtual void close()
      /* Allows sources to be closed from the abstract base object.  Closure
         might not have meaning for some types of sources, in which case this
         convenience function need not do anything. */
      { return; }
    virtual int read(kdu_byte *buf, int num_bytes) = 0;
      /* This function must be implemented in every derived class.  It
         implements the functionality of a sequential read operation, within
         the current read scope, transferring the next `num_bytes' bytes from
         the scope into the supplied buffer.  The function must return the
         actual number of bytes recovered, which will be less than `num_bytes'
         only if the read scope is exhausted.  The default read scope is the
         entire code-stream, although the object may support tile-, precinct-,
         tile-part- or packet-oriented scopes (see below).
            The system provides its own internal temporary buffering of
         compressed data to maximize internal access efficiency.  For this
         reason, the virtual `read' function defined here will usually be
         called to transfer quite a few bytes at a time. */
    virtual int get_capabilities()
      { /* The return value is the logical OR of one or more capabilities
           flags, whose values are defined above.  The various flags have the
           following interpretation:
            * KDU_SOURCE_CAP_SEQUENTIAL: If this flag is set, the source
                 supports sequential reading of data in the order expected
                 of a valid JPEG2000 code-stream, in either code-stream or
                 tile scope.  If this flag is not set, the
                 KDU_SOURCE_CAP_TILES and KDU_SOURCE_CAP_PRECINCTS flags
                 must both be set.
            * KDU_SOURCE_CAP_SEEKABLE: If this flag is set, the source supports
                 random access via the `set_codestream_scope' function.  The
                 KDU_SOURCE_CAP_SEQUENTIAL flag must also be set.
            * KDU_SOURCE_CAP_TILES: If this flag is set, the source supports
                 restrictions of the read scope to a single tile.  In this
                 case, the `set_tile_scope' function must be implemented.  If
                 KDU_SOURCE_CAP_SEQUENTIAL is also set, the object must support
                 sequential reads of any given tile, with all tile-parts of
                 the tile concatenated for the purpose of `read' calls within
                 the tile scope.
            * KDU_SOURCE_CAP_PRECINCTS: If this flag is set, the source
                 supports restrictions of the read scope to a single precinct.
                 The KDU_SOURCE_CAP_TILES flag must also be set, but
                 KDU_SOURCE_CAP_SEQUENTIAL need not be set.  The
                 `set_precinct_scope' function must be implemented and the
                 object must be capable of presenting all packets for the
                 precinct (or all packets which are currently available --
                 e.g., in a cache) as a single concatenated stream for the
                 purpose of `read' calls from within precinct scope. */
        return KDU_SOURCE_CAP_SEQUENTIAL;
      }
    virtual bool set_codestream_scope(int offset=0)
      { /* This function should be implemented by sources which support
           sequential reading of the code-stream from any position (e.g.,
           seekable file streams).  Subsequent reads transfer data from the
           location `offset' bytes into the code-stream, from the initial
           SOC marker.  If seeking or sequential access is not supported,
           the function should return false.  If seeking is supported, the
           function should return true, even if `offset' lies beyond the
           end of the code-stream. */
        return false;
      }
    virtual bool set_tile_scope(int tnum, int tiles)
      { /* This function should be implemented by sources which support
           non-sequential code-stream organizations (e.g., the client cache
           in an interactive client-server browsing application).  Subsequent
           reads will transfer data starting from the first SOT marker of
           the indicated tile.  The caller provides the total number of tiles
           in the code-stream as a convenience.  The function should return
           false only if the non-sequential functionality is not implemented.
              There are two reasons for setting the read scope to that of a
           single tile.  The first is to allow access to the tile header; the
           second is to provide scope for subsequent `set_precinct_scope'
           calls.
              Non-sequential source objects might not support sequential
           reads beyond the end of the tile header (thereafter, the scope
           may need to be explicitly set to that of a precinct within the
           tile in order to continue reading).  If they do, all tile-parts
           for the tile should appear to have been concatenated from the
           perspective of the `read' function.  In any event, once all data
           available (or accessible) for the tile has been read, subsequent
           `read' calls will return 0 until the next change of scope. */
        return false;
      }
    virtual bool set_precinct_scope(int comp_idx, int components,
                                    int res_idx, int resolutions,
                                    int precinct_idx, int precincts,
                                    int offset=0)
      { /* This function may be implemented by sources which support
           non-sequential code-stream organizations (e.g., the client cache
           in an interactive client-server browsing application).  The
           function may be invoked only from within tile scope (see
           `set_tile_scope').  Subsequent reads will transfer data starting
           from the first packet of the precinct and continuing until no
           more packets remain (thereafter, the `read' calls should return 0
           until the read scope is changed).  The function should return
           false only if it is not implemented.  If the data are not available,
           the function should return true, but subsequent reads will return
           zero.
              Precincts are identified by their component index (from 0 to
           `components'-1), their resolution level index (from 0 to
           `resolutions'-1, where 0 identifies the LL band) and their precinct
           index (from 0 through `precincts'-1).  The caller provides the
           index bounds as a convenience, to simplify the implementation of
           non-sequential sources.
              The optional `offset' argument may be used to specify a byte
           offset from the start of the precinct data, from which reading
           should commence. */
        return false;
      }
  };

/*****************************************************************************/
/*                             kdu_compressed_target                         */
/*****************************************************************************/

class kdu_compressed_target {
  /* Abstract base class must be derived to create a real compressed data
     target with which to construct an output `kdu_codestream' object.
     Supports the generation of complex file formats. */
  public: // Member functions
    virtual ~kdu_compressed_target()
      { // Allows destruction of target objects from the abstract reference.
        return;
      }
    virtual void close()
      /* Allows targets to be closed from the abstract base object.  Closure
         might not have meaning for some types of sources, in which case this
         convenience function need not do anything. */
      { return; }
    virtual bool write(kdu_byte *buf, int num_bytes) = 0;
      /* This function must be implemented in every derived class.  It
         implements the functionality of a sequential write operation,
         transferring `num_bytes' bytes from the supplied buffer to the
         target.  The function returns true unless the transfer could not
         be completed for some reason, in which case it returns false.
            The system provides its own internal temporary buffering for
         compressed data to maximize internal access efficiency.  For this
         reason, the virtual `write' function defined here will usually be
         called to transfer quite a few bytes at a time. */
    virtual void set_target_size(int num_bytes) { return; }
      /* This function may be called by the rate allocator at any point, to
         indicate the total size of the code-stream currently being generated,
         including all marker codes.  The function may be called after some
         or all of the code-stream bytes have been delivered to the `write'
         member function; in fact, it may never be called at all.  The
         code-stream rate allocation procedure should attempt to identify the
         code-stream size as soon as it can, but this might not always be
         possible.  If the target requires knowledge of the size before
         writing the data (as, for example, when generating a JP2 box), it may
         need to buffer the code-stream data until this function has been
         called or the object is destroyed. */
  };

/*****************************************************************************/
/*                                 kdu_codestream                            */
/*****************************************************************************/

class kdu_codestream {
  /* Objects of this class are only interfaces, having the size of a single
     pointer.  Copying the object has no effect on the underlying state
     information, but simply serves to provide another interface (or
     reference) to it.  Note that there is no destructor, because destroying
     an interface has no impact on the underlying state.  Unlike the other
     interface classes defined below, this one has a creation function, which
     may be used to create the entire system of compressed data containers --
     tiles, tile-components, resolutions, subbands and code-blocks. There is
     currently no precinct interface exposed to the user, but the internal
     counterpart exists.  In the future, we may add such an interface to
     facilitate direct manipulation of packet data, rather than forcing
     interaction with the code-blocks themselves. */
  // --------------------------------------------------------------------------
  public: // Lifecycle member functions
    kdu_codestream() { state = NULL; }
    bool exists()
      { return (state==NULL)?false:true; }
    bool operator!()
      { return (state==NULL)?true:false; }
    KDU_EXPORT void
      create(siz_params *siz, kdu_compressed_target *target=NULL);
      /* Creates an internal `kd_codestream' object to be accessed via this
         interface, for use in compression or other code-stream generation
         operations (e.g., transcoding). The function is deliberately not
         implemented as a constructor, since robust exception handlers cannot
         cleanup partially constructed objects if an error condition is thrown
         from within a constructor (as a general rule, constructors are the
         least robust place to do significant work).  For safety, this
         function must never be invoked on a `kdu_codestream' interface which
         already contains an embedded object.  That is, `exists()' must
         return false.
            The `target' object, if supplied, receives the compressed
         JPEG2000 code-stream when or as it becomes available.
            In order to permit determination of the basic structure, the
         caller must supply a `siz' object, whose contents have been filled
         out (and finalized).  An internal copy of this object is
         made, so the caller can feel free to destroy the `siz' object
         at any point. */
    KDU_EXPORT void
      create(kdu_compressed_source *source);
      /* Creates an internal `kd_codestream' object to be accessed via this
         interface, for use in unpacking or decompressing an existing
         code-stream (retrieved via the supplied `source' object.  For the
         same reasons mentioned above, this function is deliberately not
         implemented as a constructor and cannot be used unless (or until)
         the `kdu_codestream' interface does not reference any existing
         internal object -- i.e., `exists()' must return false.
            The function reads the main header up to but not including the
         first tile-part header.  Further code-stream reading occurs on a
         strictly as-needed basis, as tile and packet information is
         accessed. */
    KDU_EXPORT void
      share_buffering(kdu_codestream existing);
      /* When two or more codestream objects are to be used simultaneously,
         considerable memory can be saved by encouraging them to use the same
         underlying service for buffering compressed data.  This is because
         resources allocated for buffering compressed data are not returned to
         the system heap until the object is destroyed.  By sharing buffering
         resources, one code-stream may use excess buffers already freed up
         by the other code-stream.  This is particularly beneficial when
         implementing transcoders.  The internal buffering service will not
         be destroyed until all codestream objects which are sharing it have
         been destroyed, so there is no need to worry about the order in
         which the codestream objects are destroyed. */
    KDU_EXPORT void
      set_persistent();
      /* The persistent mode is important for interactive applications.  By
         default, tiles, precincts and code-blocks will be discarded as soon
         as it can be determined that the user will no longer be accessing
         them.  Moreover, after each code-block has been opened and closed,
         it will automatically be discarded.  This behaviour minimizes memory
         consumption when the image (or some region of interest) is to be
         decompressed only once.  For interactive applications, however, it may
         be desirable to leave the code-stream intact and permit multiple
         accesses to the same information so that a new region or resolution of
         interest can be defined and decompressed at will.  For these
         applications, you must invoke this member function before any
         attempt to access (i.e., open) an image tile. */
    KDU_EXPORT void
      destroy();
      /* Note: providing an explicit destructor for `kdu_codestream' is
         dangerous, since it is merely a container for a hidden reference to
         the `kd_codestream' object.  The interpretation of copying should
         be (and is) generating another reference, so the underlying object
         should not be destroyed when the copy goes out of scope.  Reference
         counting would be an elegant, though tedious and less explicit
         way of avoiding this difficulty.
            Destroys the internal representation, along with all tiles,
         tile-components and other subordinate state information for which
         interfaces are described in this header.  Cleanup operations may
         include flushing internal compressed data to an output code-stream
         file if one exists. */
  // --------------------------------------------------------------------------
  public: // Member functions used to access information
    KDU_EXPORT siz_params *
      access_siz();
      /* Every `kdu_codestream' object has a unique `siz_params' object which
         is the head of its parameter list.  The caller may use the object's
         member functions to parse command line arguments, copy parameters
         between code-streams and so forth.  Be careful to note, however, that
         the codestream object supports geometrically transformed views of
         the image, which are not reflected in the underlying code-stream
         parameters. */
    KDU_EXPORT int
      get_num_components();
      /* Returns the apparent number of image components. This is affected by
         any call to `kdu_codestream::apply_input_restrictions'. */
    KDU_EXPORT int
      get_bit_depth(int comp_idx);
      /* Get the bit-depth of one of the image components.  The interpretation
         of the component index is affected by the component range arguments,
         `first_component' and `max_components', supplied in any call to
         `kdu_codestream::apply_input_restrictions'. */
    KDU_EXPORT bool
      get_signed(int comp_idx);
      /* Returns true if the image component originally had a signed
         representation; false if it was unsigned.  The interpretation of
         the component index is affected by the component range arguments,
         `first_component' and `max_components' supplied in any call to
         `kdu_codestream::apply_input_restrictions'. */
    KDU_EXPORT void
      get_subsampling(int comp_idx, kdu_coords &subs);
      /* Returns the canvas sub-sampling factors for this image component.
         Note that the interpretation of the component index is affected by
         the component range arguments, `first_component' and `max_components',
         supplied in any call to the `apply_input_restrictions' function.
         Also, the sub-sampling factors are affected by the `transpose'
         argument supplied in any call to the `change_appearance' member
         function.
            The sub-sampling factors identify the separation between the
         notional centres of the component samples on the high resolution
         code-stream canvas.  Since component dimensions are affected
         by discarding resolution levels, but this has no impact on the
         high resolution canvas coordinate system, the sub-sampling factors
         returned by this function are corrected (shifted up) to accommodate
         the effects of discarded resolution levels as specified in any
         call to `apply_input_restrictions'. */
    KDU_EXPORT void
      get_registration(int comp_idx, kdu_coords scale, kdu_coords &crg);
      /* Returns component registration information for the indicated
         image component.  The horizontal and vertical registration offsets
         are returned in `creg' after scaling by `scale' and rounding to
         integers.  Samples from this component have horizontal locations
         (kx + crg.x/scale.x)*subs.x and vertical locations
         (ky + crg.y/scale.y)*subs.y, where kx and ky are integers ranging
         over the bounds returned by the `get_dims' member function and
         subs.x and subs.y are the sub-sampling factors for this component.
         The component offset information is recovered from any CRG marker
         segment in the code-stream; the default offsets of 0 will be used
         if no such marker segment exists.
            The interpretation of the component index is affected by
         the component range arguments, `first_component' and `max_components'
         supplied in any call to the `apply_input_restrictions' function.
         The registration offsets and `scale' argument are also corrected
         to account for the effects of any prevailing geometric
         transformations which may have been applied through a call to the
         `change_appearance' member function. */
    KDU_EXPORT void
      get_dims(int comp_idx, kdu_dims &dims);
      /* Returns the apparent dimensions and position of an image component
         on the canvas.  These are calculated from the image region on the high
         resolution canvas and the individual sub-sampling factors for the
         image component.  Note that the apparent dimensions are affected by
         any calls which may have been made to `kdu::change_appearance' or
         `kdu_codestream::apply_input_restrictions'.  In particular, both
         geometric transformations and resolution reduction requests affect
         the apparent component dimensions.  The interpretation of the
         component index is also relative to any restrictions on the range
         of apparent image components.
             If `comp_idx' is negative, the function returns the dimensions
         of the image on the high resolution canvas itself.  In this case, the
         return value is corrected for geometric transformations, but not
         resolution reduction requests.  Since the sub-sampling factors
         returned by `get_subsampling' are corrected for resolution reduction
         requests, the caller may recover the image component dimensions
         directly from the canvas dimensions returned here, by applying the
         sub-sampling factors.  For information on how to perform such
         mapping operations, see Chapter 11 in the book by Taubman and
         Marcellin. */
    KDU_EXPORT void
      get_valid_tiles(kdu_dims &indices);
      /* Returns the range of tile indices which correspond to the current
         region of interest (or the whole image, if no region of interest has
         been defined).  The indices of the first tile within the region of
         interest are returned via `indices.pos'.  Note that these may be
         negative, if geometric transformations were specified via
         `kdu_codestream::change_appearance'.  The number of tiles in each
         direction within the region of interest is returned via
         `indices.size'. Note that tile indices in the range pos through
         pos+size-1 are apparent tile indices, rather than actual code-stream
         tile indices.  They are affected not only by the prevailing region of
         interest, but also by the geometric transformation flags supplied
         during any call to `kdu_codestream::change_appearance'.  The caller
         should not attempt to attach any interpretation to the absolute
         values of these indices. */
    KDU_EXPORT bool
      find_tile(int comp_idx, kdu_coords loc, kdu_coords &tile_idx);
      /* Locates the apparent tile indices of the tile-component, which
         contains the indicated location (it is also an apparent location,
         taking into account the prevailing geometric view and the number
         of discarded levels). Note that the search is conducted within the
         domain of the relevant image component.  Returns false if the
         supplied coordinates lie outside the image or any prevailing
         region of interest. */
    KDU_EXPORT void
      get_tile_dims(kdu_coords tile_idx, int comp_idx, kdu_dims &dims);
      /* Same as `get_dims' except that it returns the location and dimensions
         of only a single tile (if `comp_idx' is negative) or tile-component.
         The tile indices must lie within the range identified by the
         `get_valid_tiles' function.  As with `get_dims', all coordinates and
         dimensions are affected by the prevailing geometric appearance
         and constraints set up using `kdu_codestream::change_appearance'
         and `kdu_codestream::apply_input_restrictions'.  As a result, the
         function returns the same dimensions as those returned by the
         `kdu_tile_comp::get_dims' function provided by the relevant
         tile-component; however, the present function has the advantage that
         it can be used without first opening the tile -- an act which may
         consume substantial internal resources and code-stream parsing. */
    KDU_EXPORT int
      get_max_tile_layers();
      /* Returns the maximum number of quality layers seen in any tile
         opened so far.  The function returns 1 (smallest number of allowable
         layers) if the no tiles have yet been opened. */
    KDU_EXPORT void
      map_region(int comp_idx, kdu_dims comp_region, kdu_dims &hires_region);
      /* Maps a region of interest, specified in the domain of a single image
         component onto the high-res canvas, yielding a region suitable for
         use with the `apply_input_restrictions' member function.  The supplied
         region is specified with respect to the same coordinate system as
         that associated with the region returned by `get_dims' -- i.e., taking
         into account component sub-sampling factors, discarded resolution
         levels and any geometric transformations (see `change_appearance')
         associated with the current appearance.  The component index is
         also interpreted relative to any restrictions on the range of
         available image components.  The region returned via
         `hires_region' lives on the high-res canvas of the underlying
         code-stream and is independent of appearance transformations,
         discarded resolution levels or component-specific attributes.  The
         hi-res region is guaranteed to be large enough to cover all samples
         belonging to the intersecton between the supplied component region
         and the region returned via `get_dims'. */
  // --------------------------------------------------------------------------
  public: // Member functions used to modify behaviour
    KDU_EXPORT void
      set_textualization(std::ostream *stream);
      /* Supplies an output stream to which code-stream parameters will
         be textualized as they become available.  Main header parameters
         are written immediately, while tile-specific parameters are written
         at the point when the tile is destroyed.  This function is legal
         only when invoked prior to the first tile access.  Note that the
         specification of regions of interest without persistence (see
         `kdu_codestream::apply_input_restrictions' and
         `kdu_codestream::set_persistent') may prevent some tiles from actually
         being read -- if these contain tile-specific parameters, they may
         not be textualized. */
    KDU_EXPORT void
      set_max_bytes(int max_bytes);
      /* If used with an input codestream object, this function sets the
         maximum number of bytes which will be read from the input code-stream.
         Additional bytes will be discarded.
            If used with an output codestream object, this function enables
         internal machinery for incrementally estimating the parameters which
         will be used by the PCRD-opt rate allocation algorithm, so that the
         block coder can be given feedback to assist it in minimizing the
         number of coding passes which must be processed -- in many cases, most
         of the coding passes will be discarded.  Note that the actual rate
         allocation performed during a call to the `flush' member function is
         independent of the value supplied here, although it is expected that
         `max_bytes' will be equal to the maximum layer byte count supplied in
         the `flush' call.  The following cautionary notes should be
         observed concerning the incremental rate control machinery enabled
         when this function is invoked on an output codestream:
               1) The rate control prediction strategy relies upon the
                  assumption that the image samples will be processed
                  incrementally, with all image components processed
                  together at the same relative rate.  It is not at all
                  appropriate to process one image component completely,
                  followed by another component and so forth.  It such
                  a processing order is intended, this function should not
                  be called.
               2) The prediction strategy may inappropriately discard
                  information, thereby harming compression, if the
                  compressibility of the first part of the image which is
                  processed is very different to that of the last part of
                  the image.  Although this is rare in our experience, it
                  may be a problem with certain types of imagery, such
                  as medical images which sometimes contain empty regions
                  near boundaries.
               3) If truly lossless compression is desired, this function
                  should not be called, no matter how large the supplied
                  `max_bytes' value.  This is because terminal coding
                  passes which do not lie on the convex hull of the
                  rate-distortion curve will be summarily discarded,
                  violating the official mandate that a lossless code-stream
                  contain all compressed bits.
            For both input and output objects, the function may only be
         called prior to the point at which the first tile is opened and it
         may only be called once, if at all. */
    KDU_EXPORT void
      set_min_slope_threshold(kdu_uint16 min_slope);
      /* This function has no impact on input codestream objects.  When
         applied to an output codestream, the function has a similar effect
         to `set_max_bytes', except that it supplies a limit on the
         distortion-length slope threshold which will be used by the rate
         control algorithm.  This is most effective if the `flush' member
         function is to be called with a set of slope thresholds, instead
         of layer size specifications (see below).  If `set_max_bytes' is
         used, the slope threshold information will be incrementally
         estimated from statistics of the compressed data as it appears.  If
         `set_max_bytes' and `set_min_slope_threshold' are both used together,
         the larger (most constraining) of the two thresholds will be used
         by the block encoder to minimize its coding efforts. */
    KDU_EXPORT void
      set_resilient(bool expect_ubiquitous_sops=false);
    KDU_EXPORT void
      set_fussy();
    KDU_EXPORT void
      set_fast();
      /* These functions may be called at any time to modify the way input
         errors are treated.  The functions have no impact on codestream
         objects used for ouput.  The three modes are mutually exclusive.
            In the resilient mode, the object makes a
         serious attempt to both detect and recover from errors in the
         code-stream.  We attempt to guarantee that decompression will not
         fail, so long as the main header is uncorrupted and there is only
         one tile with one tile-part.
             By contrast, in the fussy mode, the object makes a serious attempt
         to identify compliance violation and generates an appropriate terminal
         error message if it finds one.
             By fast mode is used by default.  In this case, compliance is
         assumed so as to minimize checking, and there is no attempt to
         recover from errors.
             The resilient mode takes an optional argument indicating whether
         or not the decompressor should expect SOP markers to appear in
         front of every packet whenever the relevant flag in the Scod byte
         of the COD marker is set.  According to the JPEG2000 standard,
         SOP markers need not appear in front of every packet when this flag
         is set; however, this weakens error resilience, since we cannot
         predict when an SOP marker should appear.  If you know that the
         code-stream has been constructed to place SOP markers in front
         of every packet (or not use them at all), then set
         `expect_ubiquitous_sops' to true, thereby allowing the error
         resilient code-stream parsing algorithm to do a better job. */
    KDU_EXPORT void
      apply_input_restrictions(int first_component, int max_components,
                               int discard_levels, int max_layers,
                               kdu_dims *region_of_interest);
      /* This function may be used only with codestream objects created
         for input (i.e., for reading).  It restricts the amount of the
         code-stream which will be accessible to the user.  Since it has an
         impact on the dimensions returned by other member functions, these
         dimensions may need to be re-acquired afterwards.  The role of this
         function is closely related to that of `change_appearance'; however,
         the latter function may be applied to both input and output
         codestream objects.
            The function may be invoked multiple times to alter the region
         of interest, resolution, image components or number of quality
         layers presented to the user.  However, unless the object is set up
         for persistence (see `kdu_codestream::set_persistent'), it may not
         be called after the first tile access.  This is because non-persistent
         objects discard data which will not be used, based on these input
         restrictions and there is no way to get the discarded data back again.
         Even in the persistent case, you may not call this function while any
         tile is open.
            Zero valued arguments are always interpreted as meaning that any
         restrictions should be removed from the relevant parameter.
            The `first_component' argument identifies the index (starting from
         0) of the first component to be presented to the user.  If this is
         non-zero, any interface functions to this or any related object which
         accept component index arguments will add this `min_component' value
         to the component index in order to obtain the index of the true
         code-stream component which is being identified.  An error message
         will be generated if `min_component' is greater than or equal to
         the total number of code-stream components.
            The `max_components' argument identifies the maximum number
         of components which will appear to be present, starting from the
         component identified by the `first_component' argument.  If
         `max_components' is 0, all components will appear.
            The `discard_levels' argument indicates the number of resolution
         levels which should be discarded. Image dimensions are effectively
         divided by 2 to the power of this number. This argument affects the
         apparent dimensions and number of DWT levels in each tile-component.
         Note, however, that neither this nor any other argument has any
         effect on the list of `kdu_params' returned by
         `kdu_codestream::access_siz'.
            The `max_layers' argument identifies the maximum number of quality
         layers which will appear to be present when precincts or code-blocks
         are accessed.  A value of 0 has the interpretation that all
         layers should be retained.
            The `region_of_interest' argument, if non-NULL, provides a
         region of interest on the high resolution grid.  Any attempt to
         access tiles or other subordinate partitions which do not intersect
         with this region will result in an error.  Note that the region may
         be larger than the actual image region.  Also, the region must be
         described in terms of the original code-stream geometry.  Specifically
         any appearance transformations supplied by the `change_appearance'
         member function have no impact on the interpretation of the region.
         You may find the `map_region' member function useful in creating
         suitable regions. */
    KDU_EXPORT void
      change_appearance(bool transpose, bool vflip, bool hflip);
      /* This function alters the apparent orientation of the image, affecting
         the apparent dimensions and regions indicated by all subordinate
         objects and interfaces.  Multiple calls are permitted and the
         functionality is supported for both input and output codestream
         objects.  Except in the case of an input object marked for persistence
         (see `kdu_codestream::set_persistent'), the function may not be called
         after the first tile access.  Even in the case of a persistent input
         object, the function may not be called while any tile is open.
            Note that this function has no impact on `kdu_params' objects in
         the list returned by `kdu_codestream::access_siz', since those
         describe the actual underlying code-stream.
            The `transpose' flag causes vertical coordinates to be transposed
         with horizontal. If `vflip' is true, vertical coordinates start (at 0)
         from the bottom of the image/canvas and work upward.  If `transpose'
         and `vflip' are both true, vertical coordinates start from the right
         and work toward the left of the true underlying representation.  If
         `hflip' is true, individual image component lines (and all related
         quantities) are flipped.  If `transpose' is also true, these lines
         correspond to columns of the true underlying representation. */
  // --------------------------------------------------------------------------
  public: // Data processing/access functions
    KDU_EXPORT kdu_tile
      open_tile(kdu_coords tile_idx);
      /* Accesses a specific tile, returning an interface which can be
         used for accessing the tile's contents, including specific
         tile-components.  Note that the returned interface contains no
         state of its own, but only an embedded reference to the real state
         information, which is buried in the implementation.  When a tile is
         first opened, the internal state information may be automatically
         created from the code-stream parameters available at that time.
         These in turn may be translated automatically from markers if this
         is an input object.  Indeed, for this to happen, any amount of the
         code-stream may need to be read and buffered internally.
             For these reasons, you should be careful only to open tiles
         which you really want to read from or write to.  Instead of walking
         through tiles to find one you are interested in, use the `find_tile'
         member function.
             The supplied tile indices must lie within the region returned
         by the `get_valid_tiles' function; otherwise, an error will be
         generated.
             The object attempts to read and buffer the minimum amount of the
         code-stream in order to satisfy the request.  Although not mandatory,
         it is a good idea to close all tiles (see `j2_tile::close' once you
         are done with them). */
    KDU_EXPORT void
      flush(int layer_bytes[], int num_layer_specs,
            kdu_uint16 *thresholds=NULL);
      /* Call this function before destroying an output object.  It performs
         the PCRD-opt rate allocation and generates the actual code-stream
         data.  In the future we may provide mechanisms for incrementally
         flushing the code-stream to save memory when working with huge
         images and spatially progressive packet progressions.  For now,
         though, this function must be called once and once only, after
         everything has been compressed.
            The `layer_bytes' array contains `num_layer_specs'.  Normally,
         `num_layer_specs' should be identical to the actual number of quality
         layers.  In this case, every non-zero entry identifies the target
         maximum number of bytes for the corresponding quality layer. Note
         that the number of bytes specified in `layer_bytes[k]' identifies
         the total number of bytes occupied by layers 0 through k, including
         all headers, marker codes and marker segments.  However, see comments
         below regarding the optional `thresholds' argument.
            Zero valued entries in the `layer_bytes' array mean that the rate
         allocator should attempt to assign roughly logarithmically spaced
         bit-rates to those quality layers.  The logarithmic spacing rule is
         applied after first subtracting a minimal header offset consisting
         of the main and tile header bytes, plus 1 byte per packet (3 bytes
         if EPH markers are being used, 7 bytes if SOP marker segments are
         being used, and 9 bytes if both SOP and EPH marker segments are being
         used).  Any or all of the entries may be 0.  If the last entry is 0,
         all generated bits will be output by the time the last quality layer
         is encountered.  If the first entry is 0 and there are multiple
         layers, the function employs the following reasonable policy to
         determine suitable rate allocation targets.
               * Let Z be the number of initial layers whose size is not
                 explicitly specified (if the last layer has no assigned
                 size, it will first be assigned the number of bytes required
                 to including all compressed information in the code-stream,
                 as described above.
               * Let R be the target number of bytes for the next layer.
               * Let H_k be the minimal header cost mentioned above (main and
                 tile headers, plus 1 byte per packet, but more if SOP
                 and/or EPH markers are used) for the first k quality layers.
               * The rate allocator will try to allocate H_1+(R-H_Z)/sqrt(2^Z)
                 bytes to the first layer.  The logarithmic spacing policy
                 will then assign roughly 2 layers per octave change in the
                 bit-rate.
         Note that the above policy will work well even if the user provides
         a `layer_bytes' array with all entries set to 0; in this case it will
         assign 2 layers per octave, with the last layer containing all
         compressed data.
            It can happen that individual tiles have fewer quality layers.
         In this case, these tiles participate only in the rate allocation
         associated with the initial quality layers and they make no
         contribution to the later (higher rate) layers.  If no tiles have
         `num_layer_specs' quality layers, the code-stream size will be
         limited to that specified in the largest `layer_bytes' entry for
         which at least one tile has a quality layer.
            It can happen that individual tiles have more quality layers
         than the number of layer specs provided here.  Packets associated
         with all such layers will be written with the "empty packet bit"
         set to 0 -- they will thus have the minimum 1 byte representation.
         These useless packet bytes are taken into account in the rate
         allocation process, so as to guarantee that the complete code-stream
         does not exceed the size specified in a final layer spec.
            The `thresholds' argument, if non-NULL, must point to an array
         with `num_layer_specs' entries.  If the first entry in the array
         is 0, the array receives the final optimized distortion-length
         thresholds found by the rate controller, based on the length
         information supplied in `layer_bytes', as described above.
         Otherwise, the array contains thresholds which should be used
         directly to control the contents of the quality layers, ignoring
         any information in the `layer_bytes' array.  In either case, when
         the function returns, the actual lengths allocated to each
         quality layer will have been written into the `layer_bytes' array.
            For the purpose of determining appropriate threshold values, we
         point out that a threshold of 0 yields the largest possible
         output size, i.e., all bytes are included by the end of that layer.
         A slope threshold of 0xFFFF yields the smallest possible output
         size, i.e., no code-blocks are included in the layer.  In this way,
         it is possible to first recover the thresholds used by the rate
         controller when compressing one image and apply these same
         thresholds to the compression of a second image.  The functionality
         may be useful in establishing a more consistent level of quality
         across a collection of images.   It may also be used to construct
         feedback-driven rate control loops for video compression. */
    KDU_EXPORT int
      trans_out(int max_bytes = INT_MAX);
      /* Use this output function, instead of `flush' when the code-stream
         has been created by a transcoding operation which has no real
         distortion information.  In this case, the individual code-block
         `pass_slopes' values are expected to hold 0xFFFF-layer_idx, where
         layer_idx is the zero-based index of the quality layer to which the
         code-block contributes (the pass slope value is 0 if a later pass
         contributes to the same layer).  This policy is described in the
         comments appearing with the definition of "kdu_block".  A modified
         form of the rate allocation algorithm is used to write output
         quality layers with the same code-block contributions as the
         quality layers in the input code-stream which is being transcoded.
            If the existing layers exceed the `max_bytes' limit, empty
         packets are written for any complete quality layers which are to
         be discarded and partial layers are formed by discarding code-blocks
         starting from the highest frequency subbands and the bottom of the
         image.
            The function returns the number of non-empty quality layers. */
  // ------------------------------------------------------------------------
  public: // Summary information reporting functions
    KDU_EXPORT int
      get_total_bytes();
      /* Returns the total number of bytes written to or read from
         the code-stream so far. */
    KDU_EXPORT int
      get_num_tparts();
      /* Returns the total number of tile-parts written to or read from
         the code-stream so far. */
    KDU_EXPORT void
      collect_timing_stats(int num_coder_iterations);
      /* Collects timing statistics, which may later be retrieved using
         `get_timing_stats'.  If `num_coder_iterations' is non-zero, the block
         encoder or decoder will be asked to time itself (it need not
         necessarily comply) by processing each block `num_coder_iterations'
         times.  If it does so, block coder throughput statistics will also be
         reported.  If `num_coder_iterations' is 0, end-to-end times are
         generally very reliable.  Otherwise, the numerous calls to the
         internal `clock()' function required to time block coding operations
         may lead to some inaccuracies.  The larger the value of
         `num_coder_iterations', the more reliable block coding times are
         likely to be, since the coder is executed multiple times between calls
         to `clock()'.  On the other hand, the end-to-end execution time needs
         to be corrected to account for multiple invocations of the block
         coder, and this correction can introduce substantial inaccuracies. */
    KDU_EXPORT double
      get_timing_stats(int *num_samples, bool coder_only=false);
      /* If `coder_only' is false, the function returns the number of
         seconds since the last call to `collect_timing_stats'.  If a block
         coder timing loop has been used to gather more accurate block coding
         statistics by running the block coder multiple times, the function
         estimates the number of seconds which would have been consumed if
         the block coder had been executed only once per code-block.
            If `coder_only' is true, the function returns the number of
         seconds required to process all code-blocks.  If the coder was invoked
         multiple times, the returned number of seconds is normalized to the
         time which would have been required by a single invocation of the
         coder.  If the coding operation was not timed, the function returns 0.
            If `num_samples' is non-NULL, it is used to return the number of
         samples associated with returned CPU time.  If `coder_only' is true,
         this is the number of code-block samples.  Otherwise, it is the
         number of samples in the current image region.  In any event, dividing
         the returned time by the number of samples yields the most appropriate
         estimate of per-sample processing time. */
    KDU_EXPORT int
      get_compressed_data_memory();
      /* Returns the total amount of heap memory allocated to storing
         compressed data.  This includes compressed code-block bytes,
         coding pass length and R-D information and layering information.
         It does not include parsing or generation state information -- the
         amount of memory required for this is reported by
         `get_compressed_state_memory'.
            In the event that the `share_buffering' member function has been
         used to associated a single compressed data buffering service with
         multiple codestream objects, the value reported here represents the
         amount of memory consumed by all such codestream objects together. */
    KDU_EXPORT int
      get_compressed_state_memory();
      /* Returns the total amount of heap memory allocated to storing the
         state of tiles, tile-components, resolutions, subbands, precincts
         and code-blocks.  This might not be an insignificant cost, either
         for compression or decompression.  For example, even if we intend
         to discard many resolution levels, we might still need to store
         state information for all code-blocks in the resolution levels to
         be discarded, since otherwise we cannot parse packet headers and
         determine the location of the packets we want.  This depends, of
         course, on the packet progression sequence, the region of interest
         and so forth.  The system does its best to minimize the amount of
         state information which is stored at any given moment, based on
         what it knows about the user's intended access patterns (e.g.,
         whether or not the compressed data needs to be persistent). */

    // ------------------------------------------------------------------------
    private: // Interface state
      kd_codestream *state;
  };

/*****************************************************************************/
/*                                   kdu_tile                                */
/*****************************************************************************/

class kdu_tile {
  /* Objects of this class are only interfaces, having the size of a single
     pointer.  Copying the object has no effect on the underlying state
     information, but simply serves to provide another interface (or
     reference) to it.  Note that there is no destructor, because destroying
     an interface has no impact on the underlying state. */
  // --------------------------------------------------------------------------
  public: // Lifecycle member functions
    kdu_tile() { state = NULL; }
    kdu_tile(kd_tile *state) { this->state = state; }
    bool exists()
      { return (state==NULL)?false:true; }
    bool operator!()
      { return (state==NULL)?true:false; }
    KDU_EXPORT void
      close();
      /* Although not strictly necessary, it is a good idea to call this
         function once you have finished reading from or writing to a tile.
         Once closed, future attempts to access the same tile via the
         `kdu_codestream::open_tile' function will generate an error, unless
         the codestream object is set for persistence (see the
         `kdu_codestream::set_persistent' function).  With non-persistent
         codestream objects, the system is free to discard all internal storage
         associated with a closed tile at its earliest convenience.  For
         persistent codestream objects, all open tiles must be closed before
         the appearance can be modified bu `kdu_codestream::change_appearance'
         or `kdu_codestream::apply_input_restrictions'. */
  // --------------------------------------------------------------------------
  public: // Identification member functions
    KDU_EXPORT int
      get_tnum();
      /* Returns the zero-based tile index of this tile as it appears in
         the relevant code-stream. */
  // --------------------------------------------------------------------------
  public: // Data processing/access member functions
    KDU_EXPORT bool
      get_ycc();
      /* Returns true if a component colour transform is to be used for the
         first 3 image components.  In the event that the number of apparent
         components is less than 3, even though the actual number of code-stream
         components may be 3 or more, the function returns false. */
    KDU_EXPORT int
      get_num_components();
      /* Returns the apparent number of image components, which is
         also the number of tile-components in the current tile.  Note
         that this value is affected by the `max_components' argument
         supplied by any call to `kdu_codestream::apply_input_restrictions'. */
    KDU_EXPORT kdu_tile_comp
      access_component(int component_idx);
      /* Valid component indices lie in the range 0 through num_comps-1 */
  // --------------------------------------------------------------------------
  private: // Interface state
    kd_tile *state;
  };

/*****************************************************************************/
/*                                kdu_tile_comp                              */
/*****************************************************************************/

class kdu_tile_comp {
  /* Objects of this class are only interfaces, having the size of a single
     pointer.  Copying the object has no effect on the underlying state
     information, but simply serves to provide another interface (or
     reference) to it.  Note that there is no destructor, because destroying
     an interface has no impact on the underlying state. */
  // --------------------------------------------------------------------------
  public: // Lifecycle member functions
    kdu_tile_comp() { state = NULL; }
    kdu_tile_comp(kd_tile_comp *state) { this->state = state; }
    bool exists()
      { return (state==NULL)?false:true; }
    bool operator!()
      { return (state==NULL)?true:false; }
  // --------------------------------------------------------------------------
  public: // Data processing/access functions
    KDU_EXPORT bool
      get_reversible();
      /* Returns true if tile-component is to be processed reversibly. */
    KDU_EXPORT void
      get_subsampling(kdu_coords &factors);
      /* Returns the horizontal and vertical sub-sampling factors for this
         image component. These are affected by the `transpose' argument
         supplied to `kdu_codestream::change_appearance'.  They are
         also affected by discarded resolution levels, as specified through
         a call to the `kdu_codestream::apply_input_restrictions'.  See the
         description of `kdu_codestream::get_subsampling' for more
         information. */
    KDU_EXPORT int
      get_bit_depth(bool internal = false);
      /* If `internal' is false, the function returns the same
         value as `kdu_codestream::get_bit_depth', i.e., the precision
         of the image component samples represented by this tile-component,
         prior to any forward colour transform.
            If `internal' is true, however, the function reports
         the maximum number of bits required to represent the tile-component
         samples in the original image domain, the colour transformed domain
         and the subband domain.  This allows applications to judge the
         most appropriate numerical representation for the data. In the
         reversible path, it is sufficient to employ integers with the
         returned bit-depth.  Otherwise, the returned bit-depth is just
         a guideline -- the use of higher precision representations for
         irreversible path processing will generally improve accuracy. */
    KDU_EXPORT bool
      get_signed();
      /* Returns true if the original sample values for this component
         had a signed representation; otherwise, returns false. */
    KDU_EXPORT int
      get_num_resolutions();
      /* Returns the total number of available resolution levels,
         which is one more than the total number of accessible DWT
         levels.  Note, however, that the return value is influenced
         by the number of discarded levels supplied in any call to
         `kdu_codestream::apply_input_restrictions'. */
    KDU_EXPORT kdu_resolution
      access_resolution(int res_level);
    KDU_EXPORT kdu_resolution
      access_resolution();
      /* A `res_level' index of 0 returns the lowest resolution level,
         corresponding to the LL band of the tile-component's DWT.  The form
         of the function without any arguments returns the highest apparent
         resolution.  It is equivalent to `res_level'=get_num_resolutions(). */
  // --------------------------------------------------------------------------
  private: // Interface state
    kd_tile_comp *state;
  };

/*****************************************************************************/
/*                                kdu_resolution                             */
/*****************************************************************************/

class kdu_resolution {
  /* Objects of this class are only interfaces, having the size of a single
     pointer.  Copying the object has no effect on the underlying state
     information, but simply serves to provide another interface (or
     reference) to it.  Note that there is no destructor, because destroying
     an interface has no impact on the underlying state. */
  // --------------------------------------------------------------------------
  public: // Lifecycle/identification/navigation member functions
    kdu_resolution() { state = NULL; }
    kdu_resolution(kd_resolution *state) { this->state = state; }
    bool exists()
      { return (state==NULL)?false:true; }
    bool operator!()
      { return (state==NULL)?true:false; }
    KDU_EXPORT kdu_resolution
      access_next();
      /* Returns an interface for the next lower resolution level.  If there
         are no more, the returned interface will have a NULL `state', which
         may be tested using the overloaded "!" operator. */
    KDU_EXPORT int
      which();
      /* Gets the resolution level index.  This is the same index which must
         be supplied to `tile_comp::access_resolution' in order to obtain
         an interface to the current object. 0 is the index of the resolution
         level which contains only the LL subband. */
    KDU_EXPORT int
      get_dwt_level();
      /* Gets the index of the DWT level associated with the highest frequency
         subbands used to build this resolution.  If no DWT is used, the
         return value will be 0.  Otherwise, the highest resolution object
         returns 1, the next one returns 2 and so forth.  The two lowest
         resolution objects both return the total number of DWT levels.  The
         function is most useful for implementations of the DWT. */
  // --------------------------------------------------------------------------
  public: // Data processing/access member functions
    KDU_EXPORT void
      get_dims(kdu_dims &dims);
      /* Returns the apparent dimensions and location of the resolution level
         on the canvas.  Note that the apparent dimensions are affected by
         the geometric transformation flags supplied during any call to
         `kdu_codestream::change_appearance', as well as any region of interest
         which may have been specified through
         `kdu_codestream::apply_input_restrictions'.
         Moreover, the region of interest, as it appears within any given
         resolution level, depends upon the region of support of the DWT
         synthesis kernels. */
    KDU_EXPORT kdu_subband
      access_subband(int band_idx);
      /* Returns an interface to the internal subband state information.  The
         `band_idx' argument must be LL_BAND if `get_res_level()' returns 0.
         Otherwise, `band_idx' must be one of HL_BAND (horizontally high-pass),
         LH_BAND (vertically high-pass) or HH_BAND. Note that these are
         apparent subbands.  If the `transpose' flag was supplied during
         a call to `kdu_codestream::change_appearance', the roles of the LH
         and HL subbands will be automatically interchanged. */
    KDU_EXPORT bool
      get_reversible();
      /* Returns true if the DWT stage built on this resolution level is
         to use reversible lifting. */
    KDU_EXPORT int
      get_kernel_id();
      /* Currently returns one of Ckernels_W5X3 or Ckernels_W9X7.  In
         any event, the return value may be used directly to construct a
         `kdu_kernels' object for querying all aspects the DWT kernels. */
    KDU_EXPORT bool
      propagate_roi();
      /* This function returns false if there is no need to propagate ROI
         mask information from this resolution level into its derived
         subbands.  The function always returns false if invoked on an
         input object (one not involved in content creation).  For output
         objects, the function's return value depends on the "Rlevels"
         attribute of the relevant "roi_params" parameter object set up
         during content creation. */
  // --------------------------------------------------------------------------
  private: // Interface state
    kd_resolution *state;
  };

/*****************************************************************************/
/*                                 kdu_subband                               */
/*****************************************************************************/

class kdu_subband {
  /* Objects of this class are only interfaces, having the size of a single
     pointer.  Copying the object has no effect on the underlying state
     information, but simply serves to provide another interface (or
     reference) to it.  Note that there is no destructor, because destroying
     an interface has no impact on the underlying state. */
  // --------------------------------------------------------------------------
  public: // Lifecycle/identification member functions
    kdu_subband() { state = NULL; }
    kdu_subband(kd_subband *state) { this->state = state; }
    bool exists()
      { return (state==NULL)?false:true; }
    bool operator!()
      { return (state==NULL)?true:false; }
    KDU_EXPORT int
      which();
      /* Returns the subband index -- one of LL_BAND, HL_BAND, LH_BAND or
         HH_BAND.  Note that the return value is the apparent subband index,
         which is affected by the `transpose' flag supplied to
         `kdu_codestream::change_appearance'. */
  // --------------------------------------------------------------------------
  public: // Data processing/access member functions
    KDU_EXPORT int
      get_K_max();
      /* Returns the maximum number of magnitude bit-planes for the subband,
         not including any ROI adjustments. */
    KDU_EXPORT int
      get_K_max_prime();
      /* Returns the maximum number of magnitude bit-planes for the subband,
         including any ROI adjustments.  K_max_prime - K_max is the ROI upshift
         value, U, which should either be 0 or a value sufficiently large to
         ensure that the foreground and background regions can be separated. */
    KDU_EXPORT bool
      get_reversible();
      /* Returns true if the subband samples are coded reversibly. */
    KDU_EXPORT float
      get_delta();
      /* Returns the quantization step size for irreversibly coded subband
         samples; returns 0 for reversibly coded subbands. */
    KDU_EXPORT float
      get_msb_wmse();
      /* Returns the contribution of a single bit error in the most significant
         magnitude bit-plane of any sample in this subband to overall image
         weighted MSE.  The number of magnitude bit-planes is returned by
         `get_K_max_prime'.  The weighted MSE contributions are expressed
         relative to a normalization of the original image data in which the
         sample values all have a nominal range from -0.5 yo +0.5.  This is
         normalization framework is used for both reversibly and irreversibly
         processed samples, so that rate allocation can be performed correctly
         across images containing both reversibly and irreversibly compressed
         tile-components. */
    KDU_EXPORT bool
      get_roi_weight(float &energy_weight);
      /* Returns true if a special energy weighting factor has been requested
         for code-blocks which contribute to the foreground region of an
         ROI mask.  In this case, the `energy_weight' value is set to the
         amount by which MSE distortions should be scaled.  This value is
         obtained by squaring the `Rweight' attribute from the relevant
         `rgn_params' coding parameter object. */
    KDU_EXPORT void
      get_dims(kdu_dims &dims);
      /* Returns the apparent dimensions and location of the subband on the
         canvas.  Note that the apparent dimensions are affected by the
         geometric transformation flags supplied during any call to
         `kdu_codestream::change_appearance', as well as any region of
         interest which may have been specified in the call to
         `kdu_codestream::apply_input_restrictions'. Moreover, the region of
         interest, as it appears within any given subband, depends upon the
         region of support of the DWT synthesis kernels. */
    KDU_EXPORT void
      get_valid_blocks(kdu_dims &indices);
      /* Returns the range of valid indices which may be used to access
         code-blocks for this subband.  Upon return, `indices.area()' is the
         total number of code-blocks in the subband.  As with all of the
         index manipulation functions, indices may well turn out to be
         negative as a result of geometric manipulations. */
    KDU_EXPORT void
      get_block_size(kdu_coords &nominal_size, kdu_coords &first_size);
      /* Returns apparent dimensions whose primary purpose is to facilitate
         the determination of buffer sizes for quantized subband samples
         prior to encoding, or after decoding.  Although the `open_block'
         member function returns a `kdu_block' object which contains complete
         information regarding each block's dimensions and the region of
         interest within the block, this function has profound implications
         for resource consumption and should not be invoked until the caller
         is ready to decode or encode a block of sample values.
            The `nominal_size' argument is used to return the dimensions
         of the code-block partition for the subband.  Note that these are
         apparent dimensions and so they are affected by the `transpose' flag
         submitted to the containing codestream object's `change_appearance'
         function.
            The `first_size' argument is used to return the dimensions of
         the first apparent code-block.  This is the code-block which has
         the smallest indices (given by the `pos' field of the `indices'
         structure) returned via `kdu_subband::get_valid_blocks'.  The actual
         code-block corresponding to these indices may be at the upper left,
         lower right, lower left or upper right corner of the subband,
         depending upon the prevailing geometric view.  Moreover, the
         dimensions returned via `first_size' are also sensitive to the
         geometric view and the prevailing region of interest.  When this
         first code-block is opened using the `kdu_subband::open' function,
         the returned `kdu_block' object should have its dimensions
         equal to the value returned via `first_size', transposed
         if necessary. */
    KDU_EXPORT kdu_block *
      open_block(kdu_coords block_idx, int *return_tpart=NULL);
    KDU_EXPORT void
      close_block(kdu_block *block);
      /* Code-block access is bracketed by calls to these two functions. It
         is currently illegal to have more than one code-block open at once.
         The underlying structure manages storage for a single `kdu_block'
         object, returning a pointer to this object in response to the
         `open_block' call.  The `block_idx' argument must provide indices
         inside the valid region identified by the `get_valid_blocks'
         function.  The returned `kdu_block' object contains all necessary
         information for encoding or decoding the code-block, including its
         dimensions, coding mode switches and so forth.
            If the containing codestream object was created for input,
         the block will have all coding pass and code-byte information intact,
         ready to be decoded.  Otherwise, all relevant members will be
         initialized to the `empty' state, ready to accept newly generated
         code bytes.
            Also, for input objects, the index of the tile-part to which the
         block belongs (starting from 0) will be returned via any non-NULL
         `return_tpart' argument.  For output objects, a negative number will
         usually be returned via this argument if it is used.
            The `close_block' function returns the `kdu_block' object to the
         internal machinery for later re-use.  For output codestream objects,
         it also causes the compressed data to be copied into more efficient
         internal structures until we are ready to generate the final
         code-stream.  For input codestream objects, closing a block will
         destroy the internal storage for that block so that the block may
         not be reopened, unless the containing codestream object is
         persistent (see `kdu_codestream::set_persistent').
            It is worth noting that opening a block may have far reaching
         implications for the internal structure.  Precincts may be created
         for the first time; packets may be read and de-sequenced; tile-parts
         may be opened and so forth.  There is no restriction on the order in
         which code-blocks may be opened, although different orders can have
         very different implications for the amount of the code-stream which
         must be buffered internally. */
    KDU_EXPORT kdu_uint16
      get_conservative_slope_threshold();
      /* This function is provided for block encoders to estimate the
         number of coding passes which they actually need to process, given
         that many of the coding passes may end up being discarded during
         rate allocation.  The function returns a conservatively estimated
         lower bound to the distortion-length slope threshold which will be
         used by the PCRD-opt algorithm during final rate allocation.  The
         coder is responsible for translating this into an estimate of the
         number of coding passes which must be processed.  The function
         returns 1 if there is no information available on which to base
         estimates -- such information will be available only if
         `kdu_codestream::set_max_bytes' or
         `kdu_codestream::set_min_slope_threshold' has been called. */
  // --------------------------------------------------------------------------
  private: // Interface state
    kd_subband *state;
  };

/*****************************************************************************/
/*                                   kdu_block                               */
/*****************************************************************************/

struct kdu_block {
  // --------------------------------------------------------------------------
  public: // Member functionns
    kdu_block();
    ~kdu_block();
    KDU_EXPORT void
      set_max_passes(int new_passes, bool copy_existing=true);
    KDU_EXPORT void
      set_max_bytes(int new_bytes, bool copy_existing=true);
    KDU_EXPORT void
      set_max_samples(int new_samples);
    KDU_EXPORT void
      set_max_contexts(int new_contexts);
      /* These functions should be called to augment the number of
         coding passes or the number of code bytes which can be stored
         in the structure.  For efficiency, call the functions only when
         you know that there is a potential problem with the current
         allocation, by testing `max_passes' and `max_bytes'.  An encoder
         may check the available storage between coding passes to minimize
         overhead, exploiting the fact that there is a well-defined upper
         bound to the number of bytes which the MQ coder may generate in any
         coding pass. */
  // --------------------------------------------------------------------------
  public: // Parameters controlling the behaviour of the encoder or decoder
    kdu_coords size;
    kdu_dims region;
    bool transpose, vflip, hflip;
    int modes;
    int orientation; // One of LL_BAND through HH_BAND
    bool resilient; // Encourages a decoder to attempt error concealment.
    bool fussy; // Encourages a decoder to check for compliance.
    int K_max_prime; // Maximum number of magnitude bit-planes for the subband.
  // --------------------------------------------------------------------------
  public: // Data produced by the encoder or consumed by the decoder
    int missing_msbs;
    int num_passes;
    int *pass_lengths;
    kdu_uint16 *pass_slopes;
    kdu_byte *byte_buffer;
    int max_passes; // Current size of `pass_lengths' & `pass_slopes' arrays
    int max_bytes; // Current size of the `byte_buffer' array.
  // --------------------------------------------------------------------------
  public: // Data produced by the decoder or consumed by the encoder
    kdu_int32 *sample_buffer;
    kdu_int32 *context_buffer;
    int max_samples; // current size of the `sample_buffer' array.
    int max_contexts; // current size of the `context_buffer' array.
  // --------------------------------------------------------------------------
  public: // Members used to collect statistics
    int start_timing()
      { /* If the block encoder or decoder supports the gathering of timing
           statistics, it should do so by calling this function at the start
           of a timing loop and `finish_timing' at the end of the loop; the
           number of times to execute the loop is the return value from the
           function. */
        if (cpu_iterations == 0) return 1;
        cpu_start = clock(); return cpu_iterations;
      }
    void finish_timing()
      {
        if (cpu_iterations == 0) return;
        cpu_time += clock() - cpu_start;
        cpu_unique_samples += size.x*size.y;
      }
    void initialize_timing(int iterations)
      { /* Used to configure the gathering of timing stats; 0 iterations means
           nothing will be timed and the block encoder or decoder should
           execute only once. */
        assert(iterations >= 0);
        this->cpu_iterations = iterations;
      }
    double get_timing_stats(int &unique_samples, double &time_wasted)
      { /* The return value is the calculated number of seconds required to
           process `unique_samples' once.  The `time_wasted' argument returns
           the number of CPU seconds wasted by iterating over a timing loop
           so as to reduce the impact of the calls to `clock' on the final
           time. */
        unique_samples = cpu_unique_samples;
        double total_time = ((double) cpu_time) / CLOCKS_PER_SEC;
        double once_time = total_time;
        if (cpu_iterations > 1) once_time /= cpu_iterations;
        time_wasted = total_time - once_time;
        return once_time;
      }
  private: // State members used by the above functions
    int cpu_iterations; // 0 unless the block coder is to be timed.
    int cpu_unique_samples; // timed samples = unique samples * num iterations
    clock_t cpu_start;
    clock_t cpu_time; // Time taken to process all `cpu_timed_samples'.
  // --------------------------------------------------------------------------
  private: // Navigation information installed by `kdu_subband::open_block'.
    friend class kdu_subband;
    kd_precinct *precinct; // Precinct to which the block belongs.
    int which_block; // Index of the block within the precinct-band's array.
  };
  /* Notes:
     This structure is used for intermediate storage of code-block
     information.  Compressed code-blocks are stored in a much more efficient
     form, which is translated to and from the form in this structure by
     the `subband::open_block' and `subband::close_block' functions.  The
     present form is designed to facilitate block coding activities.
     --------------------------------------------------------------------------
     ---------------- Parameters controlling coding behaviour -----------------
     --------------------------------------------------------------------------
        `size' records the dimensions of the code-block.  These are the
     true dimensions, used by the block encoder or decoder -- they are
     unaffected by regions of interest or geometric transformations.
        `region' identifies the region of interest inside the block.  This
     region is not corrected for any prevailing geometric transformations,
     which may be in force.  The `pos' member identifies the upper left
     hand corner of the region, relative to the upper left hand corner of
     the code-block.  Thus, if the region of interest is the entire code-block,
     `region.pos' will equal (0,0) and `region.size' will equal `size',
     regardless of the geometric transformation flags described below.
        The `transpose', `vflip' and `hflip' fields identify any geometric
     transformations which need to be applied to the code-block samples
     after they have been decoded, or before they are encoded.  These
     quantities are to be interpreted as follows:
         * During decoding, the block is first decoded over its full size.
           If indicated `region' is then extracted from the block and
           transformed as follows.  If `transpose' is true, the extracted
           region is first transposed.  After any such transposition, the
           `vflip' and `hflip' flags are used to flip the region in the
           vertical and horizontal directions, respectively.  Note that all
           of these operations can be collapsed into a function which copies
           data out of the `region' of the block into a buffer used for the
           inverse DWT.
         * During encoding, the region of interest must necessarily be the
           entire block.  The `vflip' and `hflip' flags control whether or
           not the original sample data is flipped vertically and horizontally.
           Then, if `transpose' is true, the appropriately flipped sample
           block is transposed. Finally, the transposed block is encoded.
        The `modes' field contains the logical OR of the coding mode flags.
     These flags may be tested using Cmodes_BYPASS, Cmodes_RESET,
     Cmodes_RESTART, Cmodes_CAUSAL, Cmodes_ERTERM and Cmodes_SEGMARK.
        The `orientation' field contains one of LL_BAND, HL_BAND (horizontally
     high-pass), LH_BAND (vertically high-pass) or HH_BAND.  The subband
     orientation affects context formation for block encoding and decoding.
     The value of this field is unaffected by the geometric transformation
     flags, since it has nothing to do with apparent dimensions.
        The `fussy' and `resilient' flags represent suggestions to a decoder
     to be fussy or attempt error resilience.  The interpretation is discussed
     more carefully in connection with the `kdu_codestream::set_fussy' and
     `kdu_codestream::set_resilient' functions.
     --------------------------------------------------------------------------
     -------- Data produced by the encoder or consumed by the decoder ---------
     --------------------------------------------------------------------------
        The entries in the `pass_slopes' array must be strictly decreasing,
     with the exception that 0's may be interspersed into the sequence.  When
     used for compression, the entries in this array usually hold a suitably
     normalized and shifted version of log(lambda(z)), where lambda(z) is the
     distortion-length slope value for any point on the convex HULL of the
     operational distortion-rate curve for the code-block (see Section 8.2 of
     the book by Taubman and Marcellin for more details).  Zero values
     correspond to points not on the convex HULL.  The final coding pass
     contributed by any given code-block to any quality layer can never have
     a zero-valued slope.
        When used for transcoding, the entries of the `pass_slopes' array
     should be set equal to 0xFFFF minus the index (starting from 0) of the
     quality layer to which the coding pass contributes, except where a later
     coding pass contributes to the same quality layer, in which case the
     `pass_slopes' entry should be 0.  For more details, consult the comments
     appearing with the definition of "kdu_codestream::trans_out".
        When used for input, the `pass_slopes' array is filled out following
     exactly these rules described above for transcoding.
        The `byte_buffer' array is allocated in such a way as to allow
     access to elements with indices in the range -1 through `max_bytes'-1.
     This can be useful when working with the MQ encoder.
     --------------------------------------------------------------------------
     -------- Data produced by the decoder or consumed by the encoder ---------
     --------------------------------------------------------------------------
        The `kdu_block' structure manages shared storage for encoders and
     decoders.  Specifically, the `sample_buffer' and `context_buffer' arrays
     are provided for the benefit of the block encoding and decoding
     implementations.  Their size is not automatically determined by the
     code-block dimensions.  Instead, it is the responsibility of the block
     coding engines to check the size of these arrays and augment them if
     necessary by calling the relevant member functions. */

#endif // KDU_COMPRESSED_H
