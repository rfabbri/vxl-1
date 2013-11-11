/******************************************************************************/
// File: kdu_show.cpp [scope = APPS/SHOW]
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
   Implements the application object of the interactive JPEG2000 viewer,
"kdu_show".  Menu processing and idle-time decompressor processing are all
controlled from here.  The "kdu_show" application demonstrates some of the
support offered by the Kakadu framework for interactive applications,
including persistence and incremental region-based decompression.
*******************************************************************************/

#include <crtdbg.h>

#include "stdafx.h"
#include <math.h>
#include <strstream>
#include "kdu_show.h"
#include "MainFrm.h"
#include "kdu_messaging.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/* ========================================================================== */
/*                          Process (Application) Globals                     */
/* ========================================================================== */

CKdu_showApp theApp; // There can only be one application object.
std::ostrstream err_stream;  // Object collects text of Kakadu error messages
std::ostrstream warn_stream; // Object collects text of Kakadu warning messages

/* ========================================================================== */
/*                            Internal Functions                              */
/* ========================================================================== */

/******************************************************************************/
/* STATIC                       combine_regions                               */
/******************************************************************************/

static kdu_dims
  combine_regions(kdu_dims r1, kdu_dims r2)
  /* Returns the largest rectangular region which is completely contained
     in the union of regions `r1' and `r2'. */
{
  if (!r1)
    return r2;
  if (!r2)
    return r1;

  // First try joining the regions together.
  kdu_coords joined_min = r1.pos;
  kdu_coords joined_lim = r1.pos + r1.size;
  kdu_coords r2_min = r2.pos;
  kdu_coords r2_lim = r2.pos + r2.size;
  if (joined_lim.x == r2_min.x)
    { // Tack r2 onto the right
      joined_lim.x = r2_lim.x;
      if (r2_min.y > joined_min.y)
        joined_min.y = r2_min.y;
      if (r2_lim.y < joined_lim.y)
        joined_lim.y = r2_lim.y;
      if (joined_lim.y < joined_min.y)
        joined_lim.y = joined_min.y; // No area.
    }
  else if (joined_min.x == r2_lim.x)
    { // Tack r2 onto the left
      joined_min.x = r2_min.x;
      if (r2_min.y > joined_min.y)
        joined_min.y = r2_min.y;
      if (r2_lim.y < joined_lim.y)
        joined_lim.y = r2_lim.y;
      if (joined_lim.y < joined_min.y)
        joined_lim.y = joined_min.y; // No area.
    }
  else if (joined_lim.y == r2_min.y)
    { // Tack r2 onto the bottom
      joined_lim.y = r2_lim.y;
      if (r2_min.x > joined_min.x)
        joined_min.x = r2_min.x;
      if (r2_lim.x < joined_lim.x)
        joined_lim.x = r2_lim.x;
      if (joined_lim.x < joined_min.x)
        joined_lim.x = joined_min.x; // No area.
    }
  else if (joined_min.y == r2_lim.y)
    { // Tack r2 onto the top
      joined_min.y = r2_min.y;
      if (r2_min.x > joined_min.x)
        joined_min.x = r2_min.x;
      if (r2_lim.x < joined_lim.x)
        joined_lim.x = r2_lim.x;
      if (joined_lim.x < joined_min.x)
        joined_lim.x = joined_min.x; // No area.
    }
  
  // Now take the larger of `joined', `r1' and `r2'
  kdu_dims result;
  result.pos = joined_min;
  result.size = joined_lim-joined_min;
  if (r1.area() > result.area())
    result = r1;
  if (r2.area() > result.area())
    result = r2;
  return result;
}

/* ========================================================================== */
/*                     Error and Warning Message Handlers                     */
/* ========================================================================== */

/******************************************************************************/
/* CLASS                         core_messages_dlg                            */
/******************************************************************************/

class core_messages_dlg : public CDialog
{
  public:
    core_messages_dlg(char *string, CWnd* pParent = NULL)
      : CDialog(core_messages_dlg::IDD, pParent)
      {
        this->string = string;
      }
// Dialog Data
	//{{AFX_DATA(core_messages_dlg)
	enum { IDD = IDD_CORE_MESSAGES };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(core_messages_dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
private:
  CStatic *get_static()
    {
      return (CStatic *) GetDlgItem(IDC_MESSAGE);
    }
private:
  char *string;
protected:
	// Generated message map functions
	//{{AFX_MSG(core_messages_dlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/******************************************************************************/
/*                      core_messages_dlg::DoDataExchange                     */
/******************************************************************************/

void
  core_messages_dlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(core_messages_dlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(core_messages_dlg, CDialog)
	//{{AFX_MSG_MAP(core_messages_dlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/******************************************************************************/
/*                      core_messages_dlg::OnInitDialog                       */
/******************************************************************************/

BOOL core_messages_dlg::OnInitDialog() 
{
  CDialog::OnInitDialog();

  while (*string == '\n')
    string++; // skip leading empty lines, if any.
  get_static()->SetWindowText(string);

  // Find the height of the displayed text
  int text_height = 0;
  SIZE text_size;
  CDC *dc = get_static()->GetDC();
  while (*string != '\0')
    {
      char *sp = strchr(string,'\n');
      if (sp == NULL)
        sp = string + strlen(string);
      if (string != sp)
        text_size = dc->GetTextExtent(string,sp-string);
      text_height += text_size.cy;
      string = (*sp != '\0')?sp+1:sp;
    }
  get_static()->ReleaseDC(dc);

  // Resize windows to fit the text height

  WINDOWPLACEMENT dialog_placement, static_placement;
  GetWindowPlacement(&dialog_placement);
  get_static()->GetWindowPlacement(&static_placement);
  int dialog_width = dialog_placement.rcNormalPosition.right -
    dialog_placement.rcNormalPosition.left;
  int static_width = static_placement.rcNormalPosition.right -
    static_placement.rcNormalPosition.left;
  int dialog_height = dialog_placement.rcNormalPosition.bottom -
    dialog_placement.rcNormalPosition.top;
  int static_height = static_placement.rcNormalPosition.bottom -
    static_placement.rcNormalPosition.top;

  get_static()->SetWindowPos(NULL,0,0,static_width,text_height,
                             SWP_NOZORDER | SWP_NOMOVE | SWP_SHOWWINDOW);
  SetWindowPos(NULL,0,0,dialog_width,text_height+8+dialog_height-static_height,
               SWP_NOZORDER | SWP_NOMOVE | SWP_SHOWWINDOW);  
  return TRUE;
}

/******************************************************************************/
/* STATIC                     kdu_error_callback                              */
/******************************************************************************/

static void
  kdu_error_callback()
{
  err_stream.put('\0'); // Make sure the buffer is long enough to hold NULL.
  char *string = err_stream.str();
  core_messages_dlg messages(string,theApp.frame);
  messages.DoModal();
  err_stream.rdbuf()->freeze(0);
  err_stream.seekp(0); // Reset the stream in preparation for further messages
  throw ((int) 0); // Error message generates integer exception.
}

/******************************************************************************/
/* STATIC                    kdu_warning_callback                             */
/******************************************************************************/

static void
  kdu_warning_callback()
{
  warn_stream.put('\0'); // Make sure the buffer is long enough to hold NULL.
  char *string = warn_stream.str();
  core_messages_dlg messages(string,theApp.frame);
  messages.DoModal();
  warn_stream.rdbuf()->freeze(0);
  warn_stream.seekp(0); // Reset the stream in preparation for further messages
}


/* ========================================================================== */
/*                               CAboutDlg Class                              */
/* ========================================================================== */

class CAboutDlg : public CDialog
{
public:
  CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/******************************************************************************/
/*                             CAboutDlg::CAboutDlg                           */
/******************************************************************************/

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
  //{{AFX_DATA_INIT(CAboutDlg)
  //}}AFX_DATA_INIT
}

/******************************************************************************/
/*                           CAboutDlg::DoDataExchange                        */
/******************************************************************************/

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CAboutDlg)
  //}}AFX_DATA_MAP
}

/* ========================================================================== */
/*                              CPropertyHelpDlg                              */
/* ========================================================================== */

class CPropertyHelpDlg : public CDialog
{
// Construction
public:
	CPropertyHelpDlg(char *string, kdu_coords placement,
                         CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CPropertyHelpDlg)
	enum { IDD = IDD_PROPERTYHELP };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyHelpDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
private:
  CStatic *get_static()
    {
      return (CStatic *) GetDlgItem(IDC_MESSAGE);
    }
private:
  char *string;
  kdu_coords placement;
protected:

