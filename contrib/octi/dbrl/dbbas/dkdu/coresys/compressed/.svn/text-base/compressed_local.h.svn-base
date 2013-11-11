/*****************************************************************************/
// File: compressed_local.h [scope = CORESYS/COMPRESSED]
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
   Local definitions employed by the machinery behind the interfaces defined
in "kdu_compressed.h".  These definitions should not be included from any
other scope.  They are for use only in implementing the compressed data
management machinery.
   As a general rule, we use the prefix "kd_" for internal classes and
structures, for which a "kdu_" prefixed interface class exists in the
common scope header file, "kdu_compressed.h".
******************************************************************************/

#ifndef COMPRESSED_LOCAL_H
#define COMPRESSED_LOCAL_H

#include <time.h>
#include <string.h>
#include <assert.h>
#include "kdu_params.h"
#include "kdu_compressed.h"

// The following classes and structures are all defined here:

struct kd_code_buffer;
struct kd_code_alloc;
class kd_buf_server;

class kd_compressed_output;
class kd_input;
class kd_compressed_input;
class kd_pph_input;

class kd_marker;
class kd_pp_marker_list;
class kd_pp_markers;

class kd_packet_sequencer;

class kd_header_in;
class kd_header_out;
class kd_block;

struct kd_codestream;
struct kd_tile;
struct kd_tile_comp;
struct kd_resolution;
struct kd_subband;
struct kd_precinct;
struct kd_precinct_band;


/* ========================================================================= */
/*                         Input Exception Codes                             */
/* ========================================================================= */

#define KDU_EXCEPTION_PRECISION ((kdu_uint16) 13)
    /* Thrown when a packet specifies quantities too large to represent
       within the constraints defined by the current implementation.  This
       is almost certainly due to an erroneously constructed code-stream
       and will be treated as such if the resilient mode is enabled. */
#define KDU_EXCEPTION_ILLEGAL_LAYER ((kdu_uint16) 21)
    /* Thrown when the inclusion tag tree attains an illegal state while
       decoding a packet header.  Illegal states can occur only when one
       or more packets from the same precinct have had their empty packet
       bit set to 0 (i.e. the packet was skipped).  Since no code-block can
       contribute to such packets, the index of the layer in which the
       block is first included cannot be equal to the index of a packet marked
       as empty.  If the inclusion tag tree decodes such a layer index, an
       error must have occurred and this exception is thrown. */

  /* Notes:
        The above exceptions may be thrown during packet header decoding.  In
     addition, if an illegal marker code is encountered while reading any
     portion of a packet, an exception of type "kdu_uint16" will also be
     thrown, having the value of the illegal marker code.  Marker exception
     throwing may not occur unless neither the "fussy" or the "resilient"
     mode is enabled, since watching for in-packet markers is a little
     time-consuming. */

/* ========================================================================= */
/*                           External Functions                              */
/* ========================================================================= */

extern void
  print_marker_code(kdu_uint16 code, std::ostream &out);


/* ========================================================================= */
/*                        Class/Struct Definitions                           */
/* ========================================================================= */

/*****************************************************************************/
/*                              kd_code_buffer                               */
/*****************************************************************************/

#define KD_CODE_BUFFER_LEN 28 // May not exceed 255 under any conditions.
struct kd_code_buffer { // Chunks code bytes to minimize heap transactions
    kd_code_buffer *next;
    kdu_byte buf[KD_CODE_BUFFER_LEN];
  };

/*****************************************************************************/
/*                               kd_code_alloc                               */
/*****************************************************************************/

#define KD_CODE_ALLOC_NUM 200
struct kd_code_alloc {
    kd_code_alloc *next;
    kd_code_buffer bufs[KD_CODE_ALLOC_NUM];
  };

/*****************************************************************************/
/*                               kd_buf_server                               */
/*****************************************************************************/

class kd_buf_server {
  public: // Member functions
    kd_buf_server()
      {
        alloc = NULL; free = NULL;
        total_buffers = num_allocated_buffers = peak_allocated_buffers = 0;
        num_users=0;
      }
    ~kd_buf_server(); // First call `can_destroy' to see if you can destroy it.
    bool can_destroy()
      { return (num_users == 0); }
    void attach()
      { num_users++; }
    void detach()
      { assert(num_users > 0); num_users--; }
    kd_code_buffer *get();
      /* Returns a new code buffer, with its `next' pointer set to NULL. */
    void release(kd_code_buffer *buf);
      /* Recycles a previously acquired code buffer. */
    int get_peak_bytes()
      { /* Returns the maximum number of bytes ever actually allocated by
           the server.  */
        return peak_allocated_buffers * sizeof(kd_code_buffer);
      }
  private: // Data
    kd_code_alloc *alloc;
    kd_code_buffer *free;
    int total_buffers;
    int num_allocated_buffers;
    int peak_allocated_buffers;
    int num_users; // Number of users currently sharing the object.
  };

/*****************************************************************************/
/*                             kd_compressed_output                          */
/*****************************************************************************/

class kd_compressed_output : public kdu_output {
  public: // Member functions
    kd_compressed_output(kdu_compressed_target *target)
      { this->target=target; flushed_bytes = 0; }
    virtual ~kd_compressed_output()
      { flush_buf(); }
    int get_bytes_written()
      { return flushed_bytes + (next_buf-buffer); }
  protected: // Virtual functions which implement required services.
    virtual void flush_buf()
      {
        if (next_buf > buffer)
          target->write(buffer,(next_buf-buffer));
        flushed_bytes += next_buf - buffer;
        next_buf = buffer;
      }
  private: // Data
    kdu_compressed_target *target;
    int flushed_bytes;
  };

/*****************************************************************************/
/*                                kd_input                                   */
/*****************************************************************************/

#define KD_IBUF_SIZE 512
#define KD_IBUF_PUTBACK 6 // Maximum number of bytes you can put back.

class kd_input {
  /* This abstract base class must be derived to construct meaningful
     input devices.  Currently, we have two derived classes in mind: one
     for reading from the code-stream and one for recovering packet
     headers from PPM or PPT markers.  Since there may be many low level
     byte-oriented transactions, we emphasize efficiency for such
     transactions. */
  public: // Member functions
    kd_input()
      { first_unread = first_unwritten = buffer+KD_IBUF_PUTBACK;
        exhausted = false; throw_markers=false; }
    virtual ~kd_input() { return; }
    void enable_marker_throwing(bool reject_all=false)
      { /* If `reject_all' is false, subsequent reads will throw an exception
           if a bona fide SOP or SOT marker is found. In this case, the
           function will check to make sure that the length field is correct,
           putting the length field and marker code bytes back to the source
           before throwing the exception, so that the catch statement can read
           them again.
              If `reject_all' is true, subsequent reads will throw an
           exception if any marker code in the range FF90 to FFFF is found.
           Again, the marker code itself is put back onto the stream before
           throwing the exception.
              Marker throwing is disabled before throwing an exception for
           either of the above reasons.
              Note that code-stream reading may be slowed down
           when this marker throwing is enabled.  For this reason, the
           capability is best used only when error resilience or error
           detection capabilities are required. */
        this->reject_all = reject_all; throw_markers = true; have_FF = false;
      }
    bool disable_marker_throwing()
      { /* Disable marker exception throwing.  Returns true unless the
           last byte which was read was an FF, in which case the function
           returns false.  This allows the caller to catch markers which
           were partially read.  Any code-stream segment which is not
           permitted to contain a marker code in the range FF90 through FFFF
           is also not permitted to terminate with an FF. */
        if (!throw_markers) return true;
        throw_markers = false; // Must not touch `reject_all' here.
        if (exhausted) have_FF = false;
        return !have_FF;
      }
    bool failed()
      { /* Returns true if any of the input functions, `get', `read' or
           `ignore' failed to completely fulfill its objective due to
           exhaustion of the relevant source. */
        return exhausted;
      }
    bool get(kdu_byte &byte) // throws (kdu_uint16 code): unexpected marker
      { /* Access a single byte, returning false if and only if the source is
           exhausted, in which case future calls to `failed' return true. */
        if (exhausted || ((first_unread==first_unwritten) && !load_buf()))
          return false; // If `load_buf' fails it sets `exhausted' for us.
        byte = *(first_unread++);
        if (throw_markers)
          {
            if (have_FF && (byte > 0x8F))
              process_unexpected_marker(byte);
            have_FF = (byte==0xFF);
          }
        return true;
      }
    void putback(kdu_byte byte)
      { /* You may put back more than 1 byte, but you may not call this
           function if a previous read has ever failed.  It is also illegal
           to call this function while marker throwing is enabled. */
        assert(!exhausted);
        assert(!throw_markers);
        assert(first_unread > buffer);
        *(--first_unread) = byte;
      }
    void putback(kdu_uint16 code)
      { /* This function is designed to improve the readability of code
           which puts marker codes back to the input object from which they
           were read. */
        putback((kdu_byte) code);
        putback((kdu_byte)(code>>8));
      }
    int read(kdu_byte *buf, int count);
      /* More efficient than `get' when the number of bytes to be read is
         more than 2 or 3.  Returns the number of bytes actually read.  If
         less than `count', future calls to `failed' will return true.  Note
         that this function is less efficient if marker throwing is enabled. */
    int ignore(int count);
      /* Skips over the indicated number of bytes, returning the number of
         bytes actually skipped.  If less than `count', future calls to
         `failed' will return true. */
  protected: // Data and functions used for buffer management.
    virtual bool load_buf() = 0;
      /* Returns false and sets `exhausted' to true if the source is unable
         to provide any more data.  Otherwise, augments the internal buffer
         and returns true.  Buffer manipulation must conform to the following
         conventions.  The `first_unread' pointer should be set to
         `buffer'+KD_IBUF_PUTBACK and then bytes should be loaded into the
         remaining KD_IBUF_SIZE bytes of the `buffer' array. The
         `first_unwritten' pointer should then be set to point just beyond
         the last byte actually written into the buffer. */
    kdu_byte buffer[KD_IBUF_SIZE+KD_IBUF_PUTBACK];
    kdu_byte *first_unread; // Points to next byte to retrieve from buffer.
    kdu_byte *first_unwritten; // Points beyond last available byte in buffer
    bool exhausted;
  private: // Functions
    void process_unexpected_marker(kdu_byte last_byte);
      /* This function is called when marker throwing is enabled and a
         marker code in the range FF90 through FFFF has been found.  The
         least significant byte of the marker code is supplied as the
         `last_byte' argument.  The function determines whether to throw
         the exception or not.  If not, reading continues unaffected. */
  private: // Data
    bool throw_markers; // If true, must look for unexpected markers.
    bool have_FF; // Valid only with `throw_markers'. Means last byte was FF.
    bool reject_all; // If `false' marker throwing is only for SOP's and SOT's
  };
  
