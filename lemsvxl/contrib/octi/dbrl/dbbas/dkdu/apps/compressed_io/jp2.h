/*****************************************************************************/
// File: jp2.h [scope = APPS/COMPRESSED-IO]
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
   Defines classes which together embody the capabilities expected of
conformant readers and writers of the JP2 file format.  The `jp2_source'
and `jp2_target' classes define objects which derive from the abstract
compressed I/O base classes `kdu_compressed_source' and
`kdu_compressed_target'. These may be used to construct `kdu_codestream'
objects for generating or decompressing the embedded JPEG2000 code-stream box.
Methods are also provided to describe, interpret and process the
code-stream image samples in a conformant manner.
******************************************************************************/

#ifndef JP2_H
#define JP2_H
#include "kdu_compressed.h"
#include "kdu_params.h"
#include "kdu_sample_processing.h"

// Defined here
class jp2_dimensions;
class jp2_palette;
class jp2_channels;
class jp2_colour;
class jp2_resolution;
class jp2_source;
class jp2_target;

// Defined elsewhere
class j2_dimensions;
class j2_palette;
class j2_channels;
class j2_colour;
class j2_resolution;
class j2_source;
class j2_target;


/*****************************************************************************/
/*                              jp2_dimensions                               */
/*****************************************************************************/

  /* Note: Objects of this class are merely interfaces to an internal
     implementation object, `j2_dimensions', which cannot be directly
     created by an application.  Creation and destruction of the internal
     object, as well as saving and reading of JP2 boxes are capabilities
     reserved for the internal machinery. Amongst other things, this
     protects the user against the possibility of losing control of
     resources when error conditions throw exceptions. */

class jp2_dimensions {
  public: // Lifecycle member functions
    jp2_dimensions()
      { state = NULL; }
    jp2_dimensions(j2_dimensions *state)
      { this->state = state; }
    bool operator!()
      { return (state == NULL); }
    bool exists()
      { return (state != NULL); }
  // --------------------------------------------------------------------------
  public: // Initialization member functions
    void init(kdu_coords size, int num_components, bool unknown_space=true);
      /* Initializes the internal dimensions object with the indicated number
         of image components.  It is illegal to call this function unless
         there is an internal object (`exists' must return true) and that
         object has not yet been initialized.  In practice, this happens only
         when setting up a `jp2_target' object to construct a JP2 file.  To
         complete the initialization, precision information must be supplied
         for each image component (see below).
            The `size' argument holds the height and width of the
         image region on the canvas.  This information is almost entirely
         useless since it cannot be mapped to image component dimensions
         without considering sub-sampling factors and the location of the
         image region on the canvas.  It may be useful in establishing
         the size of an scanned page when used in conjunction with explicit
         capture resolution information.
            The `unknown_space' argument is true if the colour space
         information accessed via the `jp2_colour' object is not known to
         be correct.  This is often the case in practice. */
    void set_precision(int component_idx, int bit_depth, bool is_signed=false);
      /* Sets the bit-depth and signed/unsigned characteristics of the
         indicated image component.  `component_idx' must be in the range 0
         through `num_components'-1.  It is necessary to supply this
         information for every image component.  Otherwise, the object
         will be judged incomplete.
            Note that the JP2 file format was really designed with unsigned
         representations in mind.  In some cases, the use of signed
         representations is illegal and in others it may create ambiguity.
         Users are strongly encouraged to adopt unsigned representations
         whenever using the JP2 file format. */
    void init(siz_params *siz, bool unknown_space=true);
      /* Completely initializes the object, based upon the information which
         will be included in the code-stream SIZ marker segment.  This is the
         best way to ensure consistency of the redundant image header box. */
  // --------------------------------------------------------------------------
  public: // Access member functions.
    kdu_coords get_size();
      /* Returns the size of the image region on the canvas.  This
         information is almost entirely useless by itself.  Moreover, it
         is redundant with the more complete and useful information embedded
         in the code-stream's SIZ marker segment.  If the size is needed,
         it should generally be obtained by invoking the more reliable and
         informative `kdu_codestream::get_dims' member function, assuming
         that a `kdu_codestream' object is constructed. */
    int get_num_components();
      /* Returns the number of code-stream image components.  Again, this
         information is best provided by the `kdu_codestream' object. */
    bool colour_space_known();
      /* Returns false if the colour space information provided by the
         `jp2_colour' object is unreliable. */
    int get_bit_depth(int component_idx);
      /* Returns the bit-depth of the indicated image component (indices
         start from 0). */
    bool get_signed(int component_idx);
      /* Indicates whether the indicated image component (indices start
         from 0) has a signed representation.  A false return value means
         that the quantities are unsigned.  Note that all quantities processed
         by the core Kakadu system are signed quantities.  To obtain
         unsigned values, an offset of half the dynamic range must be
         added to them.  In most cases, this is the appropriate action
         to take before subjecting the data to any palette mapping,
         lookup tables, colour conversion, etc.  However, if this function
         returns true, the data should be left in its signed representation
         and all of the above operations should be performed on the signed
         quantities.  For lookup table operations, this will mean that
         negative values must be clipped to 0.
            The JP2 file format was actually designed with unsigned
         representations in mind, so that some things make no sense, are
         clumsy or else are illegal unless the samples have an unsigned
         representation. */
  // --------------------------------------------------------------------------
  private: // Data
    j2_dimensions *state;
  };