	// Generated message map functions
	//{{AFX_MSG(CPropertyHelpDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/******************************************************************************/
/*                     CPropertyHelpDlg::CPropertyHelpDlg                     */
/******************************************************************************/

CPropertyHelpDlg::CPropertyHelpDlg(char *string, kdu_coords placement,
                                   CWnd* pParent)
	: CDialog(CPropertyHelpDlg::IDD, pParent)
{
  this->string = string;
  this->placement = placement;
}

/******************************************************************************/
/*                       CPropertyHelpDlg::OnInitDialog                       */
/******************************************************************************/

BOOL CPropertyHelpDlg::OnInitDialog() 
{
  CDialog::OnInitDialog();
  while (*string == '\n')
    string++; // Skip leading empty lines, if any
  get_static()->SetWindowText(string);

  // Move dialog window to desired position
  SetWindowPos(NULL,placement.x,placement.y,0,0,
               SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);

  // Find the height of the displayed text
  int text_height = 0;
  SIZE text_size;
  CDC *dc = get_static()->GetDC();
  while (*string != '\0')
    {
      char *sp = strchr(string,'\n');
      if (sp == NULL)
        sp = string + strlen(string);
      if (string != sp)
        text_size = dc->GetTextExtent(string,sp-string);
      text_height += text_size.cy;
      string = (*sp != '\0')?sp+1:sp;
    }
  get_static()->ReleaseDC(dc);

  // Resize windows to fit the text height

  WINDOWPLACEMENT dialog_placement, static_placement;
  GetWindowPlacement(&dialog_placement);
  get_static()->GetWindowPlacement(&static_placement);
  int dialog_width = dialog_placement.rcNormalPosition.right -
    dialog_placement.rcNormalPosition.left;
  int static_width = static_placement.rcNormalPosition.right -
    static_placement.rcNormalPosition.left;
  int dialog_height = dialog_placement.rcNormalPosition.bottom -
    dialog_placement.rcNormalPosition.top;
  int static_height = static_placement.rcNormalPosition.bottom -
    static_placement.rcNormalPosition.top;

  get_static()->SetWindowPos(NULL,0,0,static_width,text_height+8,
                             SWP_NOZORDER | SWP_NOMOVE | SWP_SHOWWINDOW);
  SetWindowPos(NULL,0,0,dialog_width,text_height+8+dialog_height-static_height,
               SWP_NOZORDER | SWP_NOMOVE | SWP_SHOWWINDOW);

  return TRUE;  // return TRUE unless you set the focus to a control
}

/******************************************************************************/
/*                      CPropertyHelpDlg::DoDataExchange                      */
/******************************************************************************/

void CPropertyHelpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyHelpDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPropertyHelpDlg, CDialog)
	//{{AFX_MSG_MAP(CPropertyHelpDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/* ========================================================================== */
/*                               CPropertiesDlg                               */
/* ========================================================================== */

class CPropertiesDlg : public CDialog
{
// Construction
public:
  CPropertiesDlg(kdu_codestream codestream, std::ostrstream *string_buf,
                 CWnd* pParent=NULL);

// Dialog Data
	//{{AFX_DATA(CPropertiesDlg)
	enum { IDD = IDD_PROPERTIESBOX };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertiesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropertiesDlg)
	afx_msg void OnDblclkPropertiesList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
// ----------------------------------------------------------------------------
protected:
  BOOL OnInitDialog();
private:
  CListBox *get_list()
    {
      return (CListBox *) GetDlgItem(IDC_PROPERTIES_LIST);
    }
private:
  kdu_codestream codestream;
  std::ostrstream *string_buf;
};

BEGIN_MESSAGE_MAP(CPropertiesDlg, CDialog)
	//{{AFX_MSG_MAP(CPropertiesDlg)
	ON_LBN_DBLCLK(IDC_PROPERTIES_LIST, OnDblclkPropertiesList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/******************************************************************************/
/*                        CPropertiesDlg::CPropertiesDlg                      */
/******************************************************************************/

CPropertiesDlg::CPropertiesDlg(kdu_codestream codestream,
                               std::ostrstream *string_buf, CWnd* pParent)
	: CDialog(CPropertiesDlg::IDD, pParent)
{
  //{{AFX_DATA_INIT(CPropertiesDlg)
		// NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
  this->codestream = codestream;
  this->string_buf = string_buf;
}

/******************************************************************************/
/*                        CPropertiesDlg::DoDataExchange                      */
/******************************************************************************/

void CPropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CPropertiesDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}

/******************************************************************************/
/*                         CPropertiesDlg::OnInitDialog                       */
/******************************************************************************/

BOOL CPropertiesDlg::OnInitDialog()
{
  CDialog::OnInitDialog();
  char *string = string_buf->str();
  int count = string_buf->pcount();
  int max_width = 0;;
  char tbuf[256];
  while (count > 0)
    {
      char *tp = tbuf;
      SIZE text_size;
      for (; (count > 0) && (*string != '\n'); count--, string++)
        if ((tp-tbuf) < 255)
          *(tp++) = *string;
      *tp = '\0';
      get_list()->AddString(tbuf);
      CDC *dc = get_list()->GetDC();
      GetTextExtentPoint32(dc->m_hDC,tbuf,tp-tbuf,&text_size);
      get_list()->ReleaseDC(dc);
      if (text_size.cx > max_width)
        max_width = text_size.cx;
      string++; count--; // skip over the newline character.
    }
  string_buf->rdbuf()->freeze(0);
  get_list()->SetCurSel(-1);
  get_list()->SetHorizontalExtent(max_width+10);
  return TRUE;
}

/******************************************************************************/
/*                   CPropertiesDlg::OnDblclkPropertiesList                   */
/******************************************************************************/

void CPropertiesDlg::OnDblclkPropertiesList() 
{
  int selection = get_list()->GetCurSel();
  int length = get_list()->GetTextLen(selection);
  char *string = new char[length+1];
  get_list()->GetText(selection,string);
  const char *attribute_id;
  kdu_params *obj = codestream.access_siz()->find_string(string,attribute_id);
  delete[] string;
  if (obj != NULL)
    {
      std::ostrstream string_buf;
      kdu_pretty_buf pretty(&string_buf,60);
      std::ostream redirected(&pretty);

      obj->describe_string(attribute_id,redirected,true);
      redirected.flush();
      char *buf = string_buf.str();
      int buf_len = string_buf.pcount();
      if (buf_len > 0)
        buf[buf_len-1] = '\0';
      POINT point;
      GetCursorPos(&point);
      CPropertyHelpDlg help(buf,kdu_coords(point.x,point.y),this);
      help.DoModal();
      string_buf.rdbuf()->freeze(0);
    }
}


/* ========================================================================== */
/*                       CKdu_showApp Class Implementation                    */
/* ========================================================================== */

