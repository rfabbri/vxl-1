/*****************************************************************************/
// File: kdu_sample_processing.h [scope = CORESYS/COMMON]
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
   Uniform interface to sample data processing services: DWT analysis; DWT
synthesis; subband sample encoding and decoding; and colour transformation.
Here, we consider the encoder and decoder objects to be sample data processing
objects, since they accept or produce unquantized subband samples.  They
build on top of the block coding services defined in "kdu_block_coding.h",
adding quantization, ROI adjustments, appearance transformations, and buffering
to interface with the DWT.
******************************************************************************/

#ifndef KDU_SAMPLE_PROCESSING_H
#define KDU_SAMPLE_PROCESSING_H

#include <assert.h>
#include "kdu_messaging.h"
#include "kdu_compressed.h"
#include <cmath>

#include "bbgm_io.h"
#include <dbsta/bsta_attributes.h>
#include <dbsta/bsta_mixture.h>
#include <dbsta/bsta_gauss_if3.h>
#include <dbsta/io/bsta_io_attributes.h>
#include <dbsta/io/bsta_io_mixture.h>
#include <dbsta/io/bsta_io_gaussian_indep.h>
#include <dbbgm/bbgm_image_of.h>
#include <vil/vil_image_view.h>

typedef bsta_gauss_if3 bsta_gauss_i;
typedef bsta_num_obs<bsta_gauss_i> gauss_indep;
typedef bsta_mixture<gauss_indep> mix_gauss_indep3;
typedef bsta_num_obs<mix_gauss_indep3> bgm_mix;
typedef bbgm_image_of<bgm_mix> gauss_model;
// Defined here:
union kdu_sample32;
union kdu_sample16;
class kdu_sample_allocator;
class kdu_line_buf;

class kdu_push_ifc_base;
class kdu_push_ifc;
class kdu_pull_ifc_base;
class kdu_pull_ifc;
class kdu_analysis;
class kdu_synthesis;
class kdu_encoder;
class kdu_decoder;
class bbgm_io;
class kdc_flow_control;
class kde_flow_control;
// Defined elsewhere:
class kdu_roi_node;


/* ========================================================================= */
/*                     Class and Structure Definitions                       */
/* ========================================================================= */

/*****************************************************************************/
/*                              kdu_sample32                                 */
/*****************************************************************************/

union kdu_sample32 {
    float fval;
    kdu_int32 ival;
};

/*****************************************************************************/
/*                              kdu_sample16                                 */
/*****************************************************************************/

#define KDU_FIX_POINT 13 // Num fraction bits in a 16-bit fixed-point value

union kdu_sample16 {
  kdu_int16 ival;
  };

/*****************************************************************************/
/*                          kdu_sample_allocator                             */
/*****************************************************************************/

