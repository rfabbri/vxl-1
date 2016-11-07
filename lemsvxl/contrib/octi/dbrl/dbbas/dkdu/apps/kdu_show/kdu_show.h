/******************************************************************************/
// File: kdu_show.h [scope = APPS/SHOW]
// Version: Kakadu, V2.2
// Author: David Taubman
// Last Revised: 20 June, 2001
/******************************************************************************/
// Copyright 2001, David Taubman, The University of New South Wales (UNSW)
// The copyright owner is Unisearch Ltd, Australia (commercial arm of UNSW)
// Neither this copyright statement, nor the licensing details below
// may be removed from this file or dissociated from its contents.
/******************************************************************************/
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
/*******************************************************************************
Description:
   Defines the main application object for the interactive JPEG2000 viewer,
"kdu_show".
*******************************************************************************/

#if !defined(AFX_KDU_SHOW_H__1AD132BD_D9F5_4FC6_8A43_3A7628C2BC92__INCLUDED_)
#define AFX_KDU_SHOW_H__1AD132BD_D9F5_4FC6_8A43_3A7628C2BC92__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "kdu_messaging.h"
#include "kdu_compressed.h"
#include "kdu_file_io.h"
#include "region_decompressor.h"
#include "MainFrm.h"

/////////////////////////////////////////////////////////////////////////////
// CKdu_showApp:
// See kdu_show.cpp for the implementation of this class
//