/*****************************************************************************/
/*                             kd_compressed_input                           */
/*****************************************************************************/

class kd_compressed_input : public kd_input {
  public: // Member functions
    kd_compressed_input(kdu_compressed_source *source)
      { this->source=source; max_bytes = bytes_available = INT_MAX; }
    void set_max_bytes(int limit)
      {
        if (limit >= max_bytes)
          return; // Only allow tighter restrictions
        bytes_available -= (max_bytes-limit); max_bytes = limit;
        if (bytes_available < 0)
          { // We have already loaded the buffer with too many bytes.
            first_unwritten += bytes_available; bytes_available = 0;
            if (first_unwritten < first_unread)
              { exhausted = true; first_unwritten = first_unread; }
          }
      }
    int get_bytes_read()
      {
        return (max_bytes-bytes_available)-(first_unwritten-first_unread);
      }
  protected: // Virtual functions which implement required services.
    virtual bool load_buf();
  private: // Data
    kdu_compressed_source *source;
    int bytes_available;
    int max_bytes;
  };

/*****************************************************************************/
/*                               kd_pph_input                                */
/*****************************************************************************/

class kd_pph_input : public kd_input {
  /* Alternate input for packet header bytes, which is used in conjunction
     with the PPM and PPT marker segments. */
  public: // Member functions
    kd_pph_input(kd_buf_server *server)
      { buf_server = server; first_buf = read_buf = write_buf = NULL; }
    virtual ~kd_pph_input();
    void add_bytes(kdu_byte *data, int num_bytes);
      /* This function is called when unpacking a PPM or PPT marker, to
         augment the current object. */
  protected: // Virtual functions which implement required services.
    virtual bool load_buf();
  private: // Data
    kd_code_buffer *first_buf;
    kd_code_buffer *read_buf, *write_buf;
    int read_pos, write_pos;
    kd_buf_server *buf_server;
  };
  /* Notes:
        `first_buf' points to the first in a linked list of buffers used to
     temporarily store PPM or PPT marker segment bytes.
        `read_buf' points to the element in the linked list of code buffers
     which is currently being read; `read_pos' points to the next element of
     the current buffer which will be read. This may be equal to
     KD_CODE_BUFFER_LEN, in which case the next element in the code buffer
     list must be accessed when the next read attempt occurs.
        `write_buf' points to the last element in the linked list of code
     buffers and `write_pos' identifies the first unwritten byte in this
     element.  `write_pos' may equal KD_CODE_BUFFER_LEN, in which case a
     new element will need to be added before further data can be written.
        `buf_server' points to the object which is used to add and release
     code buffer elements.  This is a shared resource. */

/*****************************************************************************/
/*                                kd_marker                                  */
/*****************************************************************************/

class kd_marker {
  /* Objects of this class are used to read and store code-stream marker
     segments. The most efficient way to use the class is to construct a
     single serving object which is used to read the markers one by one and
     then copy any markers which need to be preserved into new instances of
     the class, using the copy constructor.  This concentrates wasted buffer
     space in the one server marker which actually reads from the input
     code-stream. */
  public: // Member functions
    kd_marker(kd_input *input)
      { source = input; code = 0; length = 0; max_length = 0; buf = NULL; }
    kd_marker(const kd_marker &orig);
      /* Copies any marker segment stored in `orig'. Note that the `codestream'
         pointer is not copied, meaning that the `read' member function may
         not be invoked on the copy. */
    ~kd_marker() { if (buf != NULL) delete[] buf; }
    void print_current_code(std::ostream &out)
      { /* Prints a text string identifying the current marker code. */
        print_marker_code(this->code,out);
      }
    bool read(bool exclude_stuff_bytes=false, bool skip_to_marker=false);
      /* Reads a new marker (or marker segment) from the `kd_input' object
         supplied during construction.
             Returns true if successful.  Causes of failure may be: 1) the
         code-stream source is exhausted; 2) a valid marker code was not found;
         or 3) an SOP or SOT marker code was found, but followed by an invalid
         length field.  This third cause of failure is important since it
         prevents the function from attempting to consume an invalid SOP or
         SOT marker segment, in the process of which it might consume any
         number of useful packets, having their own SOP markers.
              A valid marker code must commence with an FF.  If
         `exclude_stuff_bytes' is true then the second byte of a valid marker
         code must be strictly greater than 0x8F.  Otherwise, the second byte
         is arbitrary.  If `skip_to_marker' is true, the function consumes
         bytes indefinitely, until the source is exhausted or a valid marker
         code is found.  In this case, the function returns false only if the
         source is exhausted. Otherwise, the function expects to find a valid
         marker code immediately.
             As a convenience feature, the function automatically skips over
         any EOC marker it encounters.  This has two advantages: 1) we can
         detect the end of the code-stream without having to explicitly check
         for an EOC marker; 2) we should not be overly troubled by EOC
         markers which arise due to corruption of the code-stream (this has
         quite a high likelihood of occurring if the code-stream is subject
         to corruption).
             In the event that the source is not exhausted but a valid
         marker code is not found, the function puts back any bytes it consumed
         before returning false.  This allows the caller to continue reading
         from the point where the function was first called. */
    kdu_uint16 get_code() { return code; }
      /* Returns 0 if no actual marker is available yet. */
    int get_length() { return length; }
      /* Returns length of marker segment (not including the length specifier).
         Returns 0 if no actual marker available, or if the marker is a
         delimiter (no segment). */
    kdu_byte *get_bytes() { return buf; }
      /* Returns pointer to marker segment bytes immediately following the
         length specifier. */
  private: // Data
    kd_input *source; // If NULL, `read' may not be used.
    kdu_uint16 code;
    int length;
    int max_length; // Number of bytes which the buffer can store.
    kdu_byte *buf;
  };

/*****************************************************************************/
/*                              kd_pp_marker_list                            */
/*****************************************************************************/

class kd_pp_marker_list : private kd_marker {
  friend class kd_pp_markers;
  private: // Member functions
    kd_pp_marker_list(kd_marker &copy_source) : kd_marker(copy_source)
      { next = NULL; }
  private: // Data -- These fields are manipulated by `kd_pp_markers'.
    kd_pp_marker_list *next;
    int znum;
    int bytes_read;
  };

/*****************************************************************************/
/*                                 kd_pp_markers                             */
/*****************************************************************************/

