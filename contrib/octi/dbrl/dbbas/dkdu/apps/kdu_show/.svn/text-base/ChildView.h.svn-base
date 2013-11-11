/******************************************************************************/
// File: ChildView.h [scope = APPS/SHOW]
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
   MFC-based class definitions and message mapping macros for the single child
view window in the interactive JPEG2000 image viewer, "kdu_show".
*******************************************************************************/

#if !defined(AFX_CHILDVIEW_H__9C359608_5C42_42FF_BE3C_E011ABAAF4AA__INCLUDED_)
#define AFX_CHILDVIEW_H__9C359608_5C42_42FF_BE3C_E011ABAAF4AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CKdu_showApp; // Forward declaration.

/////////////////////////////////////////////////////////////////////////////
// CChildView window

class CChildView : public CWnd
{
// Construction
public:
  CChildView();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildView)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
  virtual ~CChildView();

	// Generated message map functions
protected:
	//{{AFX_MSG(CChildView)
	afx_msg void OnPaint();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
// ----------------------------------------------------------------------------
public: // Access methods for use by the frame window
  void set_app(CKdu_showApp *app)
    { this->app = app; }
  void set_max_view_size(kdu_coords size);
    /* Called by the application object whenever the image dimensions
       change for one reason or another.  This is used to prevent the
       window from growing larger than the dimensions of the image (unless
       the image is too tiny).  During periods when no image is loaded, the
       maximum view size should be set sufficiently large that it does not
       restrict window resizing. */
  void set_scroll_metrics(kdu_coords step, kdu_coords page, kdu_coords end)
    { /* This function is called by the application to set or adjust the
         interpretation of incremental scrolling commands.  In particular, these
         parameters are adjusted whenever the window size changes.
            The `step' coordinates identify the amount to scroll by when an
         arrow key is used, or the scrolling arrows are clicked.
            The `page' coordinates identify the amount to scroll by when the
         page up/down keys are used or the empty scroll bar region is clicked.
            The `end' coordinates identify the maximum scrolling position. */
      scroll_step = step;
      scroll_page = page;
      scroll_end = end;
    }
  void check_and_report_size();
    /* Called when the window size may have changed or may need to be
       adjusted to conform to maximum dimensions established by the
       image -- e.g., called from inside the frame's OnSize message handler,
       or when the image dimensions may have changed. */
private: // Data
  CKdu_showApp *app;
  kdu_coords max_view_size;
  kdu_coords last_size;
  bool sizing; // Flag to prevent recursive calls to resizing functions.
  kdu_coords scroll_step, scroll_page, scroll_end;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDVIEW_H__9C359608_5C42_42FF_BE3C_E011ABAAF4AA__INCLUDED_)