class CKdu_showApp : public CWinApp
{
public:
  CKdu_showApp();
  ~CKdu_showApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKdu_showApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation

public:
	//{{AFX_MSG(CKdu_showApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileClose();
	afx_msg void OnUpdateFileClose(CCmdUI* pCmdUI);
	afx_msg void OnFileOpen();
	afx_msg void OnViewHflip();
	afx_msg void OnUpdateViewHflip(CCmdUI* pCmdUI);
	afx_msg void OnViewVflip();
	afx_msg void OnUpdateViewVflip(CCmdUI* pCmdUI);
	afx_msg void OnViewRotate();
	afx_msg void OnUpdateViewRotate(CCmdUI* pCmdUI);
	afx_msg void OnViewCounterRotate();
	afx_msg void OnUpdateViewCounterRotate(CCmdUI* pCmdUI);
	afx_msg void OnViewZoomOut();
	afx_msg void OnUpdateViewZoomOut(CCmdUI* pCmdUI);
	afx_msg void OnViewZoomIn();
	afx_msg void OnUpdateViewZoomIn(CCmdUI* pCmdUI);
	afx_msg void OnViewRestore();
	afx_msg void OnUpdateViewRestore(CCmdUI* pCmdUI);
	afx_msg void OnModeFast();
	afx_msg void OnUpdateModeFast(CCmdUI* pCmdUI);
	afx_msg void OnModeFussy();
	afx_msg void OnUpdateModeFussy(CCmdUI* pCmdUI);
	afx_msg void OnModeResilient();
	afx_msg void OnUpdateModeResilient(CCmdUI* pCmdUI);
	afx_msg void OnViewWiden();
	afx_msg void OnUpdateViewWiden(CCmdUI* pCmdUI);
	afx_msg void OnViewShrink();
	afx_msg void OnUpdateViewShrink(CCmdUI* pCmdUI);
	afx_msg void OnFileProperties();
	afx_msg void OnUpdateFileProperties(CCmdUI* pCmdUI);
	afx_msg void OnComponent1();
	afx_msg void OnUpdateComponent1(CCmdUI* pCmdUI);
	afx_msg void OnComponent2();
	afx_msg void OnUpdateComponent2(CCmdUI* pCmdUI);
	afx_msg void OnComponent3();
	afx_msg void OnUpdateComponent3(CCmdUI* pCmdUI);
	afx_msg void OnComponent4();
	afx_msg void OnUpdateComponent4(CCmdUI* pCmdUI);
	afx_msg void OnMultiComponent();
	afx_msg void OnUpdateMultiComponent(CCmdUI* pCmdUI);
	afx_msg void OnLayersLess();
	afx_msg void OnUpdateLayersLess(CCmdUI* pCmdUI);
	afx_msg void OnLayersMore();
	afx_msg void OnUpdateLayersMore(CCmdUI* pCmdUI);
	afx_msg void OnModeResilientSop();
	afx_msg void OnUpdateModeResilientSop(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
// ----------------------------------------------------------------------------
public: // Access methods for use by the window objects
  void set_view_size(kdu_coords size);
    /* This function should be called whenever the client view window's
       dimensions may have changed.  The function should be called at least
       whenever the WM_SIZE message is received by the client view window.
       The current object expects to receive this message before it can begin
       processing a newly opened image.  If `size' defines a larger region
       than the currently loaded image, the actual `view_dims' region used
       for rendering purposes will be smaller than the client view window's
       dimensions. */
  void set_hscroll_pos(int pos, bool relative_to_last=false);
    /* Identifies a new left hand boundary position for the current view.
       If `relative_to_last' is true, the new position is expressed relative
       to the existing left hand view boundary.  Otherwise, the new position
       is expressed relative to the image origin. */
  void set_vscroll_pos(int rel_pos, bool relative_to_last=false);
    /* Same as `set_hscroll_pos', but for vertical scrolling. */
  void paint_region(CDC *dc, kdu_dims region);
    /* Paints a portion of the viewable region to the supplied device
       context.  The location identified by `region.loc' is expressed
       relative to the upper left hand corner of the current view region
       (stored in the private `view_dims' structure).  If the region is
       not completely contained inside the current view region, or no file
       has yet been loaded, the background white colour is painted into the
       entire region first, and then the portion which intersects with the
       current view region is painted.  This allows for the possibility that
       the client view region might be larger than the rendering view region
       (e.g., because the image is too small to occupy the minimum window
       size). */
  void open_file(char *filename);
private: // Private implementation functions
  void find_channel_mapping();
    /* Executed from within `open_file' to build the most appropriate
       association between colour channels and image components, depending
       on the file format and the component sub-sampling factors. */
  void configure_reference();
    /* Determines the reference component (the single image component if there
       is one, or else the component used to build the first colour channel).
       Also sets  the `min_expansion' factors to the minimum acceptable
       expansion factors for the reference component which will allow the
       expansion factors for all components to be integers.  The caller may
       need to modify the current `expansion' factors according to the
       `min_expansion' factors set here.
          You should call this function whenever the channel construction rules
       change -- e.g., opening a new file, or setting or resetting the
       `single_component' mode to display only one component. */
  void close_file();
  void initialize_regions();
    /* Called by `open_file' after successfully opening a new compressed
       image to prepare the image regions for subsequent interaction with
       `set_view_size' and other member functions. */
  void calculate_view_centre();
    /* Call this function to determine the centre of the current view relative
       to the entire image.  The function sets `view_centre_known' to true.
       The view centre coordinates are used by the next `set_view_size'
       call to position the image relative to the view.  The most common use
       of this function is to preserve (or modify) the view centre prior to a
       call to `initialize_regions'.  The `view_centre_known'
       flag is reset whenever the view changes in any way. */
  void update_buffer_pos(kdu_coords new_pos);
    /* Called on occasions when the image region represented by the
       memory buffer is shifted in some direction.  Since the buffer is
       somewhat larger than the viewable region, quite a bit of scrolling
       and window resizing can occur without the need to update the buffer
       position. */
  void update_valid_region();
    /* Call this whenever you do something which changes the `buffer_dims'
       region, to reflect the changes into the `valid_dims' region, which
       must always be a subset of the buffered region. */
  void calculate_max_discard_levels();
    /* Called when the user first tries to reduce the resolution of an
       image.  The function will do nothing if the code-stream is being
       used by a decompression engine. */
  void  display_quality_layer_status();
    /* Displays the number of quality layers used to generate the
       image in the status bar. */
public:
  CMainFrame *frame;
private: // Private state
  bool fast, fussy, resilient, resilient_sop;
  CChildView *child; // Null until the frame is created.
  kdu_simple_file_source file_input; // Created by `open_file'.
  jp2_source jp2_input; // Created by `open_file'.
  kdu_codestream codestream; // Created by `open_file'.
  bool tiles_loaded; // True once we know that all tiles have been loaded.
  bool transpose, vflip, hflip; // Current geometry flags.
  int discard_levels;
  int max_discard_levels; // Negative if not yet known.
  int max_display_layers;
  int total_components; // Total number in code-stream
  int single_component; // Negative when performing a composite rendering.
  kdr_channel_mapping channels; // Composite rendering details.
  int reference_component; // Either the `single_component' or the first channel
  kdu_coords min_expansion; // Min expansion factors for reference component
  kdu_coords expansion; // Expansion factors for reference component
  kdu_dims image_dims;
  kdu_dims buffer_dims;
  kdu_dims view_dims;
  kdu_dims valid_dims;
  float view_centre_x, view_centre_y;
  bool view_centre_known;
  kdu_byte *buffer;
  kdu_coords buffer_extent;
  kdr_region_decompressor decompressor;
  bool in_idle;
  bool processing;
  kdu_dims region_in_process;
  kdu_dims incomplete_region;
};
  /* Notes:
        The `buffer' array alway contains interleaved RGB values, in the
     order BGR.  It is used to build top-down DIB's for painting the display.
     Each row is capable of holding `buffer_extent.x' 3-byte pixels, where
     `buffer_extent.x' is guaranteed to be a multiple of 4.  The buffer is
     capable of holding `buffer_extent.y' rows.  Note that `buffer_extent'
     may be larger than `buffer_dims.size' or even `image_dims.size'.
        The various `kdu_dims' structures all identify the location and
     dimensions of their respective regions, as they appear in the rendering
     coordinate system.  The rendering coordinate system is derived from the
     coordinate system of the `reference_component' as it appears on the
     code-stream canvas, after taking component sub-sampling factors and
     geometric manipulations into account.  In particular, the locations
     occupied by the reference component's samples on the rendering coordinate
     system are obtained by multiplying the code-stream sample coordinates
     by the sampling factors contained in `expansion'.  On the rendering
     grid, the image region, maintained by `image_dims' is obtained by
     multiplying the upper left and lower right hand coordinates of the
     reference component region by the sampling factors in `expansion' and
     subtracting half of `expansion.x' and `expansion.y' from the resulting
     coordinates.  The reason for the expansion, is that we think of each of the
     reference component's samples as occupying `expansion.y' by `expansion.x'
     grid points on the rendering grid.
        The `buffer_dims' structure identifies the location and dimensions of
     the upper left hand portion of the `buffer' which is currently active.
     This is necessarily a subset of the `image_dims' region.  The `view_dims'
     region identifies the location and dimensions of the region represented
     by the current display view (the region painted into the client view
     window).  It is necessarily a subset of the `buffer_dims' region.
     The `valid_dims' structure identifies the location and dimensions of
     currently valid buffered data.  It is necessarily a subset of the
     `buffer_dims' region.
        The `in_idle' flag is asserted whilst decompression is taking place
     in an OnIdle call.  It is used as a precaution to prevent re-entrant
     processing in a recursive call to the OnIdle function, if a message
     processing loop is incarnated indirectly inside the first call.
        If `processing' is true, the `decompressor' object has
     been started and is processing a region of the buffer.  While this is
     going on, `incomplete_region' identifies the portion of the
     `region_in_process' which has not yet been completed.  Although
     `region_in_process' and `incomplete_region' are initially both identical
     to the region supplied to `kdr_region_decompressor::start', they are
     both intersected with the `buffer_dims' region whenever it
     changes in response to user resizing or scrolling commands.  If either
     or both windows become empty in the process, the current region
     processing operation should be finished.  It may also be finished if
     the processing region becomes too small to be worth continuing.  In any
     event, a partially or entirely completed region may be used to update
     `valid_dims' once the processing is finished. */
     
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KDU_SHOW_H__1AD132BD_D9F5_4FC6_8A43_3A7628C2BC92__INCLUDED_)