/*****************************************************************************/
/*                               jp2_palette                                 */
/*****************************************************************************/

  /* Note: Objects of this class are merely interfaces to an internal
     implementation object, `j2_palette', which cannot be directly
     created by an application.  Creation and destruction of the internal
     object, as well as saving and reading of JP2 boxes are capabilities
     reserved for the internal machinery. Amongst other things, this
     protects the user against the possibility of losing control of
     resources when error conditions throw exceptions. */

class jp2_palette {
  public: // Lifecycle member functions
    jp2_palette()
      { state = NULL; }
    jp2_palette(j2_palette *state)
      { this->state = state; }
    bool operator!()
      { return (state == NULL); }
    bool exists()
      { return (state != NULL); }
  // --------------------------------------------------------------------------
  public: // Initialization member functions
    void init(int num_components, int num_entries);
      /* Initializes the internal palette object with the indicated number
         of image components and palette entries.  It is illegal to call this
         function unless there is an internal object (`exists' must return
         true) and that object has not yet been initialized.  In practice,
         this happens only when setting up a `jp2_target' object to construct
         a JP2 file.  To complete the initialization, the individual component
         lookup tables must be set up (see below). */
    void set_lut(int comp_idx, kdu_int32 *lut, int bit_depth,
                 bool is_signed=false);
      /* Sets the palette LUT for the indicated component, where `comp_idx'
         must lie in the range 0 through `num_components'-1.  The `lut' array
         must have at least `num_entries' entries and the sample values must
         lie in the range -2^{bit_depth-1} to +2^{bit_depth-1}-1, if
         `is_signed' is true, or else 0 to 2^{bit_depth}-1, if `is_signed'
         is false. */
  // --------------------------------------------------------------------------
  public: // Access member functions
    int get_num_entries();
      /* The input component samples are to be  clipped to the range
         0 through `entries'-1, where `entries' is the value returned
         by this function.  Returns 0 if no palette is being used. */
    int get_num_components();
      /* Returns the number of new image components created by the
         palette lookup table.  Note, however, that the standard does
         not require all of these components to be used.  Nor does
         it prevent any given component from being used multiple
         times.  This function returns 0 if no palette is being used. */
    int get_bit_depth(int comp_idx);
      /* Returns the bit-depth of the indicated component, where
         `comp_idx' must be in the range 0 through `num_components'-1
         and `num_components' is the value returned by
         `get_num_components'. */
    bool get_signed(int comp_idx);
      /* Returns true if the indicated component has a signed
         representation.  Otherwise, returns false.  Note that
         Palette LUT's are always converted to signed representations
         anyway, so there are not many applications which should
         need to know whether the representation is signed or
         unsigned, except when writing the de-palettized samples
         to a file which is capable of representing signed
         quantites. */
    void get_lut(int comp_idx, float lut[]);
      /* Offsets and normalizes the original information for the indicated
         palette component, writing the result to the supplied `lut' array.
         The `comp_idx' value must be in the range 0 to `num_components'-1,
         where `num_components' is the value returned by `get_num_components'.
         The `lut' array must contain at least as many entries as the value
         returned by `get_num_entries'.  The `lut' entries are filled out as
         signed quantities in the range -0.5 to 0.5. */
    void get_lut(int comp_idx, kdu_sample16 lut[]);
      /* As above, except the lut holds the signed, normalized quantites
         in a 16-bit fixed point format, having KDU_FIX_POINT fraction bits.
         Again, the range of the quantities represented using this format is
         -0.5 to 0.5.  The selection of this representation is intended to
         facilitate interaction with the sample data processing operations
         conducted by the objects defined in "kdu_sample_processing.h". */
  // --------------------------------------------------------------------------
  private: // Embedded state
    j2_palette *state;
  };