class kdu_sample_allocator {
  /* This object serves to prevent excessive memory fragmentation.
     Pre-allocation requests are made by all clients which wish to use
     its services, after which a single block of memory is allocated and
     the actual allocation requests must be made -- these must be identical
     to the pre-allocation requests.  Memory chunks allocated by this
     object to a client may not be individually returned or recycled, since
     this incurs overhead and fragmentation.
        The object may be re-used (after a call to `restart()') once all
     memory served out is no longer in use.  When re-used, the object makes
     every attempt to avoid destroying and re-allocating its memory block.
     This avoids memory fragmentation and allocation overhead when processing
     images with multiple tiles or when used for video applications. */
  public: // Member functions
    kdu_sample_allocator(bool bgm_=false)
      { bytes_reserved = bytes_used = buffer_size= 0;
	    bytes_reserved_bg = bytes_used_bg = buffer_size_bg= 0;
        pre_creation_phase=true; buffer = NULL;
		bgm_buffer=NULL;
		bgm=bgm_;
	  }
    ~kdu_sample_allocator()
      { if (buffer != NULL) delete[] buffer; 
	    if (bgm_buffer != NULL) delete[] bgm_buffer; 	
	  }
    void restart()
      { bytes_reserved = bytes_used = 0; pre_creation_phase = true; }
	void pre_alloc(bool use_shorts, int before, int after, int num_requests,bool bbgm=false)
	{ /* Reserves enough storage for `num_requests' later calls to `alloc16'
	  (if `use_shorts' is true) or `alloc32' (if `use_shorts' is false) */
		assert(pre_creation_phase);
		before+=before; after+=after;
		if (!bbgm){
			 // Two bytes per sample
			if (!use_shorts)
			{ before+=before; after+=after; } // Four bytes per sample
			bytes_reserved += num_requests*(((7+before)&~7) + ((7+after)&~7));
		}else{
			before+=before; after+=after;
			int size=sizeof(bgm_mix);
			size=7;
			bytes_reserved_bg += num_requests*(((size+before)&~size) + ((size+after)&~size));
		}
	}
	void finalize()
	{ // Call after all reservations (pre-alloc's) are in.
		assert(pre_creation_phase); pre_creation_phase = false;
			if (bytes_reserved > buffer_size){
				// Otherwise, use the previously allocated buffer.
				buffer_size = bytes_reserved;
				if (buffer != NULL) delete[] buffer;
				buffer = new kdu_byte[buffer_size];
			}
			assert(buffer != NULL);
		
			if (bytes_reserved_bg > buffer_size_bg)
			{ // Otherwise, use the previously allocated buffer.
				buffer_size_bg = bytes_reserved_bg;
				if (bgm_buffer != NULL) delete[] bgm_buffer;
				bgm_buffer = new bgm_mix[buffer_size_bg];
			}
		
	}
    kdu_sample16 *alloc16(int before, int after)
      { /* Allocate an array with space for `before' entries prior to the
           returned pointer and `after' entries after the returned pointer
           (including the entry to which the pointer refers).  The returned
           pointer is guaranteed to be aligned on an 8-byte boundary. */
        assert(!pre_creation_phase);
        before = (before+3)&~3; // Round up to nearest multiple of 8 bytes.
        after = (after+3)&~3; // Round up to nearest multiple of 8 bytes.
        kdu_sample16 *result = (kdu_sample16 *)(buffer+bytes_used);
        result += before; bytes_used += (before+after)<<1;
        assert(bytes_used <= bytes_reserved);
        return result;
      }
    kdu_sample32 *alloc32(int before, int after)
      { // Same as `alloc16', but allocates 32-bit sample arrays.
        assert(!pre_creation_phase);
        before = (before+1)&~1; // Round up to nearest multiple of 8 bytes.
        after = (after+1)&~1; // Round up to nearest multiple of 8 bytes.
        kdu_sample32 *result = (kdu_sample32 *)(buffer+bytes_used);
        result += before; bytes_used += (before+after)<<2;
        assert(bytes_used <= bytes_reserved);
        return result;
	  }
	bgm_mix *allocBg(int before, int after)
	{
		assert(!pre_creation_phase);
        before = (before+1)&~1; // Round up to nearest multiple of 8 bytes.
        after = (after+1)&~1; // Round up to nearest multiple of 8 bytes.
        bgm_mix *result = (bgm_mix*)(bgm_buffer+bytes_used_bg);
		result+=before;	bytes_used_bg+=(before+after)<<2;
        return result;
		
	}
    int get_size()
      { /* For memory consumption statistics.  Returns the maximum size buffer
           which has every been allocated upon finalization. */
        return buffer_size;
      }
  private: // Data
    bool pre_creation_phase; // True if in the pre-creation phase.
    int bytes_reserved,bytes_reserved_bg;
    int bytes_used,bytes_used_bg;
    int buffer_size,buffer_size_bg; // Must be >= `bytes_reserved' except in precreation phase
    kdu_byte *buffer;
	bgm_mix* bgm_buffer;
	bool bgm;
  };

/*****************************************************************************/
/*                              kdu_line_buf                                 */
/*****************************************************************************/