class kd_pp_markers {
  public: // Member functions
    kd_pp_markers()
      { list = NULL; }
    ~kd_pp_markers();
    void add_marker(kd_marker &copy_source); 
      /* Copies the `kd_marker' object into a new `kd_pp_marker_list'
         element, inserting it into the evolving list on the basis of its
         Zppm or Zppt index.  Markers need not be added in order. */
    void transfer_tpart(kd_pph_input *pph_input);
      /* Transfers an entire tile-part of packed packet header data from
         the internal marker list to the `pph_input' object, which may
         then be used as a source object for packet header reading.  If the
         object is managing a list of PPM markers, the function expects to
         find the total number of bytes associated with the next tile-part
         embedded in the current marker segment.  Otherwise, all marker
         segments are dumped into the `pph_input' object.  Marker segments
         whose data have been consumed are deleted automatically. */
    void ignore_tpart();
      /* Same as `transfer_tpart' except that the data are simply discarded.
         This is useful when discarding a tile for which PPM marker information
         has been provided. */
  private: // Convenience functions
    void advance_list();
  private: // Data
    bool is_ppm; // Otherwise, list contains PPT markers.
    kd_pp_marker_list *list;
  };

/*****************************************************************************/
/*                             kd_compressed_stats                           */
/*****************************************************************************/

class kd_compressed_stats {
  /* An object of this class is used to monitor statistics of the compression
     process.  One application of these statistics is the provision of feedback
     to the block encoder concerning a conservative lower bound to the
     distortion-length slope threshold which will be found by the PCRD-opt
     rate allocation algorithm when the image has been fully compressed.  This
     allows the encoder to skip coding passes which are almost certain to be
     discarded.
        The current very simple implementation makes the naive assumption
     that the compressibility of all subband samples is the same, regardless
     of the subband or resolution level.  This is OK if the number of samples
     which have been processed for each subband is proportional to the
     size of the subband, since then the average compressibility is a good
     measure to use in predicting rate control properties.  In practice,
     though, delay through the wavelet transform means that the percentage
     of samples seen for higher resolution subbands is generally higher than
     that seen for lower frequency subbands, until everything has been
     compressed. Since the higher frequency subbands are almost invariably
     more compressible, the predicted compressibility of the source is
     estimated too high and so the predicted rate-distortion slope threshold
     passes considerably more bits than the final rate allocation will.  This
     renders the prediction excessively conservative for most images.  A
     good fix for this is to keep track of the percentage of subband samples
     which have been compressed in each resolution level and use this
     information to form a more reliable predictor.  This improvement will
     be included in a future version. */
  public: // Member functions
    kd_compressed_stats(int total_samples, int target_bytes)
      {
        this->total_samples = total_samples;
        next_trim = (total_samples+7) >> 3;
        conservative_extra_samples = 4096 + (total_samples>>4);
        target_rate =
          (total_samples==0)?1.0:(((double) target_bytes)/total_samples);
        num_coded_samples = 0;
        min_quant_slope = 4095; max_quant_slope = 0;
        for (int n=0; n < 4096; n++)
          quant_slope_rates[n] = 0;
      }
    bool update(kdu_block *block)
      { /* Invoked by "kdu_subband::close_block".  If the function returns
           true, it is recommended that the compressed data be trimmed back
           to a size consistent with the target compressed length at this
           point. */
        num_coded_samples += block->size.x*block->size.y;
        assert(num_coded_samples <= total_samples);
        int quant_slope, length = 0;
        for (int n=0; n < block->num_passes; n++)
          {
            length += block->pass_lengths[n];
            if (block->pass_slopes[n] == 0)
              continue;
            quant_slope = block->pass_slopes[n] >> 4;
            if (quant_slope < min_quant_slope) min_quant_slope = quant_slope;
            if (quant_slope > max_quant_slope) max_quant_slope = quant_slope;
            quant_slope_rates[quant_slope] += length;
            length = 0;
          }
        if (num_coded_samples > next_trim)
          { next_trim += (total_samples+7)>>4; return true; }
        return false;
      }
    kdu_uint16 get_conservative_slope_threshold(bool assume_all_coded=false)
      { /* The slope threshold generated by PCRD-opt is unlikely to be
           smaller than that returned here -- remember that smaller thresholds
           mean that more compressed data are included in the code-stream.
           If `all_coded' is true, the slope threshold is based on the
           assumption that no bytes will be generated for any further samples.
           Otherwise, the assumption is that future samples will have the
           same average compressibility as those already coded.  Note that
           since compressors tend to output a higher proportion of high
           frequency subband samples than low frequency subband samples at
           any point up to the end of the image, the actual compressibility
           of future samples can be expected to be lower on average, making
           our estimate more conservative. */
        int max_bytes;
        if (assume_all_coded)
          max_bytes = (int)(target_rate * total_samples);
        else
          max_bytes = (int)(target_rate *
                            (num_coded_samples + conservative_extra_samples));
        int n, cumulative_bytes = 0;
        for (n=max_quant_slope; n >= min_quant_slope; n--)
          if ((cumulative_bytes += quant_slope_rates[n]) >= max_bytes)
            break;
        return (kdu_uint16)((n>0)?((n<<4)-1):1);
      }
  private: // Data
    double target_rate; // Expressed in bytes per sample, not bits per sample.
    int total_samples; // Total number of subband sample in entire image
    int next_trim; // Number of samples at which memory should next be trimmed
    int conservative_extra_samples; // Add to `num_coded_samples' to be safe.
    int num_coded_samples; // Number of samples already coded
    int quant_slope_rates[4096]; // See below
    int min_quant_slope, max_quant_slope;
  };
  /* Notes:
        The `quant_slope_rates' array holds the total number of coded
     bytes which belong to each of a set of 4096 quantized distortion-length
     slope bins.  The original distortion-length slopes have a 16-bit unsigned
     logarithmic representation (see discussion of the "pass_slopes" member
     array in the "kdu_block" structure).  The index of the bin to which a
     given slope, lambda, belongs is given by
         bin = floor(lambda/16).
     Rounding down means that 16*bin is the smallest slope consistent with the
     bin so that summing the entries from bin to 4095 yields the maximum number
     of code-block bytes which could be contributed to the compressed image
     if the PCRD-opt algorithm selected a slope threshold of 16*bin.  Note,
     however, that bin 0 represents only those slopes in the range 1 to 15,
     since 0 is not a valid slope threshold. */

/*****************************************************************************/
/*                             kd_packet_sequencer                           */
/*****************************************************************************/

class kd_packet_sequencer {
  public: // Member functions
    kd_packet_sequencer(kd_tile *tile);
    void save_state();
      /* Saves all quantities associated with packet sequencing, both within
         this object and in the associated tile and its constituent precincts,
         so that they can be later restored to resume sequencing from where
         we left off.  We take advantage of the fact that this function can
         only be called on tile-part boundaries, at which point a new
         progression must be instated.  Thus, the `restore_state' function
         can accomplish most of its task by invoking the `next_progression'
         function. */
    void restore_state();
      /* Restores the state saved by the `save_state' member function. */
    kd_precinct *next_in_sequence();
      /* This function returns a pointer to the next precinct for which a
         new packet must be sequenced. If the precinct does not exist, it
         is constructed automatically before the function returns.  The
         caller normally proceeds to read or write a packet of data for the
         indicated precinct, which increments its `next_layer_idx' field
         in the process. In this case, or if the precinct is destroyed in
         the meantime (this can only happen if all layers have been read
         or written), then the next time this function is called
         the next packet in the sequence will be retrieved.  However, there
         is no need for the caller to do anything at all with the returned
         precinct pointer.  In this case, the next time the function is
         called, it will return the same precinct.
             The function returns NULL if no further packets can be
         sequenced based on the information available for the current
         tile-part. In this event, the caller must read or write another
         tile-part before proceeding to sequence more packets.  The
         function determines that no further packets can be sequenced
         if one of the following two events occur: 1) no further instances
         of the POC marker are currently available; or 2) the next
         POC marker instance index exceeds the current tile-part
         index.  This policy has the desirable effect that the presence
         of multiple POC attributes during compression results in the
         generation of multiple tile-parts.
             The function also returns NULL if all packets have been
         transferred to or from the code-stream. This event may be detected
         by checking the tile's `num_transferred_packets' field, which should
         be incremented whenever a packet is read or written to or from
         the code-stream.  The current function does not modify this field
         itself. */
  private: // Member functions
    bool next_progression();
      /* Sets up the next progression (or the very first progression).
         Returns false, if no further progressions are available without
         starting a new tile-part. */
    kd_precinct *next_in_lrcp(); // These functions return NULL if there
    kd_precinct *next_in_rlcp(); // are no more packets to sequence within
    kd_precinct *next_in_rpcl(); // the current progression order.
    kd_precinct *next_in_pcrl();
    kd_precinct *next_in_cprl();
  private: // Current sequencing bounds.
    int order; // One of the orders defined in the scope of `cod_params'.
    int res_min, comp_min; // Lower bounds (inclusive) for loops.
    int layer_lim, res_lim, comp_lim; // Upper bounds (exclusive) for loops.
  private: // Current looping state.
    int layer_idx; // Current layer index
    int comp_idx; // Current component index
    int res_idx; // Current resolution level index
    kdu_coords pos; // Position indices of precinct in current tile-comp-res.
  private: // Spatial sequencing parameters.
    bool common_grids; // True if all components have power-of-2 sub-sampling.
    kdu_coords grid_min; // Common grid start for position-component type loops
    kdu_coords grid_inc; // Common grid increments for pos-component type loops
    kdu_coords grid_lim; // This is the point just beyond tile on the canvas.
    kdu_coords grid_loc; // Common grid location for pos-component type loops
  private: // Access to tile and subordinate structures.
    kd_tile *tile;
    int max_dwt_levels; // Over all components.
    kdu_params *poc; // NULL unless POC-based sequencing.
    kdu_params *saved_poc; // Used by the `save_state' member function
    int next_poc_record; // Index of next order record in current POC.
  };