/*****************************************************************************/
/*                              jp2_channels                                 */
/*****************************************************************************/

  /* Note: Objects of this class are merely interfaces to an internal
     implementation object, `j2_channels', which cannot be directly
     created by an application.  Creation and destruction of the internal
     object, as well as saving and reading of JP2 boxes are capabilities
     reserved for the internal machinery. Amongst other things, this
     protects the user against the possibility of losing control of
     resources when error conditions throw exceptions. */

class jp2_channels {
  public: // Lifecycle member functions
    jp2_channels()
      { state = NULL; }
    jp2_channels(j2_channels *state)
      { this->state = state; }
    bool operator!()
      { return (state == NULL); }
    bool exists()
      { return (state != NULL); }
  // --------------------------------------------------------------------------
  public: // Initialization member functions
    void init(int num_colours);
      /* Initializes the internal channels object with the indicated number
         of colour channels.  It is illegal to call this function unless
         there is an internal object (`exists' must return true) and that
         object has not yet been initialized.  In practice, this happens only
         when setting up a `jp2_target' object to construct a JP2 file.
         Note that the number of colour components must be either 1
         (luminance) or 3 (RGB). */
    void set_colour_mapping(int colour_idx, int codestream_component,
                            int palette_component=-1);
    void set_opacity_mapping(int colour_idx, int codestream_component,
                             int palette_component=-1);
    void set_premult_mapping(int colour_idx, int codestream_component,
                             int palette_component=-1);
       /* These functions are used to establish the relationship (if any)
          between the code-stream image components and the functions, colour,
          opacity and pre-multiplied opacity, for each colour channel.
          The `colour_idx' must lie in the range 0 through `num_colours'-1.
          A value of 0 refers to the red or luminance channel, while values
          of 1 and 2 refer to the green and blue channels, respectively.  A
          complete object must have a mapping for every colour, but need not
          necessarily have a mapping for opacity or pre-multiplied opacity
          functions. When first initialized, a `jp2_channels' object has
          mappings only for colour values and these identify the first three
          colour channels with the corresponding first three code-stream
          image components.
             The `codestream_component' argument must identify the image
          component (in the range 0 through `num_components'-1) which is used
          to create the relevant channel values.  The `palette_component'
          should be equal to -1 if the code-stream image component is used
          directly; otherwise, it identifies the zero-based index of the
          component (i.e., the lookup table) in a `jp2_palette' object,
          which operates on the code-stream image component samples.
             It should be noted that a codestream image component which is
          used as an index to a palette lookup table is not allowed to be
          used directly (i.e., it is only allowed to be used to index
          palette lookup tables).  Also, you must not use the same
          combination of source image component and palette lookup table
          (if any) for different types of reproduction functions (colour,
          opacity or pre-multiplied opacity).  You may, however, use the
          same image component or the same combination of image component
          and palette lookup table for multiple colour channels, multiple
          opacity channels or multiple pre-multiplied opacity channels. */
  // --------------------------------------------------------------------------
  public: // Access member functions
    int get_num_colours();
      /* Returns 1 for monochrome imagery and 3 for colour imagery. */
    bool get_colour_mapping(int colour_idx, int &codestream_component,
                            int &palette_component);
    bool get_opacity_mapping(int colour_idx, int &codestream_component,
                             int &palette_component);
    bool get_premult_mapping(int colour_idx, int &codestream_component,
                             int &palette_component);
       /* These functions are used to determine the relationship (if any)
          between the code-stream image components and the colour, opacity
          and pre-multiplied opacity values for each colour channel.
          The `colour_idx' must lie in the range 0 through `num_colours'-1.
          A value of 0 refers to the red or luminance channel, while values
          of 1 and 2 refer to the green and blue channels, respectively.
          Each function returns true if the relevant colour, opacity or
          pre-multpiplied opacity values exist.  It returns false if they do
          not exist.  The first function must return true for all legal
          `colour_idx' values, since every colour channel must at least
          have sample values.
             The `codestream_component' argument is used to return the
          index (starting from 0) of the code-stream image component
          which is used to create the relevant channel values.  The
          `palette_component' value will be set to -1 if the code-stream
          image component samples are to be used directly.  Otherwise, it
          will be set to the index of the component (i.e., the lookup table)
          in a `jp2_palette' object, which operates on the code-stream
          samples to produce the relevant channel values. */
  // --------------------------------------------------------------------------
  private: // Data
    j2_channels *state;
  };