class kdu_line_buf {
  /* Instances of this structure manage the buffering of a single line of
     sample values, whether image samples or subband samples.  For the
     reversible path, samples must be absolute 32- or 16-bit integers.  For
     irreversible processing, samples have a normalized representation, where
     the nominal range of the data is typically -0.5 to +0.5 (actual nominal
     ranges for the data supplied to an encoder or DWT analysis object or
     retrived from a decoder or DWT synthesis object may be explicitly set in
     the relevant constructor).  These normalized quantities may have either a
     true 32-bit floating point representation or a 16-bit fixed-point
     representation.  In the latter case, the least significant
     `KDU_FIX_POINT' bits of the integer are interpreted as binary fraction
     bits, so that 2^{KDU_FIX_POINT} represents a normalized value of 1.0.
        For simplicity, the object always allocates space to allow access to
     2 samples before the first nominal sample and 8 samples after the last
     nominal sample.  This should prove more than sufficient to accommodate
     the needs of boundary extension schemes and efficient SIMD (multi-word)
     implementations of the sample data transformations. */
  // --------------------------------------------------------------------------
  public: // Life cycle functions
    kdu_line_buf()
      { destroy(); }
    void pre_create(kdu_sample_allocator *allocator,
                    int width, bool absolute, bool use_shorts,bool bgm_=false)
      { /* Declares the characteristics of the internal storage which will
           later be created by `create'.  If `use_shorts' is true, the sample
           values will have 16 bits each and normalized values will use a
           fixed point representation with KDU_FIX_POINT fraction bits.
           Otherwise, the sample values have 32 bits each and normalized
           values use a true floating point representation. */
        assert((!pre_created) && (this->allocator == NULL));
        this->width=width; this->short_ints = use_shorts; this->bgm=bgm_; 
        this->absolute_ints=absolute; this->allocator = allocator;
		allocator->pre_alloc(use_shorts,2,width+8,1,bgm);
		pre_created = true;
      }
    void create()
      { /* Finalizes creation of storage which was initiated by `pre_create'.
           Does nothing at all if the `pre_create' function was not called,
           or the object was previously created and has not been destroyed.
           Otherwise, you may not call this function until the `allocator'
           supplied to `pre_create' has had its `finalize' member function
           called.  Automatically sets the state to active. */
        if (!pre_created) return;
        pre_created = false;
        if (short_ints)
          buf16 = allocator->alloc16(2,width+8);
        else if (!bgm)
          buf32 = allocator->alloc32(2,width+8);
		else
		  bgmBuf = allocator->allocBg(2,width+8); 
        active = true;
      }
    void destroy()
      { /* Restores the object to its uninitialized state ready for a new
           `pre_create' call. */
        width=0; absolute_ints=false; short_ints=false; pre_created=false;
        allocator=NULL; buf16=NULL; buf32=NULL; active=false;bgmBuf=NULL;
      }
    void deactivate()
      { /* You can re-activate a line which has been deactivated, but not
           one which has been destroyed. */
        active = false;
      }
    void activate()
      { /* Use to re-activate a line which has been de-activated. */
        assert((!active) && (!pre_created) &&
               ((buf32 != NULL) || (buf16 != NULL)));
        active = true;
      }
    bool is_active()
      { return active; }
    bool operator!()
      { return !active; }
  // --------------------------------------------------------------------------
  public: // Access functions
    kdu_sample32 *get_buf32()
      { /* Returns NULL if the object is not active, or data is 16 bits.
           Otherwise, returned buffer is large enough to accommodate indices
           ranging from -2 to `get_width()'+8.  See introductory comments. */
        return ((!active) || short_ints)?NULL:buf32;
      }

	bgm_mix *get_bgmBuf()
      { /* Returns NULL if the object is not active, or data is 16 bits.
           Otherwise, returned buffer is large enough to accommodate indices
           ranging from -2 to `get_width()'+8.  See introductory comments. */
        return ((!active) || short_ints)?NULL:bgmBuf;
      }

    kdu_sample16 *get_buf16()
      { /* Returns NULL if the object is not active, or data is 32 bits.
           Otherwise, returned buffer is large enough to accommodate indices
           ranging from -2 to `get_width()'+8.  See introductory comments. */
        return (active && short_ints)?buf16:NULL;
      }
    int get_width()
      { // Returns 0 if  the object has not been created.
        return width;
      }
    bool is_absolute()
      { return absolute_ints; }
  // --------------------------------------------------------------------------
  private: // Data -- should occupy 12 bytes on a 32-bit machine.
    int width; // Number of samples in buffer.
    bool absolute_ints;
    bool short_ints;
	bool bgm;
    bool pre_created; // True if `pre_create' called but `create' still pending
    bool active;
    union {
      kdu_sample32 *buf32;
	  bgm_mix* bgmBuf;
      kdu_sample16 *buf16;
      kdu_sample_allocator *allocator;
      };
  };

/*****************************************************************************/
/*                            kdu_push_ifc_base                              */
/*****************************************************************************/

class kdu_push_ifc_base {
  protected:
    friend class kdu_push_ifc;
	virtual ~kdu_push_ifc_base() { return; }
    virtual void push(kdu_line_buf &line, bool allow_exchange) = 0;
  };

/*****************************************************************************/
/*                               kdu_push_ifc                                */
/*****************************************************************************/