/*****************************************************************************/
/*                             kd_codestream                                 */
/*****************************************************************************/

struct kd_codestream { // State structure for the "kdu_codestream" interface
  public: // Member functions
    kd_codestream()
      { // We will be relying on all fields starting out as 0.
        memset(this,0,sizeof(*this));
      }
    ~kd_codestream();
    void construct_common();
      /* Called from within `kdu_codestream::create' after some initial
         construction work to set the thing up for input or output. */
    kd_tile *create_tile(int tnum);
      /* Creates and completely initializes a new tile, installing in the
         internal array of tile references and returning a pointer to the
         new tile.  The pointer returned may be equal to KD_EXPIRED_TILE if
         the tile was found not to belong to the current region of interest. */
    void var_structure_new(int num_bytes)
      { /* This function is called whenever a new tile, component, resolution,
           subband, precinct or code-block structure is created, with the
           size of the created structure.  It keeps a tally of the number of
           heap bytes tied up in such structures, since they are all
           transient and regarded as part of the compressed data storage
           system. */
        var_structure_bytes += num_bytes;
        if (var_structure_bytes > peak_var_structure_bytes)
          peak_var_structure_bytes = var_structure_bytes;
      }
    void var_structure_delete(int num_bytes)
      { /* This function is called when the elements responsible for calls
           to `var_structure_new' are deleted. */
        var_structure_bytes -= num_bytes;
        assert(var_structure_bytes >= 0);
      }
    void from_apparent(kdu_coords &pt)
      { // Convert point from apparent to real coords
        pt.x=(hflip)?(-pt.x):pt.x;
        pt.y=(vflip)?(-pt.y):pt.y;
        if (transpose) pt.transpose();
      }
    void from_apparent(kdu_dims &dims)
      { // Convert region from apparent to real coords
        if (hflip) dims.pos.x = -(dims.pos.x+dims.size.x-1);
        if (vflip) dims.pos.y = -(dims.pos.y+dims.size.y-1);
        if (transpose) dims.transpose();
      }
    void to_apparent(kdu_coords &pt)
      {  // Convert point from real to apparent coords
        if (transpose) pt.transpose();
        pt.x = (hflip)?(-pt.x):pt.x;
        pt.y = (vflip)?(-pt.y):pt.y;
      }
    void to_apparent(kdu_dims &dims)
      { // Convert region from real to apparent coords
        if (transpose) dims.transpose();
        if (hflip) dims.pos.x = -(dims.pos.x+dims.size.x-1);
        if (vflip) dims.pos.y = -(dims.pos.y+dims.size.y-1);
      }
    void trim_compressed_data();
      /* This function may be called periodically to trim away the storage
         associated with compressed data bytes which will never be included
         in the final compressed representation.  The function can do nothing
         unless `kdu_codestream::set_max_bytes' has been called to establish
         a target compressed length. */
    void get_min_header_cost(int &fixed_cost, int &per_layer_cost);
      /* Discovers the minimum total number of header bytes required by the
         code-stream.  Specifically the minimum header cost for the first
         k quality layers is equal to `fixed_cost' + `per_layer_cost'*k.
         The fixed cost includes the main header, any tile headers and the
         EOC marker.  The per-layer cost includes 1 byte for every
         precinct, plus an additional 2 bytes per precinct if EPH markers
         are used and an additional 6 bytes per precinct if SOP markers are
         used.  To understand how this minimum header cost is useful for
         rate allocation, see the description of "kdu_codestream::flush". */
    int simulate_output(int &header_bytes,
                        int layer_idx, kdu_uint16 slope_threshold,
                        bool finalize_layer, bool last_layer,
                        int max_bytes=INT_MAX, int *sloppy_bytes=NULL);
      /* This function may be called only once all precincts of all
         tile-component resolutions have been completed.  Its purpose is
         to simulate the process of generating final code-stream output for
         a single quality layer, so as to determine the number of bytes
         occupied by that layer, given a particular distortion-length slope
         threshold.  A slope threshold of 0 yields the largest possible
         output size and 0xFFFF yielding the smallest possible output size
         (all code-block bytes discarded).
            The number of bytes returned includes the extra cost of tile and
         packet headers.  The number of such header bytes is also separately
         returned via the `header_bytes' argument.  Although not strictly
         correct, the function currently incorporates the cost of additional
         tile-part headers into the layer 0 size.  This can lead to slightly
         conservative rate control when working with multiple tile-parts.
         The cost of tile-part headers, however, should not usually be
         substantial.
            The `finalize_layer' argument should be true if this is the last
         call to this function with the current layer index.  This causes the
         packet header state information to be saved to a place from which
         it will be restored in each simulation attempt for the next layer.
         Also, it is mandatory to finalize the layers during simulation
         before actually generating packet data.
            The `last_layer' argument should be true if this is the final
         layer for which any information will be generated.  If any tile
         contains more layers, their contribution will be included at this
         point as empty packets (i.e., the empty packet bit is set to 0).
         Any such empty packet is never assigned an SOP marker.
            The `max_bytes' argument may be used to supply an upper bound on
         the total number of bytes which are allowed to be generated here.
         This allows the simulation process to stop as soon as this limit
         is exceeded.  In this case, the returned byte count may not be a
         true reflection of the full cost of generating the layer, but it
         is guaranteed to exceed the `max_bytes' limit.
            When the function is called with `finalize_layer' true, the
         `max_bytes' limit must not, under any circumstances be exceeded.
         The assumption is that the caller has already done the work of
         figuring out a slope threshold which will satisfy the relevant
         rate constraint -- if not, `max_bytes' can be set to INT_MAX (the
         default).
            A non-NULL `sloppy_bytes' argument may be supplied only when
         finalizing the last layer.  In this case, the caller must be certain
         that a slope threshold of `slope_threshold'+1 can satisfy the
         limit imposed by `max_bytes'.  The function first runs the low
         level packet-oriented simulation tools with this value of
         `slope_threshold'+1 and then runs them again (this time finalizing)
         with `slope_threshold'.  On the second run, any additional coding
         passes which would be contributed due to the reduced threshold are
         trimmed away from the relevant code-blocks (they cannot be recovered
         later on) until the number of additional bytes is less than the value
         of *`sloppy_bytes'.  The *`sloppy_bytes' is progressively decremented,
         so that upon return the caller may determine the total number of
         available bytes which could not be used up.  The caller should then
         generate packet data using the supplied slope threshold.
            This `sloppy_bytes' option slows down the rate control process and
         is recommended primarily when you can expect quite a few code-block
         passes to have the same length-distortion slope value.  This happens
         during transcoding, where the only slope information available comes
         from the indices of the quality layers to which the coding passes
         originally contributed.  It may also happen if the block encoder
         does not collect a rich set of length-distortion statistics, simply
         estimating a rough prioritization instead. */
    void pcrd_opt(int layer_bytes[], kdu_uint16 slope_thresholds[],
                  int num_layers);
      /* Runs the PCRD-opt algorithm to find the slope thresholds for each
         quality layer.  Fills in the contents of the `slope_thresholds' array
         and may overwrite the contents of the `layer_bytes' array.  On entry,
         the `layer_bytes' entries have the same interpretation as that
         described in connection with the "kdu_codestream::flush" function.
         On exit, each entry in this array identifies the number of bytes
         which the layer will occupy in the final code-stream. */
    void generate_codestream(int max_layers, kdu_uint16 slope_thresholds[]);
      /* Generates the final code-stream.  The "pcrd_opt" member function
         should have been used to find the distortion-length slope thresholds
         for the various quality layers.  In any event, some simulation must
         have been performed (i.e., calls to "kd_tile::simulate_output")
         so as to set up the packet size information required to determine
         tile-part header sizes (these are required by the SOT marker
         segments). */
  public: // Data
    kd_compressed_input *in; // NULL unless an input codestream object.
    kd_marker *marker; // General marker reading service. NULL if `in' is NULL
    kd_compressed_output *out; // NULL unless an output codestream object.
    std::ostream *textualize_out; // NULL unless params to be textualized
    siz_params *siz;
    int num_components;
    kdu_coords *sub_sampling; // Component sub-sampling factors.
    float *crg_x, *crg_y; // Component registration offsets.
    int *precision; // Original component bit-depths.
    bool *is_signed; // Signed/unsigned nature of original components
    kdu_dims canvas; // Holds the image origin and dimensions.
    kdu_dims tile_partition; // Holds the tiling origin and dimensions.
    kdu_coords num_tiles; // Number of tiles in each direction.
    int first_apparent_component, num_apparent_components;
    int discard_levels, max_apparent_layers;
    int max_tile_layers; // Max layers in any tile opened so far.
    bool transpose, vflip, hflip; // Geometric manipulation parameters
    bool resilient; // If true, error resilience is maximized.
    bool expect_ubiquitous_sops; // See declaration of "set_resilient" function
    bool fussy; // If true, sensitivity to correctness is maximized.
    bool persistent;
    bool tiles_accessed; // Becomes false after first tile access
    int num_open_tiles; // Number of tiles opened, but not yet closed.
    kdu_dims region; // Current region of interest; often identical to `canvas'
    kd_tile **tile_refs; // An array of pointers to `kd_tile' objects.
    kd_tile *active_tile; // NULL unless we are in the middle of reading a tile
    kd_buf_server *buf_server; // Serves resources to buffer compressed data
    kdu_block *shared_block; // Resource checked by `kdu_subband::open_block'
    kd_pp_markers *ppm_markers; // NULL unless PPM markers are used.
    int num_tparts_used; // Number of tile-parts actually read or written.
    kd_compressed_stats *stats; // NULL unless rate prediction is being used.
    kdu_uint16 min_slope_threshold; // 0 until `set_min_slope_threshold' called
    clock_t start_time; // Used by `report_cpu_time'.
    int peak_var_structure_bytes; // Peak block, precinct & tile struct bytes
  private:
    int var_structure_bytes; // Bytes in block, precinct & tile structures
  };
  /* Notes:
         The fields listed below `initialized' become valid only once
     kdu_codestream::initialize has been called.  This is not true of the
     parameters listed above `initialized'.
         The `tile_refs' field points to an array of tile pointers all of which
     are initially NULL. When a tile is first accessed, the relevant `kd_tile'
     object is created and its pointer is inserted in this array.  When the
     tile has expired, the pointer value is set to KD_EXPIRED_TILE, marking it
     as unusable.  The array is organized in scan-line fashion based on the
     original code-stream geometry (not the apparent geometry).
         The `num_tparts_used' count does not include tiles which we skipped
     over since they had no intersection with the region of interest. */

