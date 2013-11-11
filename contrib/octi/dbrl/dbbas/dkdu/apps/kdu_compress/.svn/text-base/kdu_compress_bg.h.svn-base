#pragma once
#include "compress_local.h"
#include "bbgm_io.h"

class bbgm_io;
class kdc_flow_control;
class kdc_file_binding;
class kdu_roi_image;

class kdc_bg_flow_control {
  /* Each tile-component may be compressed independently and in any
     order; however, the sequence in which tile-component lines are
     decompressed clearly has an impact on the amount of buffering required
     for image file I/O and possibly the amount of code-stream buffering. This
     object is used to stage the tile-component processing steps so that
     the image is compressed from top to bottom in a manner consistent
     with the overall geometry and hence, in most cases, minimum system
     buffering requirements. */
  public: // Member functions
    kdc_bg_flow_control(kdc_file_binding *files, kdu_codestream rv_codestream_,
				kdc_flow_control* param_flow_,
                     int x_tnum, bool allow_shorts,bbgm_io& bgm_reader_,
                     kdu_roi_image *roi_source=NULL);
      /* Constructs a flow control object for one tile.  The caller might
         maintain an array of pointers to such objects, one for each
         horizontally adjacent tile.  The `x_tnum' argument identifies which
         horizontal tile the object is being created to represent, with 0 for
         the first apparent horizontal tile.  The `files' list provides
         references to the `image_in' objects from which image lines will be
         retrieved for compression.  The `files' pointer may be NULL if
         desired,  in which case image samples must be supplied via the
         false, 32-bit data representations will be selected for all
         processing; otherwise, the constructor will select a 16- or a 32-bit
         representation on the basis of the image component bit-depths.
         The `roi_source' argument, if non-NULL, provides access to a
         `kdu_roi_image' interface whose `acquire_node' member function
         is to be used to propagate ROI mask information down the
         tile-component processing hierarchy. */
    ~kdc_bg_flow_control();
    bool advance_components();
      /* Causes the line position for this tile to be advanced in every
         image component, by an amount which should cause at least one
         image line to become active.  Each such active image line is read
         from the relevant image file, unless there are no image files, in
         which case the caller should use the `access_compressor_line' function
         to fill in the contents of the active image lines.  The function
         returns false once all image lines in the current tile have been
         compressed, at which point the user will normally issue an
         `advance_tile' call. */
    kdu_line_buf *access_compressor_line(int comp_idx);
      /* This function may be called after `advance_components' to access any
         newly read image lines prior to compression.  This allows the caller
         to modify the image sample values, which may be essential if there
         are no file reading objects (i.e., a NULL `files' argument was given
         to the constructor).  The function returns NULL if the indicated
         component does not currently have an active line.  The number of
         components with active lines, following a call to
         `advance_components', may be as little as 1. */
    void process_components();
      /* This function must be called after every call to `advance_components'
         which returns true.  It performs any required colour conversions and
         sends the active image lines to the relevant tile-component
         compression engines.  Upon return, the object is prepared to receive
         another call to its `advance_components' member function. */
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

    struct kdc_bg_component_flow_control {
      public: // Data
        bbgm_io* bg_reader;
        kdu_line_buf line;
        int vert_subsampling;
        kdu_push_ifc compressor;
        kdu_sample_allocator allocator;
        bool reversible;
        bool allow_shorts; // True unless 16-bit representations are disallowed
        int ratio_counter; /* Initialized to 0, decremented by `count_delta';
                              when < 0, a new line must be processed, after
                              which it is incremented by `vert_subsampling'. */
        int remaining_lines;
      };

    kdu_codestream rv_codestream,param_codestream;
	kdc_flow_control* param_flow;
	bbgm_io& bgm_reader;
    kdu_dims valid_tile_indices;
    kdu_coords tile_idx;
    int x_tnum; // Starts at 0 for the first tile.
    kdu_tile tile;
    int num_components;
    kdc_bg_component_flow_control *components;
    int count_delta; // Holds the minimum of the `vert_subsampling' fields.
    bool use_ycc;
    kdu_roi_image *roi_image;
  };


bool kdu_compress_bg(const bbgm_image_of<bgm_mix>* from,bbgm_image_of<bgm_mix>* to,
	int num_comps,int nLevels,int argc=0,char** argv= 0);
bool create_size(const bbgm_image_of<bgm_mix>& from,siz_params &siz,int num_comps);