class kdu_push_ifc {
  /* All classes which support `push' calls derive from this class so
     that the caller may remain ignorant of the specific type of object to
     which samples are being delivered.  The purpose of derivation is usually
     just to introduce the correct constructor.  The objects are actually just
     interfaces to an object which is created by the constructor, which
     itself must be derived from the pure virtual base class,
     `kdu_push_ifc_base'.  The interface directs push calls to the internal
     object in a manner which should incur no cost. The interface objects may
     be copied at will; the internal object will not be destroyed when an
     interface goes out of scope.  Instead, to destroy the internal object, the
     `destroy' member function must be called explicitly. */
  public: // Member functions
    kdu_push_ifc() { state = NULL; }
    void destroy()
      { if (state != NULL) delete state; state = NULL; }
      /* Automatically destroys all objects which were created by this
         object's constructor, including lower level DWT stages and block
         encoding objects. */
    bool exists()
      { return (state==NULL)?false:true; }
    bool operator!()
      { return (state==NULL)?true:false; }
    kdu_push_ifc &operator=(kdu_analysis rhs);
    kdu_push_ifc &operator=(kdu_encoder rhs);
    virtual void push(kdu_line_buf &line, bool allow_exchange=false)
      { /* Delivers all samples from the line buffer across the interface.
           If `allow_exchange' is true, the function is allowed to exchange
           the storage associated with the supplied `line' and an internal
           line.  However, the two lines must have been allocated using the
           same `allocator' and they must have identical dimensions.  This
           capability has the potential to fragment the memory and so both
           the caller and the implementor of this function should be careful
           to use it only when exchange is likely to be efficient. */
        state->push(line,allow_exchange);
      }
  protected: // Data
     friend class kd_bg_analysis;
	 kdu_push_ifc_base *state;
	
  };

/*****************************************************************************/
/*                            kdu_pull_ifc_base                              */
/*****************************************************************************/

class kdu_pull_ifc_base {
  protected:
    friend class kdu_pull_ifc;
    virtual ~kdu_pull_ifc_base() { return; }
    virtual void pull(kdu_line_buf &line, bool allow_exchange) = 0;
  };

/*****************************************************************************/
/*                               kdu_pull_ifc                                */
/*****************************************************************************/

class kdu_pull_ifc {
  /* All classes which support `pull' calls derive from this class so
     that the caller may remain ignorant of the specific type of object from
     which samples are being retrieved.  The purpose of derivation is usually
     just to introduce the correct constructor.  The objects are actually just
     interfaces to an object which is created by the constructor, which
     itself must be derived from pure virtual base class, `kdu_pull_ifc_base'.
     The interface directs pull calls to this internal object in a manner
     which should incur no cost when compiled in release mode. These interface
     objects may be copied at will; the internal object will not be destroyed
     when an interface goes out of scope.  Instead, to destroy the internal
     object, the `destroy' member function must be called explicitly. */
  public: // Member functions
    kdu_pull_ifc() { state = NULL; }
    void destroy()
      { if (state != NULL) delete state; state = NULL; }
      /* Automatically destroys all objects which were created by this
         object's constructor, including lower level DWT stages and block
         encoding objects. */
    bool exists()
      { return (state==NULL)?false:true; }
    bool operator!()
      { return (state==NULL)?true:false; }
    kdu_pull_ifc &operator=(kdu_synthesis rhs);
    kdu_pull_ifc &operator=(kdu_decoder rhs);
    void pull(kdu_line_buf &line, bool allow_exchange=false)
      { /* Fills out the line before returning. See comments associated with
           `kdu_push_ifc::push' regarding the `allow_exchange' option. */
        state->pull(line,allow_exchange);
      }
  protected: // Data
	friend class kd_bg_synthesis;
	friend class bgm_LL_synthesis;
    kdu_pull_ifc_base *state;
  };

/*****************************************************************************/
/*                              kdu_analysis                                 */
/*****************************************************************************/

class kdu_analysis : public kdu_push_ifc {
  /* Implements a single stage of 2D DWT analysis. */
  public: // Member functions
    KDU_EXPORT
      kdu_analysis(kdu_resolution resolution, kdu_sample_allocator *allocator,
                   bool use_shorts, float normalization=1.0F,
                   kdu_roi_node *roi=NULL);
      /* Constructing an instance of this class for the highest visible
         resolution level of a tile-component, will cause the constructor
         to recursively create instances of the class for each successive
         DWT stage and also for the block encoding process.
            An `allocator' object whose `finalize()' member function has
         not yet been called must be supplied for pre-allocation of the
         various sample buffering arrays.  This same allocator will be shared
         by the entire DWT tree and by the `kdu_encoder' objects at its
         leaves.
            `use_shorts' indicates whether 16-bit or 32-bit data
         representations are to be used.  The same type of representation
         must be used throughput the DWT processing chain and line buffers
         pushed into the DWT engine must use this representation.
            The `normalization' argument will be ignored for reversibly
         transformed data.  In the irreversible case, it indicates that the
         nominal range of data pushed into the `push' interface will be from
         -0.5*R to 0.5*R where R is the `normalization' factor.  This
         capability is provided primarily to allow normalization steps to be
         skipped or approximated with simple powers of 2 during lifting
         implementations of the DWT; the factors can be folded into
         quantization step sizes.  The best way to use the normalization
         argument will generally depend upon the implementation of the DWT.
            The `roi' argument, if non-NULL, provides an appropriately
         derived ROI node object which may be used to recover region of
         interest mask information for the present tile-component.  In this
         case, the object will automatically construct an ROI processing
         tree to provide access to ROI information at the level of each
         individual subband.  The `roi' object's `release' function will
         be called when the `analysis' object is destroyed -- possibly
         sooner (if it can be determined that ROI is no longer required). */
  };