/*****************************************************************************/
/*                                kd_tile                                    */
/*****************************************************************************/

#define KD_EXPIRED_TILE ((kd_tile *)(-1))

struct kd_tile { // State structure for the "kdu_tile" interface
  public: // Member functions
    kd_tile(kd_codestream *codestream, int tnum);
      /* Sets up only enough information for the tile to locate itself and
         for a determination to be made as to whether the tile belongs to
         a current region of interest. */
    ~kd_tile();
      /* Note that this function also detaches the tile object from the
         codestream object which references it, replacing the pointer with
         the special illegal value, KD_EXPIRED_TILE. */
    void initialize();
      /* Called immediately after a successful construction.  Builds all
         subordinate structures and fills in all fields. */
    void open();
      /* Called by `kdu_codestream::open_tile' after the tile has been
         constructed and initialized (if necessary).  The purpose of this
         function is to mark the tile as open (until it is subsequently
         closed) and to reflect any prevailing region of interest and
         limitations on the number of resolution levels and/or image
         components into the dependent structures.  This allows the appearance
         of the image to be changed and then reflected into any desired
         tile when it is re-opened -- this behaviour is supported only for
         persistent input objects (see `kdu_codestream::set_persistent'). */
    bool read_tile_part_header();
      /* Returns false, if the code-stream source was exhausted before a
         new tile-part header could be completely read (up to and including
         the SOD marker).  Except in this event, the current tile become's
         the codestream object's active tile upon return. */
    void finished_reading();
      /* This function may be called as soon as we can be sure that no more
         information can be read for the tile.  When this happens, the tile
         is marked as `exhausted' and a search is conducted to see if we can
         destroy any precincts whose existence serves only to allow the parsing
         of packet headers. */
    int simulate_output(int &header_bytes,
                        int layer_idx, kdu_uint16 slope_threshold,
                        bool finalize_layer, bool last_layer,
                        int max_bytes=INT_MAX, int *sloppy_bytes=NULL);
      /* Implements the functionality of `kd_codestream::simulate_output'
         for a single tile. */
    int generate_tile_part(int max_layers, kdu_uint16 slope_thresholds[]);
      /* If all packets for the tile have already been transferred, the
         function does nothing and returns 0.  Otherwise, it generates the
         next tile-part header and all packets for that tile-part, using the
         packet sequencer to determine both the order of packets and the
         number of packets which belong to this tile-part.  The function
         returns the total number of bytes in the tile-part, including all
         marker codes and segments.
            The `max_layers' argument indicates the number of quality layers
         for which rate allocation information is available in the
         `slope_thresholds' array.  If the tile contains additional layers,
         the corresponding packets are assign the special empty packet code,
         occupying only a single byte. */
  public: // Data
    kd_codestream *codestream;
    int tnum;
    bool is_open;
    bool closed; // Once closed, the tile may no longer be accessed.
    kdu_dims dims; // Region occupied by the tile on the canvas
    kdu_dims region; // Region of interest within tile.
    bool initialized; // Fields below here are valid only once initialized
    int num_components;
    int first_apparent_component, num_apparent_components;
    int num_tparts; // Zero if number of tile-parts unknown.
    int next_tpart; // Index of next tile-part to be read or written.
    bool use_sop, use_eph, use_ycc;
    kdu_coords coding_origin;
    int num_layers;
    int num_apparent_layers;
    kd_tile_comp *comps;
    int total_precincts;
    int total_packets;
    int num_transferred_packets; // Incremented on reading/writing new packet.
    bool skipping_to_sop; // True only if trying to recover from an error.
    int next_sop_sequence_num; // Valid only if `skipping_to_sop' is true.
    int saved_num_transferred_packets; // For "kdu_packet_sequencer::save_state".
    bool exhausted; // True if no more tile-parts are available for this tile.
    kd_packet_sequencer *sequencer;
    kd_pph_input *packed_headers; // NULL unless tile has packed packet headers
  };
  /* Notes:
        If `skipping_to_sop' is true, packets are automatically discarded
     (this is a pseudo-transfer which leaves the packet empty) until one is
     reached whose sequence number (same as `num_transferred_packets') agrees
     with the value of `next_sop_sequence_num'. */

/*****************************************************************************/
/*                              kd_tile_comp                                 */
/*****************************************************************************/

struct kd_tile_comp { // State structure for the "kdu_tile_comp" interface
  public: // Member functions
    kd_tile_comp()
      { // We will be relying on all fields starting out as 0.
        memset(this,0,sizeof(*this));
      }
    ~kd_tile_comp();
  public: // Data
    kd_codestream *codestream;
    kd_tile *tile;
    int cnum;
    kdu_coords sub_sampling;
    kdu_dims dims;
    kdu_dims region;
    int dwt_levels, apparent_dwt_levels;
    bool reversible;
    int kernel_id; // One of Ckernels_W5X3 or Ckernels_W9X7.
    int recommended_extra_bits;
    kdu_coords blk; // Nominal code-block dimensions.
    int modes; // Block coder modes.  Flags defined in scope `cod_params'
    kdu_coords grid_min; // These fields are used by the packet sequencer
    kdu_coords grid_inc; // for spatially oriented progressions.  They are
    kd_resolution *resolutions; // Contains 1+`dwt_levels' entries.
  };
  /* Notes:
     `recommended_extra_bits' may be added to the sample bit-depth
     (precision) for this component to determine an appropriate number
     of bits for internal representations associated with the data
     processing path.
        In the reversible path, the value is set to the maximum bit-depth
     expansion factor, as reported in Table 17.4 of the book by Taubman and
     Marcellin, adding an extra 1 bit if the RCT has been used -- note that it
     is not desirable for luminance and chrominance components to have
     different numeric representations, since that would significantly
     complicate the colour transform procedure.
        In the irreversible path, the value is set to 7, which means
     that 16-bit representations are judged sufficient for 9 bit
     imagery or less.  Of course, the impact of selecting a reduced
     representation precision is only reduced accuracy in the
     irreversible processing path. */

/*****************************************************************************/
/*                              kd_resolution                                */
/*****************************************************************************/

