#ifndef __DISPLAY_DEBUG_H__
#define __DISPLAY_DEBUG_H__

//#include "MainGUIWindow.h"

#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

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
  std::ostream* _ostrms[20];
 public:
  DebugDisplayer();
  ~DebugDisplayer();
  void notebook(wxNotebook*nt_);
  void statusBar(wxStatusBar*sb_);
  void debugTexts(wxTextCtrl**dt,int numWins_);
  void clear(int winId=0);
  std::ostream* ostrm(int winId=0);
  std::streambuf* strmbuf(int winId=0);
  wxTextCtrl*debugText(int winId=0);
  void disp(const std::string& msg, int winId=0);
  void status(const std::string& status);
};

extern DebugDisplayer GDebugDisplayer;

#endif