class kdu_bg_analysis : public kdu_push_ifc {
  /* Implements a single stage of 2D DWT analysis. */
  public: // Member functions
    KDU_EXPORT
      kdu_bg_analysis(kdu_resolution resolution, kdu_sample_allocator *allocator,
                   bool use_shorts,bbgm_io& bgm_interface_, float normalization=1.0F,
                   kdu_roi_node *roi=NULL,kdc_flow_control* param_flow=0,kdu_push_ifc* param_analysis=0);
   
  };

/*****************************************************************************/
/*                              kdu_synthesis                                */
/*****************************************************************************/

class kdu_synthesis : public kdu_pull_ifc {
  /* Implements a single stage of 2D DWT synthesis. */
  public: // Member functions
    KDU_EXPORT
      kdu_synthesis(kdu_resolution resolution, kdu_sample_allocator *allocator,
                    bool use_shorts, float normalization=1.0F,bool bgm=false);
      /* Constructing an instance of this class for the highest visible
         resolution level of a tile-component, will cause the constructor
         to recursively create instances of the class for each successive
         DWT stage and also for the block decoding process.
            An `allocator' object whose `finalize()' member function has
         not yet been called must be supplied for pre-allocation of the
         various sample buffering arrays.  This same allocator will be shared
         by the entire DWT tree and by the `kdu_encoder' objects at its
         leaves.
            `use_shorts' indicates whether 16-bit or 32-bit data
         representations are to be used.  The same type of representation
         must be used throughput the DWT processing chain and line buffers
         pulled out of the DWT engine must use this representation.
            The `normalization' argument will be ignored for reversibly
         transformed data.  In the irreversible case, it indicates that the
         nominal range of data pulled out of the `pull' interface should be
         from -0.5*R to 0.5*R, where R is the `normalization' factor.  This
         capability is provided primarily to allow normalization steps to be
         skipped or approximated with simple powers of 2 during lifting
         implementations of the DWT; the factors can be folded into
         quantization step sizes.  The best way to use the normalization
         argument will generally depend upon the implementation of the DWT. */
  };

class kdu_bg_synthesis : public kdu_pull_ifc {
  /* Implements a single stage of 2D DWT analysis. */
  public: // Member functions
    KDU_EXPORT
      kdu_bg_synthesis(kdu_resolution resolution, kdu_sample_allocator *allocator,
                   bool use_shorts,bbgm_io& bgm_interface_, float normalization=1.0F,
                   kde_flow_control* param_flow=0,kdu_pull_ifc* param_synthesis=0);
   
  };

class bgmu_LL_synthesis : public kdu_pull_ifc {
  
  public: // Member functions
    KDU_EXPORT
      bgmu_LL_synthesis(kdu_subband subband, kdu_sample_allocator *allocator,
                   bbgm_io& bgm_interface_,kde_flow_control* param_flow=0,
				   kdu_pull_ifc* param_synthesis=0);
   
  };
/*****************************************************************************/
/*                              kdu_encoder                                  */
/*****************************************************************************/

class kdu_encoder: public kdu_push_ifc {
    /* Implements the block encoding for a single subband, inside a single
       tile-component. */
  public: // Member functions
    KDU_EXPORT
      kdu_encoder(kdu_subband subband, kdu_sample_allocator *allocator,
                  bool use_shorts, float normalization=1.0F,
                  kdu_roi_node *roi=NULL);
      /* Informs the encoder that data supplied via its `push' interface will
         have a nominal range from -0.5*R to +0.5*R where R is the value of
         `normalization'.  The `roi' argument, if non-NULL, provides an
         appropriately derived `kdu_roi_node' object whose `pull' interface
         may be used to recover ROI mask information for this subband.  Its
         `release' function will be called when the encoder is destroyed --
         possibly sooner, if it can be determined that ROI information is
         no longer required. */
  };

/*****************************************************************************/
/*                              kdu_decoder                                  */
/*****************************************************************************/

