#include "basegui_debugdisplayer.h"
#include <cassert>

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
    _ostrms[cnt] = new std::ostream((std::streambuf*)_debugTexts[cnt]);
#endif
  }
}
void DebugDisplayer::clear(int winId){
  assert(winId<_numWins);
  _debugTexts[winId]->Clear();
}
std::ostream* DebugDisplayer::ostrm(int winId){
  assert(winId<_numWins);
  return _ostrms[winId];
}
std::streambuf* DebugDisplayer::strmbuf(int winId){
  assert(winId<_numWins);
  return (std::streambuf*)_debugTexts[winId];
}
wxTextCtrl* DebugDisplayer::debugText(int winId){ 
  assert(winId<_numWins);
  return _debugTexts[winId];
}
void DebugDisplayer::disp(const std::string& msg, int winId){
  assert(winId<_numWins);
  _debugTexts[winId]->AppendText(wxString(msg.c_str()));
  _notebook->SetSelection(winId);
  _notebook->Refresh();
}
void DebugDisplayer::status(const std::string& msg){
  _statusBar->SetStatusText(wxString(msg.c_str()));
  _statusBar->Refresh();
}