/*****************************************************************************/
/*                            jp2_colour_space                               */
/*****************************************************************************/

enum jp2_colour_space {
    JP2_sLUM_SPACE,   // The standard luminance space (same gamma as sRGB)
    JP2_sRGB_SPACE,   // The standard RGB space
    JP2_sYCC_SPACE,   // YCC derivative of sRGB -- currently an ammendment
    JP2_iccLUM_SPACE, // Monochrome space described by an embedded ICC profile
    JP2_iccRGB_SPACE  /* 3-colour space described by an embedded ICC profile
                         Note that the 3 channels nominally represent some
                         type of RGB space, but just about any 3 component
                         non-opponent, device independent representation may
                         be involved. */
  };

/*****************************************************************************/
/*                               jp2_colour                                  */
/*****************************************************************************/

  /* Note: Objects of this class are merely interfaces to an internal
     implementation object, `j2_colour', which cannot be directly
     created by an application.  Creation and destruction of the internal
     object, as well as saving and reading of JP2 boxes are capabilities
     reserved for the internal machinery. Amongst other things, this
     protects the user against the possibility of losing control of
     resources when error conditions throw exceptions. */

class jp2_colour {
  public: // Lifecycle member functions
    jp2_colour()
      { state = NULL; }
    jp2_colour(j2_colour *state)
      { this->state = state; }
    bool operator!()
      { return (state == NULL); }
    bool exists()
      { return (state != NULL); }
  // --------------------------------------------------------------------------
  public: // Initialization member functions
    void init(jp2_colour_space space);
      /* Initializes the internal colour object with the indicated colour
         space description. It is illegal to call this function unless
         there is an internal object (`exists' must return true) and that
         object has not yet been initialized.  In practice, this happens only
         when setting up a `jp2_target' object to construct a JP2 file.
         Note that the `space' here must be one of JP2_sLUM_SPACE,
         JP2_sRGB_SPACE or JP2_sYCC_SPACE.  See below for spaces defined by
         an ICC profile. */
    void init(kdu_byte *icc_profile);
      /* Same as above, but the colour space is defined by an embedded ICC
         profile, contained in the supplied buffer.  The internal object
         makes a copy of this buffer.  The standard ICC header
         in this buffer identifies its length and other attributes.  The
         profile must identify a matrixed 3-colour input space, defined
         with respect to the XYZ Profile Connection Space (PCS_XYZ).  Apart
         from these requirements, the integrity of the profile may not be
         thoroughly checked. */
    void init(float gamma, float beta=0.0F, int num_points=100);
    void init(float matrix3x3[],
              float gamma, float beta=0.0F, int num_points=100);
      /* Same as above, but an embedded ICC profile is automatically
         generated to describe a luminance space (first form) or
         RGB space (second form) having the indicated gamma function
         and primary matrix (second form).
            The most commonly used form of the gamma function is not
         a pure power lay, but may be described by two parameters,
         gamma and beta, as explained in Chapter 1 of the book by
         Taubman and Marcellin.  For reference, the sRGB space has
         `gamma'=2.4 and `beta'=0.055, while the NTSC RGB space
         has `gamma'=2.2 and `beta'=0.099.  If `beta'=0, the gamma
         function is a pure power law and can be efficiently
         represented by the ICC profile.  Unfortunately, this
         form is not recommended, since inversion is ill conditioned
         and can substantially amplify quantization artifacts.  For
         the more general and more useful case where beta is non-zero,
         the function must be represented through tabulated values.
         These are uniformly spaced in the non-linear (gamma corrected)
         space.  The number of tabulated points may be explicitly
         controlled using the `num_points' argument.  Note that the
         function does not currently accept gamma values less than 1.
            The columns of the `matrix3x3' matrix hold the
         tri-stimulus responses (synthesized to achieve the desired
         appearance under the CIE D50 illuminant) associated with
         the linear (after undoing any gamma correction associated
         with the numerical channel values) colour channels.  The
         first column holds XYZ values for the linearized first (red)
         channel.  The second column holds XYZ values for the
         linearized second (green) channel.   The third column holds
         XYZ values for the linearized third (blue) channel.  The
         first row of the matrix holds X values; the second holds Y
         values; and the third holds Z values. */
  // --------------------------------------------------------------------------
  public: // Access member functions
    int get_num_colours();
      /* Returns 3 for RGB imagery and 1 for monochrome imagery.  Note that
         this is the same value returned by `jp2_channels::get_num_colours'. */
    jp2_colour_space get_space();
      /* If the return value is JP2_iccLUM_SPACE or JP2_iccRGB_SPACE, the
         colour space is defined by an embedded ICC profile; further
         information may be obtained as described below. */
    kdu_byte *get_icc_profile(int *num_bytes=NULL);
      /* Returns a pointer to a block of memory which holds the embedded
         ICC profile.  Returns NULL if the colour specification does not
         involve an embedded ICC profile.  Note that the block of memory
         belongs to the internal object; no attempt should be made to
         delete it or to use it beyond the lifetime of the object.  If
         `num_bytes' is non-NULL, the length of the profile is returned
         in *`num_bytes'. */
    bool get_icc_lut(int channel_idx, float lut[], int index_bits);
    bool get_icc_matrix(float matrix3x3[]);
      /* These functions are used to recover the ICC profile information
         which is actually usable by a conformant JP2 reader.  The standard
         suggests that JP2 readers should ignore everything else.  The
         functions both return false if there is no embedded ICC profile.
            The `get_icc_lut' function fills in the 2^{`index_bits'} entries
         of the supplied `lut' array for the indicated colour channel.  The
         `channel_idx' must be in the range 0 to 2 (0 for monochrome spaces).
         The range of inputs to the lookup table is assumed to be 0 to
         2^{index_bits}-1.  The entries of the lookup table will generally
         be obtained by evaluating a gamma function or interpolating tone
         reproduction data obtained from the embedded ICC profile.  The
         lookup table maps decompressed image component samples (possibly
         subjected to a palette mapping and possibly interpolated) into
         the linear (with respect to scene radiant intensity) values
         associated with the Profile Connection Space (PCS).  The lookup
         table entries are guaranteed to be non-negative numbers lying in
         the range 0 to 1, inclusive.
            For non-monochrome spaces, an additional matrix mapping into
         the D50 XYZ Profile Connection Space (PCS) is generally required.
         The relevant matrix may be obtained by calling the `get_icc_matrix'
         function, which returns false if the embedded ICC profile
         describes a monochrome space.  The `matrix3x3' argument receives
         the elements of a 3x3 matrix, with the first row of the matrix
         appearing first, followed by the second and third rows.  The
         PCS values, are obtained through the matrix-vector multiplication:
         (X Y Z)' = M * (R G B)' where R, G and B are the first, second
         and third colour channels, subjected to the tone reproduction curve
         described above, and the prime notation denotes transposition.
            It is worth noting that the PCS is defined to describe colour
         appearance under a D50 illuminant (graphic arts standard), which
         is different to the D65 illuminant of sRGB (most monitors are
         configured by default to use an even more blue illuminant, typically
         D93, although this is easily changed).  For this reason, an
         illuminant correction is typically required in addition to
         primary conversion and non-linear tone reproduction mapping (gamma)
         in order to recover RGB samples in a defined colour space from the
         PCS values. */
  // --------------------------------------------------------------------------
  public: // Convenient conversion functions
    void convert_icc_to_slum(kdu_line_buf &line, int width=-1);
    void convert_icc_to_srgb(kdu_line_buf &red, kdu_line_buf &green,
                             kdu_line_buf &blue, int width=-1);
      /* Implements any colour conversions required to obtain the relevant
         standard colour space: J2_sLUM_SPACE (first function); or
         J2_sRGB_SPACE (second function).  These functions may be called
         only if the current colour space is J2_iccLUM_SPACE (first function)
         or J2_iccRGB_SPACE (second function).  The functions adjust for
         differences between the illuminant, tone mapping and/or primary
         colorant properties of the actual and the standard colour
         spaces.
            Although `kdu_line_buf' objects can represent sample values in
         four different ways, these functions require the data to have a
         16-bit normalized (fixed point) representation, with a nominal
         dynamic range of -0.5 to 0.5 (offsets will be added and later
         removed, as necessary, to create unsigned data for processing
         by tone reproduction curves.  The least significant KDU_FIX_POINT
         bits of each 16 bit integer are the fraction bits of the fixed
         point representation. The selection of this representation is
         intended to facilitate interaction with the sample data processing
         operations conducted by the objects defined in
         "kdu_sample_processing.h".
            All conversions are performed in-situ, overwriting the original
         contents of the relevant buffer.  If `width' is negative, the
         function processes all samples in the line buffers (they must all
         have the same length in this case).  Otherwise, only the first
         `width' samples of each line buffer are processed. */
  // --------------------------------------------------------------------------
  private: // Data
    j2_colour *state;
  };

