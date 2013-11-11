#include "basegui_debugdisplayer.h"
#include <vcl_cassert.h>

DebugDisplayer GDebugDisplayer;

DebugDisplayer::DebugDisplayer()
{for(int i = 0; i < 20; ++i) _ostrms[i] = NULL;}
DebugDisplayer::~DebugDisplayer(){
  for(int i = 0; i < 20; ++i) delete _ostrms[i];
  //if(_ostrms){
  //  delete _ostrms;
  //}
}
void DebugDisplayer::statusBar(wxStatusBar* sb_)
  {
    assert(sb_!=0); 
    _statusBar = sb_;
   }
void DebugDisplayer::notebook(wxNotebook* nb_){
  _notebook = nb_;
}
void DebugDisplayer::debugTexts(wxTextCtrl** dt, int numWins_){
  if(numWins_>20)
    _numWins = 20;
  else
    _numWins = numWins_;
  for(int cnt=0;cnt<_numWins;cnt++) {
    _debugTexts[cnt]=dt[cnt];
    if(_ostrms[cnt]){
      delete _ostrms[cnt];
    }
#if 0
    _ostrms[cnt] = new vcl_ostream((vcl_streambuf*)_debugTexts[cnt]);
#endif
  }
}
void DebugDisplayer::clear(int winId){
  assert(winId<_numWins);
  _debugTexts[winId]->Clear();
}
vcl_ostream* DebugDisplayer::ostrm(int winId){
  assert(winId<_numWins);
  return _ostrms[winId];
}
vcl_streambuf* DebugDisplayer::strmbuf(int winId){
  assert(winId<_numWins);
  return (vcl_streambuf*)_debugTexts[winId];
}
wxTextCtrl* DebugDisplayer::debugText(int winId){ 
  assert(winId<_numWins);
  return _debugTexts[winId];
}
void DebugDisplayer::disp(const vcl_string& msg, int winId){
  assert(winId<_numWins);
  _debugTexts[winId]->AppendText(wxString(msg.c_str()));
  _notebook->SetSelection(winId);
  _notebook->Refresh();
}
void DebugDisplayer::status(const vcl_string& msg){
  _statusBar->SetStatusText(wxString(msg.c_str()));
  _statusBar->Refresh();
}