struct kd_resolution { // State structure for the "kdu_resolution" interface
  public: // Member functions
    kd_resolution()
      { // We will be relying on all fields starting out as 0.
        memset(this,0,sizeof(*this));
      }
    ~kd_resolution();
  public: // Data
    kd_codestream *codestream;
    kd_tile_comp *tile_comp;
    int res_level; // Runs from 0 (LL band) to `num_dwt_levels'
    int dwt_level; // Runs from `num_dwt_levels' (res level 0 and 1) to 1 
    bool propagate_roi;
    kdu_dims dims;
    kdu_dims region;
    kdu_dims precinct_partition; // Holds the coding origin and dimensions.
    kdu_dims precinct_indices; // Holds the range of valid precinct indices.
    kd_precinct **precinct_refs; // All pointers initially NULL.
    kdu_coords current_sequencer_pos; // Used by `kd_packet_sequencer'.
    int min_band, max_band;
    kd_subband *bands;
  };
  /* Notes:
        The `current_sequencer_pos' coordinates are used to sequence
     spatially oriented packet progressions.  They maintain the
     indices (starting from [0,0], rather than `precinct_indices.pos')
     of the precinct which is currently being sequenced.
        The `bands' array may be de-referenced with indices in the range
     0 through `max_band', even though the actual band indices may
     not be smaller than `min_band'. This is a convenience measure. */

/*****************************************************************************/
/*                                kd_subband                                 */
/*****************************************************************************/

struct kd_subband { // State structure for the "kdu_subband" interface
  public: // Member functions
    kd_subband()
      { // We will be relying on all fields starting out as 0.
        memset(this,0,sizeof(*this));
      }
  public: // Data
    kd_codestream *codestream;
    kd_resolution *resolution;
    int which_band; // one of LL_BAND, HL_BAND, LH_BAND or HH_BAND.
    kdu_coords band_idx; // Each coordinate is 0 for low-pass, 1 for high-pass.
    kdu_dims dims;
    kdu_dims region;
    int epsilon; // Ranging parameter.
    int K_max; // Maximum magnitude bit-planes, not including ROI upshift.
    int K_max_prime; // Maximum magnitude bit-planes, including ROI upshift.
    float delta; // Step size, normalized for image data range of -0.5 to 0.5
    float G_b; // Subband energy gain factor.
    float W_b; // Subband amplitude weight; needs squaring to get energy weight
    float roi_weight; // Extra amplitude weight for ROI foreground blocks.
    kdu_dims block_partition; // Holds the coding origin and dimensions.
    kdu_dims block_indices; // Holds the range of valid block indices.
    kdu_dims region_indices;  // Same as `block_indices', but restricted to
                              // blocks which overlap the region of interest.
    kdu_coords blocks_per_precinct; // precinct size / block size
    kdu_coords log2_blocks_per_precinct; // log2 of precinct size/block size
  };
  /* Notes:
        It may come as a surprise to notice that this object has no
     direct connection to code-blocks.  To access a code-block, the object
     first determines the precinct to which it belongs, creating that
     precinct if necessary; this, in turn, creates its precinct-bands and
     the associated code-blocks.  In this way, no memory whatsoever need be
     dedicated to code-blocks which lie in unaccessed precincts. */

/*****************************************************************************/
/*                             kd_header_in                                  */
/*****************************************************************************/

class kd_header_in {
  /* Manages the reading of packet header bytes from the code-stream, along
     with bit stuffing and unpacking. */
  public: // Member functions
    kd_header_in(kd_input *source)
      { this->source = source; bits_left=0; byte=0; }
    int get_bit() // throws its own "this" pointer if the source is exhausted.
      {
        if (bits_left==0)
          {
            bits_left = (byte==0xFF)?7:8;
            if (!source->get(byte))
              throw this;
          }
        bits_left--;
        return (byte >> bits_left) & 1;
      }
    kdu_uint32 get_bits(int num_bits) // thorws(kd_header_in *)
      { kdu_uint32 result = 0;
        while (num_bits > 0)
          {
            if (bits_left==0)
              {
                bits_left = (byte==0xFF)?7:8;
                if (!source->get(byte))
                  throw this;
              }
            int xfer_bits = (num_bits<bits_left)?num_bits:bits_left;
            bits_left -= xfer_bits;
            num_bits -= xfer_bits;
            result <<= xfer_bits;
            result |= ((byte >> bits_left) & ~(0xFF << xfer_bits));
          }
        return result;
      }
    void finish()
      { // Call this when the header is all read, to consume any stuffing byte
        if ((bits_left == 0) && (byte == 0xFF))
          {
            bits_left = 7;
            if (!source->get(byte))
              throw this;
          }
      }
  private: // Data
    kd_input *source; // Provides the input mechanism.
    kdu_byte byte;
    int bits_left;
  };

/*****************************************************************************/
/*                            kd_header_out                                  */
/*****************************************************************************/

class kd_header_out {
  /* Manages the simulated and real output of packet header bytes. */
  public: // Member functions
    kd_header_out(kdu_output *out=NULL) // Null for simulating header
      { byte = 0; bits_left = 8; completed_bytes = 0; this->out = out; }
    void put_bit(int bit)
      { // Output a header bit
        assert(bit == (bit & 1));
        if (bits_left == 0)
          { if (out != NULL) out->put(byte);
            completed_bytes++;
            bits_left = (byte==0xFF)?7:8;
            byte = 0; }
        byte += byte + bit;
        bits_left--;
      }
    void put_bits(kdu_int32 val, int num_bits)
      { // Output the least significant `num_bits' of `val'
        while (num_bits > 0)
          put_bit((val >> (--num_bits)) & 1);
      }
    int finish()
      { // Returns the total number of bytes consumed by the packet header.
        if (bits_left < 8)
          {
            byte <<= bits_left;
            if (out != NULL)
              out->put(byte);
            completed_bytes++;
            if (byte == 0xFF)
              { // Need the stuffing byte.
                if (out != NULL)
                  out->put((kdu_byte) 0);
                completed_bytes++;
              }
          }
        return completed_bytes;
      }
  private: // Data
    kdu_byte byte;
    int bits_left;
    int completed_bytes;
    kdu_output *out;
  };

/*****************************************************************************/
/*                                 kd_block                                  */
/*****************************************************************************/