/*****************************************************************************/
/*                             jp2_resolution                                */
/*****************************************************************************/

  /* Note: Objects of this class are merely interfaces to an internal
     implementation object, `j2_resolution', which cannot be directly
     created by an application.  Creation and destruction of the internal
     object, as well as saving and reading of JP2 boxes are capabilities
     reserved for the internal machinery. Amongst other things, this
     protects the user against the possibility of losing control of
     resources when error conditions throw exceptions. */

     class jp2_resolution {
  public: // Lifecycle member functions
    jp2_resolution()
      { state = NULL; }
    jp2_resolution(j2_resolution *state)
      { this->state = state; }
    bool operator!()
      { return (state == NULL); }
    bool exists()
      { return (state != NULL); }
  // --------------------------------------------------------------------------
  public: // Initialization member functions
    void init(float aspect_ratio);
      /* Initializes the internal resolution object, setting the aspect
         ratio (vertical grid spacing divided by horizontal grid spacing)
         for display and capture resolutions to the indicated value.
         It is illegal to call this function unless there is an internal
         object (`exists' must return true) and that object has not yet
         been initialized.  In practice, this happens only when setting
         up a `jp2_target' object to construct a JP2 file. */
    void set_different_capture_aspect_ratio(float aspect_ratio);
      /* Sets the capture aspect ratio to be different to the display
         aspect ratio (as set up by `init').  This is rarely required,
         since it makes little sense for the aspect ratios to differ. */
    void set_resolution(float resolution, bool for_display=true);
      /* Sets the vertical resolution in high resolution canvas grid
         points per metre.  If `for_display' is true, this is the default
         display resolution; oherwise, it is the capture resolution.  There
         is no need to explicitly specify either of these resolutions.  If
         no resolution is specified the relevant box will not be written in
         the JP2 file, except when a non-unity aspect ratio is selected.  In
         the latter case, a default display resolution box will be created
         having a default vertical display resolution of 1 grid point per
         metre. */
  // --------------------------------------------------------------------------
  public: // Access member functions
    float get_aspect_ratio(bool for_display=true);
      /* Always returns a positive aspect ratio representing the vertical
         grid spacing divided by the horizontal grid spacing on the
         code-stream's high resolution canvas.  If `for_display' is false,
         the capture aspect ratio is returned instead of the display aspect
         ratio, although these should be identical in any reasonable use of
         the relevant JP2 boxes. */
    float get_resolution(bool for_display=true);
      /* Returns a positive value if and only if explicit resolution
         information is available.  If `for_display' is true, the return
         value identifies the default vertical display resolution
         (in canvas grid points per metre).  Otherwise, the return value is
         the vertical capture resolution (again in canvas grid points per
         metre).  In both cases, the returned values may be multiplied by the
         aspect ratio to recover horizontal resolution. */
  // --------------------------------------------------------------------------
  private: // Data
    j2_resolution *state;
  };