class kdu_decoder: public kdu_pull_ifc {
    /* Implements the block decoding for a single subband, inside a single
       tile-component. */
  public: // Member functions
    KDU_EXPORT
      kdu_decoder(kdu_subband subband, kdu_sample_allocator *allocator,
                  bool use_shorts, float normalization=1.0F);
      /* Informs the decoder that data retrieved via its `pull' interface
         should have a nominal range from -0.5*R to +0.5*R, where R is the
         value of `normalization'. */
  };

/*****************************************************************************/
/*                    Base Casting Assignment Operators                      */
/*****************************************************************************/

inline kdu_push_ifc &kdu_push_ifc::operator=(kdu_analysis rhs)
  { state = rhs.state; return *this; }
inline kdu_push_ifc &kdu_push_ifc::operator=(kdu_encoder rhs)
  { state = rhs.state; return *this; }

inline kdu_pull_ifc &kdu_pull_ifc::operator=(kdu_synthesis rhs)
  { state = rhs.state; return *this; }
inline kdu_pull_ifc &kdu_pull_ifc::operator=(kdu_decoder rhs)
  { state = rhs.state; return *this; }


/* ========================================================================= */
/*                     External Function Declarations                        */
/* ========================================================================= */

extern KDU_EXPORT void
  kdu_convert_rgb_to_ycc(kdu_line_buf &c1, kdu_line_buf &c2, kdu_line_buf &c3);
  /* The line buffers must be compatible with respect to dimensions and data
     type.  The forward ICT (RGB to YCbCr transform) is performed if the data
     is normalized (i.e. `frac_bits' non-zero).  Otherwise, the RCT is
     performed. */
extern KDU_EXPORT void
  kdu_convert_ycc_to_rgb(kdu_line_buf &c1, kdu_line_buf &c2, kdu_line_buf &c3,
                         int width=-1);
  /* Inverts the effects of the forward transform above.  If `width' is
     negative, the number of samples in each line is determined from the
     line buffers themselves.  Otherwise, only `width' samples are actually
     processed. */