class kd_block {
  /* For a very small size (only 24 bytes on 32-bit machines), this class
     defines a lot of member functions!. */
  // --------------------------------------------------------------------------
  public: // Lifecycle member functions
    void cleanup(kd_precinct_band *pband); // Must call this before destroying
  // --------------------------------------------------------------------------
  public: // Packet manipulation member functions
    int parse_packet_header(kd_header_in &head, kd_buf_server *buf_server,
                            int layer_idx); // Throws (kdu_uint16 code)
      /* Parses bits in the packet header corresponding to the current
         block for the current layer (this must be the next layer for which
         information has not yet been parsed).  The function returns the
         number of new body bytes contributed by the code-block in this layer
         and also stores this value internally to be used in subsequent calls
         to the `read_body_bytes' member function.
            The `kd_header_in' object may throw an exception if the packet
         data terminates unexpectedly.  Moreover, to assist in locating and
         possibly recovering from errors, the present function may throw a
         16-bit unsigned integer exception code, which is either the value of
         an illegal marker code encountered in the packet header, or one of
         the special exception codes, KDU_EXCEPTION_PRECISION or
         KDU_EXCEPTION_ILLEGAL_LAYER. */
    void read_body_bytes(kd_input *source, kd_buf_server *buf_server);
      /* This function should be called after parsing the header of a
         packet.  It reads the code-bytes contributed by the code-block to
         this packet. */
    void retrieve_data(kdu_block *block, int max_layers);
      /* Retrieves compressed code-bytes, number of coding passes and codeword
         segment lengths from the current code-block and stores them in the
         `block' object.  All fields which are common for compression and
         decompression have already been filled out by the caller.  The
         byte buffer is allocated so as to be large enough to allow a
         2 byte marker code to be appended to the compressed data,
         if desired -- see declaration of "kdu_subband::open_block".
            The `max_layers' field identifies the maximum number of
         quality layers for which code-block contributions are to be
         retrieved. */
    void store_data(kdu_block *block, kd_buf_server *buf_server);
      /* Stores compressed code-bytes, pass length and pass slope information
         from the `block' object into the current code-block.  This function
         is invoked by "kdu_subband::close_block" when the containing
         codestream object was not constructed for input.  Also reflects
         the `msbs_w' value into higher nodes in the tag tree. */
    bool trim_data(kdu_uint16 slope_threshold, kd_buf_server *buf_server);
      /* This function serves two purposes: it helps reduce the amount of
         compressed data which must be buffered in memory while compressing
         an image; and it allows individual block contributions to be
         discarded during packet construction in order to tightly fit the
         generated bit-rate to the target compressed bit-rate.
            The function sets all coding pass slopes which are less than or
         equal to the supplied slope threshold to 0 and then discards all
         code bytes which could not possibly be included in the final
         code-stream, as a result of this modification.
            If anything was discarded, the function returns true.  Otherwise,
         it returns false. */
    int start_packet(int layer_idx, kdu_uint16 slope_threshold);
      /* Call this function only during rate allocation or packet generation.
         Before the first call to this function, you must reset the tag tree
         (see the "reset_output_tree" member function).  If you are calling
         the function multiple times with the same layer for rate allocation
         purposes, you must use "save_output_tree" and "restore_output_tree"
         as described below.
            The function leaves behind information about the number of new
         coding passes and the number of new code bytes which would be
         contributed by this code-block if the supplied threshold were actually
         used.  The final contributing coding pass must have a non-zero
         distortion-length slope which is strictly greater than the slope
         threshold.  It also updates the tag tree structure to reflect any
         new knowledge about the first layer to which this code-block would
         contribute.  This information is picked up by "write_packet_header".
            The function's return value is the total number of new code-bytes
         contributed by the block.  This allows the PCRD-opt algorithm to
         determine roughly the right slope threshold before committing to the
         work of simulating packet header generation. */
    void write_packet_header(kd_header_out &head, int layer_idx,
                             bool simulate);
      /* Call this function only after a `start_packet' call.  It writes
         (or simulates) the writing of packet header information corresponding
         to the code-block inclusion information set up by `start_packet'.
         If `simulate' is true, some internal state adjustments are delayed
         until the next call to "save_output_tree", so multiple calls to
         "start_packet" and "write_packet_header" may be issued with the same
         layer index to determine an optimal slope threshold.  Otherwise,
         if `simulate' is false, the internal state information is fully
         updated and the "save_output_tree" and "restore_output_tree" functions
         need not and should not be called. */
    void write_body_bytes(kdu_output *dest);
      /* This function should be called after a non-simulated
         "write_packet_header" call to output the newly included body bytes.
         The actual number of bytes to be written was left behind by
         the preceding "start_packet" call in the `temp_length' field. */
  // --------------------------------------------------------------------------
  public: // Static functions used to operate on tag-trees built from kd_block
    static kd_block *build_tree(kdu_coords size, int *num_nodes=NULL);
      /* Builds a full tag tree returning a pointer to the upper left hand
         corner leaf node.  Apart from the `up_down' navigation pointers,
         all fields are initialized to 0.  This is appropriate for tag tree
         decoding, but encoders will need to call "reset_output_tree" and
         use "start_packet" to install appropriate node values.  The
         `num_nodes' argument, if not NULL, is used to return a count of
         the total number of nodes in the allocated array, including leaf
         nodes and higher level tag tree nodes.  This is primarily of
         interest for memory accounting purposes. */
    static void reset_output_tree(kd_block *tree, kdu_coords size);
      /* Call this function immediately before simulating or actually
         generating any packet data for the relevant code-blocks.  Once
         the PCRD-opt algorithm has completed, you should call this function
         a second time to prepare for actual packet output.  The function
         initializes the threshold state variables, `layer_w_bar' and
         `msbs_w_bar' to 0, and the value state variables, `layer_w' and
         `msbs_w' to their maximum representable values.  The succession of
         "start_packet" calls will update the value variables `layer_w'
         and `msbs_w' in such a way that the tag tree coding process runs
         correctly in each subsequent call to "write_packet_header". */
    static void save_output_tree(kd_block *tree, kdu_coords size);
      /* Call this only during rate allocation, before moving on to a new
         layer.  The function saves the current state (`msbs_wbar' and
         `layer_wbar') of each node in the tag tree.  For leaf nodes, it also
         commits the number of new coding passes found in the last call to
         "start_packet". */
    static void restore_output_tree(kd_block *tree, kdu_coords size);
      /* Call this during rate allocation before supplying a new slope
         threshold for the same layer index, in a new "start_packet" call.
         The function restores the `msbs_w_bar' and `layer_w_bar' values
         previously saved by "save_output_tree". */
  // --------------------------------------------------------------------------
  public: // State identification functions
    void set_discard()
      { /* Use this function only with input-oriented operations.  It informs
           the object that none of the code-blocks associated with this
           block will ever be accessed, so that it is free to skip over them
           when reading a packet.  This can save a lot of memory if you have
           a defined region of interest for decompression. */
        num_passes = 255;
      }
    bool empty()
      { // Returns true if the block has no code-bytes yet.
        return first_buf == NULL;
      }
    void set_modes(int modes)
      {
        assert(modes == (modes & 0xFF));
        this->modes = (kdu_byte) modes;
      }
  // --------------------------------------------------------------------------
  private: // Convenience functions for transferring data to/from code buffers.
    void start_buffering(kd_buf_server *buf_server) // Call before `put_byte'
      { assert(first_buf == NULL);
        first_buf = current_buf = buf_server->get(); buf_pos = 0; }
    void put_byte(kdu_byte val, kd_buf_server *buf_server)
      {
        assert(current_buf != NULL);
        if (buf_pos==KD_CODE_BUFFER_LEN)
          { buf_pos=0; current_buf=current_buf->next=buf_server->get(); }
        current_buf->buf[buf_pos++] = val;
      }
    kdu_byte get_byte()
      {
        if (buf_pos==KD_CODE_BUFFER_LEN)
          { buf_pos=0; current_buf=current_buf->next;
            assert(current_buf != NULL); }
        return current_buf->buf[buf_pos++];
      }
  // --------------------------------------------------------------------------
  private: // Data (lots of unions to make it as small as possible!)
    union {
      kd_code_buffer *first_buf;
      kdu_uint16 save_layer_w; // Used for saving non-leaf node state
      };
    union {
      kd_code_buffer *current_buf;
      kdu_uint16 save_layer_wbar; // Used for saving_non-leaf node state
      };
    union {
      kdu_byte buf_pos; // Location of next read/write byte in `current_buf'.
      kdu_byte save_msbs_wbar; // Used for saving non-leaf node state
      };
    kdu_byte modes;
    kdu_byte msbs_w;
    union {
      kdu_byte msbs_wbar;
      kdu_byte beta;
      };
    union {
      kdu_uint16 layer_w; // Used only when `pass_idx'=0
      kdu_uint16 body_bytes_offset; // Used in packet output if `pass_idx' > 0
      kdu_byte save_beta; // Used during output simulation if `pass_idx' > 0
      };
    union {
      kdu_uint16 layer_wbar;
      kdu_uint16 num_bytes; // Used only for input operations
      kdu_byte pending_new_passes; // Used only for output operations
      };
    kdu_byte num_passes;
    kdu_byte pass_idx;
    kdu_uint16 temp_length; // Temporary storage of length quantities.
    kd_block *up_down;
  };
  /* Notes:
        This structure has been designed to provide sufficient state
     information for each code-block to perform packet header parsing as
     well as block encoding and decoding.  For coding purposes, the information
     represented by the structure is generally transferred to or from a
     much less tightly packed representation for speed of execution of the
     block coding routines.  As it stands, the total size of this structure
     should be 24 bytes on a 32-bit architecture. An array of code-blocks is
     maintained by each precinct-band.  The array contains one entry for
     every block in the relevant precinct-band, but also contains additional
     entries to be used as tag-tree nodes.  The total number of such extra
     nodes is about 1/3 the number of code-blocks and should not be a
     significant cause of concern for overall memory consumption. There is
     also substantial benefit to be had by locating the tag tree nodes in
     the same contiguous memory block.  Notice that the structure contains
     no reference back to the precinct-band to which it belongs.  When
     necessary, this reference must be supplied separately to interested
     functions. One consequence of this memory saving step is that the
     object's destructor cannot do anything useful.  It only checks to make
     sure that the explicit `cleanup' function has already been called.
        If a block is to be discarded (since it does not intersect with a
     user-specified region of interest), the `num_passes' field will hold
     the otherwise impossible value, 255.
        The first 3 fields constitute the compressed data buffer management
     mechanism which is used to store all quantities whose length is
     unpredictable a priori. In particular, a linked list of `kd_code_buffer'
     structures is provided, which grows on demand through access to the
     codestream object's embedded `buffering' object.  Data is stored in these
     buffers in the following formats:
           * Input Format -- [<layer #> (<#bytes> <#passes> ...)] data [<...
             That is, for each layer to which the code-block makes non-empty
             contribution, we record first the layer number (2 bytes) and
             then one or more #bytes/#passes pairs.  #bytes is represented
             using 2 bytes, while #passes is represented using only 1 byte.
             There is one such 3 byte pair for each layer and one additional
             3 byte pair for each codeword segment terminating pass which
             is not the final pass contributed in the layer.  In this way,
             sufficient information is provided to recover all termination
             lengths, as well as to build layer associations.  The most
             significant bit of the #bytes field is used as a marker to
             indicate whether or not this is the last such field prior to
             the appearance of the code bytes contributed by that layer.  A
             1 means that there are more fields to go before the code bytes
             appear.
           * Output Format -- [<RD slope> <#bytes>] ... [<RD ...] data
             That is, all of the header information for all coding passes
             appears up front, followed by all of the code bytes associated
             with those coding passes.  The reason for putting all the
             header information up front is that it facilitates the
             identification of appropriate layer contributions by the
             rate control algorithm.  Each coding pass has a 4 byte header,
             with 2 bytes for the distortion-length slope value and 2 bytes
             for the number of code-bytes for the pass.
     The particular format which is used to store data depends upon the
     member functions which are used, since these implicitly determine
     whether the codestream object is being used for input or output.
        The `modes' field holds the block coder mode fields.  Some of these
     are required even for correct packet header parsing, regardless of whether
     the block is to be actually decoded.
        `msbs_w' and `msbs_wbar' are the state information required
     for tag-tree coding of the number of missing MSB's for a code-block
     which is about to make its first non-empty contribution to the
     code-stream. Similarly, `layer_w' and `layer_wbar' hold the state
     information required for tag-tree coding of the layer index in which a
     code-block first contributes to the code-stream.  The operation of
     tag-tree coding and the `w' and `wbar' terminology are explained in
     Section 8.4 of the book by Taubman and Marcellin.
        The `beta' field shares storage with `msbs_wbar'.  This is not
     required until the number of missing MSB's has been encoded or decoded,
     at which point we initialize `beta' to 3 (see the JPEG2000 standard or
     Section 12.5.4 of the book by Taubman and Marcellin).  A useful
     consequence of this behaviour is that `beta' may be tested at any
     point of the packet header encoding or decoding process to determine
     whether or not the code-block has yet contributed to the code-stream
     -- the value will be non-zero if and only if it has done so.
        The `num_bytes' field shares storage with `layer_wbar'.  It
     identifies the total number of bytes included by the code-block in
     all packets read or written so far.  The value is taken to be 0 if
     the code-block has not yet contributed to any packets (`beta'=0),
     during which time the `layer_wbar' state variable is in use.
        The `up_down' field is used to efficiently navigate the tag-tree
     structure.  When the tag tree is not actually being navigated, the
     field points up to the current node's parent in the tag tree.  The
     root mode is readily identified by the fact that it has a NULL `up_down'
     pointer. During navigation, the tag-tree coder first walks from a
     leaf node up to the root, modifying the `up_down' pointers as it goes
     so that each successive parent's `up_down' field points bacl down to the
     child from which the parent was reached.  The coder then walks back down
     from the root to the original leaf node, restoring the `up_down' fields to
     their original `up' state. */

