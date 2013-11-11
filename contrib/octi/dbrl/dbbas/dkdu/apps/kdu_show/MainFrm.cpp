/******************************************************************************/
// File: MainFrm.cpp [scope = APPS/SHOW]
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
   Implementation of a small number of frame window functions for the
interactive JPEG2000 image viewer application, "kdu_show".  Client-area windows
messages are processed by the child view object, implemented in
 "ChildView.cpp".
*******************************************************************************/

#include "stdafx.h"
#include "kdu_show.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_WM_DROPFILES()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

/******************************************************************************/
/*                          CMainFrame::CMainFrame                            */
/******************************************************************************/

CMainFrame::CMainFrame()
{
  app = NULL;
}

/******************************************************************************/
/*                          CMainFrame::~CMainFrame                           */
/******************************************************************************/

CMainFrame::~CMainFrame()
{
}

/******************************************************************************/
/*                           CMainFrame::OnCreate                             */
/******************************************************************************/

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
    return -1;

  // create a view to occupy the client area of the frame
  if (!m_wndView.Create(NULL, NULL,
                        AFX_WS_DEFAULT_VIEW | WS_HSCROLL | WS_VSCROLL,
                        CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
    {
      TRACE0("Failed to create view window\n");
      return -1;
    }

  // create a status bar
  if (!m_wndStatusBar.Create(this) ||
      !m_wndStatusBar.SetIndicators(indicators,
      sizeof(indicators)/sizeof(UINT)))
    {
      TRACE0("Failed to create status bar\n");
      return -1;      // fail to create
    }
  return 0;
}

/******************************************************************************/
/*                        CMainFrame::PreCreateWindow                         */
/******************************************************************************/

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
  if( !CFrameWnd::PreCreateWindow(cs) )
    return FALSE;
  cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
  cs.lpszClass = AfxRegisterWndClass(0);
  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
  CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
  CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

/******************************************************************************/
/*                           CMainFrame::OnSetFocus                           */
/******************************************************************************/

void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
  m_wndView.SetFocus();
}

/******************************************************************************/
/*                            CMainFrame::OnCmdMsg                            */
/******************************************************************************/

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra,
                          AFX_CMDHANDLERINFO* pHandlerInfo)
{
  // let the view have first crack at the command
  if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
    return TRUE;

  // otherwise, do default handling
  return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

/******************************************************************************/
/*                             CMainFrame::OnSize                             */
/******************************************************************************/

void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
  CFrameWnd::OnSize(nType, cx, cy); // Let the framework resize the client first
  if (nType == SIZE_RESTORED)
    m_wndView.check_and_report_size();
}

/******************************************************************************/
/*                           CMainFrame::OnDropFiles                          */
/******************************************************************************/

void CMainFrame::OnDropFiles(HDROP hDropInfo) 
{
  char fname[1024];
  DragQueryFile(hDropInfo,0,fname,1023);
  DragFinish(hDropInfo);
  if (app != NULL)
    app->open_file(fname);
}
