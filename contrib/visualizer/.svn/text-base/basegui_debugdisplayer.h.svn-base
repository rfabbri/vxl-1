#ifndef __DISPLAY_DEBUG_H__
#define __DISPLAY_DEBUG_H__

//#include "MainGUIWindow.h"

#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_vector.h>
#include <vcl_string.h>

#include <wx/wx.h>
#include <wx/mdi.h>
#include <wx/sashwin.h>
#include <wx/laywin.h>
#include <wx/notebook.h>

class DebugDisplayer {
 private:
  int _numWins;
  wxStatusBar* _statusBar;
  wxNotebook* _notebook;
  wxTextCtrl* _debugTexts[20]; // It should be plenty?
  vcl_ostream* _ostrms[20];
 public:
  DebugDisplayer();
  ~DebugDisplayer();
  void notebook(wxNotebook*nt_);
  void statusBar(wxStatusBar*sb_);
  void debugTexts(wxTextCtrl**dt,int numWins_);
  void clear(int winId=0);
  vcl_ostream* ostrm(int winId=0);
  vcl_streambuf* strmbuf(int winId=0);
  wxTextCtrl*debugText(int winId=0);
  void disp(const vcl_string& msg, int winId=0);
  void status(const vcl_string& status);
};

extern DebugDisplayer GDebugDisplayer;

#endif