/*****************************************************************************/
/*                             kd_precinct_band                              */
/*****************************************************************************/

struct kd_precinct_band {
  /* Used to store state information for an element of the precinct partition
     within a single subband.  Once we get down to precincts and ultimately
     code-blocks, we have to be careful not to waste too much memory, since
     an image may have a very large number of these.  Moreover, it may be
     necessary to maintain tag-trees and other state information even for
     precincts which do not lie within our region of interest, so that the
     relevant packet headers can be parsed -- otherwise, we may have no way
     of knowing how to skip over packets which are not interesting to us. */
  public: // Member functions
    kd_precinct_band() { blocks = NULL; }
  public: // Data
    kd_subband *subband;
    kdu_dims block_indices; /* Range of code-block indices for this precinct.
        This is a subset of the `block_indices' specified by the `subband'. */
    kd_block *blocks;
  };
  /* Notes:
     In order to make the representation of this object as efficient as
     possible, the two tag trees and all additional information required to
     correctly parse packet headers is maintained within the code-block
     array itself.  The `blocks' pointer is NULL if the precinct contains
     no blocks, or is to be skipped over by taking advantage of SOP
     markers.  Otherwise, the `blocks' array consists of one element for each
     code-block, organized in raster fashion, plus additional elements to
     represent higher nodes in the tag-tree structure.  See the comments
     associated with the definition of `kd_block' for more information. */

/*****************************************************************************/
/*                                kd_precinct                                */
/*****************************************************************************/

#define KD_EXPIRED_PRECINCT ((kd_precinct *)(-1))

struct kd_precinct {
  public: // Member functions
    kd_precinct(kd_resolution *resolution, kdu_coords pos_idx);
      /* `pos_idx' holds the horizontal and vertical position of the
         precinct (starting from 0) within the array of precincts for
         the relevant resolution level.  The constructor puts a pointer
         to itself into the `resolution' object's `precinct_refs' array. */
    ~kd_precinct();
      /* Note that this function also detaches the precinct object from the
         resolution object which references it, replacing the pointer with
         the special illegal value, KD_EXPIRED_PRECINCT. */
    bool read_packet();
      /* Reads a new layer of this precinct from the code-stream.  Note
         that it is illegal to call this function if the precinct does
         not belong to a currently active tile.  The function returns false
         if an SOT marker (or an EOC marker) is encountered.  Note that the
         SOT marker segment is fully read into the codestream marker service.
         Before returning false in this way, the function sets the
         `active_tile' pointer in the codestream object to NULL.  The caller
         may determine the cause of failure by invoking the codestream marker
         service's `get_code' function, which will return one of SOT, EOC or 0,
         where 0 occurs only if the code-stream has been exhausted.
            If the function succeeds, it returns true, after first incrementing
         both the `next_layer_idx' member field and the containing `kd_tile'
         structure's `num_transferred_packets' field. */
    int simulate_packet(int &header_bytes,
                        int layer_idx, kdu_uint16 slope_threshold,
                        bool finalize_layer, bool last_layer,
                        int max_bytes=INT_MAX, bool trim_to_limit=false);
      /* Implements the functionality of `kd_tile::simulate_output'
         for a single precinct.  Note that the total number of packet bytes
         is left behind in the internal `layer_bytes' array.  This allows the
         determination of tile-part length fields later on when we come to
         construct the code-stream.  The function returns this total number
         of bytes.  It also returns the number of packet header bytes via
         `header_bytes' (the function return value includes the cost of these
         header bytes).
            If the `max_bytes' limit is exceeded, the function may return
         prematurely, without having completed the simulation.  This is
         unacceptable when finalizing a layer, so you must be sure that
         `max_bytes' is sufficient in that case.
            If `trim_to_limit' is true, `finalize_layer' and `last_layer'
         must also both be true.  The function then trims away all coding
         passes from the precinct's code-blocks which are associated with
         slope values of `slope_threshold'+1 or less, until the limit
         associated with `max_bytes' is satisfied.  This functionality is
         used to implement the `sloppy_bytes' capabilities advertised by the
         `kd_tile::simulate_output' function. */
    int write_packet(kdu_uint16 slope_threshold, bool empty_packet=false);
      /* Writes out the next packet for this precinct, using the supplied
         distortion-length slope threshold.  Increments the `next_layer_idx'
         field as well as the containing tile's `num_transferred_packets'
         field when done.  If `empty_packet' is true, an empty packet should
         be issued, having a length of 1 byte, plus the length of any required
         EPH marker (no SOP marker will be attached to empty packets).  The
         function returns the total number of bytes output for the packet. */
  private: // Member functions
    bool handle_corrupt_packet();
      /* This function implements alternate processing for the `read_packet'
         function in the event that a packet is found to be corrupt.  It is
         called whenever packet corruption is detected, including when the
         precinct is known to have had previously corrupt packets. The function
         manipulates the containing tile's `skipping_to_sop' and
         `next_sop_sequence_num' fields, advancing to the next SOP or SOT
         marker if necessary (the function may be called after detecting an
         SOP marker already, whose sequence number is not as expected).  If
         an SOT marker is encountered, the function renders the current tile
         inactive and returns false (this is the behaviour expected of
         `read_packet').  Otherwise, it updates the precinct and tile state to
         identify the packet as having been read. */
  public: // Data
    kd_resolution *resolution;
    kdu_coords pos_idx; // Index within the resolution object's array.
    int num_layers;
    int next_layer_idx;
    bool corrupted; // True an error is encountered while reading a packet
    int saved_next_layer_idx; // Used by `kd_packet_sequencer::save_state()'.
    kd_precinct_band bands[4];
    int num_outstanding_blocks;
    int total_block_nodes; // Used for memory accounting purposes.
    int *packet_bytes;  // Packet lengths generated during simulated output.
  };
  /* Notes:
        `bands' holds 4 elements, even though no resolution level can have
     more than 3 subbands.  Valid elements in the array have indices in the
     range `min_bands' through `max_bands' where these are public members of
     the `resolution' structure.
        `num_outstanding_blocks' indicates the number of code-blocks in
     the region of interest which have yet to be read or written.  Once this
     reaches 0, we may be able to destroy the precinct. */

#endif // COMPRESSED_LOCAL_H