BEGIN_MESSAGE_MAP(CKdu_showApp, CWinApp)
	//{{AFX_MSG_MAP(CKdu_showApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
	ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE, OnUpdateFileClose)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_VIEW_HFLIP, OnViewHflip)
	ON_UPDATE_COMMAND_UI(ID_VIEW_HFLIP, OnUpdateViewHflip)
	ON_COMMAND(ID_VIEW_VFLIP, OnViewVflip)
	ON_UPDATE_COMMAND_UI(ID_VIEW_VFLIP, OnUpdateViewVflip)
	ON_COMMAND(ID_VIEW_ROTATE, OnViewRotate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ROTATE, OnUpdateViewRotate)
	ON_COMMAND(ID_VIEW_COUNTER_ROTATE, OnViewCounterRotate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_COUNTER_ROTATE, OnUpdateViewCounterRotate)
	ON_COMMAND(ID_VIEW_ZOOM_OUT, OnViewZoomOut)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_OUT, OnUpdateViewZoomOut)
	ON_COMMAND(ID_VIEW_ZOOM_IN, OnViewZoomIn)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_IN, OnUpdateViewZoomIn)
	ON_COMMAND(ID_VIEW_RESTORE, OnViewRestore)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RESTORE, OnUpdateViewRestore)
	ON_COMMAND(ID_MODE_FAST, OnModeFast)
	ON_UPDATE_COMMAND_UI(ID_MODE_FAST, OnUpdateModeFast)
	ON_COMMAND(ID_MODE_FUSSY, OnModeFussy)
	ON_UPDATE_COMMAND_UI(ID_MODE_FUSSY, OnUpdateModeFussy)
	ON_COMMAND(ID_MODE_RESILIENT, OnModeResilient)
	ON_UPDATE_COMMAND_UI(ID_MODE_RESILIENT, OnUpdateModeResilient)
	ON_COMMAND(ID_VIEW_WIDEN, OnViewWiden)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WIDEN, OnUpdateViewWiden)
	ON_COMMAND(ID_VIEW_SHRINK, OnViewShrink)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHRINK, OnUpdateViewShrink)
	ON_COMMAND(ID_FILE_PROPERTIES, OnFileProperties)
	ON_UPDATE_COMMAND_UI(ID_FILE_PROPERTIES, OnUpdateFileProperties)
	ON_COMMAND(ID_COMPONENT1, OnComponent1)
	ON_UPDATE_COMMAND_UI(ID_COMPONENT1, OnUpdateComponent1)
	ON_COMMAND(ID_COMPONENT2, OnComponent2)
	ON_UPDATE_COMMAND_UI(ID_COMPONENT2, OnUpdateComponent2)
	ON_COMMAND(ID_COMPONENT3, OnComponent3)
	ON_UPDATE_COMMAND_UI(ID_COMPONENT3, OnUpdateComponent3)
	ON_COMMAND(ID_COMPONENT4, OnComponent4)
	ON_UPDATE_COMMAND_UI(ID_COMPONENT4, OnUpdateComponent4)
	ON_COMMAND(ID_MULTI_COMPONENT, OnMultiComponent)
	ON_UPDATE_COMMAND_UI(ID_MULTI_COMPONENT, OnUpdateMultiComponent)
	ON_COMMAND(ID_LAYERS_LESS, OnLayersLess)
	ON_UPDATE_COMMAND_UI(ID_LAYERS_LESS, OnUpdateLayersLess)
	ON_COMMAND(ID_LAYERS_MORE, OnLayersMore)
	ON_UPDATE_COMMAND_UI(ID_LAYERS_MORE, OnUpdateLayersMore)
	ON_COMMAND(ID_MODE_RESILIENT_SOP, OnModeResilientSop)
	ON_UPDATE_COMMAND_UI(ID_MODE_RESILIENT_SOP, OnUpdateModeResilientSop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/******************************************************************************/
/*                         CKdu_showApp::CKdu_showApp                         */
/******************************************************************************/

CKdu_showApp::CKdu_showApp()
{
  _CrtSetBreakAlloc(27);

  fast = true; fussy = resilient = resilient_sop = false;
  child = NULL;
  tiles_loaded = false;
  discard_levels = 0;
  max_discard_levels = -1;
  total_components = single_component = reference_component = 0;
  expansion = min_expansion = kdu_coords(0,0);
  image_dims.pos = image_dims.size = kdu_coords(0,0);
  view_dims = buffer_dims = valid_dims = image_dims;
  view_centre_known = false;
  buffer = NULL;
  buffer_extent = kdu_coords(0,0);
  in_idle = false;
  processing = false;
}

/******************************************************************************/
/*                        CKdu_showApp::~CKdu_showApp                         */
/******************************************************************************/

CKdu_showApp::~CKdu_showApp()
{
  if (codestream.exists())
    codestream.destroy();
  file_input.close();
  jp2_input.close();
  if (buffer != NULL)
    delete[] buffer;
}

/******************************************************************************/
/*                         CKdu_showApp::InitInstance                         */
/******************************************************************************/

BOOL CKdu_showApp::InitInstance()
{
  AfxEnableControlContainer();
#ifdef _AFXDLL
  Enable3dControls(); // Call this when using MFC in a shared DLL
#else
  Enable3dControlsStatic(); // Call this when linking to MFC statically
#endif
  SetRegistryKey(_T("Kakadu"));
  LoadStdProfileSettings(4);
  frame = new CMainFrame;
  m_pMainWnd = frame;
  child = frame->get_child();
  child->set_app(this);
  frame->LoadFrame(IDR_MAINFRAME,
                   WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU |
                   WS_THICKFRAME | WS_MINIMIZEBOX,
                   NULL,NULL);
  frame->ShowWindow(SW_SHOW);
  frame->UpdateWindow();
  frame->set_app(this);
  frame->DragAcceptFiles(TRUE);

  kdu_customize_errors(&err_stream,50,kdu_error_callback);
  kdu_customize_warnings(&warn_stream,50,kdu_warning_callback);

  char *filename = m_lpCmdLine;
  while ((*filename != '\0') && ((*filename == ' ') || (*filename == '\t')))
    filename++;
  if (*filename != '\0')
    {
      if (*filename == '\"')
        {
          char *ch = strrchr(filename+1,'\"');
          if (ch != NULL)
            {
              filename++;
              *ch = '\0';
            }
        }
      open_file(filename);
    }
  return TRUE;
}

/******************************************************************************/
/*                          CKdu_showApp::open_file                           */
/******************************************************************************/

void
  CKdu_showApp::open_file(char *filename)
{
  close_file();
  try {
      kdu_compressed_source *input;
      char *suffix = strrchr(filename,'.');
      if ((suffix != NULL) && ((suffix[1] == 'j') || (suffix[1] == 'J')) &&
          ((suffix[2] == 'p') || (suffix[2] == 'P')) &&
          ((suffix[3] == '2') || (suffix[3] == 'x') || (suffix[3] == 'X')))
        { jp2_input.open(filename); input = &jp2_input; }
      else
        { file_input.open(filename); input = &file_input; }
      codestream.create(input);
      tiles_loaded = false;
      total_components = codestream.get_num_components();
      transpose = vflip = hflip = false;
      discard_levels = 0;
      max_discard_levels = -1; // We don't yet know how many levels there are.
      max_display_layers = 1<<16;
      find_channel_mapping();
      single_component = -1;
      configure_reference();
      expansion = min_expansion;
      codestream.set_persistent();
      if (fast)
        codestream.set_fast();
      if (resilient || resilient_sop)
        codestream.set_resilient(resilient_sop);
      if (fussy)
        codestream.set_fussy();
      initialize_regions();
      char title[256];
      strcpy(title,"Kakadu show: ");
      char *delim = strrchr(filename,'\\');
      if ((delim == NULL) || (delim[1] == '\0'))
        delim = filename-1;
      strncat(title,delim+1,255-strlen(title));
      m_pMainWnd->SetWindowText(title);
    }
  catch (int)
    {
      close_file();
    }
}

/******************************************************************************/
/*                      CKdu_showApp::find_channel_mapping                    */
/******************************************************************************/

void
  CKdu_showApp::find_channel_mapping()
{
  int c;

  channels.clear();
  if (jp2_input.exists())
    { // JP2 file should guide the channel mapping
      channels.colour = jp2_input.access_colour();
      jp2_channels chnl = jp2_input.access_channels();
      jp2_palette palette = jp2_input.access_palette();
      channels.num_channels = chnl.get_num_colours();
      for (c=0; c < channels.num_channels; c++)
        {
          int plt_comp;
          chnl.get_colour_mapping(c,channels.source_components[c],plt_comp);
          if (plt_comp >= 0)
            { // Set up palette lookup table
              int i, num_entries = palette.get_num_entries();
              assert(num_entries <= 1024);
              int palette_bits = 1;
              while ((1<<palette_bits) < num_entries)
                palette_bits++;
              channels.palette_bits = palette_bits;
              channels.palette[c] = new kdu_sample16[1<<palette_bits];
              palette.get_lut(plt_comp,channels.palette[c]);
              for (i=num_entries; i < (1<<palette_bits); i++)
                (channels.palette[c])[i] = (channels.palette[c])[num_entries-1];
            }
        }
    }
  else
    { // Raw code-stream input.
      channels.num_channels = (total_components >= 3)?3:1;
      kdu_coords ref_subs; codestream.get_subsampling(0,ref_subs);
      for (c=0; c < channels.num_channels; c++)
        {
          channels.source_components[c] = c;
          kdu_coords subs; codestream.get_subsampling(c,subs);
          if (subs != ref_subs)
            break;
        }
      if (c < channels.num_channels)
        channels.num_channels = 1;
    }
}

/******************************************************************************/
/*                      CKdu_showApp::configure_reference                     */
/******************************************************************************/

void
  CKdu_showApp::configure_reference()
{
  reference_component = single_component;
  min_expansion = kdu_coords(1,1);
  if (reference_component >= 0)
    return;
  reference_component = channels.source_components[0];
  kdu_coords ref_subs;
  codestream.get_subsampling(reference_component,ref_subs);
  kdu_coords min_subs = ref_subs;
  int c;
  for (c=0; c < channels.num_channels; c++)
    {
      kdu_coords subs;
      codestream.get_subsampling(channels.source_components[c],subs);
      if (subs.x < min_subs.x)
        min_subs.x = subs.x;
      if (subs.y < min_subs.y)
        min_subs.y = subs.y;
    }
  min_expansion.x = ref_subs.x / min_subs.x;
  min_expansion.y = ref_subs.y / min_subs.y;
  for (c=0; c < channels.num_channels; c++)
    {
      kdu_coords subs;
      codestream.get_subsampling(channels.source_components[c],subs);
      if ((((subs.x * min_expansion.x) % ref_subs.x) != 0) ||
          (((subs.y * min_expansion.y) % ref_subs.y) != 0))
        {
          kdu_warning w; w << "The supplied JP2 file contains colour channels "
          "whose sub-sampling factors are not integer multiples of one "
          "another.  Although this is legal, it makes rendering the image "
          "impossible without non-integer interpolation of one or more "
          "components.  It is highly inadvisable to construct compressed "
          "representations of this form, since few applications are likely "
          "to support them.  The current application will proceed to display "
          "only the first component, by default.";
          channels.num_channels = 1;
          channels.colour = jp2_colour(NULL);
          min_expansion = kdu_coords(1,1);
        }
    }
}

/******************************************************************************/
/*                          CKdu_showApp::close_file                          */
/******************************************************************************/

void
  CKdu_showApp::close_file()
{
  if (processing)
    {
      processing = false;
      decompressor.finish();
    }
  if (codestream.exists())
    codestream.destroy();
  file_input.close();
  jp2_input.close();
  if (buffer != NULL)
    delete[] buffer;
  buffer = NULL;
  buffer_extent = kdu_coords(0,0);
  channels.clear();
  expansion = min_expansion = kdu_coords(0,0);
  image_dims.pos = image_dims.size = kdu_coords(0,0);
  view_dims = buffer_dims = image_dims;
  if (child != NULL)
    {
      child->set_max_view_size(kdu_coords(20000,20000));
      if (child->GetSafeHwnd() != NULL)
        child->Invalidate();
    }
  view_centre_known = false;
}

/******************************************************************************/
/*                 CKdu_showApp::calculate_max_discard_levels                 */
/******************************************************************************/

void
  CKdu_showApp::calculate_max_discard_levels()
{
   if (processing)
    {
      processing = false;
      if (!decompressor.finish())
        {
          close_file();
          return;
        }
    }

  try { // We may be parsing the code-stream for the first time
      max_discard_levels = -1;
      codestream.apply_input_restrictions(0,0,0,0,NULL);
      kdu_dims valid_tiles; codestream.get_valid_tiles(valid_tiles);
      kdu_coords idx;
      for (idx.y=0; idx.y < valid_tiles.size.y; idx.y++)
        for (idx.x=0; idx.x < valid_tiles.size.x; idx.x++)
          {
            kdu_tile tile = codestream.open_tile(valid_tiles.pos+idx);
            for (int c=0; c < total_components; c++)
              {
                kdu_tile_comp tc = tile.access_component(c);
                int num_levels = tc.get_num_resolutions()-1;
                if ((max_discard_levels < 0) ||
                    (num_levels < max_discard_levels))
                  max_discard_levels = num_levels;
              }
            tile.close();
          }
      tiles_loaded = true;
    }
  catch (int)
    {
      close_file();
    }
}

/******************************************************************************/
/*                      CKdu_showApp::initialize_regions                      */
/******************************************************************************/

void
  CKdu_showApp::initialize_regions()
{
  // First make sure that there is no processing going on.
  if (processing)
    {
      processing = false;
      if (!decompressor.finish())
        {
          close_file();
          return;
        }
    }

  // Next destroy any existing buffering.
  if (buffer != NULL)
    delete[] buffer;
  buffer = NULL;
  buffer_extent = kdu_coords(0,0);

  // Now find the image region by expanding the reference component.
  assert(codestream.exists());
  codestream.apply_input_restrictions(0,0,discard_levels,0,NULL);
  codestream.change_appearance(transpose,vflip,hflip);
  configure_reference();
  if ((expansion.x < min_expansion.x) || (expansion.y < min_expansion.y))
    expansion = min_expansion;
  codestream.get_dims(reference_component,image_dims);
  assert((expansion.x >= min_expansion.x) && (expansion.y >= min_expansion.y));
  image_dims.size.x *= expansion.x;
  image_dims.pos.x = (image_dims.pos.x * expansion.x) - ((expansion.x-1)>>1);
  image_dims.size.y *= expansion.y;
  image_dims.pos.y = (image_dims.pos.y * expansion.y) - ((expansion.y-1)>>1);

  // Reset the buffer and view dimensions to zero size.
  valid_dims.pos = buffer_dims.pos = view_dims.pos = image_dims.pos;
  valid_dims.size = buffer_dims.size = view_dims.size = kdu_coords(0,0);

  // Send a message to the child view window identifying the
  // maximum allowable image dimensions.  We expect to hear back (possibly
  // after some windows message processing) concerning
  // the actual view dimensions via the `set_view_size' function.
  child->set_max_view_size(image_dims.size);
}

/******************************************************************************/
/*                    CKdu_showApp::calculate_view_centre                     */
/******************************************************************************/

void
  CKdu_showApp::calculate_view_centre()
{
  if ((!codestream) || (!image_dims))
    return;
  view_centre_known = true;
  view_centre_x =
    (float)(view_dims.pos.x + view_dims.size.x/2 - image_dims.pos.x) /
      ((float) image_dims.size.x);
  view_centre_y =
    (float)(view_dims.pos.y + view_dims.size.y/2 - image_dims.pos.y) /
      ((float) image_dims.size.y);
}

/******************************************************************************/
/*                        CKdu_showApp::set_view_size                         */
/******************************************************************************/

void
  CKdu_showApp::set_view_size(kdu_coords size)
{
  if (!codestream)
    return;

  // Set view region to the largest subset of the image region consistent with
  // the size of the new viewing region.

  kdu_dims new_view_dims = view_dims;
  new_view_dims.size = size;
  if (view_centre_known)
    {
      new_view_dims.pos.x = image_dims.pos.x - (size.x / 2) +
        (int) floor(0.5 + image_dims.size.x*view_centre_x);
      new_view_dims.pos.y = image_dims.pos.y - (size.y / 2) +
        (int) floor(0.5 + image_dims.size.y*view_centre_y);
      view_centre_known = false;
    }
  if (new_view_dims.pos.x < image_dims.pos.x)
    new_view_dims.pos.x = image_dims.pos.x;
  if (new_view_dims.pos.y < image_dims.pos.y)
    new_view_dims.pos.y = image_dims.pos.y;
  kdu_coords view_lim = new_view_dims.pos + new_view_dims.size;
  kdu_coords image_lim = image_dims.pos + image_dims.size;
  if (view_lim.x > image_lim.x)
    new_view_dims.pos.x -= view_lim.x-image_lim.x;
  if (view_lim.y > image_lim.y)
    new_view_dims.pos.y -= view_lim.y-image_lim.y;
  new_view_dims &= image_dims;
  bool need_redraw = new_view_dims.pos != view_dims.pos;
  view_dims = new_view_dims;

  // Get preferred minimum dimensions for the new buffer region.
  size = view_dims.size;
  size.x += (size.x>>4)+100; // A small boundary minimizes impact of scrolling
  size.y += (size.y>>4)+100;

  // Make sure buffered region is no larger than image
  if (size.x > image_dims.size.x)
    size.x = image_dims.size.x;
  if (size.y > image_dims.size.y)
    size.y = image_dims.size.y;
  kdu_dims new_buffer_dims;
  new_buffer_dims.size = size;
  new_buffer_dims.pos = buffer_dims.pos;

  // Make sure the buffer region is contained within the image
  kdu_coords buffer_lim = new_buffer_dims.pos + new_buffer_dims.size;
  if (buffer_lim.x > image_lim.x)
    new_buffer_dims.pos.x -= buffer_lim.x-image_lim.x;
  if (buffer_lim.y > image_lim.y)
    new_buffer_dims.pos.y -= buffer_lim.y-image_lim.y;
  assert(new_buffer_dims == (new_buffer_dims & image_dims));

  // See if the buffered region includes any new locations at all.
  if ((new_buffer_dims.pos != buffer_dims.pos) ||
      (new_buffer_dims != (new_buffer_dims & buffer_dims)) ||
      (view_dims != (view_dims & new_buffer_dims)))
    { // We will need to reshuffle or resize the buffer anyway, so might
      // as well get the best location for the buffer.
      new_buffer_dims.pos.x = view_dims.pos.x -
        (new_buffer_dims.size.x-view_dims.size.x) / 2;
      new_buffer_dims.pos.y = view_dims.pos.y -
        (new_buffer_dims.size.y-view_dims.size.y) / 2;
      if (new_buffer_dims.pos.x < image_dims.pos.x)
        new_buffer_dims.pos.x = image_dims.pos.x;
      if (new_buffer_dims.pos.y < image_dims.pos.y)
        new_buffer_dims.pos.y = image_dims.pos.y;
      buffer_lim = new_buffer_dims.pos + new_buffer_dims.size;
      if (buffer_lim.x > image_lim.x)
        new_buffer_dims.pos.x -= buffer_lim.x - image_lim.x;
      if (buffer_lim.y > image_lim.y)
        new_buffer_dims.pos.y -= buffer_lim.y - image_lim.y;
      assert(view_dims == (view_dims & new_buffer_dims));
      assert(new_buffer_dims == (image_dims & new_buffer_dims));
      assert(view_dims == (new_buffer_dims & view_dims));
    }

  // Now deal with growth in the physical buffer size.
  kdu_coords new_buffer_extent = new_buffer_dims.size;
  if (new_buffer_extent.x < buffer_extent.x)
    new_buffer_extent.x = buffer_extent.x; // Physical buffer can only grow
  if (new_buffer_extent.y < buffer_extent.y)
    new_buffer_extent.y = buffer_extent.y; // Physical buffer can only grow
  new_buffer_extent.x += (4-new_buffer_extent.x) & 3; // Make multiple of 4.
  if (new_buffer_extent != buffer_extent)
    {
      kdu_byte *new_buffer =
        new kdu_byte[new_buffer_extent.x*new_buffer_extent.y*3];
      kdu_byte *sp = buffer;
      kdu_byte *dp = new_buffer;
      for (int y=0; y < buffer_dims.size.y; y++,
           sp += buffer_extent.x*3, dp += new_buffer_extent.x*3)
        memcpy(dp,sp,(size_t)(buffer_dims.size.x*3));
      buffer_extent = new_buffer_extent;
      delete[] buffer;
      buffer = new_buffer;
    }

  // Now deal with changes in the buffered region dimensions
  // We may grow the original buffer size first, as necessary and then check
  // for changes in position.
  int extra_cols = new_buffer_dims.size.x - buffer_dims.size.x;
  if (extra_cols > 0)
    { // Fill extra samples with a grey background.
      kdu_byte *spp = buffer;
      for (int y=0; y < buffer_dims.size.y; y++, spp+=buffer_extent.x*3)
        {
          kdu_byte *sp = spp + buffer_dims.size.x*3;
          for (int c=extra_cols; c > 0; c--)
            { *(sp++) = 128; *(sp++) = 128; *(sp++) = 128; }
        }
      buffer_dims.size.x = new_buffer_dims.size.x;
    }
  int extra_rows = new_buffer_dims.size.y - buffer_dims.size.y;
  if (extra_rows > 0)
    { // Fill extra samples rows with a grey background.
      kdu_byte *spp = buffer + (buffer_dims.size.y*buffer_extent.x*3);
      for (int r=extra_rows; r > 0; r--, spp += buffer_extent.x*3)
        {
          kdu_byte *sp = spp;
          for (int c=new_buffer_dims.size.x; c > 0; c--)
            { *(sp++) = 128; *(sp++) = 128; *(sp++) = 128; }
        }
      buffer_dims.size.y = new_buffer_dims.size.y;
    }
  update_buffer_pos(new_buffer_dims.pos);
  buffer_dims.size = new_buffer_dims.size; // In case the size shrunk

  // Now reflect changes in the view size to the appearance of scroll bars.

  SCROLLINFO sc_info; sc_info.cbSize = sizeof(sc_info);
  sc_info.fMask = SIF_DISABLENOSCROLL | SIF_ALL;
  sc_info.nMin = 0;
  sc_info.nMax = image_dims.size.x-1;
  sc_info.nPage = view_dims.size.x;
  sc_info.nPos = view_dims.pos.x - image_dims.pos.x;
  child->SetScrollInfo(SB_HORZ,&sc_info);
  sc_info.fMask = SIF_DISABLENOSCROLL | SIF_ALL;
  sc_info.nMin = 0;
  sc_info.nMax = image_dims.size.y-1;
  sc_info.nPage = view_dims.size.y;
  sc_info.nPos = view_dims.pos.y - image_dims.pos.y;
  child->SetScrollInfo(SB_VERT,&sc_info);
  kdu_coords step = view_dims.size;
  step.x = (step.x >> 4) + 1;
  step.y = (step.y >> 4) + 1;
  kdu_coords page = view_dims.size - step;
  child->set_scroll_metrics(step,page,image_dims.size-view_dims.size);

  // Finally, reflect any buffer changes into the valid data region.
  update_valid_region();
  if (need_redraw)
    child->Invalidate();
  child->UpdateWindow();
}

/******************************************************************************/
/*                       CKdu_showApp::set_hscroll_pos                        */
/******************************************************************************/

void
  CKdu_showApp::set_hscroll_pos(int pos, bool relative_to_last)
{
  if (!codestream)
    return;

  view_centre_known = false;
  if (relative_to_last)
    pos += view_dims.pos.x;
  else
    pos += image_dims.pos.x;
  if (pos < image_dims.pos.x)
    pos = image_dims.pos.x;
  if ((pos+view_dims.size.x) > (image_dims.pos.x+image_dims.size.x))
    pos = image_dims.pos.x+image_dims.size.x-view_dims.size.x;
  if (pos != view_dims.pos.x)
    {
      RECT update;
      child->ScrollWindowEx(view_dims.pos.x-pos,0,NULL,NULL,NULL,&update,0);
      view_dims.pos.x = pos;
      if (view_dims != (view_dims & buffer_dims))
        { // The view is no longer fully contained in the buffered region.
          kdu_coords buf_pos = buffer_dims.pos;
          buf_pos.x = view_dims.pos.x - (buffer_dims.size.x-view_dims.size.x)/2;
          if (buf_pos.x < image_dims.pos.x)
            buf_pos.x = image_dims.pos.x;
          int image_lim = image_dims.pos.x+image_dims.size.x;
          int buf_lim = buf_pos.x + buffer_dims.size.x;
          if (buf_lim > image_lim)
            buf_pos.x -= (buf_lim-image_lim);
          update_buffer_pos(buf_pos);        
        }
      // Repaint the erased area -- note that although the scroll function
      // is supposed to be able to invalidate the relevant regions of the
      // window, rendering this code unnecessary, that functionality appears
      // to be able to fail badly under certain extremely fast scrolling
      // sequences. Best to do the job ourselves.
      kdu_dims update_dims;
      update_dims.pos.x = update.left;
      update_dims.pos.y = update.top;
      update_dims.size.x = update.right-update.left;
      update_dims.size.y = update.bottom-update.top;
      CDC *dc = child->GetDC();
      paint_region(dc,update_dims);
      child->ReleaseDC(dc);
    }
  child->SetScrollPos(SB_HORZ,pos-image_dims.pos.x);
}

/******************************************************************************/
/*                       CKdu_showApp::set_vscroll_pos                        */
/******************************************************************************/

void
  CKdu_showApp::set_vscroll_pos(int pos, bool relative_to_last)
{
  if (!codestream)
    return;

  view_centre_known = false;
  if (relative_to_last)
    pos += view_dims.pos.y;
  else
    pos += image_dims.pos.y;
  if (pos < image_dims.pos.y)
    pos = image_dims.pos.y;
  if ((pos+view_dims.size.y) > (image_dims.pos.y+image_dims.size.y))
    pos = image_dims.pos.y+image_dims.size.y-view_dims.size.y;
  if (pos != view_dims.pos.y)
    {
      RECT update;
      child->ScrollWindowEx(0,view_dims.pos.y-pos,NULL,NULL,NULL,&update,0);
      view_dims.pos.y = pos;
      if (view_dims != (view_dims & buffer_dims))
        { // The view is no longer fully contained in the buffered region.
          kdu_coords buf_pos = buffer_dims.pos;
          buf_pos.y = view_dims.pos.y - (buffer_dims.size.y-view_dims.size.y)/2;
          if (buf_pos.y < image_dims.pos.y)
            buf_pos.y = image_dims.pos.y;
          int image_lim = image_dims.pos.y+image_dims.size.y;
          int buf_lim = buf_pos.y + buffer_dims.size.y;
          if (buf_lim > image_lim)
            buf_pos.y -= (buf_lim-image_lim);
          update_buffer_pos(buf_pos);        
        }
      // Repaint the erased area -- note that although the scroll function
      // is supposed to be able to invalidate the relevant regions of the
      // window, rendering this code unnecessary, that functionality appears
      // to be able to fail badly under certain extremely fast scrolling
      // sequences.  Best to do the job ourselves.
      kdu_dims update_dims;
      update_dims.pos.x = update.left;
      update_dims.pos.y = update.top;
      update_dims.size.x = update.right-update.left;
      update_dims.size.y = update.bottom-update.top;
      CDC *dc = child->GetDC();
      paint_region(dc,update_dims);
      child->ReleaseDC(dc);
    }
  child->SetScrollPos(SB_VERT,pos-image_dims.pos.y);
}

/******************************************************************************/
/*                      CKdu_showApp::update_buffer_pos                       */
/******************************************************************************/

void
  CKdu_showApp::update_buffer_pos(kdu_coords pos)
{
  if (pos.y < buffer_dims.pos.y)
    { // Shift buffered data down.
      int r, overlap_rows = buffer_dims.size.y + pos.y - buffer_dims.pos.y;
      if (overlap_rows < 0)
        overlap_rows = 0;
      kdu_byte *dp = buffer + (buffer_dims.size.y*buffer_extent.x*3);
      kdu_byte *sp = dp + (pos.y-buffer_dims.pos.y)*(buffer_extent.x*3);
      for (r=0; r < overlap_rows; r++)
        {
          sp -= buffer_extent.x*3; dp -= buffer_extent.x*3;
          memcpy(dp,sp,(size_t)(buffer_dims.size.x*3));
        }
      for (; r < buffer_dims.size.y; r++)
        {
          dp -= buffer_extent.x*3;
          memset(dp,128,(size_t)(buffer_dims.size.x*3));
        }
      buffer_dims.pos.y = pos.y;
    }
  else if (pos.y > buffer_dims.pos.y)
    { // Shift buffered data up.
      int r, overlap_rows = buffer_dims.size.y - pos.y + buffer_dims.pos.y;
      if (overlap_rows < 0)
        overlap_rows = 0;
      kdu_byte *dp = buffer;
      kdu_byte *sp = dp + (pos.y-buffer_dims.pos.y)*(buffer_extent.x*3);
      for (r=0; r < overlap_rows; r++)
        {
          memcpy(dp,sp,(size_t)(buffer_dims.size.x*3));
          sp += buffer_extent.x*3; dp += buffer_extent.x*3;
        }
      for (; r < buffer_dims.size.y; r++)
        {
          memset(dp,128,(size_t)(buffer_dims.size.x*3));
          dp += buffer_extent.x*3;
        }
      buffer_dims.pos.y = pos.y;
    }

  if (pos.x < buffer_dims.pos.x)
    { // Shift buffered data to the right.
      int r, c, overlap_cols = buffer_dims.size.x + pos.x - buffer_dims.pos.x;
      if (overlap_cols < 0)
        overlap_cols = 0;
      kdu_byte *dp = buffer + (buffer_dims.size.x*3);
      kdu_byte *sp = dp + (pos.x-buffer_dims.pos.x)*3;
      for (r=buffer_dims.size.y; r > 0; r--)
        {
          for (c=0; c < overlap_cols; c++)
            { *(--dp) = *(--sp); *(--dp) = *(--sp); *(--dp) = *(--sp); }
          for (; c < buffer_dims.size.x; c++, sp -= 3)
            { *(--dp) = 128; *(--dp) = 128; *(--dp) = 128; }
          sp += (buffer_extent.x+buffer_dims.size.x)*3;
          dp += (buffer_extent.x+buffer_dims.size.x)*3;
        }
      buffer_dims.pos.x = pos.x;
    }
  else if (pos.x > buffer_dims.pos.x)
    { // Shift buffered data to the left.
      int r, c, overlap_cols = buffer_dims.size.x - pos.x + buffer_dims.pos.x;
      if (overlap_cols < 0)
        overlap_cols = 0;
      kdu_byte *dp = buffer;
      kdu_byte *sp = dp + (pos.x-buffer_dims.pos.x)*3;
      for (r=buffer_extent.y; r > 0; r--)
        {
          for (c=0; c < overlap_cols; c++)
            { *(dp++) = *(sp++); *(dp++) = *(sp++); *(dp++) = *(sp++); }
          for (; c < buffer_dims.size.x; c++, sp+=3)
            { *(dp++) = 128; *(dp++) = 128; *(dp++) = 128; }
          sp += (buffer_extent.x-buffer_dims.size.x)*3;
          dp += (buffer_extent.x-buffer_dims.size.x)*3;
        }
      buffer_dims.pos.x = pos.x;
    }
  update_valid_region();
}

/******************************************************************************/
/*                      CKdu_showApp::update_valid_region                     */
/******************************************************************************/

void
  CKdu_showApp::update_valid_region()
{
  valid_dims &= buffer_dims;
  if (processing)
    {
      region_in_process &= buffer_dims;
      incomplete_region &= buffer_dims;
    }
}

/******************************************************************************/
/*                        CKdu_showApp::paint_region                          */
/******************************************************************************/

void
  CKdu_showApp::paint_region(CDC *dc, kdu_dims region)
{
  kdu_dims region_on_buffer = region;
  region_on_buffer.pos += view_dims.pos; // Convert to absolute region
  region_on_buffer &= buffer_dims; // Intersect with buffer region
  region_on_buffer.pos -= buffer_dims.pos; // Make relative to buffer region
  if (region_on_buffer.size != region.size)
    { /* Need to erase the region first and then modify it to reflect the
         region we are about to actually paint. */
      dc->BitBlt(region.pos.x,region.pos.y,region.size.x,region.size.y,
                 NULL,0,0,WHITENESS);
      region = region_on_buffer;
      region.pos += buffer_dims.pos; // Convert to absolute region
      region.pos -= view_dims.pos; // Make relative to client region.
    }
  if (!region_on_buffer)
    return;
  assert(region.size == region_on_buffer.size);

  BITMAPINFO info;
  memset(&info,0,sizeof(info));
  info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  info.bmiHeader.biWidth = buffer_extent.x;
  info.bmiHeader.biHeight = -region.size.y;
  info.bmiHeader.biPlanes = 1;
  info.bmiHeader.biBitCount = 24;
  info.bmiHeader.biCompression = BI_RGB;

  kdu_byte *start = buffer +
    (region_on_buffer.pos.y*buffer_extent.x + region_on_buffer.pos.x)*3;
  int success =
    SetDIBitsToDevice(dc->GetSafeHdc(),region.pos.x,region.pos.y,
                      region.size.x,region.size.y,0,0,0,region.size.y,
                      start,&info,DIB_RGB_COLORS);
  if (!success)
    { // This should not be necessary, except that there appears to be a nasty
      // bug in the Windows OS or MFC.  After a window has been automatically
      // resized (required sometimes after rotation or zooming operations to
      // make sure the image fits inside the window), the framework sometimes
      // gets itself into an inexplicable (and certainly undocumented) funk,
      // where attempts to paint to a region which includes the last line of
      // the window sometimes cause the entire paint operation to fail.  I
      // have tried just about every version of the painting functions,
      // defering painting operations thru the windows message loop and all
      // sorts of things.  To create the problem yourself (let me know if you
      // are able to fix it) try viewing an image which is wider than it is
      // tall.  Rotate the image thru 360 degrees so that the window is forced
      // to shrink in the horizontal direction.  Then try scrolling to the
      // right (scrolling to the left seems not to be so problematic).  My
      // only conclusion at this point is that it is a Microsoft bug which I
      // can do nothing about.  The current work-around, simply does not update
      // the last line of the viewing window, which is not ideal.  Manually
      // resizing the window horizontally can get the framework out of this
      // difficulty.  Windows NT has also been observed to be a lot more
      // robust to this problem than Windows 95.
      success =
        SetDIBitsToDevice(dc->GetSafeHdc(),region.pos.x,region.pos.y,
                          region.size.x,region.size.y-1,0,0,0,region.size.y-1,
                          start,&info,DIB_RGB_COLORS);
    }
}

/******************************************************************************/
/*                 CKdu_showApp::display_quality_layer_status                 */
/******************************************************************************/

void
  CKdu_showApp::display_quality_layer_status()
{
  if (!codestream)
    return;

  char string[128];
  if (max_display_layers >= codestream.get_max_tile_layers())
    sprintf(string,"All quality layers in use.");
  else
    sprintf(string,"Using %d of %d quality layers.",
            max_display_layers,codestream.get_max_tile_layers());
  frame->SetMessageText(string);
}

/******************************************************************************/
/*                            CKdu_showApp::OnIdle                            */
/******************************************************************************/

BOOL CKdu_showApp::OnIdle(LONG lCount) 
  /* Note: this function implements a very simple heuristic for scheduling
     regions for processing.  It is intended primarily to demonstrate
     capabilities of the underlying framework -- in particular the
     `kd_region_decompressor' object and the rest of the Kakadu JPEG2000
     platform on which it is built. */
{
  if (in_idle || !codestream)
    return FALSE; // Don't need to be called from recursive loop again.

  in_idle = true;
  if (!processing)
    { // Decide on a region to process.
      if (valid_dims.area() < (buffer_dims.area()>>2))
        region_in_process = buffer_dims; // No point skirting round `valid_dims'
      else
        { /* The new region should share a boundary with `valid_dims' so that
             they can be added together to form a new rectangular valid region.
             Of the four possible regions of this form, pick the one whose
             intersection with the current view is largest. */
          kdu_coords valid_min = valid_dims.pos;
          kdu_coords valid_lim = valid_min + valid_dims.size;
          kdu_coords buffer_min = buffer_dims.pos;
          kdu_coords buffer_lim = buffer_min + buffer_dims.size;
          int needed_left = valid_min.x - buffer_min.x;
          int needed_right = buffer_lim.x - valid_lim.x;
          int needed_above = valid_min.y - buffer_min.y;
          int needed_below = buffer_lim.y - valid_lim.y;
          assert((needed_left >= 0) && (needed_right >= 0) &&
                 (needed_above >= 0) && (needed_below >= 0));
          kdu_dims region_left = valid_dims;
          region_left.pos.x = buffer_min.x; region_left.size.x = needed_left;
          kdu_dims region_right = valid_dims;
          region_right.pos.x = valid_lim.x; region_right.size.x = needed_right;
          kdu_dims region_above = valid_dims;
          region_above.pos.y = buffer_min.y; region_above.size.y = needed_above;
          kdu_dims region_below = valid_dims;
          region_below.pos.y = valid_lim.y; region_below.size.y = needed_below;
          region_in_process = region_left;
          if (((region_in_process & view_dims).area() <
               (region_right & view_dims).area()) || !region_in_process)
            region_in_process = region_right;
          if (((region_in_process & view_dims).area() <
               (region_above & view_dims).area()) || !region_in_process)
            region_in_process = region_above;
          if (((region_in_process & view_dims).area() <
               (region_below & view_dims).area()) || !region_in_process)
            region_in_process = region_below;
        }
      if (!(!region_in_process))
        {
          decompressor.start(codestream,(single_component<0)?(&channels):NULL,
                             single_component,discard_levels,max_display_layers,
                             region_in_process,expansion);
          incomplete_region = region_in_process;
          processing = true;
        }
    }
  if (processing)
    { // Process some more data.
      kdu_dims new_region;
      if ((!decompressor.process(buffer,buffer_extent.x,buffer_dims,
                                 incomplete_region,8192,new_region)) ||
          !incomplete_region)
        {
          processing = false;
          if (!decompressor.finish())
            { // Code-stream failure.  Must destroy it.
              close_file();
            }
          else
            valid_dims = combine_regions(valid_dims,region_in_process);
        }
      // In any event, paint any newly decompressed region right away.
      new_region &= view_dims; // No point in painting invisible regions.
      if (!(!new_region))
        {
          new_region.pos -= view_dims.pos; // Make relative to view region.
          CDC *dc = child->GetDC();
          paint_region(dc,new_region);
          child->ReleaseDC(dc);
        }
    }
  in_idle = false;
  if (valid_dims != buffer_dims)
    return TRUE;
  display_quality_layer_status(); // All processing done, keep status up to date
  return CWinApp::OnIdle(lCount); // Give low-level idle processing a go.
}

/******************************************************************************/
/*                          CKdu_showApp::OnAppAbout                          */
/******************************************************************************/

void CKdu_showApp::OnAppAbout()
{
  CAboutDlg aboutDlg;
  aboutDlg.DoModal();
}

/******************************************************************************/
/*                          CKdu_showApp::OnFileClose                         */
/******************************************************************************/

void CKdu_showApp::OnFileClose() 
{
  if (codestream.exists())
    close_file();	
}

/******************************************************************************/
/*                       CKdu_showApp::OnUpdateFileClose                      */
/******************************************************************************/

void CKdu_showApp::OnUpdateFileClose(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(codestream.exists());
}

/******************************************************************************/
/*                          CKdu_showApp::OnFileOpen                          */
/******************************************************************************/

void CKdu_showApp::OnFileOpen() 
{
  char filename[1024];
  OPENFILENAME ofn; memset(&ofn,0,sizeof(ofn)); ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = m_pMainWnd->GetSafeHwnd();
  ofn.lpstrFilter =
    "JP2 compatible file (*.jp2, *.jpx)\0*.jp2;*.jpx\0"
    "JPEG2000 unwrapped code-stream (*.j2c)\0*.j2c\0"
    "Arbitrary name (*.*)\0*.*\0\0";
  ofn.nFilterIndex = 3;
  ofn.lpstrFile = filename; filename[0] = '\0';
  ofn.nMaxFile = 1023;
  ofn.lpstrTitle = "Open JPEG2000 Compressed Image";
  ofn.Flags = OFN_FILEMUSTEXIST;
  if (!GetOpenFileName(&ofn))
    return;
  open_file(filename);
}

/******************************************************************************/
/*                        CKdu_showApp::OnFileProperties                      */
/******************************************************************************/

void CKdu_showApp::OnFileProperties() 
{
  if (!codestream)
    return;
  if (processing)
    {
      processing = false;
      if (!decompressor.finish())
        {
          close_file();
          return;
        }
    }

  // Make sure we have all tile information.
  if (!tiles_loaded)
    {
      try { // We may be parsing the code-stream for the first time
          max_discard_levels = -1;
          codestream.apply_input_restrictions(0,total_components,0,0,NULL);
          kdu_dims valid_tiles; codestream.get_valid_tiles(valid_tiles);
          kdu_coords idx;
          for (idx.y=0; idx.y < valid_tiles.size.y; idx.y++)
            for (idx.x=0; idx.x < valid_tiles.size.x; idx.x++)
              {
                kdu_tile tile = codestream.open_tile(valid_tiles.pos+idx);
                tile.close();
              }
          tiles_loaded = true;
        }
      catch (int)
        {
          close_file();
          return;
        }
    }

  // Textualize the properties into an `ostrstream' object.
  std::ostrstream string_buf;
  kdu_params *root = codestream.access_siz();
  string_buf << "<<<<< Main Header >>>>>\n";
  root->textualize_attributes(string_buf,-1,-1);
  codestream.apply_input_restrictions(0,total_components,0,0,NULL);
  codestream.change_appearance(false,false,false);
  kdu_dims valid_tiles; codestream.get_valid_tiles(valid_tiles);
  kdu_coords idx;
  for (idx.y=0; idx.y < valid_tiles.size.y; idx.y++)
    for (idx.x=0; idx.x < valid_tiles.size.x; idx.x++)
      {
        kdu_dims tile_dims;
        codestream.get_tile_dims(valid_tiles.pos+idx,-1,tile_dims);
        int tnum = idx.x + idx.y*valid_tiles.size.x;
        string_buf << "<<<<< Tile " << tnum << " >>>>>"
                 << " Canvas coordinates: "
                 << "y = " << tile_dims.pos.y
                 << "; x = " << tile_dims.pos.x
                 << "; height = " << tile_dims.size.y
                 << "; width = " << tile_dims.size.x << "\n";
      root->textualize_attributes(string_buf,tnum,tnum);
    }

  // Display the properties.
  CPropertiesDlg properties(codestream,&string_buf);
  properties.DoModal();
}

/******************************************************************************/
/*                     CKdu_showApp::OnUpdateFileProperties                   */
/******************************************************************************/

void CKdu_showApp::OnUpdateFileProperties(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(codestream.exists());
}


/******************************************************************************/
/*                          CKdu_showApp::OnViewHflip                         */
/******************************************************************************/

void CKdu_showApp::OnViewHflip()
{
  if (!codestream)
    return;
  hflip = !hflip;
  calculate_view_centre();
  view_centre_x = 1.0F - view_centre_x;
  initialize_regions();
}

/******************************************************************************/
/*                       CKdu_showApp::OnUpdateViewHflip                      */
/******************************************************************************/

void CKdu_showApp::OnUpdateViewHflip(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(codestream.exists());
}

/******************************************************************************/
/*                          CKdu_showApp::OnViewVflip                         */
/******************************************************************************/

void CKdu_showApp::OnViewVflip() 
{
  if (!codestream)
    return;
  vflip = !vflip;
  calculate_view_centre();
  view_centre_y = 1.0F - view_centre_y;
  initialize_regions();
}

/******************************************************************************/
/*                       CKdu_showApp::OnUpdateViewVflip                      */
/******************************************************************************/

void CKdu_showApp::OnUpdateViewVflip(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(codestream.exists());
}

/******************************************************************************/
/*                          CKdu_showApp::OnViewRotate                        */
/******************************************************************************/

void CKdu_showApp::OnViewRotate() 
{
  if (!codestream)
    return;
  // Need to effectively add an extra transpose, followed by an extra hflip.
  transpose = !transpose;
  expansion.transpose();
  min_expansion.transpose();
  bool tmp = hflip; hflip = vflip; vflip = tmp;
  hflip = !hflip;
  calculate_view_centre();
  float f_tmp = view_centre_y;
  view_centre_y = view_centre_x;
  view_centre_x = 1.0F-f_tmp;
  initialize_regions();
}

/******************************************************************************/
/*                       CKdu_showApp::OnUpdateViewRotate                     */
/******************************************************************************/

void CKdu_showApp::OnUpdateViewRotate(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(codestream.exists());
}

/******************************************************************************/
/*                      CKdu_showApp::OnViewCounterRotate                     */
/******************************************************************************/

void CKdu_showApp::OnViewCounterRotate() 
{
  if (!codestream)
    return;
  // Need to effectively add an extra transpose, followed by an extra vflip.
  transpose = !transpose;
  expansion.transpose();
  min_expansion.transpose();
  bool tmp = hflip; hflip = vflip; vflip = tmp;
  vflip = !vflip;
  calculate_view_centre();
  float f_tmp = view_centre_x;
  view_centre_x = view_centre_y;
  view_centre_y = 1.0F-f_tmp;
  initialize_regions();
}

/******************************************************************************/
/*                   CKdu_showApp::OnUpdateViewCounterRotate                  */
/******************************************************************************/

void CKdu_showApp::OnUpdateViewCounterRotate(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(codestream.exists());
}

/******************************************************************************/
/*                         CKdu_showApp::OnViewZoomOut                        */
/******************************************************************************/

void CKdu_showApp::OnViewZoomOut() 
{
  if (!codestream)
    return;
  if (max_discard_levels < 0)
    calculate_max_discard_levels();
  if ((expansion.x > min_expansion.x) || (expansion.y > min_expansion.y))
    {
      expansion.x >>= 1;
      expansion.y >>= 1;
      if ((expansion.x < min_expansion.x) || (expansion.y < min_expansion.y))
        expansion = min_expansion;
    }
  else if (discard_levels < max_discard_levels)
    discard_levels++;
  calculate_view_centre();
  initialize_regions();
}

/******************************************************************************/
/*                      CKdu_showApp::OnUpdateViewZoomOut                     */
/******************************************************************************/

void CKdu_showApp::OnUpdateViewZoomOut(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(codestream.exists() &&
                 ((max_discard_levels < 0) ||
                  (discard_levels < max_discard_levels) ||
                  (expansion.x > min_expansion.x) ||
                  (expansion.y > min_expansion.y)));
}

/******************************************************************************/
/*                         CKdu_showApp::OnViewZoomIn                         */
/******************************************************************************/

void CKdu_showApp::OnViewZoomIn() 
{
  if (!codestream)
    return;
  discard_levels--;
  if (discard_levels < 0)
    {
      discard_levels = 0;
      expansion.x *= 2;
      expansion.y *= 2;
    }
  calculate_view_centre();
  initialize_regions();
}

/******************************************************************************/
/*                      CKdu_showApp::OnUpdateViewZoomIn                      */
/******************************************************************************/

void CKdu_showApp::OnUpdateViewZoomIn(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(codestream.exists());
}

/******************************************************************************/
/*                         CKdu_showApp::OnViewRestore                        */
/******************************************************************************/

void CKdu_showApp::OnViewRestore() 
{
  if (!codestream)
    return;
  transpose = vflip = hflip = NULL;
  discard_levels = 0;
  expansion = kdu_coords(1,1); // May be increased inside `initialize_regions'
  calculate_view_centre();
  initialize_regions();
}

/******************************************************************************/
/*                      CKdu_showApp::OnUpdateViewRestore                     */
/******************************************************************************/

void CKdu_showApp::OnUpdateViewRestore(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(codestream.exists() &&
                 (transpose || vflip || hflip || (discard_levels > 0) ||
                  (expansion != min_expansion)));
}

/******************************************************************************/
/*                        CKdu_showApp::OnViewWiden                           */
/******************************************************************************/

void CKdu_showApp::OnViewWiden() 
{
  RECT rect;
  kdu_dims target_dims;

  m_pMainWnd->GetWindowRect(&rect);
  target_dims.pos.x = rect.left;
  target_dims.pos.y = rect.top;
  target_dims.size.x = rect.right-rect.left;
  target_dims.size.y = rect.bottom-rect.top;
  target_dims.pos.x -= target_dims.size.x / 4;
  target_dims.pos.y -= target_dims.size.y / 4;
  target_dims.size.x += target_dims.size.x / 2;
  target_dims.size.y += target_dims.size.y / 2;
  if (target_dims.pos.x < 0)
    target_dims.pos.x = 0;
  if (target_dims.pos.y < 0)
    target_dims.pos.y = 0;
  m_pMainWnd->SetWindowPos(NULL,target_dims.pos.x,target_dims.pos.y,
                           target_dims.size.x,target_dims.size.y,
                           SWP_SHOWWINDOW | SWP_NOZORDER);
}

/******************************************************************************/
/*                      CKdu_showApp::OnUpdateViewWiden                       */
/******************************************************************************/

void CKdu_showApp::OnUpdateViewWiden(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(!codestream ||
                 ((view_dims.size.x < image_dims.size.x) ||
                  (view_dims.size.y < image_dims.size.y)));
}

/******************************************************************************/
/*                        CKdu_showApp::OnViewShrink                          */
/******************************************************************************/

void CKdu_showApp::OnViewShrink() 
{
  RECT rect;
  kdu_coords target_size;

  m_pMainWnd->GetWindowRect(&rect);
  target_size.x = rect.right-rect.left;
  target_size.y = rect.bottom-rect.top;
  target_size.x -= target_size.x / 3;
  target_size.y -= target_size.y / 3;
  if (target_size.x < 100)
    target_size.x = rect.right-rect.left;
  if (target_size.y < 100)
    target_size.y = rect.bottom-rect.top;
  m_pMainWnd->SetWindowPos(NULL,0,0,target_size.x,target_size.y,
                           SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOZORDER);
}

/******************************************************************************/
/*                      CKdu_showApp::OnUpdateViewShrink                      */
/******************************************************************************/

void CKdu_showApp::OnUpdateViewShrink(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(TRUE);
}

/******************************************************************************/
/*                          CKdu_showApp::OnModeFast                          */
/******************************************************************************/

void CKdu_showApp::OnModeFast() 
{
  fast = true; fussy = false; resilient = false; resilient_sop = false;
  if (codestream.exists())
    codestream.set_fast();
}

/******************************************************************************/
/*                       CKdu_showApp::OnUpdateModeFast                       */
/******************************************************************************/

void CKdu_showApp::OnUpdateModeFast(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(TRUE);
  pCmdUI->SetCheck(fast);
}

/******************************************************************************/
/*                         CKdu_showApp::OnModeFussy                          */
/******************************************************************************/

void CKdu_showApp::OnModeFussy() 
{
  fast = false; fussy = true; resilient = false; resilient_sop = false;
  if (codestream.exists())
    codestream.set_fussy();
}

/******************************************************************************/
/*                      CKdu_showApp::OnUpdateModeFussy                       */
/******************************************************************************/

void CKdu_showApp::OnUpdateModeFussy(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(TRUE);
  pCmdUI->SetCheck(fussy);
}

/******************************************************************************/
/*                       CKdu_showApp::OnModeResilient                        */
/******************************************************************************/

void CKdu_showApp::OnModeResilient() 
{
  fast = false; fussy = false; resilient = true; resilient_sop = false;
  if (codestream.exists())
    codestream.set_resilient();
}

/******************************************************************************/
/*                   CKdu_showApp::OnUpdateModeResilient                      */
/******************************************************************************/

void CKdu_showApp::OnUpdateModeResilient(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(TRUE);
  pCmdUI->SetCheck(resilient);
}

/******************************************************************************/
/*                     CKdu_showApp::OnModeResilientSop                       */
/******************************************************************************/

void CKdu_showApp::OnModeResilientSop() 
{
  fast = false; fussy = false; resilient = false; resilient_sop = true;
  if (codestream.exists())
    codestream.set_resilient(true);
}

/******************************************************************************/
/*                 CKdu_showApp::OnUpdateModeResilientSop                     */
/******************************************************************************/

void CKdu_showApp::OnUpdateModeResilientSop(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(TRUE);
  pCmdUI->SetCheck(resilient_sop);
}

/******************************************************************************/
/*                        CKdu_showApp::OnComponent1                          */
/******************************************************************************/

void CKdu_showApp::OnComponent1() 
{
  if ((!codestream) || (single_component == 0))
    return;
  single_component = 0;
  calculate_view_centre();
  initialize_regions();
}

/******************************************************************************/
/*                     CKdu_showApp::OnUpdateComponent1                       */
/******************************************************************************/

void CKdu_showApp::OnUpdateComponent1(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(codestream.exists());
  pCmdUI->SetCheck(codestream.exists() && (single_component == 0));
}

/******************************************************************************/
/*                        CKdu_showApp::OnComponent2                          */
/******************************************************************************/

void CKdu_showApp::OnComponent2() 
{
  if ((!codestream) || (total_components < 2) || (single_component == 1))
    return;
  single_component = 1;
  calculate_view_centre();
  initialize_regions();
}

/******************************************************************************/
/*                     CKdu_showApp::OnUpdateComponent2                       */
/******************************************************************************/

void CKdu_showApp::OnUpdateComponent2(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(codestream.exists() && (total_components >= 2));
  pCmdUI->SetCheck(codestream.exists() && (single_component == 1));
}

/******************************************************************************/
/*                        CKdu_showApp::OnComponent3                          */
/******************************************************************************/

void CKdu_showApp::OnComponent3() 
{
  if ((!codestream) || (total_components < 3) || (single_component == 2))
    return;
  single_component = 2;
  calculate_view_centre();
  initialize_regions();
}

/******************************************************************************/
/*                     CKdu_showApp::OnUpdateComponent3                       */
/******************************************************************************/

void CKdu_showApp::OnUpdateComponent3(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(codestream.exists() && (total_components >= 3));
  pCmdUI->SetCheck(codestream.exists() && (single_component == 2));
}

/******************************************************************************/
/*                        CKdu_showApp::OnComponent4                          */
/******************************************************************************/

void CKdu_showApp::OnComponent4() 
{
  if ((!codestream) || (total_components < 4) || (single_component == 3))
    return;
  single_component = 3;
  calculate_view_centre();
  initialize_regions();
}

/******************************************************************************/
/*                     CKdu_showApp::OnUpdateComponent4                       */
/******************************************************************************/

void CKdu_showApp::OnUpdateComponent4(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(codestream.exists() && (total_components >= 4));
  pCmdUI->SetCheck(codestream.exists() && (single_component == 3));
}

/******************************************************************************/
/*                       CKdu_showApp::OnMultiComponent                       */
/******************************************************************************/

void CKdu_showApp::OnMultiComponent() 
{
  if ((!codestream) || (single_component < 0))
    return;
  single_component = -1;
  calculate_view_centre();
  initialize_regions();
}

/******************************************************************************/
/*                   CKdu_showApp::OnUpdateMultiComponent                     */
/******************************************************************************/

void CKdu_showApp::OnUpdateMultiComponent(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(codestream.exists());
  pCmdUI->SetCheck(codestream.exists() && (single_component < 0));
}

/******************************************************************************/
/*                        CKdu_showApp::OnLayersLess                          */
/******************************************************************************/

void CKdu_showApp::OnLayersLess()
{
  if (!codestream)
    return;
  if (max_display_layers > codestream.get_max_tile_layers())
    max_display_layers = codestream.get_max_tile_layers();
  if (max_display_layers <= 1)
    { max_display_layers = 1; return; }
  max_display_layers--;
  if (processing)
    {
      processing = false;
      if (!decompressor.finish())
        {
          close_file();
          return;
        }
    }
  valid_dims.size = kdu_coords(0,0);
  display_quality_layer_status();
}

/******************************************************************************/
/*                     CKdu_showApp::OnUpdateLayersLess                       */
/******************************************************************************/

void CKdu_showApp::OnUpdateLayersLess(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(codestream.exists() && (max_display_layers > 1));
}

/******************************************************************************/
/*                        CKdu_showApp::OnLayersMore                          */
/******************************************************************************/

void CKdu_showApp::OnLayersMore() 
{
  if (!codestream)
    return;
  bool need_update = (max_display_layers < codestream.get_max_tile_layers());
  max_display_layers++;
  if (max_display_layers >= codestream.get_max_tile_layers())
    max_display_layers = 1<<16;
  if (processing)
    {
      processing = false;
      if (!decompressor.finish())
        {
          close_file();
          return;
        }
    }
  if (need_update)
    {
      valid_dims.size = kdu_coords(0,0);
      display_quality_layer_status();
    }
}

/******************************************************************************/
/*                     CKdu_showApp::OnUpdateLayersMore                       */
/******************************************************************************/

void CKdu_showApp::OnUpdateLayersMore(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(codestream.exists() &&
                 (max_display_layers < codestream.get_max_tile_layers()));
}