class bbgm_io{
public:
	bbgm_io(const gauss_model* model_in,gauss_model* model_out,int levels)
		:in(model_in),out(model_out),min_var(0.0f)
	{
		this->y_coords=new int*[levels+1];
		this->levels_=levels;
		this->index=0;
		for (int i=0;i<levels+1;i++)
		{
			y_coords[i] =new int[4];
			for (int j=0;j<4;j++)
				y_coords[i][j]=0;
		}
		has_been_deleted=false;
		
	}
	
	
	bbgm_io():in(0),out(0),min_var(0.0f)
	{
		this->y_coords=0;
		this->index=0;
		this->levels_=0;
	}
	bool get(kdu_line_buf& line,int x_tnum)
	{
		assert(index< in->nj());
		for (unsigned int i=0;i<in->ni();i++)
			line.get_bgmBuf()[i]=(*in)(i,index);	
		index++;
		return true;
	}
	bool put(kdu_line_buf& line,int x_tnum)
	{
		assert(index< out->nj());
		bgm_mix* buf=line.get_bgmBuf();
		for (unsigned int i=0;i<out->ni();i++)
			(*out)(i,index)=buf[i];	
		index++;
		return true;
	}
	~bbgm_io()
	{
		if (y_coords!=NULL)
		{
			for (int i=0;i<levels_+1;i++)
			{
				if(y_coords[i]!=NULL)
					delete y_coords[i];
			}
			delete [] y_coords;
			y_coords=NULL;
		}
	}
	bool putLine(bgm_mix* line,int subband,kdu_dims low_dims,
		kdu_dims high_dims,int level)
	{
		int low_width=low_dims.size.x;
		int low_height=low_dims.size.y;
		int high_width=high_dims.size.x;
		int high_height=high_dims.size.y;
		int x0,y0,x1,y1;

		switch(subband)
		{
		case LH_BAND:
			x0=0;
			y0=low_height;
			x1=low_width ;
			y1=high_height;
			break;
		case HL_BAND:
			x0=low_width;
			y0=0;
			x1=high_width;
			y1=low_height;
			break;
		case HH_BAND:
			x0=low_width;
			y0=low_height;
			x1=high_width;
			y1=high_height;
			break;
		case LL_BAND:
			x0=0;
			y0=0;
			x1=low_width;
			y1=low_height;
			break;
		}

		assert(y_coords[level][subband]<y1);
		for (int i=x0,j=0;i<x0+x1;i++,j++)
		{
			(*out)(i,y0+y_coords[level][subband])=line[j];
		}
		y_coords[level][subband]++;
		return true;

	}
	/*bbgm_io& operator =(bbgm_io& other_object)
	{
		this->in=other_object.in;
		this->out=other_object.out;
		this->levels_=other_object.levels_;
		this->y_coords=other_object.y_coords;
		this->index=other_object.index;
		this->comp_coords=other_object.comp_coords;

		return *this;
	}*/

	
	void init_bgm_line(bgm_mix* line,int width)
	{
		for (int i=0;i<width;i++)
			while(line[i].num_components()!=0)
			{
			   line[i].remove_last();
			}	
	}
	void clean_bgm_line(bgm_mix* line,int width)
	{
		for (int i=0;i<width;i++)
		{
			line[i].sort();
			while (line[i].weight(line[i].num_components()-1)==0.0f)
			{
				line[i].remove_last();
				if(line[i].num_components()==0)
					break;
			}
			if (line[i].num_components()>0)
				line[i].normalize_weights();
			
		}
	
	}
	void convert_rv_to_param(kdu_line_buf& line,bgm_mix* rv_line,int param_ind,bool forward=true)
	{
		int mode_idx=param_ind /7; 
		int parameter_no=param_ind %7;
		kdu_sample32* buf=line.get_buf32();
		int width=line.get_width();
		switch (parameter_no)
		{
		case 0: //weight
			put_weight(buf,rv_line,width,mode_idx,forward);
			break;
		case 1:
			put_mean(buf,rv_line,0,width,mode_idx,forward);
			break;
		case 2:
			put_mean(buf,rv_line,1,width,mode_idx,forward);
			break;
		case 3:
			put_mean(buf,rv_line,2,width,mode_idx,forward);
			break;
		case 4:
			put_var(buf,rv_line,0,width,mode_idx,forward);
			break;
		case 5:
			put_var(buf,rv_line,1,width,mode_idx,forward);
			break;
		case 6:
			put_var(buf,rv_line,2,width,mode_idx,forward);
			break;
		}

	}
	bool exists()
	{
		return ((in)|| (out));
	}
	
	
private:
	void put_mean(kdu_sample32* param_line,bgm_mix* rv_line,int mean_index,
		int width ,unsigned int mode_index,bool forward)
	{

		for (int i=0;i<width;i++)
		{
			if (mode_index<rv_line[i].num_components())
			{
				gauss_indep& tmp_dist=rv_line[i].distribution(mode_index);
				if (forward)
				{
					param_line[i].fval=tmp_dist.mean()[mean_index];						
				}
				else if(rv_line[i].weight(mode_index)!=0.0f)
				{
					gauss_indep::vector_ mean=tmp_dist.mean();
					mean[mean_index]=param_line[i].fval;
					tmp_dist.set_mean(mean);
					
					/*if (mean_index==2){
						if(tmp_dist.mean().two_norm()==0.0f)
							rv_line[i].set_weight(mode_index,0.0f);
					
					} */
				}
			}
			else
				param_line[i].fval=0;
		}

	}

	void put_var(kdu_sample32* param_line,bgm_mix* rv_line,int var_index,
		int width ,unsigned int mode_index,bool forward)
	{

		for (int i=0;i<width;i++)
		{
			if (mode_index<rv_line[i].num_components())
			{
				gauss_indep& tmp_dist=rv_line[i].distribution(mode_index);
				if (forward)
					param_line[i].fval =sqrt(tmp_dist.covar()[var_index]);
				else if(rv_line[i].weight(mode_index)!=0.0f)
				{
					gauss_indep::vector_ covar=tmp_dist.covar();
					float val=param_line[i].fval!=0.0f ?
						param_line[i].fval*param_line[i].fval:
						min_var;
					covar[var_index]=val;
					tmp_dist.set_covar(covar);
				}
			}
			else 
				param_line[i].fval=0.0f;
		}

	}

	void put_weight(kdu_sample32* param_line,bgm_mix* rv_line,
		int width ,unsigned int mode_index,bool forward)
	{

		for (int i=0;i<width;i++)
		{
			if (mode_index<rv_line[i].num_components())
				if (forward)
					param_line[i].fval=rv_line[i].weight(mode_index);
				else;
			else if(!forward)
			{
				rv_line[i].insert(gauss_indep(),param_line[i].fval);
			}
			else
			   param_line[i].fval=0;
			if((forward)&&(mode_index==0))
				assert(param_line[i].fval!=0.0);

		}
	}
	

