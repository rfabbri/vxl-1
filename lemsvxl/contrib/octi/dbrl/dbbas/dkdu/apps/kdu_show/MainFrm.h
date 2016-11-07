/******************************************************************************/
// File: MainFrm.h [scope = APPS/SHOW]
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
   MFC-based class definitions and message mapping macros for the single frame
window in the interactive JPEG2000 image viewer, "kdu_show".
*******************************************************************************/

#if !defined(AFX_MAINFRM_H__F2E6C859_6A9A_4E1D_B7E6_B3196130B999__INCLUDED_)
#define AFX_MAINFRM_H__F2E6C859_6A9A_4E1D_B7E6_B3196130B999__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ChildView.h"
#include "kdu_compressed.h"
class CKdu_showApp; // Forward declaration.

class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
  virtual ~CMainFrame();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // embedded view window
  CStatusBar  m_wndStatusBar;
  CChildView    m_wndView;
private:
  CKdu_showApp *app;
// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
// ----------------------------------------------------------------------------
public: // Access methods for use by the application object
  CChildView *get_child()
    {
      return &m_wndView;
    }
  void set_app(CKdu_showApp *app)
    {
      this->app = app;
    }
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__F2E6C859_6A9A_4E1D_B7E6_B3196130B999__INCLUDED_)