/*****************************************************************************/
/*                               jp2_source                                  */
/*****************************************************************************/

  /* Note: Objects of this class are merely interfaces to an internal
     implementation object, `j2_source'.  To destroy the internal object
     you must invoke the explicit `close' function. */

class jp2_source : public kdu_compressed_source {
  public: // Lifecycle functions
    jp2_source()
      { state = NULL; }
    bool operator!()
      { return (state == NULL); }
    bool exists()
      { return (state != NULL); }
    void open (char *fname);
      /* Generates an error if the file cannot be opened or is found not
         to be compatible with the JP2 format. */
    void close();
      /* Destroys the internal object and resets the state of the interface
         so that `exists' returns false.  It is safe to close an object
         which was never opened. */
  // --------------------------------------------------------------------------
  public: // JP2 specific access functions
    jp2_dimensions access_dimensions();
    jp2_palette access_palette();
    jp2_channels access_channels();
    jp2_colour access_colour();
    jp2_resolution access_resolution();
      /* These functions return objects which may be used to access the
         information carried with the JP2 file.  The information reported
         by each of these objects must be interpreted by conformant JP2
         readers.  What this means is application dependent, but a conformant
         rendering device should be prepared to perform palette translation,
         aspect ratio correction and colour conversion to an appropriate
         space.  The information provided by `access_dimensions' is redundant
         with the code-stream and most applications should ignore it, using
         the `kdu_codestream' object's capabilities to return the relevant
         quantities.  Nevertheless, the information may prove useful if
         the code-stream is not to be opened for some reason. */
  // --------------------------------------------------------------------------
  public: // Function overrides to implement compressed data I/O
    int read(kdu_byte *buf, int num_bytes);
  // --------------------------------------------------------------------------
  private: // Data
    j2_source *state;
  };