	const gauss_model *in ;
	gauss_model *out;
	int levels_;
	unsigned int index;
	int** y_coords;
	const float min_var;
	bool has_been_deleted;

};
class vil_image_io{
public:
	vil_image_io(const vil_image_view<vxl_byte> image_in,vil_image_view<vxl_byte> image_out,
		int levels,bool use_shorts_,bool reversible_,bool decompress=false)
		:in(image_in),out(image_out),use_shorts(use_shorts_),reversible(reversible_){
				this->levels_=levels;
				if (decompress)
					this->comps_=image_out.nplanes();
				else
					this->comps_=image_in.nplanes();
				this->index=new unsigned int[this->comps_];
				this->y_coords=new int**[this->comps_];

				for (int k=0;k<this->comps_;k++){
					this->y_coords[k]=new int*[levels+1];
					for (int i=0;i<levels+1;i++){
						this->y_coords[k][i] =new int[4];
						for (int j=0;j<4;j++)
							this->y_coords[k][i][j]=0;
					}
					this->index[k]=0;

					has_been_deleted=false;

				}										   
	}
	
	
	vil_image_io():in(vil_image_view<vxl_byte>()),out(vil_image_view<vxl_byte>())
	{
		this->y_coords=0;
		this->index=0;
		this->levels_=0;
		this->comps_=0;
	}
	bool get(int n,kdu_line_buf& line,int x_tnum)
	{
		assert(index[n]< in.nj());
			if (use_shorts)
				for (unsigned int i=0;i<in.ni();i++)
					line.get_buf16()[i].ival=in(i,index[n],n);	
			else if(!reversible)
				for (unsigned int i=0;i<in.ni();i++)
				   line.get_buf32()[i].fval=static_cast<float>(in(i,index[n],n))/128-0.5;
			else
				for (unsigned int i=0;i<in.ni();i++)
					line.get_buf32()[i].ival=in(i,index[n],n);	
				
		index[n]++;
		return true;
	}
	bool put(int n,kdu_line_buf& line,int x_tnum)
	{
		assert(index[n]< out.nj());
		
		if (use_shorts)
			for (unsigned int i=0;i<out.ni();i++)
				out(i,index[n],n)=static_cast<vxl_byte>(line.get_buf16()[i].ival);
		else if (!reversible)
			for (unsigned int i=0;i<out.ni();i++)
				out(i,index[n],n)=static_cast<vxl_byte>((line.get_buf32()[i].fval+0.5)*128);
		else
			for (unsigned int i=0;i<out.ni();i++)
				out(i,index[n],n)=static_cast<vxl_byte>(line.get_buf32()[i].ival);
		index[n]++;
		return true;
	}
	~vil_image_io()
	{
		if (y_coords!=NULL)	{

			for (int k=0;k<comps_;k++){
				for (int i=0;i<levels_+1;i++){
					if(y_coords[k][i]!=NULL)
						delete y_coords[k][i];
				}
				delete [] y_coords[k];
				
			}
			delete [] y_coords;
			delete [] index;
			y_coords=NULL;
		}
	}
	bool putLine(kdu_line_buf& line,int subband,kdu_dims low_dims,
		kdu_dims high_dims,int level,int comp)
	{
		int low_width=low_dims.size.x;
		int low_height=low_dims.size.y;
		int high_width=high_dims.size.x;
		int high_height=high_dims.size.y;
		int x0,y0,x1,y1;

		switch(subband)
		{
		case LH_BAND:
			x0=0;
			y0=low_height;
			x1=low_width ;
			y1=high_height;
			break;
		case HL_BAND:
			x0=low_width;
			y0=0;
			x1=high_width;
			y1=low_height;
			break;
		case HH_BAND:
			x0=low_width;
			y0=low_height;
			x1=high_width;
			y1=high_height;
			break;
		case LL_BAND:
			x0=0;
			y0=0;
			x1=low_width;
			y1=low_height;
			break;
		}

		assert(y_coords[comp][level][subband]<y1);
		for (int i=x0,j=0;i<x0+x1;i++,j++)
		{
			out(i,y0+y_coords[comp][level][subband],comp)=static_cast<vxl_byte>
				(line.get_buf16()[j].ival);	//kinda broken
		}
		y_coords[comp][level][subband]++;
		return true;

	}
	/*bbgm_io& operator =(bbgm_io& other_object)
	{
		this->in=other_object.in;
		this->out=other_object.out;
		this->levels_=other_object.levels_;
		this->y_coords=other_object.y_coords;
		this->index=other_object.index;
		this->comp_coords=other_object.comp_coords;

		return *this;
	}*/

	
			
	bool exists()
	{
		return ((in.top_left_ptr())|| (out.top_left_ptr()));
	}
	
bool use_shorts,reversible;	
private:
	const vil_image_view<vxl_byte> in ;
	vil_image_view<vxl_byte> out;
	int levels_,comps_;
	unsigned int* index;
	int*** y_coords;
	bool has_been_deleted;
	

};
#endif // KDU_SAMPLE_PROCESSING
