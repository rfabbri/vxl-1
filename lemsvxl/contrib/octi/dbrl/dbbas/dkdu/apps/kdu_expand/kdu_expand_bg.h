#pragma once	
// Core includes
#include "kdu_elementary.h"
#include "kdu_compressed.h"
#include "kdu_sample_processing.h"
#include "expand_local.h"
// Application includes
#include "kdu_image.h"

// Defined here:

class kde_file_binding;
class kde_flow_control_bg;



/*****************************************************************************/
/*                             kde_flow_control_bg                              */
/*****************************************************************************/

class kde_flow_control_bg {
  /* Each tile-component may be decompressed independently and in any
     order; however, the sequence in which tile-component lines are
     decompressed clearly has an impact on the amount of buffering required
     for image file I/O and possibly the amount of code-stream buffering. This
     object is used to stage the tile-component processing steps so that
     the image is decompressed from top to bottom in a manner consistent
     with the overall geometry and hence, in most cases, minimum system
     buffering requirements. */
  public: // Member functions
    kde_flow_control_bg(kde_file_binding *files, int num_channels,
                     kdu_codestream codestream,kde_flow_control* param_flow, int x_tnum, bool allow_shorts,
					 bbgm_io& bgm_reader_);
      /* Constructs a flow control object for one tile.  The caller might
         maintain an array of pointers to such objects, one for each
         horizontally adjacent tile.
            The `files' list provides references to the `image_out' objects
         to which the decompressed image lines will be written.  The `files'
         pointer may be NULL if desired, in which case nothing will be written,
         but the decompressed image lines may still be accessed incrementally
         using the `access_decompressed_line' member function.
            The `num_channels' argument identifies the number of decompressed
         output channels to be produced.  If `channels' and `palette' are
         non-existent, there is a 1-1 correspondence between channels and
         image components.  Otherwise, there are at most 3 channels, which
         are interpreted as the colour channels, and the image components may
         be mapped to channels through a palette.
            The `x_tnum' argument identifies which horizontal tile the object
         is being created to represent, with 0 for the first apparent
         horizontal tile.
            If `allow_shorts' is false, 32-bit data representations will be
         selected for all processing; otherwise, the constructor will select
         a 16- or a 32-bit representation on the basis of the image component
         bit-depths.  Note, however, that when decompressing an image
         component which is used to map image samples through a palette, a
         16-bit fixed point representation will be used whenever the
         irreversible processing path is involved.  This avoids unnecessary
         conversion to and from floating point representations.
            The `channels' and `palette' objects should be non-empty (i.e.,
         their `exists' member functions should return true) only if a JP2
         compatible file is being decompressed and the channels are to be
         interpreted as colour channels rather than image components.  In
         this case, the object automatically determines which image
         components are required and applies any palette mapping on the
         fly. */
    ~kde_flow_control_bg();
    bool advance_components();
      /* Causes the line position for this tile to be advanced in every
         image component, by an amount which should cause at least one
         image line to be synthesized in at least one component and will not
         cause any more than one image line to be synthesized in any component.
         Synthesized image lines are colour converted, if necessary, and
         then written out to the relevant image files.   The function returns
         false once all image lines have been synthesized for the current tile,
         at which point the user will normally issue an `advance_tile' call. */
    kdu_line_buf *access_decompressed_line(int channel_idx);
      /* This function may be called after `advance_components' to access any
         newly decompressed image lines.  This allows the caller to modify the
         contents of the decompressed data if desired, or to send it to
         an alternate destination (other than, or in addition to an image
         file writing object).  The function returns NULL if no new data is
         available for the indicated channel (component or mapped colour
         channel). */
    void process_components();
      /* This function must be called after every call to `advance_components'
         which returns true.  It writes the decompressed image lines to the
         file writing objects supplied during construction (unless the
         constructor's `files' argument was NULL) and prepares the object to
         receive another call to its `advance_components' member function. */
    bool advance_tile();
      /* Moves on to the next vertical tile at the same horizontal tile
         position, returning false if all tiles have been processed.  Note
         that the function automatically invokes the existing tile's
         `kdu_tile::close' member function. */
    int get_buffer_memory()
      { /* Returns the maximum number of buffer memory bytes used by the
           current object for sample data processing.  This includes all memory
           used by the DWT implementation and for intermediate buffering of
           subband samples between the DWT and the block coding system.  The
           maximum is taken over all tiles which this object has been used
           to process. */
        int bytes = 0;
        for (int c=0; c < num_components; c++)
          bytes += components[c].allocator.get_size();
        return bytes;
      }
  private: // Data

    struct kde_component_flow_control_bg {
        bool reversible;
        bool is_signed; // True if original data had a signed representation
        int bit_depth; // Bit-depth of original data.
        bool mapped_by_channel; // True if needed by a channel
        int palette_bits; // Num bits in palette index.  0 if no palette used.
        kdu_tile_comp tc;
        kdu_resolution res;
		bbgm_io reader;
        int width;
        int vert_subsampling;
        int ratio_counter; /* Initialized to 0, decremented by `count_delta';
                              when < 0, a new line must be processed, after
                              which it is incremented by `vert_subsampling'. */
        int remaining_lines;
        bool allow_shorts; // True unless 16-bit representations are disallowed
        kdu_line_buf line;
        kdu_pull_ifc decompressor; // Non-existent for unused components.
        kdu_sample_allocator allocator; // Used only if the component is used.
      };

    struct kde_channel_bg {
		kde_component_flow_control_bg *source_component;
        kdu_sample16 *lut; // NULL unless a palette mapping is used.
        kdu_line_buf line; // This will only ever be 16-bit fixed point samples
        kdu_sample_allocator *allocator; // Points to the one owned by source
        int width;
        bbgm_io* writer;
      };
      /* Notes:
            When a palette is used, the `lut' array is non-NULL and points
         to an array with 2^{palette_bits} entries, where `palette_bits' is
         found in the `source_component' object referenced from this channel.
         The actual number of palette entries need not be a power of 2, in
         which case some number of final `lut' entries will all hold the same
         value.  When palette mapping is involved, the `source_component'
         must use either a 16- or 32-bit reversible representation or a
         16-bit fixed point representation.  Floating point representations
         are not selected.  The palette output is always 16-bit fixed point
         data.  This simplifies the palette mapping process somewhat. */

    kdu_codestream codestream;
    kdu_dims valid_tile_indices;
    kdu_coords tile_idx;
    int x_tnum; // Starts from 0 for the first valid tile.
    kdu_tile tile;
    int num_components, num_channels;
    kde_component_flow_control_bg *components;
	kde_flow_control* param_flow;
	bbgm_io& bgm_interface;
    kde_channel_bg *channels;
    int count_delta; // Holds the minimum of the `vert_subsampling' fields.
    bool use_ycc;
  };
  bool kde_expand_bg(const bbgm_image_of<bgm_mix>* from,bbgm_image_of<bgm_mix>* to,
	  int argc, char* argv[],kdu_dims* dims=0);
  bool kde_decompress_vil_image(vil_image_view<vxl_byte>& to,
	  int argc, char* argv[],kdu_dims* region_dims=0);
  bool copy_sz_from_input(siz_params* rv_sz,siz_params* param_sz);