/*****************************************************************************/
/*                               jp2_target                                  */
/*****************************************************************************/

  /* Note: Objects of this class are merely interfaces to an internal
     implementation object, `j2_target'.  To destroy the internal object
     you must invoke the explicit `close' function. */

class jp2_target : public kdu_compressed_target {
  public: // Lifecycle functions
    jp2_target()
      { state = NULL; }
    bool operator!()
      { return (state == NULL); }
    bool exists()
      { return (state != NULL); }
    void open (char *fname);
      /* Generates an error if the file cannot be opened. */
    void close();
      /* Destroys the internal object and resets the state of the interface
         so that `exists' returns false.  It is safe to close an object
         which was never opened. */
  // --------------------------------------------------------------------------
  public: // Access to attributes which may need to be set.
    jp2_dimensions access_dimensions(); // You must access and initialize this
    jp2_colour access_colour();         // You must access and initialize this
    jp2_palette access_palette();       // Defaults to no palette
    jp2_channels access_channels();     // Defaults to using initial components
    jp2_resolution access_resolution(); // Defaults to 1:1 aspect ratio
      /* These functions may be used to gain access to and thence initialize
         any or all of the attributes of the JP2 file being constructed.
         Any changes must be effected prior to the first attempt to write
         code-stream data to the object.  The palette, channels and
         resolution information will be automatically assigned default
         parameters if necessary, but the `jp2_dimensions' and `jp2_colour'
         objects must be accessed and explicitly initialized, or else an
         error will be generated when the first attempt is made to write
         code-stream data. */
  // --------------------------------------------------------------------------
  public: // Function overrides to implement compressed data I/O
    bool write(kdu_byte *buf, int num_bytes);
  // --------------------------------------------------------------------------
  private: // Data
    j2_target *state;
  };

#endif // JP2_H
