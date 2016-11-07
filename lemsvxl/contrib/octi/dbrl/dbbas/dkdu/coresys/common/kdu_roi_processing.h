/*****************************************************************************/
// File: kdu_roi_processing.h [scope = CORESYS/COMMON]
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
   Uniform interfaces to ROI information services.
******************************************************************************/

#ifndef KDU_ROI_PROCESSING_H
#define KDU_ROI_PROCESSING_H

#include <assert.h>
#include "kdu_messaging.h"
#include "kdu_compressed.h"

// Defined here:
class kdu_roi_node;
class kdu_roi_level;
class kdu_roi_image;

// Defined elsewhere:
class kd_roi_level;

/*****************************************************************************/
/*                                kdu_roi_node                               */
/*****************************************************************************/

class kdu_roi_node {
  public: // Member functions
    virtual void release() { return; }
      /* When a `kdu_roi_node' object is acquired from another
         object (`kdu_roi_level' or `kdu_roi_image') resources may
         or may not be allocated by the granting object.  For safety,
         always call this function after you are done with the node,
         so that the derived object has an opportunity to release
         any such resources.  The default implementation does
         nothing. */
    virtual void pull(kdu_byte buf[], int width) = 0;
  };
  /* Notes:
        This abstract base class defines an interface to a derived object
     which is able to supply ROI mask information.  The `pull' function
     retrieves a single line of ROI information from some tile-component,
     at some resolution or in some subband.  The scope depends upon the
     object which supplied the relevant interface.  It is illegal to request
     more lines than are available. */

/*****************************************************************************/
/*                               kdu_roi_level                               */
/*****************************************************************************/

class kdu_roi_level {
  public: // Member functions
    kdu_roi_level()
      { state = NULL; }
    bool exists()
      { return (state != NULL); }
    bool operator!()
      { return (state == NULL); }
    void create(kdu_resolution res, kdu_roi_node *source);
      /* Creates the internal object to which this class provides an
         interface.  The `source' object may not be NULL.  Its `release'
         function will automatically be called upon destruction of the
         current object, or prior to that time (if the object is able to
         figure out that no further ROI information is required). */
    void destroy();
      /* Destroys the internal object. */
    kdu_roi_node *acquire_node(int band_idx);
      /* Provides the interface through which each subband's ROI information
         may be accessed.  The node must be released prior to destroying
         the granting roi-level object. */
  private: // Data
    kd_roi_level *state;
  };
  /* Notes:
        This object propagates ROI information from one tile-component
     resolution to its four respective subbands.  The `source' object
     supplied to the `create' function provides ROI information for
     the intermediate LL subband at the input of the relevant DWT stage,
     while `acquire_band' provides `kdu_roi_node' interfaces to ROI
     information corresponding to the derived subbands.
        The object is actually only an interface to an internal object
     whose implementation is not in public view (see "roi.cpp" and
     "roi_local.h"). */

/*****************************************************************************/
/*                               kdu_roi_image                               */
/*****************************************************************************/

class kdu_roi_image {
  public: // Member functions
    virtual ~kdu_roi_image() { return; }
      /* Allows destruction from the abstract base. */
    virtual kdu_roi_node *
      acquire_node(int component, kdu_dims tile_region) = 0;
      /* The tile regions for which this function is called should
         constitute a partition of the image region.  The function may
         return NULL if the entire region or component is to be interpreted
         as part of the foreground.  The caller should remember to release
         the node when done with it. */
  };
  /* Notes:
        This abstract base class is not derived by any class defined by
     the Kakadu core.  To use ROI features, the application must supply
     a suitable derivation.  Some useful examples are provided with the
     "kdu_compress" application.  The derived object might obtain ROI
     information interactively from a user (e.g., in a clinical application).
     However the ROI information is obtained, the derived object must be
     capable of supplying a suitably derived `kdu_roi_node' object capable
     of delivering the ROI information for any given image component and
     any given tile region. */

#endif // KDU_ROI_PROCESSING_H
